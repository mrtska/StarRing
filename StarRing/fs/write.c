/*
 * write.c
 *
 *  Created on: 2015/01/22
 *      Author: StarRing
 */


#include <fs/write.h>






long writev(unsigned long fd, const struct iovec *vec, unsigned long vlen) {


	//stdout 標準出力
	if(fd == 1) {


		long ret = 0;

		char buf[1024];
		int i;
		for(i = 0; i < vlen; i++) {

			const struct iovec *v = &vec[i];

			memset(buf, 0x00, 1024);
			strncpy(buf, v->iov_base, v->iov_len);
			kprintf("%s", buf);
			ret += v->iov_len;
		}

		return ret;
	}

	return 0;
}































