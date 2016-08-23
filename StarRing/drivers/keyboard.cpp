

#include <drivers/keyboard.h>

#include <idt.h>
#include <stdio.h>
#include <apic.h>
#include <system.h>
#include <registers.h>




//ACPICAのヘッダ
extern "C" {

#include "../acpica/include/acpi.h"

}





class keyboard keyboard;

extern "C" void keyboard_handler(void);

//USキーボード配列
unsigned char key_US[128] = {
		0,  27, '1', '2', '3', '4', '5', '6', '7', '8',    /* 9 */
		  '9', '0', '-', '=',
		  '\b',            /* Backspace */
		  '\t',            /* Tab */
		  'q', 'w', 'e', 'r',    /* 19 */
		  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',        /* Enter key */
		   0,            /* 29   - Control */
		  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',    /* 39 */
		  '\'', '`',   0,        /* Left shift */
		  '\\', 'z', 'x', 'c', 'v', 'b', 'n',            /* 49 */
		  'm', ',', '.', '/',   0,                    /* Right shift */
		  '*',
		   0,    /* Alt */
		  ' ',    /* Space bar */
		   0,    /* Caps lock */
		   0,    /* 59 - F1 key ... > */
		   0,   0,   0,   0,   0,   0,   0,   0,
		   0,    /* < ... F10 */
		   0,    /* 69 - Num lock*/
		   0,    /* Scroll Lock */
		   0,    /* Home key */
		   0,    /* Up Arrow */
		   0,    /* Page Up */
		  '-',
		   0,    /* Left Arrow */
		   0,
		   0,    /* Right Arrow */
		  '+',
		   0,    /* 79 - End key*/
		   0,    /* Down Arrow */
		   0,    /* Page Down */
		   0,    /* Insert Key */
		   0,    /* Delete Key */
		   0,   0,   0,
		   0,    /* F11 Key */
		   0,    /* F12 Key */
		   0,    /* All other keys are undefined */
};

//USキーボード配列 Shift押下時
unsigned char key_US_shift[128] =
    {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',    /* 9 */
   '(', ')', '_', '+',
   '\b',            /* Backspace */
   '\t',            /* Tab */
   'Q', 'W', 'E', 'R',    /* 19 */
   'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
   '\n',            /* Enter key */
    0,              /* 29   - Control */
   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',    /* 39 */
   '\'', '`',   0,        /* Left shift */
   '\\', 'Z', 'X', 'C', 'V', 'B', 'N',              /* 49 */
   'M', '<', '>', '?',   0,                         /* Right shift */
   '*',
    0,    /* Alt */
   ' ',   /* Space bar */
    0,    /* Caps lock */
    0,    /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,    /* < ... F10 */
    0,    /* 69 - Num lock*/
    0,    /* Scroll Lock */
    0,    /* Home key */
    0,    /* Up Arrow */
    0,    /* Page Up */
   '-',
    0,    /* Left Arrow */
    0,
    0,    /* Right Arrow */
   '+',
    0,    /* 79 - End key*/
    0,    /* Down Arrow */
    0,    /* Page Down */
    0,    /* Insert Key */
    0,    /* Delete Key */
    0,   0,   0,
    0,    /* F11 Key */
    0,    /* F12 Key */
    0,    /* All other keys are undefined */
    };




void keyboard::keyboard_init() {


	//IDTに割り込みハンドラを、IOAPICに割り込みベクタを登録
	idt.register_interrupt(0x21, keyboard_handler);
	apic.ioapic_set_redirect_table(0x1, 0x21 | IOAPIC_LEVEL_TRIGGER);


	//タイプマティックレートを最大に
	//write_command_encoder(KEYBOARD_ENC_SET_TYPEMATIC);
	//write_command_encoder(0);

	//write_command_encoder(KEYBOARD_ENC_SETLED);
	//write_command_encoder(0x2);


}

INTERRUPT_HANDLER void exec_keyboard_handler(struct registers *regs) {

	//kprintf("regs %p\n", regs->i.rip);


	auto code = inb(KEYBOARD_PORT_ENCODER);



	//コマンド成功時
	if(code == 0xFA) {


		apic.apic_eoi();
		return;
	}


	if(code & 0x80) {


	} else {

		if(code == 1) {



			ACPI_STATUS r;
			int n = 5;
			r = AcpiEnterSleepStatePrep(n);
			if (ACPI_FAILURE(r)) {
				kprintf("sleep prep: %s\n", AcpiFormatException(r));
			}

			r = AcpiEnterSleepState(n);
			if (ACPI_FAILURE(r)) {
				kprintf("sleep: %s\n", AcpiFormatException(r));
			}
		}


		if(code == 0x3F) {

			AcpiReset();
		}

		kprintf("%c", key_US[code]);
	}






	apic.apic_eoi();
}






