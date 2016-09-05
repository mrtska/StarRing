
#pragma once



#include <pci.h>











class storage_device {





protected:





public:


	virtual bool verify_pci_id(struct pci_device *device) = 0;




	virtual void storage_device_init(struct pci_device *device) = 0;







};


















