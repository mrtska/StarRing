	.file	"page.c"
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC0:
	.string	"PML4 = %u, PDPTE = %u, PDE = %u, PTE = %u\n"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC1:
	.string	"Next CR3 = %p\n"
.LC2:
	.string	"Paging init\n"
	.text
	.p2align 4,,15
	.globl	page_init
	.type	page_init, @function
page_init:
.LFB13:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	xorl	%r8d, %r8d
	movl	$8, %ecx
	xorl	%edx, %edx
	xorl	%esi, %esi
	movl	$.LC0, %edi
	xorl	%eax, %eax
	call	kprintf
	movl	$kernel_pml4, %esi
	movl	$.LC1, %edi
	xorl	%eax, %eax
	call	kprintf
	movabsq	$4503599627370495, %rax
	andl	$kernel_pml4, %eax
#APP
# 33 "page.c" 1
	mov %rax, %cr3
# 0 "" 2
#NO_APP
	movl	$.LC2, %edi
	xorl	%eax, %eax
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	jmp	kprintf
	.cfi_endproc
.LFE13:
	.size	page_init, .-page_init
	.globl	kernel_pml4
	.section	.table,"aw",@progbits
	.align 8
	.type	kernel_pml4, @object
	.size	kernel_pml4, 8
kernel_pml4:
	.zero	8
	.ident	"GCC: (GNU) 4.7.3"
	.section	.note.GNU-stack,"",@progbits
