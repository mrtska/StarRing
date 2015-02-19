/*
[Module ahci.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php

Created on: 2015/02/19
*/

#pragma once


#include <system.h>
#include <drivers/pci.h>
#include <drivers/storage.h>


#define AHCI_PCI_REGISTER_AHCI_BASE_ADDR 0x24



#define AHCI_BASE_ADDRESS_MASK 0xFFFF0000
//AHCIベースアドレスレジスタ
union ahci_abar {

	struct {

		unsigned rte : 1;			//リソースタイプインジケーター
		unsigned tp : 2;			//タイプ
		unsigned pf : 1;			//プリフェッチ可能
		unsigned char _reserved;
		unsigned ba : 20;			//ベースアドレス
	} __attribute__((packed)) bf;

	unsigned int data;

} __attribute__((packed));




//AHCIデバイス
struct ahci_device {

	//ストレージデバイス
	struct storage_device *storage_device;

	//AHCIベースアドレスレジスタ
	union ahci_abar abar;

	//AHCIベースアドレス
	unsigned long ahci_base_address;




};


void ahci_init(struct storage_device *device);





