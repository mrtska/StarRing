/*
[Module serial.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/12/23
*/

#include <system.h>
#include <drivers/serial.h>

static volatile int is_enable;

void serial_init(void) {

	unsigned char test = inb(0x3F8 + 5);
	if(test == 0xFF) {
		trace();
		is_enable = false;
		return;
	}

	outb(0x3F8 + 1, 0x00);
	outb(0x3F8 + 3, 0x80);
	outb(0x3F8 + 0, 0x03);
	outb(0x3F8 + 1, 0x00);
	outb(0x3F8 + 3, 0x03);
	outb(0x3F8 + 2, 0xC7);
	outb(0x3F8 + 4, 0x0B);

	is_enable = 1;

}



static void write_serial(char c) {

	while(((inb(0x3F8 + 5) & 0x20) == 0));


	outb(0x3F8, c);
}


void writes_serial(char *c) {

	if(!is_enable) {

		return;
	}
	while(*c) {

		write_serial(*c++);
	}
}

void writes_serial_count(char *c, unsigned long size) {

	if(!is_enable) {

		return;
	}
	int i;
	for(i = 0; i < size; i++) {

		write_serial(*c++);
	}
}
