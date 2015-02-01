/*
 * iovec.h
 *
 *  Created on: 2015/01/29
 *      Author: StarRing
 */

#pragma once

#include <string.h>


struct iovec {
        void   *iov_base;    // ベースアドレス
        size_t  iov_len;     // 長さ
};

