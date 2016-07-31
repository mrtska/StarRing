#pragma once



#include <string.h>





//MEMBER�̃I�t�Z�b�g���v�Z���� GCC�̂��̂��g��
#ifndef offsetof
#define offsetof(TYPE, MEMBER) \
	((size_t) ((char*)&((TYPE *)(0))->MEMBER - (char*)0))
#endif






//�\���̂̃t�B�[���h�̃I�t�Z�b�g���v�Z����
#define container_of(ptr, type, member) ({						\
		const typeof(((type*)0)->member) *__mptr = (ptr);		\
		(type*)((char*)__mptr - offsetof(type, member));})		\



