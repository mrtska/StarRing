
#include <idt.h>
#include <stdio.h>
#include <string.h>
#include <system.h>


//リンカスクリプトに書いた物を使う
extern void *interrupt_descriptor_table;

//リンカスクリプトに書いた物を使う
class idt idt(reinterpret_cast<void*>(&interrupt_descriptor_table));

idt::idt(void *p) {

	idt_pointer = static_cast<union interrupt_descriptor*>(p);
}

void idt::load_idtr() {

	struct {

		unsigned short limit;
		unsigned long base;
	} __attribute__((packed)) idtr;


	idtr.limit = 256 * 16 - 1;
	idtr.base = reinterpret_cast<unsigned long>(idt_pointer);

	asmv("lidt %0" :: "m"(idtr));
}

void idt::idt_init() {

	load_idtr();
}


//IDTに登録 構造体を作る
void idt::register_interrupt(int id, void (*func)()) {


	union interrupt_descriptor desc;


	desc.bitfield.p = 1;	//ある
	desc.offset0_15 = PTR_LOW(func);
	desc.offset16_31 = PTR_MIDDLE(func);
	desc.offset32_63 = PTR_HIGH(func);
	desc.bitfield.fixed = 0xE;	//割り込みゲート

	desc.bitfield.dpl = 0;
	desc.bitfield.ist = 0;
	desc.bitfield.selecter = 8;	//コードセグメント




	this->register_interrupt(id, &desc);
}

//IDTに登録
void idt::register_interrupt(int id, union interrupt_descriptor *desc) {

	memcpy(&this->idt_pointer[id], desc, sizeof(*desc));
}


void idt::print_interrupt(int id) {

	union interrupt_descriptor *desc = &this->idt_pointer[id];

	kprintf("p %p\n", desc->bitfield.offset0_15);


}























