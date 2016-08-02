#pragma once

#ifndef __GNUC__

#define __attribute()
#define __sync_lock_test_and_set() true
#define __sync_lock_release()

#endif

//�X�s�����b�N�쐬
static void inline __attribute((always_inline)) spin_lock(unsigned char volatile *lock) {

	while (__sync_lock_test_and_set(lock, 0x01)) {

	}
}

//�X�s�����b�N����
static void inline spin_unlock(unsigned char volatile *lock) {

	__sync_lock_release(lock);
}
