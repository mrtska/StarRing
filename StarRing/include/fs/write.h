/*
 * write.h
 *
 *  Created on: 2015/01/22
 *      Author: StarRing
 */

#pragma once

#include <system.h>
#include <fs/iovec.h>



long writev(unsigned long fd, const struct iovec *vec, unsigned long vlen);
