/*
 [Module fat32.c]
 Copyright(c) 2015 mrtska.starring

 This software is released under the MIT License.

 http://opensource.org/licenses/mit-license.php
 Created on: 2014/10/05
 */

#include <fs/fat32.h>
#include <drivers/ide.h>
#include <slab.h>
#include <string.h>
#include <internal/stdio.h>
#include <fs/vfs.h>

//FAT32の情報を格納 グローバル変数での実装は複数のFAT32デバイスに対応できないので FIXME
struct fat32_info info;

//VFSのルートノード ルートがFAT32がルートディレクトリにマウントされるとは限らないので FIXME
struct fs_node root_node;

static void conv_file_name(struct fat32_directory_entry *entry, char *buf);
static int read_directory_entry(const char *path, struct fat32_directory_entry *buf);

//ファイルシステム初期化
void fat32_filesystem_init(void) {

	//バッファ 1セクタ分
	unsigned char buf1[0x200];

	//デバイスからブートセクタをロード FAT32デバイスがIDEとは限らないので FIXME
	ide_read_sector_via_dma(0x0000, buf1, 1);

	//FAT32マスターブートレコード どのデバイスにもMBRがあるとは限らないので FIXME
	struct mbr *mbr = (struct mbr*) buf1;

	//ブート可能なパーティションを見つける MBRのみ FIXME
	int i;
	for(i = 0; i < 4; i++) {
		if(mbr->partitions[i].PT_BootID)
			break;
	}

	//バッファ 1セクタ分
	unsigned char *buf2 = kmalloc(0x200, 0);

	//ブート可能なパーティションの最初のセクタをロード FIXME
	ide_read_sector_via_dma(mbr->partitions[i].PT_LbaOfs, buf2, 1);

	//情報を構造体へ格納
	info.boot_sectors = (struct fat32_boot_sector*) buf2;
	info.mbr_offset = mbr->partitions[i].PT_LbaOfs;
	info.FAT_region = info.boot_sectors->BPB_RsvdSecCnt + mbr->partitions[i].PT_LbaOfs;
	info.FAT_size = info.boot_sectors->BPB_FATSz32 * info.boot_sectors->BPB_NumFATs;
	info.root_directory_entry = info.FAT_region + info.FAT_size;
	info.cluster_size = info.boot_sectors->BPB_BytsPerSec * info.boot_sectors->BPB_SecPerClus;

	//ルートノードを設定
	root_node.filename[0] = '/';
	root_node.filename[1] = 0x00;
	root_node.open = fat32_fopen;
	root_node.close = fat32_fclose;
	root_node.read = fat32_fread;
	root_node.write = fat32_fwrite;
	root_node.finddir = fat32_finddir;

	root_node.flags = FS_READONLY;

	//VFSのルートにマウント FIXME
	vfs_mount("/", &root_node);
}

