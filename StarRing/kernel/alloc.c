/*
 [Module alloc.c]
 Copyright(c) 2015 mrtska.starring

 This software is released under the MIT License.

 http://opensource.org/licenses/mit-license.php
 Created on: 2014/04/27
 */

#include <system.h>
#include <mem/alloc.h>
#include <spinlock.h>
#include <string.h>
#include <internal/stdio.h>

static struct phys_memory_info phys_memory;
static __inline__ void set_bit(unsigned long num) {

	phys_memory.bit_map[num / 64] |= (1 << (num % 64));
}

static __inline__ void clear_bit(unsigned long num) {

	phys_memory.bit_map[num / 64] &= ~(1 << (num % 64));
}

static __inline__ _Bool test_bit(unsigned long num) {

	return (phys_memory.bit_map[num / 64] & (1 << (num % 64))) > 0;
}


//空いているメモリブロックを探す
void find_memory_block(unsigned long *block_num) {

	unsigned int bitmap_index;
	unsigned int bit_count;

	for(bitmap_index = 0; bitmap_index < phys_memory.map_size; bitmap_index++) {

		//このunsigned longのビットが全て立っていたらとっとと次へ
		if(phys_memory.bit_map[bitmap_index] != 0xFFFFFFFFFFFFFFFF) {

			//全てのビットを走査
			for(bit_count = 0; bit_count < 64; bit_count++) {

				if(!test_bit((bitmap_index * 64) + bit_count)) {

					*block_num = bitmap_index * (sizeof(unsigned long) * 8) + bit_count;
					return;
				}
			}
		}
	}

	kprintf("[memoryallocater/find_memory_block] free memory not found. %d\n", bitmap_index);
	STOP;
	return;
}

//1ページ分(2MB)を使用中にする
static void __alloc_blocks(unsigned long phys_addr) {

	//ビットマップのナンバー
	unsigned long num = phys_addr / MEMORY_BLOCK_SIZE;
	set_bit(num);
	phys_memory.alloc_blocks++;
	phys_memory.free_blocks--;
}

//使用中のメモリアドレスをメモリマネージャに登録する
void alloc_blocks(unsigned long physical_addr, unsigned long size) {

	physical_addr = physical_addr & 0xFFFFFFFFFFF00000;

	//サイズをMEMORY_BLOCK_SIZEで割り、何ビット使用中にするか計算する
	size /= MEMORY_BLOCK_SIZE;
	size = (size == 0) ? 1 : size;

	//kprintf("[kernel/alloc_blocks] phys %p, count %p\n", physical_addr, size);
	int i;
	for(i = 0; i < size; i++) {

		__alloc_blocks(physical_addr);
		physical_addr += MEMORY_BLOCK_SIZE;
	}
}

//使用後のメモリアドレスをメモリマネージャから解放する
void free_blocks(unsigned long physical_addr, unsigned long size) {

	unsigned long num = (unsigned long) physical_addr / MEMORY_BLOCK_SIZE;
	size /= MEMORY_BLOCK_SIZE;
	size = (size == 0) ? 1 : size;
	int i;
	for(i = 0; i < size; i--) {

		clear_bit(num++);
		phys_memory.alloc_blocks--;
		phys_memory.free_blocks++;
	}
}
//1ページ分メモリブロックを割り当てる
void *alloc_memory_block(void) {

	unsigned long num;
	void *physical_addr;

	if(phys_memory.free_blocks <= 0) {
		trace();
		return NULL;
	}

	find_memory_block(&num);
	set_bit(num);

	physical_addr = (void*) (num * MEMORY_BLOCK_SIZE);

	phys_memory.alloc_blocks++;
	phys_memory.free_blocks--;
//	kprintf("[alloc_memory_block] physical addr = %p\n", physical_addr);
	return physical_addr;
}

//1ページ分メモリブロックを開放する
void free_memory_block(void *phys_addr) {

	unsigned long num = (unsigned long) phys_addr / MEMORY_BLOCK_SIZE;

	clear_bit(num);

	phys_memory.alloc_blocks--;
	phys_memory.free_blocks++;

	memset((void*) PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(phys_addr), 0x00, MEMORY_BLOCK_SIZE);
}


static struct kmem_cache *cache_4k;

void *alloc_memory_block4k(void) {


	return kmem_cache_alloc(cache_4k, 0);
}

void free_memory_block4k(void *p) {

	kmem_cache_free(cache_4k, p);
}

void allocator_4k_init(void) {


	cache_4k = kmem_cache_create("4kb_page", 0x1000, 0x1000, 0, NULL);

}

//物理メモリ割り当てerの初期化 memory_sizeはbyte単位
void allocator_init(unsigned long memory_size) {


	void *physical_memory_bitmap = (void*) 0xFFFF880000400000;


	phys_memory.memory_size = memory_size;							//メモリサイズを代入
	phys_memory.memory_blocks = memory_size / MEMORY_BLOCK_SIZE;	//メモリブロックサイズで割って必要なメモリブロック数を代入する
	phys_memory.free_blocks = phys_memory.memory_blocks;			//初期値は0
	phys_memory.alloc_blocks = 0;									//メモリブロック数分割り当て済みにする この後に利用可能な箇所を増やしていく
	phys_memory.bit_map = physical_memory_bitmap;					//実体はbootlinker.ldに 最大32TBまで対応

	phys_memory.map_size = phys_memory.memory_blocks / 64;			//unsigned longで一つの要素に付き64x0x200000バイト管理できるので64で割ったサイズがマップサイズになる
	memset(physical_memory_bitmap, 0x00, phys_memory.map_size);
	kprintf("memory blocks = %p\n", phys_memory.bit_map);

	memory_allocate_and_free();							//メモリマップをビットマップに反映する

	//メモリマップをパースする
	parse_mmap();



}



void *alloc_kernel_memory(unsigned int size) {

	void *p = alloc_memory_block();

	p += 0xFFFF880000000000;
	return p;
}
void free_kernel_memory(void *buf) {

	buf -= 0xFFFF880000000000;
	free_memory_block(buf);
}

//使用不可メモリ領域などの情報を物理メモリ管理に登録する
void memory_allocate_and_free(void) {

	// カーネル本体が置かれている領域も割り当て済みにする

	alloc_blocks(KERNEL_START, KERNEL_END);
	alloc_blocks(PHYSICAL_MAP_ADDRESS, MEMORY_BLOCK_SIZE);
}

