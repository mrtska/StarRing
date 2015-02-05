export LANG=C

#定義群
HOME_DIR	= "$(CURDIR)"

CC		:= x86_64-pc-linux-gnu-gcc -pipe -O2 -g
LD		:= x86_64-pc-linux-gnu-ld
AS		:= x86_64-pc-linux-gnu-as
OBJCOPY	:= x86_64-pc-linux-gnu-objcopy
NM		:= x86_64-pc-linux-gnu-nm
RM		:= rm
MAKE	:= make
CP		:= cp
CD		:= cd
MAKE	:= make
INCLUDE := include

export CC
export LD
export OBJCOPY
export NM
export RM
export CP
export CD
export MAKE
export INCLUDE


OBJECTS := boot/boot.sys internal/internal.sys kernel/kernel.sys acpica/acpica.sys fs/fs.sys drivers/drivers.sys

.PHONY: all
all: compile

.PHONY: compile
compile:
	nasm -f bin -o boot/ap_boot.o boot/ap_boot.S
	touch ./boot/boot.S
	
	$(CD) boot;$(MAKE) 		#ブート時処理をコンパイル
	$(CD) internal;$(MAKE)	#内部処理をコンパイル
	$(CD) kernel;$(MAKE)	#カーネル本体をコンパイル
	$(CD) acpica;$(MAKE)	#ACPICA
	$(CD) fs;$(MAKE)		#ファイルシステム関連
	$(CD) drivers;$(MAKE)	#ドライバ


.PHONY: install
install:
	
	$(LD) -nodefaultlibs -Map kernel.map -T boot/bootlinker.ld -o kernel.sys $(OBJECTS)
	$(CP) -f ./kernel.sys /cygdrive/g/KERNEL.SYS


.PHONY: clean
clean:
	$(CD) boot;$(MAKE) clean
	$(CD) internal;$(MAKE) clean
	$(CD) kernel;$(MAKE) clean
	$(CD) acpica;$(MAKE) clean
	$(CD) fs;$(MAKE) clean
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
	