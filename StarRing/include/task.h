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
#include <fs/vfs.h>
#include <fs/mmap.h>
#include <regs.h>
#include <tss.h>

#define TASK_RUNNING		0x01
#define TASK_SLEEPING		0x02
#define TASK_WAITING		0x04
#define TASK_START_WAITING	0x08


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
	void *elf_base;					//ELFファイル本体の位置
	void *interp_base;				//インタープリターの本体の位置
	unsigned long interp_addr;
	unsigned long entry_point;		//エントリポイント
	unsigned long entry_interp;
	int cpu_id;						//このプロセスを実行中しているCPUのAPIC ID

	//ヒープ領域
	unsigned long start_brk;
	unsigned long end_brk;

	//ファイルディスクリプタマネージャー
	struct file_descriptor *fd;
	struct list_head fd_list;

	unsigned long mmap_base;
	struct list_head mmap_list;

	int argc;
	char *cmdline;



	struct regs registers;


	unsigned int flags;				//フラグ
};



void schedule(struct regs *registers);
void task_init(void);
void tss_init(void);
int create_process(char *name, struct fs_node *node, int argc, char *cmdline, int flags);

struct process *lookup_process_by_pid(int pid);


void native_enter_user_64(unsigned long stack, unsigned long entry);











