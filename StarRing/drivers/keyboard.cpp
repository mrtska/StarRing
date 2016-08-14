

#include <drivers/keyboard.h>

#include <idt.h>
#include <stdio.h>
#include <apic.h>
#include <system.h>

class keyboard keyboard;

extern "C" void keyboard_handler(void);






void keyboard::keyboard_init() {


	//IDTに割り込みハンドラを、IOAPICに割り込みベクタを登録
	idt.register_interrupt(0x21, keyboard_handler);
	apic.ioapic_set_redirect_table(0x1, 0x21 | IOAPIC_LEVEL_TRIGGER);


	//タイプマティックレートを最大に
	write_command_encoder(KEYBOARD_ENC_SET_TYPEMATIC);
	write_command_encoder(0);

	write_command_encoder(KEYBOARD_ENC_SETLED);
	write_command_encoder(0x2);


}

extern "C" void exec_keyboard_handler() {


	kprintf("k ");

	inb(KEYBOARD_PORT_ENCODER);



	apic.apic_eoi();
}






