/*
[Module read.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2015/01/24
*/

#pragma once


#include <system.h>
#include <fs/iovec.h>
#include <string.h>

long read(unsigned long fd, const char *buf, size_t count);
int readv(unsigned long fd, const struct iovec *vec, unsigned long vlen);





















