/*
[Module task.h]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/10/25
*/

#pragma once


#include <list.h>
#include <stdio.h>
#include <fs/fs.h>
#include <regs.h>

#define TASK_RUNNING		0x01
#define TASK_SLEEPING		0x02
#define TASK_WAITING		0x04
#define TASK_START_WAITING	0x08

#define TSS_DESC_DPL0 (0x00 << 1)
#define TSS_DESC_DPL1 (0x01 << 1)
#define TSS_DESC_DPL2 (0x02 << 1)
#define TSS_DESC_DPL3 (0x03 << 1)

#define TSS_DESC_P 0x80
#define TSS_DESC_TYPE_AVAIL 0x9

struct tss_descriptor {

	unsigned short limit1;
	unsigned short base1;
	unsigned char base2;
	unsigned char flags;
	unsigned char limit2;
	unsigned char base3;
	unsigned int base4;
	unsigned int reserved;
} __attribute__((packed));

struct tss {

	unsigned int reserved1;
	unsigned long rsp[3];
	unsigned long reserved2;
	unsigned long ist[7];
	unsigned short reserved3[5];
	unsigned short io_map_base;
} __attribute__((packed));


//スレッド構造体
struct thread {






	struct regs registers;
};


//プロセス構造体
struct process {

	struct list_head list;			//プロセスリスト
	char *name;						//プロセスの名前
	unsigned int userid;			//ユーザーID
	unsigned int pid;				//プロセスID
	unsigned long *stack;			//スタックポインタ
	unsigned long *page_tables;		//ページテーブルを保管する領域
	unsigned long *base;			//実際にELFが展開される先
	struct fs_node *file;			//ファイルの本体のノード
	void *file_base;				//ELFファイル本体の位置
	unsigned long entry_point;		//エントリポイント
	int cpu_id;						//このプロセスを実行中しているCPUのAPIC ID

	//ヒープ領域
	unsigned long start_brk;
	unsigned long end_brk;

	struct file_descriptor *fd;
	struct list_head fd_list;




	struct regs registers;


	int flags;						//フラグ
};



void schedule(struct regs *registers);
void task_init(void);
void tss_init(void);
int create_process(char *name, struct fs_node *node, int flags);

struct process *lookup_process_by_pid(int pid);


void native_enter_user_64(unsigned long stack, unsigned long entry);











