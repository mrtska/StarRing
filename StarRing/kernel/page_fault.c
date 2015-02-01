/*
 * page_fault.c
 *
 *  Created on: 2014/04/30
 *      Author: StarRing
 */

#include <system.h>
#include <internal/stdio.h>
#include <regs.h>

#include <page/page.h>
#include <page/page_types.h>
#include <mem/phys.h>
#include <apic.h>
#include <task.h>
#include <smp.h>
#include <mem/vmm.h>
#include <spinlock.h>

static unsigned char page_lock;

//ページフォールト例外ハンドラ
void do_page_fault(unsigned long fault_addr, struct regs_with_error *r) {


	spin_lock(&page_lock);
	if(fault_addr == 0) {

		kprintf("Null Pointer Exception, Error Code = %b\n", r->reg1);
	} else {

		kprintf("Error message = Page Fault, Error Code = %b, apic id = %d\n", r->reg1, apic_read(APIC_ID_R) >> 24);
	}
	kprintf("fault address = 0x%p, rip = %p, id %d\n", (unsigned long) fault_addr, r->i.rip, apic_read(APIC_ID_R) >> 24);
	//kprintf("pml4e %X, pdpte %X, pde %X\n", pml4_index(fault_addr), pdpt_index(fault_addr), pd_index(fault_addr));

	unsigned int apic_id = apic_read(APIC_ID_R) >> 24;
	struct smp_table_entry *table_entry = &smp_table[apic_id];
	struct process *process = table_entry->execute_process;

	if(process != NULL) {

		kprintf("current process = %s, id = %d\n", process->name, apic_id);
	}
	kprintf("rsi %p, rdi %p, rsp %p\n", r->c.rsi, r->c.rdi, r->i.rsp);
	kprintf("rax %p, rbx %p, rcx %p\n", r->c.rax, r->c.rbx, r->c.rcx);
	kprintf("rdx %p, r8  %p, r9  %p\n", r->c.rdx, r->c.r8, r->c.r9);
	kprintf("r10 %p, r11 %p, r12 %p\n", r->c.r10, r->c.r11, r->c.r12);
	kprintf("r13 %p, r14 %p, r15 %p\n", r->c.r13, r->c.r14, r->c.r15);
	kprintf("rbp %p cs %X, ss %X\n", r->c.rbp, r->i.cs, r->i.ss);

	spin_unlock(&page_lock);
	STOP;

	if((r->i.rsp & 0xFFF000) == (fault_addr & 0xFFF000)) {

		map_page(fault_addr, (unsigned long) alloc_memory_block(1), process->page_tables, FLAGS_USER_PAGE | FLAGS_WRITABLE_PAGE | FLAGS_LARGE_PAGE);
		return;
	}

	STOP;

	static int count;
	if(++count == 1) {

		STOP;
	}
	count++;

	//unsigned long cr3 = (unsigned long) process->page_tables;

	//map_page(fault_addr, alloc_memory_block(1), cr3, FLAGS_USER_PAGE);


}
