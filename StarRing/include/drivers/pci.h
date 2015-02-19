/*
[Module pci.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/10/13
*/

#pragma once



#include <acpi.h>
#include <slab.h>

#include <system.h>
#include <mm.h>
#include <list.h>




#define PCI_CONFIG_ADDRESS_R				0x0CF8
#define PCI_CONFIG_DATA_R					0x0CFC

#define PCI_REGISTER_VENDOR_ID				0x00
#define PCI_REGISTER_DEVICE_ID				0x02
#define PCI_REGISTER_COMMAND				0x04
#define PCI_REGISTER_STATUS					0x06
#define PCI_REGISTER_REVISION				0x08
#define PCI_REGISTER_CLASSCODE				0x09
#define PCI_REGISTER_CACHE_LINE_SIZE		0x0C
#define PCI_REGISTER_MASTER_LAYTENCY_TIMER	0x0D
#define PCI_REGISTER_HEADER_TYPE			0x0E
#define PCI_REGISTER_BIST					0x0F
#define PCI_REGISTER_BAR0					0x10
#define PCI_REGISTER_BAR1					0x14
#define PCI_REGISTER_BAR2					0x18
#define PCI_REGISTER_BAR3					0x1C
#define PCI_REGISTER_BAR4					0x20
#define PCI_REGISTER_BAR5					0x24
#define PCI_REGISTER_CARDBUS_CIS			0x28
#define PCI_REGISTER_SUB_SYSTEM_VENDOR_ID	0x2C
#define PCI_REGISTER_SUB_SYSTEM_ID			0x2E
#define PCI_REGISTER_EXTEND_ROM_ADDRESS		0x30
#define PCI_REGISTER_CAPABILITIES_POINTER	0x34
#define PCI_REGISTER_INTERRUPT_INFORMATION	0x3C
#define PCI_REGISTER_MINIMUM_GRANT			0x3E
#define PCI_REGISTER_MAXIMUM_LATENCY		0x3F



struct pci_config {

	unsigned reserved : 2;
	unsigned register_addr : 6;
	unsigned feature_num : 3;
	unsigned device_num : 5;
	unsigned char bus_num;
	unsigned reserved2 : 7;
	unsigned enable : 1;
} __attribute__((packed));

struct pci_classcode {

	unsigned char base_class;
	unsigned char sub_class;
	unsigned char device;
};



//PCIデバイス情報
//マルチファンクションデバイスの場合は別のPCIデバイスとして認識される
struct pci_device {

	//PCIデバイスのリスト
	struct list_head list;

	unsigned char device_num;	//デバイスナンバー
	unsigned short vender_id;	//ベンダーID
	unsigned short device_id;	//デバイスID
	unsigned char func_num;		//機能番号
	unsigned char bus;			//バス番号
	unsigned int class_code;	//クラスコード
	char *vender_name;			//ベンダー
	char *device_name;			//デバイス
};





static __inline__ void pci_pack_class_code(unsigned int class_code, struct pci_classcode *class) {

	class->base_class = (class_code & 0xFF0000) >> 16;
	class->sub_class = (class_code & 0xFF00) >> 8;
	class->device = class_code & 0xFF;
}


//PCIテーブルの初期化
void pci_init();

struct pci_device *pci_find_device(unsigned short vendor, unsigned short device);

struct pci_device *pci_find_device_by_class_code(unsigned int class_code);

int send_config(unsigned char register_addr, unsigned char device_num, unsigned char feature_num, unsigned char bus);

unsigned int pci_conf_read(struct pci_device *d, int reg);

void pci_conf_write(struct pci_device *device, int reg, unsigned int value);

struct list_head *get_pci_device_list(void);










