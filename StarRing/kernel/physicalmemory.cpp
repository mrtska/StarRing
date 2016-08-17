
#include <system.h>
#include <multiboot2.h>
#include <physicalmemory.h>
#include <stdio.h>
#include <string.h>

//物理メモリ管理
class physical_memory physical_memory;

physical_memory::physical_memory() {

	memset(this->physical_memory_map, 0, sizeof(unsigned long) * 1000);

}
/*
static const char *get_mmap_type(unsigned int type) {

	switch(type) {
	case 1:
		return "AVAILABLE";
	case 2:
		return "RESERVED";
	case 3:
		return "ACPI_RECLAIMABLE";
	case 4:
		return "NVS";
	case 5:
		return "NADRAM";
	}
	return "";
}*/


void physical_memory::parse_multiboot_header(void *addr) {

	unsigned long longaddr = reinterpret_cast<unsigned long>(addr);

	//アライメントが狂っていたらエラー
	if(longaddr & 7) {

		kprintf("Unaligned mbi: 0x%X\n", addr);
		return;
	}


	for(struct multiboot_tag *tag = reinterpret_cast<struct multiboot_tag*>(longaddr + 8); tag->type != MULTIBOOT_TAG_TYPE_END; tag = reinterpret_cast<struct multiboot_tag*>(reinterpret_cast<unsigned long>(tag) + ((tag->size + 7) & ~7))) {


		switch(tag->type) {
		case MULTIBOOT_TAG_TYPE_CMDLINE: {

			char *cmdline = reinterpret_cast<struct multiboot_tag_string*>(tag)->string;
			if(strlen(cmdline) > 0) {

				strcpy(this->cmdline, cmdline);
				kprintf("cmdline %p, %s\n", this->cmdline, this->cmdline);
			} else {

				kprintf("cmdline is empty.\n");
			}

			break;
		}
		case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME: {

			char *name = reinterpret_cast<struct multiboot_tag_string*>(tag)->string;

			strcpy(this->bootloader_name, name);

			kprintf("bootloader: %s\n", this->bootloader_name);
			break;
		}
		case MULTIBOOT_TAG_TYPE_MODULE: {


			break;
		}
		case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO: {

			struct multiboot_tag_basic_meminfo *info = reinterpret_cast<struct multiboot_tag_basic_meminfo*>(tag);

			this->lower_memory = info->mem_lower * 1024;
			this->upper_memory = info->mem_upper * 1024;

			kprintf("lower_memory: %u byte, upper_memory: %u byte\n", this->lower_memory, this->upper_memory);
			break;
		}
		case MULTIBOOT_TAG_TYPE_BOOTDEV: {

			struct multiboot_tag_bootdev *device = reinterpret_cast<struct multiboot_tag_bootdev*>(tag);

			kprintf("Boot device 0x%X, partition %X, sub-partition %X\n", device->biosdev, device->slice, device->part);
			break;
		}
		case MULTIBOOT_TAG_TYPE_MMAP: {

			struct multiboot_tag_mmap *mmap = reinterpret_cast<struct multiboot_tag_mmap*>(tag);

			int index = 0;


			for(struct multiboot_mmap_entry* entry = reinterpret_cast<struct multiboot_mmap_entry*>(mmap->entries); reinterpret_cast<unsigned long>(entry) < reinterpret_cast<unsigned long>(tag) + tag->size; entry = reinterpret_cast<struct multiboot_mmap_entry*>(reinterpret_cast<unsigned long>(entry) + mmap->entry_size)) {

				this->mmap_entry[index].address = entry->addr;
				this->mmap_entry[index].length = entry->len;
				this->mmap_entry[index].type = entry->type;

				index++;
			}

			for(auto entry : this->mmap_entry) {

				if(entry.type == 0) {

					break;
				}

				//kprintf("addr: %p, end: %p type: %s\n", entry.address, entry.address + entry.length, get_mmap_type(entry.type));
				this->max_memory_address = entry.address + entry.length;
			}

			break;
		}
		case MULTIBOOT_TAG_TYPE_VBE: {

			kprintf("MULTIBOOT_TAG_TYPE_VBE\n");
			break;
		}
		case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {

			kprintf("MULTIBOOT_TAG_TYPE_FRAMEBUFFER\n");
			break;
		}
		case MULTIBOOT_TAG_TYPE_ELF_SECTIONS: {

			kprintf("MULTIBOOT_TAG_TYPE_ELF_SECTIONS\n");
			break;
		}
		case MULTIBOOT_TAG_TYPE_APM: {

			kprintf("MULTIBOOT_TAG_TYPE_APM\n");
			break;
		}
		case MULTIBOOT_TAG_TYPE_EFI32: {

			kprintf("MULTIBOOT_TAG_TYPE_EFI32\n");
			break;
		}
		case MULTIBOOT_TAG_TYPE_EFI64: {

			kprintf("MULTIBOOT_TAG_TYPE_EFI64\n");
			break;
		}
		case MULTIBOOT_TAG_TYPE_SMBIOS: {

			kprintf("MULTIBOOT_TAG_TYPE_SMBIOS\n");
			break;
		}
		case MULTIBOOT_TAG_TYPE_ACPI_OLD: {

			kprintf("MULTIBOOT_TAG_TYPE_ACPI_OLD\n");
			break;
		}
		case MULTIBOOT_TAG_TYPE_ACPI_NEW: {

			struct multiboot_tag_new_acpi *acpi = reinterpret_cast<struct multiboot_tag_new_acpi*>(tag);

			this->acpi_rsdp_address = reinterpret_cast<unsigned long>(acpi->rsdp);
			//kprintf("MULTIBOOT_TAG_TYPE_ACPI_NEW rsdp:%p\n", acpi->rsdp);
			break;
		}
		case MULTIBOOT_TAG_TYPE_NETWORK: {

			kprintf("MULTIBOOT_TAG_TYPE_NETWORK\n");
			break;
		}
		}
	}

}


