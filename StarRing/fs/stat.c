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

	struct fs_node *node = kopen(filename, 0);

	if(!node) {

		trace();
		return -ENOENT;
	}

	return fstat_fs(node, statbuf);
}


int fstat(unsigned int fd, struct stat *statbuf) {

	struct fs_node *node = get_node(fd);
	if(!node) {

		trace();
		return -EBADF;
	}

	return fstat_fs(node, statbuf);
}







