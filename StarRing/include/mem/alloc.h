/*
[Module alloc.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/04/27
*/

#pragma once

#include <page/page.h>



#define MAGIC 0xDEADFACE

#define MEMORY_BLOCK_SIZE	0x200000
#define PHYSICAL_MAP_ADDRESS 0x400000

extern unsigned long memory_size;


//物理メモリ情報
struct phys_memory_info {

	//メモリ総量
	unsigned long memory_size;

	//メモリブロック総量
	unsigned long memory_blocks;

	//割り当て済みメモリブロックの数
	unsigned long alloc_blocks;

	//開放済みメモリブロックの数
	unsigned long free_blocks;

	//管理に使うメモリマップの場所
	unsigned long *bit_map;

	//マップのサイズ
	unsigned long map_size;

};

//使用不可メモリ領域などの情報を物理メモリ管理に登録する
void memory_allocate_and_free(void);

unsigned long get_max_physical_address(void);
void parse_mmap(void);
void *alloc_kernel_memory(unsigned int size);
void free_kernel_memory(void *buf);


void allocator_init(unsigned long memory_size);

void find_memory_block(unsigned long *block_num);

void alloc_blocks(unsigned long physical_addr, unsigned long size);

void free_blocks(unsigned long physical_addr, unsigned long size);

void *alloc_memory_block(void);

void free_memory_block(void *phys_addr);



void *alloc_memory_block4k(void);

void free_memory_block4k(void *p);

static __inline__ void* alloc_memory_block_4k_phys(void) {

	void *p = alloc_memory_block4k();
	p = (void*) VIRTUAL_ADDRESS_TO_PHYSICAL_ADDRESS(p);
	return p;
}

static __inline__ void alloc_memory_block_4k_phys(void* p) {

	p = (void*) PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(p);
	free_memory_block4k(p);
}


void allocator_4k_init(void);





















