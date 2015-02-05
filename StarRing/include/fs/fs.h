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


//ファイルノード項目
#define FS_FILE				0x1
#define FS_DIRECTORY		0x2
#define FS_CHARDEVICE		0x4
#define FS_BLOCKDEVICE		0x8
#define FS_PIPE				0x10
#define FS_SYMLINK			0x20
#define FS_MOUNTPOINT		0x40

struct fs_node;


//ファイルノード読込み/書込み関数ポインタ
typedef unsigned int (*read_func) (struct fs_node*, unsigned int, unsigned int, unsigned char*);
typedef unsigned int (*write_func) (struct fs_node*, unsigned int, unsigned int, unsigned char*);

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
typedef int (*fstat_func) (struct fs_node*, unsigned int, struct stat*);

//ファイルサイズ取得関数ポインタ
typedef int (*get_size_func) (struct fs_node*);


//ファイルディスクリプタ
struct file_descriptor {

	struct list_head list;	//ファイルディスクリプタのリスト
	int fd;					//ファイルディスクリプタ
	int flags;				//フラグ
	int mode;				//モード
	struct fs_node *node;	//fdに対応したファイルノード
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

	//ファイルオペレーション
	read_func read;			//unsigned int : struct fs_node *node, unsigned int offset, unsigned int size, unsigned char *buffer
	write_func write;		//unsigned int : struct fs_node *node, unsigned int offset, unsigned int size, unsigned char *buffer
	open_func open;			//void : struct fs_node *node, unsigned int flags
	close_func close;		//void : struct fs_node *node
	readdir_func readdir;	//
	finddir_func finddir;	//struct fs_node* : struct fs_node *node, char *name
	create_func create;		//
	mkdir_func mkdir;		//
	ioctl_func ioctl;		//
	fstat_func fstat;		//
	get_size_func get_size;	//

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

















