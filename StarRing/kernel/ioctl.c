/*
[Module ioctl.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2015/01/22
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













