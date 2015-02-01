/*
[Module page.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/04/28
*/

#pragma once


#include <internal/stdio.h>
#include <system.h>
#include <task.h>

#define FLAGS_LARGE_PAGE	0x01
#define FLAGS_WRITABLE_PAGE	0x02
#define FLAGS_USER_PAGE		0x04
#define FLAGS_ALLOC_512		0x08
#define FLAGS_NEW_ALLOC		0x10
#define FLAGS_NO_EXECUTE	0x20


#define PG_PRESENT      (1<<0)            /* Page is present. */
#define PG_WRITE        (1<<1)            /* Page is writable. */
#define PG_USER         (1<<2)            /* Page is accessible in CPL3. */
#define PG_PWT          (1<<3)            /* Page has write-through caching. */
#define PG_NOCACHE      (1<<4)            /* Page has caching disabled. */
#define PG_ACCESSED     (1<<5)            /* Page has been accessed. */
#define PG_DIRTY        (1<<6)            /* Page has been written to. */
#define PG_LARGE        (1<<7)            /* Page is a large page. */
#define PG_GLOBAL       (1<<8)            /* Page won't be cleared in TLB. */
#define PG_NX			0x8000000000000000

#define MASK_PDE_ADDR	0x0000FFFFFFE00000
#define MASK_FRAME_ADDR	0x000FFFFFFFFFF000





#define VIRTUAL_ADDRESS_TO_PHYSICAL_ADDRESS(p) ((unsigned long)p - 0xFFFF880000000000)
#define PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(p) ((unsigned long)p + 0xFFFF880000000000)



extern unsigned long *kernel_pml4;






//ページング初期化
void page_init();

void setup_user_page(struct process *process);
void combine_kernel_pml4(unsigned long destination_pml4);

void map_page(unsigned long virt, unsigned long phys, unsigned long *cr3, int flag);
unsigned long get_page_directory(unsigned long virt, unsigned long cr3);

//CR2取得
static __inline__ unsigned long get_cr2(void) {

	unsigned long value;
	asmv("mov %%cr2, %%rax" : "=a"(value));
	return value;
}

static __inline__ unsigned long pml4_index(unsigned long addr) {

	return (addr >> 39) & 0x1FF;
}

static __inline__ unsigned long pdpt_index(unsigned long addr) {

	return (addr >> 30) & 0x1FF;
}

static __inline__ unsigned long pd_index(unsigned long addr) {

	return (addr >> 21) & 0x1FF;
}

static __inline__ unsigned long pt_index(unsigned long virtual_addr) {

	return (virtual_addr >> 12) & 0x1FF;
}
