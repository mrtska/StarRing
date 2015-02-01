/*
[Module acpi.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/08/15
*/

#pragma once

#include <internal/stdio.h>


//ACPIテーブル
struct acpi_table {

	unsigned long rsdp;
	unsigned long rsdt;
	unsigned long xsdt;
	unsigned long facp;
	unsigned long dsdt;
	int rsdt_nentry;
	int xsdt_nentry;
};

enum acpi_apic_type_code {

	ACPI_PROCESSOR_LOCAL_APIC = 0,
	ACPI_IO_APIC = 1,
	ACPI_INTERRUPT_SOURCE_OVERRIDE = 2,
	ACPI_NMI_SOURCE = 3,
	ACPI_LOCAL_APIC_NMI_STRUCTURE = 4,
	ACPI_LOCAL_APIC_ADDRESS_OVERRIDE_STRUCTURE = 5,
	ACPI_IO_SAPIC = 6,
	ACPI_PLATFORM_INTERRUPT_SOURCES = 8
};



#define ACPI_RSDP() (acpi_table.rsdp)
#define ACPI_RSDT() (acpi_table.rsdt)
#define ACPI_XSDT() (acpi_table.xsdt)
#define ACPI_FACP() (acpi_table.facp)
#define ACPI_DSDT() (acpi_table.dsdt)
#define ACPI_SIG(a,b,c,d) (((d)<<24) | ((c)<<16) | ((b)<<8) | (a))
#define ACPI_R64ADDR(b,o) (*((unsigned int*)((b)+(o))))
#define ACPI_R32(b,o) (*((unsigned int*)((b)+(o))))
#define ACPI_R64(b,o) (*((unsigned long*)((b)+(o))))
#define ACPI_R16(b,o) (*((unsigned short*)((b)+(o))))
#define ACPI_R8(b,o) (*((unsigned char*)((b)+(o))))


extern struct acpi_table acpi_table;



//ACPI関連の初期化
void acpi_init(void);
void acpica_start(void);

//指定したシグニチャを持つ記述子を見つける
unsigned long find_acpi_description_entry(unsigned int sig);










