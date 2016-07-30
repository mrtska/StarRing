/*
[Module mm.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/08/17
*/

#pragma once

//mmioリード/ライト

static __inline__ unsigned long mmio_read64(unsigned long p) {

	p += 0xFFFF880000000000;
	return (*((volatile unsigned long*)(p)));
}


static __inline__ unsigned int mmio_read32(unsigned long p) {


	p += 0xFFFF880000000000;
	return (*((volatile unsigned int*)(p)));
}


static __inline__ unsigned short mmio_read16(unsigned long p) {

	p += 0xFFFF880000000000;
	return (*((volatile unsigned short*)(p)));
}


static __inline__ unsigned char mmio_read8(unsigned long p) {

	p += 0xFFFF880000000000;
	return (*((volatile unsigned char*)(p)));
}


static __inline__ void mmio_write64(unsigned long p, unsigned long v) {

	p += 0xFFFF880000000000;
	(*((volatile unsigned long*)(p))) = v;
}

static __inline__ void mmio_write32(unsigned long p, unsigned int v) {

	p += 0xFFFF880000000000;
	(*((volatile unsigned int*)(p))) = v;
}

static __inline__ void mmio_write16(unsigned long p, unsigned short v) {

	p += 0xFFFF880000000000;
	(*((volatile unsigned short*)(p))) = v;
}

static __inline__ void mmio_write8(unsigned long p, unsigned char v) {

	p += 0xFFFF880000000000;
	(*((volatile unsigned char*)(p))) = v;
}