//物理メモリ管理を初期化する
void physical_memory::physical_memory_init(unsigned long addr) {


	//ブートローダからの情報をいただく
	parse_multiboot_header(reinterpret_cast<void*>(addr));




	for(auto entry : this->mmap_entry) {

		if(entry.type == 0) {

			break;
		}

		//使用中の物を登録するのでAVAILABLEは無視
		if(entry.type == MULTIBOOT_MEMORY_AVAILABLE) {

			continue;
		}

		//AVAILABLE以外を使用中にする
		set_physical_memory_bit(entry.address, entry.length);



		//kprintf("addr: %p, end: %p type: %s\n", entry.address, entry.address + entry.length, get_mmap_type(entry.type));
	}
}

void physical_memory::set_physical_memory_bit(unsigned long addr, unsigned long len) {

	//2MiB以下は切り落とす
	addr &= ~0x1FFFFF;

	if(len == 0) {

		STOP;
	}


	if(len & 0x1FFFFF) {

		len += MEMORY_BLOCK_SIZE;
		len &= ~0x1FFFFF;
	}


	do {


		//建てるべきbitの位置
		unsigned long shift = addr / MEMORY_BLOCK_SIZE;

		//shiftから配列のインデックスを計算して割り出す

		physical_memory_map[shift / 64] |= (1 << (shift % 64));


		len -= MEMORY_BLOCK_SIZE;

	} while(len != 0);



}

unsigned long physical_memory::alloc_physical_memory() {


	for(int i = 0; i < 1000; i++) {

		if(this->physical_memory_map[i] == 0xFFFFFFFFFFFFFFFFUL) {

			continue;
		}

		for(int j = 0; j < 64; j++) {

			unsigned long shift = i * 64 + j;

			if((this->physical_memory_map[shift / 64] & (1 << (shift % 64))) == 0) {

				unsigned long long addr = shift * MEMORY_BLOCK_SIZE;

				if(addr >= this->max_memory_address) {

					kprintf("[physical_memory] out of memory\nkernel panic.\n");
					STOP;
				}

				this->physical_memory_map[shift / 64] |= (1 << (shift % 64));
				return addr;
			}

		}
	}

	//ここに到達するということは1TBのメモリを積んでいてかつ使いきったということ
	kprintf("[physical_memory] cannot allocate physical memory.\n");
	STOP;
	return 0;
}

void physical_memory::release_physical_memory(unsigned long addr) {

	int shift = addr / MEMORY_BLOCK_SIZE;

	physical_memory_map[shift / 64] &= ~(1 << (shift % 64));
}


















































