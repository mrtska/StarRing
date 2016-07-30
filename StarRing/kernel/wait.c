/*
[Module wait.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/08/27
*/

#include <wait.h>
#include <drivers/hpet.h>
#include <system.h>
#include <apic.h>
#include <smp.h>



struct wait_list wait_list;


void wait_list_init(void) {

	wait_list.list1_cpu = 0xFF;
	wait_list.list2_cpu = 0xFF;
	wait_list.list3_cpu = 0xFF;
	wait_list.list4_cpu = 0xFF;
	wait_list.list1 = 0x0000000000000000;
	wait_list.list2 = 0x0000000000000000;
	wait_list.list3 = 0x0000000000000000;
	wait_list.list4 = 0x0000000000000000;
}


//ウェイトイベント
unsigned int wait_event(unsigned long *remained_time) {


	int apic_id = apic_read(APIC_ID_R) >> 24;
	smp_table[apic_id].flags |= SMP_PROCESSOR_WAITING;	//プロセッサ情報を更新する

	int flag = *remained_time;
	//イベントが起きるまでループ
	while(1) {

		flag = *remained_time;	//割り込みが来たかどうかのフラグ

		mfence();		//メモリバリア
		if(!flag) {

			smp_table[apic_id].flags &= ~SMP_PROCESSOR_WAITING;	//プロセッサ情報を更新する
			return 1;
		}
		asmv("hlt");	//CPUぶん回しはもったいないので割り込みが来るまで休憩
	}

	//ここに来ることは無いはず
	return 0;
}


void wait_sec(unsigned int sec) {

	wait_msec(sec * 1000);
}

//指定したミリ秒待つ
void wait_msec(unsigned long msec) {

	wait_usec(msec * 1000);
}

//指定したマイクロ秒待つ
void wait_usec(unsigned long usec) {

	wait_nsec(usec * 1000);
}

//指定したナノ秒待つ
void wait_nsec(unsigned long nsec) {

	wait_list.list1_cpu = apic_read(APIC_ID_R) >> 24;
	wait_list.list1 = nsec;

	wait_event(&wait_list.list1);
}








