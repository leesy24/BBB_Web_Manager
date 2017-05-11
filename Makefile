CROSS	= /opt/lemonix/cdt/bin/arm-linux-
CC		= $(CROSS)gcc 
STRIP	= $(CROSS)strip
AR		= $(CROSS)ar
LD		= $(CROSS)ld
RANLIB  =$(CROSS)ranlib

#LIBS	=-L./ -lcgic
LIBS	= -D__PIC__  -L./ -lcgic
#EXEC 	= getagent setagent
EXEC 	= setagent getagent

#LDFLAGS = -Wl,-elf2flt -Os
#DEFS =		 -DHAVE__PROGNAME=1 -DHAVE_FCNTL_H=1 -DHAVE_GRP_H=1 -DHAVE_MEMORY_H=1 -DHAVE_PATHS_H=1 -DHAVE_POLL_H=1 -DHAVE_SYS_POLL_H=1 -DTIME_WITH_SYS_TIME=1 -DHAVE_DIRENT_H=1 -DHAVE_LIBCRYPT=1 -DHAVE_STRERROR=1 -DHAVE_WAITPID=1 -DHAVE_VSNPRINTF=1 -DHAVE_DAEMON=1 -DHAVE_SETSID=1 -DHAVE_GETADDRINFO=1 -DHAVE_GETNAMEINFO=1 -DHAVE_GAI_STRERROR=1 -DHAVE_SIGSET=1 -DHAVE_ATOLL=1 -DHAVE_UNISTD_H=1 -DHAVE_GETPAGESIZE=1 -DHAVE_SELECT=1 -DHAVE_POLL=1 -DHAVE_TM_GMTOFF=1 -DHAVE_INT64T=1 -DHAVE_SOCKLENT=1 
#INCLS =		-I.
#CFLAGS =	-DEMBED -D__UCLINUX__ -Os
#export FLTFLAGS = -z -s 4096
IFLAGS = -I./SB_APIs/include

all: libcgic.a $(EXEC)

libcgic.a: ./cgic205/cgic.o ./cgic205/cgic.h
	rm -f libcgic.a
	$(AR) rc libcgic.a ./cgic205/cgic.o
	$(RANLIB) libcgic.a

#getagent: getagent.c iDisplayHtml.o agent.o ./SB_APIs/SB_APIs.a libcgic.a
getagent: getagent.c iDisplayHtml.o agent.o libcgic.a
	$(CC) $(CFLAGS) $^ $(LDFLAGS) $(IFLAGS) -o getagent.cgi ${LIBS}
	$(STRIP) getagent.cgi

#setagent: setagent.c iDisplayHtml.o agent.o ./SB_APIs/SB_APIs.a libcgic.a 
setagent: setagent.c iDisplayHtml.o agent.o libcgic.a 
	$(CC) $(CFLAGS) $^ $(LDFLAGS) $(IFLAGS) -o setagent.cgi ${LIBS}
	$(STRIP) setagent.cgi


clean: 
	rm -f *.o *.cgi *.cgi.elf *.a
