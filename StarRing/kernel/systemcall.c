/*
 [Module systemcall.c]
 Copyright(c) 2015 mrtska.starring

 This software is released under the MIT License.

 http://opensource.org/licenses/mit-license.php
 Created on: 2014/11/06
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
#include <mem/alloc.h>

#include <fs/fs.h>
#include <fs/vfs.h>
#include <fs/ioctl.h>
#include <fs/read.h>
#include <fs/write.h>
#include <fs/open.h>
#include <fs/close.h>
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

void sys_exit(int err_code);
long sys_read(unsigned int fd, char *buf, size_t count);
long sys_write(unsigned int fd, const char *buf, size_t count);
unsigned long sys_open(const char *filename, int flags, int mode);
int sys_close(unsigned int fd);
int sys_stat(const char *filename, struct stat *statbuf);
int sys_fstat(unsigned int fd, struct stat *statbuf);

unsigned long sys_lseek(unsigned int fd, unsigned long offset, unsigned int whence);
long sys_mmap(unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, int fd, unsigned long offset);
long sys_mprotect(unsigned long start, size_t len, unsigned long prot);
long sys_munmap(unsigned long addr, unsigned long len);
int sys_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg);
long sys_readv(unsigned long fd, const struct iovec *vec, unsigned long vlen);
long sys_writev(unsigned long fd, const struct iovec *vec, unsigned long vlen);
long sys_access(const char *filename, int mode);
unsigned long sys_get_vram(void);
unsigned long sys_get_height(void);
unsigned long sys_get_width(void);
unsigned long sys_get_depth(void);
long sys_brk(unsigned long size);


int sys_getpid(void);

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

(unsigned long) sys_read, (unsigned long) sys_write, (unsigned long) sys_open, (unsigned long) sys_close, (unsigned long)sys_stat, (unsigned long) sys_fstat,	//0 ~ 5
		0, 0, (unsigned long) sys_lseek, (unsigned long) sys_mmap, (unsigned long)sys_mprotect,	//10
		(unsigned long)sys_munmap, (unsigned long) sys_brk, 0, 0, 0,	//15
		(unsigned long) sys_ioctl, 0, 0, (unsigned long) sys_readv, (unsigned long) sys_writev,	//20
		(unsigned long) sys_access, 0, 0, 0, 0,	//25
		0, 0, 0, 0, 0,	//30
		0, 0, 0, 0, 0,	//35
		0, 0, 0, (unsigned long) sys_getpid, 0,	//40
		0, 0, 0, 0, 0,	//45
		0, 0, 0, 0, 0,	//50
		0, 0, 0, 0, 0,	//55
		0, 0, 0, 0, 0,	//60
		0, 0, (unsigned long) sys_new_name, 0, 0,	//65
		0, 0, 0, 0, 0,	//70
		0, 0, 0, 0, 0,	//75
		0, 0, 0, 0, 0,	//80
		0, 0, 0, 0, 0,	//85
		0, 0, 0, (unsigned long) sys_readlink, 0,	//90
		0, 0, 0, 0, 0,	//95
		0, 0, 0, 0, 0,	//100
		0, (unsigned long) sys_getuid, 0, (unsigned long) sys_getgid, 0,	//105
		0, (unsigned long) sys_geteuid, (unsigned long) sys_getegid, 0, 0,	//110
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
		(unsigned long) sys_exit_group, 0, 0, 0, 0,	//235
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
unsigned long do_systemcall_handler(unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5, unsigned long arg6) {

	unsigned long rax;
	asmv("" : "=a"(rax));

	if(systemcall_table[rax]) {

		unsigned long (*syscall_handler)(long, long, long, long, long, long) = (void*) systemcall_table[rax];

		//kprintf("system call %d, %p, %X, %p\n", rax, syscall_handler, arg1, arg2);

		unsigned long ret = syscall_handler(arg1, arg2, arg3, arg4, arg5, arg6);

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
	while(1)
		asmv("hlt");
	//とりあえず止める
}

long sys_read(unsigned int fd, char *buf, size_t count) {

	kprintf("[kernel/sys_read] fd %d, buf %p, size %X\n", fd, buf, count);
	return read(fd, buf, count);
}

long sys_write(unsigned int fd, const char *buf, size_t count) {

	//kprintf("[kernel/sys_write] fd %d, buf %p, count %X\n", fd, buf, count);
	return write(fd, buf, count);
}

unsigned long sys_open(const char *filename, int flags, int mode) {

	//kprintf("[kernel/sys_open] file %s, flags %X, mode %X\n", filename, flags, mode);
	return open(filename, flags, mode);
}
int sys_close(unsigned int fd) {

	kprintf("[kernel/sys_close] fd %d\n", fd);

	return close(fd);
}

int sys_stat(const char *filename, struct stat *statbuf) {

	kprintf("[kernel/sys_stat] name %s, stat %p\n", filename, statbuf);
	return stat(filename, statbuf);
}

int sys_fstat(unsigned int fd, struct stat *statbuf) {

	kprintf("[kernel/sys_fstat] fd %d, stat %p\n", fd, statbuf);
	return fstat(fd, statbuf);
}

unsigned long sys_lseek(unsigned int fd, unsigned long offset, unsigned int whence) {

	return lseek(fd, offset, whence);
}

long sys_mmap(unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, int fd, unsigned long offset) {

	kprintf("[kernel/sys_mmap] %p, len %X, prot %X, flags %X, fd %X, offset %X\n", addr, len, prot, flags, fd, offset);
	addr = mmap(addr, len, prot, flags, fd, offset);
	kprintf("[kernel/sys_mmap] return %p\n", addr);
	return addr;
}

long sys_mprotect(unsigned long start, size_t len, unsigned long prot) {

	kprintf("[kernel/sys_mprotect] start %p, len %X, prot %X\n", start, len, prot);
	return mprotect(start, len, prot);
}


long sys_munmap(unsigned long addr, unsigned long len) {

	kprintf("[kernel/sys_munmap] addr %p, len %p\n", addr, len);
	return munmap(addr, len);
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
	struct fs_node *node = kopen(filename, 0);
	if(!node) {

		return -ENOENT;
	}

	return 0;
}

long sys_new_name(struct new_utsname *name) {

	kprintf("[kernel/sys_new_name] %p\n", name);
	return new_name(name);
}


int sys_getpid(void) {

	//kprintf("[kernel/sys_getpid]\n");
	return get_process()->pid;
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

	struct process *process = get_process();

	kprintf("[kernel/sys_brk] start %p, end %p\n", process->start_brk, process->end_brk);

	if(!process) {

		return -1;
	}

	if(brk < process->start_brk) {

		return process->start_brk;
	}

	if(process->end_brk == brk) {

		return process->end_brk;
	}

	if(brk <= process->end_brk) {
		process->end_brk = brk;
		return brk;
	}

	return process->end_brk = brk;
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
	kprintf("STOP process %s, id %d, exit %X\n", process->name, get_apic_id(), exit_code);

	STOP;
}

