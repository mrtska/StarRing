/*
 * keyboard.c
 *
 *  Created on: 2014/05/01
 *      Author: StarRing
 */

#include <internal/stdio.h>
#include <desc.h>
#include <system.h>
#include <apic.h>
#include <smp.h>
#include <drivers/keyboard.h>
#include "../acpica/include/acpi.h"

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



//キーボードステータス NumLockなど
static struct key_state state;


//キーボード割り込みハンドラ
extern void keyboard_handler(void);




//コマンドをキーボードコントローラに書きこむ
static void write_command_to_controller(unsigned char command) {

	while(true) {

		if((read_status() & KEYBOARD_STATUS_IBF) == 0) {

			outb(KEYBOARD_PORT_CONTROLLER, command);
			return;
		}
	}
}
//キーボードエンコーダにコマンドを書き込む
static void write_command_to_encoder(unsigned char command) {

	while(true) {

		if((read_status() & KEYBOARD_STATUS_IBF) == 0) {

			outb(KEYBOARD_PORT_ENCODER, command);
			return;
		}
	}
}

static void set_LED(void) {

	unsigned char write_data = 0;

	if(state.caps_lock) {

		write_data |= 0x4;
	}
	if(state.num_lock) {

		write_data |= 0x2;
	}
	if(state.scr_lock) {

		write_data |= 0x1;
	}

	write_command_to_encoder(KEYBOARD_ENC_SETLED);
	write_command_to_encoder(write_data);

}

static void set_typematic_rate(void) {

	write_command_to_encoder(KEYBOARD_ENC_SET_TYPEMATIC);
	write_command_to_encoder(0);
}

//キーボード初期化 LEDなど
void keyboard_init(void) {

	set_intr_gate(0x21, keyboard_handler);
	io_apic_set_redirect(1,0, IOAPIC_LEVEL_TRIGGER | 0x21);

	memset(&state, 0x00, sizeof(struct key_state));
	state.num_lock = true;

	set_LED();
	set_typematic_rate();

	write_command_to_controller(KEYBOARD_CTRL_COM_TEST_MOUSE);
}




//キーボード割り込みハンドラ本体
void do_keyboard_handler(unsigned long* addr) {

	static bool is_E0 = false;
	unsigned int scan_code = inb(KEYBOARD_PORT_ENCODER);

	//コマンド正常終了時
	if(scan_code == 0xFA) {

		goto end;
	}

	if(scan_code == 0xE0) {

		is_E0 = true;
		goto end;
	}

	if(is_E0) {

		scan_code |= 0xE000;
		is_E0 = false;
	}


	//ブレイクコード
	if(scan_code & 0x80) {

		scan_code &= 0xE07F;

		switch(scan_code) {
		case L_SHIFT:
			state.L_shift = 0;
			break;
		case R_SHIFT:
			state.R_shift = 0;
			break;
		case L_CTRL:
			state.L_ctrl = 0;
			break;
		case R_CTRL:
			state.R_ctrl = 0;
			break;
		case L_ALT:
			state.L_alt = 0;
			break;
		case R_ALT:
			state.R_alt = 0;
			break;
		}
	} else {

		switch(scan_code) {
		case L_SHIFT:
			state.L_shift = 1;
			break;
		case R_SHIFT:
			state.R_shift = 1;
			break;
		case L_CTRL:
			state.L_ctrl = 1;
			break;
		case R_CTRL:
			state.R_ctrl = 1;
			break;
		case L_ALT:
			state.L_alt = 1;
			break;
		case R_ALT:
			state.R_alt = 1;
			break;
		}

		if(scan_code == 1) {


			asmv("cli");
			if(state.L_shift || state.R_shift) {

				ACPI_STATUS r;
				int n = 5;
				r = AcpiEnterSleepStatePrep(n);
				if (ACPI_FAILURE(r)) {
					kkprintf("sleep prep: %s\n", AcpiFormatException(r));
				}

				r = AcpiEnterSleepState(n);
				if (ACPI_FAILURE(r)) {
					kkprintf("sleep: %s\n", AcpiFormatException(r));
				}
				goto end;
			}

			AcpiReset();
		}
		if(state.L_shift || state.R_shift) {

			//kkprintf("%c %X", key_US_shift[scan_code], scan_code);
		} else {

			//kkprintf("%c", key_US[scan_code]);
		}

		//kprintf(" scan_code 0x%X\n", scan_code);
		write_fs(smp_table_get_input_node(get_apic_id()), 0, 0, (void*) &scan_code);

	}
	end:
	//割り込み終了
	apic_eoi();
}

