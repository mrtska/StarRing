/*
[Module stdout.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2015/01/29
*/

#include <system.h>
#include <fs/stdout.h>
#include <fs/stat.h>


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

static int fstat_stdout(struct fs_node *node, struct stat *stat) {


	stat->st_ino = 0xD;
	stat->st_mode = S_IFCHR;

	return 0;
}

static struct file_system_operations stdout_operations = {

		.read = read_stdout,
		.write = write_stdout,
		.writev = writev_stdout,
		.open = open_stdout,
		.close = close_stdout,
		.fstat = fstat_stdout
};



struct fs_node *create_stdout(struct file_system *fs) {

	struct fs_node *node = kmalloc(sizeof(struct fs_node), 8);

	strcpy(node->filename, "stdout");

	node->fs = fs;
	node->fs->opt = &stdout_operations;


	node->flags = FS_CHARDEVICE;

	return node;
}











