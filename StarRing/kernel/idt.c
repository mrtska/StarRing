/*
[Module idt.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/04/04
*/


#include <desc.h>
#include <internal/stdio.h>


gate_desc idt_table[256] __attribute__((__section__(".bss..aligned"), __aligned__(0x1000)));
struct desc_ptr idt_descr = { 256 * 16 - 1, (unsigned long) &idt_table};


//IDT初期化
void idt_init(void) {

	lidt((const struct desc_ptr*)&idt_descr);

}









