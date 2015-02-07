/*
[Module mmap.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2015/01/29
*/

#include <system.h>
#include <fs/mmap.h>
#include <page/page.h>
#include <errno.h>
#include <task.h>
#include <apic.h>
#include <smp.h>
#include <mem/alloc.h>
#include <list.h>


static void register_mmap_entry(struct process *process, unsigned long addr, unsigned long len) {

	struct mmap_entry *entry = kmalloc(sizeof(struct mmap_entry), 0);
	entry->addr = addr;
	entry->len = len - 1;
	list_add_tail(&entry->list, &process->mmap_list);
}

static unsigned long get_mmap_address(struct process *process, unsigned long len) {

	unsigned long base = process->mmap_base;

	struct list_head *ptr;
	list_for_each(ptr, &process->mmap_list) {

		struct mmap_entry *entry = list_entry(ptr, struct mmap_entry, list);
		unsigned long start = entry->addr;
		unsigned long end = start + entry->len;

		if(base >= start && base <= end) {

			base += entry->len;
		}
	}
	register_mmap_entry(process, base, len);
	return base;
}

static unsigned long mmap_anon_4k(unsigned long len, unsigned int page_flags) {

	struct process *process = get_process();
	unsigned long addr = get_mmap_address(process, len);

	map_page(addr, (unsigned long) alloc_memory_block_4k_phys(), process->page_tables, page_flags);

	return addr;
}





unsigned long mmap(unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, int fd, unsigned long offset) {


	//struct process *process = get_process();
	unsigned int page_flags = FLAGS_USER_PAGE;

	if(prot & PROT_WRITE) {

		page_flags |= FLAGS_WRITABLE_PAGE;
	}

	if(flags & MAP_ANONYMOUS) {

		if(addr == 0) {

			if(len < MEMORY_BLOCK_SIZE) {

				return mmap_anon_4k(len, page_flags);
			} else {

				trace();
			}
		}

		return 0;
	} else {

		if(fd == -1) {

			return -EBADF;
		}




	}


	STOP;
	return 0;
}

















