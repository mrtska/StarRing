/*
 * systemcall.c
 *
 *  Created on: 2014/11/06
 *      Author: StarRing
 */


#include <system.h>
#include <string.h>
#include <slab.h>
#include <smp.h>
#include <task.h>
#include <apic.h>
#include <list.h>
#include <msr.h>

#include <page/page.h>

#include <fs/fs.h>
#include <fs/vfs.h>
#include <fs/ioctl.h>
#include <fs/read.h>
#include <fs/write.h>
#include <fs/open.h>
#include <fs/seek.h>
#include <fs/mmap.h>
#include <fs/stat.h>

#include <prctl.h>
#include <utsname.h>
#include <uid.h>
#include <errno.h>

#define MSR_STAR	0xC0000081
#define MSR_LSTAR	0xC0000082
#define MSR_CSTAR	0xC0000083


#define SYS_RET	asmv("sysretq")


void sys_exit(int err_code);
void sys_read(unsigned int fd, char *buf, size_t count);
void sys_write(unsigned int fd, const char *buf, size_t count);
unsigned long sys_open(const char *filename, int flags, int mode);
int sys_close(unsigned int fd);



int sys_fstat(unsigned int fd, struct stat *statbuf);

unsigned long sys_lseek(unsigned int fd, unsigned long offset, unsigned int whence);
long sys_mmap(unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, unsigned long fd, unsigned long offset);
int sys_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg);
long sys_readv(unsigned long fd, const struct iovec *vec, unsigned long vlen);
long sys_writev(unsigned long fd, const struct iovec *vec, unsigned long vlen);
long sys_access(const char *filename, int mode);
unsigned long sys_get_vram(void);
unsigned long sys_get_height(void);
unsigned long sys_get_width(void);
unsigned long sys_get_depth(void);
long sys_brk(unsigned long size);
long sys_new_name(struct new_utsname *name);

unsigned int sys_readlink(const char *path, char *buf, int bussiz);

unsigned int sys_getuid(void);
unsigned int sys_getgid(void);
unsigned int sys_geteuid(void);
unsigned int sys_getegid(void);


long sys_arch_prctl(int code, unsigned long addr);


void sys_exit_group(int exit_code);


long sys_set_tid_address(int *tidptr);
unsigned long systemcall_table[] = {

		(unsigned long)sys_read, (unsigned long)sys_write, (unsigned long)sys_open, (unsigned long)sys_close, 0, (unsigned long)sys_fstat,	//0 ~ 5
		0, 0, (unsigned long)sys_lseek, (unsigned long)sys_mmap, 0,	//10
		0, (unsigned long)sys_brk, 0, 0, 0,	//15
		(unsigned long)sys_ioctl, 0, 0, (unsigned long)sys_readv, (unsigned long)sys_writev,	//20
		(unsigned long)sys_access, 0, 0, 0, 0,	//25
		0, 0, 0, 0, 0,	//30
		0, 0, 0, 0, 0,	//35
		0, 0, 0, 0, 0,	//40
		0, 0, 0, 0, 0,	//45
		0, 0, 0, 0, 0,	//50
		0, 0, 0, 0, 0,	//55
		0, 0, 0, 0, 0,	//60
		0, 0, (unsigned long)sys_new_name, 0, 0,	//65
		0, 0, 0, 0, 0,	//70
		0, 0, 0, 0, 0,	//75
		0, 0, 0, 0, 0,	//80
		0, 0, 0, 0, 0,	//85
		0, 0, 0, (unsigned long)sys_readlink, 0,	//90
		0, 0, 0, 0, 0,	//95
		0, 0, 0, 0, 0,	//100
		0, (unsigned long)sys_getuid, 0, (unsigned long)sys_getgid, 0,	//105
		0, (unsigned long)sys_geteuid, (unsigned long)sys_getegid, 0, 0,	//110
		0, 0, 0, 0, 0,	//115
		0, 0, 0, 0, 0,	//120
		0, 0, 0, 0, 0,	//125
		0, 0, 0, 0, 0,	//130
		0, 0, 0, 0, 0,	//135
		0, 0, 0, 0, 0,	//140
		0, 0, 0, 0, 0,	//145
		0, 0, 0, 0, 0,	//150
		0, 0, 0, 0, 0,	//155
		0, 0, (unsigned long) sys_arch_prctl, 0, 0,	//160
		0, 0, 0, 0, 0,	//165
		0, 0, 0, 0, 0,	//170
		0, 0, 0, 0, 0,	//175
		0, 0, 0, 0, 0,	//180
		0, 0, 0, 0, 0,	//185
		0, 0, 0, 0, 0,	//190
		0, 0, 0, 0, 0,	//195
		0, 0, 0, 0, 0,	//200
		0, 0, 0, 0, 0,	//205
		0, 0, 0, 0, 0,	//210
		0, 0, 0, 0, 0,	//215
		0, 0, (unsigned long) sys_set_tid_address, 0, 0,	//220
		0, 0, 0, 0, 0,	//225
		0, 0, 0, 0, 0,	//230
		(unsigned long)sys_exit_group, 0, 0, 0, 0,	//235
		0, 0, 0, 0, 0,	//240
		0, 0, 0, 0, 0,	//245
		0, 0, 0, 0, 0,	//250
		0, 0, 0, 0, 0,	//255
		0, 0, 0, 0, 0,	//260
		0, 0, 0, 0, 0,	//265
		0, 0, 0, 0, 0,	//270
		0, 0, 0, 0, 0,	//275
		0, 0, 0, 0, 0,	//280
		0, 0, 0, 0, 0,	//285
		0, 0, 0, 0, 0,	//290
		0, 0, 0, 0, 0,	//295
		0, 0, 0, 0, 0,	//300
		0, 0, 0, 0, 0,	//305
		0, 0, 0, 0, 0,	//310
		0, 0, 0, 0, 0,	//315
		0, 0, 0, 0, 0,	//300
		0, 0, 0, 0, 0,	//305
		0, 0, 0, 0, 0,	//310
		0, 0, 0, 0, 0,	//315
		0, 0, 0, 0, 0,	//300
		0, 0, 0, 0, 0,	//305
		0, 0, 0, 0, 0,	//310
		0, 0, 0, 0, 0,	//315
};



