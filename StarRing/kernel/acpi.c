/*
[Module acpi.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/04/06
*/

#include <string.h>
#include <internal/stdio.h>
#include <acpi.h>
#include "../acpica/include/actypes.h"
#include "../acpica/include/acexcep.h"
#include "../acpica/include/actbl.h"
#include "../acpica/include/acpiosxf.h"
#include "../acpica/include/acrestyp.h"
#include "../acpica/include/acpixf.h"
#include <slab.h>
#include <mm.h>
#include <ioapic.h>
#include <desc.h>
#include <page/page.h>

//ACPIテーブル
struct acpi_table acpi_table;

//テーブルのチェックサムが正しいか判定
static int table_checksum(unsigned long addr, int n) {

	int i;
	unsigned char sum = 0;
	unsigned char *ptr8 = (unsigned char*) addr;
	for(i = 0; i < n; i++) {

		sum += ptr8[i];
	}
	if(sum == 0) {

		return 1;
	}

	return 0;
}
static int is_valid_sig8_checksum(unsigned long addr, unsigned int sig0, unsigned int sig1) {

	unsigned int *ptr32 = (unsigned int*) addr;

	if((ptr32[0] == sig0) && (ptr32[1] == sig1)) {

		return table_checksum(addr, 20);
	}

	return 0;
}
static bool is_valid_sig4(unsigned long addr, unsigned int sig0) {

	unsigned int *ptr32 = (unsigned int*) addr;

	return (ptr32[0] == sig0);
}

//RSDPを探す
static unsigned long find_RSDP_region(unsigned long start, unsigned long end) {

	unsigned long cur;
	unsigned int sigRSD = ACPI_SIG('R', 'S', 'D', ' ');
	unsigned int sigPTR = ACPI_SIG('P', 'T', 'R', ' ');

	for(cur = start; cur < end; cur += 16) {

		if(is_valid_sig8_checksum(cur, sigRSD, sigPTR)) {
			return cur;
		}
	}

	return 0;
}

//XSDTを読み取る
static int read_xsdt(unsigned long xsdt) {

	int r = is_valid_sig4(xsdt, ACPI_SIG('X', 'S', 'D', 'T'));
	unsigned int num_entry, len;

	if(!r) {

		return false;
	}

	len = ACPI_R32(xsdt, 4);
	len -= 36;
	num_entry = len / 8;
	acpi_table.xsdt_nentry = num_entry;

	return true;
}
//RSDTを読み取る
static bool read_rsdt(unsigned long rsdt) {

	int r = is_valid_sig4(rsdt, ACPI_SIG('R', 'S', 'D', 'T'));
	unsigned int num_entry, len;

	if(!r) {

		return false;
	}

	len = ACPI_R32(rsdt, 4);
	len -= 36;
	num_entry = len / 4;

	acpi_table.rsdt_nentry = num_entry;

	return true;
}

//RSDPを探す
static unsigned long find_RSDP(void) {

	unsigned long ebda_addr;
	unsigned long rsdp;

	ebda_addr = *(unsigned short*) PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(0x40E);
	ebda_addr = PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(ebda_addr);

	rsdp = find_RSDP_region(ebda_addr, ebda_addr + 1024);

	if(rsdp) {

		return rsdp;
	}


	rsdp = find_RSDP_region(PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(0xE0000), PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(0x100000));
	if(rsdp) {

		return rsdp;
	}

	return 0;
}

//指定されたACPI記述子を探す
unsigned long find_acpi_description_entry(unsigned int sig) {

	int i;
	int n = acpi_table.rsdt_nentry;
	unsigned long rsdt = acpi_table.rsdt;
	unsigned long xsdt = acpi_table.xsdt;

	if(rsdt) {

		for(i = 0; i < n; i++) {

			unsigned long addr = PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(ACPI_R32(rsdt, 36 + i * 4));
			if(sig == *(unsigned int*) addr) {

				return addr;
			}
		}
	}

	if(xsdt) {

		for(i = 0; i < n; i++) {

			unsigned long addr = PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(ACPI_R64(xsdt, 36 + i * 8));
			if(sig == *(unsigned int*) addr) {

				return addr;
			}
		}
	}

	return 0;
}

//FACPを読み取る
static bool read_facp(unsigned long facp) {

	unsigned long dsdt;
	int r;

	dsdt = PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(ACPI_R32(facp, 40));
	r = is_valid_sig4(dsdt, ACPI_SIG('D', 'S', 'D', 'T'));

	if(!r) {

		return false;
	}

	acpi_table.dsdt = dsdt;

	return true;
}

extern void acpi_interrupt_handler();

void do_acpi_interrupt_handler() {

	trace();

	apic_eoi();
}


