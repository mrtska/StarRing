/*
[Module storage.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php

Created on: 2015/02/17
*/

#pragma once

typedef void (*read_storage) (unsigned long lba, unsigned char* buffer, unsigned long sector);
typedef void (*write_storage) (unsigned long lba, unsigned char* buffer, unsigned long sector);




struct storage_operations {

	read_storage pio_read;
	read_storage dma_read;
	write_storage pio_write;
	write_storage dma_write;
};

struct storage_device {


	unsigned int classcode;			//PCIクラスコード
	unsigned int num;				//番号

	struct storage_operations *opts;//ストレージ処理


	//ストレージごとにバッファとなるメモリを与える
	void *storage_buffer;
};













void scan_storage_device(void);


















