/*
[Module mmap.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2015/01/29
*/

#pragma once


#define PROT_READ	0x1		// page can be read
#define PROT_WRITE	0x2		// page can be written
#define PROT_EXEC	0x4		// page can be executed
#define PROT_SEM	0x8		// page may be used for atomic ops
#define PROT_NONE	0x0		// page can not be accessed
#define PROT_GROWSDOWN	0x01000000	// mprotect flag: extend change to start of growsdown vma
#define PROT_GROWSUP	0x02000000	// mprotect flag: extend change to end of growsup vma

#define MAP_SHARED	0x01		// Share changes
#define MAP_PRIVATE	0x02		// Changes are private
#define MAP_TYPE	0x0f		// Mask for type of mapping
#define MAP_FIXED	0x10		// Interpret addr exactly
#define MAP_ANONYMOUS	0x20		//	 don't use a file
#define MAP_32BIT	0x40		// only give out 32bit addresses
#define MAP_GROWSDOWN	0x0100		// stack-like segment
#define MAP_DENYWRITE	0x0800		// ETXTBSY
#define MAP_EXECUTABLE	0x1000		// mark it as an executable
#define MAP_LOCKED	0x2000		// pages are locked
#define MAP_NORESERVE	0x4000		// don't check for reservations
#define MAP_POPULATE	0x8000		// populate (prefault) pagetables
#define MAP_NONBLOCK	0x10000		// do not block on IO
#define MAP_STACK	0x20000		// give out an address that is best suited for process/thread stacks
#define MAP_HUGETLB	0x40000		// create a huge page mapping





unsigned long mmap(unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, int fd, unsigned long offset);


















