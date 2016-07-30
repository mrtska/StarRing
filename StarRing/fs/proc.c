/*
[Module proc.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2015/02/01
*/




#include <fs/vfs.h>


static unsigned int read_proc(struct fs_node *node, unsigned int offset, unsigned int size, unsigned char *buffer) {


	return 0;
}


static unsigned int write_proc(struct fs_node *node, unsigned int offset, unsigned int size, unsigned char *buffer) {

	return 0;
}

static void open_proc(struct fs_node *node, unsigned int flags) {

	return;
}

static void close_proc(struct fs_node *node) {

	return;
}







struct fs_node *create_proc_filesystem(void) {

	struct fs_node *node = kmalloc(sizeof(struct fs_node), 8);

	strcpy(node->filename, "null");
	node->read = read_proc;
	node->write = write_proc;
	node->open = open_proc;
	node->close = close_proc;

	return node;
}






