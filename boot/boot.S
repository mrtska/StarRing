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

    cli 
    movl %eax, %edi
    movl %ebx, %esi
1:
hlt
jmp 1b

.code64
    cli