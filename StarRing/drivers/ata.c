/*
[Module ata.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/10/09
*/

#include <drivers/ata.h>
#include <drivers/dma.h>
#include <drivers/pci.h>
#include <internal/stdio.h>
#include <system.h>
#include <wait.h>
#include <desc.h>
#include <apic.h>
#include <page/page.h>
#include <mem/alloc.h>


static void ata_non_data(struct ata_param *param, int device);
static void ata_read_pio_by_param(struct ata_param *param, int device);
static void ata_write_sector(unsigned long lba, unsigned char *buf, unsigned long sector);
static void ata_read_sector_via_dma(unsigned long lba, unsigned char *buf, unsigned long sector);
static void ata_write_sector_via_dma(unsigned long lba, unsigned char *buf, unsigned long sector);



static struct storage_operations ata_storage_operations = {

		.pio_read = NULL,
		.pio_write = NULL,
		.dma_read = ata_read_sector_via_dma,
		.dma_write = ata_write_sector_via_dma
};




//BUSYビットが折れるのを待つ
static void ata_ready(void) {

	while(inb(ATA_STATUS_R) & ATA_STATUS_BUSY)
		;
	mfence();
}

static void ata_ready_with_drq(void) {

	unsigned char status = inb(ATA_AL_STATUS_R);
	while((status & ATA_STATUS_BUSY) && (status & ATA_STATUS_DRQ)) {
	}
}

static void ata_io_wait(void) {

	inb(ATA_AL_STATUS_R);
	inb(ATA_AL_STATUS_R);
	inb(ATA_AL_STATUS_R);
	inb(ATA_AL_STATUS_R);
}


//ATA デバイスセレクションプロトコル
static void device_selection(int slave) {

	unsigned char status = inb(ATA_STATUS_R);
	while((status & ATA_STATUS_BUSY) && (status & ATA_STATUS_DRQ)) {	//BUSYビットとDRQビットが折れるのを待つ
	}

	outb(ATA_DEVICE_HEAD_R, slave << 4);	//デバイスを選択
	wait_nsec(400);
	while((status & ATA_STATUS_BUSY) && (status & ATA_STATUS_DRQ)) {	//BUSYビットとDRQビットが折れるのを待つ
	}
}


static volatile int interrupt_wait;


void wait_interrupt(void) {

	while(true) {

		if(interrupt_wait) {
			break;
		}
	}
	mfence();
	interrupt_wait = 0;

	inb(ATA_STATUS_R);
}


void do_ide_handler(void) {

	interrupt_wait = 1;
	apic_eoi();

}

extern void ide_handler(void);

static void ata_device_init(int device) {


	//---Identify Deviceコマンド---
	struct ata_param param;
	param.features = 0;
	param.sector_count = 1;
	param.sector_number = 0;
	param.cylinder_low = 0;
	param.cylinder_high = 0;
	param.device_head = 0;
	param.command = ATA_CMD_IDENTIFY;	//デバイス識別コマンド

	//リザルト
	struct ata_identify identify;

	param.buf = (unsigned char*) &identify;
	ata_read_pio_by_param(&param, device);	//結果を取得

	//---文字列入れ替え---
	unsigned char * ptr = (unsigned char *) &identify.model;
	int i;
	for(i = 0; i < 39; i += 2) {
		unsigned char tmp = ptr[i + 1];
		ptr[i + 1] = ptr[i];
		ptr[i] = tmp;
	}
	memset(&identify.model[39], 0x00, 1);	//最終文字にヌル文字を代入しておく
	//------

	//kprintf("ATA FIRM = %s\n", identify.model);
	//kprintf("MAX sectors %X, %X, head = %X\n", identify.sectors_28, identify.sectors_48, identify.heads);

	//uDMAをサポートしているかどうかのフィールド
	unsigned short uDMA = identify.uDMA_support.uDMA_union;


	unsigned int max_version;		//最新対応バージョン
	unsigned int current_version;	//現在設定されているバージョン

	for(i = 6; i >= 0; i--) {

		if((uDMA >> i) & 0x0001) break;
	}
	//サポートしている最大のバージョン
	max_version = i;


	for(i = 6; i >= 0; i--) {

		if((uDMA >> (i + 8)) & 0x0001) break;
	}

	//現在のバージョン
	current_version = i;

	//現在のバージョンよりも上のバージョンに対応していたら、それに変更する
	if(max_version > current_version) {

		struct ata_param param_features;
		param_features.features = 0x03;
		param_features.sector_count = 70;
		param_features.sector_number = 0;
		param_features.cylinder_low = 0;
		param_features.cylinder_high = 0;
		param_features.device_head = 0;
		param_features.command = ATA_CMD_SET_FEATURES;

		//SET FEATURESコマンド実行
		ata_non_data(&param_features, device);
	}

	//もう一度Identify Deviceコマンドを実行
	ata_read_pio_by_param(&param, device);

	uDMA = identify.uDMA_support.uDMA_union;


	for(i = 6; i >= 0; i--) {

		if((uDMA >> (i + 8)) & 0x0001) break;
	}
	kprintf("Current uDMA version = %d\n", i);

}


