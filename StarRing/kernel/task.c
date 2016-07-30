/*
[Module task.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/10/25
*/


#include <system.h>
#include <task.h>
#include <page/page.h>
#include <spinlock.h>
#include <elf.h>
#include <apic.h>
#include <gdt.h>
#include <smp.h>
#include <wait.h>
#include <env.h>

#include <mem/alloc.h>

//プロセスを管理するリスト
LIST_HEAD(processes);

//スピンロッカー
unsigned char task_lock;

unsigned char tss_init_lock;


void tss_init(void) {


	spin_lock(&tss_init_lock);

	int apic_id = apic_read(APIC_ID_R) >> 24;

	struct tss *kernel_tss = kmalloc(sizeof(struct tss), 0);

	const unsigned long tss_base = (unsigned long) kernel_tss;
	const unsigned long tss_limit = sizeof(struct tss) * 0x10;
	struct tss_descriptor *tss = (struct tss_descriptor*) &get_gdt(apic_id)->gdt[GDT_ENTRY_KERNEL_TSS];

	tss->limit1 = tss_limit & 0xFF;
	tss->limit2 = (tss_limit & 0xF0000) >> 16;

	tss->base1 = tss_base & 0xFFFF;
	tss->base2 = (tss_base & 0xFF0000) >> 16;
	tss->base3 = (tss_base & 0xFF000000) >> 24;
	tss->base4 = tss_base >> 32;

	tss->flags = TSS_DESC_P | TSS_DESC_DPL0 | TSS_DESC_TYPE_AVAIL;


	//kprintf("tss %p, addr %p id %d, stack %p\n", &tss[apic_id], tss_base, apic_id, (unsigned long) &ap_stack[apic_id][0x1FF0]);


	kernel_tss->io_map_base = 104;

	kernel_tss->rsp[0] = (unsigned long) &ap_stack[apic_id][0x1FF0];
	kernel_tss->rsp[1] = (unsigned long) &ap_stack[apic_id][0x1FF0];
	kernel_tss->rsp[2] = (unsigned long) &ap_stack[apic_id][0x1FF0];

/*	kernel_tss->ist[0] = 0xFFFFFFFF80004000;
	kernel_tss->ist[1] = 0xFFFFFFFF80004000;
	kernel_tss->ist[2] = 0xFFFFFFFF80004000;
	kernel_tss->ist[3] = 0xFFFFFFFF80004000;
	kernel_tss->ist[4] = 0xFFFFFFFF80004000;
	kernel_tss->ist[5] = 0xFFFFFFFF80004000;
	kernel_tss->ist[6] = 0xFFFFFFFF80004000;*/

	write_tr(GDT_ENTRY_KERNEL_TSS * 0x8);

	spin_unlock(&tss_init_lock);
}



//プロセスIDからプロセスを取得する
struct process *lookup_process_by_pid(int pid) {

	struct list_head *ptr;
	struct process *process;

	list_for_each(ptr, &processes)
	{

		process = list_entry(ptr, struct process, list);
		if(process->pid == pid) {

			return process;
		}
	}
	return NULL;
}

//プロセスIDを生成する
static int get_next_pid(void) {

	int i;

	struct list_head *ptr;
	struct process *process;
	bool flag = true;

	for(i = 1; i < 0xFF; i++) {

		list_for_each(ptr, &processes)
		{

			process = list_entry(ptr, struct process, list);
			if(i == process->pid) {

				flag = false;
				break;
			} else {

				flag = true;
			}
		}
		if(flag) {

			return i;
		}
	}
	return -1;
}

//タスクを初期化する
void task_init(void) {

	struct fs_node *f = kopen("/weston", 0);

	create_process("/weston", f, 1, "", 0);
}

int create_process(char *name, struct fs_node *node, int argc, char *cmdline, int flags) {

	//プロセスの初期化
	struct process *process = kmalloc(sizeof(struct process), 0);
	process->name = name;
	process->stack = (unsigned long*) PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(alloc_memory_block());
	process->pid = get_next_pid();
	process->file = node;
	process->userid = 0;

	//ページテーブルの初期化
	process->page_tables = (unsigned long*) PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(alloc_memory_block());
	process->base = (unsigned long*) PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(alloc_memory_block());

	//ヒープマネージャの初期化

	//ファイルの初期化
	process->file = node;
	process->elf_base = (void*) PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(alloc_memory_block());
	process->interp_base = (void*) PHYSICAL_ADDRESS_TO_VIRTUAL_ADDRESS(alloc_memory_block());
	process->interp_addr = 0x7FFFF0000000;

	process->mmap_base = 0x7F0000000000;
	INIT_LIST_HEAD(&process->mmap_list);

	process->argc = argc;
	process->cmdline = cmdline;

	//ファイルディスクリプタリストを初期化
	INIT_LIST_HEAD(&process->fd_list);

	struct file_descriptor *stdout = kmalloc(sizeof(struct file_descriptor), 0);
	struct file_descriptor *stdin = kmalloc(sizeof(struct file_descriptor), 0);
	struct file_descriptor *stderr = kmalloc(sizeof(struct file_descriptor), 0);

	stdin->fd = 0;

	stdout->fd = 1;
	stdout->node = kopen("/dev/stdout", 0);

	stderr->fd = 2;
	stderr->node = kopen("/dev/stderr", 0);

	list_add_tail(&stdin->list, &process->fd_list);
	list_add_tail(&stdout->list, &process->fd_list);
	list_add_tail(&stderr->list, &process->fd_list);



	//ユーザーページテーブルを構築する
	setup_user_page(process);

	list_add_tail(&process->list, &processes);

	process->flags = TASK_START_WAITING;
	return 0;
}

