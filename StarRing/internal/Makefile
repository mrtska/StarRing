#繝悶�ｼ繝域凾
FILE := printf.o tag_parse.o
TARGET := internal.sys


.c.o:
	$(CC) -mcmodel=kernel -I../$(INCLUDE) -ffreestanding -nostdlib -mno-red-zone -Wall -c $< -o $@

.S.o:
	nasm -felf64 -o $@ $<

#繧ｳ繝ｳ繝代う繝ｫ
compile: $(TARGET)
$(TARGET):$(FILE)

	$(LD) -r -Map $(TARGET).map -o $(TARGET) $(FILE)


#繝舌う繝翫Μ豸亥悉
clean:
	$(RM) -f $(FILE) $(TARGET)


#繝�繧ｹ繝医さ繝ｼ繝�
test:
#	$(CC) -E -I../$(INCLUDE) -o test.E main.c

