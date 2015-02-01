/*
 * ipi.c
 *
 *  Created on: 2014/12/09
 *      Author: StarRing
 *  プロセッサ間割り込み
 */

#include <apic.h>
#include <ioapic.h>
#include <desc.h>
#include <ipi.h>

extern void ipi_handler(void);



void ipi_init(void) {

	set_intr_gate(0x44, ipi_handler);
	io_apic_set_redirect(23, 0, 0x44);

}



void do_ipi_handler(void) {

	kprintf("%s id = %d\n", __func__, apic_read(APIC_ID_R) >> 24);
	apic_eoi();
}























