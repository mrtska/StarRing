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
#include <mem/phys.h>
#include <page/page.h>

phys_memory_info_t phys_memory;
extern unsigned long physical_memory_bitmap;


//空いているメモリブロックを探す
void find_memory_block(unsigned long *block_num, int num) {

	unsigned int bitmap_index;
	unsigned int bit_count;
	int success = 0;

	for(bitmap_index = 0; bitmap_index < phys_memory.map_size; bitmap_index++) {

		//このunsigned longのビットが全て立っていたらとっとと次へ
		if(phys_memory.bit_map[bitmap_index] != 0xFFFFFFFFFFFFFFFF) {

			//全てのビットを走査
			for(bit_count = 0; bit_count < 64; bit_count++) {

				int i;
				for(i = 0; i < num; i++) {
					if(!test_bit(bit_count - i)) {

						success++;
						if(success == num) {
							*block_num = bitmap_index * (sizeof(unsigned long) * 8) + bit_count;
							return;
						}
						continue;
					} else {

						success = 0;
					}
				}
			}
		}
	}
	*block_num = 0x7E;
	return;
}

//使用中のメモリアドレスをメモリマネージャに登録する
void alloc_blocks(unsigned long physical_addr, unsigned long size) {

	unsigned long num = physical_addr / MEMORY_BLOCK_SIZE;
	size = size / MEMORY_BLOCK_SIZE;
	int i;
	for(i = size; 0 <= i; i--) {

		set_bit(num);
		phys_memory.alloc_blocks++;
		phys_memory.free_blocks--;
		if(num != 0) {
			num--;
		}
	}
}

//使用後のメモリアドレスをメモリマネージャから解放する
void free_blocks(unsigned long physical_addr, int size) {

	unsigned long num = (unsigned long) physical_addr / MEMORY_BLOCK_SIZE;
	size = size / MEMORY_BLOCK_SIZE;

	int i;
	for(i = size; 0 <= i; i--) {

		clear_bit(num--);

		phys_memory.alloc_blocks--;
		phys_memory.free_blocks++;
	}

}
//nページ分メモリブロックを割り当てる
void *alloc_memory_block(int page_num) {

	unsigned long num;
	void *physical_addr;


	if(phys_memory.free_blocks <= 0) {
		trace();
		return NULL;
	}

	find_memory_block(&num, page_num);

	int i;
	for(i = 0; i < page_num; i++) {

		set_bit(num - i);
	}

	physical_addr = (void*) (num * MEMORY_BLOCK_SIZE);

	phys_memory.alloc_blocks++;
	phys_memory.free_blocks--;
	//kprintf("[alloc_memory_block] physical addr = %p\n", physical_addr);
	return physical_addr;
}

//1ページ分メモリブロックを開放する
void free_memory_block(void *phys_addr) {

	unsigned long num = (unsigned long) phys_addr / MEMORY_BLOCK_SIZE;

	clear_bit(num);

	phys_memory.alloc_blocks--;
	phys_memory.free_blocks++;

	memset((void*)PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(phys_addr), 0x00, MEMORY_BLOCK_SIZE);
}


//物理メモリ割り当てerの初期化　memory_sizeはbyte単位
void allocator_init(unsigned long memory_size) {

	phys_memory.memory_size		= memory_size;						//メモリサイズを代入
	phys_memory.memory_blocks	= memory_size / MEMORY_BLOCK_SIZE;	//メモリブロックサイズで割って総メモリブロック数を代入する
	phys_memory.free_blocks		= 0;								//初期値は0
	phys_memory.alloc_blocks	= phys_memory.memory_blocks;		//メモリブロック数分割り当て済みにする この後に利用可能な箇所を増やしていく
	phys_memory.bit_map			= &physical_memory_bitmap;			//実体はbootlinker.ldに 最大32GBまで対応

	phys_memory.map_size = phys_memory.memory_blocks / 64;			//unsigned longで一つの要素に付き64x0x200000バイト管理できるので64で割ったサイズがマップサイズになる

	kprintf("memory blocks = %p\n", phys_memory.map_size);
	//memset(phys_memory.bit_map, 0xFF, phys_memory.map_size * 8);	//メモリマップを0で初期化する 一つのマップの要素に8バイト使うので8を掛ける

	memory_allocate_and_free(&phys_memory);							//メモリマップをビットマップに反映する
	//kprintf("Allocator init | Bit Map Size = %u, memory capacity %uB\n", phys_memory.map_size, memory_size);



}

