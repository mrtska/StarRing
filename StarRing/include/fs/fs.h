/*
[Module fs.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/09/30
*/

#pragma once

#include <task.h>
#include <fs/stat.h>
#include <fs/iovec.h>
#include <drivers/storage.h>

//ファイルノード項目
#define FS_FILE				0x1
#define FS_DIRECTORY		0x2
#define FS_CHARDEVICE		0x4
#define FS_BLOCKDEVICE		0x8
#define FS_PIPE				0x10
#define FS_SYMLINK			0x20
#define FS_MOUNTPOINT		0x40
#define FS_READONLY			0x80

#define S_IFMT   0170000  /* ファイルのタイプ */
#define S_IFIFO  0010000  /* 名前付きパイプ (fifo) */
#define S_IFCHR  0020000  /* キャラクタ型特殊ファイル */
#define S_IFDIR  0040000  /* ディレクトリ */
#define S_IFBLK  0060000  /* ブロック型特殊ファイル */
#define S_IFREG  0100000  /* 通常 */
#define S_IFLNK  0120000  /* シンボリックリンク */
#define S_IFSOCK 0140000  /* ソケット */
#define S_IFWHT  0160000  /* ホワイトアウト */
#define S_ISUID  0004000  /* 実行時にユーザ ID を設定 */
#define S_ISGID  0002000  /* 実行時にグループ ID を設定 */
#define S_ISVTX  0001000  /* 使用後にもスワップされたテキストを保存 */
#define S_IRUSR  0000400  /* 読取り権限の所有者 */
#define S_IWUSR  0000200  /* 書込み権限の所有者 */
#define S_IXUSR  0000100  /* 実行 / 検索権限の所有者 */

struct fs_node;


//ファイルノード読込み/書込み関数ポインタ
typedef unsigned int (*read_func) (struct fs_node*, unsigned int, unsigned int, unsigned char*);
typedef unsigned int (*write_func) (struct fs_node*, unsigned int, unsigned int, unsigned char*);
typedef unsigned int (*writev_func) (struct fs_node*, const struct iovec*, unsigned long);

//ファイルノードオープン/クローズ関数ポインタ
typedef void (*open_func) (struct fs_node*, unsigned int);
typedef void (*close_func) (struct fs_node*);

//ディレクトリ解析関数ポインタ
typedef struct dirent *(*readdir_func) (struct fs_node*, unsigned int);
typedef struct fs_node *(*finddir_func) (struct fs_node*, char*);

//ファイル/ディレクトリ作成関数ポインタ
typedef void (*create_func) (struct fs_node*, char*, unsigned short);
typedef void (*mkdir_func) (struct fs_node*, char*, unsigned short);

//ioctl関数ポインタ
typedef int (*ioctl_func) (struct fs_node*, int, void*);

//fstat関数ポインタ
typedef int (*fstat_func) (struct fs_node*, struct stat*);

//ファイルサイズ取得関数ポインタ
typedef int (*get_size_func) (struct fs_node*);

struct file_system_operations {

	//ファイルオペレーション
	read_func read;			//unsigned int : struct fs_node *node, unsigned int offset, unsigned int size, unsigned char *buffer
	write_func write;		//unsigned int : struct fs_node *node, unsigned int offset, unsigned int size, unsigned char *buffer
	writev_func writev;
	open_func open;			//void : struct fs_node *node, unsigned int flags
	close_func close;		//void : struct fs_node *node
	readdir_func readdir;	//
	finddir_func finddir;	//struct fs_node* : struct fs_node *node, char *name
	create_func create;		//
	mkdir_func mkdir;		//
	ioctl_func ioctl;		//
	fstat_func fstat;		//
	get_size_func get_size;	//

};

//ファイルディスクリプタ
struct file_descriptor {

	struct list_head list;	//ファイルディスクリプタのリスト
	int fd;					//ファイルディスクリプタ
	int flags;				//フラグ
	int mode;				//モード
	struct fs_node *node;	//fdに対応したファイルノード
};



struct file_system {

	struct storage_device *storge;
	struct file_system_operations *opt;




};

//ファイルノード マウントポイントごとに設定される
struct fs_node {

	char filename[256];

	//アクセス制限用マスク
	unsigned int mask;

	//オーナーユーザー
	unsigned int uid;

	//オーナーグループ
	unsigned int gid;

	//ファイルノードフラグ
	unsigned int flags;

	//iノード番号
	unsigned int inode;

	//ファイルサイズ
	unsigned long length;

	//オープンフラグ(read, write, append...)
	unsigned int open_flag;

	unsigned int atime;		//アクセスタイム
	unsigned int mtime;		//変更タイム
	unsigned int ctime;		//作成タイム

	struct file_system *fs;


	//シンボリックリンク用ポインタ
	struct fs_node *ptr;

	unsigned long seek;

	//ファイルシステムドライバが使用する
	void *data;
};

//ディレクトリエントリ
struct dirent {

	unsigned int inode;		//iNode
	char name[256];			//ファイルネーム
};

















