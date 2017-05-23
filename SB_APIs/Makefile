CROSS	= arm-linux-gnueabihf-
CC		= $(CROSS)gcc 
STRIP	= $(CROSS)strip
AR		= $(CROSS)ar
LD		= $(CROSS)ld
RANLIB  =$(CROSS)ranlib

LIBS	= -D__PIC__

IFLAGS = -I./SB_APIs/include

all: libsb.a

libsb.a: SB_Network.o SB_System.o
	$(AR) rc $@ $^
	$(RANLIB) $@

clean: 
	rm -f SB_Network.o SB_System.o libsb.a
