

.code32

.section .multibootheader

#define PG_PRESENT      (1<<0)	//ページが有効か
#define PG_WRITE        (1<<1)		//ページが書き込み可能か
#define PG_USER         (1<<2)		//ページがユーザーページかどうか
#define PG_PWT          (1<<3)		//ライトスルーか
#define PG_NOCACHE      (1<<4)	//キャッシュするか
#define PG_ACCESSED     (1<<5)	//ソフトウェアにアクセスされたか
#define PG_DIRTY        (1<<6)		//ソフトウェアに書き込まれたか
#define PG_LARGE        (1<<7)		//ラージページか
#define PG_GLOBAL       (1<<8)		//グローバルページか


//マルチブート2マジック
#define MAGIC 0xE85250D6
#define ARCH 0

HEADER:

	//マジック
	.long MAGIC
	//アーキテクチャ
	.long ARCH
	//オフセット
	.long HEADER_END - HEADER
	//チェックサム
	.long -(MAGIC + ARCH + (HEADER_END - HEADER))

	//ヘッダ終了
	.long 0
	.long 8

HEADER_END:

.globl ENTRYPOINT
.section .boot
ENTRYPOINT:

	//割り込み無効
	cli

	//ブート情報を退避
	movl %eax, %edi
	movl %ebx, %esi

	//EFLAGS初期化
	push	$0
	popf


	//PML4セットアップ
	movl $(_boot_pdpt) + (PG_PRESENT | PG_WRITE), (_boot_pml4)

	//PDPTセットアップ
	movl $(_boot_pd) + (PG_PRESENT | PG_WRITE), (_boot_pdpt)

	//0xFFFFFFFF80000000~
	movl $(_boot_pdpt) + (PG_PRESENT | PG_WRITE), (_boot_pml4) + 0xFF8

	//0xFFFF880000000000~
	movl $(_boot_pdpt) + (PG_PRESENT | PG_WRITE), (_boot_pml4) + 0x880

	//0xFFFFFFFF80000000~
	movl $(_boot_pd) + (PG_PRESENT | PG_WRITE), (_boot_pdpt) + 0xFF0

	//ページディレクトリを設定
	movl $(_boot_pd), %ebx
	movl $(PG_PRESENT | PG_WRITE | PG_LARGE | PG_GLOBAL), %eax
	movl %eax, (%ebx)
	movl $0, 4(%ebx)


	//仮想アドレスと物理アドレスをストレートマッピング
/*	add	$8, %ebx
    addl $0x200000, %eax // 2MB : 0x200000
    movl %eax, (%ebx)
    movl $0, 4(%ebx)

    add $8, %ebx
    addl $0x200000, %eax // 2MB : 0x400000
    movl %eax, (%ebx)
    movl $0, 4(%ebx)

    add $8, %ebx
    addl $0x200000, %eax // 2MB : 0x600000
    movl %eax, (%ebx)
    movl $0, 4(%ebx)

    add $8, %ebx
    addl $0x200000, %eax // 2MB : 0x800000
    movl %eax, (%ebx)
    movl $0, 4(%ebx)*/

	//PAEを有効化
	movl %cr4, %eax
	bts  $5, %eax
	bts  $8, %eax
	bts  $9, %eax
	bts  $10, %eax
	movl %eax, %cr4

	//ページテーブルを設定
	movl $(_boot_pml4), %eax
	movl %eax, %cr3

	//MSR.LMEビットを建て、ロングモードへ
	movl $0xC0000080, %ecx
	rdmsr
	bts  $8, %eax
	bts  $0, %eax
	wrmsr

	//ページング有効化
	movl %cr0, %eax
	bts  $31, %eax
	bts  $1, %eax
	movl %eax, %cr0

	//一つ目のGDTをロード
	movl $(_lmode_gdt_start), (_boot_gdtr) + 0x2
	lgdt (_boot_gdtr)

	//CPUパイプライン初期化
	ljmp $0x8, $(long_mode)


.code64
.align 8
.extern main
.extern bss_init
.extern ctors
//ロングモード
long_mode:

	//highへ相対ジャンプ
	movq $high, %rax
	jmp  *%rax

high:
	//ディレクションフラグをクリア
	cld

	//仮のスタックを設定
	movq $((0x4000) + 0xFFFFFFFF80000000), %rax
	movq %rax, %rsp

	pushq %rsi
	pushq %rdi
	pushq %rdx


    call bss_init

	movq $ctors, %rdx

construct:

	movq (%rdx), %rax


	cmpq $0, %rax

    je constructed

	pushq %rdx


	call *%rax

	popq %rdx

	addq $8, %rdx

	jmp construct

constructed:


    popq %rdx
	popq %rdi
	popq %rsi



	//main.c#mainを呼び出し
	call main

	//戻ってきたらバグ
	cli
1:
hlt
jmp 1b


.align 0x1000

//GDT
_lmode_gdt_start:
    .quad 0
    .quad 0x002F98000000FFFF
.L_lmode_gdt_end:

_boot_gdtr:
    .word .L_lmode_gdt_end - _lmode_gdt_start - 1
    .long 0

.align 0x10





.section .page_table

//ブート時ページテーブル
.align 0x1000
.globl _boot_pml4, _boot_pdpt, _boot_pd
_boot_pml4:

.space 0x1000

_boot_pdpt:

.space 0x1000

_boot_pd:

.space 0x1000



mboot_magic:
	.long 0
mboot_info:
	.long 0

























