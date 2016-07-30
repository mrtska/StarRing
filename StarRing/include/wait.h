/*
[Module wait.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/08/27
*/

#pragma once



struct wait_list {

	unsigned char list1_cpu;
	unsigned long list1;
	unsigned char list2_cpu;
	unsigned long list2;
	unsigned char list3_cpu;
	unsigned long list3;
	unsigned char list4_cpu;
	unsigned long list4;
};



void wait_list_init(void);


//指定した時間待つ関数 精度はお察し
void wait_sec(unsigned int sec);
void wait_msec(unsigned long msec);
void wait_usec(unsigned long usec);
void wait_nsec(unsigned long nsec);














