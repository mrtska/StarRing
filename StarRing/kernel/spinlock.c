/*
[Module ]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/04/27
*/

#include <spinlock.h>
/*


void __inline__ __attribute((always_inline)) spin_lock(unsigned char volatile *lock) {

	while(__sync_lock_test_and_set(lock, 0x01)) {

	}
}

void __inline__ spin_unlock(unsigned char volatile *lock) {

	__sync_lock_release(lock);
}

*/

