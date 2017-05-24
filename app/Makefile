CROSS	= arm-linux-gnueabihf-
#LDFLAGS 	+= -L/opt/lemonix/cdt/lib -L/opt/lemonix/cdt/bin
#IFLAGS += -I/opt/lemonix/cdt/include  -I./include
IFLAGS = -I../SB_APIs/include
#CFLAGS =	-O2 -g  -Wall -Wno-nonnull -Wunused-label $(IFLAGS)
CFLAGS =	-O2 -g  -Wall -Wno-unused-label -Wno-unused-variable -Wno-unused-but-set-variable $(IFLAGS)
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
				testdk					nand_erase
TARGET_C = $(addsuffix .c, $(TARGET))
TARGET_BIN = $(addprefix bin/, $(TARGET))
TARGET_OBJ = $(addsuffix .o, $(TARGET))

#LIBS		=   -lrt SB_APIs/SB_APIs.a
LIBS		=   -lrt ../SB_APIs/libsb.a

.PHONY: all clean

all: $(TARGET_BIN)

$(TARGET_BIN): $(TARGET_OBJ) 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@  $<  $(LIBS) 
	$(STRIP) $@

clean: 
	rm -f *.o $(TARGET_BIN) $(TARGET_OBJ)

#release:
#	cp  -f $(TARGET) $(DEST)
#	cp  -f sb_default_config $(DEST_ETC)

