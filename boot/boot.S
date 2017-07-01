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

    //GRUB2から渡される情報を退避
    movl %eax, %edi
    movl %ebx, %esi

    //EFLAGS初期化
    push $0
    popf

    cld

    movl $1, %eax
    movl $25, %ecx
    movl $(msg), %esi
    movl $0xB8000, %edx
copy:
    movb (%esi), %al
    movb %al, (%edx)

    incl %esi
    addl $2, %edx

    decl %ecx
    cmpl $0, %ecx
    je end
    jmp copy

end:
hlt
jmp end

.align 8
msg:
.ascii "Hello World! StarRing OS!"

#define PG_PRESENT  (1<<0)  //ページが有効か
#define PG_WRITE    (1<<1)  //ページが書き込み可能か
#define PG_USER     (1<<2)  //ページがユーザーページかどうか
#define PG_PWT      (1<<3)  //ライトスルーか
#define PG_NOCACHE  (1<<4)  //キャッシュするか
#define PG_ACCESSED (1<<5)  //ページにアクセスされたか
#define PG_DIRTY    (1<<6)  //ページに書き込まれたか
#define PG_LARGE    (1<<7)  //ラージページか
#define PG_GLOBAL   (1<<8)  //グローバルページか


boot_pdpt:  //ブート用ページディレクトリポインターテーブル
    .space 0x1000
boot_pd:    //ブート用ページディレクトリ
    .space 0x1000



1:
hlt
jmp 1b


.code64
.align 8
    cli