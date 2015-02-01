/*
 * elf.c
 *
 *  Created on: 2014/11/26
 *      Author: StarRing
 */

#include <system.h>
#include <elf.h>
#include <page/page.h>
#include <fs/vfs.h>
#include <apic.h>


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

void debug_elf() {

	trace();
}

int load_elf64_shared_library(struct process *process, struct elf64_dynamic_table *exe_table) {

	//パスをライブラリルートとつなげる
	char buf[0x200] = "/library/lib64/";
	strcat(buf, exe_table->name);

	//ライブラリオープン
	struct fs_node *library = kopen(buf, 0);

	kprintf("[load_elf64_shared_library] name = %s, size = %X\n", library->filename, library->length);

	//nodeをもとにfile_baseにデータを読み込む
	void *p = alloc_kernel_memory(library->length);
	read_fs(library, 0, library->length, p);

	//ELFヘッダ
	struct elf_ident *ident = (struct elf_ident*) p;

	//ELFマジックの確認
	if(verify_elf_header(ident)) {

		kprintf("error\n");
		STOP;
	}

	//64ビットコード
	if(ident->class == 0x02) {

		struct elf64_header *header = (struct elf64_header*) p;
		struct elf64_program_header *pheader = read_file_offset(header, header->e_phoff);

		kprintf("entry point %X\n", header->e_entry);
		int i;
		for(i = 0; i < header->e_phnum; i++) {

			const struct elf64_program_header *cur_p = &pheader[i];
			kprintf("type = %s, flags = %s, addr = %X, offset = %X, page %X\n", conv_pt(cur_p->p_type), conv_pf(cur_p->p_flags), cur_p->p_vaddr, cur_p->p_offset, cur_p->p_align);

			switch(cur_p->p_type) {
			case ELF_PT_LOAD: {

				unsigned char *base = alloc_memory_block(1);

				//ページの属性を設定する
				int page_flags = FLAGS_USER_PAGE;

				if(cur_p->p_flags & ELF_PF_W) {

					page_flags |= FLAGS_WRITABLE_PAGE;
				}

				if(!(cur_p->p_flags & ELF_PF_X)) {

					page_flags |= FLAGS_WRITABLE_PAGE;
				}

				map_page(cur_p->p_vaddr & MASK_FRAME_ADDR, (unsigned long) base, process->page_tables, page_flags);
				memcpy((void*) cur_p->p_vaddr, read_file_offset(header, cur_p->p_offset), cur_p->p_fizesz);

				break;
			}
			case ELF_PT_INTERP: {

				//Ignored.
				break;
			}

			case ELF_PT_DYNAMIC: {

				struct elf64_dynamic_table table;

				struct elf64_dyn *dyn = read_file_offset(header, cur_p->p_offset);

				while(dyn->d_tag != DT_NULL) {

					switch(dyn->d_tag) {

					case DT_NEEDED: {

						table.index = dyn->d_un.d_val;
						break;
					}
					case DT_PLTRELSZ:

						table.pltrel_size = dyn->d_un.d_val;
						break;
					case DT_PLTGOT:

						table.pltgot = dyn->d_un.d_ptr;
						break;
					case DT_HASH:

						table.hash = dyn->d_un.d_ptr;
						break;
					case DT_STRTAB:

						table.strtab = dyn->d_un.d_ptr;
						break;
					case DT_SYMTAB:

						table.symtab = dyn->d_un.d_ptr;
						break;
					case DT_RELA:

						table.rela = dyn->d_un.d_ptr;
						break;
					case DT_RELASZ:

						table.rela_size = dyn->d_un.d_val;
						break;
					case DT_RELAENT:

						table.rela_entry = dyn->d_un.d_val;
						break;
					case DT_STRSZ:

						table.str_size = dyn->d_un.d_val;
						break;
					case DT_SYMENT:

						table.sym_entry = dyn->d_un.d_val;
						break;
					case DT_INIT:

						table.init_addr = dyn->d_un.d_ptr;
						break;
					case DT_FINI:

						table.fini_addr = dyn->d_un.d_ptr;
						break;
					case DT_RPATH:

						table.rpath = dyn->d_un.d_val;
						break;
					case DT_REL:

						table.rel = dyn->d_un.d_ptr;
						break;
					case DT_RELSZ:

						table.rel_size = dyn->d_un.d_val;
						break;
					case DT_RELENT:

						table.rel_entry = dyn->d_un.d_val;
						break;
					case DT_PLTREL:

						table.plt_rel = dyn->d_un.d_val;
						break;
					case DT_DEBUG:

						table.debug = dyn->d_un.d_ptr;
						break;
					case DT_TEXTREL:

						break;
					case DT_JMPREL:

						table.jmp_rel = dyn->d_un.d_ptr;
						break;

					default:

						kprintf(".dynamic %d\n", dyn->d_tag);
						break;
					}
					dyn++;
				}
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
				table.name = &table.strtab[table.index];
				kprintf("NEEDED %s\n", table.name);
				kprintf("relaent %X, rela size %X\n", sizeof(struct elf64_rela), table.rela_size);

				struct elf64_sym *sym = (void*) table.symtab;
				struct elf64_rela *rela = (void