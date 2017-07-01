
CC		:= gcc
CPP		:= g++

CFLAGS := -MMD -nostdlib -mcmodel=kernel -ffreestanding -pipe -mno-red-zone -Wall
CPPFLAGS := -MMD -nostdlib -mcmodel=kernel -ffreestanding -pipe -std=c++11 -mno-red-zone -Wall

HEADERFILES := $(wildcard **/*.h)

build/%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@
build/%.o: %.cpp
	$(CPP) $(CPPFLAGS) -I./include -c $< -o $@

ASMFILES := $(wildcard **/*.S)
CPPFILES := $(wildcard **/*.cpp)
OBJECTS = $(addprefix build/,$(ASMFILES:.S=.o)) $(addprefix build/,$(CPPFILES:.cpp=.o))

OBJDIR := build build/boot build/kernel

mkdir:
	mkdir -p $(OBJDIR)


build: mkdir $(OBJECTS) install

-include $(OBJECTS:.o=.d)

install:
	$(LD) -nodefaultlibs -static -Map kernel.map -T boot/linkerscript.ld -o kernel.sys $(OBJECTS) -L/usr/lib/gcc/x86_64-pc-linux-gnu/7.1.0/ -L/usr/lib64 -lsupc++ -lgcc_eh -lgcc -lstdc++


.PHONY: clean
clean:
	rm -rf build
	echo "cleaned."

test:
	echo $(OBJECTS)