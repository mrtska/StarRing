/*
[Module iovec.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2015/01/29
*/

#pragma once

#include <string.h>


struct iovec {
        void   *iov_base;    // ベースアドレス
        size_t  iov_len;     // 長さ
};