//初期化
void ata_init(struct storage_device *storage) {


	unsigned int intr = (pci_conf_read(storage->pci_device, PCI_REGISTER_INTERRUPT_PIN));

	kprintf("ata interrupt %s, %d\n", storage->pci_device->vender_name, intr);
	STOP;

	set_intr_gate(0x2C, ide_handler);	//割り込みハンドラ登録
	io_apic_set_redirect(14, 0, 0x2C);	//IO APICに登録

	storage->opts = &ata_storage_operations;

	ata_device_init(0);
	ata_device_init(1);
	//バスマスターDMA初期化
	dma_init();
}


static void ata_non_data(struct ata_param *param, int device) {


	device_selection(device);

	//---レジスタにパラメーターを設定---
	outb(ATA_DEVICE_CTRL_R, 0);
	outb(ATA_FEATURES_R, param->features);
	outb(ATA_LBA_LOW_R, param->sector_number);
	outb(ATA_LBA_MID_R, param->cylinder_low);
	outb(ATA_LBA_HIGH_R, param->cylinder_high);
	outb(ATA_DEVICE_HEAD_R, param->device_head | device << 4);
	outb(ATA_SECTOR_COUNT_R, param->sector_count);
	outb(ATA_COMMAND_R, param->command);
	//------

	wait_nsec(400);	//400ナノ秒待つ

	wait_interrupt();	//割り込みが来たら終了
}

static void ata_read_pio(unsigned long lba, unsigned char *buf, unsigned long sector) {

	outb(ATA_FEATURES_R, 0x00);
	ata_ready_with_drq();

	outb(ATA_LBA_LOW_R, (lba & 0xFF));
	outb(ATA_LBA_MID_R, (lba >> 8) & 0xFF);
	outb(ATA_LBA_HIGH_R, (lba >> 16) & 0xFF);
	outb(ATA_DEVICE_HEAD_R, ((lba >> 24) & 0xFF) | 0x40);
	outb(ATA_SECTOR_COUNT_R, sector);

	outb(ATA_COMMAND_R, ATA_CMD_READ_PIO);

	ata_ready();

}

//PIOリード
static void ata_read_pio_by_param(struct ata_param *param, int device) {

	//デバイスセレクション
	device_selection(device);
	unsigned char *buf = param->buf;
	//---レジスタにパラメーターを設定---
	outb(ATA_DEVICE_CTRL_R, 0);
	outb(ATA_FEATURES_R, param->features);
	outb(ATA_LBA_LOW_R, param->sector_number);
	outb(ATA_LBA_MID_R, param->cylinder_low);
	outb(ATA_LBA_HIGH_R, param->cylinder_high);
	outb(ATA_DEVICE_HEAD_R, param->device_head | device << 4);
	outb(ATA_SECTOR_COUNT_R, param->sector_count);
	outb(ATA_COMMAND_R, param->command);
	//------

	wait_nsec(400);

	int i;
	for(i = 0; i < param->sector_count; i++) {

		//エラー確認
		if(inb(ATA_STATUS_R) & ATA_STATUS_ERROR) {

			kprintf("ERROR! pio read %d %b\n", i, inb(ATA_ERROR_R));
			while(1) asmv("hlt");
			return;
		}
		//割り込み待ち
		wait_interrupt();

		//データリード
		insw(ATA_DATA_R, buf, 0x100);
		buf += 0x200;
	}

	inb(ATA_AL_STATUS_R);
	inb(ATA_STATUS_R);
/*

	if(inb(ATA_STATUS_R) & ATA_STATUS_ERROR) {

		kprintf("ide read error!, %b\n", inb(ATA_ERROR_R));
		while(1) asmv("hlt");
	}*/
}

