/*
[Module slab.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/08/12
*/

#include <slab.h>
#include <internal/stdio.h>
#include <spinlock.h>


static int kmem_cache_grow(struct kmem_cache *cache, int flags);
static int kmem_cache_reap(struct kmem_cache *cache, struct list_head *list);
static int setup_initial_cache(struct kmem_cache *cache, const char *name, size_t size);
static __inline__ struct kmem_cache *alloc_kmem_cache(void);

struct kmem_cache kmalloc_cache[E_KMALLOC_NUM];

//汎用キャッシュ
static const struct initial_cache initial_cache[E_KMALLOC_NUM] = {
	{ 8,		"kmalloc-8"        },
	{ 16,		"kmalloc-16"       },
	{ 32,		"kmalloc-32"       },
	{ 64,		"kmalloc-64"       },
	{ 96,		"kmalloc-96"       },
	{ 128,		"kmalloc-128"      },
	{ 192,		"kmalloc-192"      },
	{ 256,		"kmalloc-256"      },
	{ 512,		"kmalloc-512"      },
	{ 1024,		"kmalloc-1024"     },
	{ 2048,		"kmalloc-2048"     },
	{ 4096,		"kmalloc-4096"     },
	{ 8192,		"kmalloc-8192"     },
	{ 16384,	"kmalloc-16384"    },
	{ 32768,	"kmalloc-32768"    },
	{ 65536,	"kmalloc-65536"    },
	{ 131072,	"kmalloc-131072"   },
};




//kmem_cacheの名前
static const char kmem_cache_name[] = "kmem_cache";
//kmem_cacheのキャッシュ
static struct kmem_cache kmem_cache_cache;

//全てのkmem_cacheをこのリストで連結する
struct list_head list_cache;

//malloc lock
static unsigned char malloc_lock;

//動的メモリ割り当て
void *kmalloc(size_t size, int flags) {

	spin_lock(&malloc_lock);
	E_KMALLOC size_index;
	void *object;

	for(size_index = E_KMALLOC_8; size_index < E_KMALLOC_NUM; size_index++) {

		if(size <= initial_cache[size_index].size) {

			object = kmem_cache_alloc(&kmalloc_cache[size_index], flags);


			if(object == NULL) {

				continue;
			}

			//kprintf("[kmalloc] id %d, return %p\n", get_apic_id(), object);
			spin_unlock(&malloc_lock);
			return object;
		}
	}

	spin_unlock(&malloc_lock);
	kprintf("[kmalloc] can not allocate memory size %X\n", size);
	return NULL;
}

//動的メモリ解放
void kfree(void *object, size_t size) {

	E_KMALLOC size_index;


	memset(object, 0x00, size);

	for(size_index = E_KMALLOC_8; size_index < E_KMALLOC_NUM; size_index++) {

		if(size <= initial_cache[size_index].size) {

			kmem_cache_free(&kmalloc_cache[size_index], object);
			break;
		}
	}
}

/*
void *malloc(size_t size) {

	spin_lock(&malloc_lock);

	E_KMALLOC size_index;
	void *object;

	for(size_index = E_KMALLOC_8; size_index < E_KMALLOC_NUM; size_index++) {

		if(size <= initial_cache[size_index].size) {

			object = kmem_cache_alloc(&kmalloc_cache[size_index], 0);

			if(object == NULL) {
				kprintf("WARNING\n");
				continue;
			}
			spin_unlock(&malloc_lock);
			return object;
		}
	}
	kprintf("[slaballocater] over size! size = %p\n", size);
	char buf[32];
	sprintf(buf, "%u", size);

	struct kmem_cache *cache = kmem_cache_create(buf, size, 0, 0, NULL);

	spin_unlock(&malloc_lock);

	return kmem_cache_alloc(cache, 0);

}*/
void free(void *obj) {



}



//スラブのサイズを計算する
static int calc_slab_size(struct kmem_cache *cache) {

	unsigned int slab_size;
	unsigned int obj_num;

	//オンスラブの処理
	if(!is_off_slab(cache)) {

		slab_size = PAGE_SIZE;
		obj_num = slab_size - sizeof(struct slab);
		obj_num /= cache->obj_size + sizeof(kmem_bufctl);
	} else {	//オフスラブの処理

		for(slab_size = (PAGE_SIZE << 1); slab_size < SLAB_MAX_PAGE_SIZE; slab_size = slab_size << 1) {

			obj_num = slab_size / cache->obj_size;

			if(obj_num != 0) {
				break;
			}
		}
	}

	//オブジェクトのサイズが最大サイズを上回った場合
	if(obj_num == 0) {
		slab_size = 0;
		return SLAB_ERROR;
	}

	cache->slab_size = slab_size;
	cache->obj_num = obj_num;

	return SLAB_OK;
}





