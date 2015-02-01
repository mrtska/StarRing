/*
 * slab.h
 *
 *  Created on: 2014/07/30
 *      Author: StarRing
 */

#pragma once

#include <string.h>
#include <list.h>
#include <system.h>
#include <mem/phys.h>


typedef unsigned long kmem_bufctl;



struct initial_cache{

	size_t size;
	const char name[15];
};

typedef enum {

	E_KMALLOC_8,
	E_KMALLOC_16,
	E_KMALLOC_32,
	E_KMALLOC_64,
	E_KMALLOC_96,
	E_KMALLOC_128,
	E_KMALLOC_192,
	E_KMALLOC_256,
	E_KMALLOC_512,
	E_KMALLOC_1024,
	E_KMALLOC_2048,
	E_KMALLOC_4096,
	E_KMALLOC_8192,
	E_KMALLOC_16384,
	E_KMALLOC_32768,
	E_KMALLOC_65536,
	E_KMALLOC_131072,
	E_KMALLOC_NUM,
} E_KMALLOC;




//スラブ構造体
struct slab {

	struct list_head list_slab;
	unsigned long color_offset;
	void *s_mem;
	unsigned int count;
	kmem_bufctl free;
};

//ページ情報を格納する
struct page {

	unsigned long base;		//物理アドレス
	unsigned long addr;		//仮想アドレス カーネル領域
};

//kmem_cache構造体 スラブアロケーターの核
struct kmem_cache {

	//キャッシュ管理に使用
	const char *name;
	struct list_head list;

	//スラブの属性
	unsigned int slab_size;
	int align;
	size_t color_num;
	unsigned int color_algin;
	unsigned int color_next;
	unsigned int flags;

	//コンストラクタ
	void (*constructor)(void *obj);

	//リスト
	struct list_head list_partial;	//一部使用済みリスト
	struct list_head list_full;		//全て使用済みリスト
	struct list_head list_free;		//全て開放済みリスト
	unsigned int free_limit;		//開放上限

	//オブジェクトの属性
	size_t obj_size;
	unsigned int obj_num;
	unsigned int  total_obj_num;

	struct page page;	//ページ情報
};


extern struct kmem_cache kmalloc_cache[E_KMALLOC_NUM];


//スラブアロケーターを初期化
int init_slaballocator(void);


//キャッシュを作成する
struct kmem_cache *kmem_cache_create(const char *name, size_t size, unsigned int align, unsigned int flags, void (*constructor)(void*));


//キャッシュを割り当てる
void *kmem_cache_alloc(struct kmem_cache *cache, int flags);


//キャッシュを開放する
void kmem_cache_free(struct kmem_cache *cp, void *buf);


//キャッシュを破壊する
void kmem_cache_destroy(struct kmem_cache *cp);


void *malloc(size_t size);
void free(void *obj);



void *kmalloc(size_t size, int flags);
void kfree(void *object, size_t size);



#define PAGE_SIZE	0x200000


//オブジェクトの最大サイズ これを超えるとオフスラブになる
#define SLAB_LARGE_OBJECT_SIZE	PAGE_SIZE / 4


//スラブのフラグに関するマクロ
#define SLAB_FLAG_ON_SLAB		0x00000000
#define SLAB_FLAG_OFF_SLAB		0x00000001
#define SLAB_FLAG_INIT_CACHE	0x00000002

//ステータス管理
#define SLAB_OK					0x00000000
#define SLAB_ERROR				0x00000001

//オブジェクトの最最大サイズ これを超えるとエラーになる
#define SLAB_MAX_PAGE_SIZE		PAGE_SIZE << 4





//キャッシュがオフスラブか判定する
static __inline__ bool is_off_slab(struct kmem_cache *cache) {

	return (SLAB_FLAG_OFF_SLAB == (cache->flags & SLAB_FLAG_OFF_SLAB));
}

//キャッシュがオンスラブか判定する
static __inline__ bool is_on_slab(struct kmem_cache *cache) {

	return (SLAB_FLAG_ON_SLAB == (cache->flags & SLAB_FLAG_ON_SLAB));
}








