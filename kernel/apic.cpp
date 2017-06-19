

#include <system.h>
#include <stdio.h>
#include <apic.h>
#include <physicalmemory.h>

class apic apic;





//Local APICを初期化する
void apic::apic_init() {


	//APICがある物理アドレスを使用中にする
	physical_memory.set_physical_memory_bit(LOCAL_APIC_BASE_ADDRESS, 0x200000);

	this->local_apic_register = reinterpret_cast<struct local_apic_register*>(read_physical_address(LOCAL_APIC_BASE_ADDRESS));



	this->local_apic_register->spurious_interrupt_vecter |= 0x100;


}

void apic::ioapic_write(unsigned int selecter, unsigned long value) {


	unsigned long index = 0xFEC00000;
	unsigned long data = 0xFEC00010;

	unsigned int value_low = value & 0xFFFFFFFF;
	unsigned int value_high = (value >> 32) & 0xFFFFFFFF;

	write_physical_address(index, selecter);
	write_physical_address(data, value_low);
	write_physical_address(index, selecter + 1);
	write_physical_address(data, value_high);



}
void apic::ioapic_set_redirect_table(int irq, unsigned long data) {



	unsigned int selecter = (irq * 2) + 0x10;


	ioapic_write(selecter, data);





}


