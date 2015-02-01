/*
 * beep.c
 *
 *  Created on: 2014/10/20
 *      Author: StarRing
 */

#include <drivers/beep.h>
#include <system.h>
#include <wait.h>






void beep(int time) {

	unsigned int div = 1193;
	unsigned int tmp = inb(0x61);
	kprintf("0x61 = %b\n", tmp);

	outb(0x43, 0xb6);
	outb(0x42, (unsigned int) (div) );
	outb(0x42, (unsigned int) (div >> 8));
	tmp &= 0x0F;
	if (tmp != (tmp | 3)) {
		outb(0x61, tmp | 3);
	}
	wait_sec(time);

	outb(0x61, (inb(0x61) & 0xFC));

	trace();


}









































