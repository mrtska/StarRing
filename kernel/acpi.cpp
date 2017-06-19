
#include <system.h>
#include <stdio.h>
#include <acpi.h>
#include <acpica.h>
#include <idt.h>




//ACPICAのヘッダ
extern "C" {

#include "../acpica/include/acpi.h"

}

#include <slaballocator.h>

//ACPI管理クラス
class acpi acpi;



ACPI_STATUS get_device_callback(ACPI_HANDLE Object, UINT32 NestingLevel, void *Context, void **ReturnValue) {


	ACPI_BUFFER buffer;
	buffer.Length = ACPI_ALLOCATE_BUFFER;





	AcpiGetName(Object, ACPI_FULL_PATHNAME, &buffer);

	//kprintf("device path %s\n", buffer.Pointer);


	return AE_OK;
}


void acpi::acpi_init() {


	acpica_start();

	void *returnvalue;

	AcpiGetDevices(nullptr, get_device_callback, nullptr, &returnvalue);

}





INTERRUPT_HANDLER void exec_acpi_interrupt_handler() {


	trace();
}



