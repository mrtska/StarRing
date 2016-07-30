/*
[Module main.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php

Created on: 2014/04/02

*/

#include <system.h>
#include <internal/stdio.h>
#include <internal/tag_parse.h>
#include <multiboot2.h>

#include <desc.h>
#include <cpuid.h>
#include <ioapic.h>
#include <apic.h>
#include <trap.h>
#include <timer.h>
#include <gdt.h>
#include <mem/alloc.h>
#include <page/page.h>

#include <wait.h>
#include <smp.h>
#include <slab.h>
#include <acpi.h>
#include <task.h>
#include <systemcall.h>
#include <ipi.h>
#include <mem/vmm.h>
#include <spinlock.h>
#include <env.h>

#include <drivers/pci.h>
#include <drivers/hpet.h>
#include <drivers/vbox.h>
#include <drivers/vmware.h>
#include <drivers/ata.h>
#include <drivers/serial.h>
#include <drivers/keyboard.h>
#include <drivers/storage.h>

#include <fs/vfs.h>
#include <fs/fat32.h>
#include <fs/input.h>
#include <fs/stdout.h>
#include <fs/stderr.h>

//BSSセクション
extern unsigned int _bss_start;
extern unsigned int _bss_end;

//BSSセクション初期化
static void bss_init(void) {

	unsigned int *start = &_bss_start;
	unsigned int *end = &_bss_end;
	while(start < end) {
		*start++ = 0x00;
	}
}

//画面をクリア
static void cls(void) {

	int c = 25 * 80;
	while(c--) {
		kprintf(" ");
	}
	resetPos();
}

//レガシーPICを無効化
static void disable_legacy_pic(void) {

	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);

	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);

	outb(0x21, 0xFF);
	outb(0xA1, 0xFF);
}

//BSPレディ
static volatile unsigned char bsp_ready;

//エントリポイント
void main(unsigned long magic, unsigned long mboot) {


	//BSSセクションを0初期化する
	bss_init();

	serial_init();

	//マルチブート2のマジックナンバー確認
	if(magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
		kprintf("Invalid magic number: 0x%X\n", magic);
		kprintf("boot failed...");
		return;
	}

	//レガシーPICの無効化
	disable_legacy_pic();

	//一時的に割り込みを無効にする
	asmv("cli");


	//画面をクリア
	cls();

	idt_init();
	//例外ハンドラ登録
	trap_init();

	kprintf("Welcome to StarRing! (compiled at (Date %s), (Time %s))\n", __DATE__, __TIME__);



	//GRUB2からの情報をパースする
	tag_parse(mboot);

	unsigned long memory_size = get_max_physical_address();
	kprintf("physical memory size %p\n", memory_size);

	//ページアロケーター初期化
	allocator_init(memory_size);




	//仮想メモリマネージャー初期化
	vmm_allocator_init();

	//正式なページテーブルを初期化&ロード
	page_init(memory_size);

	//スタックポインタを正しいものに
	{
		void *p = &ap_stack[0][0x1000];
		asmv("movq %0, %%rsp" : : "r"(p));
		p = &ap_stack[0][0];
		asmv("movq %0, %%rbp" : : "r"(p));
	}
	//スラブアロケーター初期化
	init_slaballocator();

	//正式なGDTをロード
	gdt_init();

	//マシンチェック
	machine_check();

	//ACPIテーブルをパース
	acpi_init();


	//ACPIテーブルを元にAPICを初期化
	apic_init();

	//ウェイトリスト初期化
	wait_list_init();


	/*
	 kprintf("dsdt %p\n", acpi_table.dsdt);
	 kprintf("facp %p\n", acpi_table.facp);
	 kprintf("rsdp %p\n", acpi_table.rsdp);
	 kprintf("rsdt %p\n", acpi_table.rsdt);
	 kprintf("xsdt %p\n", acpi_table.xsdt);*/


	//acpica_start();

	//PCIデバイスを初期化
	pci_init();

	//IOAPICを初期化
	io_apic_init(apic_info.ioapic_addr);

	//キーボードドライバを初期化
	keyboard_init();



	//TSSを初期化
	tss_init();

	//システムコール初期化
	systemcall_init();

	//プロセッサ間割り込み初期化
	ipi_init();

	//HPETドライバ初期化
	hpet_init();


	//VFS初期化
	vfs_init();

	//キーボードブロックデバイス生成
	struct fs_node *input = create_input_device(alloc_file_system(), 0);
	smp_table_set_input_node(0, input);

	//APIC有効化
	apic_write(APIC_SVR, apic_read(APIC_SVR) | APIC_ENABLED);

	//割り込みを許可
	asmv("sti");

	//HPETの駆動を開始
	hpet_oneshot(hpet_msec_to_tick(100));

	//他のCPUを起動 BSP
	smp_init();
	bsp_ready = 1;

	scan_storage_device();
	STOP;


	//マウントポイント作成
	vfs_mount("/dev/null", create_null_device(alloc_file_system()));
	vfs_mount("/dev/zero", create_null_device(alloc_file_system()));
	vfs_mount("/dev/stdout", create_stdout(alloc_file_system()));
	vfs_mount("/dev/stderr", create_stderr(alloc_file_system()));

	environment_valiable_init();

	allocator_4k_init();



	//初期化プロセスを生成
	task_init();

	int x = 854;
	int y = 480;

	monitor_width = x;
	monitor_height = y;
	monitor_depth = 4;

	/*
	 if(pci_find_device(VMWARE_SVGA_VENDOR_ID, VMWARE_SVGA_DEVICE_ID)) {

	 vmware_graphics_init(x, y);
	 } else {

	 vbox_graphics_init(x, y);
	 }
	 */

	kprintf("bsp end\n");

	while(1) {

		asmv("hlt");
	}

}


//APメイン処理
void do_ap_main(void) {

	int apic_id = apic_read(APIC_ID_R) >> 24;

	//一応トラップを初期化する
	trap_init();

	//TSS初期化
	tss_init();

	//システムコール呼び出しを初期化
	systemcall_init();

	//BSPの処理終了を待つ
	while(!bsp_ready);

	//SMPテーブルにAPを登録
	smp_ap_init();
	//キーボードブロックデバイス生成
	struct fs_node *input = create_input_device(alloc_file_system(), apic_id);
	smp_table_set_input_node(apic_id, input);

	//APIC有効化
	apic_write(APIC_SVR, apic_read(APIC_SVR) | APIC_ENABLED);

	//割り込みを有効化
	asmv("sti");
	smp_table_set_flags(apic_id, SMP_PROCESSOR_INTERRUPT_RECEIVABLE);

	//kprintf("ap_main id = %d\n", apic_id);

	//APはアイドル時、ここのHLTで止まっている
	while(1) {

		schedule(NULL);
		asmv("hlt");
	}
}

//アプリケーションプロセッサ C言語エントリポイント
void ap_main(void) {

	//APのページテーブルを変更する
	write_cr3(VIRTUAL_ADDRESS_TO_PHYSICAL_ADDRESS(kernel_pml4));

	//スタックポインタを正しく設定
	void *p = &ap_stack[apic_read(APIC_ID_R) >> 24][0x2000];
	asmv("movq %0, %%rsp" : : "r"(p));
	//---GDT/IDT更新---
	gdt_init();
	idt_init();
	//------

	do_ap_main();
}
