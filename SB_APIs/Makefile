CROSS	= arm-linux-gnueabihf-
CC		= $(CROSS)gcc 
STRIP	= $(CROSS)strip
AR		= $(CROSS)ar
LD		= $(CROSS)ld
RANLIB  =$(CROSS)ranlib

LIBS	= -D__PIC__

CFLAGS += -I./include

all: libsb.a

libsb.a: SB_Network.o SB_System.o SB_Serial.o
	$(AR) rc $@ $^
	$(RANLIB) $@

clean: 
	rm -f *.o *.a
