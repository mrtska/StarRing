/*
[Module phys.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/05/23
*/

#include <mem/phys.h>
#include <multiboot2.h>
#include <internal/stdio.h>
#include <system.h>
#include <string.h>
#include <page/page.h>

extern unsigned long kernel_end;

//使用不可メモリ領域などの情報を物理メモリ管理に登録する
void memory_allocate_and_free(phys_memory_info_t *info) {


	alloc_blocks(0, 0x100000);

	/* カーネル本体が置かれている領域も割り当て済みにする */
	const unsigned long len = ((unsigned long) &kernel_end - 0xFFFFFFFF80000000) / MEMORY_BLOCK_SIZE;

	alloc_blocks(KERNEL_START, len);


}

void *alloc_kernel_memory(unsigned int size) {

	size |= MEMORY_BLOCK_SIZE;
	void *p = alloc_memory_block(size / MEMORY_BLOCK_SIZE);

	p += 0xFFFF880000000000;
	return p;
}
void free_kernel_memory(void *buf) {

	buf -= 0xFFFF880000000000;
	free_memory_block(buf);
}

