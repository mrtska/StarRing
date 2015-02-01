/*
[Module null_device.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/11/15
*/


#include <fs/vfs.h>


static unsigned int rw_null(struct fs_node *node, unsigned int offset, unsigned int size, unsigned char *buffer) {

	return 0;
}

static void open_null(struct fs_node *node, unsigned int flags) {

	return;
}

static void close_null(struct fs_node *node) {

	return;
}







struct fs_node *create_null_device(void) {

	struct fs_node *node = kmalloc(sizeof(struct fs_node), 8);

	strcpy(node->filename, "null");
	node->read = rw_null;
	node->write = rw_null;
	node->open = open_null;
	node->close = close_null;

	return node;
}

























     
