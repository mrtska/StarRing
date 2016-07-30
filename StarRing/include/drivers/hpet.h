/*
[Module hpet.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/08/27
*/

#pragma once


#define HPET_GCAP	0x0000
#define HPET_GCAP_HI	0x0004
#define HPET_GEN_CONF	0x0010

#define HPET_ENABLE_CNF (1<<0)
#define HPET_LEG_RT_CNF	(1<<1)

#define HPET_GINTR_STA	0x0020
#define HPET_MAIN_CNT	0x00f0
#define HPET_MAIN_CNT_HI	0x00f4

#define HPET_TIMN_CONF(N) (0x00100+0x0020*(N) + 0x0000)
#define HPET_TIMN_CONF_HI(N) (0x00100+0x0020*(N) + 0x0004)

#define HPET_TIMN_COMPARATOR(N) (0x00100+0x0020*(N) + 0x0008)
#define HPET_TIMN_FSBIR(N) (0x00100+0x0020*(N) + 0x0010)

#define HPET_TIM0_CONF HPET_TIMN_CONF(0)
#define HPET_TIM0_CONF_HI HPET_TIMN_CONF_HI(0)
#define HPET_TIM0_COMPARATOR HPET_TIMN_COMPARATOR(0)
#define HPET_TIM0_FSBIR HPET_TIMN_FSBIR(0)
#define HPET_TIM1_CONF HPET_TIMN_CONF(1)
#define HPET_TIM1_CONF_HI HPET_TIMN_CONF_HI(1)
#define HPET_TIM1_COMPARATOR HPET_TIMN_COMPARATOR(1)
#define HPET_TIM1_FSBIR HPET_TIMN_FSBIR(1)
#define HPET_TIM2_CONF HPET_TIMN_CONF(2)
#define HPET_TIM2_CONF_HI HPET_TIMN_CONF_HI(2)
#define HPET_TIM2_COMPARATOR HPET_TIMN_COMPARATOR(2)
#define HPET_TIM2_FSBIR HPET_TIMN_FSBIR(2)

#define HPET_CONF_INT_ROUTE_CAP64(n) (((uint64_t)n)<<32ULL)
#define HPET_CONF_INT_ROUTE_CAP32(n) (n)
#define HPET_CONF_FSB_INT_DEL_CAP (1<<15)
#define HPET_CONF_FSB_EN_CNF (1<<14)
#define HPET_CONF_INT_ROUTE_CNF(n) ((n)<<(9))
#define HPET_CONF_INT_MASK (0x1f<<9)
#define HPET_CONF_32MODE_CNF (1<<8)
/* 7 reserved */
#define HPET_CONF_VAL_SET_CNF (1<<6)
#define HPET_CONF_SIZE_CAP (1<<5)
#define HPET_CONF_PER_INT_CAP (1<<4)
#define HPET_CONF_TYPE_CNF (1<<3)
#define HPET_CONF_INT_ENB_CNF (1<<2)
#define HPET_CONF_INT_TYPE_CNF (1<<1)


#define HPET_FLAG_4K (1<<0)
#define HPET_FLAG_64K (1<<1)

#define HPET_WRITE(r,val) (mmio_write64(r + hpet_base_addr, val))
#define HPET_READ(r) (mmio_read64(r + hpet_base_addr))


extern unsigned long hpet_base_addr;
extern unsigned int hpet_freq_khz;


//HPETの比較器を制御する
struct hpet_comparator {

	unsigned int *ready_ptr;
	unsigned int ready_bits;

	_Bool interrupt_fired;
};

static __inline__ unsigned int hpet_nsec_to_tick(unsigned long nsec) {

	return (nsec * hpet_freq_khz) / 1000000;
}

static __inline__ unsigned int hpet_usec_to_tick(unsigned int usec) {

	return (usec * hpet_freq_khz) / 1000;
}

static __inline__ unsigned int hpet_msec_to_tick(unsigned int msec) {

	return (msec * hpet_freq_khz);
}



//HPET 初期化
void hpet_init(void);



void hpet_oneshot(unsigned int ticks);
void hpet_periodic(unsigned int ticks);






