

#include <apic.h>
#include <physicalmemory.h>

class apic apic;





//Local APICを初期化する
void apic::apic_init() {


	physical_memory.set_physical_memory_bit(LOCAL_APIC_BASE_ADDRESS, 0x200000);

	//this->local_apic_register = LOCAL_APIC_BASE_ADDRESS;







}




