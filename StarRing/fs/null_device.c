/*
[Module null_device.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/11/15
*/


#include <fs/vfs.h>


static unsigned int rw_null(struct fs_node *node, unsigned int offset, unsigned int size, unsigned char *buffer);
static void open_null(struct fs_node *node, unsigned int flags);
static void close_null(struct fs_node *node);

static struct file_system_operations null_device_oprations = {

		.read = rw_null,
		.write = rw_null,
		.open = open_null,
		.close = close_null
};


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
	node->opt = &null_device_oprations;

	return node;
}



