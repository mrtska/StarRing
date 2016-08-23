
#pragma once


#include <system.h>

#define HPET_OFFSET_ID					0x00
#define HPET_OFFSET_PERIOD				0x04
#define HPET_OFFSET_GCONFIG				0x10
#define HPET_OFFSET_STATUS				0x20
#define HPET_OFFSET_MAIN_COUNTER		0xF0


#define HPET_OFFSET_TIMN_CONFIG(N)		(0x100 + 0x20 * (N))

#define HPET_OFFSET_TIMN_COMPARATOR(N)	(0x108 + 0x20 * (N))

#define HPET_CONFIG_ENABLE				0x001
#define HPET_CONFIG_LEGACY				0x002


#define HPET_TN_LEVEL					0x0002
#define HPET_TN_ENABLE					0x0004
#define HPET_TN_PERIODIC				0x0008
#define HPET_TN_PERIODIC_CAP			0x0010
#define HPET_TN_64BIT_CAP				0x0020
#define HPET_TN_SETVAL					0x0040
#define HPET_TN_32BIT					0x0100
#define HPET_TN_ROUTE					0x3e00
#define HPET_TN_FSB						0x4000
#define HPET_TN_FSB_CAP					0x8000

extern class hpet hpet;












class hpet final {

private:

	unsigned long hpet_base_address;

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

	void write_hpet64(unsigned long offset, unsigned long value) {

		unsigned long addr = hpet_base_address + offset;

		*reinterpret_cast<unsigned long*>(translate_physical_address(addr)) = value;
	}
	unsigned long read_hpet64(unsigned long offset) {

		unsigned long addr = hpet_base_address + offset;

		return *reinterpret_cast<unsigned long*>(translate_physical_address(addr));
	}

	void write_hpet32(unsigned long offset, unsigned int value) {

		unsigned long addr = hpet_base_address + offset;

		*reinterpret_cast<unsigned int*>(translate_physical_address(addr)) = value;
	}
	unsigned int read_hpet32(unsigned long offset) {

		unsigned long addr = hpet_base_address + offset;

		return *reinterpret_cast<unsigned int*>(translate_physical_address(addr));
	}

	void reset_counter() {

		write_hpet64(HPET_OFFSET_MAIN_COUNTER, 0);
	}
	void start_counter() {

		unsigned int cfg = read_hpet32(HPET_OFFSET_GCONFIG);
		cfg |= HPET_CONFIG_ENABLE;
		write_hpet32(HPET_OFFSET_GCONFIG, cfg);
	}

	void stop_counter() {

		unsigned int cfg = read_hpet32(HPET_OFFSET_GCONFIG);
		cfg &= ~HPET_CONFIG_ENABLE;
		write_hpet32(HPET_OFFSET_GCONFIG, cfg);
	}



public:



	void hpet_init();


	void restart_counter() {

		stop_counter();
		reset_counter();
		start_counter();
	}




};
















