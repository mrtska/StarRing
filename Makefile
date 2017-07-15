
CC		:= clang
CPP		:= clang++
LD		:= ld.lld

CFLAGS := -g -MMD -nostdlib -mcmodel=kernel -ffreestanding -pipe -mno-red-zone -Wall
CPPFLAGS := -g -MMD -mno-sse -stdlib=libc++ -mcmodel=kernel -ffreestanding -pipe -std=c++1z -mno-red-zone -Wall

HEADERFILES := $(wildcard **/*.h)

build/%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@
build/%.o: %.cpp
	$(CPP) $(CPPFLAGS) -I./include -I./include/std -c $< -o $@

ASMFILES := $(wildcard **/*.S)
CPPFILES := $(wildcard **/*.cpp)
OBJECTS = $(addprefix build/,$(ASMFILES:.S=.o)) $(addprefix build/,$(CPPFILES:.cpp=.o))

OBJDIR := build build/boot build/arch build/kernel build/util build/drivers

mkdir:
	mkdir -p $(OBJDIR)


build: mkdir $(OBJECTS) install

-include $(OBJECTS:.o=.d)

install:
	$(LD) -static -m elf_x86_64 -Map kernel.map -script boot/linkerscript.ld -o kernel.sys /usr/local/lib/libc++.a /usr/local/lib/libc++abi.a /usr/local/lib/libunwind.a $(OBJECTS)


.PHONY: clean
clean:
	rm -rf build
	echo "cleaned."

test:
	echo $(OBJECTS)