//kmem_cacheを初期化する
static int init_kmem_cache(struct kmem_cache *cache, const char *name, size_t obj_size, unsigned int align, unsigned int flags, void (*constructor) (void*)) {

	int status = SLAB_OK;


	//kmem_cacheを管理する情報を初期化
	INIT_LIST_HEAD(&cache->list);
	cache->name = name;

	//オブジェクトの属性を初期化
	cache->obj_size = obj_size;
	cache->total_obj_num = 0;

	//スラブの属性を初期化
	if(SLAB_LARGE_OBJECT_SIZE <= cache->obj_size) {

		cache->flags |= SLAB_FLAG_OFF_SLAB;			//オフスラブに設定
	}

	status = calc_slab_size(cache);

	if(status != SLAB_OK) {
		kprintf("error! init_kmem_cache!!\n");
		return status;
	}

	//コンストラクタを設定
	cache->constructor = constructor;

	INIT_LIST_HEAD(&cache->list_partial);
	INIT_LIST_HEAD(&cache->list_full);
	INIT_LIST_HEAD(&cache->list_free);

	return status;
}



//最初のキャッシュをセットアップする
static int setup_initial_cache(struct kmem_cache *cache, const char *name, size_t size) {

	int status;
	status = init_kmem_cache(cache, name, size, 0, SLAB_FLAG_INIT_CACHE, NULL);


	if(status != SLAB_OK) {

		return status;
	}

	status = calc_slab_size(cache);

	if(status != SLAB_OK) {

		return status;
	}

	list_add_tail(&cache->list, &list_cache);

	if(is_on_slab(cache)) {

		status = kmem_cache_grow(cache, SLAB_FLAG_INIT_CACHE);
	}

	return status;
}

//スラブアロケーター初期化
int init_slaballocator(void) {

	int i;
	int status;

	INIT_LIST_HEAD(&list_cache);

	for(i = 0; i < E_KMALLOC_NUM; i++) {

		status = setup_initial_cache(&kmalloc_cache[i], initial_cache[i].name, initial_cache[i].size);

		if(status != SLAB_OK) {

			return status;
		}
	}

	status = setup_initial_cache(&kmem_cache_cache, kmem_cache_name, sizeof(kmem_cache_cache));

	return status;
}

//kmem_cacheを割り当てる
static __inline__ struct kmem_cache *alloc_kmem_cache(void) {

	return (kmem_cache_alloc(&kmem_cache_cache, 0));
}

static __inline__ void free_kmem_cache(struct kmem_cache *cache) {

	kmem_cache_free(&kmem_cache_cache, cache);
}

//キャッシュ生成
struct kmem_cache *kmem_cache_create(const char *name, size_t size, unsigned int align, unsigned int flags, void (*constructor)(void*)) {

	struct kmem_cache *cache;
	int status;

	//kmem_cache_cacheからkmem_cacheを割り当てる
	cache = alloc_kmem_cache();

	if(cache == NULL) {
		kprintf("[kmem_cache_create] ERROR!\n");
		return NULL;
	}

	//キャッシュを初期化する
	status = init_kmem_cache(cache, name, size, align, flags, constructor);

	//正しく初期化できなかった場合
	if(status != SLAB_OK) {
		kprintf("[kmem_cache_create] ERROR!\n");
		free_kmem_cache(cache);
		return NULL;
	}

	//キャッシュをキャッシュリストの最後に追加する
	list_add_tail(&cache->list, &list_cache);

	return cache;
}

//リストからスラブを取得する
static __inline__ struct slab *get_slab_list_entry(struct list_head *list) {

	return list_first_entry(list, struct slab, list_slab);
}

//一部割り当て済みリストからスラブを取得する
static __inline__ struct slab *get_partial_slab(struct kmem_cache *cache) {

	return get_slab_list_entry(&cache->list_partial);
}

