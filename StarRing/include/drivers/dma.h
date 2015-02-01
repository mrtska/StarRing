/*
 * dma.h
 *
 *  Created on: 2014/10/11
 *      Author: StarRing
 */

#pragma once

#include <system.h>

#define PCI_DMA_BUS_MASTER_VENDOR_ID	0x8086
#define PCI_DMA_BUS_MASTER_DEVICE_ID	0x27B9

#define DMA_PRIMARY_COMMAND_R	0x00
#define DMA_PRIMARY_STATUS_R	0x02
#define DMA_PRIMARY_PRDT_R		0x04

#define DMA_SECONDRY_COMMAND_R	0x08
#define DMA_SECONDRY_STATUS_R	0x0A
#define DMA_SECONDRY_PRDT_R		0x0C

#define DMA_COMMAND_DMA_START	0x00
#define DMA_COMMAND_DMA_RW		0x04

extern int dma_base_port;
extern unsigned long prdt_pointer;

struct prd {

	unsigned int data_buffer;
	unsigned short byte_count;
	unsigned int reserved :15;
	unsigned int msb :1;
}__attribute__((packed));

static __inline__ void dma_write_value(int reg, unsigned char value) {

	outb(dma_base_port + reg, value);
}

static __inline__ unsigned char dma_read_value(int reg) {

	return inb(dma_base_port + reg);
}

//DMA転送するときに使うPRDを設定する バッファに仮想アドレスは使えない 0xFFFFFFFF以下の物理アドレスのみ使える
static __inline__ void set_prdt(struct prd *prd, unsigned char *buf, unsigned int size) {

	//データサイズ

	while(size != 0) {

		prd->data_buffer = (unsigned int) (unsigned long) buf;


		//サイズが0xFFFFを上回っていたら
		if(size > 0xFFFF) {

			prd->byte_count = 0xFFFF;
			size -= 0xFFFF;
			prd->msb = 0;
			buf += 0xFFFF;
		} else {

			prd->byte_count = size;
			size = 0;
			prd->msb = 1;
		}

		prd++;

	}
}

void dma_init(void);

