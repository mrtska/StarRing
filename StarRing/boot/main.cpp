
#include <system.h>
#include <stdio.h>
#include <gdt.h>
#include <idt.h>

#include <physicalmemory.h>
#include <virtualmemory.h>

#include <apic.h>
#include <trap.h>

#include <drivers/keyboard.h>

//レガシーPICを無効化
static void disable_legacy_pic(void) {

	asmv("cli");

	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);

	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);

	outb(0x21, 0xFF);
	outb(0xA1, 0xFF);
}

void main(unsigned long magic, unsigned long mboot) {


	kprintf("Hello,Eclipse\n");

	disable_legacy_pic();

	//GDT初期化
	gdt.gdt_init();

	//IDT初期化
	idt.idt_init();

	//物理メモリ管理初期化
	physical_memory.physical_memory_init(mboot);

	//仮想メモリ管理初期化
	virtual_memory.virtual_memory_init();

	trap_init();

	//Advanced Programmable Interrupt Controller初期化
	apic.apic_init();



	keyboard.keyboard_init();



	kprintf("return\n");

	asmv("sti");
	STOP;
	return;

}





