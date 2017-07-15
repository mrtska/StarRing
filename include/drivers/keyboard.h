#pragma once

#include <arch/idt.h>


namespace starring::drivers {



    class keyboard_interrupt_handler : public arch::interrupt_handler {

        virtual void handler(arch::registers *regs);

    public:
        keyboard_interrupt_handler() : interrupt_handler("keyboard") {


        }


    };


    class keyboard_device {

    private:
        class keyboard_interrupt_handler interrupt_handler;

    public:


        void init();

    };

}




