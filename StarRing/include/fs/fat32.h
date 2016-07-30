/*
[Module fat32.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/10/05
*/

#pragma once

#include <fs/vfs.h>


#define FAT32_MAX_ROOT_DIRECTORY_COUNT 65535


#define FAT32_ATTR_READ_ONLY		0x01
#define FAT32_ATTR_HIDDEN			0x02
#define FAT32_ATTR_SYSTEM			0x04
#define FAT32_ATTR_VOLUME_ID		0x08
#define FAT32_ATTR_DIRECTORY		0x10
#define FAT32_ATTR_ARCHIVE			0x20
#define FAT32_ATTR_LONG_FILE_NAME	0x0F




//パーティション
struct partition {

	unsigned char PT_BootID;
	unsigned char PT_StartHd;
	unsigned short PT_StartCySc;
	unsigned char PT_System;
	unsigned char PT_EndHd;
	unsigned short PT_EndCySc;
	unsigned int PT_LbaOfs;
	unsigned int PT_LbaSize;
};


//マスターブートレコード
struct mbr {

	unsigned char MBR_bootcode[446];
	struct partition partitions[4];
	unsigned short MBR_Sig;
} __attribute__((packed));


//FAT32ブートセクタ
struct fat32_boot_sector {

	char BS_JmpBoot[3];					//ジャンプ命令
	char BS_OEMName[8];					//OEMネーム
	unsigned short BPB_BytsPerSec;		//1セクタあたりのバイト数
	unsigned char BPB_SecPerClus;		//1クラスタあたりのセクタ数
	unsigned short BPB_RsvdSecCnt;		//予約済みセクタ
	unsigned char BPB_NumFATs;			//FAT領域の数
	unsigned short BPB_RootEntCnt;		//ルートディレクトリエントリの数 FAT32では常に0
	unsigned short BPB_TotSec16;		//全セクタ数の合計 FAT32では常に0
	unsigned char BPB_Media;			//メディアタイプ リムーバブルディスクでは0xF0
	unsigned short BPB_FatSz16;			//FAT領域のセクタ数 FAT23では常に0
	unsigned short BPB_SecPerTrk;		//1トラックあたりのセクタ数
	unsigned short BPB_NumHeaders;
	unsigned int BPB_HiddSec;
	unsigned int BPB_TotSec32;
	unsigned int BPB_FATSz32;
	unsigned short BPB_ExtFlags;
	unsigned short BPB_FSVer;
	unsigned int BPB_RootClus;
	unsigned short BPB_FSInfo;
	unsigned short BPB_BkBootSec;
	unsigned char BPB_Reserved[12];
	unsigned char BS_DrvNum;
	unsigned char BS_Reserved1;
	unsigned char BS_BootSig;
	unsigned int BS_VolID;
	char BS_VolLab[11];
	char BS_FilSysType[8];
} __attribute__((packed));

//FAT32ファイルシステム情報 単位はセクタ
struct fat32_info {

	unsigned int mbr_offset;
	unsigned int FAT_region;
	unsigned int FAT_size;
	unsigned int root_directory_entry;
	struct fat32_boot_sector *boot_sectors;
	unsigned int cluster_size;
};


//FAT32ディレクトリエントリ
struct fat32_directory_entry {

	char DIR_Name[11];
	unsigned char DIR_Attr;
	unsigned char DIR_NTRes;
	unsigned char DIR_CrtTimeTenth;
	unsigned short DIR_CrtTime;
	unsigned short DIR_CrtDate;
	unsigned short DIR_LstAccDate;
	unsigned short DIR_FstClusHI;
	unsigned short DIR_WrtTime;
	unsigned short DIR_WrtDate;
	unsigned short DIR_FstClusLO;
	unsigned int DIR_FileSize;
} __attribute__((packed));


#define FAT32_LAST_LONG_ENTRY 0x40
//FAT32LFNエントリ
struct fat32_lfn_entry {

	unsigned char LDIR_Ord;
	short LDIR_Name1[5];
	unsigned char LDIR_Attr;
	unsigned char LDIR_Type;
	unsigned char LDIR_Chksum;
	short LDIR_Name2[6];
	unsigned short LDIR_FstClusLO;
	short LDIR_Name3[2];
} __attribute__((packed));


struct fat32_cluster_cache {

	unsigned char dirty;
	unsigned int *clusters;
	unsigned int current_cluster;
	unsigned int current_sector;
	unsigned char *data;
};


void fat32_filesystem_init(struct file_system *fs);


//ファイルオープン
void fat32_fopen(struct fs_node *node, unsigned int flags);

//ファイルクローズ
void fat32_fclose(struct fs_node *node);

struct fs_node *fat32_finddir(struct fs_node *node, char *name);

//ファイルライト/リード
unsigned int fat32_fread(struct fs_node *node, unsigned int offset, unsigned int size, unsigned char*buffer);
unsigned int fat32_fwrite(struct fs_node *node, unsigned int offset, unsigned int size, unsigned char*buffer);

int fat32_fstat(struct fs_node *node, struct stat *stat);

