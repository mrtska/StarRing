/*
[Module ide.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/10/09
*/

#pragma once

#include <system.h>
#include <drivers/ata.h>

static __inline__ void outsw(unsigned short port, unsigned char *data, unsigned long size) {

	asmv("rep outsw" : "+S"(data), "+c"(size) : "d"(port));
}

static __inline__ void insw(unsigned short port, unsigned char * data, unsigned long size) {
	asmv("rep insw" : "+D"(data), "+c"(size) : "d"(port) : "memory");
}


void ata_init(void);

void ide_non_data(struct ata_param *param);

void ide_read_pio(struct ata_param *param);

void ide_write_sector(unsigned int lba, unsigned char *buf, int sector);

void ide_read_sector_via_dma(unsigned int lba, unsigned char *buf, int byte);

void ide_write_sector_via_dma(unsigned int lba, unsigned char *buf, int sector);
