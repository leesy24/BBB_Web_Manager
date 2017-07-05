CROSS	= arm-linux-gnueabihf-
#LDFLAGS 	+= -L/opt/lemonix/cdt/lib -L/opt/lemonix/cdt/bin
#IFLAGS += -I/opt/lemonix/cdt/include  -I./include
IFLAGS = -I../SB_APIs/include
CFLAGS =	-O2 -g  -Wall $(IFLAGS)
#DEST		= ../../ramdisk/root/sbin	
#DEST_ETC	= ../../ramdisk/root/etc

CC		=	$(CROSS)gcc 
STRIP	=	$(CROSS)strip
AR		= 	$(CROSS)ar

#TARGET		=	eddy					pinetd						def						\
#				ddns_agent				upgrade						upgradetftp				\
#				wifi					test_nand					test_mmc				\
#				test_rtc				test_sio					test_adc				\
#				test_gpio_led			test_keypad 				test_lcd				\
#				test_dio				test_gpio_pin				test_bluetooth			\
#				test_spi_eeprom			test_tcp_server				test_tcp_client			\
#				test_serial				test_read_config			test_serial_to_lan-2	\
#				test_serial_to_lan-1	test_udp_server				test_udp_client			\
#				testdk					nand_erase

TARGET		=	eddy					pinetd						def						\
				ddns_agent				upgrade						upgradetftp				\
				wifi					test_nand					test_mmc				\
				test_rtc				test_sio					test_adc				\
				test_gpio_led			test_keypad 				\
				test_dio				test_gpio_pin				test_bluetooth			\
				test_spi_eeprom			\
				test_serial				test_read_config			\
				testdk					nand_erase					wan						\
				serial					serial_monitor				serial_get_rxtx
TARGET_C = $(addsuffix .c, $(TARGET))
TARGET_BIN = $(addprefix bin/, $(TARGET))
TARGET_OBJ = $(addsuffix .o, $(TARGET))

#LIBS		=   -lrt SB_APIs/SB_APIs.a
LIBS		=   -lrt ../SB_APIs/libsb.a

.PHONY: all clean scp*

all: $(TARGET_BIN)

bin/%: %.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@  $<  $(LIBS) 
	$(STRIP) $@
	chmod 755 $@

clean: 
	rm -f $(TARGET_BIN) $(TARGET_OBJ)
	@echo "TARGET_OBJ=" $(TARGET_OBJ)

#release:
#	cp  -f $(TARGET) $(DEST)
#	cp  -f sb_default_config $(DEST_ETC)

scp19:
	-scp $(TARGET_BIN) root@192.168.10.19:/sbin/
	
scp3:
	-scp $(TARGET_BIN) root@192.168.10.3:/sbin/

