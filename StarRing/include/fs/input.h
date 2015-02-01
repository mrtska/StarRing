/*
 * input.h
 *
 *  Created on: 2015/01/27
 *      Author: StarRing
 */

#pragma once



#include <fs/vfs.h>





struct input_device_data {

	int key_code[0x100];
	int cursor;
	int count;
};










void wait_type_key(unsigned int key_code);
struct fs_node *create_input_device(char num);












