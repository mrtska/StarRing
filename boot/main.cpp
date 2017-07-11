
#include <starring.h>
#include <util/kprintf.h>
#include <string>

extern unsigned long bss_start;
extern unsigned long bss_end;

namespace starring::boot {

    static void disable_legacy_pic() {

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

    static void clear_bss() {

        unsigned long *start = &bss_start;
        unsigned long *end = &bss_end;
        while(start < end) {
            *start++ = 0x00;
        }
    }

    extern "C" void main(unsigned long magic, unsigned long mboot) {

        disable_legacy_pic();
        clear_bss();

        util::kputs("Hello, World!!!");
        
        starring.gdt.init();



        starring.boot();

        util::kputs("boot end");
        STOP;
    }
}



