
#pragma once




#define GDT_ENTRY_KERNEL_CS			1
#define GDT_ENTRY_KERNEL_DS			2
#define GDT_ENTRY_DEFAULT_USER32_CS	3
#define GDT_ENTRY_DEFAULT_USER_DS	4
#define GDT_ENTRY_DEFAULT_USER_CS	5
#define GDT_ENTRY_KERNEL_TSS		6
#define GDT_ENTRY_USER_TSS			7







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


extern class gdt gdt;


class gdt final {


private:

	union global_descriptor *gdt_pointer;

public:


	gdt(void *gdt_pointer) {

		this->gdt_pointer = static_cast<union global_descriptor*>(gdt_pointer);
	}

	//GDTを初期化
	void gdt_init(void);
	void load_gdtr();


};
























