#ブート時
FILE := keyboard.o hpet.o ahci.o fb/vbox.o #vbox.o vmware.o ata.o beep.o dma.o serial.o storage.o ahci.o
TARGET := drivers.sys


.cpp.o:
	#$(CC) -mcmodel=kernel -I../$(INCLUDE) -include ../acpica/include/platform/acstarring.h -ffreestanding -nostdlib -mno-red-zone -Wall -c $< -o $@
	$(CPP) -mcmodel=kernel -I../$(INCLUDE) -ffreestanding -nostdlib -mno-red-zone -Wall -c $< -o $@

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
	$(CC) -S -I../$(INCLUDE) -o test.S page.c

