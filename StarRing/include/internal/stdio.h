/*
 * stdio.h
 *
 *  Created on: 2014/04/03
 *      Author: StarRing
 */

#include <stdarg.h>

#pragma once

//おなじみの関数 カーネル版
int kkprintf(const char *format, ...);
int kprintf(const char *format, ...);
int sprintf(char *data, const char *format, ...);

int vprintf(const char *format, va_list args);

//カーソルをもとに位置に戻す
void resetPos(void);

