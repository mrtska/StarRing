#pragma once


namespace starring::arch {

#define INTERRUPT_HANDLER extern "C"




    struct registers {

        unsigned long rip;
        unsigned long cs;
        unsigned long rflags;
        unsigned long rsp;
        unsigned long ss;
        
        unsigned long rbp;
        unsigned long r15;
        unsigned long r14;
        unsigned long r13;
        unsigned long r12;
        unsigned long r11;
        unsigned long r10;
        unsigned long r9;
        unsigned long r8;
        unsigned long rdi;
        unsigned long rsi;
        unsigned long rdx;
        unsigned long rcx;
        unsigned long rbx;
        unsigned long rax;
    };


    union interrupt_descriptor {

        struct {

            unsigned short offset0_15;
            unsigned short selector;
            unsigned short attribute;
            unsigned short offset16_31;

            unsigned int offset32_63;
            unsigned int reserved;
        };

        struct {

            unsigned short offset0_15;
            unsigned short selector;

            unsigned char ist : 3;
            unsigned char reserved0 : 5;
            unsigned char gate : 4;
            unsigned char reserved1 : 1;
            unsigned char dpl : 2;
            unsigned char p : 1;

            unsigned short offset16_31;

            unsigned int offset32_63;
            unsigned int reserved;

        } bitfield;


    } __attribute__((packed));


    class interrupt_handler {
        friend class interrupt_descriptor_table;

    private:
        //適用されている割り込みベクタ
        unsigned int vector;

        //割り込みハンドラの名前
        const char *name;

    public:
        virtual void handler(struct registers *regs) = 0;

        interrupt_handler(const char *name) {

            this->name = name;
        }

        const char *get_name() {

            return this->name;
        }

        unsigned int get_vector() {

            return this->vector;
        }

    };

    enum class idt_gate {

        TASK_GATE = 5, //最近のOSでは使わない
        INTERRUPT_GATE = 0xE, //割り込みゲート
        TRAP_GATE = 0xF //トラップゲート
    };

    class interrupt_descriptor_table {

    private:
        interrupt_descriptor *idt_pointer;

        interrupt_handler *interrupt_handlers[0x100];

        void load();
    public:

        void apply(unsigned int vector, interrupt_handler *handler, idt_gate gate);
        void init();
    };




}






















