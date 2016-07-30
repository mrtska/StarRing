/*
[Module list.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/06/12
*/

#pragma once

#include <stddef.h>

//デバッグ用
#define LIST_POISON1  ((void*) 0x00111100)
#define LIST_POISON2  ((void*) 0x00222200)


struct list_head {

	struct list_head *prev;
	struct list_head *next;
};



//リストを初期化する コンパイル時に初期化する内容が決まってる場合はこちらを使う
#define LIST_HEAD_INIT(name) { &(name), &(name) }


//リストを定義する
#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)






//リストを初期化する コンパイル時に内容が決まっていない場合はこちらを使う インライン関数
static __inline__ void INIT_LIST_HEAD(struct list_head *list) {

	list->next = list;
	list->prev = list;
}

//要素をリストに追加する
static __inline__ void __list_add(struct list_head *new, struct list_head *prev, struct list_head *next) {

	next->prev	= new;
	new->next	= next;
	new->prev	= prev;
	prev->next	= new;
}
static __inline__ void list_add(struct list_head *new, struct list_head *head) {

	__list_add(new, head, head->next);
}

//要素をリストの最後に追加する
static __inline__ void list_add_tail(struct list_head *new, struct list_head *head) {

	__list_add(new, head->prev, head);
}

//要素を削除する
static __inline__ void __list_del(struct list_head *prev, struct list_head *next) {

	next->prev = prev;
	prev->next = next;
}
static __inline__ void __list_del_entry(struct list_head *entry) {

	__list_del(entry->prev, entry->next);
}
static __inline__ void list_del(struct list_head *entry) {

	__list_del(entry->prev, entry->next);
	entry->next = LIST_POISON1;
	entry->prev = LIST_POISON2;
}

//要素を置換する
static __inline__ void list_replace(struct list_head *old, struct list_head *new) {

	new->next		= old->next;
	new->next->prev	= new;
	new->prev		= old->prev;
	new->prev->next	= new;
}
static __inline__ void list_replace_init(struct list_head *old, struct list_head *new) {

	list_replace(old, new);
	INIT_LIST_HEAD(old);
}

//要素を削除後、再初期化する
static __inline__ void list_del_init(struct list_head *entry) {

	__list_del_entry(entry);
	INIT_LIST_HEAD(entry);
}

//リストを移動する
static __inline__ void list_move(struct list_head *list, struct list_head *head) {

	__list_del_entry(list);
	list_add(list, head);
}

//リストをテイルに移動する
static __inline__ void list_move_tail(struct list_head *list, struct list_head *head) {

	__list_del_entry(list);
	list_add_tail(list, head);
}

//そのリストが最後かどうか比較する
static __inline__ int list_is_last(const struct list_head *list, const struct list_head *head) {

	return list->next == head;
}

//リストが空かどうか比較する
static __inline__ int list_empty(const struct list_head *head) {

	return head->next == head;
}

//リストが空かどうか比較する
static __inline__ int list_empty_careful(struct list_head *head) {

	struct list_head *next = head->next;
	return (next == head) && (next == head->prev);
}

//リストを左に回転する
static __inline__ void list_rotate_left(struct list_head *head) {

	struct list_head *first;

	if(!list_empty(head)) {

		first = head->next;
		list_move_tail(first, head);
	}
}

//リストが単一のエントリのみを持っているか比較
static __inline__ int list_is_singular(const struct list_head *head) {

	return !list_empty(head) && (head->next == head->prev);
}


static __inline__ void __list_cut_position(struct list_head *list, struct list_head *head, struct list_head *entry) {

	struct list_head *new_first = entry->next;
	list->next			= head->next;
	list->next->prev	= list;
	list->prev			= entry;
	entry->next			= list;
	head->next			= new_first;
	new_first->prev		= head;
}

//リストをカットする（？）
static __inline__ void list_cut_position(struct list_head *list, struct list_head *head, struct list_head *entry) {

	if(list_empty(head)) {
		return;
	}

	if(list_is_singular(head) && (head->next != entry && head != entry)) {
		return;
	}

	if(entry == head) {
		INIT_LIST_HEAD(list);
	} else {
		__list_cut_position(list, head, entry);
	}
}

//ノードをリストに挿入する
static __inline__ void __list_splice(const struct list_head *list, struct list_head *prev, struct list_head *next) {

	struct list_head *first		= list->next;
	struct list_head *last		= list->prev;

	first->prev	= prev;
	prev->next	= first;

	last->next	= next;
	next->prev	= last;
}

