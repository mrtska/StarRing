
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

	ctors  = .;
	.ctors ALIGN(0x1000) : AT(ADDR(.ctors) - VIRT_BASE) {
		*(.ctors)
	}
	ctors_end = .;
	. = ALIGN(0x1000);

	.text ALIGN(0x1000) : AT(ADDR(.text) - VIRT_BASE) {
		*(.text)
		*(.gnu.linkonce.t*)
	}

    . = ALIGN(0x1000);
    global_descriptor_table = .;
    . += 0x1000;
	. = ALIGN(0x1000);
	interrupt_descriptor_table = .;
	. += 0x1000;

	.data ALIGN(0x1000) : AT(ADDR(.data) - VIRT_BASE) {
		*(.data)
		*(.gnu.linkonce.d*)
	}

	. = ALIGN(0x1000);

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

		ap_stack = .;
		. += 0x20000;

	}


	kernel_end = .;

	end = .;

	/DISCARD/ : {
		*(.comment)
		*(.eh_frame)
	}

}

