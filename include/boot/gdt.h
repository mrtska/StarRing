#pragma once



namespace starring::boot {
        
    union global_descriptor {

        unsigned long data;

        struct {

            unsigned int a;
            unsigned int b;
        };

        struct {
            unsigned short limit0;
            unsigned short base0;
            unsigned base1 :8, type :4, s :1, dpl :2, p :1;
            unsigned limit1 :4, avl :1, l :1, d :1, g :1, base2 :8;
        };
    } __attribute__((packed));

    class global_descriptor_table {

    private:
        const int GDT_ENTRY_KERNEL_CODE_SEGMENT = 1;
        const int GDT_ENTRY_KERNEL_DATA_SEGMENT = 2;

        global_descriptor *gdt_pointer = nullptr;

        void set(const int selector, unsigned int base, unsigned int limit, unsigned short flags);
        void load();
    public:


        void init();
    };
}