//完全に開放済みリストからスラブを取得する
static __inline__ struct slab *get_free_slab(struct kmem_cache *cache) {

	return get_slab_list_entry(&cache->list_free);
}




#define SLAB_KMEM_BUFCTL_END			((~0UL))
#define SLAB_KMEM_BUFCTL_ALLOCATED		(((~0UL) - 1))

//スラブからkmem_bufctlを取得する
static __inline__ kmem_bufctl *get_kmem_bufctl(struct slab *slab) {

	return ((kmem_bufctl*) (slab + 1));
}

//キャッシュとスラブからオブジェクトを取得する
static __inline__ void *get_object(struct kmem_cache *cache, struct slab *slab, unsigned int index) {

	return ((void*) ((unsigned long) slab->s_mem + cache->obj_size * index));
}

//フリーのオブジェクトを割り当てる関数
void *alloc_free_object(struct kmem_cache *cache, struct slab *slab) {

	kmem_bufctl free;
	kmem_bufctl *kmem_bufctl;


	if(slab->free != SLAB_KMEM_BUFCTL_END) {


		kmem_bufctl = get_kmem_bufctl(slab);

		free = slab->free;
		slab->free = kmem_bufctl[slab->free];
		kmem_bufctl[free] = SLAB_KMEM_BUFCTL_ALLOCATED;
		slab->count++;


		return (get_object(cache, slab, free));
	}

	return (NULL);
}

//キャッシュ割り当て
void *kmem_cache_alloc(struct kmem_cache *cache, int flags) {

	int status;
	struct slab *slab;
	void *allocated;


	if(!list_empty(&cache->list_partial)) {

		slab = get_partial_slab(cache);

		allocated = alloc_free_object(cache, slab);


		if(allocated) {
			if(cache->obj_num <= slab->count) {
				list_move(&slab->list_slab, &cache->list_full);
			}
		} else {

			kprintf("kmem_cache_alloc#allocated1\n");
		}

		return allocated;
	} else {

		status = SLAB_OK;
		if(list_empty(&cache->list_free)) {

			status = kmem_cache_grow(cache, flags);
		}


		if(status == SLAB_OK) {

			slab = get_free_slab(cache);
			allocated = alloc_free_object(cache, slab);

			if(allocated) {

				list_move(&slab->list_slab, &cache->list_partial);
			} else {

				kprintf("kmem_cache_alloc#allocated2 %p\n", allocated);
			}

			return allocated;
		} else {

			kprintf("kmem_cache_alloc#error\n");

			return NULL;
		}

	}

}

//オブジェクトからスラブを取得する
static __inline__ struct slab *get_on_slab(void *object) {

	return (struct slab*)(((unsigned long)object)& ~(PAGE_SIZE - 1));
}


static __inline__ bool is_object_within_slab(struct kmem_cache *cache, struct slab *slab, void *object) {

	unsigned long start_obj;
	unsigned long end_obj;

	start_obj = (unsigned long)get_object(cache, slab, 0);
	end_obj = start_obj + cache->obj_size * (cache->obj_num + 1) - 1;


	return ((start_obj <= (unsigned long)object) && ((unsigned long)object <= end_obj));
}

//リストのノードの中にオブジェクトが無いか探索する
static __inline__ __attribute__((always_inline)) bool walk_list_test_object_within(struct list_head *list, struct kmem_cache *cache, struct slab *slab, void *object) {

	struct list_head *pos;

	list_for_each(pos, list) {

		slab = list_entry(pos, struct slab, list_slab);

		if(is_object_within_slab(cache, slab, object)) {
			return 1;
		}
	}
	slab = NULL;
	return 0;
}

//オブジェクトからインデックスを取得する
static __inline__ unsigned int get_object_index(struct kmem_cache *cache, struct slab *slab, void *object) {


	unsigned long index = ((unsigned long) object) - ((unsigned long)slab->s_mem);

	index /= cache->obj_size;

	return (unsigned int) index;
}

