/*
 * seek.h
 *
 *  Created on: 2015/01/28
 *      Author: StarRing
 */

#pragma once





#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2



unsigned long lseek(unsigned int fd, unsigned long offset, unsigned int whence);