unsigned long user_stack;

unsigned char system_call_stack[0x1000];

extern void systemcall_handler(void);
unsigned long do_systemcall_handler(unsigned long rax, unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {


	if(systemcall_table[rax]) {

		unsigned long (*syscall_handler)(long, long, long, long) = (void*) systemcall_table[rax];

		//kprintf("system call %d, %p, %X, %p\n", rax, syscall_handler, arg1, arg2);

		unsigned long ret = syscall_handler(arg1, arg2, arg3, arg4);

		return ret;
	} else {

		kprintf("unknown system call %d\n", rax);
		while(1) asmv("hlt");
	}
	return 0;
}



//システムコール呼び出しの準備 MSRにハンドラの位置を書き込む MSRが無いCPUなんて知らぬ(´･ω･｀)
void systemcall_init(void) {

	write_msr(MSR_STAR, 0x0018000800000000);
	write_msr(MSR_LSTAR, (unsigned long) &systemcall_handler);
	write_msr(MSR_CSTAR, (unsigned long) &systemcall_handler);
}



void sys_exit(int err_code) {

	//TODO ここにプロセスが使用していたメモリ領域の開放、プロセスの開放を行う これを忘れると・・・




	kprintf("Task Exit\n");
	while(1) asmv("hlt");//とりあえず止める
}


void sys_read(unsigned int fd, char *buf, size_t count) {


	trace();
	return;
}

void sys_write(unsigned int fd, const char *buf, size_t count) {


	//struct process *process = lookup_process_by_pid(smp_table[apic_read(APIC_ID_R) >> 24].execute_process_pid);
	//buf = get_page_directory((unsigned long)buf, (unsigned long) process->page_tables) + ((unsigned long) buf & 0x1FFFFFUL);

	trace();


}


unsigned long sys_open(const char *filename, int flags, int mode) {

	kprintf("sys_open file %s, flags %X, mode %X\n", filename, flags, mode);
	return open(filename, flags, mode);
}
int sys_close(unsigned int fd) {

	kprintf("sys_close %d\n", fd);
	struct process *process = get_process();

	struct list_head *ptr;
	list_for_each(ptr, &process->fd_list) {

		struct file_descriptor *desc = list_entry(ptr, struct file_descriptor, list);
		if(desc->fd == fd) {	//対応しているファイルディスクリプタを見つけ、削除する

			close_fs(desc->node);
			list_del(&desc->list);
			kfree(desc, sizeof(struct file_descriptor));
			return 0;
		}

	}

	return -1;
}


int sys_fstat(unsigned int fd, struct stat *statbuf) {

	kprintf("[kernel/sys_fstat] fd %d, stat %p\n", fd, statbuf);
	return fstat(fd, statbuf);
}


unsigned long sys_lseek(unsigned int fd, unsigned long offset, unsigned int whence) {

	return lseek(fd, offset, whence);
}

long sys_mmap(unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, unsigned long fd, unsigned long offset) {

	kprintf("[kernel/sys_mmap] %p, len %X, prot %X, flags %X, fd %X, offset %X\n", addr, len, prot, flags, fd, offset);
	return mmap(addr, len, prot, flags, fd, offset);
}
int sys_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg) {

	kprintf("sys_ioctl fd %d, cmd %X, arg %p\n", fd, cmd, arg);
	return ioctl(fd, cmd, arg);
}

