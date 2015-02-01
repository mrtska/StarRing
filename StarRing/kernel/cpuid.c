/*
[Module cpuid.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/05/07
*/

#include <cpuid.h>
#include <internal/stdio.h>

cpu_features features;


static void parse_reg(char *buf, cpuid_reg *info) {

	int i;
	buf[16] = '\0';
	for(i = 0; i < 4; i++) {

		buf[i]		= info->eax >> (8 * i);
		buf[i + 4]	= info->ebx >> (8 * i);
		buf[i + 8]	= info->ecx >> (8 * i);
		buf[i + 12]	= info->edx >> (8 * i);
	}

}

//CPU情報取得
void machine_check(void) {

	cpuid_reg info;
	int i;

	cpuid(CPUID_GETVENDORSTRING, &info);
	if(info.ebx != 0x756E6547) {
		kprintf("Support only Intel CPUs\n");
		return;
	}

	cpuid(CPUID_GETFEATURES, &info);

	features.model	= (info.eax >> 4) & 0xF;
	features.family	= (info.eax >> 20) & 0xFF;
	features.type	= (info.eax >> 12) & 0xF;
	features.brand	= (info.ebx) & 0xFF;
/*

	kprintf("Processor type = ");
	switch(features.type) {
	case 0:
		kprintf("Original OEM Processor\n");
		break;
	case 1:
		kprintf("Intel OverDrive Processor\n");
		break;
	case 2:
		kprintf("Dual Processor (not applicable to Intel486 processors)\n");
		break;
	case 3:
		kprintf("Intel Reserved\n");
		break;
	}*/

	char string[17];
	cpuid(CPUID_INTELBRANDSTRING, &info);
	parse_reg(string, &info);


	for(i = 0; i < 16; i++) {

		features.cpu_name[i] = string[i];
	}
	kprintf("CPU name = ");

	cpuid(CPUID_INTELBRANDSTRINGMORE, &info);
	parse_reg(string, &info);
	for(i = 0; i < 16; i++) {
		features.cpu_name[i + 16] = string[i];
	}

	cpuid(CPUID_INTELBRANDSTRINGEND, &info);
	parse_reg(string, &info);
	for(i = 0; i < 16; i++) {
		features.cpu_name[i + 32] = string[i];
	}
	kprintf("%s\n", features.cpu_name);
}








