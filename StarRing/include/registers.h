
#pragma once



struct intr_regs {

	unsigned long rip;
	unsigned long cs;
	unsigned long rflags;
	unsigned long rsp;
	unsigned long ss;
};

struct common_regs {

	unsigned long rbp;
	unsigned long r15;
	unsigned long r14;
	unsigned long r13;
	unsigned long r12;
	unsigned long r11;
	unsigned long r10;
	unsigned long r9;
	unsigned long r8;
	unsigned long rdi;
	unsigned long rsi;
	unsigned long rdx;
	unsigned long rcx;
	unsigned long rbx;
	unsigned long rax;
};


struct registers {

	struct common_regs c;
	struct intr_regs i;
};

struct registers_with_error {

	struct common_regs c;
	unsigned long error;
	struct intr_regs i;
};
