/*
[Module hpet.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/08/27
*/

#include <drivers/hpet.h>
#include <acpi.h>
#include <string.h>
#include <mm.h>
#include <ioapic.h>
#include <system.h>
#include <desc.h>
#include <apic.h>
#include <wait.h>
#include <smp.h>
#include <task.h>


//HPETは三つの比較器を持っている
struct hpet_comparator comparators[3];


//HPET メモリマップドI/O アドレス おそらく0xFED00000
unsigned long hpet_base_addr;

//HPET フラグ
int hpet_flags = 0;

//HPET 周波数
unsigned int hpet_freq_khz;

//HPET割り込みハンドラ
extern void hpet_handler(void);
extern void hpet_periodic_handler(void);

//HPET 周波数初期化
static void hpet_init_hz(void) {

	unsigned int hpet_period = mmio_read32(hpet_base_addr + HPET_GCAP_HI);
	hpet_freq_khz = 1000000000 / (hpet_period / 1000);
}

//HPETを起動
void hpet_start(void) {

	unsigned int cnf = HPET_READ(HPET_GEN_CONF);
	cnf |= HPET_ENABLE_CNF;
	HPET_WRITE(HPET_GEN_CONF, cnf);
}

//HPETを停止
void hpet_stop(void) {

	unsigned int cnf = HPET_READ(HPET_GEN_CONF);
	cnf &= ~HPET_ENABLE_CNF;
	HPET_WRITE(HPET_GEN_CONF, cnf);
}


//HPET初期化
void hpet_init(void) {

	//割り込みハンドラ登録
	asmv("cli");
	set_intr_gate(0x22, hpet_periodic_handler);
	io_apic_set_redirect(2, 0, 0x22);
	set_intr_gate(0x28, hpet_handler);
	io_apic_set_redirect(8, 0, 0x28);
	mfence();


	unsigned int cnf;
	unsigned long addr = find_acpi_description_entry(ACPI_SIG('H', 'P', 'E', 'T'));	//ACPIテーブルからHPETのベースアドレスを取得する
	int num;
	int pa;


	if((void*)addr == NULL) {

		kprintf("ERROR! could not find HPET!\n");
		return;
	}

	num = ACPI_R8(addr, 52);
	if(num != 0) {

		kprintf("ERROR! too many HPET!\n");
		return;
	}

	hpet_base_addr = ACPI_R32(addr, 44);
	hpet_flags = 0;
	pa = ACPI_R8(addr, 8);
	if(pa == 0) {


	} else if(pa == 1) {

		hpet_flags |= HPET_FLAG_4K;
	} else if(pa == 2) {

		hpet_flags |= HPET_FLAG_64K;
	}
	hpet_init_hz();

	cnf = HPET_READ(HPET_GEN_CONF);	//コンフィグレジスタ取得
	cnf |= HPET_LEG_RT_CNF;			//レガシーリプレースメントモード
	HPET_WRITE(HPET_GEN_CONF, cnf);	//コンフィグレジスタ書き込み
}

//HPET周期モード起動 タスクスイッチ用
void hpet_periodic(unsigned int ticks) {

	hpet_stop();

	unsigned int cnf = HPET_CONF_32MODE_CNF | HPET_CONF_INT_ENB_CNF | HPET_CONF_TYPE_CNF;
	HPET_WRITE(HPET_TIMN_CONF(0), cnf);

	HPET_WRITE(HPET_MAIN_CNT, 0);
	HPET_WRITE(HPET_TIMN_COMPARATOR(0), ticks);



	hpet_start();
}

//HPETワンショットモード起動
void hpet_oneshot(unsigned int ticks) {

	hpet_stop();

	unsigned int cnf =HPET_CONF_32MODE_CNF | HPET_CONF_INT_ENB_CNF;
	HPET_WRITE(HPET_TIMN_CONF(1), cnf);

	HPET_WRITE(HPET_MAIN_CNT, 0);
	HPET_WRITE(HPET_TIMN_COMPARATOR(1), ticks);


	hpet_start();
}


extern struct wait_list wait_list;


//タスクスイッチ時に呼ばれる
void do_hpet_periodic_handler(struct regs *registers) {


	int apic_id = apic_read(APIC_ID_R) >> 24;

	//待ちリストを更新する
	if(wait_list.list1 != 0) {

		if(wait_list.list1 < 1000000) {

			if(wait_list.list1_cpu != apic_id) {

				io_apic_set_redirect(23, wait_list.list1_cpu << 24, 0x44);
				//apic_write(APIC_ICR_LOW, 0x1000);
			}

			wait_list.list1 = 0;
		} else {

			wait_list.list1 -= 1000000;
		}
	}

	//どのCPUがタスクスイッチしているかを保持する
	static int current_cpu;


	//---次にタスクスイッチするCPUを選択---
	int i;
	for(i = current_cpu + 1; i < 16; i++) {


		//有効になっているコアを探す ブート直後はBSPのみ
		if(smp_table[i].flags & SMP_PROCESSOR_INTERRUPT_RECEIVABLE) {

			current_cpu = i;
			break;
		} else {

			current_cpu = -1;
		}
	}
	if(current_cpu == -1) {

		current_cpu = 0;
	}
	//---

	//IOAPICリダイレクションテーブルを更新して割り込みを受け取るCPUを切り替える
	io_apic_set_redirect(8, current_cpu << 24, 0x28);
	//kprintf("hpet apic id = %d, next = %d, rip = %p\n", apic_read(APIC_ID_R) >> 24, current_cpu, registers->i.rip);
	apic_eoi();
}

//HPET割り込みハンドラ本体
void do_hpet_handler(struct regs *registers) {

/*
	if(smp_table[get_apic_id()].flags & SMP_PROCESSOR_IS_USER) {


		kkprintf("id %d, hpet CC %X, SS %X\n", get_apic_id(), registers->i.cs, registers->i.ss);
	}*/

	hpet_oneshot(hpet_msec_to_tick(1));
	do_hpet_periodic_handler(NULL);

}

