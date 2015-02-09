/*
[Module stdout.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2015/01/29
*/

#include <system.h>
#include <fs/stdout.h>



static unsigned int read_stdout(struct fs_node *node, unsigned int offset, unsigned int size, unsigned char *buffer) {


	trace();
	return 0;
}

static unsigned int write_stdout(struct fs_node *node, unsigned int offset, unsigned int size, unsigned char *buffer) {


	kprintf("%s", buffer);
	return 0;
}


static unsigned int writev_stdout(struct fs_node *node, const struct iovec *vec, unsigned long len) {

	char *buf = kmalloc(0x1000, 0);
	char *p = buf;
	int ret = 0;
	int i;
	for(i = 0; i < len; i++) {

		char *base = vec[i].iov_base;
		int ilen = vec[i].iov_len;

		strncpy(buf, base, ilen);
		buf += ilen;
		ret += ilen;
	}

	kprintf("%s", p);

	kfree(p, 0x1000);

	return ret;
}

static void open_stdout(struct fs_node *node, unsigned int flags) {

	trace();
	return;
}

static void close_stdout(struct fs_node *node) {

	trace();
	return;
}







struct fs_node *create_stdout(void) {

	struct fs_node *node = kmalloc(sizeof(struct fs_node), 8);

	strcpy(node->filename, "stdout");
	node->read = read_stdout;
	node->write = write_stdout;
	node->writev = writev_stdout;
	node->open = open_stdout;
	node->close = close_stdout;


	node->flags = FS_CHARDEVICE;

	return node;
}











