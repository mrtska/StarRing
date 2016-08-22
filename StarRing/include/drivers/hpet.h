
#pragma once


#include <system.h>

#define HPET_GCAP	0x0000
#define HPET_GEN_CONF	0x0010


#define HPET_TIMN_CONF(N)		(0x00100 + 0x0020 * (N) + 0x0000)
#define HPET_TIMN_CONF_HI(N)	(0x00100 + 0x0020 * (N) + 0x0004)

#define HPET_TIMN_COMPARATOR(N) (0x00100+0x0020*(N) + 0x0008)

#define HPET_CONF_32MODE_CNF (1<<8)

#define HPET_CONF_VAL_SET_CNF (1<<6)
#define HPET_CONF_SIZE_CAP (1<<5)
#define HPET_CONF_PER_INT_CAP (1<<4)
#define HPET_CONF_TYPE_CNF (1<<3)
#define HPET_CONF_INT_ENB_CNF (1<<2)
#define HPET_CONF_INT_TYPE_CNF (1<<1)

extern class hpet hpet;












class hpet final {

private:

	unsigned long hpet_base_address = 0xFED00000;

	unsigned int hpet_freq_khz;

	unsigned int hpet_nsec_to_tick(unsigned long nsec) {

		return (nsec * hpet_freq_khz) / 1000000;
	}

	unsigned int hpet_usec_to_tick(unsigned int usec) {

		return (usec * hpet_freq_khz) / 1000;
	}

	unsigned int hpet_msec_to_tick(unsigned int msec) {

		return (msec * hpet_freq_khz);
	}

	void write_hpet(unsigned long offset, unsigned long value) {

		unsigned long addr = hpet_base_address + offset;

		*reinterpret_cast<unsigned long*>(translate_physical_address(addr)) = value;
	}
	unsigned long read_hpet(unsigned long offset) {

		unsigned long addr = hpet_base_address + offset;

		return *reinterpret_cast<unsigned long*>(translate_physical_address(addr));
	}

public:



	void hpet_init();






};
















