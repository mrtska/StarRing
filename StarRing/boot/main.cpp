
#include <system.h>
#include <stdio.h>
#include <gdt.h>
#include <idt.h>

#include <physicalmemory.h>
#include <virtualmemory.h>
#include <slaballocator.h>

#include <apic.h>
#include <trap.h>

#include <acpi.h>
#include <pci.h>

#include <drivers/keyboard.h>
#include <drivers/hpet.h>



//レガシーPICを無効化
static void disable_legacy_pic() {

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

	//例外管理初期化
	trap_init();

	//物理メモリ管理初期化
	physical_memory.physical_memory_init(mboot);

	//仮想メモリ管理初期化
	virtual_memory.virtual_memory_init();
	STOP;

STOP;

	//Advanced Programmable Interrupt Controller初期化
	apic.apic_init();

	//キーボード初期化
	keyboard.keyboard_init();

	//カーネル用スラブアロケータ初期化
	slab_allocator.slab_allocator_init();

	//ACPI管理初期化
	//acpi.acpi_init();

	//PCIデバイス初期化
	pci.pci_init();

	hpet.hpet_init();


	kprintf("return");

	asmv("sti");


	STOP;
	return;

}





