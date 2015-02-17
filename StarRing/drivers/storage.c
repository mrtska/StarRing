/*
[Module storage.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php

Created on: 2015/02/17
*/


#include <system.h>
#include <drivers/storage.h>
#include <drivers/pci.h>

void scan_storage_device(void) {


	pci_find_device_by_class_code(PCI_CLASS_CODE_BUS_MASTER);

}




