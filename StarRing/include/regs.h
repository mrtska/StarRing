/*
[Module regs.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/04/27
*/

#pragma once


//レジスタをまとめた構造体群

typedef struct {

	unsigned long rip;
	unsigned long cs;
	unsigned long rflags;
	unsigned long rsp;
	unsigned long ss;
} intr_regs;

typedef struct {

	unsigned long rbp;
	unsigned long r15;
	unsigned long r14;
	unsigned long r13;
	unsigned long r12;
	unsigned long r11;
	unsigned long r10;
	unsigned long r9;
	unsigned long r8;
	unsigned long rdi;
	unsigned long rsi;
	unsigned long rdx;
	unsigned long rcx;
	unsigned long rbx;
	unsigned long rax;
} comm_regs;

struct regs {

	comm_regs c;
	intr_regs i;
};

struct regs_with_error {

	comm_regs c;
	unsigned long reg1;
	intr_regs i;
};
