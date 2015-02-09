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

		trace();
		return -ENOENT;
	}

	long ret = writev_fs(node, vec, vlen);

	return ret;
}

