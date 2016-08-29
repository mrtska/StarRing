
#pragma once

#include <drivers/storage.h>













class ahci_storage_device : public storage_device {


public:

	virtual bool verify_pci_id(struct pci_device *device);

	virtual void storage_device_init(struct pci_device *device);













};

















