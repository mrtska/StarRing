/*
[Module input.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2015/01/27
*/


#include <fs/input.h>
#include <apic.h>
#include <smp.h>
#include <spinlock.h>
#include <ioapic.h>
static unsigned int read_input(struct fs_node *node, unsigned int offset, unsigned int size, unsigned char *buffer) {

	struct input_device_data *data = node->data;
	*buffer = data->key_code[data->cursor];
	data->key_code[data->cursor] = 0;
	if(data->cursor <= 0) {

		return 1;
	}
	data->cursor--;
	//kprintf("input cursor %d, data %X\n", data->cursor, data->key_code[data->cursor]);
	return 1;
}

static unsigned int write_input(struct fs_node *node, unsigned int offset, unsigned int size, unsigned int *buffer) {

	struct input_device_data *data = node->data;
	data->key_code[data->cursor] = *buffer;
	data->cursor++;
	return 1;
}

static void open_input(struct fs_node *node, unsigned int flags) {

	trace();
	return;
}

static void close_input(struct fs_node *node) {

	trace();
	return;
}


void wait_type_key(unsigned int key_code) {

	int apic_id = get_apic_id();
	struct input_device_data buf;
	memset(&buf, 0x00, sizeof(struct input_device_data));

	io_apic_set_cpu(1, apic_id);
	while(buf.key_code[buf.cursor] != key_code) {


		read_fs(smp_table_get_input_node(apic_id), 0, 0, (void*)&buf);
		if(buf.cursor <= 0) {

			asmv("sti;hlt");
			continue;
		}

		kkprintf("wait type %X, type %X, cursor %d\n", key_code, buf.key_code[buf.cursor], buf.cursor);

		asmv("sti;hlt");

	}
}



//inputはキーボードのこと
struct fs_node *create_input_device(char num) {

	struct fs_node *node = kmalloc(sizeof(struct fs_node), 8);

	strcpy(node->filename, "/dev/input");
	node->filename[11] = num;
	node->read = read_input;
	node->write = write_input;
	node->open = open_input;
	node->close = close_input;

	node->data = kmalloc(sizeof(struct input_device_data), 8);

	struct input_device_data *data = node->data;
	data->cursor = 0;
	//kprintf("create_input_device %d\n", num);

	return node;
}






























