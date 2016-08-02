
#include <idt.h>
#include <stdio.h>
#include <system.h>


//リンカスクリプトに書いた物を使う
extern void *interrupt_descripter_table;

//割り込みハンドラを管理するクラス
class idt idt(reinterpret_cast<void*>(&interrupt_descripter_table));

idt::idt(void *p) {
	
	idt_pointer = p;
	kprintf("idt#constructer %p\n", p);
}

void idt::load_idtr() {

	struct {

		unsigned short limit;
		unsigned long base;
	} __attribute__((packed)) idtr;


	idtr.limit = 0;
	idtr.base = reinterpret_cast<unsigned long>(&idt_pointer);

	kprintf("idtr %p\n", idtr.base);
	asmv("lidt %0" :: "m"(idtr));
}

void idt::idt_init() {
	idt idt(reinterpret_cast<void*>(&interrupt_descripter_table));
	load_idtr();

}

void idt::register_interrupt(int id, interrupt_descripter desc) {







}
