/*
 * env.h
 *
 *  Created on: 2015/01/30
 *      Author: StarRing
 */

#pragma once


#include <list.h>

extern struct list_head env;


struct env_entry {

	struct list_head list;
	char *name;
};



void environment_valiable_init(void);
