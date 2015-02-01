/*
 * ioapic.c
 *
 *  Created on: 2014/04/05
 *      Author: StarRing
 */

#include <ioapic.h>
#include <system.h>
#include <internal/stdio.h>
#include <mm.h>

//IO APIC初期化
void io_apic_init(unsigned int io_apic_addr) {







}


//IO APIC リダイレクションテーブルに割り込みハンドラを登録する
void io_apic_set_redirect(int irq, unsigned int entry_high, unsigned int entry_low) {

	unsigned long ioapic_addr = apic_info.ioapic_addr;	//恐らく0xFEC00000
	unsigned long index = ioapic_addr;					//インデックスレジスタのアドレス
	unsigned long data = ioapic_addr + 0x10;			//データレジスタのアドレス
	unsigned int entry = (irq * 2) + 0x10;				//インデックスレジスタに代入する値

	mmio_write32(index, entry);			//インデックスレジスタへ代入
	mmio_write32(data, entry_low);	//データレジスタに値をセット
	mmio_write32(index, entry + 1);
	mmio_write32(data, entry_high);

}

void io_apic_set_cpu(int irq, int cpu) {

	unsigned long ioapic_addr = apic_info.ioapic_addr;	//恐らく0xFEC00000
	unsigned long index = ioapic_addr;					//インデックスレジスタのアドレス
	unsigned long data = ioapic_addr + 0x10;			//データレジスタのアドレス
	unsigned int entry = (irq * 2) + 0x10;				//インデックスレジスタに代入する値



	mmio_write32(index, entry + 1);	//インデックスレジスタへ代入
	mmio_write32(data, cpu << 24);	//データレジスタに値をセット
}
