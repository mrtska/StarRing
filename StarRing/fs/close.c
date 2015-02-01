/*
[Module close.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php

Created on: 2015/02/02
*/

#include <system.h>
#include <fs/close.h>
#include <smp.h>
#include <apic.h>
#include <errno.h>

int close(unsigned int fd) {

	struct process *process = get_process();

	struct list_head *ptr;
	list_for_each(ptr, &process->fd_list) {

		struct file_descriptor *desc = list_entry(ptr, struct file_descriptor, list);
		if(desc->fd == fd) {	//対応しているファイルディスクリプタを見つけ、削除する

			close_fs(desc->node);
			list_del(&desc->list);
			kfree(desc, sizeof(struct file_descriptor));
			return 0;
		}
	}
	return -EBADF;
}
