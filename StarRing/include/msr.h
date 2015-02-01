/*
[Module msr.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/04/05
*/

#pragma once

#include <system.h>
#include <cpuid.h>

static __inline__ bool hasMSR(void) {

	unsigned long rax;
	unsigned long rdx;
	asmv("cpuid" : "=a"(rax), "=d"(rdx) : "0"(CPUID_GETFEATURES));

	return rdx & CPUID_FLAG_MSR;
}

static __inline__ void get_msr(unsigned int msr, unsigned int *low, unsigned int *high) {

	asmv("rdmsr" : "=a"(*low), "=d"(*high) : "c"(msr));
}

static __inline__ void set_msr(unsigned int msr, unsigned int low, unsigned int high) {

	asmv("wrmsr" : : "a"(low), "d"(high), "c"(msr));
}

static __inline__ unsigned long read_msr(unsigned int msr) {

	unsigned int low, high;

	asmv("rdmsr" : "=a" (low), "=d" (high) : "c" (msr));
	return low | ((unsigned long)high << 32);
}

static __inline__ void write_msr(unsigned int msr, unsigned long val) {

	asmv("wrmsr" : /* no output */ : "c" (msr), "a" (val), "d" (val >> 32) : "memory");
}