//ACPI初期化 AMLには触れない
void acpi_init(void) {

	unsigned long r;
	bool result;

	memset(&acpi_table, 0x00, sizeof(struct acpi_table));

	//RSDPを見つける
	r = find_RSDP();
	if(!r) {

		kprintf("ERROR! can not find RSDP.\n");
		trace();
		return;
	}
	kprintf("RSDP = %p value = %p\n", r, *(unsigned long*) (r));

	acpi_table.rsdp = r;
	acpi_table.rsdt = PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(ACPI_R32(r, 16));

	kprintf("RSDT = %p\n", acpi_table.rsdt);

	acpi_table.xsdt = PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(ACPI_R64(r, 24));

	if(acpi_table.xsdt) {

		result = read_xsdt(acpi_table.xsdt);
		if(!result) {
			kprintf("ERROR! can not read XSDT.\n");
			return;
		}
	}

	result = read_rsdt(acpi_table.rsdt);
	if(!result) {

		kprintf("ERROR! can not read RSDT.\n");
		return;
	}

	r = find_acpi_description_entry(ACPI_SIG('F', 'A', 'C', 'P'));

	if(!r) {

		kprintf("ERROR! can not read fACP.\n");
		return;
	}


	acpi_table.facp = r;
	result = read_facp(acpi_table.facp);

}

//ACPICAスタート
void acpica_start(void) {

	ACPI_STATUS status;
	ACPI_OBJECT obj;
	ACPI_OBJECT_LIST pic_args = { 1, &obj };


	status = AcpiInitializeSubsystem();
	if(ACPI_FAILURE(status)) {
		kprintf("ERROR! AcpiInitializetionSubSystem.\n");
		return;
	}

	status = AcpiInitializeTables(NULL, 4, 0);
	if(ACPI_FAILURE(status)) {
		kprintf("ERROR! AcpiInitializetionTables.\n");
		return;
	}
	STOP;

	status = AcpiLoadTables();
	if(ACPI_FAILURE(status)) {
		kprintf("ERROR! AcpiLoadTables.\n");
		return;
	}

	status = AcpiEnableSubsystem(ACPI_FULL_INITIALIZATION);

	if(ACPI_FAILURE(status)) {
		kprintf("ERROR! AcpiEnableSubsystem.\n");
		return;
	}

	status = AcpiInitializeObjects(ACPI_FULL_INITIALIZATION);

	if(ACPI_FAILURE(status)) {
		kprintf("ERROR! AcpiInitializeObjects.\n");
		return;
	}

	obj.Type = ACPI_TYPE_INTEGER;
	obj.Integer.Value = 1;

	status = AcpiEvaluateObject(NULL, "\\_PIC", &pic_args, NULL);

	if(ACPI_FAILURE(status)) {
		kprintf("ERROR! AcpiEvaluateObject.\n");
		return;
	}

}

//OSを初期化する 特にする処理は無い
ACPI_STATUS AcpiOsInitialize(void) {

	return AE_OK ;
}

//特にする処理は無い
ACPI_STATUS AcpiOsTerminate(void) {

	trace();
	return AE_OK ;
}

//ACPIテーブルのアドレスを返す
ACPI_PHYSICAL_ADDRESS AcpiOsGetRootPointer(void) {

	return acpi_table.rsdp;
}

//前定義されていたものをオーバーライドする
ACPI_STATUS AcpiOsPredefinedOverride(const ACPI_PREDEFINED_NAMES *init, ACPI_STRING *new_val) {

	*new_val = NULL;
	return AE_OK ;
}

//ACPIテーブルをオーバーライドする
ACPI_STATUS AcpiOsTableOverride(ACPI_TABLE_HEADER *exist, ACPI_TABLE_HEADER **n) {

	*n = NULL;	//オーバーライドしない
	return AE_OK ;
}

void *AcpiOsMapMemory(ACPI_PHYSICAL_ADDRESS paddr, ACPI_SIZE l) {

	return (void*) (paddr);

}

void AcpiOsUnmapMemory(void *la, ACPI_SIZE l) {

}

//メモリ割り当て
void *AcpiOsAllocate(ACPI_SIZE size) {
	//kprintf("AcpiOsAllocate size = %u\n", size);
	return kmalloc(size, 0);
}

void AcpiOsFree(void *mem) {

//	kprintf("free %p\n", mem);
}

ACPI_THREAD_ID AcpiOsGetThreadId(void) {

	return 1;
}

ACPI_STATUS AcpiOsExecute(ACPI_EXECUTE_TYPE t, ACPI_OSD_EXEC_CALLBACK cb, void *ctxt) {

	trace();
	return AE_OK ;
}

void AcpiOsSleep(ACPI_INTEGER msec) {

	trace();
}

void AcpiOsStall(UINT32 usec) {

	trace();
}

ACPI_STATUS AcpiOsCreateSemaphore(UINT32 max, UINT32 init, ACPI_HANDLE *r) {

	return AE_OK ;
}

ACPI_STATUS AcpiOsDeleteSemaphore(ACPI_HANDLE h) {

	return AE_OK ;
}

ACPI_STATUS AcpiOsWaitSemaphore(ACPI_HANDLE h, UINT32 u, UINT16 to) {

	return AE_OK ;
}

