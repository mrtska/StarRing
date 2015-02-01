/*
 * phys.h
 *
 *  Created on: 2014/05/23
 *      Author: StarRing
 */

#pragma once

#include <multiboot2.h>
#include <mem/alloc.h>


//マルチブート情報のメモリマップ
extern struct multiboot_tag_mmap *mmap_tag;



//使用不可メモリ領域などの情報を物理メモリ管理に登録する
void memory_allocate_and_free(phys_memory_info_t *info);


void *alloc_kernel_memory(unsigned int size);
void free_kernel_memory(void *buf);








