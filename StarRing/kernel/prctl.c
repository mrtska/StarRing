/*
[Module prctl.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
*/

#include <prctl.h>
#include <system.h>
#include <gdt.h>
#include <apic.h>



long arch_prctl(int code, unsigned long addr) {


	int apic_id = get_apic_id();

	switch(code) {

	case ARCH_SET_FS: {


		struct desc_struct *desc = &get_gdt(apic_id)->gdt[GDT_ENTRY_TLS_MIN];

		desc->base0 = addr & 0xFFFF;
		desc->base1 = (addr & 0x00FF0000) >> 16;
		desc->base2 = (addr & 0xFF000000) >> 24;
		desc->s = 1;
		desc->dpl = 0x3;
		desc->p = 1;
		desc->limit = 0xF;
		desc->limit0 = 0xFFFF;
		desc->d = 1;
		desc->g = 1;
		desc->avl = 1;
		desc->l = 0;
		desc->type = 3;
		asmv("mov %0, %%ax" : : "i"(GDT_ENTRY_FS_TLS_SEL));
		asmv("mov %ax, %fs");
		break;
	}
	}



	return 0;
}
