/*
[Module env.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2015/01/30
*/

#pragma once


#include <list.h>

extern struct list_head env;


struct env_entry {

	struct list_head list;
	char *name;
};



void environment_valiable_init(void);
