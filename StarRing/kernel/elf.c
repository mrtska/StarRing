/*
[Module elf.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/11/26
*/

#include <system.h>
#include <elf.h>
#include <page/page.h>
#include <fs/vfs.h>
#include <apic.h>
#include <env.h>
#include <fs/auxvec.h>
#include <mem/alloc.h>

static int verify_elf_header(struct elf_ident *ident) {

	//ELFマジックの確認
	if(ident->magic1 != 0x7F) {

		kprintf("[kernel/run_process] Invalid elf magic1! %X\n", ident->magic1);
		return 2;
	}
	if(ident->magic2 != 'E') {

		kprintf("[kernel/run_process] Invalid elf magic2! %X\n", ident->magic2);
		return 2;
	}
	if(ident->magic3 != 'L') {

		kprintf("[kernel/run_process] Invalid elf magic3! %X\n", ident->magic3);
		return 2;
	}
	if(ident->magic4 != 'F') {

		kprintf("[kernel/run_process] Invalid elf magic4! %X\n", ident->magic4);
		return 2;
	}

	return 0;
}

static void *read_file_offset(void *file, unsigned int offset) {

	return file + offset;
}

#define NEXT_POINTER(p, c) p += (sizeof(unsigned long) * c)

#define NEW_AUX_ENT(id, val)		\
	do {							\
		vector[aux_index++] = id;	\
		vector[aux_index++] = val;	\
		NEXT_POINTER(p, 2);			\
	} while (0)


void elf64_create_table(struct process *process, int argc, void *p) {

	kprintf("elf64 p %p\n", p);
	void *str_addr = (void*) 0x7FFFFFFFF000;
	unsigned long env_addr[0x200];
	unsigned long name;
	unsigned long arch;


	int i = 0;
	struct list_head *ptr;
	list_for_each(ptr, &env) {

		struct env_entry *entry = list_entry(ptr, struct env_entry, list);
		int len = strlen(entry->name) + 1;
		env_addr[i++] = (unsigned long) str_addr;
		strcpy(str_addr, entry->name);
		str_addr += len;
	}

	int len = strlen(process->name) + 1;
	name = (unsigned long) str_addr;
	strcpy(str_addr, process->name);
	str_addr += len;




	char *plat = "x86_64";
	len = strlen(plat) + 1;
	arch = (unsigned long) str_addr;
	memcpy(str_addr, plat, len);
	str_addr += len;
	NEXT_POINTER(str_addr, 1);
	str_addr = (void*)((unsigned long)str_addr & 0xFFFFFFFFFFFFFFF8);
	unsigned long *random_pointer = str_addr;
	elf_write64(str_addr, 0xFACE);

	elf_write64(p, argc);
	NEXT_POINTER(p, 1);

	elf_write64(p, name);
	NEXT_POINTER(p, 1);


	elf_write64(p, 0);
	NEXT_POINTER(p, 1);


	i = 0;
	while(env_addr[i] != 0) {

		*(unsigned long*)p = env_addr[i++];
		NEXT_POINTER(p, 1);

	}
	p = (void*)((unsigned long)p & 0xFFFFFFFFFFFFFFF8);


	elf_write64(p, 0);
	NEXT_POINTER(p, 1);


	struct elf64_header *header = (struct elf64_header*) process->file_base;

	unsigned long *vector = p;
	int aux_index = 0;

	NEW_AUX_ENT(AT_SYSINFO_EHDR, 0);
	NEW_AUX_ENT(AT_HWCAP, 0x1FABFBFF);
	NEW_AUX_ENT(AT_PAGESZ, 0x1000);
	NEW_AUX_ENT(AT_CLKTCK, 1000000l);
	NEW_AUX_ENT(AT_PHDR, (unsigned long) read_file_offset((void*)0x400000, header->e_phoff));
	NEW_AUX_ENT(AT_PHENT, sizeof(struct elf64_program_header));
	NEW_AUX_ENT(AT_PHNUM, header->e_phnum);
	NEW_AUX_ENT(AT_BASE, 0);
	NEW_AUX_ENT(AT_FLAGS, 0);
	NEW_AUX_ENT(AT_ENTRY, process->entry_point);
	NEW_AUX_ENT(AT_UID, 0);
	NEW_AUX_ENT(AT_EUID, 0);
	NEW_AUX_ENT(AT_GID, 0);
	NEW_AUX_ENT(AT_EGID, 0);
	NEW_AUX_ENT(AT_SECURE, 0);
	NEW_AUX_ENT(AT_RANDOM, (unsigned long)random_pointer);
	NEW_AUX_ENT(AT_EXECFN, name);
	NEW_AUX_ENT(AT_PLATFORM, arch);


	NEW_AUX_ENT(AT_NULL, 0);


}


int load_elf_binary(struct process *process) {

	write_cr3(VIRTUAL_ADDRESS_TO_PHYSICAL_ADDRESS(process->page_tables));

	struct fs_node *node = process->file;
	kprintf("load_elf_binary name %s, size %X\n", node->filename, node->length);

	//nodeをもとにfile_baseにデータを読み込む
	read_fs(node, 0, node->length, process->file_base);

	//ELFヘッダ
	struct elf_ident *ident = (struct elf_ident*) process->file_base;

	//ELFマジックの確認
	verify_elf_header(ident);

	//64ビットコード
	if(ident->class == 0x02) {

		struct elf64_header *header = (struct elf64_header*) process->file_base;

		struct elf64_program_header *pheader = (void*) ((unsigned char*) header + (header->e_phoff));


		process->entry_point = header->e_entry;
		int i;
		for(i = 0; i < header->e_phnum; i++) {

			const struct elf64_program_header *cur_p = &pheader[i];

			kprintf("type %u, offset %X, addr %X, size %X, page %X\n", cur_p->p_type, cur_p->p_offset, cur_p->p_vaddr, cur_p->p_memsz, cur_p->p_fizesz);

			switch(cur_p->p_type) {

			case ELF_PT_NULL:

				//IGNORED!!
				break;

			case ELF_PT_LOAD: {

				unsigned char *base;

				//ページの属性を設定する
				int page_flags = FLAGS_USER_PAGE;

				//ラージページに設定
				if(cur_p->p_align == 0x200000) {

					page_flags |= FLAGS_LARGE_PAGE;
					base = alloc_memory_block();
				} else {

					base = alloc_memory_block4k();
				}

				//書き込み権限を与える
				if(cur_p->p_flags & ELF_PF_W) {

					page_flags |= FLAGS_WRITABLE_PAGE;
				}

				//実行権限を剥奪
				if(!(cur_p->p_flags & ELF_PF_X)) {

					page_flags |= FLAGS_NO_EXECUTE;
				}


				map_page(cur_p->p_vaddr, (unsigned long) base, process->page_tables, page_flags);
				memcpy((void*) cur_p->p_vaddr, read_file_offset(header, cur_p->p_offset), cur_p->p_fizesz);

				unsigned long brk = (cur_p->p_vaddr + cur_p->p_fizesz + MEMORY_BLOCK_SIZE) & 0xFFFFFFFFFFF00000;
				process->start_brk = process->end_brk = (process->start_brk > brk) ? process->start_brk : brk;
				break;
			}
			case ELF_PT_INTERP: {

				//Ignored.
				break;
			}

			case ELF_PT_NOTE:

				//Ignored.
				break;

			}
		}
	}
	kprintf("id %d\n", apic_read(APIC_ID_R) >> 24);
	return 0;

}