long sys_readv(unsigned long fd, const struct iovec *vec, unsigned long vlen) {

	kprintf("[kernel/sys_readv] fd %d, vec_base %p, vec_len %X, vlen %X\n", fd, vec->iov_base, vec->iov_len, vlen);
	return readv(fd, vec, vlen);
}

long sys_writev(unsigned long fd, const struct iovec *vec, unsigned long vlen) {

	//kprintf("[kernel/sys_writev] fd %d, vec %p, vlen %X\n", fd, vec, vlen);
	return writev(fd, vec, vlen);
}

long sys_access(const char *filename, int mode) {

	kprintf("[kernel/sys_access] filename %s, mode %X\n", filename, mode);
	return -ENOENT;
}

long sys_new_name(struct new_utsname *name) {

	kprintf("[kernel/sys_new_name] %p\n", name);
	return new_name(name);
}


unsigned int sys_readlink(const char *path, char *buf, int bufsiz) {

	kprintf("[kernel/sys_readlink] path %s, buf %p, size %X\n", path, buf, bufsiz);
	return 0;
}

unsigned int sys_getuid(void) {

	kprintf("[kernel/sys_getuid]\n");
	return getuid();
}

unsigned int sys_getgid(void) {

	kprintf("[kernel/sys_getuid]\n");
	return getgid();
}

unsigned int sys_geteuid(void) {

	kprintf("[kernel/sys_geteuid]\n");
	return geteuid();
}
unsigned int sys_getegid(void) {

	kprintf("[kernel/sys_getegid]\n");
	return getegid();
}

//VRAMのアドレスを返す ページテーブルを対応させる
unsigned long sys_get_vram(void) {

	struct process *process = get_process();
	map_page(0xE0000000, 0xE0000000, process->page_tables, FLAGS_USER_PAGE | FLAGS_WRITABLE_PAGE);

	return 0x00000000E0000000;
}

unsigned long monitor_height;

unsigned long sys_get_height(void) {

	return monitor_height;
}

unsigned long monitor_width;

unsigned long sys_get_width(void) {

	return monitor_width;
}

unsigned long monitor_depth;


unsigned long sys_get_depth(void) {

	return monitor_depth;
}


long sys_brk(unsigned long brk) {


	kprintf("[kernel/sys_brk] %p\n", brk);


	struct process *process = smp_table[apic_read(APIC_ID_R) >> 24].execute_process;

	if(!process) {

		return -1;
	}


	unsigned long start_brk	= 0xF000000;
	//unsigned long end_brk	= 0xF100000;

	if(!process->heap_base) {

		process->heap_base = alloc_memory_block(1);
		process->brk = start_brk;
		map_page(start_brk, (unsigned long) process->heap_base, process->page_tables, FLAGS_USER_PAGE | FLAGS_WRITABLE_PAGE | FLAGS_NO_EXECUTE);
	}


	if(brk < start_brk) {

		return process->brk;
	}


	if(process->brk == brk) {

		return process->brk;
	}
	if (brk <= process->brk) {
		process->brk = brk;
		return brk;
	}

	return process->brk = brk;
}



long sys_arch_prctl(int code, unsigned long addr) {

	kprintf("[kernel/sys_arch_prctl] code %X, addr %p\n", code, addr);
	return arch_prctl(code, addr);
}






long sys_set_tid_address(int *tidptr) {


	kprintf("[kernel/sys_set_tid_address] %p, %d\n", tidptr, *tidptr);

	return 0;
}




void sys_exit_group(int exit_code) {

	struct process *process = smp_table[apic_read(APIC_ID_R) >> 24].execute_process;
	kprintf("STOP process %s, id %d\n", process->name, apic_read(APIC_ID_R) >> 24);

	STOP;
}























