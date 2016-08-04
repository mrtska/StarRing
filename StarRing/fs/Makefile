#繝悶�ｼ繝域凾
FILE := vfs.o fat32.o null_device.o open.o close.o write.o read.o input.o seek.o mmap.o stdout.o stderr.o stat.o
TARGET := fs.sys


.c.o:
	$(CC) -mcmodel=kernel -I../$(INCLUDE) -include ../acpica/include/platform/acstarring.h -ffreestanding -nostdlib -mno-red-zone -Wall -c $< -o $@

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
	$(CC) -S -I../$(INCLUDE) -o test.S page.c

