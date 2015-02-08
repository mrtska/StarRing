/*
[Module elf.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/11/26
*/

#include <system.h>
#include <string.h>
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


static void *copy_env(char *addr, unsigned long *list) {

	int i = 0;
	struct list_head *ptr;
	list_for_each(ptr, &env) {

		struct env_entry *entry = list_entry(ptr, struct env_entry, list);
		int len = strlen(entry->name) + 1;
		list[i++] = (unsigned long) addr;
		strcpy(addr, entry->name);
		addr += len;

		kprintf("[copy_env] %s, %p\n", entry->name, list[i - 1]);
	}

	return addr;
}

static void *copy_name(char *addr, char *name, unsigned long *index) {

	int len = strlen(name) + 1;
	strcpy(addr, name);
	*index = (unsigned long) addr;
	addr += len;
	kprintf("[copy_name] %s, %p\n", name, *index);

	return addr;
}

static void *copy_arg(char *addr, const char *cmdline, unsigned long *index) {


	char buf[0x100];
	strcpy(buf, cmdline);
	char *tok = strtok(buf, ' ');

	int i = 0;
	while(tok != 0x00) {

		int len = strlen(tok) + 1;
		strcpy(addr, tok);
		index[i] = (unsigned long) addr;
		addr += len;
		kprintf("[copy_arg] %s, %p\n", cmdline, index[i]);
		tok = strtok(0, '/');
		i++;
	}

	return addr;
}

static void *copy_platform(char *addr, unsigned long *arch) {

	char *plat = "x86_64";
	int len = strlen(plat) + 1;
	memcpy(addr, plat, len);
	*arch = (unsigned long) addr;
	addr += len;
	kprintf("[copy_platform] %s, %p\n", plat, *arch);
	return addr;
}


void elf64_create_table(struct process *process, void *p) {

	kprintf("elf64 p %p\n", p);
	void *str_addr = (void*) 0x7FFFFFFFF000;
	unsigned long env_addr[0x100];
	unsigned long cmd_addr[0x100];
	unsigned long name;
	unsigned long arch;

	memset(env_addr, 0x00, sizeof(unsigned long) * 0x100);
	memset(cmd_addr, 0x00, sizeof(unsigned long) * 0x100);


	str_addr = copy_env(str_addr, env_addr);
	str_addr = copy_name(str_addr, process->name, &name);
	str_addr = copy_arg(str_addr, process->cmdline, cmd_addr);
	str_addr = copy_platform(str_addr, &arch);


	NEXT_POINTER(str_addr, 1);

	str_addr = (void*)((unsigned long)str_addr & 0xFFFFFFFFFFFFFFF8);
	unsigned long *random_pointer = str_addr;
	elf_write64(str_addr, 0xFACE);

	elf_write64(p, process->argc);
	NEXT_POINTER(p, 1);

	elf_write64(p, name);
	NEXT_POINTER(p, 1);

	int i = 0;
	while(cmd_addr[i] != 0) {

		elf_write64(p, cmd_addr[i++]);
		NEXT_POINTER(p, 1);
	}

	elf_write64(p, 0);
	NEXT_POINTER(p, 1);


	i = 0;
	while(env_addr[i] != 0) {

		elf_write64(p, env_addr[i++]);
		NEXT_POINTER(p, 1);

	}
	elf_write64(p, 0);
	NEXT_POINTER(p, 1);


	struct elf64_header *header = (struct elf64_header*) process->elf_base;

	unsigned long *vector = p;
	int aux_index = 0;

	NEW_AUX_ENT(AT_SYSINFO_EHDR, 0);
	NEW_AUX_ENT(AT_HWCAP, 0x1FABFBFF);
	NEW_AUX_ENT(AT_PAGESZ, 0x1000);
	NEW_AUX_ENT(AT_CLKTCK, 1000000);
	NEW_AUX_ENT(AT_PHDR, (unsigned long) read_file_offset((void*)0x400000, header->e_phoff));
	NEW_AUX_ENT(AT_PHENT, sizeof(struct elf64_program_header));
	NEW_AUX_ENT(AT_PHNUM, header->e_phnum);
	NEW_AUX_ENT(AT_BASE, process->interp_addr);
	NEW_AUX_ENT(AT_FLAGS, 0);
	NEW_AUX_ENT(AT_ENTRY, process->entry_point);
	NEW_AUX_ENT(AT_UID, 1000);
	NEW_AUX_ENT(AT_EUID, 1000);
	NEW_AUX_ENT(AT_GID, 1000);
	NEW_AUX_ENT(AT_EGID, 1000);
	NEW_AUX_ENT(AT_SECURE, 0);
	NEW_AUX_ENT(AT_RANDOM, (unsigned long)random_pointer);
	NEW_AUX_ENT(AT_EXECFN, name);
	NEW_AUX_ENT(AT_PLATFORM, arch);


	NEW_AUX_ENT(AT_NULL, 0);


	kprintf("[auxv] phdr %p\n", read_file_offset((void*)0x400000, header->e_phoff));
}

