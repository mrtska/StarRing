export LANG=C

CD		:= cd
MAKE	:= make


.PHONY: all
all: compile

.PHONY: compile
compile:
	$(CD) StarRing;$(MAKE)

.PHONY: install
install:
	$(CD) StarRing;$(MAKE) install

.PHONY: clean
clean:
	$(CD) StarRing;$(MAKE) clean