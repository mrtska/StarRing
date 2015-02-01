#ブート時
FILE := printf.o string.o
TARGET := lib.sys


.c.o:
	$(CC) -mcmodel=kernel -nostdlib -I../$(INCLUDE) -ffreestanding -Wall -c $< -o $@

.S.o:
	nasm -felf64 -o $@ $<

#コンパイル
compile: $(TARGET)
$(TARGET):$(FILE)
	
	$(LD) -r -Map $(TARGET).map -o $(TARGET) $(FILE)


#バイナリ消去
clean:
	$(RM) -f $(FILE) $(TARGET)


#テストコード
test:
#	$(CC) -E -I../$(INCLUDE) -o test.E main.c

