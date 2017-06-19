
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

    . = ALIGN(0x1000);
	ctors  = .;
	.ctors ALIGN(0x1000) : AT(ADDR(.ctors) - VIRT_BASE) {
		*(.ctors*)
		*(.init_array)
	}
    . = ALIGN(0x1000);
	ctors_end = .;

	.text ALIGN(0x1000) : AT(ADDR(.text) - VIRT_BASE) {
		*(.text)
		*(.text.*)
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
		*(.data.*)
		*(.gnu.linkonce.d*)
	}

	. = ALIGN(0x1000);

	.rodata ALIGN(0x1000) : AT(ADDR(.rodata) - VIRT_BASE) {
		*(.rodata)
		*(.rodata.*)
		*(.gnu.linkonce.r*)
	}

    .eh_frame_hdr ALIGN(0x1000) : AT(ADDR(.eh_frame) - VIRT_BASE) {

        *(.eh_frame_hdr)
        *(.eh_frame_hdr) *(.eh_frame_entry .eh_frame_entry.*)

    }
	.eh_frame ALIGN(0x1000) : AT(ADDR(.eh_frame) - VIRT_BASE) {

        *(.eh_frame)
        *(.eh_frame.*)

	}

	_loadEnd = . - VIRT_BASE;

	. = ALIGN(0x1000);
	.bss ALIGN(0x1000) : AT(ADDR(.bss) - VIRT_BASE) {
		_bss_start = .;
		*(.bss)
		*(.bss.*)
		. = ALIGN(0x1000);
		_bss_end = .;
		*(COMMON)


	}

	. = ALIGN(0x1000);

    . = 0xFFFFFFFFA0000000;
    . += 0x20000000;

	kernel_end = .;

	end = .;

	/DISCARD/ : {
		*(.comment)
	}

}

