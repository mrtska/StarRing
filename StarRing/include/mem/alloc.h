/*
[Module alloc.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/04/27
*/

#pragma once



#define MAGIC 0xDEADFACE

#define MEMORY_BLOCK_SIZE	0x200000

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




void allocator_init(unsigned long memory_size);

void find_memory_block(unsigned long *block_num, int num);

void alloc_blocks(unsigned long physical_addr, unsigned long size);

void free_blocks(unsigned long physical_addr, int size);

void *alloc_memory_block(int page_num);

void free_memory_block(void *phys_addr);

























