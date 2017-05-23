CROSS	=	/opt/lemonix/cdt/bin/arm-linux-

CC =     $(CROSS)gcc
AR =     $(CROSS)ar
RANLIB = $(CROSS)ranlib

LIBS=-L./ -lcgic

all: libcgic.a 

libcgic.a: cgic.o cgic.h
	rm -f libcgic.a
	$(AR) rc libcgic.a cgic.o
	$(RANLIB) libcgic.a

clean:
	rm -f *.o *.a cgictest.cgi capture

