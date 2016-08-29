
#pragma once


#include <list.h>


#define PCI_CONFIG_ADDRESS				0x0CF8
#define PCI_CONFIG_DATA					0x0CFC


#define PCI_REGISTER_VENDOR_DEVICE_ID			0x00
#define PCI_REGISTER_COMMAND_STATUS				0x04
#define PCI_REGISTER_REVISION_CLASSCODE			0x08
#define PCI_REGISTER_BIST_HEADER_TYPE			0x0C

#define PCI_REGISTER_BAR0						0x10
#define PCI_REGISTER_BAR1						0x14
#define PCI_REGISTER_BAR2						0x18
#define PCI_REGISTER_BAR3						0x1C
#define PCI_REGISTER_BAR4						0x20
#define PCI_REGISTER_BAR5						0x24




extern class pci pci;

//PCIデバイスの情報とか
struct pci_device {

	class list<struct pci_device> list;

	unsigned char bus;		//バス番号 だいたい0だよね
	unsigned char dev;		//デバイス番号
	unsigned char function;	//機能番号 1つのデバイスに複数の機能があるとうんぬん


	unsigned short vendor;	//ベンダーID
	unsigned short device;	//デバイスID

	unsigned char classcode;	//クラスコード これでこのデバイスがどんなデバイスか大体分かる












};


union pci_device_selector {

	struct {

		unsigned char reserved : 2;
		unsigned char reg : 6;
		unsigned char feature : 3;
		unsigned char device : 5;
		unsigned char bus;
		unsigned char reserved1 : 7;
		unsigned char enable : 1;
	};

	unsigned int data;

} __attribute__((packed));


class pci final {


private:

	class list<struct pci_device> pci_device_list;

	void search_pci_device();
	void search_pci_device(unsigned char bus, unsigned char device, unsigned char function);

public:


	void pci_init();


	unsigned int read_configuration(unsigned short bus, unsigned short device, unsigned short feature, unsigned int reg);
	unsigned int read_configuration(struct pci_device *device, unsigned int reg);

	void write_configuration(unsigned short bus, unsigned short device, unsigned short feature, unsigned int reg, unsigned int value);


	class list<struct pci_device> get_pci_device_list() {

		return this->pci_device_list;
	}




};





























