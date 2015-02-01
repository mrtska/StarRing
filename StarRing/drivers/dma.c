/*
[Module dma.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php

Created on: 2014/10/24
 */


#include <drivers/dma.h>
#include <drivers/pci.h>
#include <mm.h>
#include <page/page.h>

//PCIバスマスターのベースIOポート
int dma_base_port;

//PRDTのポインタ
unsigned long prdt_pointer;

void dma_init(void) {

	//PCIバスマスタのクラスコードを持つデバイスを探す
	struct pci_device *device = pci_find_device_by_class_code(PCI_CLASS_CODE_BUS_MASTER);

	//ポートを0xE000に指定する
	pci_conf_write(device, PCI_REGISTER_BAR4, 0xE000);

	//一応BAR4からポートを取得
	int port = pci_conf_read(device, PCI_REGISTER_BAR4) & ~0x3;	//PCIバスマスターのBAR4からDMAのポートを取得する
	//kprintf("port = %X\n", port);

	//ポートを設定
	dma_base_port = port;

	//PRDTの仮想アドレスを設定
	prdt_pointer = PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(0x500);

	//PRDTの物理アドレスを設定
	outl(port + DMA_PRIMARY_PRDT_R, 0x500);

}
