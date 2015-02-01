/*
[Module stdio.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/04/03
*/


#pragma once

#include <stdarg.h>
#include <fs/fs.h>
#include <string.h>




//文字が数字だったらtrue
static __inline__ int isdigit(int ch) {

	return (ch >= '0') && (ch <= '9');
}
static __inline__ int isxdigit(int ch) {

	if(isdigit(ch)) {
		return 1;
	}
	if((ch >= 'a') && (ch <= 'f')) {
		return 1;
	}
	return (ch >= 'A') && (ch <= 'F');
}



