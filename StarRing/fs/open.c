/*
 * open.c
 *
 *  Created on: 2015/01/24
 *      Author: StarRing
 */

#include <fs/open.h>
#include <system.h>
#include <task.h>
#include <slab.h>
#include <fs/vfs.h>
#include <errno.h>
#include <apic.h>
#include <list.h>
#include <smp.h>


int open(const char *filename, int flags, int mode) {

	int ret = 0;

	struct fs_node *node = kopen(filename, mode);

	//ファイルが見つからなかった時
	if(!node) {

		return -ENOENT;
	}

	struct process *process = get_process();

	struct list_head *p;

restart:
	list_for_each(p, &process->fd_list) {

		struct file_descriptor *entry = list_entry(p, struct file_descriptor, list);
		int fd = entry->fd;

		if(ret == fd) {

			ret++;
			goto restart;
		}

	}

	if(ret >= 510) {

		return -EMFILE;
	}

	struct file_descriptor *desc = kmalloc(sizeof(struct file_descriptor), 0);
	desc->fd = ret;
	desc->node = node;
	desc->flags = flags;
	desc->mode = mode;

	list_add_tail(&desc->list, &process->fd_list);


	return ret;
}
































