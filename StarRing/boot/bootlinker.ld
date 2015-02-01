/*
[Module bootlinker.ld]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php

  Created on: 2014/04/02
  Author: StarRing
*/
ENTRY(ENTRYPOINT)

VIRT_BASE = 0xFFFFFFFF80000000;

SECTIONS {



	. = 0x100000;

	.boot : {
		*(.multibootheader)
		_loadStart = .;
		*(.boot)
		. = ALIGN(0x1000);
		*(.page_table)
		. = ALIGN(0x1000);

		Stack = .;

	}


	. += VIRT_BASE;

	.text ALIGN(0x1000) : AT(ADDR(.text) - VIRT_BASE) {
		*(.text)
		*(.gnu.linkonce.t*)
	}

	.data ALIGN(0x1000) : AT(ADDR(.data) - VIRT_BASE) {
		*(.data)
		*(.gnu.linkonce.d*)
	}

	.rodata ALIGN(0x1000) : AT(ADDR(.rodata) - VIRT_BASE) {
		*(.rodata*)
		*(.gnu.linkonce.r*)
	}

	_loadEnd = . - VIRT_BASE;

	. = ALIGN(0x1000);
	.bss ALIGN(0x1000) : AT(ADDR(.bss) - VIRT_BASE) {
		_bss_start = .;
		*(.bss)
		. = ALIGN(0x1000);
		_bss_end = .;
		*(COMMON)

		. = ALIGN(0x10);

		stack_pointer = .;

		. += 0x100000;

		. = ALIGN(0x10);
		physical_memory_bitmap = .;
		. += 0x1000;


		ap_stack = .;
		. += 0x20000;
	}


	. = ALIGN(0x1000);

	kernel_end = .;

	end = .;

	/DISCARD/ : {
		*(.comment)
		*(.eh_frame)
	}

}









