#pragma once


//�X�s�����b�N�쐬
static void __inline__ __attribute((always_inline)) spin_lock(unsigned char volatile *lock) {

	while (__sync_lock_test_and_set(lock, 0x01)) {

	}
}

//�X�s�����b�N����
static void __inline__ spin_unlock(unsigned char volatile *lock) {

	__sync_lock_release(lock);
}
