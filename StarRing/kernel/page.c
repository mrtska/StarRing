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
void page_init(unsigned long memory_max) {

	kernel_pml4 = vmm_page_table_alloc();

	kprintf("kernel %p\n", kernel_pml4);

	//物理メモリ ストレートマッピング
	unsigned long i;
	for(i = 0; i < memory_max; i += 0x40000000) {

		map_page(0xFFFF880000000000 + i, i, kernel_pml4, FLAGS_WRITABLE_PAGE | FLAGS_LARGE_PAGE | FLAGS_ALLOC_512MB);
	}

	//カーネルテキスト
	map_page(0xFFFFFFFF80000000, 0x00000000, kernel_pml4, FLAGS_WRITABLE_PAGE | FLAGS_LARGE_PAGE | FLAGS_ALLOC_512MB);

	kprintf("pml4 %p\n", kernel_pml4[0x110]);
	write_cr3((unsigned long) VIRTUAL_ADDRESS_TO_PHYSICAL_ADDRESS(kernel_pml4));
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


		if(source[i] & PG_PRESENT) {

			dest[i] = source[i];
		}
	}
}

//ページテーブルを構築
void map_pt(unsigned long virt, unsigned long phys, unsigned long *pt, int flags) {

	unsigned int pte = pt_index(virt);

	if((unsigned long)pt < 0xFFFF880000000000ULL) {

		pt = (unsigned long*) PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(pt);
	}

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

	pt[pte] = phys | (page_flags);

	//kprintf("map_pt %p, phys %p\n", pt, phys);
}

//ページディレクトリを構築する
void map_pd(unsigned long virt, unsigned long phys, unsigned long *pd, int flags) {

	unsigned int pde = pd_index(virt);

	if((unsigned long)pd < 0xFFFF880000000000ULL) {

		pd = (unsigned long*) PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(pd);
	}

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

		if(flags & FLAGS_ALLOC_512MB) {

			int i;
			for(i = 0; i < 0x200; i++, phys += 0x200000) {

				pd[i] = phys | (page_flags);
			}

			//kprintf("map_pd %p, %p\n", p[i], virt);
		} else {

			pd[pde] = phys | (page_flags);
			//kprintf("map_pd %p, %p\n", pde, virt);
		}
	} else {

		unsigned long p;

		if(pd[pde] & PG_PRESENT) {

			p = (pd[pde] & MASK_FRAME_ADDR);
		} else {

			p = (unsigned long) vmm_page_table_alloc();
		}
		//PDにPTをセットする
		if(p > 0xFFFF880000000000) {

			pd[pde] = VIRTUAL_ADDRESS_TO_PHYSICAL_ADDRESS(p) | (page_flags);
		} else {

			pd[pde] = p | (page_flags);
		}
		map_pt(virt, phys, (unsigned long*)p, flags);
	}
}

//ページディレクトリポインターを構築する
void map_pdpt(unsigned long virt, unsigned long phys, unsigned long *pdpt, int flags) {



	if((unsigned long)pdpt < 0xFFFF880000000000ULL) {

		pdpt = (unsigned long*) PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(pdpt);
	}

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

	if(pdpt[pdpte] & PG_PRESENT) {

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

	//PDPTが存在するか
	if(pml4[pml4e] & PG_PRESENT) {

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

//ページをマップする cr3は仮想アドレスを指定する
void map_page(unsigned long virt, unsigned long phys, unsigned long *cr3, int flags) {

	kprintf("[kernel/map_page] %p\n", virt);
	map_pml4(virt, phys, cr3, flags);
}

