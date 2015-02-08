/*
[Module stat.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2015/02/01
*/

#include <fs/stat.h>
#include <system.h>
#include <fs/vfs.h>
#include <errno.h>

int stat(const char *filename, struct stat *statbuf) {





	return 0;
}


int fstat(unsigned int fd, struct stat *statbuf) {

	struct fs_node *node = get_node(fd);
	if(!node) {

		return -EBADF;
	}

	statbuf->st_uid = node->uid;
	statbuf->st_gid = node->gid;
	statbuf->st_ino = node->inode;
	statbuf->st_size = node->length;

	return 0;
}







