/*
 * apic.c
 *
 *  Created on: 2014/08/14
 *      Author: StarRing
 */

#include <apic.h>
#include <acpi.h>
#include <desc.h>

#define NUM_MAX_CPU 16

//APICの情報を記録
struct apic_info apic_info;

extern void apic_error_handler(void);
extern void apic_timer_handler(void);

//APIC初期化
void apic_init(void) {

	unsigned long apic = find_acpi_description_entry(ACPI_SIG('A', 'P', 'I', 'C'));
	int off, len;
	int num_processor;
	unsigned long ioapic_addr = 0;

	if(!apic) {
		kprintf("ERROR! can not found APIC Signature.\n");
		return;
	}

	kprintf("APIC = %p\n", apic);
	len = ACPI_R32(apic, 4);

	//オフセット計算
	len -= 44;
	off = 44;

	num_processor = 0;

	while(len > 0) {

		enum acpi_apic_type_code code = ACPI_R8(apic, off);
		int str_len = ACPI_R8(apic, off + 1);
		int id;

		switch(code) {
		case ACPI_PROCESSOR_LOCAL_APIC:

			id = ACPI_R8(apic, off + 3);
			if(id >= NUM_MAX_CPU) {
				kprintf("ERROR! too many cpus.\n");
				return;
			}
			num_processor++;
			break;
		case ACPI_IO_APIC:
			if(ioapic_addr != 0) {
				kprintf("ERROR! too many ioapic.\n");
				return;
			}
			ioapic_addr = ACPI_R32(apic, off + 4);
			apic_info.ioapic_id = ACPI_R8(apic, off + 2);
			break;
		default:
			//kprintf("default code = %u\n", code);
			break;
		}

		off += str_len;
		len -= str_len;
	}

	if(ioapic_addr == 0) {
		kprintf("ERROR! IO-APIC not found.\n");
		return;
	}

	apic_info.ioapic_addr = ioapic_addr;
	apic_info.num_processor = num_processor;

	//APICエラーハンドラ登録
	set_intr_gate(0x43, apic_error_handler);
	apic_write(APIC_LVT_ERROR_REGISTER, 66);
/*

	//APIC Timer設定
	set_intr_gate(0x20, apic_timer_handler);

	apic_write(APIC_DFR, 0xFFFFFFFF);
	apic_write(APIC_LDR, (apic_read(APIC_LDR) & 0x00FFFFFF) | 1);

	unsigned int tmp;

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
	unsigned int cpubusfreq = ((0xFFFFFFFF - apic_read(APIC_TIMER_CURRENT_COUNT)) + 1) * 16 * 100;
	tmp = cpubusfreq / 10 / 16;

	apic_write(APIC_DCR, APIC_TDIV_16);
	apic_write(APIC_TIMER_INITIAL_COUNT, tmp);
	apic_write(APIC_LVT_TIMER, 0x20 | TIMER_PERIODIC | APIC_DISABLED);
*/

}

void enable_apic_timer(void) {

	//apic_write(APIC_LVT_TIMER, apic_read(APIC_LVT_TIMER) & ~APIC_DISABLED);
}

void do_apic_timer_handler(void) {

	apic_eoi();

	kprintf("%s id = %d\n", __FUNCTION__, apic_read(APIC_ID_R) >> 24);
}

//APICエラーハンドラ本体
void do_apic_error_handler(void) {

	trace();

}

