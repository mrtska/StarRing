/*
[Module ahci.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php

Created on: 2015/02/19
*/

#pragma once



#include <drivers/pci.h>



#define AHCI_PCI_REGISTER_AHCI_BASE_ADDR 0x24

//AHCIベースアドレスレジスタ
struct ahci_abar {

	unsigned rte : 1;			//リソースタイプインジケーター
	unsigned tp : 2;			//タイプ
	unsigned pf : 1;			//プリフェッチ可能
	unsigned char _reserved;

};










