/*
[Module stddef.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/07/24
*/

#pragma once

#include <string.h>





//MEMBERのオフセットを計算する GCCのものを使う
#ifndef offsetof
#define offsetof(TYPE, MEMBER) \
	((size_t) ((char*)&((TYPE *)(0))->MEMBER - (char*)0))
#endif






//構造体のフィールドのオフセットを計算する
#define container_of(ptr, type, member) ({						\
		const typeof(((type*)0)->member) *__mptr = (ptr);		\
		(type*)((char*)__mptr - offsetof(type, member));})		\
















