/*
[Module page.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/04/20
*/


#include <page/page.h>
#include <internal/stdio.h>
#include <system.h>
#include <string.h>
#include <mem/alloc.h>
#include <mem/vmm.h>
#include <apic.h>

//---ページテーブル群---
unsigned long *kernel_pml4;

//ページ初期化
void page_init(void) {

	kernel_pml4 = vmm_page_table_alloc();

	kprintf("kernel %p\n", kernel_pml4);


	//物理メモリ ストレートマッピング
	map_page(0xFFFF880000000000, 0x00000000, kernel_pml4, FLAGS_WRITABLE_PAGE | FLAGS_ALLOC_512);
	map_page(0xFFFF880040000000, 0x40000000, kernel_pml4, FLAGS_WRITABLE_PAGE | FLAGS_ALLOC_512);
	map_page(0xFFFF880080000000, 0x80000000, kernel_pml4, FLAGS_WRITABLE_PAGE | FLAGS_ALLOC_512);
	map_page(0xFFFF8800C0000000, 0xC0000000, kernel_pml4, FLAGS_WRITABLE_PAGE | FLAGS_ALLOC_512);

	//カーネルテキスト
	map_page(0xFFFFFFFF80000000, 0x00000000, kernel_pml4, FLAGS_WRITABLE_PAGE | FLAGS_ALLOC_512);

	kprintf("pml4 %p\n", kernel_pml4[0x110]);
	write_cr3((unsigned long) VIRTUAL_ADDRESS_TO_PHYSICAL_ADDRESS(kernel_pml4));

	mfence();

}


void setup_user_page(struct process *process) {

	unsigned long *pml4 = process->page_tables;

	combine_kernel_pml4((unsigned long) pml4);
}

//カーネルページテーブルを接合する
void combine_kernel_pml4(unsigned long destination_pml4) {

	unsigned long *source = kernel_pml4;
	unsigned long *dest = (unsigned long*) destination_pml4;

	//カーネル空間のみ接合する
	int i;
	for(i = 1; i < 0x200; i++) {

		if(source[i] & 1) {

			dest[i] = source[i];
		}
	}
}


//ページディレクトリを構築する
void map_pd(unsigned long virt, unsigned long phys, unsigned long *pd, int flags) {

	unsigned int pde = pd_index(virt);
	//unsigned long *pd = (unsigned long*) ((unsigned long)pdpt & MASK_FRAME_ADDR);

	if((unsigned long)pd < 0xFFFF880000000000ULL) {

		pd = (unsigned long*) PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(pd);
	}

	unsigned long *p = pd;

	//kprintf("map_pd   %p\n", p);
	int page_flags = PG_PRESENT;

	if(flags & FLAGS_WRITABLE_PAGE) {

		page_flags |= PG_WRITE;
	}


	if(flags & FLAGS_USER_PAGE) {

		page_flags |= PG_USER;
	}

	if(flags & FLAGS_NO_EXECUTE) {

		page_flags |= PG_NX;
	}

	if(flags & FLAGS_LARGE_PAGE) {

		page_flags |= PG_LARGE;

		if(flags & FLAGS_ALLOC_512) {

			int i;
			for(i = 0; i < 0x200; i++, phys += 0x200000) {

				p[i] = phys | (page_flags);
			}

			//kprintf("map_pd %p, %p\n", p[i], virt);
		} else {

			p[pde] = phys | (page_flags);
		}

	}
}

//ページディレクトリポインターを構築する
void map_pdpt(unsigned long virt, unsigned long phys, unsigned long *pdpt, int flags) {



	unsigned int pdpte = pdpt_index(virt);

	int page_flags = PG_PRESENT;

	if(flags & FLAGS_WRITABLE_PAGE) {

		page_flags |= PG_WRITE;
	}

	if(flags & FLAGS_USER_PAGE) {

		page_flags |= PG_USER;
	}

	if(flags & FLAGS_NO_EXECUTE) {

		page_flags |= PG_NX;
	}

	unsigned long p;

	if((unsigned long)pdpt < 0xFFFF880000000000ULL) {

		pdpt = (unsigned long*) PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(pdpt);
	}

	if(pdpt[pdpte] & 1) {

		p = (pdpt[pdpte] & MASK_FRAME_ADDR);

	} else {

		p = (unsigned long) vmm_page_table_alloc();
	}


	//PDPTにPDをセットする
	if(p > 0xFFFF880000000000) {

		pdpt[pdpte] = VIRTUAL_ADDRESS_TO_PHYSICAL_ADDRESS(p) | (page_flags);

		//kprintf("map_pdpt %p, e %d\n", pdpt[pdpte], pdpte);
	} else {

		pdpt[pdpte] = p | (page_flags);

	}


	//kprintf("map_pdpt %p, e %d\n", pdpt, pdpte);
	map_pd(virt, phys, (unsigned long*) p, flags);
}

//PML4を構築する
void map_pml4(unsigned long virt, unsigned long phys, unsigned long *pml4, int flags) {

	unsigned int pml4e = pml4_index(virt);

	int page_flags = PG_PRESENT;

	if(flags & FLAGS_WRITABLE_PAGE) {

		page_flags |= PG_WRITE;
	}

	if(flags & FLAGS_USER_PAGE) {

		page_flags |= PG_USER;
	}

	if(flags & FLAGS_NO_EXECUTE) {

		page_flags |= PG_NX;
	}



	unsigned long p;

	if(pml4[pml4e] & 1) {

		p = pml4[pml4e] & MASK_FRAME_ADDR;

		//kprintf("map_pml4 %p, e %X, v = %p\n", p, pml4e, virt);
	} else {

		p = (unsigned long) vmm_page_table_alloc();
	}

	//PML4にPDPTをセットする
	if(p > 0xFFFF880000000000) {

		pml4[pml4e] = VIRTUAL_ADDRESS_TO_PHYSICAL_ADDRESS(p) | (page_flags);
	} else {

		pml4[pml4e] = p | (page_flags);
	}


	map_pdpt(virt, phys, (unsigned long*) p, flags);
}

// map_page(0xFFFFFFFFC0000000, 0x2000000, NULL, 0);

//ページをマップする processにNULLを指定した場合は現在のcr3が使用される 一度に512個のページテーブルが構築される cr3は仮想アドレスを指定する
void map_page(unsigned long virt, unsigned long phys, unsigned long *cr3, int flags) {


	map_pml4(virt, phys, cr3, FLAGS_LARGE_PAGE | flags);
}

unsigned long get_page_directory(unsigned long virt, unsigned long cr3) {

	unsigned int pml4e = pml4_index(virt);
	unsigned int pdpte = pdpt_index(virt);
	unsigned int pde = pd_index(virt);

	unsigned long *pml4 = (unsigned long*) cr3;
	unsigned long *pointer = (unsigned long*) PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(pml4[pml4e] & MASK_FRAME_ADDR);
	unsigned long *pd = (unsigned long*) PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(pointer[pdpte] & MASK_FRAME_ADDR);

	return pd[pde];
}