//ディレクトリエントリをロード パスは絶対パス
static int read_directory_entry(const char *path, struct fat32_directory_entry *buf) {

	//ストリングバッファ
	char tokbuf[0x100];

	//パスをコピー
	strcpy(tokbuf, path);

	//文字列を分割する
	char *tok = strtok(tokbuf, '/');	//パスを分割

	//バッファを取得
	unsigned char *lbuf = kmalloc(0x200, 0);

	//ルートディレクトリのLBA
	unsigned int lba = info.root_directory_entry;

	//分割し終わったら終了 パスが不正な場合以外はこのループから抜けずに関数が終了する
	while(tok != 0x00) {

//ディレクトリエントリが1セクタ分では足りなかった場合にLBAをインクリメントしてリスタートする
		restart:
		//kprintf("tok = %s, lba = %X\n", tok, lba);
		//ディレクトリエントリをロード
		ide_read_sector_via_dma(lba, lbuf, 1);
		struct fat32_directory_entry *entry = (struct fat32_directory_entry*) lbuf;

		//ロードしたディレクトリエントリを走査して該当するものを探す
		int i;
		for(i = 0; (entry[i].DIR_Name[0] & 0xFF) != 0x00; i++) {

			//ディレクトリエントリが無効なエントリだったらスルーする
			if((entry[i].DIR_Name[0] & 0xFF) == 0xE5) {

				//ロードし足りなかった場合LBAを変えて再度ロード
				if(i == 15) {

					lba++;
					goto restart;
				}
				continue;
			}

			//ストリングバッファ
			char strbuf[0x100];
			//kprintf("strbuf %s, tok %s, %d\n", &entry[i].DIR_Name, tok, i);
			conv_file_name(&entry[i], strbuf);	//ファイル名を変換
			//kprintf("strbuf %s, tok %s, %d\n", strbuf, tok, i);

			//ファイル名比較
			if(strcmp(tok, strbuf) == 0) {

				struct fat32_directory_entry ent = entry[i];

				//クラスタ番号
				unsigned int cluster = ent.DIR_FstClusHI << 16 | ent.DIR_FstClusLO;

				//LBAを次のディレクトリ/ファイルのものに更新
				lba = info.root_directory_entry + (cluster - 2) * info.boot_sectors->BPB_SecPerClus;

				//該当するノードがディレクトリなら次のものを探す ファイルならbufにエントリを代入して終了
				if(entry[i].DIR_Attr & FAT32_ATTR_DIRECTORY) {

					//kprintf("directory = %s, LBA = %X\n", tok, lba);
					tok = strtok(0, '/');
					if(tok == NULL) {

						*buf = ent;
						return 0;
					}
					goto restart;
				} else {

					//kprintf("file = %s, LBA = %X\n", tok, lba);
					*buf = ent;
					return 0;
				}
			}

			//ロードし足りなかった場合LBAを変えて再度ロード
			if(i == 15) {

				lba++;
				goto restart;
			}

		}

		goto fail;
	}
	fail:
	//kprintf("can not find %s\n", path);
	kfree(lbuf, 0x200);	//ファイルを見つけられなかった時はfreeする
	return -1;
}

//ファイル名（SFN,LFN）を使いやすい形に変換する
static void conv_file_name(struct fat32_directory_entry *entry, char *buf) {

	//SFNのファイル名
	char left_name[9];

	//SFNの拡張子
	char right_name[4];

	//ローカル変数初期化
	memset(left_name, 0x00, 9);
	memset(right_name, 0x00, 4);

	int i;
	//ファイルを取得
	for(i = 0; i < 8; i++) {

		//空白になったら終了
		if(entry->DIR_Name[i] == ' ')
			break;

		//代入
		left_name[i] = entry->DIR_Name[i];

		//小文字判定
		if(entry->DIR_NTRes & 0x08) {

			//数字は判定に含まない
			if(left_name[i] >= 'A' && left_name[i] <= 'Z') {

				left_name[i] += 0x20;
			}
		}
	}
	//拡張子を取得
	for(i = 8; i < 11; i++) {

		if(entry->DIR_Name[i] == ' ') {

			break;
		}
		right_name[i - 8] = entry->DIR_Name[i];

		//小文字判定
		if(entry->DIR_NTRes & 0x08) {

			//数字は判定に含まない
			if(right_name[i - 8] >= 'A' && right_name[i - 8] <= 'Z') {

				right_name[i - 8] += 0x20;
			}
		}
	}

	//ディレクトリはそのままbufへ ファイルは.を付ける
	if((entry->DIR_Attr & FAT32_ATTR_DIRECTORY) || right_name[0] == 0) {

		sprintf(buf, "%s", left_name);
	} else {

		sprintf(buf, "%s.%s", left_name, right_name);
	}

	//LFNの場合
	if(left_name[6] == '~') {

		char offset = left_name[7] - '0';

		entry -= offset;
		struct fat32_lfn_entry *lfn = (struct fat32_lfn_entry*) entry;

		char *tmp = buf;

		//LFNパーシング
		while(true) {

			//一つ目
			for(i = 0; i < 5; i++) {
				if((lfn->LDIR_Name1[i] & 0xFF) == 0xFF) {
					break;
				}
				tmp[i] = lfn->LDIR_Name1[i] & 0xFF;
			}
			//二つ目
			for(i = 5; i < 11; i++) {
				if((lfn->LDIR_Name2[i - 5] & 0xFF) == 0xFF) {
					break;
				}
				tmp[i] = lfn->LDIR_Name2[i - 5] & 0xFF;
			}
			//三つ目
			for(i = 11; i < 13; i++) {
				if((lfn->LDIR_Name3[i - 11] & 0xFF) == 0xFF) {
					break;
				}
				tmp[i] = lfn->LDIR_Name3[i - 11] & 0xFF;
			}
			tmp += 13;

			if(lfn->LDIR_Ord & FAT32_LAST_LONG_ENTRY)
				break;

			//LFNエントリはマイナス方向に続きがあるのでデクリメントする
			lfn--;
		}

	}
}

