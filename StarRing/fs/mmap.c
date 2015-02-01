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

unsigned long mmap(unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, int fd, unsigned long offset) {



	unsigned int page_flags = FLAGS_USER_PAGE;

	if(prot & PROT_WRITE) {

		page_flags |= FLAGS_WRITABLE_PAGE;
	}

	if(flags & MAP_ANONYMOUS) {

		trace();
	} else {

		if(fd == -1) {

			return -EBADF;
		}


	}


	STOP;
	return 0;
}

















