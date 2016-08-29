
#include <system.h>
#include <drivers/fb/vbox.h>
#include <pci.h>
#include <string.h>

class list<class frame_buffer> frame_buffer::frame_buffer_list;




class vbox_frame_buffer vbox_frame_buffer;


bool vbox_frame_buffer::verify_pci_id(struct pci_device *pci_device) {

	if(pci_device->vendor == VBOX_GFX_VENDOR_ID && pci_device->device == VBOX_GFX_DEVICE_ID) {

		return true;
	}
	return false;
}


void vbox_frame_buffer::frame_buffer_init(struct pci_device *pci_device) {

	trace();

	this->pci_device = pci_device;

	kprintf("VBOX, BAR0 = %X\n", pci.read_configuration(pci_device, PCI_REGISTER_BAR0));
	kprintf("VBOX, BAR1 = %X\n", pci.read_configuration(pci_device, PCI_REGISTER_BAR1));
	kprintf("VBOX, BAR2 = %X\n", pci.read_configuration(pci_device, PCI_REGISTER_BAR2));
	kprintf("VBOX, BAR3 = %X\n", pci.read_configuration(pci_device, PCI_REGISTER_BAR3));


	outw(VBOX_GFX_DISPLAY_INDEX, VBOX_GFX_DISPLAY_WIDTH);
	outw(VBOX_GFX_DISPLAY_DATA, 640);
	outw(VBOX_GFX_DISPLAY_INDEX, VBOX_GFX_DISPLAY_HEIGHT);
	outw(VBOX_GFX_DISPLAY_DATA, 360);


	//outw(VBOX_GFX_DISPLAY_INDEX, VBOX_GFX_DISPLAY_ENABLE);
	//outw(VBOX_GFX_DISPLAY_DATA, 1);


	/*for(int x = 1; x <= 640; x++) {

		for(int y = 1; y <= 360; y++) {

			for(int i = 1; i <= 4; i++) {

				memset(reinterpret_cast<void*>(0xFFFF8000E0000000 +  x * y * i), 0xAA, 1);
			}

		}
	}*/




}






