//FAT32のファイルをfs_nodeに代入する
struct fs_node *fat32_file_init(const char *filename) {

	struct fs_node *node = kmalloc(sizeof(struct fs_node), 0);

	//ファイル名からディレクトリエントリを取得
	struct fat32_directory_entry file;
	int ret = read_directory_entry(filename, &file);

	//ファイルが
	if(ret == -1) {

		trace();
		return NULL;
	}

	//ファイルパスをコピー
	strcpy(node->filename, filename);

	//ファイルサイズを代入
	node->length = file.DIR_FileSize;

	//ファイルかディレクトリかを判定
	if(file.DIR_Attr & FAT32_ATTR_DIRECTORY) {

		node->flags = FS_DIRECTORY;
	} else {

		node->flags = FS_FILE;
		node->data = kmalloc(sizeof(struct fat32_cluster_cache), 0);
	}

	node->inode = (file.DIR_FstClusHI << 16) | file.DIR_FstClusLO;

	//各種ハンドラ設定
	node->open = fat32_fopen;
	node->close = fat32_fclose;
	node->read = fat32_fread;
	node->write = fat32_fwrite;
	node->finddir = fat32_finddir;
	node->fstat = fat32_fstat;

	node->flags |= FS_READONLY;

	node->seek = 0;



	return node;
}

//fopen関数 FIXME
void fat32_fopen(struct fs_node *node, unsigned int flags) {

	unsigned char buf[0x100];
	//kprintf("[fat32/fopen] path = %s\n", node->filename);
	read_fs(node, 0, 0x100, buf);

	if(strncmp("!<symlink>", (char*) buf, 10) == 0) {

		node->flags |= FS_SYMLINK;
		char name[0x100];
		strcpy(name, node->filename);
		char *n = strrchr(name, '/') + 1;
		*n = 0;
		//kprintf("[fat32/fopen] sym fopen %s\n", name);

		node->ptr = kopen(strcat(name, (char*) &buf[0xA]), 0);
		node->length = node->ptr->length;
	}

}

//fclose 特に何もしない
void fat32_fclose(struct fs_node *node) {

}

//オフセットからスタートクラスタを取得する
static int get_cluster_index(unsigned long offset) {

	return offset / info.cluster_size;
}

static void read_block_by_lba(unsigned long lba, int sector, void *p) {

	ide_read_sector_via_dma(lba, p, sector);
}

static unsigned long read_block_by_cluster(unsigned int cluster, unsigned char *p) {

	unsigned long lba = info.root_directory_entry + (cluster - 2) * info.boot_sectors->BPB_SecPerClus;
	ide_read_sector_via_dma(lba, p, 0x20);
	//kprintf("read sector %X lba %X,      %X %X\n", p, lba, p[0], p[1]);
	return lba;
}

static unsigned int read_fat_entry(unsigned char *fatbuf, unsigned int fat_offset) {

	return (*(unsigned int*) &fatbuf[fat_offset]) & 0x0FFFFFFF;
}

//FAT領域のデータをロードしてクラスタチェインを返す キャッシュがある場合はそれを返す
static unsigned int *read_fat(struct fat32_cluster_cache *cache, unsigned int start_cluster, unsigned long size) {

	if(cache->clusters && !cache->dirty) {

		return cache->clusters;
	}
	cache->dirty = 0;

	unsigned int fat_sector = info.FAT_region + (start_cluster * 4 / info.boot_sectors->BPB_BytsPerSec);
	unsigned int fat_offset = (start_cluster * 4) % info.boot_sectors->BPB_BytsPerSec;
	unsigned int *ret = kmalloc(0x1000, 0);
	unsigned int before_fat_sector = fat_sector;
	unsigned char fatbuf[0x200];
	read_block_by_lba(fat_sector, 1, fatbuf);

	ret[0] = start_cluster;

	int i;
	for(i = 1;; i++) {

		unsigned int cluster = read_fat_entry(fatbuf, fat_offset);
		ret[i] = cluster;

		//kprintf("cluster %X, sector %X, offset %X\n", cluster, fat_sector, fat_offset);
		if(cluster >= 0x0FFFFFF8) {

			break;
		}

		fat_sector = info.FAT_region + (cluster * 4 / info.boot_sectors->BPB_BytsPerSec);
		fat_offset = (cluster * 4) % info.boot_sectors->BPB_BytsPerSec;

		if(before_fat_sector != fat_sector) {

			read_block_by_lba(fat_sector, 1, fatbuf);
			before_fat_sector = fat_sector;
		}
	}
	cache->clusters = ret;

	return ret;
}

