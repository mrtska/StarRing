
#include <starring.h>
#include <util/kprintf.h>
#include <string>


namespace starring::util {

    const unsigned short *TEXT_MODE_PTR = reinterpret_cast<const unsigned short*>(0xFFFFFFFF800B8000);

    vga_textmode textmode;
    

    void kprint(const char c) {

        unsigned short *where = const_cast<unsigned short*>(TEXT_MODE_PTR + (textmode.y * 80 + textmode.x));
        *where = c | 7 << 8;
        textmode.add();
    }

    void kputs(const char *str) {

        for(int i = 0; i < std::strlen(str); i++) {

            kprint(str[i]);
        }
        textmode.newline();
    }
}


















