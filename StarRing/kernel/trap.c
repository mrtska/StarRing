/*
 * trap.c
 *
 *  Created on: 2014/04/06
 *      Author: StarRing
 */

#include <system.h>
#include <internal/stdio.h>
#include <trap.h>
#include <desc.h>
#include <page/page_types.h>
#include <apic.h>
#include <spinlock.h>
#include <fs/input.h>

static const char *exception_messages[32] = {
	"Division by zero",				/* 0 */
	"Debug",
	"Non-maskable interrupt",
	"Breakpoint",
	"Detected overflow",
	"Out-of-bounds",				/* 5 */
	"Invalid opcode",
	"No coprocessor",
	"Double fault",
	"Coprocessor segment overrun",
	"Bad TSS",						/* 10 */
	"Segment not present",
	"Stack fault",
	"General protection fault",
	"Page fault",
	"Unknown interrupt",			/* 15 */
	"Coprocessor fault",
	"Alignment check",
	"Machine check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

#include <regs.h>


//0除算エラー割り込み
void do_divide_error(struct regs *r) {


	kkprintf("Divide error!! RIP = %p, id = %d Ignored\n", r->i.rip, apic_read(APIC_ID_R) >> 24);

}

void do_invalid_opcode(struct regs *r) {


	kkprintf("Invalid Opcode!! RIP %p, id %d\n", r->i.rip, apic_read(APIC_ID_R) >> 24);
	STOP;
}

void do_debug_single_step(struct regs *r) {



	kprintf("single step next rip %p, id %d\n", r->i.rip, get_apic_id());
	kprintf("rax %p, rbx %p, rcx %p\n", r->c.rax, r->c.rbx, r->c.rcx);
	kprintf("rdx %p, r8  %p, r9  %p\n", r->c.rdx, r->c.r8, r->c.r9);
	kprintf("r10 %p, r11 %p, r12 %p\n", r->c.r10, r->c.r11, r->c.r12);
	kprintf("r13 %p, r14 %p, r15 %p\n", r->c.r13, r->c.r14, r->c.r15);
	kprintf("rsi %p, rdi %p, rsp %p\n", r->c.rsi, r->c.rdi, r->i.rsp);
	kprintf("rbp %p, rflags %p\n", r->c.rbp, r->i.rflags);

	wait_type_key(0x1C);


}


unsigned char except_lock;
//汎用例外ハンドラ本体
void do_except(unsigned long code, struct regs_with_error *r) {


	static int hoge = 0;

	spin_lock(&except_lock);
	kprintf("Error message %s, Error code %X, id = %d\n", exception_messages[code], r->reg1, apic_read(APIC_ID_R) >> 24);
	kprintf("RFLAGS %X, \n", r->i.rflags);
	kprintf("rip %p\n", r->i.rip);
	kprintf("rsi %p, rdi %p, rsp %p\n", r->c.rsi, r->c.rdi, r->i.rsp);
	kprintf("rax %p, rbx %p, rcx %p\n", r->c.rax, r->c.rbx, r->c.rcx);
	kprintf("rdx %p, r8  %p, r9  %p\n", r->c.rdx, r->c.r8, r->c.r9);
	kprintf("r10 %p, r11 %p, r12 %p\n", r->c.r10, r->c.r11, r->c.r12);
	kprintf("r13 %p, r14 %p, r15 %p\n", r->c.r13, r->c.r14, r->c.r15);
	kprintf("rbp %p cs %X, ss %X\n", r->c.rbp, r->i.cs, r->i.ss);


	spin_unlock(&except_lock);
	STOP;
	while(++hoge == 1) asmv("sti;hlt");
	hoge++;
}





extern void divide_error(void);
extern void debug_single_step(void);
extern void except2(void);
extern void except3(void);
extern void except4(void);
extern void except5(void);
extern void invalid_opcode(void);
extern void except7(void);
extern void except8(void);
extern void except9(void);
extern void except10(void);
extern void except11(void);
extern void except12(void);
extern void except13(void);
extern void page_fault(void);
extern void except15(void);
extern void except16(void);
extern void except17(void);
extern void except18(void);
extern void except19(void);
extern void except20(void);
extern void except21(void);
extern void except22(void);
extern void except23(void);
extern void except24(void);
extern void except25(void);
extern void except26(void);
extern void except27(void);
extern void except28(void);
extern void except29(void);
extern void except30(void);
extern void except31(void);



void trap_init(void) {



	set_intr_trap(0, &divide_error, 0);
	set_intr_trap(1, &debug_single_step, 0);
	set_intr_trap(2, &except2, 0);
	set_intr_trap(3, &except3, 0);
	set_intr_trap(4, &except4, 0);
	set_intr_trap(5, &except5, 0);
	set_intr_trap(6, &invalid_opcode, 0);
	set_intr_trap(7, &except7, 0);
	set_intr_trap(8, &except8, 0);
	set_intr_trap(9, &except9, 0);
	set_intr_trap(10, &except10, 0);
	set_intr_trap(11, &except11, 0);
	set_intr_trap(12, &except12, 0);
	set_intr_trap(13, &except13, 0);
	set_intr_trap(14, &page_fault, 0);
	set_intr_trap(15, &except15, 0);
	set_intr_trap(16, &except16, 0);
	set_intr_trap(17, &except17, 0);
	set_intr_trap(18, &except18, 0);
	set_intr_trap(19, &except19, 0);
	set_intr_trap(20, &except20, 0);
	set_intr_trap(21, &except21, 0);
	set_intr_trap(22, &except22, 0);
	set_intr_trap(23, &except23, 0);
	set_intr_trap(24, &except24, 0);
	set_intr_trap(25, &except25, 0);
	set_intr_trap(26, &except26, 0);
	set_intr_trap(27, &except27, 0);
	set_intr_trap(28, &except28, 0);
	set_intr_trap(29, &except29, 0);
	set_intr_trap(30, &except30, 0);
	set_intr_trap(31, &except31, 0);

}

