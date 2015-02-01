/*
 * system.h
 *
 *  Created on: 2014/04/02
 *      Author: StarRing
 */


#ifndef __SYSTEM_INCLUDEGUARD__
#define __SYSTEM_INCLUDEGUARD__


#define NAME "StarRing"
#define VERSION 1.0.0

#define KERNEL_START	0x100000
#define KERNEL_END		(0x400000)


#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define zeromem(s, n)	memset(s, 0, n)

typedef unsigned char uc;
typedef unsigned short us;
typedef unsigned int ui;
typedef unsigned long ul;
typedef unsigned long long ull;
typedef _Bool bool;

#define true	1
#define false	0

#define Null ((void*)0)

#include <slab.h>
#include <internal/stdio.h>


//インラインアセンブラ
#define asmv __asm__ __volatile__

#include <debug.h>

#define STOP while(1) asmv("hlt")


#define trace() kprintf("[%s]\n", __FUNCTION__)
#define lfence() asmv("lfence":::"memory")
#define sfence() asmv("sfence":::"memory")
#define mfence() asmv("mfence":::"memory")


#define get_apic_id() apic_read(APIC_ID_R) >> 24
#define get_process() smp_table[get_apic_id()].execute_process


extern bool is_graphics_mode;


extern unsigned long monitor_height;
extern unsigned long monitor_width;
extern unsigned long monitor_depth;
void load_system_font(char *path);




//OUT命令 オペランドサイズ可変
static __inline__ void outb(int port, unsigned char value) {

	asmv("outb %b0, %w1" : : "a"(value), "Nd"(port));
}
static __inline__ void outw(int port, unsigned short value) {

	asmv("outw %w0, %w1" : : "a"(value), "Nd"(port));
}
static __inline__ void outl(int port, unsigned int value) {

	asmv("outl %0, %w1" : : "a"(value), "Nd"(port));
}

//IN命令 オペランドサイズ可変
static __inline__ unsigned char inb(int port) {

	unsigned char value;
	asmv("inb %w1, %0" : "=a"(value) : "Nd"(port));
	return value;
}
static __inline__ unsigned short inw(int port) {

	unsigned short value;
	asmv("inw %w1, %0" : "=a"(value) : "Nd"(port));
	return value;
}
static __inline__ unsigned int inl(unsigned short port) {

	unsigned int value;
	asmv("inl %w1, %0" : "=a"(value) : "Nd"(port));
	return value;
}

//CR3取得
static __inline__ unsigned long read_cr3(void) {

	unsigned long value;
	asmv("mov %%cr3, %0" : "=r"(value) : : "%rax");
	return  value;
}

static __inline__ void write_cr3(unsigned long val) {

	asmv("mov %%rax, %%cr3" : : "a"(val));
}

//CR4取得
static __inline__ unsigned long read_cr4(void) {

	unsigned long value;
	asmv("mov %%cr4, %%rax" : "=a"(value));
	return value;
}

static __inline__ void write_cr4(unsigned long val) {

	asmv("mov %0, %%cr4" : : "r"(val));
}

static __inline__ void write_tr(unsigned int selector) {

	asmv("ltr %0" : : "m"(selector));
}

static __inline__ unsigned long read_cr0(void) {

	unsigned long value;
	asmv("mov %%cr0, %%rax" : "=a"(value));
	return value;
}
static __inline__ unsigned int read_mxcsr(void) {

	unsigned int value;
	asmv("stmxcsr %0" : "=m"(value));
	return value;
}
























#endif /* SYSTEM_H_ */
