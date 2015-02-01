/*
 * vmm.c
 *
 *  Created on: 2015/01/07
 *      Author: StarRing
 */

#include <mem/vmm.h>
#include <page/page.h>
#include <mem/alloc.h>

struct vmm vmm;


//仮想メモリマネージャー初期化
void vmm_allocator_init() {


	unsigned long p = (unsigned long)  alloc_memory_block(1);
	p += 0xFFFF880000000000;
	vmm.base = (unsigned long*) p;



	kprintf("vmm = %p\n", &vmm.base[0x1000]);


}

void vmm_allocator_post_init(void) {


}

//ページテーブルを作るための領域を確保する 使用しないと再度アロケートされる
void *vmm_page_table_alloc(void) {

	int i;
	for(i = 0; i < 0x200000; i += 0x1000) {

		if(!vmm.base[i]) {

			vmm.base[i] = 0x40;
			return &vmm.base[i];
		}
	}

	return NULL;

}

//解放する
void vmm_page_table_free(void *table) {

	unsigned char p = (unsigned char) table & 0x1FFFFF;
	vmm.base[p] = 0x0000;




}










