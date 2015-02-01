/*
 * ioctl.c
 *
 *  Created on: 2015/01/22
 *      Author: StarRing
 */

#include <fs/ioctl.h>






int ioctl(unsigned int fd, unsigned int cmd, unsigned long arg) {


	switch(cmd) {

	case TCGETS:

		*(unsigned long*) arg = 0xFFFFFFFFDEAD;
		break;
	}





	return 0;
}