static void ata_write_sector(unsigned long lba, unsigned char *buf, unsigned long sector) {

	//割り込み許可

	outb(ATA_FEATURES_R, 0x00);
	ata_ready_with_drq();

	outb(ATA_LBA_LOW_R, (lba & 0xFF));
	outb(ATA_LBA_MID_R, (lba >> 8) & 0xFF);
	outb(ATA_LBA_HIGH_R, (lba >> 16) & 0xFF);
	outb(ATA_DEVICE_HEAD_R, ((lba >> 24) & 0xFF) | 0x40);
	outb(ATA_SECTOR_COUNT_R, sector);

	outb(ATA_COMMAND_R, ATA_CMD_WRITE_PIO);

	kprintf("write IDE STATUS = %b\n", inb(ATA_STATUS_R));

	outsw(ATA_DATA_R, buf, 256);

	while(interrupt_wait) {
		asmv("hlt");
	}
	interrupt_wait = true;

	kprintf("write IDE STATUS = %b, ERROR = %b\n", inb(ATA_STATUS_R), inb(ATA_ERROR_R));

	outb(ATA_COMMAND_R, ATA_CMD_CACHE_FLUSH);

	kprintf("write IDE STATUS = %b\n", inb(ATA_DEVICE_HEAD_R));
	ata_ready();
	ata_io_wait();

}

//IDE/HDDからデータをDMA転送する bufは仮想アドレスのカーネル空間でなくてはならない
static void ata_read_sector_via_dma(unsigned long lba, unsigned char *buf, unsigned long sector) {


	if(sector <= 0) {

		kprintf("sector is zero!, lba = %X\n", lba);
		return;
	}
	//エラー回数
	int error = 0;
	struct prd *prd = (struct prd*) prdt_pointer;
	void *p = alloc_memory_block();
	set_prdt(prd, p, sector * 0x200);	//PRDTを設定

	ata_ready();

	device_selection(0);
retry:

	outb(ATA_DEVICE_CTRL_R, 0x0);	//割り込み無し
	outb(ATA_FEATURES_R, 0x00);		//FEATUREには0x00をセットする

	//LBA設定
	outb(ATA_LBA_LOW_R, (lba & 0xFF));
	outb(ATA_LBA_MID_R, (lba >> 8) & 0xFF);
	outb(ATA_LBA_HIGH_R, (lba >> 16) & 0xFF);
	outb(ATA_DEVICE_HEAD_R, ((lba >> 24) & 0xFF) | 0xE0);
	outb(ATA_SECTOR_COUNT_R, sector);

	ata_ready();

	outb(ATA_COMMAND_R, ATA_CMD_READ_DMA);	//コマンド発行


	dma_write_value(DMA_PRIMARY_COMMAND_R, 0x9);	//DMA転送開始

		//転送完了待ち
	wait_interrupt();

	//リセット
	dma_write_value(DMA_PRIMARY_COMMAND_R, 0x0);	//DMA転送終了

	//ステータスレジスタロード
	if(inb(ATA_STATUS_R) & ATA_STATUS_ERROR) {

		kprintf("ide read error!, lba = %X, error %d, %b\n", lba, ++error, inb(ATA_ERROR_R));

		if(error == 5) {

			kprintf("abort\n");
			while(1) asmv("hlt");
		}

		goto retry;
	}

	memcpy(buf, (const void*) PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(p), sector * 0x200);
//	kprintf("read sector %X, lba %X                  %X %X\n", buf, lba, buf[0], buf[1]);
	mfence();
	free_memory_block(p);

}

static void ata_write_sector_via_dma(unsigned long lba, unsigned char *buf, unsigned long sector) {

}

