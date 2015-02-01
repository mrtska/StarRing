/*
 * prctl.h
 *
 *  Created on: 2015/01/23
 *      Author: StarRing
 */

#pragma once



#define ARCH_SET_GS 0x1001
#define ARCH_SET_FS 0x1002
#define ARCH_GET_FS 0x1003
#define ARCH_GET_GS 0x1004





long arch_prctl(int code, unsigned long addr);
