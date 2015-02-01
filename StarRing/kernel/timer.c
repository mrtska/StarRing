/*
 * timer.c
 *
 *  Created on: 2014/04/06
 *      Author: StarRing
 */

#include <timer.h>
#include <internal/stdio.h>
#include <desc.h>
#include <system.h>
#include <apic.h>

extern void timer_handler(void);
void timer_init(unsigned int quantum) {

	/*

	outb(0x61, (inb(0x61) & 0xFD) | 1);
	outb(0x43, 0xB2);
	outb(0x42, 0x9B);
	inb(0x60);
	outb(0x42, 0x2E);

	tmp = (unsigned int) inb(0x61) & 0xFE;
	outb(0x61, (unsigned int) tmp);
	outb(0x61, (unsigned int) tmp | 1);

	apic_write(APIC_TIMER_INITIAL_COUNT, 0xFFFFFFFF);

	while(!(inb(0x61) & 0x20)) {
		kprintf(".");
	}

	apic_write(APIC_LVT_TIMER, APIC_DISABLED);

	cpu_bus_freq = ((0xFFFFFFFF - (unsigned int) (apic_read(APIC_TIMER_CURRENT_COUNT))) + 1) * 16 * 100;
	tmp = cpu_bus_freq / quantum / 16;


	apic_write(APIC_TIMER_INITIAL_COUNT, tmp < 16 ? 16 : tmp);

	apic_write(APIC_LVT_TIMER, 0x22 | TIMER_PERIODIC);
	apic_write(APIC_DCR, APIC_TDIV_16);
*/

	trace();
}

void do_timer_handler(unsigned long* addr) {

	static unsigned long count;

	kprintf("do_timer_handler %u\n", count++);

	apic_write(APIC_EOI, 0);

}

