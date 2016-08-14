
#pragma once






#define LOCAL_APIC_BASE_ADDRESS 0xFEE00000UL


#define IOAPIC_LEVEL_TRIGGER (1 << 15)



struct local_apic_register {


	unsigned long reserved0[4];

	//RW
	unsigned int local_apic_id;

	unsigned int aligned0[3];

	//RO
	unsigned int local_apic_version;
	unsigned int aligned1[3];

	unsigned long reserved1[8];

	//RW
	unsigned int task_priority;
	unsigned int aligned2[3];

	//RO
	unsigned int arbitration_priority;
	unsigned int aligned3[3];

	//RO
	unsigned int processor_priority;
	unsigned int aligned4[3];

	//WO
	unsigned int eoi;
	unsigned int aligned5[3];

	//RO
	unsigned int remote_read;
	unsigned int aligned6[3];

	//RW
	unsigned int logical_destination;
	unsigned int aligned7[3];

	//RW
	unsigned int destination_format;
	unsigned int aligned8[3];

	//RW
	unsigned int spurious_interrupt_vecter;
	unsigned int aligned9[3];


	//RO
	unsigned int in_service0;
	unsigned int aligned10[3];

	unsigned int in_service1;
	unsigned int aligned11[3];

	unsigned int in_service2;
	unsigned int aligned12[3];

	unsigned int in_service3;
	unsigned int aligned13[3];

	unsigned int in_service4;
	unsigned int aligned14[3];

	unsigned int in_service5;
	unsigned int aligned15[3];

	unsigned int in_service6;
	unsigned int aligned16[3];

	unsigned int in_service7;
	unsigned int aligned17[3];



	//RO
	unsigned int trigger_mode0;
	unsigned int aligned18[3];

	unsigned int trigger_mode1;
	unsigned int aligned19[3];

	unsigned int trigger_mode2;
	unsigned int aligned20[3];

	unsigned int trigger_mode3;
	unsigned int aligned21[3];

	unsigned int trigger_mode4;
	unsigned int aligned22[3];

	unsigned int trigger_mode5;
	unsigned int aligned23[3];

	unsigned int trigger_mode6;
	unsigned int aligned24[3];

	unsigned int trigger_mode7;
	unsigned int aligned25[3];


	//RO
	unsigned int interrupt_request0;
	unsigned int aligned26[3];

	unsigned int interrupt_request1;
	unsigned int aligned27[3];

	unsigned int interrupt_request2;
	unsigned int aligned28[3];

	unsigned int interrupt_request3;
	unsigned int aligned29[3];

	unsigned int interrupt_request4;
	unsigned int aligned30[3];

	unsigned int interrupt_request5;
	unsigned int aligned31[3];

	unsigned int interrupt_request6;
	unsigned int aligned32[3];

	unsigned int interrupt_request7;
	unsigned int aligned33[3];



	//RO
	unsigned int error_status;
	unsigned int aligned34[3];

	unsigned long reserved2[12];

	//RW
	unsigned int lvt_cmci;
	unsigned int aligned35[3];

	//RW
	unsigned int interrupt_command0;
	unsigned int aligned36[3];

	unsigned int interrupt_command1;
	unsigned int aligned37[3];


	//RW
	unsigned int lvt_timer;
	unsigned int aligned38[3];


	//RW
	unsigned int lvt_thermal_sensor;
	unsigned int aligned39[3];


	//RW
	unsigned int lvt_performance_monitoring_counters;
	unsigned int aligned40[3];

	//RW
	unsigned int lvt_lint0;
	unsigned int aligned41[3];

	//RW
	unsigned int lvt_lint1;
	unsigned int aligned42[3];

	//RW
	unsigned int lvt_error;
	unsigned int aligned43[3];

	//RW
	unsigned int initial_count;
	unsigned int aligned44[3];

	//RO
	unsigned int current_count;
	unsigned int aligned45[3];


	unsigned long reserved3[8];

	//RW
	unsigned int divide_configuration;
	unsigned int aligned46[3];

	unsigned long reserved4[2];

} __attribute__((packed));







extern class apic apic;


class apic final {

private:



	void ioapic_write(unsigned int selecter, unsigned long value);


public:

	struct local_apic_register *local_apic_register;


	void apic_init();



	void ioapic_set_redirect_table(int irq, unsigned long data);

	void apic_eoi() {

		local_apic_register->eoi = 0;
	}



};










