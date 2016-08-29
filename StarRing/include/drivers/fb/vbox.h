
#pragma once



#include <drivers/fb.h>



//PCIデバイスのID
#define VBOX_GFX_VENDOR_ID			0x80EE
#define VBOX_GFX_DEVICE_ID			0xBEEF


#define VBOX_GFX_DISPLAY_INDEX		0x01CE
#define VBOX_GFX_DISPLAY_DATA		0x01CF


#define VBOX_GFX_DISPLAY_WIDTH		0x1
#define VBOX_GFX_DISPLAY_HEIGHT		0x2
#define VBOX_GFX_DISPLAY_BPP		0x3
#define VBOX_GFX_DISPLAY_ENABLE		0x4


#define VBOX_GFX_DISPLAY_X_OFFSET	0x8
#define VBOX_GFX_DISPLAY_Y_OFFSET	0x9








class vbox_frame_buffer : public frame_buffer {



private:




public:


	virtual bool verify_pci_id(struct pci_device *pci_device);

	virtual void frame_buffer_init(struct pci_device *pci_device);


	vbox_frame_buffer() {

	}




};
















