





.text
.code64


//レジスタを全てスタックにぶち込む
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

//スタックからレジスタを全て復元する
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



.macro isr_entry sym
.global \sym
.extern exec_\sym
.align 0x10
\sym:
    save_registers
    movq %rsp, %rsi
    call exec_\sym

    rest_registers

    //addq $8, %rsp

    iretq

.endm

isr_entry divide_error
isr_entry debug
isr_entry nmi
isr_entry breakpoint
isr_entry overflow
isr_entry bound_range
isr_entry invalid_opcode
isr_entry device_not_available
isr_entry double_fault
isr_entry coprocessor_segment_overrun
isr_entry invalid_tss
isr_entry segment_not_present
isr_entry stack_segment_fault
isr_entry general_protection
isr_entry page_fault













