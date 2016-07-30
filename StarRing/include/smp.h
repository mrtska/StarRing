/*
[Module smp.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/08/27
*/

#pragma once

#include <string.h>
#include <task.h>
#include <fs/vfs.h>

#define SMP_PROCESSOR_BSP		0x01
#define SMP_PROCESSOR_EXISTED	0x02
#define SMP_PROCESSOR_ENABLED	0x04
#define SMP_PROCESSOR_RELAXED	0x08
#define SMP_PROCESSOR_WAITING	0x10
#define SMP_PROCESSOR_IS_USER	0x20
#define SMP_PROCESSOR_INTERRUPT_RECEIVABLE	0x40

struct smp_table_entry {

	unsigned char flags;
	struct process *execute_process;
	struct fs_node *input_node;

};





extern struct smp_table_entry smp_table[16];

extern unsigned char ap_stack[16][0x4000];

static __inline__ void smp_table_set_input_node(unsigned int apic_id, struct fs_node *node) {

	smp_table[apic_id].input_node = node;
}

static __inline__ struct fs_node *smp_table_get_input_node(unsigned int apic_id) {

	return smp_table[apic_id].input_node;
}


static __inline__ void smp_table_set_flags(unsigned int apic_id, unsigned int flags) {

	smp_table[apic_id].flags |= flags;
}

static __inline__ void smp_table_clear_flags(unsigned int apic_id, unsigned int flags) {

	smp_table[apic_id].flags &= ~flags;
}




//AP起動
void smp_init(void);

void smp_ap_init(void);































