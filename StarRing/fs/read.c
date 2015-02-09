/*
[Module read.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2015/01/24
*/

#include <fs/read.h>
#include <errno.h>
#include <fs/vfs.h>
#include <fs/fcntl.h>


long read(unsigned long fd, const char *buf, size_t count) {


	struct fs_node *node = get_node(fd);

	if(!node) {

		kprintf("[kernel/read] can not find file\n");
		return -EMFILE;
	}

	read_fs(node, node->seek, count, (unsigned char*) buf);
	node->seek += count;
	return count;
}

int readv(unsigned long fd, const struct iovec *vec, unsigned long vlen) {


	struct fs_node *node = get_node(fd);

	if(!node) {

		kprintf("[kernel/readv] can not find file\n");
		return -EMFILE;
	}

	long ret = 0;

	int i;
	for(i = 0; i < vlen; i++) {

		const struct iovec *v = &vec[i];

		read_fs(node, v->iov_len, 1, v->iov_base);
		kprintf("[kernel/readv] base %X, len %X\n",  v->iov_base, v->iov_len);
		node->seek += v->iov_len;

	}

	return ret;
}
