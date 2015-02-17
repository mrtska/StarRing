/*
[Module input.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2015/01/27
*/

#pragma once



#include <fs/vfs.h>





struct input_device_data {

	unsigned int key_code[0x100];
	unsigned int cursor;
	unsigned int count;
};










void wait_type_key(unsigned int key_code);
struct fs_node *create_input_device(struct file_system *fs, char num);