//リストを結合する
static __inline__ void list_splice(const struct list_head *list, struct list_head *head) {

	if(!list_empty(list)) {
		__list_splice(list, head, head->next);
	}
}

//リストをテイルに結合する
static __inline__ void list_splice_tail(const struct list_head *list, struct list_head *head) {

	if(!list_empty(list)) {
		__list_splice(list, head->prev, head);
	}
}

//リストを結合する その後初期化する
static __inline__ void list_splice_init(struct list_head *list, struct list_head *head){

	if(!list_empty(list)) {
		__list_splice(list, head, head->next);
		INIT_LIST_HEAD(list);
	}
}

//リストをテイルに結合する その後初期化する
static __inline__ void list_splice_tail_init(struct list_head *list, struct list_head *head) {

	if(!list_empty(list)) {
		__list_splice(list, head->prev, head);
		INIT_LIST_HEAD(list);
	}
}

//リストからエントリを取得する
#define list_entry(ptr, type, member) container_of(ptr, type, member)

//リストから最初のエントリを取得する
#define list_first_entry(ptr, type, member) list_entry((ptr)->next, type, member)

//リストから最後のエントリを取得する
#define list_last_entry(ptr, type, member) list_enry((ptr)->prev, type, member)

//リストから最初のエントリを取得する リストがNULLだった場合、Nullを返す
#define list_first_entry_or_null(ptr, type, member) (!list_empty(ptr) ? list_first_entry(ptr, type, member) : NULL)

//リストから次のエントリを取得する
#define list_next_entry(pos, member) list_entry((pos)->member.next, typeof(*(pos)), member)

//リストから前のエントリを取得する
#define list_prev_entry(pos, member) list_entry((pos)->member.prev, typeof(*(pos)), member)

//リストをイテレートする
#define list_for_each(pos, head) for(pos = (head)->next; pos != (head); pos = pos->next)

//リストを後ろにイテレートする
#define list_for_each_prev(pos, head) for(pos = (head)->prev; pos != (head); pos = pos->prev)

//リストをイテレートする ループカーソルを指定する
#define list_for_each_safe(pos, n, head) for(pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

//リストを後ろにイテレートする ループカーソルを指定する
#define list_for_each_prev_safe(pos, n, head) for(pos = (head)->prev, n = pos->prev; pos != (head); pos = n, n = pos->prev)

//リストからエントリを取得する
#define list_prepare_entry(pos, head, member) ((pos) ? : list_entry(head, typeof(*pos), member))

//途中からリストをイテレートする
#define list_for_each_entry_continue(pos, head, member) for(pos = list_next_entry(pos, member); &pos->member != (head); pos = list_next_entry(pos, member))

//途中から逆順にリストをイテレートする
#define list_for_each_entry_continue_reverse(pos, head, member) for(pos = list_prev_entry(pos, member); &pos->member != (head); pos = list_prev_entry(pos, member))

//現在のループカーソルからリストをイテレートする
#define list_for_each_entry_from(pos, head, member) for(; &pos->member != (head); pos = list_next_entry(pos, member))

//カーソル位置を保存してからリストのノードをイテレートする
#define list_for_each_entry_safe(pos, n, head, member) for(pos = list_first_entry(head, typeof(*pos), member), n = list_next_entry(pos, member); &pos->member != (head); pos = n, n = list_next_entry(n, member))

//カーソル位置を保存して途中からリストのノードをイテレートする
#define list_for_each_entry_safe_continue(pos, n, head, member) for(pos = list_next_entry(pos, member), n = list_next_entry(pos, member); &pos->member != (head); pos = n, n = list_next_entry(n, member))

//現在のカーソル位置からリストのノードをイテレートする
#define list_for_each_entry_safe_from(pos, n, head, member) for(n = list_next_entry(pos, member); &pos->member != (head); pos = n, n = list_next_entry(n, member))

//カーソル位置を保存してからリストのノードを逆順にイテレートする
#define list_for_each_entry_safe_reverse(pos, n, head, member) for(pos = list_last_entry(head, typeof(*pos), member),		\ n = list_prev_entry(pos, member); &pos->member != (head); pos = n, n = list_prev_entry(n, member))

//ループ状態を保存する
#define list_safe_reset_next(pos, n, member) n = list_next_entry(pos, member)


