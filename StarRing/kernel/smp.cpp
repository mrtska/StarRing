

#include <system.h>
#include <smp.h>
#include <string.h>
#include <wait.h>
#include <apic.h>

extern unsigned char ap_boot_start;
extern unsigned char ap_boot_end;


class smp smp;



void smp::smp_init() {





	unsigned char *code_ptr = reinterpret_cast<unsigned char*>(translate_physical_address(0x40000));


	long ap_boot_size = reinterpret_cast<long>(&ap_boot_end - &ap_boot_start);

	//ブートコードをコピー
	memcpy(static_cast<void*>(code_ptr), static_cast<const void*>(translate_physical_address(reinterpret_cast<unsigned long>(&ap_boot_start))), ap_boot_size);


	apic.local_apic_register->interrupt_command0 = 0x000C4500;


	waiter.wait_msec(10);


	apic.local_apic_register->interrupt_command0 = 0x000C4640;


	waiter.wait_msec(1);

	apic.local_apic_register->interrupt_command0 = 0x000C4640;







}


































