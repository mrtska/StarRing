/*
 * pci.h
 *
 *  Created on: 2014/08/18
 *      Author: StarRing
 */

#pragma once



/*

#include <mm.h>

#define PCI_PRIMARY_BUS 0x18


#define PCI_CLS_MULTIMEDIA 4
#define PCI_SUBCLS_HD_AUDIO 3

#define PCI_CLS_BRIDGE 6
#define PCI_SUBCLS_HOST_BRIDGE 0
#define PCI_SUBCLS_PCI_BRIDGE 4

#define PCI_CLS_SERIAL_BUS 0x0c
#define PCI_SUBCLS_USB_HOST_CONTROLLER 0x03

#define PCI_PI_UHCI 0x00
#define PCI_PI_OHCI 0x10
#define PCI_PI_EHCI 0x20
#define PCI_PI_XHCI 0x30

#define PCI_VENDOR_REALTEK 0x10EC

#define BDF_BUS(x) (((x)>>20)&0x7F)
#define BDF_DEV(x) (((x)>>15)&0x1F)
#define BDF_FN(x) (((x)>>12)&0x07)
#define BDF_DEVFN(x) (((x)>>12)&0xFF)






extern struct pci_root pci_root0;


struct pci_device {

	unsigned long conf_addr;
	unsigned long bus_dev_fn;
	unsigned short vendor_id;
	unsigned short device_id;
	unsigned char scc;
	unsigned char bcc;
	unsigned char pi;
};

struct pci_irq_table {

	int addr;
	int irq[4];
};

struct pci_bridge {

	int devid;
	int busno;
	int num_irq_table;
	struct pci_irq_table *irq_table;
};

struct pci_root {

	unsigned long mcfg_addr;
	int num_dev;
	struct pci_device *devices;
	int num_bridge;
	struct pci_bridge *bridges;
	int nr_bus;


#define PCITREE_SIZEOF_NODE 2
#define PCITREE_OFFSET_BRIDGEID 0
#define PCITREE_OFFSET_NUM_CHILD 1
	int *tree;
};



//PCIの列挙
void lspci(struct pci_root *pci);
void lspci_tree(struct pci_root *pci);

*/
