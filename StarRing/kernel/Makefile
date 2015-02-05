#ブート時
FILE := string.o idt.o gdt.o apic.o ioapic.o syscall.o trap.o page.o spinlock.o alloc.o page_fault.o cpuid.o phys.o slab.o acpi.o pci.o smp.o wait.o task.o systemcall.o elf.o ipi.o vmm.o ioctl.o
FILE += prctl.o sys.o env.o

TARGET := kernel.sys


.c.o:
	$(CC) -mcmodel=kernel -I../$(INCLUDE) -include ../acpica/include/platform/acstarring.h -ffreestanding -nostdlib -mno-red-zone -Wall -c $< -o $@

.S.o:
	$(CC) -mcmodel=kernel -I../$(INCLUDE) -ffreestanding -nostdlib -mno-red-zone -Wall -c $< -o $@

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

