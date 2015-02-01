/*
 * apic.h
 *
 *  Created on: 2014/04/05
 *      Author: StarRing
 */

#pragma once

#include <ioapic.h>
#include <internal/stdio.h>
#include <mm.h>

extern struct apic_info apic_info;

#define APIC_BASE_ADDRESS			0xFEE00000

//APICのバージョン情報があるアドレス
#define APIC_VERSION_ADDRESS		0xFEE00030

//APICIDがあるアドレス
#define APIC_ID_R	0xFEE00020

//APIC SVRがあるアドレス
#define APIC_SVR			0xFEE000F0

//APIC LVTエラーレジスタ
#define APIC_LVT_ERROR_REGISTER		0xFEE00370

//APIC 割り込みコマンドレジスタ
#define APIC_ICR_LOW			0xFEE00300
#define APIC_ICR_HIGH				0xFEE00310

//APIC ディスティネーションフォーマットレジスタ
#define APIC_DFR					0xFEE000E0

//APIC ロジカルディスティネーションレジスタ
#define APIC_LDR					0xFEE000D0

//APIC タスク優先度レジスタ
#define  APIC_TPR					0xFEE00080

//APIC 割り込み終了通知
#define APIC_EOI					0xFEE000B0

//APIC 温度センサレジスタ
#define APIC_THMR					0xFEE00330

//APIC CMCIレジスタ
#define APIC_CMCI					0xFEE002F0

//APIC パフォーマンスモニタリングカウンタレジスタ
#define APIC_PMCR					0xFEE00370

//APIC エラーステータスレジスタ
#define APIC_ESR					0xFEE00280

//APIC デリバリーモード
#define		APIC_DM_FIXED		0x00000
#define		APIC_DM_FIXED_MASK	0x00700
#define		APIC_DM_LOWEST		0x00100
#define		APIC_DM_SMI		0x00200
#define		APIC_DM_REMRD		0x00300
#define		APIC_DM_NMI		0x00400
#define		APIC_DM_INIT		0x00500
#define		APIC_DM_STARTUP		0x00600
#define		APIC_DM_EXTINT		0x00700




//APIC タイマー
#define APIC_LVT_TIMER				0xFEE00320
#define APIC_LVT_PERF				0xFEE00340
#define APIC_LVT_LINT0				0xFEE00350
#define APIC_LVT_LINT1				0xFEE00360

#define APIC_TIMER_INITIAL_COUNT	0xFEE00380
#define APIC_TIMER_CURRENT_COUNT	0xFEE00390
//APIC ディバイドコンフィグレーションレジスタ
#define APIC_DCR					0xFEE003E0
#define APIC_TDIV_1					0xB
#define APIC_TDIV_2					0x0
#define APIC_TDIV_4					0x1
#define APIC_TDIV_8					0x2
#define APIC_TDIV_16				0x3
#define APIC_TDIV_32				0x8
#define APIC_TDIV_64				0x9
#define APIC_TDIV_128				0xA

#define TIMER_PERIODIC				0x20000

//APIC有効化
#define APIC_ENABLED				0x100

//APIC 無効化
#define APIC_DISABLED				0x10000

#define APIC_NMI					(4 << 8)

//ICRのBUSYフラグ
#define APIC_ICR_BUSY				0x1000

#define IMCR_ADDRESS				0x22    // IMCR アドレス
#define IMCR_DATA					0x23    // IMCR データ
#define IMCR_REG_SEL				0x70    // IMCR reg select
#define IMCR_IOAPIC_ON				0x01    // IMCR IOAPIC 有効化
#define IMCR_IOAPIC_OFF				0x00    // IMCR IOAPIC 無効化



#define    GET_LAPIC_VERSION(x)          ((x) & 0xFFu)
#define    GET_LAPIC_MAXLVT(x)           (((x) >> 16) & 0xFFu)


struct apic_info {

	int num_processor;
	unsigned long ioapic_addr;
	int ioapic_id;
};



//IDからデータをロード/セーブ
#define apic_read(p) mmio_read32(p)
#define apic_write(p, v) mmio_write32(p, v)





//IPIを送信
static __inline__ void apic_ipi(unsigned int dest, unsigned int id, unsigned char vec) {

	apic_write(APIC_ICR_LOW, id | vec);
}

static __inline__ int apic_get_max_lvt(void) {

	unsigned int v = apic_read(APIC_VERSION_ADDRESS);

	return GET_LAPIC_MAXLVT(v);
}

static __inline__ void apic_eoi(void) {

	apic_write(APIC_EOI, 0);
}
//APIC初期化
void apic_init(void);
void enable_apic_timer(void);





