
#include <system.h>
#include <stdio.h>
#include <gdt.h>
#include <idt.h>

#include <physicalmemory.h>
#include <virtualmemory.h>
#include <slaballocator.h>
#include <wait.h>
#include <smp.h>

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

extern unsigned int _bss_start;
extern unsigned int _bss_end;

//BSSセクション初期化
extern "C" void bss_init(void) {

	unsigned int *start = &_bss_start;
	unsigned int *end = &_bss_end;
	while(start < end) {
		*start++ = 0x00;
	}
}

int main(unsigned long magic, unsigned long mboot) {


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

	//Advanced Programmable Interrupt Controller初期化
	apic.apic_init();

	//キーボード初期化
	keyboard.keyboard_init();

	//カーネル用スラブアロケータ初期化
	slab_allocator.slab_allocator_init();

	//ACPI管理初期化
	acpi.acpi_init();

	//PCIデバイス初期化
	pci.pci_init();

	//HPET初期化
	hpet.hpet_init();

	asmv("sti");

	//AP起動
	smp.smp_init();

	kprintf("return");


	STOP;
	return 0;

}


unsigned char ap_stack[16][0x4000];

extern "C" void ap_main() {


	virtual_memory.apply_kernel_page();


	//スタックポインタを正しく設定
	asmv("movq %0, %%rsp" : : "r"(&ap_stack[*reinterpret_cast<int*>(0xFFFF8000FEE00020) >> 24][0x2000]));

	//GDT初期化
	gdt.gdt_init();

	//IDT初期化
	idt.idt_init();



	kprintf("apic id %p\n", apic.get_local_apic_id());

	STOP;

}



