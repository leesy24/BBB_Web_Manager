CROSS=arm-linux-gnueabihf-
CFLAGS=-g -Wall
CC=$(CROSS)gcc
AR=$(CROSS)ar
RANLIB=$(CROSS)ranlib
LIBS=-L./ -lcgic

all: libcgic.a cgictest.cgi capture

install: libcgic.a
	cp libcgic.a /usr/local/lib
	cp cgic.h /usr/local/include
	@echo libcgic.a is in /usr/local/lib. cgic.h is in /usr/local/include.

libcgic.a: cgic.o cgic.h
	rm -f libcgic.a
	$(AR) rc libcgic.a cgic.o
	$(RANLIB) libcgic.a

#mingw32 and cygwin users: replace .cgi with .exe

cgictest.cgi: cgictest.o libcgic.a
	$(CC) cgictest.o -o $@ ${LIBS}

capture: capture.o libcgic.a
	$(CC) cgictest.o -o $@ ${LIBS}

clean:
	rm -f *.o *.a cgictest.cgi capture

