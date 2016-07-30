/*
[Module vbox.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/09/08
*/

#include <drivers/vbox.h>
#include <internal/stdio.h>
#include <drivers/pci.h>
#include <system.h>

unsigned long vram;


//VirtualBoxグラフィックスアダプター初期化
void vbox_graphics_init(unsigned int x, unsigned int y) {

	//0xA0000から0xBFFFFを有効、垂直、水平同期を有効にする
	outb(VBOX_MISC_W, 0xC3);

	//256KBアクセスを有効にする
	outb(VBOX_SEQUENCER_I, 4);
	outb(VBOX_SEQUENCER_D, 2);

	inb(VBOX_STATUS_0_R);

	outb(VBOX_ATTRIBUTE_I, 0);

	//etc
	outb(VBOX_CRTCTRL_I, 0x11);
	outb(VBOX_CRTCTRL_D, 0);

	//VGAをリセット
	outb(VBOX_SEQUENCER_I, 0);
	outb(VBOX_SEQUENCER_D, 1);


	outw(VBOX_DISPLAY_I, 0);
	outw(VBOX_DISPLAY_D, 0xB0C0);

	outw(VBOX_DISPLAY_I, 4);
	outw(VBOX_DISPLAY_D, 0);

	outw(VBOX_DISPLAY_I, 1);
	outw(VBOX_DISPLAY_D, x);

	outw(VBOX_DISPLAY_I, 2);
	outw(VBOX_DISPLAY_D, y);

	outw(VBOX_DISPLAY_I, 3);
	outw(VBOX_DISPLAY_D, 32);

	outw(VBOX_DISPLAY_I, 5);
	outw(VBOX_DISPLAY_D, 0);

	outw(VBOX_DISPLAY_I, 6);
	outw(VBOX_DISPLAY_D, x);

	outw(VBOX_DISPLAY_I, 7);
	outw(VBOX_DISPLAY_D, y);

	outw(VBOX_DISPLAY_I, 4);
	outw(VBOX_DISPLAY_D, 1);


	//リセット
	outb(VBOX_SEQUENCER_I, 0);
	outb(VBOX_SEQUENCER_D, 3);

	outb(VBOX_ATTRIBUTE_I, 0x20);


	//グラフィックスモード有効
	outb(VBOX_GRAPHICS_I, 6);
	outb(VBOX_GRAPHICS_D, 5);


	kprintf("VBOX, BAR0 = %X\n");
	kprintf("VBOX, BAR1 = %X\n", pci_conf_read(pci_find_device(VBOX_GRAPHICS_ADAPTER_VENDOR_ID, VBOX_GRAPHICS_ADAPTER_DEVICE_ID), PCI_REGISTER_BAR1));
	kprintf("VBOX, BAR2 = %X\n", pci_conf_read(pci_find_device(VBOX_GRAPHICS_ADAPTER_VENDOR_ID, VBOX_GRAPHICS_ADAPTER_DEVICE_ID), PCI_REGISTER_BAR2));
	kprintf("VBOX, BAR3 = %X\n", pci_conf_read(pci_find_device(VBOX_GRAPHICS_ADAPTER_VENDOR_ID, VBOX_GRAPHICS_ADAPTER_DEVICE_ID), PCI_REGISTER_BAR3));
	kprintf("VBOX, BAR4 = %X\n", pci_conf_read(pci_find_device(VBOX_GRAPHICS_ADAPTER_VENDOR_ID, VBOX_GRAPHICS_ADAPTER_DEVICE_ID), PCI_REGISTER_BAR4));

	vram = pci_conf_read(pci_find_device(VBOX_GRAPHICS_ADAPTER_VENDOR_ID, VBOX_GRAPHICS_ADAPTER_DEVICE_ID), PCI_REGISTER_BAR0) & ~0xF;

}