void kmem_cache_free(struct kmem_cache *cache, void *object) {

	struct slab *slab;
	unsigned int obj_index;
	//kmem_bufctl bufctl_temp;
	kmem_bufctl *kmem_bufctl;
	struct list_head *list;

	slab = NULL;

	//オンスラブの場合
	if(is_on_slab(cache)) {

		slab = get_on_slab(object);
	} else {	//オフスラブの場合

		list = NULL;
		//一部割り当て済みリストが空でない場合
		if(!list_empty(&cache->list_partial)) {

			list = &cache->list_partial;
		}

		if(list) {
			//一部割り当て済みリストを探索する
			walk_list_test_object_within(list, cache, slab, object);
		}

		if(slab == NULL) {

			list = NULL;
			//全て割り当て済みリストが空でない場合
			if(!list_empty(&cache->list_full)) {

				list = &cache->list_full;
			}

			if(list) {
				//全て割り当て済みリストを探索する
				walk_list_test_object_within(list, cache, slab, object);
			}
		}
	}

	//スラブがNULLでない場合
	if(slab) {

		obj_index = get_object_index(cache, slab, object);

		kmem_bufctl = get_kmem_bufctl(slab);

		if(kmem_bufctl[obj_index] == SLAB_KMEM_BUFCTL_ALLOCATED) {

			kmem_bufctl[obj_index] = slab->free;

			slab->free = obj_index;
			slab->count--;

			if(slab->count == 0) {

				list_move(&slab->list_slab, &cache->list_free);
			}
		}
	}
}

void kmem_cache_destroy(struct kmem_cache *cache) {

	int status = SLAB_ERROR;

	if(!list_empty(&cache->list_partial)) {

		status = kmem_cache_reap(cache, &cache->list_partial);
	}

	if(!list_empty(&cache->list_full)) {

		status = kmem_cache_reap(cache, &cache->list_full);
	}
	if(!list_empty(&cache->list_free)) {

		status = kmem_cache_reap(cache, &cache->list_free);
	}

	if(cache && (status = SLAB_OK)) {

		list_del(&cache->list);

		free_kmem_cache(cache);
	}

}

static __inline__ void init_slab(struct slab *slab, void *s_mem) {

	INIT_LIST_HEAD(&slab->list_slab);
	slab->s_mem = s_mem;
	slab->count = 0;
}

static __inline__ void init_kmem_bufctl(struct kmem_cache *cache, struct slab *slab) {

	int i;
	kmem_bufctl *kmem_bufctl = get_kmem_bufctl(slab);
	for(i = 0; i < (cache->obj_num - 1); i++) {

		kmem_bufctl[i] = i + 1;
	}

	kmem_bufctl[cache->obj_num - 1] = SLAB_KMEM_BUFCTL_END;

	slab->free = 0;
}




int kmem_cache_grow(struct kmem_cache *cache, int flags) {

	struct slab *slab;
	void *s_mem;
	kmem_bufctl *kmem_bufctl;
	int i;
	void *object;

	//オフスラブの場合
	if(is_off_slab(cache)) {

		slab = (struct slab*) kmalloc(sizeof(struct slab) + sizeof(kmem_bufctl) * cache->obj_num, 0);

		if(slab == NULL) {
			kprintf("error! kmem_cache_grow#off_slab\n");
			return SLAB_ERROR;
		}

		s_mem = alloc_kernel_memory(cache->slab_size);
	} else { //オンスラブの場合

		slab = (struct slab*) alloc_kernel_memory(cache->slab_size);

		if(slab == NULL) {
			kprintf("error! kmem_cache_grow#on_slab\n");
			return SLAB_ERROR;
		}

		kmem_bufctl = get_kmem_bufctl(slab);

		s_mem = (void*) (kmem_bufctl + cache->obj_num);
	}

	init_slab(slab, s_mem);

	init_kmem_bufctl(cache, slab);

	list_add_tail(&slab->list_slab, &cache->list_free);

	cache->total_obj_num += cache->obj_num;

	if(cache->constructor != NULL) {

		for(i = 0; i < cache->obj_num; i++) {

			object = get_object(cache, slab, i);
			cache->constructor(object);
		}
	}

	return SLAB_OK;
}

int kmem_cache_reap(struct kmem_cache *cache, struct list_head *list) {

	struct list_head *pos;
	struct slab *slab;

	list_for_each(pos, list)
	{

		slab = list_entry(pos, struct slab, list_slab);

		if(slab->count) {

			return SLAB_ERROR;
		}

		if(is_on_slab(cache)) {

			free_kernel_memory((void*) slab);
		} else {

			free_kernel_memory(slab->s_mem);
			kfree((void*) slab, cache->slab_size);
		}
	}

	return SLAB_OK;
}

