/*
 * gdt.cpp
 *
 *  Created on: 2016/08/08
 *      Author: StarRing
 */

#include <gdt.h>
#include <system.h>


extern void *global_descriptor_table;

class gdt gdt(reinterpret_cast<void*>(&global_descriptor_table));


void gdt::load_gdtr() {

	struct {

		unsigned short limit;
		unsigned long base;
	} __attribute__((packed)) gdtr;


	gdtr.limit = 15;
	gdtr.base = reinterpret_cast<unsigned long>(gdt_pointer);

	asmv("lgdt %0" :: "m"(gdtr));

	asmv("mov $0x0, %ax");
	asmv("mov %ax, %ds");
	asmv("mov %ax, %es");
	asmv("mov %ax, %fs");
	asmv("mov %ax, %gs");
	asmv("mov %ax, %ss");
}


//GDT初期化
void gdt::gdt_init(void) {




#define GDT_ENTRY_INIT(selector, base, limit, flags) \
	this->gdt_pointer[selector].a = ((limit) & 0xffff) | (((base) & 0xffff) << 16); \
	this->gdt_pointer[selector].b = (((base) & 0xff0000) >> 16) | (((flags) & 0xf0ff) << 8) | ((limit) & 0xf0000) | ((base) & 0xff000000);


	GDT_ENTRY_INIT(GDT_ENTRY_KERNEL_CS, 0, 0xFFFFF, 0xA09B);
	GDT_ENTRY_INIT(GDT_ENTRY_KERNEL_DS, 0, 0xFFFFF, 0xC093);



	this->load_gdtr();

}
