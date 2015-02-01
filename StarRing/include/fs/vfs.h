/*
[Module vfs.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/09/28
*/

#pragma once


#include <fs/fs.h>
#include <slab.h>


//仮想ファイルシステムエントリ
struct vfs_entry {

	char *name;
	struct fs_node *file;
};


struct fs_node *create_null_device(void);



//VFS初期化
void vfs_init(void);

//VFSマウント
void vfs_mount(char *path, struct fs_node *node);

//カーネル用ファイルオープン
struct fs_node *kopen(const char *name, unsigned int flags);
struct fs_node *get_node(unsigned int fd);
struct file_descriptor *get_file_descriptor(unsigned int fd);


//ファイルリード/ライト
unsigned int read_fs(struct fs_node *node, unsigned int offset, unsigned int size, unsigned char *buffer);
unsigned int write_fs(struct fs_node *node, unsigned int offset, unsigned int size, unsigned char *buffer);

//ファイルオープン/クローズ
void open_fs(struct fs_node *node, unsigned int flags);
void close_fs(struct fs_node *node);

//ファイルサーチ
struct fs_node *finddir_fs(struct fs_node *node, char *name);


