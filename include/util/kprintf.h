
#pragma once
#include <starring.h>

namespace starring::util {

    class vga_textmode {

    public:
        int x;
        int y;


        void move() {

            int cur_pos = this->x + this->y * 80;
            outb(0x3D4, 15);
            outb(0x3D5, cur_pos);
            outb(0x3D4, 14);
            outb(0x3D5, cur_pos >> 8);
        }
        void add() {

            this->x++;
            if(this->x > 80) {

                this->y++;
                this->x = 0;
            }
            move();
        }
        void newline() {

            this->x = 0;
            this->y++;
            move();
        }
    };

    //テキストモード時にB8000に書き込み
    void kputs(const char* msg);
    void kputs(unsigned long msg);
    void kputs(void* num);
    void kprint(const char c);
}

