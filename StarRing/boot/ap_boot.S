;
;[Module ap_boot.S]
;Copyright(c) 2015 mrtska.starring
;
;This software is released under the MIT License.
;
;http://opensource.org/licenses/mit-license.php

; Created on: 2014/08/14

%define SEGDESC_S	(1<<12)
%define SEGDESC_P	(1<<15)
%define SEGDESC_AVL	(1<<20)
%define SEGDESC_L	(1<<21)
%define SEGDESC_DB	(1<<22)
%define SEGDESC_G	(1<<23)

%define SEGDESC_EXECREAD (0xa<<8)
%define SEGDESC_READWRITE (0x2<<8)
%define SEGDESC_TSS (0x09<<8)

%define SEGDESC_DPL_SHIFT 13
%define SEGDESC_LIMIT_SHIFT 16
%define SEGDESC_BASE_SHIFTL 0
%define SEGDESC_BASE_SHIFTH 24

%define	SEGDESC_RW64 (SEGDESC_P|SEGDESC_L|SEGDESC_G|SEGDESC_S|SEGDESC_READWRITE)
%define	SEGDESC_RW32 (SEGDESC_P|SEGDESC_DB|SEGDESC_G|SEGDESC_S|SEGDESC_READWRITE)
%define	SEGDESC_RW16 (SEGDESC_P|SEGDESC_DB|SEGDESC_S|SEGDESC_READWRITE)

%define	SEGDESC_EXEC16 (SEGDESC_P|SEGDESC_S|SEGDESC_EXECREAD)
%define	SEGDESC_EXEC (SEGDESC_P|SEGDESC_DB|SEGDESC_G|SEGDESC_S|SEGDESC_EXECREAD)
%define	SEGDESC_EXEC64 (SEGDESC_P|SEGDESC_L|SEGDESC_G|SEGDESC_S|SEGDESC_EXECREAD)

%define SEGDESC_TSS64 (SEGDESC_P|SEGDESC_TSS)

;アプリケーションプロセッサーのエントリポイント
[BITS 16]
ap_boot_16:

	;配置されているアドレスが0x40000なのでデータセグメントに0x4000を入れる
	mov	ax, 0x4000
	mov	ds, ax

	;プロテクトモード有効化
	mov eax, cr0
	or eax, 1
	mov cr0, eax

;キャッシュをフラッシュ
	jmp flush
flush:

	;GDTをロード
	lgdt [gdtdesc]

	;セグメントの設定
	mov ax, 8
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	;32ビットコードへジャンプする
	jmp dword 16:(ap_boot_32+0x40000)

[BITS 32]
;boot.Sのap_boot_32にジャンプする
ap_boot_32:

	mov eax, 0x100000 + 0x1200
	jmp eax

;一応
A:
HLT
JMP A


ap_boot_end:


;GDT
align	16
gdt:
	dw	0, 0
	dd	0

	; read write (8)
	dw	0xffff, 0
	dd	(0xf<<SEGDESC_LIMIT_SHIFT) | (SEGDESC_RW32) | (0<<SEGDESC_DPL_SHIFT)

	; exec read (16)
	dw	0xffff, 0
	dd	(0xf<<SEGDESC_LIMIT_SHIFT) | (SEGDESC_EXEC) | (0<<SEGDESC_DPL_SHIFT)

gdtdesc:
	dw	8*3-1
	dd	(gdt+0x40000)

align	100h



