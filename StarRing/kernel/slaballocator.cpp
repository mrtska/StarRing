
#include <system.h>
#include <slaballocator.h>
#include <virtualmemory.h>



class slab_allocator slab_allocator;




void slab_allocator::calculate_slab_size(class kmem_cache *cache) {


	if(cache->slab_flags == ON_SLAB) {

		cache->slab_size = MEMORY_BLOCK_SIZE;
		cache->object_count = cache->slab_size - sizeof(struct slab);
		cache->object_count /= cache->object_size + sizeof(kmem_bufctl);
	} else {

		for(cache->slab_size = (MEMORY_BLOCK_SIZE << 1);; cache->slab_size = cache->slab_size << 1) {

			cache->object_count = cache->slab_size / cache->object_size;

			if(cache->object_count != 0) {
				break;
			}
		}
	}
}


void slab_allocator::slab_allocator_init() {


	//キャッシュの名前
	this->kmem_cache_cache.name = "kmem_cache";

	//kmem_cacheなのでclass kmem_cacheのサイズ
	this->kmem_cache_cache.object_size = sizeof(class kmem_cache);
	this->kmem_cache_cache.current_object_count = 0;

	//kmem_cacheは512KiBもないのでオンスラブ
	this->kmem_cache_cache.slab_flags = ON_SLAB;

/*
	if(this->kmem_cache_cache.slab_size >= SLAB_LARGE_OBJECT_SIZE) {

		this->kmem_cache_cache.sl
	}*/


	//スラブのサイズを計算
	calculate_slab_size(&this->kmem_cache_cache);

	//kmem_cacheのリストに入れる 管理用
	kmem_cache_list.add_list_tail(&this->kmem_cache_cache.list);






	kprintf("object_count = %p\n", kmem_cache_create("kmem8", 8));


	kprintf("object_count = %p\n", kmem_cache_create("kmem8", 8));

	kprintf("object_count = %p\n", kmem_cache_create("kmem8", 8));


	kprintf("sizeof kmem_cache %u\n", sizeof(kmem_cache));






	trace();
	STOP;


}


class kmem_cache* slab_allocator::kmem_cache_create(const char *name, size_t size) {


	class kmem_cache *cache = static_cast<class kmem_cache*>(this->kmem_cache_cache.kmem_cache_alloc());



	//kmem_cacheのリストに入れる 管理用
	kmem_cache_list.add_list_tail(&cache->list);


	//キャッシュの名前
	cache->name = name;

	//kmem_cacheなのでclass kmem_cacheのサイズ
	cache->object_size = size;
	cache->current_object_count = 0;


	//スラブがオンスラブかオフスラブか判定
	if(cache->object_size >= SLAB_LARGE_OBJECT_SIZE) {

		this->kmem_cache_cache.slab_flags = OFF_SLAB;
	} else {

		this->kmem_cache_cache.slab_flags = ON_SLAB;
	}


	//スラブのいろいろを計算
	calculate_slab_size(cache);


	return cache;
}



//kmem_cacheを使用してメモリを割り当てる
void *kmem_cache::kmem_cache_alloc() {


	//一部割り当て済みのリストに空きがあればそれを使う
	if(!this->partial.is_empty()) {


		struct slab *slab = this->partial.value(offsetof(&slab::list));

		void *addr = alloc_object(slab);

		//メモリが割り当てられていっぱいになったらリストを移動してアドレスを返す
		if(addr) {

			if(this->object_count <= slab->count) {

				this->full.move(&slab->list);
			}

			return addr;
		}

		//メモリが割り当てられずにスラブがいっぱいになっていたらリストを移動して新しくgrowする
		if(this->object_count <= slab->count) {

			this->full.move(&slab->list);
		}
	}


	//空のスラブが無かったら新しく作る
	if(this->free.is_empty()) {

		kmem_cache_grow();
	}

	struct slab *slab = this->free.value(offsetof(&slab::list));

	void *addr = alloc_object(slab);

	if(addr) {

		this->partial.move(&slab->list);

		return addr;
	} else {

		trace_s("panic.");
		STOP;
		return nullptr;
	}
}


void kmem_cache::kmem_cache_free(void *addr) {









}





void kmem_cache::kmem_cache_grow() {


	struct slab *slab;			//新しく作るスラブ
	kmem_bufctl *kmem_bufctl;	//





	//オンスラブだったら
	if(this->slab_flags == ON_SLAB) {

		slab = static_cast<struct slab*>(virtual_memory.alloc_virtual_memory());

		if(slab == nullptr) {


			trace_s("panic.\n");
			STOP;
		}

		kmem_bufctl = get_kmem_bufctl(slab);

		slab->address = kmem_bufctl + this->object_count;
		slab->count = 0;


	} else {


	}

	//kmem_bufctl領域を初期化
	kmem_bufctl_init(slab);

	//全て解放済みのリストに入れる
	this->free.add_list_tail(&slab->list);

	kprintf("act %p\n", slab);

	this->current_object_count += this->object_count;
}