void load_elf_interp(struct process *process, char *path) {

	kprintf("[load_elf_interp] path %s\n", path);
	char path_buf[0x200] = "/library";

	strcat(path_buf, path);
	kprintf("path %s\n", path_buf);

	struct fs_node *node = kopen(path_buf, 0);

	map_page(process->interp_addr, VIRTUAL_ADDRESS_TO_PHYSICAL_ADDRESS(process->interp_base), process->page_tables, FLAGS_USER_PAGE | FLAGS_WRITABLE_PAGE | FLAGS_LARGE_PAGE);
	void *file_base = process->interp_base;



	read_fs(node, 0, node->length, file_base);
	kprintf("[interp] file base %p, %X\n", file_base, node->length);


	struct elf64_header *header = (struct elf64_header*) file_base;
	struct elf64_program_header *pheader = (void*) ((unsigned char*) header + (header->e_phoff));

	process->entry_interp = header->e_entry;

	int i;
	for(i = 0; i < header->e_phnum; i++) {

		const struct elf64_program_header *cur_p = &pheader[i];

		kprintf("type %X, offset %X, addr %X, size %X, page %X, align %X\n", cur_p->p_type, cur_p->p_offset, cur_p->p_vaddr, cur_p->p_memsz, cur_p->p_fizesz, cur_p->p_align);

		switch(cur_p->p_type) {

		case ELF_PT_NULL:

			//IGNORED!!
			break;

		case ELF_PT_LOAD: {

			unsigned char *base;

			//ページの属性を設定する
			int page_flags = FLAGS_USER_PAGE;

			//ラージページに設定
			if(cur_p->p_align == 0x1000) {

				base = alloc_memory_block4k();
			} else {

				page_flags |= FLAGS_LARGE_PAGE;
				base = alloc_memory_block();
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
			break;
		}

		case ELF_PT_NOTE:

			//Ignored.
			break;

		}
	}
	//STOP;
}


int load_elf_binary(struct process *process) {

	write_cr3(VIRTUAL_ADDRESS_TO_PHYSICAL_ADDRESS(process->page_tables));

	struct fs_node *node = process->file;
	kprintf("load_elf_binary name %s, size %X\n", node->filename, node->length);

	//nodeをもとにfile_baseにデータを読み込む
	read_fs(node, 0, node->length, process->elf_base);

	//ELFヘッダ
	struct elf_ident *ident = (struct elf_ident*) process->elf_base;

	//ELFマジックの確認
	verify_elf_header(ident);

	//64ビットコード
	if(ident->class == 0x02) {

		struct elf64_header *header = (struct elf64_header*) process->elf_base;

		struct elf64_program_header *pheader = (void*) ((unsigned char*) header + (header->e_phoff));


		//インタープリターがあったらインタープリターをロードする
		int i;
		for(i = 0; i < header->e_phnum; i++) {

			const struct elf64_program_header *cur_p = &pheader[i];

			kprintf("type %u, offset %X, addr %X, size %X, page %X\n", cur_p->p_type, cur_p->p_offset, cur_p->p_vaddr, cur_p->p_memsz, cur_p->p_fizesz);

			switch(cur_p->p_type) {

			}
		}

		process->entry_point = header->e_entry;
		for(i = 0; i < header->e_phnum; i++) {

			const struct elf64_program_header *cur_p = &pheader[i];

			kprintf("type %u, offset %X, addr %X, size %X, page %X, align %X\n", cur_p->p_type, cur_p->p_offset, cur_p->p_vaddr, cur_p->p_memsz, cur_p->p_fizesz, cur_p->p_align);

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

				load_elf_interp(process, read_file_offset(header, cur_p->p_offset));
				break;
			}
			case ELF_PT_NOTE:

				//Ignored.
				break;

			}
		}
	}
	kprintf("id %d\n", get_apic_id());
	return 0;

}

