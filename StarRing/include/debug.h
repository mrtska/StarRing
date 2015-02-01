/*
 * debug.h
 *
 *  Created on: 2015/01/26
 *      Author: StarRing
 */

#pragma once


#include <system.h>













static __inline__ __attribute__((always_inline)) void enable_single_step(void) {

	asmv("pushf");
	asmv("pop %rax");
	asmv("bts $8, %rax");
	asmv("push %rax");
	asmv("popf");
}


















