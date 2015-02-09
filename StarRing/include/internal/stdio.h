/*
[Module stdio.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/04/03
*/

#include <stdarg.h>

#pragma once

//おなじみの関数 カーネル版
int kkprintf(const char *format, ...);
int kprintf(const char *format, ...);
int sprintf(char *data, const char *format, ...);

int vprintf(const char *format, va_list args);
int vvprintf(const char *format, void **args);
int vsprintf(char *buf, const char *format, va_list args);
int vvsprintf(char *buf, const char *format, void **args);
//カーソルをもとに位置に戻す
void resetPos(void);

