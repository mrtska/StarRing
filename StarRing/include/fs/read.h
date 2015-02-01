/*
 * read.h
 *
 *  Created on: 2015/01/24
 *      Author: StarRing
 */

#pragma once


#include <system.h>
#include <fs/iovec.h>

int readv(unsigned long fd, const struct iovec *vec, unsigned long vlen);





















