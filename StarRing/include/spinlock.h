#pragma once


//スピンロック作成
static void __inline__ __attribute((always_inline)) spin_lock(unsigned char volatile *lock) {

	while (__sync_lock_test_and_set(lock, 0x01)) {

	}
}

//スピンロック解除
static void __inline__ spin_unlock(unsigned char volatile *lock) {

	__sync_lock_release(lock);
}
