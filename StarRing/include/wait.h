
#pragma once

#include <list.h>



extern class waiter waiter;


struct wait_entry {

	class list<struct wait_entry> list;

	int msec;

	int remaining;
};





class waiter {


private:

	class list<struct wait_entry> wait_list;




public:



	void wait_msec(int msec);

	void wait_sec(int sec) {

		wait_msec(sec * 1000);
	}

	class list<struct wait_entry> *get_wait_list() {

		return &this->wait_list;
	}

	waiter() {

		wait_list.reset();
	}


};



























