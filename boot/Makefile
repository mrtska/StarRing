#ブート時
FILE := boot.o main.o
TARGET := boot.sys


.cpp.o:

	$(CPP) -I../$(INCLUDE) -c $< -o $@

.S.o:
	$(CC) -c -o $@ $<

#コンパイル
compile: $(TARGET)
$(TARGET): $(FILE)
	
	$(LD) -r -Map boot.map -o $(TARGET) $(FILE)
	


#バイナリ消去
clean:
	$(RM) -f $(FILE) $(TARGET)
	$(RM) -f *.E
	$(RM) -f ap_boot.o

#テストコード
test:
	$(CC) -S -I../$(INCLUDE) -o test.s test.c

