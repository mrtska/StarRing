
ENTRY(start)

VIRT_BASE = 0xFFFFFFFF80000000;

SECTIONS {

    . = 0x100000;

    .boot : {

        *(.boot)
		. = ALIGN(0x1000);
    }

    . += VIRT_BASE;

	.text ALIGN(0x1000) : AT(ADDR(.text) - VIRT_BASE) {
        
		*(.text)
	}

    .data ALIGN(0x1000) : AT(ADDR(.data) - VIRT_BASE) {

		*(.data)
	}

	.eh_frame ALIGN(0x1000) : AT(ADDR(.eh_frame) - VIRT_BASE) {

        *(.eh_frame)
	}

    .bss ALIGN(0x1000) : AT(ADDR(.bss) - VIRT_BASE) {
		
        _bss_start = .;
		*(.bss)
		_bss_end = .;
	}

    /DISCARD/ : {
		*(.comment)
        *(.note.GNU-stack)
	}

}