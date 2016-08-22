

#include <system.h>
#include <drivers/hpet.h>
#include <idt.h>
#include <registers.h>
#include <apic.h>


//ACPICAのヘッダ
extern "C" {

#include "../acpica/include/acpi.h"

}

INTERRUPT_HANDLER void hpet_handler();


class hpet hpet;



void hpet::hpet_init() {

/*

	ACPI_TABLE_HEADER *header;

	AcpiGetTable(const_cast<char*>(ACPI_SIG_HPET), 0, &header);

	ACPI_TABLE_HPET *hpet = reinterpret_cast<ACPI_TABLE_HPET*>(header);
*/


	//this->hpet_base_address = 0xFED00000;//hpet->Address.Address;

	//429B17F
	unsigned int hpet_period = read_hpet(HPET_GCAP) >> 32;
	kprintf("period %p\n", this->hpet_base_address);
	hpet_freq_khz = 1000000000 / (hpet_period / 1000);


	unsigned int cnf = HPET_CONF_INT_ENB_CNF | HPET_CONF_TYPE_CNF;
	write_hpet(HPET_TIMN_CONF(0), cnf);

	write_hpet(HPET_TIMN_COMPARATOR(0), hpet_msec_to_tick(1000));
	write_hpet(HPET_GEN_CONF, 2);


	idt.register_interrupt(0x22, hpet_handler);
	apic.ioapic_set_redirect_table(0x2, 0x22);

	write_hpet(HPET_GEN_CONF, 3);

}


INTERRUPT_HANDLER void exec_hpet_handler(struct registers *regs) {

	trace();
	apic.apic_eoi();
}

