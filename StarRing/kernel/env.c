/*
 * env.c
 *
 *  Created on: 2015/01/30
 *      Author: StarRing
 */

#include <env.h>
#include <system.h>
#include <string.h>
#include <fs/vfs.h>

LIST_HEAD(env);


void environment_valiable_init(void) {

	struct fs_node *node = kopen("/config/env", 0);
	struct env_entry *entry = kmalloc(sizeof(struct env_entry), 0);
	unsigned char *p = kmalloc(node->length, 0);


	read_fs(node, 0, node->length, p);

	char *tok = strtok((char*)p, '\n');

	while(tok != NULL) {

		unsigned char *buf = kmalloc(0x100, 0);
		kprintf("env %s\n", tok);
		strcpy(buf, tok);
		entry->name = buf;

		tok = strtok(0, '\n');
	}

	list_add_tail(&entry->list, &env);
}






