/*
[Module ]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2015/01/07
*/

#pragma once

#include <system.h>



//仮想メモリを管理する
struct vmm {





	unsigned char *base;
};





void vmm_allocator_init();
void vmm_allocator_post_init(void);
void *vmm_page_table_alloc(void);
void vmm_page_table_free(void *table);
