/*
[Module desc.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/04/04
*/

#pragma once

#include <system.h>
#include <string.h>
#include <gdt.h>

#define GDT_ENTRIES 16


//アドレス取得
#define PTR_LOW(x) ((unsigned long long)(x) & 0xFFFF)
#define PTR_MIDDLE(x) (((unsigned long long)(x) >> 16) & 0xFFFF)
#define PTR_HIGH(x) ((unsigned long long)(x) >> 32)

//ディスクリプタ用構造体
struct desc_struct64 {

	unsigned short offset_low;
	unsigned short segment;
	unsigned ist : 3, zero0 : 5, type : 5, dpl : 2, p : 1;
	unsigned short offset_middle;
	unsigned int offset_high;
	unsigned int zero1;
} __attribute__((packed));

//ゲート用構造体
struct gate_struct {

	unsigned short offset_low;
	unsigned short segment;
	unsigned ist : 3, zero0 : 5, type : 5, dpl : 2, p : 1;
	unsigned short offset_middle;
	unsigned int offset_high;
	unsigned int zero1;
} __attribute__((packed));


//ディスクリプタとサイズを保持する構造体
struct desc_ptr {

	unsigned short size;
	unsigned long address;
} __attribute__((packed));

enum {
	GATE_INTERRUPT = 0xE,
	GATE_TRAP = 0xF,
	GATE_CALL = 0xC,
	GATE_TASK = 0x5,
};

typedef struct desc_struct64 gate_desc;

extern gate_desc idt_table[256];







//IDTをロード
static __inline__ void lidt(const struct desc_ptr *ptr) {

	asmv("lidt %0" : : "m"(*ptr));
}

//GDTをロード
static __inline__ void lgdt(const struct desc_ptr *ptr) {

	asmv("lgdt %0" : : "m"(*ptr));
}

static __inline__ void write_idt(gate_desc *idt, int entry, const gate_desc *gate) {

	memcpy(&idt[entry], gate, sizeof(*gate));
}




static __inline__ void pack_gate(gate_desc *gate, unsigned type, unsigned long func, unsigned dpl, unsigned ist, unsigned seg) {

	gate->offset_low	= PTR_LOW(func);
	gate->segment		= seg;
	gate->ist			= ist;
	gate->p				= 1;
	gate->dpl			= dpl;
	gate->zero0			= 0;
	gate->zero1			= 0;
	gate->type			= type;
	gate->offset_middle	= PTR_MIDDLE(func);
	gate->offset_high	= PTR_HIGH(func);
}

//内部 直接使用不可
static __inline__ void _set_gate(int gate, unsigned type, void *addr, unsigned dpl, unsigned ist, unsigned seg) {

	gate_desc s;

	pack_gate(&s, type, (unsigned long)addr, dpl, ist, seg);

	write_idt(idt_table, gate, &s);
}

//IDTゲートをセット
static __inline__ void set_intr_gate(int gate, void *addr) {

	_set_gate(gate, GATE_INTERRUPT, addr, 0, 0, 8);
}
//IDTゲートと一緒にISTビットも指定する
static __inline__ void set_intr_trap(int gate, void *addr, unsigned ist) {

	_set_gate(gate, GATE_INTERRUPT, addr, 0, 0, 8);
}
static __inline__ void set_system_intr_gate_ist(int gate, void *addr, unsigned ist) {

	_set_gate(gate, GATE_INTERRUPT, addr, 3, ist, 8);
}



//IDT初期化
void idt_init(void);





































