#pragma once




//割り込みディスクリプタ
union interrupt_descripter {

	struct {

		unsigned short offset0_15;
		unsigned short selecter;
		unsigned short attribute;
		unsigned short offset16_31;

		unsigned int offset32_63;
		unsigned int reserved;
	};

	struct {
	
		unsigned short offset0_15;
		unsigned short selecter;

		unsigned char ist : 3;
		unsigned char reserved0 : 5;
		unsigned char fixed : 4;
		unsigned char reserved1 : 1;
		unsigned char dpl : 2;
		unsigned char p : 1;

		unsigned short offset16_31;

		unsigned int offset32_63;
		unsigned int reserved;




	} bitfield;


} __attribute__((packed));

extern class idt idt;

class idt {

private:
	void *idt_pointer;

public:

	idt(void *idt_pointer);

	//割り込みベクタを初期化する
	void idt_init();

	//割り込みを登録する
	void register_interrupt(int id, union interrupt_descripter desc);

	void load_idtr();
};



