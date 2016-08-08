/*
 * trap.cpp
 *
 *  Created on: 2016/08/08
 *      Author: StarRing
 */

#include <system.h>
#include <idt.h>



extern "C" void divide_error(void);
extern "C" void debug(void);
extern "C" void nmi(void);
extern "C" void breakpoint(void);
extern "C" void overflow(void);
extern "C" void bound_range(void);
extern "C" void invalid_opcode(void);
extern "C" void device_not_available(void);
extern "C" void double_fault(void);
extern "C" void coprocessor_segment_overrun(void);
extern "C" void invalid_tss(void);
extern "C" void segment_not_present(void);
extern "C" void stack_segment_fault(void);
extern "C" void general_protection(void);
extern "C" void page_fault(void);




void trap_init() {


	idt.register_interrupt(0, divide_error);
	idt.register_interrupt(1, debug);
	idt.register_interrupt(2, nmi);
	idt.register_interrupt(3, breakpoint);
	idt.register_interrupt(4, overflow);
	idt.register_interrupt(5, bound_range);
	idt.register_interrupt(6, invalid_opcode);
	idt.register_interrupt(7, device_not_available);
	idt.register_interrupt(8, double_fault);
	idt.register_interrupt(9, coprocessor_segment_overrun);
	idt.register_interrupt(10, invalid_tss);
	idt.register_interrupt(11, segment_not_present);
	idt.register_interrupt(12, stack_segment_fault);
	idt.register_interrupt(13, general_protection);
	idt.register_interrupt(14, page_fault);







}




extern "C" void exec_divide_error() {

	trace();
}



extern "C" void exec_debug() {

	trace();
}

extern "C" void exec_nmi() {

	trace();
}

extern "C" void exec_breakpoint() {

	trace();
}

extern "C" void exec_overflow() {

	trace();
}

extern "C" void exec_bound_range() {

	trace();
}

extern "C" void exec_invalid_opcode() {

	trace();
}

extern "C" void exec_device_not_available() {

	trace();
}

extern "C" void exec_double_fault() {

	trace();
}

extern "C" void exec_coprocessor_segment_overrun() {

	trace();
}

extern "C" void exec_invalid_tss() {

	trace();
}

extern "C" void exec_segment_not_present() {

	trace();
}

extern "C" void exec_stack_segment_fault() {

	trace();
}

extern "C" void exec_general_protection() {

	trace();
}

extern "C" void exec_page_fault() {

	trace();
}




















