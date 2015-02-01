/*
 * ioapic.h
 *
 *  Created on: 2014/04/05
 *      Author: StarRing
 */

#pragma once

#include <msr.h>
#include <internal/stdio.h>
#include <apic.h>
#include <mm.h>

#define IOAPIC_ID_OFFSET				0x00
#define IOAPIC_VER_OFFSET				0x01
#define IOAPIC_ARB_OFFSET				0x02
#define IOAPIC_REDIRECTION_TABLE_OFFSET	0x10


#define IOAPIC_DESTINATION_ID(n) (((unsigned long)(n))<<56ULL)
#define IOAPIC_EDID(n) (((unsigned long)(n))<<48ULL) /* ?? */

#define IOAPIC_DESTINATION_ID32(n) (((unsigned int)(n))<<(56-32)) /* hi */
#define IOAPIC_EDID32(n) (((unsigned int)(n))<<(48-32)) /* hi */

#define IOAPIC_MASK (1<<16)
#define IOAPIC_LEVEL_TRIGGER (1<<15)
#define IOAPIC_REMOTE_IRR (1<<14)
#define IOAPIC_INTERRUPT_PIN_LO (1<<13)
#define IOAPIC_DELIVERY_PENDING (1<<12) /* ro */
#define IOAPIC_DESITINATION_LOGICAL (1<<11)
#define IOAPIC_DELIVERY_FIXED ((0x00)<<8) /* 000 */
#define IOAPIC_DELIVERY_LOWEST_PRIORITY ((0x01)<<8) /* 001 */
#define IOAPIC_DELIVERY_EXTINT ((0x07)<<8) /* 111 */
#define IOAPIC_VECTOR(n) (n)


static __inline__ unsigned int read_io_apic(unsigned long apic, unsigned int reg) {

	*((unsigned int*) apic) = reg;
	return *((unsigned int*) (apic + 0x10));
}

static __inline__ void write_io_apic(unsigned long apic, unsigned int reg, unsigned int value) {

	*((unsigned long*) apic) = reg;
	*((unsigned long*) (apic + 0x10)) = value;
}

//IOAPICの初期化
void io_apic_init(unsigned int io_apic_addr);

void io_apic_enable(int irq);

void io_apic_set_redirect(int irq, unsigned int entry_high, unsigned int entry_low);
void io_apic_set_cpu(int irq, int cpu);
