#ブート時
FILE := stdc++.o string.o printf.o idt.o physicalmemory.o virtualmemory.o apic.o interrupt.o exception.o trap.o gdt.o acpi.o acpica.o slaballocator.o pci.o sched.o smp.o wait.o #pci.o smp.o wait.o task.o systemcall.o elf.o ipi.o vmm.o ioctl.o
#FILE += prctl.o sys.o env.o

TARGET := kernel.sys


.c.o:
	$(CC) -I../$(INCLUDE) -c $< -o $@

.cpp.o:
	$(CPP) -I../$(INCLUDE) -c $< -o $@

.S.o:
	$(CC) -I../$(INCLUDE) -c $< -o $@

#コンパイル
compile: $(TARGET)
$(TARGET):$(FILE)

	$(LD) -r -Map $(TARGET).map -o $(TARGET) $(FILE)


#バイナリ消去
clean:
	$(RM) -f $(FILE) $(TARGET)


#テストコード
test:
	$(CC) -masm=intel -S -I../$(INCLUDE) -o test.S slab.c

