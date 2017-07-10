
#pragma once


//インラインアセンブラ
#define asmv __asm__ __volatile__

#define lfence() asmv("lfence":::"memory")
#define sfence() asmv("sfence":::"memory")
#define mfence() asmv("mfence":::"memory")

//停止
#define STOP while(true) asmv("hlt")


#define INTERRUPT_HANDLER extern "C"

namespace starring {


    //OUT命令 オペランドサイズ可変
    static inline void outb(int port, unsigned char value) {

        asmv("outb %b0, %w1" : : "a"(value), "Nd"(port));
    }
    static inline void outw(int port, unsigned short value) {

        asmv("outw %w0, %w1" : : "a"(value), "Nd"(port));
    }
    static inline void outl(int port, unsigned int value) {

        asmv("outl %0, %w1" : : "a"(value), "Nd"(port));
    }

    //IN命令 オペランドサイズ可変
    static inline unsigned char inb(int port) {

        unsigned char value;
        asmv("inb %w1, %0" : "=a"(value) : "Nd"(port));
        return value;
    }
    static inline unsigned short inw(int port) {

        unsigned short value;
        asmv("inw %w1, %0" : "=a"(value) : "Nd"(port));
        return value;
    }
    static inline unsigned int inl(unsigned short port) {

        unsigned int value;
        asmv("inl %w1, %0" : "=a"(value) : "Nd"(port));
        return value;
    }

    class starring {

        public:
        void boot();


    };

    extern class starring starring;
}

