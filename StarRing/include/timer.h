/*
[Module timer.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/04/06
*/

#pragma once

#include <system.h>

#define NS_PER_MSEC		1000000UL
#define NS_PER_SEC		1000000000UL

#define PM_TIMER_HZ		3579545
#define PM_TIMER_OVERFLOW	((0x1000000 * 1000000000ULL) / PM_TIMER_HZ)


//タイマー初期化
void timer_init(unsigned int quantom);



