//64bitユーザー領域に移行する
void enter_user_64(struct process *process, unsigned int apic_id) {

	asmv("cli");

	//プロセッサ情報をユーザー領域にする
	smp_table[apic_id].flags |= SMP_PROCESSOR_IS_USER;
	//smp_table[apic_id].flags &= ~SMP_PROCESSOR_INTERRUPT_RECEIVABLE;

	//エントリポイント
	unsigned long entry = process->entry_interp;


	kprintf("enter_user_64 id = %d, entry = %p\n", apic_id, entry);
	native_enter_user_64(0x7FFFFFFFD000, entry);
}

//32bitユーザー領域に移行する
void enter_user_32(struct process *process) {

	//エントリポイント
	unsigned long entry = process->entry_point;

	//プロセッサ情報をユーザー領域にする
	smp_table[apic_read(APIC_ID_R) >> 24].flags |= SMP_PROCESSOR_IS_USER;
	write_cr3((unsigned long) process->page_tables);


	asmv("push $0x023");
	asmv("pushq %0" : : "i"(0xB001FFF0));
	asmv("push $0x200");
	asmv("pushq $0x2B");
	asmv("pushq %0" : : "r"(entry));
	asmv("iret");
}

void save_registers(struct regs *registers, struct process *process) {

	process->registers.c.rax = registers->c.rax;
	process->registers.c.rbx = registers->c.rbx;
	process->registers.c.rcx = registers->c.rcx;
	process->registers.c.rdx = registers->c.rdx;
	process->registers.c.rsi = registers->c.rsi;
	process->registers.c.rdi = registers->c.rdi;
	process->registers.c.r8 = registers->c.r8;
	process->registers.c.r9 = registers->c.r9;
	process->registers.c.r10 = registers->c.r10;
	process->registers.c.r11 = registers->c.r11;
	process->registers.c.r12 = registers->c.r12;
	process->registers.c.r13 = registers->c.r13;
	process->registers.c.r14 = registers->c.r14;
	process->registers.c.r15 = registers->c.r15;
	process->registers.c.rbp = registers->c.rbp;

	process->registers.i.rsp = registers->i.rsp;
	process->registers.i.rip = registers->i.rip;
	process->registers.i.cs = registers->i.cs;
	process->registers.i.ss = registers->i.ss;
	process->registers.i.rflags = registers->i.rflags;
}


//プロセスを実行する
void run_process(struct process *process) {

	//APIC ID
	int apic_id = get_apic_id();

	load_elf_binary(process);
	map_page(0x7FFFFFE00000, (unsigned long) alloc_memory_block(), process->page_tables, FLAGS_WRITABLE_PAGE | FLAGS_USER_PAGE | FLAGS_LARGE_PAGE);	//スタック領域を0x7FFFFFFFF000に割り当てる
	elf64_create_table(process, (unsigned char*)0x7FFFFFFFD000);

	//map_page(0xF0000000, VIRTUAL_ADDRESS_TO_PHYSICAL_ADDRESS(process->heap_base), process->page_tables, FLAGS_WRITABLE_PAGE | FLAGS_USER_PAGE | FLAGS_LARGE_PAGE);	//
	smp_table[apic_id].execute_process = process;

	kprintf("run process id = %d\n", apic_id);
	//ユーザー領域へ
	enter_user_64(process, apic_id);
}

void schedule(struct regs *registers) {

	struct list_head *ptr;
	struct process *process;

	spin_lock(&task_lock);

	int apic_id = apic_read(APIC_ID_R) >> 24;

	//BSPはユーザータスクの処理をしない
	if(apic_id == 0) {

		spin_unlock(&task_lock);
		return;
	}

	struct process *current_process = get_process();

	//kprintf("cur proc %p\n", current_process);

	//現在のプロセスをウェイト状態にする
	if(current_process != NULL && registers != NULL) {
		trace();
		current_process->flags |= TASK_WAITING;
		save_registers(registers, current_process);
	}

	//プロセスリストを走査する
	list_for_each(ptr, &processes)
	{

		process = list_entry(ptr, struct process, list);

		//スタートウェイト状態のプロセスを実行する
		if(process->flags & TASK_START_WAITING) {

			kprintf("schedule#TASK_START_WAITING\n");
			process->flags &= ~TASK_START_WAITING;
			process->flags |= TASK_RUNNING;
			spin_unlock(&task_lock);
			run_process(process);
			return;
		}

		//タスクスイッチするタスク
		if(process->flags & TASK_WAITING) {

			kprintf("schedule#TASK_WAITING rsp = %p, id = %d\n", process->registers.i.rsp, apic_id);
			process->flags &= ~TASK_WAITING;
			process->flags |= TASK_RUNNING;
			spin_unlock(&task_lock);
			asmv("hlt");
			return;
		}

	}
	spin_unlock(&task_lock);

	//ウェイト状態のプロセスが無かったら休憩 ここからCPUにタイマー割り込みが入ってまたscheduleが呼ばれる

}

