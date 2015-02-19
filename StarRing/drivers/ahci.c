/*
[Module ahci.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php

Created on: 2015/02/18
*/


#include <drivers/ahci.h>




void ahci_init(struct storage_device *device) {

	struct ahci_device *ahci_device = kmalloc(sizeof(struct ahci_device), 0);

	ahci_device->storage_device = device;

	//PCIコンフィグレーションレジスタからベースアドレスをロード
	union ahci_abar *abar = &ahci_device->abar;
	abar->data = pci_conf_read(device->pci_device, AHCI_PCI_REGISTER_AHCI_BASE_ADDR);

	//AHCIベースアドレス
	ahci_device->ahci_base_address = abar->data & AHCI_BASE_ADDRESS_MASK;




	kprintf("RTE %d, TP %d, PF %d, BA %X\n", abar->bf.rte, abar->bf.tp, abar->bf.pf, abar->bf.ba);
	kprintf("%X\n", abar->data);



	device->device = ahci_device;
	STOP;
}
































