/*
[Module page.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/04/28
*/

#pragma once

#include <internal/stdio.h>
#include <system.h>
#include <task.h>

//map_page関数のフラグ
#define FLAGS_LARGE_PAGE	0x01
#define FLAGS_WRITABLE_PAGE	0x02
#define FLAGS_USER_PAGE		0x04
#define FLAGS_ALLOC_512MB		0x08
#define FLAGS_NEW_ALLOC		0x10
#define FLAGS_NO_EXECUTE	0x20


#define PG_PRESENT		(1<<0)	// 物理メモリ存在しているページ
#define PG_WRITE		(1<<1)	// 書き込み可能ページ
#define PG_USER			(1<<2)	// ユーザープロセス書き込み可能ページ
#define PG_PWT			(1<<3)	// ライトスルーページ
#define PG_NOCACHE		(1<<4)	// キャッシュ無効ページ
#define PG_ACCESSED		(1<<5)	// アクセスされたページ
#define PG_DIRTY		(1<<6)	// 書き込みがあったページ
#define PG_LARGE		(1<<7)	// 2MBラージページ
#define PG_GLOBAL		(1<<8)	// TLBからフラッシュされないページ
#define PG_NX			0x8000000000000000	//実行不可ページ

//アドレスマスク
#define MASK_PDE_ADDR	0x0000FFFFFFE00000
#define MASK_FRAME_ADDR	0x000FFFFFFFFFF000


//仮想アドレスを物理アドレスへ
#define VIRTUAL_ADDRESS_TO_PHYSICAL_ADDRESS(p) ((unsigned long)p - 0xFFFF880000000000)

//物理アドレスを仮想アドレスへ
#define PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(p) ((unsigned long)p + 0xFFFF880000000000)

//カーネルページテーブル
extern unsigned long *kernel_pml4;


//ページング初期化
void page_init(unsigned long memory_max);

//ユーザーページ作成
void setup_user_page(struct process *process);

//カーネル仮想アドレス空間とユーザー仮想アドレス空間を繋げる
void combine_kernel_pml4(unsigned long destination_pml4);

//ページをマップする
void map_page(unsigned long virt, unsigned long phys, unsigned long *cr3, int flag);

//リニアアドレスからインデックスを各取得する
static __inline__ unsigned long pml4_index(unsigned long addr) {

	return (addr >> 39) & 0x1FF;
}
static __inline__ unsigned long pdpt_index(unsigned long addr) {

	return (addr >> 30) & 0x1FF;
}
static __inline__ unsigned long pd_index(unsigned long addr) {

	return (addr >> 21) & 0x1FF;
}
static __inline__ unsigned long pt_index(unsigned long virtual_addr) {

	return (virtual_addr >> 12) & 0x1FF;
}
