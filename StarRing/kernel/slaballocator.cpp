
#include <system.h>
#include <slaballocator.h>
#include <virtualmemory.h>




class slab_allocator slab_allocator;



static const struct kmalloc_cache kmalloc_str[KMALLOC_DEFINE_COUNT] = {
		{ 8,		"kmalloc-8"		},
		{ 16,		"kmalloc-16"	},
		{ 32,		"kmalloc-32"	},
		{ 64,		"kmalloc-64"	},
		{ 96,		"kmalloc-96"	},
		{ 128,		"kmalloc-128"	},
		{ 192,		"kmalloc-192"	},
		{ 256,		"kmalloc-256"	},
		{ 512,		"kmalloc-512"	},
		{ 1024,		"kmalloc-1024"	},
		{ 2048,		"kmalloc-2048"	},
		{ 4096,		"kmalloc-4096"	},
		{ 8192,		"kmalloc-8192"	},
		{ 16384,	"kmalloc-16384"	},
		{ 32768,	"kmalloc-32768"	},
		{ 65536,	"kmalloc-65536"	}
};


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


	//スラブのサイズを計算
	calculate_slab_size(&this->kmem_cache_cache);

	//kmem_cacheのリストに入れる 管理用
	kmem_cache_list.add_list_tail(&this->kmem_cache_cache.list);


	for(int i = 0; i < KMALLOC_DEFINE_COUNT; i++) {

		struct kmalloc_cache kmallocc = kmalloc_str[i];

		this->kmalloc_list[i] = this->kmem_cache_create(kmallocc.name, kmallocc.size);

	}
}

void *slab_allocator::kmalloc(unsigned long size) {


	for(int i = 0; i < KMALLOC_DEFINE_COUNT; i++) {

		class kmem_cache *cache = this->kmalloc_list[i];

		if(cache->object_size >= size) {

			return cache->kmem_cache_alloc();
		}


	}


	trace_s("panic. kmalloc");
	STOP;
	return nullptr;
}

void slab_allocator::kfree(void *addr) {


	for(int i = 0; i < KMALLOC_DEFINE_COUNT; i++) {


		class kmem_cache *cache = this->kmalloc_list[i];

		bool ret = false;

		struct slab *slab = reinterpret_cast<struct slab*>(reinterpret_cast<unsigned long>(addr) & ~(MEMORY_BLOCK_SIZE - 1));


		cache->partial.foreach_safe(offsetof(&slab::list), [slab, &ret, cache, addr](struct slab *list_slab) {


			if(ret) {

				return;
			}

			if(slab == list_slab) {

				cache->kmem_cache_free(addr);
				ret = true;
			}
		});

		if(ret) {

			return;
		}

		cache->full.foreach(offsetof(&slab::list), [slab, &ret, cache, addr](struct slab *list_slab) {



			if(ret) {

				return;
			}

			if(slab == list_slab) {

				cache->kmem_cache_free(addr);
				ret = true;
			}
		});


		if(ret) {

			return;
		}
	}

	kprintf("warning. kfree %p\n", addr);
	STOP;
}



class kmem_cache* slab_allocator::kmem_cache_create(const char *name, unsigned long size) {


	class kmem_cache *cache = static_cast<class kmem_cache*>(this->kmem_cache_cache.kmem_cache_alloc());


	cache->list.reset();
	cache->free.reset();
	cache->partial.reset();
	cache->full.reset();

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


		struct slab *slab = this->partial.first(offsetof(&slab::list));

		void *addr = alloc_object(slab);


		//メモリが割り当てられていっぱいになったらリストを移動してアドレスを返す
		if(addr) {

			if(this->object_count <= slab->count) {

				slab->list.move(&this->full);
			}

			return addr;
		}

		//メモリが割り当てられずにスラブがいっぱいになっていたらリストを移動して新しくgrowする
		if(this->object_count <= slab->count) {

			slab->list.move(&this->full);
		}

	}

	//空のスラブが無かったら新しく作る
	if(this->free.is_empty()) {

		kmem_cache_grow();
	}

	struct slab *slab = this->free.first(offsetof(&slab::list));

	void *addr = alloc_object(slab);




	if(addr) {

		slab->list.move(&this->partial);

		//kprintf("new alloc %p\n", addr);

		return addr;
	} else {

		trace_s("panic.");
		STOP;
		return nullptr;
	}
}


void kmem_cache::kmem_cache_free(void *addr) {


	struct slab *slab;


	if(this->slab_flags == ON_SLAB) {

		slab = reinterpret_cast<struct slab*>(reinterpret_cast<unsigned long>(addr) & ~(MEMORY_BLOCK_SIZE - 1));

	} else {


		trace_s("panic. kmem_cache_free");
		STOP;

	}

	unsigned int index = (reinterpret_cast<unsigned long>(addr) - reinterpret_cast<unsigned long>(slab->address)) / this->object_size;

	kmem_bufctl *kmem_bufctl = get_kmem_bufctl(slab);


	if(kmem_bufctl[index] == KMEM_BUFCTL_ALLOCATED) {


		kmem_bufctl[index] = slab->free;
		slab->free = index;
		slab->count--;

		if(slab->count == 0) {

			slab->list.move(&this->free);
		}

	}
}

void kmem_cache::kmem_cache_grow() {


	struct slab *slab;			//新しく作るスラブ
	kmem_bufctl *kmem_bufctl;	//

	//オンスラブだったら
	if(this->slab_flags == ON_SLAB) {

		slab = static_cast<struct slab*>(virtual_memory.alloc_virtual_memory());
		slab->list.reset();

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

	this->current_object_count += this->object_count;
}












