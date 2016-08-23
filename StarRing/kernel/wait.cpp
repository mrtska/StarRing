

#include <system.h>
#include <wait.h>
#include <slaballocator.h>


class waiter waiter;




void waiter::wait_msec(int msec) {


	struct wait_entry *entry = new struct wait_entry;

	entry->msec = msec;
	entry->remaining = msec;

	this->wait_list.add_list_tail(&entry->list);

	//タイマーの割り込みがremainingを減らしているのでいつかは抜けられるはず
	while(entry->remaining > 0) {

		asmv("hlt");
	}
}


























