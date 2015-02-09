/*
[Module mmap.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2015/01/29
*/

#pragma once

#include <list.h>


#define PROT_READ	0x1		// 読込可能ページ
#define PROT_WRITE	0x2		// 書込可能ページ
#define PROT_EXEC	0x4		// 実行可能ページ
#define PROT_SEM	0x8		// page may be used for atomic ops
#define PROT_NONE	0x0		// アクセス不可ページ
#define PROT_GROWSDOWN	0x01000000	// mprotect flag: extend change to start of growsdown vma
#define PROT_GROWSUP	0x02000000	// mprotect flag: extend change to end of growsup vma

#define MAP_SHARED	0x01		// 変更を共有するマップ
#define MAP_PRIVATE	0x02		// 変更を共有しないマップ
#define MAP_TYPE	0x0f		// Mask for type of mapping
#define MAP_FIXED	0x10		// 指定したアドレスにマップする
#define MAP_ANONYMOUS	0x20	// ファイルをマップしない
#define MAP_32BIT	0x40		// 32ビット領域にマップ
#define MAP_GROWSDOWN	0x0100		// stack-like segment
#define MAP_DENYWRITE	0x0800		// ETXTBSY
#define MAP_EXECUTABLE	0x1000		// mark it as an executable
#define MAP_LOCKED	0x2000		// pages are locked
#define MAP_NORESERVE	0x4000		// don't check for reservations
#define MAP_POPULATE	0x8000		// populate (prefault) pagetables
#define MAP_NONBLOCK	0x10000		// do not block on IO
#define MAP_STACK	0x20000		// give out an address that is best suited for process/thread stacks
#define MAP_HUGETLB	0x40000		// create a huge page mapping


struct mmap_entry {

	struct list_head list;
	unsigned long addr;
	unsigned long len;
};


unsigned long mmap(unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, int fd, unsigned long offset);
long mprotect(unsigned long start, size_t len, unsigned long prot);
long munmap(unsigned long addr, unsigned long len);

















