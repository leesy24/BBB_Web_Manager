CROSS	= arm-linux-gnueabihf-
CC		= $(CROSS)gcc 
STRIP	= $(CROSS)strip
AR		= $(CROSS)ar
LD		= $(CROSS)ld
RANLIB  =$(CROSS)ranlib

LIBS	= -D__PIC__
EXEC 	= setagent.cgi getagent.cgi

CFLAGS += -I../SB_APIs/include -I../cgic205

all: ../SB_APIs/libsb.a ../cgic205/libcgic.a $(EXEC)

getagent.cgi: getagent.c iDisplayHtml.o agent.o ../SB_APIs/libsb.a ../cgic205/libcgic.a
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o getagent.cgi ${LIBS}
	$(STRIP) getagent.cgi

setagent.cgi: setagent.c iDisplayHtml.o agent.o ../SB_APIs/libsb.a ../cgic205/libcgic.a
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o setagent.cgi ${LIBS}
	$(STRIP) setagent.cgi

../cgic205/libcgic.a:
	make -C ../cgic205

../SB_APIs/libsb.a:
	make -C ../SB_APIs

clean: 
	make -C ../cgic205 $@
	rm -f *.o *.cgi
