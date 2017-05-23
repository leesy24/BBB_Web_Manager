CROSS	= arm-linux-gnueabihf-
CC		= $(CROSS)gcc 
STRIP	= $(CROSS)strip
AR		= $(CROSS)ar
LD		= $(CROSS)ld
RANLIB  =$(CROSS)ranlib

LIBS	= -D__PIC__
EXEC 	= setagent.cgi getagent.cgi

IFLAGS = -I../SB_APIs/include -I../cgic205

all: ../SB_APIs/libsb.a ../cgic205/libcgic.a $(EXEC)

getagent.cgi: getagent.c iDisplayHtml.o agent.o ../SB_APIs/libsb.a ../cgic205/libcgic.a
	$(CC) $(CFLAGS) $^ $(LDFLAGS) $(IFLAGS) -o getagent.cgi ${LIBS}
	$(STRIP) getagent.cgi

setagent.cgi: setagent.c iDisplayHtml.o agent.o ../SB_APIs/libsb.a ../cgic205/libcgic.a
	$(CC) $(CFLAGS) $^ $(LDFLAGS) $(IFLAGS) -o setagent.cgi ${LIBS}
	$(STRIP) setagent.cgi


clean: 
	make -C ../SB_APIs $@
	make -C ./cgic205 $@
	rm -f *.o *.cgi
