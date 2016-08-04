
#include <system.h>
#include <stdio.h>
#include <idt.h>

#include <physicalmemory.h>

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


	kprintf("Hello,Eclipse.\n");

	disable_legacy_pic();

	//IDT初期化
	idt.idt_init();

	//物理メモリ管理初期化
	physical_memory.physical_memory_init(mboot);





	kprintf("return\n");
	return;

}





