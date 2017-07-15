#include <starring.h>
#include <drivers/keyboard.h>
#include <util/kprintf.h>

namespace starring::drivers {

    void keyboard_interrupt_handler::handler(arch::registers *regs) {

        util::kputs("keyboard interrupt.");

    }

    void keyboard_device::init() {

        util::kputs("keyboard initialization.");
        starring.idt.apply(0x22, &interrupt_handler, arch::idt_gate::INTERRUPT_GATE);
    }




}