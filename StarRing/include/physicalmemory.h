
#pragma once

#include <system.h>



#define MEMORY_BLOCK_SIZE 0x200000



//物理メモリ管理
extern class physical_memory physical_memory;



//メモリマップを取得
struct memory_map_entry {

	unsigned long address;
	unsigned long length;

	unsigned char type;


};



//物理的なメモリを管理する
class physical_memory final {


private:

	//コマンドライン
	char cmdline[256];

	//ブートローダの名前 GRUB
	char bootloader_name[256];

	//メモリの容量 byte単位
	unsigned long lower_memory = 0;
	unsigned long upper_memory = 0;

	struct memory_map_entry mmap_entry[128];


	//物理メモリが空いている場所を管理する 128GBまで
	unsigned long physical_memory_map[1000];

	unsigned long max_memory_address = 0;


	unsigned long acpi_rsdp_address = 0;



	//bootloaderからのいろいろな情報をパースする
	void parse_multiboot_header(void *addr);


public:


	physical_memory();

	void physical_memory_init(unsigned long addr);


	char *get_cmdline() {

		return this->cmdline;
	}
	char *get_bootloader_name() {

		return this->bootloader_name;
	}

	unsigned long get_lower_memory() {

		return this->lower_memory;
	}
	unsigned long get_upper_memory() {

		return this->upper_memory;
	}

	unsigned long get_max_memory_address() {

		return this->max_memory_address;
	}

	unsigned long get_acpi_rsdp_address() {

		return this->acpi_rsdp_address;
	}

	unsigned long alloc_physical_memory();

	void release_physical_memory(unsigned long addr);


	void set_physical_memory_bit(unsigned long addr, unsigned long len);





};



























