export LANG=C

#螳夂ｾｩ鄒､
HOME_DIR	= "$(CURDIR)"

CC		:= x86_64-pc-linux-gnu-g++ -pipe -std=c++11
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


OBJECTS := boot/boot.sys kernel/kernel.sys #acpica/acpica.sys fs/fs.sys drivers/drivers.sys

.PHONY: all
all: compile

.PHONY: compile
compile:
#	nasm -f bin -o boot/ap_boot.o boot/ap_boot.S
#	touch ./boot/boot.S
	
	$(CD) boot;$(MAKE) 		#繝悶�ｼ繝域凾蜃ｦ逅�繧偵さ繝ｳ繝代う繝ｫ
#	$(CD) internal;$(MAKE)	#蜀�驛ｨ蜃ｦ逅�繧偵さ繝ｳ繝代う繝ｫ
	$(CD) kernel;$(MAKE)	#繧ｫ繝ｼ繝阪Ν譛ｬ菴薙ｒ繧ｳ繝ｳ繝代う繝ｫ
#	$(CD) acpica;$(MAKE)	#ACPICA
#	$(CD) fs;$(MAKE)		#繝輔ぃ繧､繝ｫ繧ｷ繧ｹ繝�繝�髢｢騾｣
#	$(CD) drivers;$(MAKE)	#繝峨Λ繧､繝�


.PHONY: install
install:
	
	$(LD) -nodefaultlibs -Map kernel.map -T boot/bootlinker.ld -o kernel.sys $(OBJECTS)
	$(CP) -f ./kernel.sys /cygdrive/g/KERNEL.SYS
	sync

.PHONY: clean
clean:
	$(CD) boot;$(MAKE) clean
#	$(CD) internal;$(MAKE) clean
	$(CD) kernel;$(MAKE) clean
#	$(CD) acpica;$(MAKE) clean
#	$(CD) fs;$(MAKE) clean
#	$(CD) drivers;$(MAKE) clean
	
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
	