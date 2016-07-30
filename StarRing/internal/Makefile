#ブート時
FILE := printf.o tag_parse.o
TARGET := internal.sys


.c.o:
	$(CC) -mcmodel=kernel -I../$(INCLUDE) -ffreestanding -nostdlib -mno-red-zone -Wall -c $< -o $@

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

