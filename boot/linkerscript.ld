

BASE      = 0x100000;
VIRT_BASE = 0xFFFFFFFF80000000;


SECTIONS {

	. = 0x100000;

	.boot : {

		*(.boot)
	}

	. = VIRT_BASE; 

	.text ALIGN(0x1000) : AT(ADDR(.text) - VIRT_BASE) {
        
		*(.text)
		. = ALIGN(0x1000);
		. += 0x1000;
		PROVIDE(global_descriptor_table_address = .);
		. += 0x1000;
		interrupt_descriptor_table = .;
		. += 0x1000;
		bsp_kernel_stack = .;
	}

    .data ALIGN(0x1000) : AT(ADDR(.data) - VIRT_BASE) {

		*(.data)
	}

	.rodata ALIGN(0x1000) : AT(ADDR(.rodata) - VIRT_BASE) {

		*(.rodata)
	}

	.eh_frame ALIGN(0x1000) : AT(ADDR(.eh_frame) - VIRT_BASE) {

        *(.eh_frame)
	}

    .bss ALIGN(0x1000) : AT(ADDR(.bss) - VIRT_BASE) {
		
        bss_start = .;
		*(.bss)
		bss_end = .;
	}

    /DISCARD/ : {
		*(.symtab)
		*(.strtab)
		*(.comment)
        *(.note.GNU-stack)
	}

}
ENTRY(start)

