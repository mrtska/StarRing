/*
 * gdt.h
 *
 *  Created on: 2014/04/26
 *      Author: StarRing
 */

#pragma once

#include <desc.h>

#define GDT_ENTRY_KERNEL_CS			1
#define GDT_ENTRY_KERNEL_DS			2
#define GDT_ENTRY_DEFAULT_USER32_CS	3
#define GDT_ENTRY_DEFAULT_USER_DS	4
#define GDT_ENTRY_DEFAULT_USER_CS	5
#define GDT_ENTRY_KERNEL_TSS		6
#define GDT_ENTRY_USER_TSS			7


#define GDT_ENTRY_TLS_MIN 12
#define GDT_ENTRY_TLS_MAX 14

/* TLS indexes for 64bit - hardcoded in arch_prctl */
#define FS_TLS 0
#define GS_TLS 1


#define GDT_ENTRY_FS_TLS_SEL ((GDT_ENTRY_TLS_MIN+FS_TLS)*8 + 3)
#define GDT_ENTRY_GS_TLS_SEL ((GDT_ENTRY_TLS_MIN+GS_TLS)*8 + 3)


#define SEG_DPL0		0x00000000
#define SEG_DPL1		0x00002000
#define SEG_DPL2		0x00004000
#define SEG_DPL3		0x00006000

#define SEG_NOTSYS		0x00001000
#define SEG_PRESENT		0x00008000

#define SEG_CODE_READ	0x00000A00  // 読込#define SEG_CODE_EO		0x00000800  // 実行#define SEG_64BIT_CODE	0x00200000  // 64ビットコードセグメント#define SEG_TSS64		0x00000900  // 64ビットタスクステートセグメント#define SEG_TSS64_BUSY	0x00000B00  // Busy時64ビットタスクステートセグメント
#define SEG_DATA_RO		0x00000000
#define SEG_DATA_RW		0x00000200

#define SEG_BIG			0x00800000
#define SEG_32BIT		0x00400000
#define SEG_RESERV		0x00100000
#define SEG_LDT			0x00000004
#define SEG_GDT			0x00000000
#define SEG_RPL0		0x00000000
#define SEG_RPL1		0x00000001
#define SEG_RPL2		0x00000002
#define SEG_RPL3		0x00000003

#define CALC_SELECTOR(x, y) (((x) << 3) | (y))

#define GDT_ENTRY_INIT(flags, base, limit) { { { 							\
		.a = ((limit) & 0xffff) | (((base) & 0xffff) << 16), 				\
		.b = (((base) & 0xff0000) >> 16) | (((flags) & 0xf0ff) << 8) | 		\
			((limit) & 0xf0000) | ((base) & 0xff000000), 					\
	} } }

extern struct gdt_page gdt_page;


struct desc_struct {
	union {
		struct {
			unsigned int a;
			unsigned int b;
		};
		struct {
			unsigned short limit0;
			unsigned short base0;
			unsigned base1 :8, type :4, s :1, dpl :2, p :1;
			unsigned limit :4, avl :1, l :1, d :1, g :1, base2 :8;
		};

		unsigned long data;
	};
}__attribute__((packed));

typedef struct _seg_descriptor {

	unsigned limit_0_15 :16;
	unsigned base_0_15 :16;
	unsigned base_16_23 :8;
	unsigned access :8;
	unsigned limit_16_19 :4;
	unsigned available :1;
	unsigned longmode :1;
	unsigned special :1;
	unsigned granularity :1;
	unsigned base_24_31 :8;
}__attribute__ ((packed)) seg_descriptor_t;

//GDTページ
struct gdt_page {

	struct desc_struct gdt[16];
}__attribute__((__aligned__(4096)));

//GDTがあるポインタを取得する
struct gdt_page *get_gdt(int cpu);

//GDTを初期化
void gdt_init(void);
