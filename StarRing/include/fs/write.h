/*
[Module write.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2015/01/22
*/

#pragma once

#include <system.h>
#include <fs/iovec.h>


long write(unsigned long fd, const char *buf, size_t count);
long writev(unsigned long fd, const struct iovec *vec, unsigned long vlen);
