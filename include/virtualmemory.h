
#pragma once

#include <system.h>



#define PAGE_ADDRESS_MASK 0x000FFFFFFFFFF000

extern class virtual_memory virtual_memory;


//page map level 4 entry
union pml4e {

	unsigned long data;

	struct {

		unsigned long present:1;	//プレゼントフラグ
		unsigned long rw:1;		//書き込み可能か
		unsigned long us:1;		//ユーザーがアクセス可能か
		unsigned long pwt:1;		//ライトスルーか
		unsigned long pcd:1;		//キャッシュが無効か
		unsigned long a:1;			//ユーザーにそのページがアクセスされたか
		unsigned long ignored0:1;
		unsigned long reserved0:1;
		unsigned long ignored1: 4;

		unsigned long address: 40;	//物理アドレス



		unsigned short ignroed2:11;


		unsigned long xd:1;
	} __attribute__((packed));


} __attribute__((packed));

//page directory pointer table entry
union pdpte {

	unsigned long data;


	struct {

		unsigned long present:1;	//プレゼントフラグ
		unsigned long rw:1;		//書き込み可能か
		unsigned long us:1;		//ユーザーがアクセス可能か
		unsigned long pwt:1;		//ライトスルーか
		unsigned long pcd:1;		//キャッシュが無効か
		unsigned long a:1;			//ユーザーにそのページがアクセスされたか
		unsigned long d:1;			//ユーザーにそのページが書き込まれたか
		unsigned long one:1;		//1GBページかどうか
		unsigned long g:1;			//グローバルかどうか
		unsigned long ignored1: 3;
		unsigned long pat:1;		//attributeタイプ
		unsigned long reserved: 17;

		unsigned long address: 22;	//物理アドレス

		unsigned long ignroed2:11;


		unsigned long xd:1;		//実行不可なページか
	} page1gb __attribute__((packed));

	struct {

		unsigned long present:1;	//プレゼントフラグ
		unsigned long rw:1;		//書き込み可能か
		unsigned long us:1;		//ユーザーがアクセス可能か
		unsigned long pwt:1;		//ライトスルーか
		unsigned long pcd:1;		//キャッシュが無効か
		unsigned long a:1;			//ユーザーにそのページがアクセスされたか
		unsigned long ignored0:1;
		unsigned long zero:1;		//1GBページかどうか
		unsigned long ignored1: 4;

		unsigned long address: 40;	//物理アドレス



		unsigned long ignroed2:11;


		unsigned long xd:1;		//実行不可なページか
	} normal __attribute__((packed));



} __attribute__((packed));

//page directory entry
union pde {



	unsigned long data;


	struct {

		unsigned long present:1;	//プレゼントフラグ
		unsigned long rw:1;		//書き込み可能か
		unsigned long us:1;		//ユーザーがアクセス可能か
		unsigned long pwt:1;		//ライトスルーか
		unsigned long pcd:1;		//キャッシュが無効か
		unsigned long a:1;			//ユーザーにそのページがアクセスされたか
		unsigned long d:1;			//ユーザーにそのページが書き込まれたか
		unsigned long one:1;		//1GBページかどうか
		unsigned long g:1;			//グローバルかどうか
		unsigned long longignored1: 3;
		unsigned long pat:1;		//attributeタイプ
		unsigned long reserved:8;

		unsigned long address: 31;	//物理アドレス

		unsigned long ignroed2:11;

		unsigned long xd:1;		//実行不可なページか
	} page2mb __attribute__((packed));

	struct {

		unsigned long present:1;	//プレゼントフラグ
		unsigned long rw:1;		//書き込み可能か
		unsigned long us:1;		//ユーザーがアクセス可能か
		unsigned long pwt:1;		//ライトスルーか
		unsigned long pcd:1;		//キャッシュが無効か
		unsigned long a:1;			//ユーザーにそのページがアクセスされたか
		unsigned long ignored0:1;
		unsigned long zero:1;		//2MBページかどうか
		unsigned long ignored1: 4;

		unsigned long address: 40;	//物理アドレス

		unsigned long ignroed2:11;

		unsigned long xd:1;		//実行不可なページか
	} normal __attribute__((packed));

} __attribute__((packed));


//page table entry
union pte {

	unsigned long data;

	struct {

		unsigned long present:1;	//プレゼントフラグ
		unsigned long rw:1;		//書き込み可能か
		unsigned long us:1;		//ユーザーがアクセス可能か
		unsigned long pwt:1;		//ライトスルーか
		unsigned long pcd:1;		//キャッシュが無効か
		unsigned long a:1;			//ユーザーにそのページがアクセスされたか
		unsigned long d:1;			//ユーザーにそのページが書き込まれたか
		unsigned long pat:1;		//attributeタイプ
		unsigned long g:1;			//グローバルかどうか
		unsigned long ignored1: 3;

		unsigned long address: 40;	//物理アドレス

		unsigned long ignroed2:11;

		unsigned long xd:1;		//実行不可なページか
	} __attribute__((packed));
} __attribute__((packed));


enum page_flag {

	PRESENT = 1,
	WRITE = 2,
	USER = 4,
	PWT = 8,
	PCD = 16,
	LARGE = 128,
	GLOBAL = 256,
	XD = 0x8000000000000000
};


class virtual_memory final {



private:

	//カーネルページのpml4の物理アドレス
	unsigned long physical_base_address;

	//カーネルページテーブルの場所
	void *kernel_page_tables;
	unsigned long offset;




	void inline write_cr3(unsigned long phys) {

		asmv("movq %0, %%cr3" ::"r"(phys));
		asmv("invlpg %0" :: "m"(phys));
	}

	unsigned long inline read_cr3() {

		unsigned long ret;
		asmv("movq %%cr3, %0" : "=r"(ret));

		return ret;
	}


	//仮想アドレスからページテーブルのインデックスを各取得する
	inline unsigned long pml4_index(unsigned long addr) {

		return (addr >> 39) & 0x1FF;
	}

	inline union pdpte *get_pdpte(union pml4e pml4) {

		unsigned long phys = pml4.data & PAGE_ADDRESS_MASK;

		return reinterpret_cast<union pdpte*>(phys);
	}

	inline unsigned long pdpt_index(unsigned long addr) {

		return (addr >> 30) & 0x1FF;
	}

	inline union pde *get_pde(union pdpte pdpt) {

		unsigned long phys = pdpt.data & PAGE_ADDRESS_MASK;

		return reinterpret_cast<union pde*>(phys);
	}

	inline unsigned long pd_index(unsigned long addr) {

		return (addr >> 21) & 0x1FF;
	}
	inline unsigned long pt_index(unsigned long virtual_addr) {

		return (virtual_addr >> 12) & 0x1FF;
	}

public:

	void virtual_memory_init();

	void inline apply_kernel_page() {

		this->write_cr3(this->physical_base_address);
	}

	void *alloc_kernel_page();

	void clear_kernel_page() {

		offset = 0;
		kernel_page_tables = nullptr;
	}

	void map_virtual_memory(unsigned long addr, unsigned long flags, bool is2MB);
	void map_virtual_memory(unsigned long addr, unsigned long phys, unsigned long flags, bool is2MB);

	void *alloc_virtual_memory();

};























