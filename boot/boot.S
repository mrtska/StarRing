.code32
.section .boot
#define MAGIC 0xE85250D6
#define ARCH 0

//マルチブート2ヘッダ
MB2_HEADER:
    .long MAGIC
    .long ARCH
    .long MB2_HEADER_END  - MB2_HEADER
    .long -(MAGIC + ARCH + (MB2_HEADER_END - MB2_HEADER))
    .long 0     //ヘッダ終了
    .long 8
MB2_HEADER_END:

.globl start
start:

    //割り込み無効
    cli
    cld

    //GRUB2から渡される情報を退避
    movl %eax, %edi
    movl %ebx, %esi

    //EFLAGS初期化
    push $0
    popf

#define PG_PRESENT  (1<<0)  //ページが有効か
#define PG_WRITE    (1<<1)  //ページが書き込み可能か
#define PG_USER     (1<<2)  //ページがユーザーページかどうか
#define PG_PWT      (1<<3)  //ライトスルーか
#define PG_NOCACHE  (1<<4)  //キャッシュするか
#define PG_ACCESSED (1<<5)  //ページにアクセスされたか
#define PG_DIRTY    (1<<6)  //ページに書き込まれたか
#define PG_LARGE    (1<<7)  //ラージページか
#define PG_GLOBAL   (1<<8)  //グローバルページか

    //PML4にPDPTを設定
    movl $(boot_pdpt) + (PG_PRESENT | PG_WRITE), (boot_pml4)

    //PDPTにPDを設定
    movl $(boot_pd) + (PG_PRESENT | PG_WRITE), (boot_pdpt)

    //0xFFFFFFFF80000000- ページディレクトリポインター → PML4
    movl $(boot_pdpt) + (PG_PRESENT | PG_WRITE), (boot_pml4) + 0xFF8
    //0xFFFF880000000000-
    movl $(boot_pdpt) + (PG_PRESENT | PG_WRITE), (boot_pml4) + 0x880

    //0xFFFFFFFF80000000- ページディレクトリ → ページディレクトリポインター
    movl $(boot_pd) + (PG_PRESENT | PG_WRITE), (boot_pdpt) + 0x0FF0

    //0x0000000000000000- ページディレクトリ → ページディレクトリポインター
    //movl $(PG_PRESENT | PG_WRITE | PG_LARGE | PG_GLOBAL), (boot_pd)
    //movl $0, (boot_pd) + 4


    movl $(boot_pd), %ebx
	movl $(PG_PRESENT | PG_WRITE | PG_LARGE | PG_GLOBAL), %eax
	movl %eax, (%ebx)
	movl $0, 4(%ebx)

    movl %cr4, %eax
    bts $5, %eax    //PAE有効化
    bts $8, %eax    //グローバルページ有効化
    movl %eax, %cr4

    //ブート用PML4をCR3に設定
    movl $(boot_pml4), %eax
    movl %eax, %cr3

    movl $0xC0000080, %ecx
    rdmsr
    bts $0, %eax    //SCE syscall, sysretを有効化
    bts $8, %eax    //LME ロングモード有効化
    bts $11, %eax   //NXE 実行不可ページ有効化
    wrmsr

    movl %cr0, %eax
//  bts $1, %eax    //マルチブート2なので既に建っている
    bts $31, %eax   //PE ページング有効化
    movl %eax, %cr0

    movl $(boottime_gdt_start), (boottime_gdtr) + 0x2
    lgdt (boottime_gdtr)

    ljmp $0x8, $(longmode)

.align 0x1000

//ブート用GDT
boottime_gdt_start:
    .quad 0x0
    .quad 0x002F98000000FFFF
boottime_gdt_end:

boottime_gdtr:
    .word boottime_gdt_end - boottime_gdt_start - 1
    .long 0x0


.align 0x1000
boot_pml4: //ブート用PML4
    .space 0x1000

boot_pdpt:  //ブート用ページディレクトリポインターテーブル
    .space 0x1000
boot_pd:    //ブート用ページディレクトリ
    .space 0x1000

.code64
.extern main
.align 8
longmode:

    //C++のmainへ
    movq $(main), %rax
    jmp *%rax

1:
hlt
jmp 1b