#define OFFSET_MASK(x) (x & (info.cluster_size - 1))

//ファイルを引数に合わせてロードする
unsigned int fat32_fread(struct fs_node *node, unsigned int offset, unsigned int size, unsigned char *buffer) {

	struct fat32_cluster_cache *cache = node->data;
	//ディレクトリエントリ
	struct fat32_directory_entry file;

	//シンボリックリンクだったらリンク先をロードする
	if(node->flags & FS_SYMLINK) {

		read_directory_entry(node->ptr->filename, &file);
		cache->dirty = 1;
	} else {

		read_directory_entry(node->filename, &file);
	}

	//ファイルのスタートクラスタ
	unsigned int start_cluster = (file.DIR_FstClusHI << 16) | file.DIR_FstClusLO;

	//クラスタを取得
	unsigned int *clusters = read_fat(cache, start_cluster, node->length);

	if(node->flags & FS_SYMLINK) {

		//kprintf("[fat32/fread] symbolic link %s, offset %X, size %X\n", node->ptr->filename, offset, size);
	} else {

		//kprintf("[fat32/fread] path = %s, offset = %X, size = %X\n", node->filename, offset, size);
	}

	unsigned char *buf = kmalloc(info.cluster_size, 0);

	int i;
	int index;
	for(i = 1, index = get_cluster_index(offset);; i++, index++) {

		//クラスタ番号をロード
		unsigned int cluster = clusters[index];

		//終了クラスタ
		if(cluster >= 0x0FFFFFF8) {

			//kprintf("[fat32/fread] cluster break\n");
			break;
		}

		//クラスタ番号の領域をロード
		read_block_by_cluster(cluster, buf);
		//kprintf("cluster %X, data %X, %X, %X\n", cluster, buf[0], buf[1], buf[2]);

		//読込サイズを超えたら終了
		if(size <= i * info.cluster_size) {

			if(i == 1) {

				//kprintf("[fat32/fread] i0 read end\n");
				memcpy(buffer, buf + OFFSET_MASK(offset), size);
				break;
			}

			//kprintf("[fat32/fread] read end, %X, %p\n", i * info.cluster_size, buffer);
			//memcpy(buffer, buf, OFFSET_MASK(offset));
			memcpy(buffer, buf, info.cluster_size - (i * info.cluster_size - size));
			break;
		}
		//最初はオフセットを考慮する
		if(i == 1) {

			memcpy(buffer, buf + OFFSET_MASK(offset), info.cluster_size - OFFSET_MASK(offset));
			buffer += info.cluster_size - OFFSET_MASK(offset);
		} else {

			memcpy(buffer, buf, info.cluster_size);
			buffer += info.cluster_size;
		}
		//バッファを進める
	}
	/*
	 unsigned char *dist_buf = buffer;
	 buf = dist_buf;
	 kprintf("buffer dump\n");
	 for(i = 0; i < 0x10; i++) {

	 kprintf("%2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X\n",
	 buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);
	 buf += 0x10;
	 }
	 STOP;*/

	return 0;
}
unsigned int fat32_fwrite(struct fs_node *node, unsigned int offset, unsigned int size, unsigned char *buffer) {

	return 0;
}

//ファイルを見つける
struct fs_node *fat32_finddir(struct fs_node *node, char *name) {

	return fat32_file_init(name);
}


int fat32_fstat(struct fs_node *node, struct stat *stat) {

	stat->st_nlink = 1;

	stat->st_uid = node->uid;
	stat->st_gid = node->gid;
	stat->st_ino = node->inode;
	stat->st_size = node->length;



	kprintf("[fat32/fstat] size %X\n", node->length);

	if(node->flags & FS_FILE) {

		stat->st_mode = S_IFREG;
	} else if(node->flags & FS_DIRECTORY) {

		stat->st_mode = S_IFDIR;
	}

	return 0;
}





