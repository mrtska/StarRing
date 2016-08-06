
#pragma once






#define LOCAL_APIC_BASE_ADDRESS 0xFEE00000UL






struct local_apic_register {


	unsigned long reserved0[4];

	//RW
	unsigned long local_apic_id[2];

	//RO
	unsigned long local_apic_version[2];

	unsigned long reserved1[8];

	//RW
	unsigned long task_priority[2];

	//RO
	unsigned long arbitration_priority[2];

	//RO
	unsigned long processor_priority[2];

	//WO
	unsigned long eoi[2];

	//RO
	unsigned long remote_read[2];

	//RW
	unsigned long logical_destination[2];

	//RW
	unsigned long destination_format[2];

	//RW
	unsigned long spurious_interrupt_vecter[2];

	//RO
	unsigned long in_service[16];

	//RO
	unsigned long trigger_mode[16];

	//RO
	unsigned long interrupt_request[16];

	//RO
	unsigned long error_status[2];

	unsigned long reserved2[12];

	//RW
	unsigned long lvt_cmci[2];

	//RW
	unsigned long interrupt_command[4];

	//RW
	unsigned long lvt_timer[2];

	//RW
	unsigned long lvt_thermal_sensor[2];

	//RW
	unsigned long lvt_performance_monitoring_counters[2];

	//RW
	unsigned long lvt_lint0[2];

	//RW
	unsigned long lvt_lint1[2];

	//RW
	unsigned long lvt_error[2];

	//RW
	unsigned long initial_count[2];

	//RO
	unsigned long current_count[2];

	unsigned long reserved3[8];

	//RW
	unsigned long divide_configuration[2];

	unsigned long reserved4[2];

};


extern class apic apic;


class apic final {



public:

	struct local_apic_register *local_apic_register;


	void apic_init();

};










