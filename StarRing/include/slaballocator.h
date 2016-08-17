
#pragma once

#include <list.h>
#include <system.h>
#include <physicalmemory.h>

#define SLAB_LARGE_OBJECT_SIZE MEMORY_BLOCK_SIZE / 4

extern class slab_allocator slab_allocator;


typedef unsigned int kmem_bufctl;

enum slab_flags {

	ON_SLAB,	//スラブの情報がオブジェクトと同じページにある
	OFF_SLAB,	//スラブの情報がオブジェクトと同じページに無い

	KMEM_BUFCTL_END = 0xFFFFFFFF,
	KMEM_BUFCTL_ALLOCATED = 0xFFFFFFFE
};



struct slab {

	//スラブを管理するためのリスト
	class list<struct slab> list;

	//オブジェクトの最初のアドレス
	void *address;

	//使用中のオブジェクトの数
	int count;

	//フリーのオブジェクトの位置
	kmem_bufctl free;
};




class kmem_cache {

	friend class slab_allocator;

private:

	//キャッシュの名前
	const char *name;

	//kmem_cacheのリスト
	class list<class kmem_cache> list;


	//スラブのサイズ
	size_t slab_size;

	//オブジェクトのサイズ
	size_t object_size;

	//オブジェクトの最大数
	int object_count;

	//現在のオブジェクトの数
	int current_object_count;

	enum slab_flags slab_flags;



	//一部使用済みスラブのリスト
	class list<struct slab> partial;

	//全て使用済みスラブのリスト
	class list<struct slab> full;

	//未使用スラブのリスト
	class list<struct slab> free;


	//スラブの構造体の次のアドレスがkmem_bufctl
	kmem_bufctl *get_kmem_bufctl(struct slab *slab) {

		return reinterpret_cast<kmem_bufctl*>(slab + 1);
	}

	void kmem_bufctl_init(struct slab *slab) {

		//一番最初の部分
		kmem_bufctl *kmem_bufctl = get_kmem_bufctl(slab);


		//全て解放済みなので順番にインデックスする
		for(int i = 0; i < this->object_count - 1; i++) {

			kmem_bufctl[i] = i + 1;
		}

		//一番最後はダメ
		kmem_bufctl[this->object_count - 1] = KMEM_BUFCTL_END;

		//最初に割り当てる場所は0番目
		slab->free = 0;
	}

	//スラブからオブジェクトを割り当てる
	void *alloc_object(struct slab *slab) {


		kmem_bufctl free;

		//スラブがいっぱいだったら論外
		if(slab->free != KMEM_BUFCTL_END) {

			kmem_bufctl *kmem_bufctl = get_kmem_bufctl(slab);

			free = slab->free;
			slab->free = kmem_bufctl[slab->free];
			kmem_bufctl[free] = KMEM_BUFCTL_ALLOCATED;
			slab->count++;

			return reinterpret_cast<void*>(reinterpret_cast<unsigned long>(slab->address) + (this->object_size * slab->free));
		} else {

			return nullptr;
		}
	}



public:



	const char *get_name() {

		return this->name;
	}


	//メモリを割り当てる
	void *kmem_cache_alloc();

	void kmem_cache_free(void *addr);

	//キャッシュを広げる
	void kmem_cache_grow();



};






class slab_allocator final {


private:


	//kmem_cacheのキャッシュ
	class kmem_cache kmem_cache_cache;


	//kmem_cacheのリスト
	class list<class kmem_cache> kmem_cache_list;


	void calculate_slab_size(class kmem_cache *cache);


public:



	void slab_allocator_init();


	struct kmem_cache *kmem_cache_create(const char *name, size_t size);


	//カーネルメモリを割り当てる
	void *kmalloc(size_t size);

	//カーネルメモリを開放する
	void kfree(void *addr);


	class list<class kmem_cache> get_kmem_cache_list() {

		return this->kmem_cache_list;
	}

};




















