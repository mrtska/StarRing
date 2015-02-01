/*
[Module gdt.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/04/05
*/

#include <desc.h>
#include <gdt.h>
#include <internal/stdio.h>
#include <apic.h>

//GDT本体
struct gdt_page default_gdt_page = {

		.gdt = {
				[GDT_ENTRY_KERNEL_CS] = GDT_ENTRY_INIT(0xA09B, 0, 0xFFFFF),
				[GDT_ENTRY_KERNEL_DS] = GDT_ENTRY_INIT(0xC093, 0, 0xFFFFF),
				[GDT_ENTRY_DEFAULT_USER32_CS] = GDT_ENTRY_INIT(0xC0FB, 0, 0xFFFFF),
				[GDT_ENTRY_DEFAULT_USER_DS] = GDT_ENTRY_INIT(0xC0F3, 0, 0xFFFFF),
				[GDT_ENTRY_DEFAULT_USER_CS] = GDT_ENTRY_INIT(0xA0FB, 0, 0xFFFFF),
				[GDT_ENTRY_KERNEL_TSS] = GDT_ENTRY_INIT(0, 0, 0), }
};

//GDTベースポインタ
static volatile unsigned char *gdt_base;

//GDT初期化
void gdt_init(void) {

	int apic_id = apic_read(APIC_ID_R) >> 24;

	if(apic_id == 0) {

		gdt_base = alloc_kernel_memory(0x200000);
	}
	struct gdt_page *gdt_page = (struct gdt_page*) &gdt_base[(apic_id * 0x1000)];

	int i;
	for(i = 0; i < 0x10; i++) {

		gdt_page->gdt[i].data = default_gdt_page.gdt[i].data;
	}


	struct desc_ptr *gdtr = kmalloc(sizeof(struct desc_ptr), 0);

	gdtr->size = sizeof(struct gdt_page) - 1;
	gdtr->address = (unsigned long) gdt_page;

	kprintf("gdt_page %p, %p\n", gdt_page, gdtr);

	lgdt(gdtr);


	asmv("mov $0x0, %ax");
	asmv("mov %ax, %ds");
	asmv("mov %ax, %es");
	asmv("mov %ax, %fs");
	asmv("mov %ax, %gs");
	asmv("mov %ax, %ss");

}

struct gdt_page *get_gdt(int cpu) {

	return (struct gdt_page*) &gdt_base[(cpu * 0x1000)];
}

