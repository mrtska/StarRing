/*
 * alloc.h
 *
 *  Created on: 2014/04/27
 *      Author: StarRing
 */

#pragma once



#define MAGIC 0xDEADFACE

#define MEMORY_BLOCK_SIZE	0x200000

extern unsigned long memory_size;


//物理メモリ情報
typedef struct phys_memory_info {

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

} phys_memory_info_t;



extern phys_memory_info_t phys_memory;





static __inline__ void set_bit(unsigned long num) {

	phys_memory.bit_map[num / 64] |= (1 << (num % 64));
}

static __inline__ void clear_bit(unsigned long num) {

	phys_memory.bit_map[num / 64] &= ~(1 << (num % 64));
}

static __inline__ _Bool test_bit(unsigned long num) {


	return (phys_memory.bit_map[num / 64] & (1 << (num % 64))) > 0;
}





void allocator_init(unsigned long memory_size);

void find_memory_block(unsigned long *block_num, int num);

void alloc_blocks(unsigned long physical_addr, unsigned long size);

void free_blocks(unsigned long physical_addr, int size);

void *alloc_memory_block(int page_num);

void free_memory_block(void *phys_addr);

























