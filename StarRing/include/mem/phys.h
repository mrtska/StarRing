/*
[Module phys.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/05/23
*/

#pragma once

#include <multiboot2.h>
#include <mem/alloc.h>


//マルチブート情報のメモリマップ
extern struct multiboot_tag_mmap *mmap_tag;



//使用不可メモリ領域などの情報を物理メモリ管理に登録する
void memory_allocate_and_free(struct phys_memory_info *info);


void *alloc_kernel_memory(unsigned int size);
void free_kernel_memory(void *buf);








