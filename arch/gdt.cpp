
#include <arch/gdt.h>
#include <starring.h>
#include <util/kprintf.h>
#include <string>

extern unsigned long global_descriptor_table_address;

namespace starring::arch {

    struct gdtr {

        unsigned short limit;
        unsigned long base;
    } __attribute__((packed)) gdtr;

    //GDTをgdtrレジスタにロードする
    //farジャンプをしないと切り替わらない
    void global_descriptor_table::load() {

        gdtr.limit = 0xFF;
        gdtr.base = reinterpret_cast<unsigned long>(gdt_pointer);
    
        asmv("lgdt %0" :: "m"(gdtr));

        asmv("mov $0x0, %ax");
        asmv("mov %ax, %ds");
        asmv("mov %ax, %es");
        asmv("mov %ax, %fs");
        asmv("mov %ax, %gs");
        asmv("mov %ax, %ss");

        //farジャンプして切り替え
        asmv("pushq $0x8");
        asmv("pushq $(jmpl)");
        asmv("lretq");
        asmv("jmpl:");
    }

    //GDTに追加
    void global_descriptor_table::set(const int selector, unsigned int base, unsigned int limit, unsigned short flags) {

        if(gdt_pointer == nullptr) {

            util::kputs("PANIC.");
            util::kputs(__FUNCTION__);
            STOP;
        }

        auto gate = &gdt_pointer[selector];

        gate->a = (limit & 0xFFFF) | ((base & 0xFFFF) << 16);
        gate->b = ((base & 0xFF0000) >> 16) | ((flags & 0xF0FF) << 8) | (limit & 0xF0000) | (base & 0xFF000000);

    }


    //本番GDTを初期化
    void global_descriptor_table::init() {

        gdt_pointer = reinterpret_cast<global_descriptor*>(&global_descriptor_table_address);

        //必要なGDTを追加
        set(GDT_ENTRY_KERNEL_CODE_SEGMENT, 0, 0xFFFFF, 0xA09B);
        set(GDT_ENTRY_KERNEL_DATA_SEGMENT, 0, 0xFFFFF, 0xC093);
        load();
    }
}