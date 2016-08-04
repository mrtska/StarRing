#繝悶�ｼ繝域凾
FILE := string.o printf.o idt.o physicalmemory.o#gdt.o apic.o ioapic.o syscall.o trap.o page.o spinlock.o alloc.o page_fault.o cpuid.o slab.o acpi.o pci.o smp.o wait.o task.o systemcall.o elf.o ipi.o vmm.o ioctl.o
#FILE += prctl.o sys.o env.o

TARGET := kernel.sys


.cpp.o:
	#$(CC) -mcmodel=kernel -I../$(INCLUDE) -include ../acpica/include/platform/acstarring.h -ffreestanding -nostdlib -mno-red-zone -Wall -c $< -o $@
	$(CC) -mcmodel=kernel -I../$(INCLUDE) -ffreestanding -nostdlib -mno-red-zone -Wall -c $< -o $@

.S.o:
	$(CC) -mcmodel=kernel -I../$(INCLUDE) -ffreestanding -nostdlib -mno-red-zone -Wall -c $< -o $@

#繧ｳ繝ｳ繝代う繝ｫ
compile: $(TARGET)
$(TARGET):$(FILE)

	$(LD) -r -Map $(TARGET).map -o $(TARGET) $(FILE)


#繝舌う繝翫Μ豸亥悉
clean:
	$(RM) -f $(FILE) $(TARGET)


#繝�繧ｹ繝医さ繝ｼ繝�
test:
	$(CC) -masm=intel -S -I../$(INCLUDE) -o test.S slab.c

