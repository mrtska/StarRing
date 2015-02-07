/*
[Module tss.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php

Created on: 2015/02/07
*/

#pragma once


#define TSS_DESC_DPL0 (0x00 << 1)
#define TSS_DESC_DPL1 (0x01 << 1)
#define TSS_DESC_DPL2 (0x02 << 1)
#define TSS_DESC_DPL3 (0x03 << 1)

#define TSS_DESC_P 0x80
#define TSS_DESC_TYPE_AVAIL 0x9

struct tss_descriptor {

	unsigned short limit1;
	unsigned short base1;
	unsigned char base2;
	unsigned char flags;
	unsigned char limit2;
	unsigned char base3;
	unsigned int base4;
	unsigned int reserved;
} __attribute__((packed));

struct tss {

	unsigned int reserved1;
	unsigned long rsp[3];
	unsigned long reserved2;
	unsigned long ist[7];
	unsigned short reserved3[5];
	unsigned short io_map_base;
} __attribute__((packed));
