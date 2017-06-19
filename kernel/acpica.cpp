
#include <system.h>
#include <acpica.h>



#include <idt.h>
#include <slaballocator.h>
#include <pci.h>

//ACPICAのヘッダ
extern "C" {

#include "../acpica/include/acpi.h"

}


INTERRUPT_HANDLER void acpi_interrupt_handler();



//ACPICAスタート
void acpica_start() {

	//ACPICAが返す関数のステータス
	ACPI_STATUS status;




	status = AcpiInitializeSubsystem();


	if(ACPI_FAILURE(status)) {
		kprintf("ERROR! AcpiInitializetionSubSystem.\n");
		return;
	}

	status = AcpiInitializeTables(nullptr, 16, false);
	if(ACPI_FAILURE(status)) {
		kprintf("ERROR! AcpiInitializetionTables.\n");
		return;
	}

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


/*	ACPI_OBJECT obj;
	ACPI_OBJECT_LIST pic_args = { 1, &obj };
	obj.Type = ACPI_TYPE_INTEGER;
	obj.Integer.Value = 1;


	status = AcpiEvaluateObject(nullptr, const_cast<char*>("\\_PIC"), &pic_args, nullptr);

	if(ACPI_FAILURE(status)) {
		kprintf("ERROR! AcpiEvaluateObject.\n");
		return;
	}*/
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

	return physical_memory.get_acpi_rsdp_address();
}

//前定義されていたものをオーバーライドする
ACPI_STATUS AcpiOsPredefinedOverride(const ACPI_PREDEFINED_NAMES *init, ACPI_STRING *new_val) {

	*new_val = nullptr;
	return AE_OK;
}

//ACPIテーブルをオーバーライドする
ACPI_STATUS AcpiOsTableOverride(ACPI_TABLE_HEADER *exist, ACPI_TABLE_HEADER **n) {

	*n = nullptr;	//オーバーライドしない
	return AE_OK;
}

//物理アドレスを仮想アドレスに変換してあげる
void *AcpiOsMapMemory(ACPI_PHYSICAL_ADDRESS paddr, ACPI_SIZE l) {

	return translate_physical_address(paddr);
}

//仮想アドレスとサイズが帰ってくるんだけどそれをどうしろと(´・ω・｀)
void AcpiOsUnmapMemory(void *la, ACPI_SIZE l) {


	//trace();
}

//メモリ割り当て
void *AcpiOsAllocate(ACPI_SIZE size) {


	void *a = slab_allocator.kmalloc(size);
	//kprintf("allocating %p\n", a);
	return a;
}

void AcpiOsFree(void *mem) {

	//kprintf("freeing %p\n", mem);
	return slab_allocator.kfree(mem);
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


	trace();
	kprintf("install interrupt: %p\n", h);
	idt.register_interrupt(intn, acpi_interrupt_handler);
/*	set_intr_gate(intn + 0x20, acpi_interrupt_handler);
	io_apic_set_redirect(intn, 0, intn + 0x20);*/
	return AE_OK;
}

ACPI_STATUS AcpiOsRemoveInterruptHandler(UINT32 intn, ACPI_OSD_HANDLER h) {

	trace();
	return AE_OK ;
}

ACPI_STATUS AcpiOsReadMemory(ACPI_PHYSICAL_ADDRESS a, UINT64 *val, UINT32 wid) {

/*	kprintf("Acpi %p\n", a);
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
	}*/
	trace();
	return AE_OK ;
}

ACPI_STATUS AcpiOsWriteMemory(ACPI_PHYSICAL_ADDRESS a, UINT64 val, UINT32 wid) {

/*	switch(wid) {
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
	}*/
	trace();
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



	switch(wid) {
	case 32:
		*val = pci.read_configuration(id->Bus, id->Device, id->Function, reg) & 0xFFFFFFFF;
		break;
	case 16:
		*val = pci.read_configuration(id->Bus, id->Device, id->Function, reg) & 0xFFFF;
		break;
	case 8:
		*val = pci.read_configuration(id->Bus, id->Device, id->Function, reg) & 0xFF;
		break;
	case 64:
		kprintf("AcpiOsReadPciConfiguration 64 is not implemented.\n");
		break;
	}

	return AE_OK ;
}

ACPI_STATUS AcpiOsWritePciConfiguration(ACPI_PCI_ID *id, UINT32 reg, UINT64 val, UINT32 wid) {

	//trace();
	return AE_OK ;
}

void AcpiOsPrintf(const char *format, ...) {

	va_list list;
	va_start(list, format);
	vprintf(format, list);
	va_end(list);

}

void AcpiOsVprintf(const char *format, va_list args) {

	vprintf(format, args);
}

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

	*NewAddress = (unsigned long) nullptr;
	return AE_OK;
}

BOOLEAN AcpiOsReadable(void *Pointer, ACPI_SIZE Length) {

	trace();
	return true;
}


ACPI_STATUS AcpiOsGetLine(char *Buffer, UINT32 BufferLength, UINT32 *BytesRead) {

	trace();
	return AE_OK;
}




