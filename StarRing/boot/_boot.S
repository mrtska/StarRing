;
; boot.S
;
;  Created on: 2014/04/01
;      Author: StarRing
;

[BITS 32]
;マルチブートヘッダセクション開始
[SECTION .multibootheader]


MAGIC equ 0xE85250D6
ARCH  equ 0

HEADER:
	;マジック
	DD MAGIC
	;アーキテクチャ
	DD ARCH
	;オフセット
	DD HEADEREND - HEADER
	;チェックサム
	DD -(MAGIC + ARCH + (HEADEREND - HEADER))

	;マルチブートヘッダ終了符号
	DD 0
	DD 8

HEADEREND:

;エントリポイント
[GLOBAL ENTRYPOINT]
[SECTION .boot]
[EXTERN Stack]
ENTRYPOINT:

	mov edi, eax
	mov esi, ebx

	;一つ目のGDTをロード
	lgdt [GDT1]

	;CPUパイプライン初期化
	jmp 8:PipeLineflush

PipeLineflush:
	;スタックポインタ初期化
	mov eax, 0x10
	mov ds, ax
	mov ss, ax
	mov esp, Stack

;ページング、ロングモード移行
[EXTERN early_page_init]
SETUP:
	;ページング
	call early_page_init

	;PAEを有効化
	mov eax, cr4
	or eax, 1 << 5
	mov cr4, eax

	;ロングモード移行
	mov ecx, 0xC0000080
	rdmsr
	or  eax, 1 << 8
	bts eax, 0
	wrmsr

	;ページング有効化
	mov eax, cr0
	or  eax, 1 << 31
	mov cr0, eax

;セットアップ終了

	;二つ目のGDTをロード
	mov eax, GDT2
	lgdt [eax]

	;パイプライン初期化
	jmp 0x8:GDT2Ready

mboot_info1:
	DQ 0x0
mboot_info2:
	DQ 0x0

[BITS 64]
[EXTERN main]
[EXTERN gdt_init]
GDT2Ready:

	;レジスタ初期化

	mov rsp, Stack + 0xFFFFFFFF80000000

	mov esi, [mboot_info1]
	mov edi, [mboot_info2]

	push 0
	popf

	lgdt [GDT3]
	xor eax, eax
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax

	mov rax, main
	call rax
	cli

;ハングアップ 呼ばれないはず
A:
hlt
jmp A


stack:
	0x4000

;グローバルディスクリプタテーブル
TmpGDT:
	DQ 0x0000000000000000
	DQ 0x00CF9A000000FFFF
	DQ 0x00CF92000000FFFF
	DQ 0x0000000000000000	;/* NULL descriptor */
	DQ 0x00AF9A000000FFFF	;/* __KERNEL_CS */
	DQ 0x00CF92000000FFFF	;/* __KERNEL_DS */
	DQ 0x0000000000000000
	DQ 0x00A09A0000000000
	DQ 0x00A0920000000000

;Protected Mode
GDT1:
	DW 23
	DD TmpGDT
GDT2:
	DW 23
	DD TmpGDT + 24
	DD 0

;ロングモード用
[EXTERN gdt_page]
GDT3:
	DW 16 * 8 - 1
	DQ gdt_page



[SECTION .page_table]
PML4:


