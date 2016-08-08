
#include <virtualmemory.h>
#include <physicalmemory.h>
#include <stdio.h>


class virtual_memory virtual_memory;




void *virtual_memory::alloc_kernel_page() {

	if(kernel_page_tables == nullptr) {

		kprintf("[virtual_memory] cannot allocate kernel page.\n");
	}

	void *addr = reinterpret_cast<void*>(reinterpret_cast<unsigned long>(kernel_page_tables) + offset);
	offset += 0x1000;



	//kprintf("[virtual_memory] allocate %p\n", addr);

	return addr;
}
void virtual_memory::virtual_memory_init() {



	this->physical_base_address = read_cr3();


	unsigned long kernel_tables = physical_memory.alloc_physical_memory();
	this->kernel_page_tables = reinterpret_cast<unsigned long*>(kernel_tables);

	//カーネルページを割り当てるアドレスを現在のページテーブルにマッピングする
	map_virtual_memory(kernel_tables, kernel_tables, PRESENT | WRITE | GLOBAL, true);


	//新しくCR3に代入するベースアドレス
	unsigned long new_base_address = physical_memory.alloc_physical_memory();

	map_virtual_memory(new_base_address, new_base_address, PRESENT | WRITE | GLOBAL, true);

	//新しいカーネルページの場所
	this->physical_base_address = new_base_address;


	//新しいページテーブルにもここだけストレートマッピング
	map_virtual_memory(kernel_tables, kernel_tables, PRESENT | WRITE | GLOBAL, true);
	map_virtual_memory(new_base_address, new_base_address, PRESENT | WRITE | GLOBAL, true);


	//FFFFFFFF80000000から512MB分ストレートマッピングする
	for(unsigned long addr = 0xFFFFFFFF80000000, phys = 0; addr < 0xFFFFFFFFA0000000; addr += MEMORY_BLOCK_SIZE, phys += MEMORY_BLOCK_SIZE) {

		map_virtual_memory(addr, phys, PRESENT | WRITE | GLOBAL, true);
	}


	//FFFF800000000000に物理アドレスを実装している容量全てストレートマッピングする
	for(unsigned long addr = 0xFFFF800000000000, phys = 0; phys < physical_memory.get_max_memory_address(); addr += MEMORY_BLOCK_SIZE, phys += MEMORY_BLOCK_SIZE) {

		map_virtual_memory(addr, phys, PRESENT | WRITE | GLOBAL, true);
	}


	this->write_cr3(this->physical_base_address);
}


//物理アドレスを仮想アドレスを云々
void virtual_memory::map_virtual_memory(unsigned long addr, unsigned long flags, bool is2MB) {

	this->map_virtual_memory(addr, physical_memory.alloc_physical_memory(), flags, is2MB);
}

void virtual_memory::map_virtual_memory(unsigned long addr, unsigned long phys, unsigned long flags, bool is2MB) {


	//kprintf("[virtual_memory] mapping: virtual:%p, phys:%p\n", addr, phys);

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

		pdpt = &reinterpret_cast<union pdpte*>(alloc_kernel_page())[pdpte];
		pml4->data = reinterpret_cast<unsigned long>(pdpt) & PAGE_ADDRESS_MASK;

		//TODO pdpt = alloc memory and set flags.

	}

	pml4->data |= flags;

	if(pdpt->normal.present) {

		pd = &get_pde(*pdpt)[pde];
		pdpt->data |= flags;

	} else {

		pd = &reinterpret_cast<union pde*>(alloc_kernel_page())[pde];
		pdpt->data = (reinterpret_cast<unsigned long>(pd) & PAGE_ADDRESS_MASK) | flags;
		//TODO pd = alloc memory and set flags.

	}



	//ラージページか
	if(is2MB) {


		pd->data |= flags | LARGE;
		pd->data |= phys & 0x000FFFFFFFE00000;



	} else {

		if(pd->normal.present) {


		} else {


		}

		STOP;

	}




}































