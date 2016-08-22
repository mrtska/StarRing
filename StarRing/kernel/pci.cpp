
#include <system.h>
#include <pci.h>
#include <slaballocator.h>


class pci pci;


//PCIデバイスを探す こっちのメソッドは無効なパラメータを指定したらダメ
void pci::search_pci_device(unsigned char bus, unsigned char device, unsigned char function) {


	auto conf = read_configuration(bus, device, function, PCI_REGISTER_VENDOR_DEVICE_ID);

	if(conf == 0xFFFFFFFF) {

		return;
	}

	struct pci_device *pcidev = new struct pci_device();

	pcidev->bus = bus;
	pcidev->dev = device;
	pcidev->function = function;

	pcidev->vendor = conf & 0xFFFF;
	pcidev->device = (conf >> 16) & 0xFFFF;

	auto classcode = read_configuration(bus, device, function, PCI_REGISTER_REVISION_CLASSCODE);

	pcidev->classcode = (classcode >> 24) & 0xFF;

	this->pci_device_list.add_list_tail(&pcidev->list);
}


void pci::search_pci_device() {


	for(int bus = 0; bus < 256; bus++) {

		for(int device = 0; device < 32; device++) {

			auto conf = read_configuration(bus, device, 0, PCI_REGISTER_VENDOR_DEVICE_ID);

			if(conf != 0xFFFFFFFF) {

				search_pci_device(bus, device, 0);

				auto header = read_configuration(bus, device, 0, PCI_REGISTER_BIST_HEADER_TYPE);

				//複数機能あり
				if(header & 0x00800000) {

					for(int func = 1; func < 8; func++) {

						search_pci_device(bus, device, func);
					}
				}
			}
		}
	}
}


void pci::pci_init() {


	search_pci_device();


/*


	this->pci_device_list.foreach(offsetof(&pci_device::list), [](struct pci_device *dev) {


		kprintf("vendor 0x%X, device 0x%X, bus %u, dev %u, func %u, class %X\n", dev->vendor, dev->device, dev->bus, dev->dev, dev->function, dev->classcode);

	});




*/


}


unsigned int pci::read_configuration(unsigned short bus, unsigned short device, unsigned short feature, unsigned int reg) {

	unsigned int ctrl = 0;
	ctrl |= reg;
	ctrl |= (feature << 8);
	ctrl |= (device << 11);
	ctrl |= (bus << 16);
	ctrl |= (1 << 31);
	outl(PCI_CONFIG_ADDRESS, ctrl);

	return inl(PCI_CONFIG_DATA);

}

