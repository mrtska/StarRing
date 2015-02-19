/*
[Module storage.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php

Created on: 2015/02/17
*/


#include <system.h>
#include <mem/alloc.h>

#include <drivers/storage.h>
#include <drivers/pci.h>
#include <drivers/ata.h>
#include <drivers/ahci.h>
#include <list.h>



static struct storage_device storage[0x10];



//ATA,SATA,SCSI,USBなどのPCIストレージデバイスを認識する
void scan_storage_device(void) {

	int index = 0;

	struct list_head *p;
	list_for_each(p, get_pci_device_list()) {

		struct pci_device *device = list_entry(p, struct pci_device, list);

		struct pci_classcode class;
		pci_pack_class_code(device->class_code, &class);

		if(class.base_class != 1) {

			continue;
		}


		struct storage_device *storage_device = &storage[index++];
		storage_device->pci_device = device;
		storage_device->storage_buffer = alloc_memory_block();

		switch(class.sub_class) {

		//PATA
		case 1:

			//PATAドライバ初期化
			storage_device->type = STORAGE_TYPE_PATA;
			ata_init(storage_device);
			break;
		//AHCI
		case 6:

			//AHCIドライバ初期化
			storage_device->type = STORAGE_TYPE_AHCI;
			ahci_init(storage_device);
			break;
		default:

			kprintf("storage.c %X %X %X\n", class.base_class, class.sub_class, class.device);
			STOP;
			break;
		}




	}
}




