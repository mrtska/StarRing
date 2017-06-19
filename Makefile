export LANG=C

#螳夂ｾｩ鄒､
HOME_DIR	= "$(CURDIR)"

CPP		:= /usr/bin/g++ -nostdlib -mcmodel=kernel -ffreestanding -pipe -std=c++11 -mno-red-zone -Wall
CC		:= /usr/bin/gcc -nostdlib -mcmodel=kernel -ffreestanding -pipe -mno-red-zone  -Wall
LD		:= /usr/bin/ld 
AS		:= /usr/bin/as
OBJCOPY	:= /usr/bin/objcopy
NM		:= /usr/bin/nm
RM		:= rm
MAKE	:= make
CP		:= cp
CD		:= cd
MAKE	:= make
INCLUDE := include

export CC
export CPP
export LD
export OBJCOPY
export NM
export RM
export CP
export CD
export MAKE
export INCLUDE


OBJECTS := boot/boot.sys kernel/kernel.sys drivers/drivers.sys acpica/acpica.sys #fs/fs.sys 

.PHONY: compile
compile:
	nasm -f bin -o boot/ap_boot.o boot/ap_boot.S
#	touch ./boot/boot.S
	$(RM) -f boot/boot.o
	
	$(CD) boot;$(MAKE) 		#
#	$(CD) internal;$(MAKE)	#
	$(CD) kernel;$(MAKE)	#
	$(CD) acpica;$(MAKE)	#ACPICA
#	$(CD) fs;$(MAKE)		#
	$(CD) drivers;$(MAKE)	#


.PHONY: install
install:
	
	$(LD) -nodefaultlibs -static -Map kernel.map -T boot/bootlinker.ld -o kernel.sys $(OBJECTS) -L/usr/lib/gcc/x86_64-pc-linux-gnu/7.1.0/ -L/usr/lib64 -lsupc++ -lgcc_eh -lgcc -lstdc++
	
	  
.PHONY: copy 
copy:
	
	$(CP) -f ./kernel.sys /cygdrive/g/KERNEL.SYS
	sync
	
.PHONY: clean
clean:
	$(CD) boot;$(MAKE) clean
#	$(CD) internal;$(MAKE) clean
	$(CD) kernel;$(MAKE) clean
	$(CD) acpica;$(MAKE) clean
#	$(CD) fs;$(MAKE) clean
	$(CD) drivers;$(MAKE) clean
	
.PHONY: test
test:
	$(CD) boot;$(MAKE) test
	$(CD) kernel;$(MAKE) test
	
	
.PHONY: mount
mount:
	imdisk -a -m F: -t file -f "C:\Users\StarRing\VirtualBox VMs\StarRing\HardDisk-flat.vmdk" -o hd -v 1 -s 1GB

.PHONY: umount
umount:
	imdisk -D -m F:
	