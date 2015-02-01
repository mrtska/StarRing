/*
[Module syscall.S]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/04/06
*/


.text
.code64
.macro irq_entry sym do_sym
.global \sym
.extern \do_sym
.align 0x10
\sym:

	save_registers
	movq %rsp, %rdi
	call \do_sym

	rest_registers

	iretq
.endm

back:
.quad 0

.macro isr_entry sym do_sym num
.global \sym\num
.extern \do_sym
.align 0x10
\sym\num:

	save_registers
	movq $\num, %rdi
	movq %rsp, %rsi
	call \do_sym

	rest_registers

	//addq $8, %rsp

	iretq

.endm

.global save_regs
save_regs:
	xchg (%rsp), %rax
	push %rbx
	push %rcx
	push %rdx
	push %rsi
	push %rdi
	push %rbp
	push %r8
	push %r9
	jmp *%rax

.globl rest_regs
rest_regs:
	pop %rax
	xchg 0x40(%rsp), %rax
	pop %r9
	pop %r8
	pop %rbp
	pop %rdi
	pop %rsi
	pop %rdx
	pop %rcx
	pop %rbx
	ret

.macro save_registers

	push %rax
	push %rbx
	push %rcx
	push %rdx
	push %rsi
	push %rdi
	push %r8
	push %r9
	push %r10
	push %r11
	push %r12
	push %r13
	push %r14
	push %r15
	push %rbp

.endm

.macro rest_registers

	pop %rbp
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	pop %r11
	pop %r10
	pop %r9
	pop %r8
	pop %rdi
	pop %rsi
	pop %rdx
	pop %rcx
	pop %rbx
	pop %rax

.endm



irq_entry apic_timer_handler do_apic_timer_handler	//0x20
irq_entry keyboard_handler do_keyboard_handler		//0x21
irq_entry hpet_periodic_handler do_hpet_periodic_handler//0x22


//HPET割り込みハンドラ 0x28
.global hpet_handler
.extern do_hpet_handler
hpet_handler:

	save_registers
	mov %rsp, %rdi
	call do_hpet_handler
	rest_registers


	iretq





irq_entry ide_handler do_ide_handler				//0x2C
irq_entry apic_error_handler do_apic_error_handler	//0x43
irq_entry ipi_handler do_ipi_handler				//0x44
irq_entry acpi_interrupt_handler do_acpi_interrupt_handler	//0x29
irq_entry svga_handler do_svga_handler				// ?


isr_entry except do_except 2
isr_entry except do_except 3
isr_entry except do_except 4
isr_entry except do_except 5

isr_entry except do_except 7
isr_entry except do_except 8
isr_entry except do_except 9
isr_entry except do_except 10
isr_entry except do_except 11
isr_entry except do_except 12
isr_entry except do_except 13
isr_entry except do_except 15
isr_entry except do_except 16
isr_entry except do_except 17
isr_entry except do_except 18
isr_entry except do_except 19
isr_entry except do_except 20
isr_entry except do_except 21
isr_entry except do_except 22
isr_entry except do_except 23
isr_entry except do_except 24
isr_entry except do_except 25
isr_entry except do_except 26
isr_entry except do_except 27
isr_entry except do_except 28
isr_entry except do_except 29
isr_entry except do_except 30
isr_entry except do_except 31



//0除算エラーハンドラ
.global divide_error
.extern do_divide_error
divide_error:

	save_registers
	mov %rsp, %rdi
	call do_divide_error
	rest_registers


	iretq



//無効オペコード例外ハンドラ
.global debug_single_step
.extern do_debug_single_step
debug_single_step:

	save_registers
	mov %rsp, %rdi
	call do_debug_single_step
	rest_registers

	iretq


//無効オペコード例外ハンドラ
.global invalid_opcode
.extern do_invalid_opcode
invalid_opcode:

	save_registers
	mov %rsp, %rdi
	call do_invalid_opcode
	rest_registers

	iretq



//ページフォルト例外ハンドラ

.align 0x10
.global page_fault
.extern do_page_fault
page_fault:
	save_registers		//レジスタ退避
	mov %cr2, %rdi		//フォルトアドレス取得
	mov %rsp, %rsi
	call do_page_fault
	rest_registers		//レジスタリストア

	add $8, %rsp		//エラーコード消去

	iretq




.globl systemcall_handler
.text
//システムコールハンドラ
systemcall_handler:

/*
cli
1:
hlt
jmp 1b*/
	//ユーザースタックを退避
	movq %rsp, user_stack
	//システムコールスタックを代入
	movq $system_call_stack, %rsp

	//レジスタ退避
	push %rbx
	push %rcx
	push %rdx
	push %rsi
	push %rdi
	push %r8
	push %r9
	push %r10
	push %r11
	push %r12
	push %r13
	push %r14
	push %r15
	push %rbp

	//引数指定

	call do_systemcall_handler	//ハンドラ呼び出し

	//RAX以外のレジスタを戻す
	pop %rbp
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	pop %r11
	pop %r10
	pop %r9
	pop %r8
	pop %rdi
	pop %rsi
	pop %rdx
	pop %rcx
	pop %rbx

	//ユーザースタックへ
	movq user_stack, %rsp



	//Ring-3へ
	sysretq
1:
hlt
jmp 1b

.globl native_enter_user_64
native_enter_user_64:

	pushq $0x023
	pushq %rdi
	pushq $0x200
	pushq $0x2B
	pushq %rsi


	xorq %rax, %rax
	xorq %rbx, %rbx
	xorq %rcx, %rcx
	xorq %rdx, %rdx
	xorq %rdi, %rdi
	xorq %rsi, %rsi
	xorq %r8, %r8
	xorq %r9, %r9
	xorq %r10, %r10
	xorq %r11, %r11
	xorq %r12, %r12
	xorq %r13, %r13
	xorq %r14, %r14
	xorq %r15, %r15
	xorq %rbp, %rbp

	iretq






