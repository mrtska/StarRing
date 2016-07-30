/*
[Module seek.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2015/01/28
*/

#pragma once





#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2



unsigned long lseek(unsigned int fd, unsigned long offset, unsigned int whence);
