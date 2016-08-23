

#include <system.h>
#include <drivers/hpet.h>
#include <idt.h>
#include <registers.h>
#include <apic.h>
#include <wait.h>
#include <slaballocator.h>

//ACPICAのヘッダ
extern "C" {

#include "../acpica/include/acpi.h"

}

INTERRUPT_HANDLER void hpet_handler();


class hpet hpet;



void hpet::hpet_init() {


	ACPI_TABLE_HEADER *header;

	AcpiGetTable(const_cast<char*>(ACPI_SIG_HPET), 0, &header);

	ACPI_TABLE_HPET *hpet = reinterpret_cast<ACPI_TABLE_HPET*>(header);


	this->hpet_base_address = hpet->Address.Address;


	unsigned int gconf = read_hpet32(HPET_OFFSET_GCONFIG);

	kprintf("conf %p\n", gconf);

	//設定とかを初期化
	write_hpet32(HPET_OFFSET_GCONFIG, 0);

//	int count = (read_hpet32(HPET_OFFSET_ID) & 0x1F00) >> 8;

	//429B17F
	unsigned int hpet_period = read_hpet32(HPET_OFFSET_PERIOD);
	hpet_freq_khz = 1000000000 / (hpet_period / 1000);



	unsigned int cnf = HPET_TN_ENABLE;
	write_hpet64(HPET_OFFSET_TIMN_CONFIG(0), cnf);
	write_hpet64(HPET_OFFSET_TIMN_COMPARATOR(0), hpet_msec_to_tick(1));
	write_hpet64(HPET_OFFSET_GCONFIG, 2);


	idt.register_interrupt(0x22, hpet_handler);
	apic.ioapic_set_redirect_table(0x2, 0x22);

	restart_counter();

}

//1ミリ秒に1回呼ばれる感じ
INTERRUPT_HANDLER void exec_hpet_handler(struct registers *regs) {


	//wait_listがあったら減らす
	if(!waiter.get_wait_list()->is_empty()) {


		waiter.get_wait_list()->foreach_safe(offsetof(&wait_entry::list), [](struct wait_entry *entry) {


			if(entry->remaining == 0) {

				entry->list.remove();
				delete entry;
				return;
			}

			entry->remaining--;

		});
	}

	apic.apic_eoi();

	hpet.restart_counter();
}

