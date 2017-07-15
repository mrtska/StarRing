#include <arch/idt.h>
#include <util/kprintf.h>
#include <string>

extern unsigned long interrupt_descriptor_table_address;

namespace starring::arch {

    
    struct idtr {

        unsigned short limit;
        unsigned long base;
    } __attribute__((packed)) idtr;

    void interrupt_descriptor_table::load() {

        idtr.limit = 256 * 16 - 1;
        idtr.base = reinterpret_cast<unsigned long>(idt_pointer);

        asmv("lidt %0" :: "m"(idtr));
    }

    void interrupt_descriptor_table::init() {


        idt_pointer = reinterpret_cast<interrupt_descriptor*>(&interrupt_descriptor_table_address);
        util::kputs("IDT initlization.");

        

        load();
    }


    extern "C" void internal_handler(arch::registers *regs) {


        util::kputs("interrupt!");

    }
    extern "C" {
    

        void __attribute__((unused)) _() {

            asmv("internal_handler_raw:");
            asmv("push %rax");
            asmv("push %rbx");
            asmv("push %rcx");
            asmv("push %rdx");
            asmv("push %rsi");
            asmv("push %rdi");
            asmv("push %r8");
            asmv("push %r9");
            asmv("push %r10");
            asmv("push %r11");
            asmv("push %r12");
            asmv("push %r13");
            asmv("push %r14");
            asmv("push %r15");
            asmv("push %rbp");

            asmv("movq %rsp, %rdi");
            asmv("call internal_handler");

            asmv("pop %rbp");
            asmv("pop %r15");
            asmv("pop %r14");
            asmv("pop %r13");
            asmv("pop %r12");
            asmv("pop %r11");
            asmv("pop %r10");
            asmv("pop %r9");
            asmv("pop %r8");
            asmv("pop %rdi");
            asmv("pop %rsi");
            asmv("pop %rdx");
            asmv("pop %rcx");
            asmv("pop %rbx");
            asmv("pop %rax");

            asmv("iretq");
        }


        extern unsigned long internal_handler_raw;
    }

    void interrupt_descriptor_table::apply(unsigned int vector, interrupt_handler *handler, idt_gate gate) {

        handler->vector = vector;

        auto idt = &idt_pointer[vector];
        this->interrupt_handlers[vector] = handler;

        unsigned long ptr = reinterpret_cast<unsigned long>(&internal_handler_raw);

        idt->offset0_15 = ((unsigned long)(ptr) & 0xFFFF);
        idt->offset16_31 = (((unsigned long)(ptr) >> 16) & 0xFFFF);
        idt->offset32_63 = ((unsigned long)(ptr) >> 32);

        idt->bitfield.dpl = 0;
        idt->bitfield.gate = static_cast<unsigned char>(gate);
        idt->bitfield.selector = 0x8;   //カーネル コードセグメント
        idt->bitfield.p = 1;
        

    }


}