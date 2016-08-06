
#include <virtualmemory.h>
#include <physicalmemory.h>
#include <stdio.h>


class virtual_memory virtual_memory;



void virtual_memory::virtual_memory_init() {



	kprintf("current cr3: %p\n", read_cr3());

	this->physical_base_address = read_cr3();


	unsigned long kernel_tables = physical_memory.alloc_physical_memory();
	this->kernel_page_tables = reinterpret_cast<unsigned long*>(kernel_tables);

	//カーネルページを割り当てるアドレスを現在のページテーブルにマッピングする
	map_virtual_memory(kernel_tables, kernel_tables, PRESENT | WRITE, true);

STOP;
	kprintf("page fault test %p\n", *(reinterpret_cast<unsigned long*>(0x200000)));

STOP;

	//新しいカーネルページの場所
	this->physical_base_address = physical_memory.alloc_physical_memory();




	//FFFF800000000000に物理アドレスを実装している容量全てストレートマッピングする
	for(unsigned long addr = 0xFFFF800000000000, phys = 0; addr < physical_memory.get_max_memory_address(); addr += MEMORY_BLOCK_SIZE, phys += MEMORY_BLOCK_SIZE) {

		map_virtual_memory(addr, phys, PRESENT | WRITE, true);
	}





}


void virtual_memory::map_virtual_memory(unsigned long addr, unsigned long flags, bool is2MB) {




}

void map_pml4(unsigned long addr, unsigned long phys, unsigned long flags, bool is2MB) {



}

//物理アドレスを仮想アドレスを云々
void virtual_memory::map_virtual_memory(unsigned long addr, unsigned long phys, unsigned long flags, bool is2MB) {


	kprintf("[virtual_memory] mapping: virtual:%p, phys:%p\n", addr, phys);

	if(phys & 0x1FFFFF) {

		kprintf("[virtual_memory] invalid alignment %p\n", phys);
		STOP;
	}

	//仮想アドレスからエントリのインデックスを取得
	auto pml4e = pml4_index(addr);
	auto pdpte = pdpt_index(addr);
	auto pde = pd_index(addr);


	//CR3とインデックスからpml4を取得
	union pml4e *pml4 = &reinterpret_cast<union pml4e*>(this->physical_base_address)[pml4e];

	union pdpte *pdpt;
	union pde *pd;

	if(pml4->present) {


		pdpt = &get_pdpte(*pml4)[pdpte];

	} else {


		//TODO pdpt = alloc memory and set flags.
		kprintf("[virtual_memory] UnImplementedException in pml4\n");

		STOP;
	}

	pml4->data |= flags;

	if(pdpt->normal.present) {

		 pd = &get_pde(*pdpt)[pde];

	} else {

		//TODO pd = alloc memory and set flags.
		kprintf("[virtual_memory] UnImplementedException in pdpt\n");

		STOP;

	}

	pdpt->data |= flags;


	//ラージページか
	if(is2MB) {


		pd->data |= phys | flags | LARGE;

		trace();


	} else {
/*
		if(pd->normal.present) {


		} else {




		}*/

		STOP;

	}





	kprintf("pml4e %p %d pml4 address %p\n", pml4, pml4e, pml4->data);
	kprintf("pdpt  %p %d pdpt address %p\n", pdpt, pdpte, pdpt->data);
	kprintf("pd    %p %d pd   address %p\n", pd, pde, pd->data);
	kprintf("sizeof %X\n", sizeof(union pde));

}































