/*
[Module pci.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/08/18
*/

#include <drivers/pci.h>
#include <pci_list.h>
#include <drivers/vbox.h>





//PCIデバイスのリスト
static LIST_HEAD(pci_device_list);


static void store_pci_data(unsigned short vender_id, unsigned short device_id, int device_num, int func_num, int bus, unsigned int class_code, char *vendor_name, char *device_name) {

	struct pci_device *device = kmalloc(sizeof(struct pci_device), 0x8);

	device->vender_id = vender_id;
	device->device_id = device_id;
	device->device_num = device_num;
	device->func_num = func_num;
	device->bus = bus;
	device->vender_name = vendor_name;
	device->device_name = device_name;
	device->class_code = class_code;

	list_add_tail(&device->list, &pci_device_list);
}

void pci_init(void) {



	unsigned short vendor_id;
	unsigned short device_id;
	unsigned int device_num, j, func_num;


	int bus;
	for(bus = 0; bus < 256; bus++) {	//全てのバスを走査

		unsigned int result;
		for(device_num = 0; device_num < 32; device_num++) {	//全てのデバイスを走査

			result = send_config(PCI_REGISTER_VENDOR_ID, device_num, 0, bus);	//ベンダーIDを取得

			//ベンダーIDが0xFFFFFFFFの場合はPCIデバイスが存在しないのでとっとと次へ
			if(result == 0xFFFFFFFF) {

				continue;
			}

			for(func_num = 0; func_num < 8; func_num++) {	//全ての機能番号を走査

				//ヘッダータイプを取得
				unsigned char header = (send_config(PCI_REGISTER_CACHE_LINE_SIZE, device_num, func_num, bus) >> 16) & 0xFF;
				unsigned int class_code = (send_config(PCI_REGISTER_CLASSCODE, device_num, func_num, bus) >> 8) & 0xFFFFFF;

				//クラスコードが0xFFFFFFの場合は存在しないのでとっとと次へ
				if(class_code == 0xFFFFFF) {

					continue;
				}

				vendor_id = result & 0xFFFF;
				device_id = (result >> 16) & 0xFFFF;

				for(j = 0; j < PCI_DEVTABLE_LEN; j++) {

					if(PciDevTable[j].VenId == vendor_id && PciDevTable[j].DevId == device_id) {

						void *p = 0;
						int k;
						for(k = 0; k < PCI_VENTABLE_LEN; k++) {

							if(PciVenTable[k].VenId == vendor_id) {

								p = PciVenTable[k].VenFull;
								break;
							}
						}

						store_pci_data(vendor_id, device_id, device_num, func_num, bus, class_code, p, PciDevTable[j].ChipDesc);	//データを格納
						goto tail;
					}
				}
				kprintf("Unknown PCI Device, Vendor = %X, Device = %X\n", vendor_id, device_id);

tail:
				if(header != 0x80) {	//マルチファンクションでなければ終わり
					break;
				}
			}

		}
	}

	struct list_head *ptr;
	struct pci_device *device;

	list_for_each(ptr, &pci_device_list) {

		device = list_entry(ptr, struct pci_device, list);

		kprintf("venID %X, devID %X, func %d, class %X, bus %d\n", device->vender_id, device->device_id, device->func_num, device->class_code, device->bus);
		//pci_conf_write(device, PCI_REGISTER_COMMAND, 0);
	}



}

//PCIデバイスをベンダーIDとデバイスIDからルックアップする
struct pci_device *pci_find_device(unsigned short vendor_id, unsigned short device_id) {


	struct list_head *ptr;
	struct pci_device *device;

	list_for_each(ptr, &pci_device_list) {

		device = list_entry(ptr, struct pci_device, list);

		if(device->vender_id == vendor_id && device->device_id == device_id) {

			return device;
		}
	}

	return NULL;
}

//PCIデバイスをクラスコードからルックアップする
struct pci_device *pci_find_device_by_class_code(unsigned int class_code) {

	struct list_head *ptr;
	struct pci_device *device;

	list_for_each(ptr, &pci_device_list) {

		device = list_entry(ptr, struct pci_device, list);
		if(device->class_code == class_code) {

			return device;
		}
	}

	return NULL;
}

int send_config(unsigned char register_addr, unsigned char device_num, unsigned char feature_num, unsigned char bus) {

	unsigned int ctrl = 0;
	ctrl |= register_addr;
	ctrl |= (feature_num << 8);
	ctrl |= (device_num << 11);
	ctrl |= (bus << 16);
	ctrl |= (1 << 31);
	outl(PCI_CONFIG_ADDRESS_R, ctrl);

	return inl(PCI_CONFIG_DATA_R);
}

unsigned int pci_conf_read(struct pci_device *device, int reg) {

	unsigned int a = send_config(reg & 0xFFFFFFFC, device->device_num, device->func_num, device->bus);

	switch(reg) {
	case PCI_REGISTER_VENDOR_ID:
		return a & 0xFFFF;
	case PCI_REGISTER_DEVICE_ID:
		return (a >> 16) & 0xFFFF;
	case PCI_REGISTER_COMMAND:
		return a & 0xFFFF;
	case PCI_REGISTER_STATUS:
		return (a >> 16) & 0xFFFF;
	case PCI_REGISTER_REVISION:
		return a & 0xFF;
	case PCI_REGISTER_CLASSCODE:
		return (a >> 8) & 0xFFFFFF;
	case PCI_REGISTER_CACHE_LINE_SIZE:
		return a & 0xFF;
	case PCI_REGISTER_MASTER_LAYTENCY_TIMER:
		return (a >> 8) & 0xFF;
	case PCI_REGISTER_HEADER_TYPE:
		return (a >> 16) & 0xFF;
	case PCI_REGISTER_BIST:
		return (a >> 24) & 0xFF;
	default:	//BAR
		return a;
	}
}


void pci_conf_write(struct pci_device *device, int reg, unsigned int value) {

	send_config(reg & 0xFFFFFFFC, device->device_num, device->func_num, device->bus);

	outl(PCI_CONFIG_DATA_R, value);
}


struct list_head *get_pci_device_list(void) {

	return &pci_device_list;
}

