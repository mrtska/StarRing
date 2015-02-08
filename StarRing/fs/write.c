/*
 [Module write.c]
 Copyright(c) 2015 mrtska.starring

 This software is released under the MIT License.

 http://opensource.org/licenses/mit-license.php
 Created on: 2015/01/22
 */

#include <fs/vfs.h>
#include <fs/write.h>

#include <errno.h>

long write(unsigned long fd, const char *buf, size_t count) {

	kprintf("%s", buf);

	return count;
}

long writev(unsigned long fd, const struct iovec *vec, unsigned long vlen) {

	struct fs_node *node = get_node(fd);

	if(!node) {

		return -ENOENT;
	}

	long ret = 0;

	int i;
	for(i = 0; i < vlen; i++) {

		const struct iovec *v = &vec[i];

		write_fs(node, 0, v->iov_len, v->iov_base);
		ret += v->iov_len;
	}

	return ret;
}

