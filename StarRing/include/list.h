
#pragma once

#include <stddef.h>


template <typename T>
class list {







public:

	//左方向のポインタ
	class list *prev;

	//右方向のポインタ
	class list *next;



	//リストに追加
	void add_list(class list *data) {

		this->next->prev = data;

		data->next = this->next;
		data->prev = this;

		this->next = data;
	}

	//リストの最後に追加 現在のリストのインスタンスが最初なのでその前が最後
	void add_list_tail(class list *data) {

		this->prev->next = data;

		data->prev = this->prev;
		data->next = this;

		this->prev = data;
	}

	//リストから切り離して初期化する
	void remove() {

		this->prev->next = this->next;
		this->next->prev = this->prev;


		//初期化
		this->prev = this->next = this;
	}

	//リストをtoに移動する
	void move(class list *to) {

		remove();
		to->add_list_tail(this);
	}



	//リストが空だったらnextやprevに自分のポインタが入っているはず
	bool is_empty() {

		return this->next == this;
	}


	T *value(unsigned long offset) {

		return container_of<T>(this->next, reinterpret_cast<unsigned char*>(offset));
	}


	void foreach(unsigned long offset, void (*func)(T*)) {

		class list<T> *pos;
		for(pos = this->next; pos != this; pos = pos->next) {


			func(container_of<T>(pos, reinterpret_cast<unsigned char*>(offset)));
		}
	}


	list() {

		this->prev = this;
		this->next = this;
	}



};




































