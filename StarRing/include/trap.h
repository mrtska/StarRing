/*
[Module trap.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/04/06
*/

#pragma once

/* Interrupts/Exceptions */
enum {
	X86_TRAP_DE = 0, 	/*  0, Divide-by-zero */
	X86_TRAP_DB, 		/*  1, Debug */
	X86_TRAP_NMI,		/*  2, Non-maskable Interrupt */
	X86_TRAP_BP, 		/*  3, Breakpoint */
	X86_TRAP_OF, 		/*  4, Overflow */
	X86_TRAP_BR, 		/*  5, Bound Range Exceeded */
	X86_TRAP_UD, 		/*  6, Invalid Opcode */
	X86_TRAP_NM, 		/*  7, Device Not Available */
	X86_TRAP_DF, 		/*  8, Double Fault */
	X86_TRAP_OLD_MF,	/*  9, Coprocessor Segment Overrun */
	X86_TRAP_TS, 		/* 10, Invalid TSS */
	X86_TRAP_NP, 		/* 11, Segment Not Present */
	X86_TRAP_SS, 		/* 12, Stack Segment Fault */
	X86_TRAP_GP, 		/* 13, General Protection Fault */
	X86_TRAP_PF, 		/* 14, Page Fault */
	X86_TRAP_SPURIOUS,	/* 15, Spurious Interrupt */
	X86_TRAP_MF, 		/* 16, x87 Floating-Point Exception */
	X86_TRAP_AC, 		/* 17, Alignment Check */
	X86_TRAP_MC, 		/* 18, Machine Check */
	X86_TRAP_XF, 		/* 19, SIMD Floating-Point Exception */
	X86_TRAP_IRET = 32,	/* 32, IRET Exception */
};

//レジスタ値
struct pt_regs {
	unsigned long r15;
	unsigned long r14;
	unsigned long r13;
	unsigned long r12;
	unsigned long bp;
	unsigned long bx;
/* arguments: non interrupts/non tracing syscalls only save up to here*/
	unsigned long r11;
	unsigned long r10;
	unsigned long r9;
	unsigned long r8;
	unsigned long ax;
	unsigned long cx;
	unsigned long dx;
	unsigned long si;
	unsigned long di;
	unsigned long orig_ax;
/* end of arguments */
/* cpu exception frame or undefined */
	unsigned long ip;
	unsigned long cs;
	unsigned long flags;
	unsigned long sp;
	unsigned long ss;
/* top of stack page */
};


//トラップを初期化
void trap_init(void);

