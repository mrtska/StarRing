/*
[Module smp.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/08/27
*/

#include <smp.h>
#include <apic.h>
#include <wait.h>
#include <mm.h>
#include <page/page.h>


extern unsigned char ap_boot_start;
extern unsigned char ap_boot_end;



struct smp_table_entry smp_table[16];
unsigned char ap_stack[16][0x4000];


//AP起動
void smp_init(void) {


	//ブートコードの場所
	unsigned char *code_ptr = (unsigned char*) PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(0x40000);

	unsigned int ap_boot_size = (unsigned int) (&ap_boot_end - &ap_boot_start);



	//ブートコードをコピー
	memcpy((void*) code_ptr, (const void*) PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(&ap_boot_start), ap_boot_size);


	//APIC INIT IPI
	apic_write(APIC_ICR_LOW, 0x000C4500);

	wait_msec(10);

	//Start Up IPI
	apic_write(APIC_ICR_LOW, 0x000C4640);

	wait_usec(200);

	apic_write(APIC_ICR_LOW, 0x000C4640);

	wait_usec(200);


	//BSP
	smp_table[apic_read(APIC_ID_R) >> 24].flags |= SMP_PROCESSOR_BSP | SMP_PROCESSOR_EXISTED | SMP_PROCESSOR_ENABLED | SMP_PROCESSOR_INTERRUPT_RECEIVABLE;
}


void smp_ap_init(void) {

	int apic_id = apic_read(APIC_ID_R) >> 24;


	smp_table[apic_id].flags = SMP_PROCESSOR_EXISTED | SMP_PROCESSOR_ENABLED;
}


