ACPI_STATUS AcpiOsSignalSemaphore(ACPI_SEMAPHORE h, UINT32 u) {

	return AE_OK;
}

ACPI_STATUS AcpiOsCreateLock(ACPI_HANDLE *o) {

	return AE_OK ;
}

void AcpiOsDeleteLock(ACPI_HANDLE h) {

}

ACPI_CPU_FLAGS AcpiOsAcquireLock(ACPI_HANDLE h) {

	return 0;
}

void AcpiOsReleaseLock(ACPI_HANDLE h, ACPI_CPU_FLAGS f) {

}

ACPI_STATUS AcpiOsInstallInterruptHandler(UINT32 intn, ACPI_OSD_HANDLER h, void *ctxt) {


	kprintf("install interrupt: %d\n", intn);
	set_intr_gate(intn + 0x20, acpi_interrupt_handler);
	io_apic_set_redirect(intn, 0, intn + 0x20);
	return AE_OK;
}

ACPI_STATUS AcpiOsRemoveInterruptHandler(UINT32 intn, ACPI_OSD_HANDLER h) {

	trace();
	return AE_OK ;
}

ACPI_STATUS AcpiOsReadMemory(ACPI_PHYSICAL_ADDRESS a, UINT64 *val, UINT32 wid) {

	kprintf("Acpi %p\n", a);
	switch(wid) {
	case 32:
		*val = mmio_read32(a);
		break;
	case 16:
		*val = mmio_read16(a);
		break;
	case 8:
		*val = mmio_read8(a);
		break;
	default:
		break;
	}
	return AE_OK ;
}

ACPI_STATUS AcpiOsWriteMemory(ACPI_PHYSICAL_ADDRESS a, UINT64 val, UINT32 wid) {

	switch(wid) {
	case 32:
		mmio_write32(a, val);
		break;
	case 16:
		mmio_write16(a, val);
		break;
	case 8:
		mmio_write8(a, val);
		break;
	default:
		break;
	}
	return AE_OK ;
}

ACPI_STATUS AcpiOsReadPort(ACPI_IO_ADDRESS addr, UINT32 *val, UINT32 wid) {

	switch(wid) {
	case 32:
		*val = inl(addr);
		break;
	case 16:
		*val = inw(addr);
		break;
	case 8:
		*val = inb(addr);
		break;
	}
	return AE_OK ;
}

ACPI_STATUS AcpiOsWritePort(ACPI_IO_ADDRESS addr, UINT32 val, UINT32 wid) {

	switch(wid) {
	case 32:
		outl(addr, val);
		break;
	case 16:
		outw(addr, val);
		break;
	case 8:
		outb(addr, val);
		break;
	}
	return AE_OK ;
}

ACPI_STATUS AcpiOsReadPciConfiguration(ACPI_PCI_ID *id, UINT32 reg, UINT64 *val, UINT32 wid) {

	/*unsigned int b = id->Bus;
	unsigned int d = id->Device;
	unsigned int f = id->Function;

	switch(wid) {
	case 32:
		*(UINT32*) val = pci_conf_read32_bdf(&pci_root0, b, d, f, reg);
		break;
	case 16:
		*(UINT16*) val = pci_conf_read16_bdf(&pci_root0, b, d, f, reg);
		break;
	case 8:
		*(UINT8*) val = pci_conf_read8_bdf(&pci_root0, b, d, f, reg);
		break;
	case 64:
		kprintf("AcpiOsReadPciConfiguration 64 is not implemented.\n");
		break;
	}*/

	//kprintf("read config %x = %x\n", reg, *(UINT32*)val);

	return AE_OK ;
}

ACPI_STATUS AcpiOsWritePciConfiguration(ACPI_PCI_ID *id, UINT32 reg, UINT64 val, UINT32 wid) {

	//trace();
	return AE_OK ;
}

void AcpiOsPrintf(const char *format, ...) {

	/*va_list list;
	va_start(list, format);
	vprintf(format, list);
	va_end(list);*/

}
/*
 void AcpiOsVprintf(const char *format, va_list args) {



 }*/

UINT64 AcpiOsGetTimer(void) {

	trace();
	return 0;
}

ACPI_STATUS AcpiOsSignal(UINT32 func, void *i) {

	trace();
	return AE_OK ;
}

void AcpiOsDerivePciId(ACPI_HANDLE rhandle, ACPI_HANDLE chandle, ACPI_PCI_ID **pic_id) {

	trace();
}

ACPI_STATUS AcpiOsValidateInterface(char *iface) {

	trace();
	return AE_OK ;
}

void AcpiOsWaitEventsComplete(void) {

	trace();
}

ACPI_STATUS AcpiOsPhysicalTableOverride(ACPI_TABLE_HEADER *ExistingTable, ACPI_PHYSICAL_ADDRESS *NewAddress, UINT32 *NewTableLength) {

	*NewAddress = (unsigned long) NULL;
	//kprintf("Exist %4s\n", ExistingTable->Signature);

	return AE_OK ;
}

