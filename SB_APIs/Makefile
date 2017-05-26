CROSS	= arm-linux-gnueabihf-
CC		= $(CROSS)gcc 
STRIP	= $(CROSS)strip
AR		= $(CROSS)ar
LD		= $(CROSS)ld
RANLIB  =$(CROSS)ranlib

LIBS	= -D__PIC__

CFLAGS += -I./include

SRC = SB_Network.c SB_System.c SB_Serial.c SB_Environment.c
OBJ = $(subst .c,.o,$(SRC))

.PHONY: all clean

all: libsb.a

libsb.a: $(OBJ)
	$(AR) rc $@ $^
	$(RANLIB) $@

clean: 
	rm -f *.o *.a

