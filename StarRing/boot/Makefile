#繝悶�ｼ繝域凾
FILE := boot.o main.o
TARGET := boot.sys


.cpp.o:

	$(CC) -mcmodel=kernel -I../$(INCLUDE) -ffreestanding -nostdlib -mno-red-zone -Wall -c $< -o $@

.S.o:
	$(CC) -c -o $@ $<

#繧ｳ繝ｳ繝代う繝ｫ
compile: $(TARGET)
$(TARGET): $(FILE)
	$(LD) -r -Map boot.map -o $(TARGET) $(FILE)
	


#繝舌う繝翫Μ豸亥悉
clean:
	$(RM) -f $(FILE) $(TARGET)
	$(RM) -f *.E
	$(RM) -f ap_boot.o

#繝�繧ｹ繝医さ繝ｼ繝�
test:
	$(CC) -S -I../$(INCLUDE) -o test.s test.c

