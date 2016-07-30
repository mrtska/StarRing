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
	entry->len = len;
	list_add_tail(&entry->list, &process->mmap_list);
}

static int unregister_mmap_entry(struct process *process, unsigned long addr, unsigned long len) {

	struct list_head *ptr;
	list_for_each(ptr, &process->mmap_list) {

		struct mmap_entry *entry = list_entry(ptr, struct mmap_entry, list);

		if(entry->addr == addr && entry->len == len) {

			//FIXME
			return 0;
		}
	}

	return -1;
}

static unsigned long get_mmap_address(struct process *process, unsigned long len, unsigned long page_size) {

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
	//kprintf("[kernel/mmap_address] return %p\n", base);
	return base;
}

static unsigned long mmap_anon_4k(struct process *process, unsigned long len, unsigned int page_flags) {

	unsigned long addr = get_mmap_address(process, len, 0x1000);

	int i;
	for(i = 0; i < len; i += 0x1000) {

		map_page(addr + i, (unsigned long) alloc_memory_block_4k_phys(), process->page_tables, page_flags);
	}

	return addr;
}

static unsigned long mmap_anon_2m(struct process *process, unsigned long len, unsigned int page_flags) {

	unsigned long addr = get_mmap_address(process, len, 0x200000);

	int i;
	for(i = 0; i < len; i += 0x200000) {

		map_page(addr + i, (unsigned long) alloc_memory_block(), process->page_tables, page_flags | FLAGS_LARGE_PAGE);
	}

	return addr;
}




unsigned long mmap(unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, int fd, unsigned long offset) {


	struct process *process = get_process();
	unsigned int page_flags = FLAGS_USER_PAGE | FLAGS_WRITABLE_PAGE;

	if(prot & PROT_WRITE) {

		page_flags |= FLAGS_WRITABLE_PAGE;
	}

	if(flags & MAP_ANONYMOUS) {

		if(addr == 0) {

			if(len < MEMORY_BLOCK_SIZE) {

				return mmap_anon_4k(process, len, page_flags);
			} else {

				return mmap_anon_2m(process, len, page_flags | FLAGS_LARGE_PAGE);
			}
		} else if(flags & MAP_FIXED) {

			map_page(addr, (unsigned long) alloc_memory_block(), process->page_tables, page_flags);
			return addr;
		}

		trace();
		return 0;
	} else {

		if(fd == -1) {

			return -EBADF;
		}

		struct fs_node *node = get_node(fd);
		if(addr == 0) {

			unsigned long p;

			if(len < MEMORY_BLOCK_SIZE) {

				if(len % 0x1000 != 0) {

					len += 0x1000;
					len &= ~(0x1000 - 1);
				}
				p = mmap_anon_4k(process, len, page_flags);
			} else {

				if(len % 0x200000 != 0) {

					len += 0x200000;
					len &= ~(0x200000 - 1);
				}

				p = mmap_anon_2m(process, len, page_flags | FLAGS_LARGE_PAGE);
			}
			read_fs(node, offset, len, (unsigned char*) p);
			return p;
		} else if(flags & MAP_FIXED) {

			map_page(addr, (unsigned long) alloc_memory_block(), process->page_tables, page_flags | FLAGS_LARGE_PAGE);
			read_fs(node, offset, len, (unsigned char*)addr);
			return addr;
		}
	}


	STOP;
	return 0;
}

long munmap(unsigned long addr, unsigned long len) {


	return unregister_mmap_entry(get_process(), addr, len);
}

long mprotect(unsigned long start, size_t len, unsigned long prot) {

	return 0;
}














