#include "sb_include.h"
#include "sb_define.h"
#include "sb_shared.h"
#include "sb_config.h"
#include "sb_extern.h"
#include "sb_ioctl.h"
#include "sb_misc.h"
#include <sys/mount.h>

void default_gpio_set ();
void binary_print (char port, int t);
void binary_print_error (char port, int t, int value);
int test_port (int fd, char ID, int a, int b);
void S4M_gpio_led ();
void CPU_gpio_led ();
void S4M_gpio_pin ();
void CPU_gpio_pin ();
void keypad ();
void sd_card ();
void nand_flash ();
void rtc ();
void adc ();
void eeprom ();
void DIO ();
void reset ();

int Product_ID = EDDY_CPU;
//========================================================================
int main (int argc, char *argv[])
{	
	system ("kt pinetd com detect udhcpd tcp");

	Product_ID = SB_GetModel ();
	printf ("Product ID = %s\n", (Product_ID == EDDY_S4M) ? "Eddy_S4M" : "Eddy-CPU");	              

	if (argc == 2)
		{
		if (!strncmp(argv[1], "gpio", 4))
			{
			if (Product_ID == EDDY_S4M)
				S4M_gpio_pin ();
			else
				CPU_gpio_pin ();

			default_gpio_set ();
			return 0;
			}	
			
		if (!strncmp(argv[1], "led", 3))
			{
			if (Product_ID == EDDY_S4M)
				S4M_gpio_led ();				
			else	
				CPU_gpio_led ();
				
			default_gpio_set ();
			return 0;
			}	

		if (!strncmp(argv[1], "key", 3))
			{
			if (Product_ID != EDDY_S4M)
				{
				keypad ();
				default_gpio_set ();
				}
			return 0;
			}	

		if (!strncmp(argv[1], "mmc", 3))
			{
			sd_card ();
			default_gpio_set ();
			return 0;
			}	

		if (!strncmp(argv[1], "rtc", 3))
			{
			rtc ();
			return 0;
			}	

		if (!strncmp(argv[1], "adc", 3))
			{
			adc ();
			return 0;
			}	

		if (!strncmp(argv[1], "eeprom", 3))
			{
			if (Product_ID != EDDY_S4M)	eeprom ();
			return 0;
			}	

		if (!strncmp(argv[1], "dio", 3))
			{
			if (Product_ID != EDDY_S4M) DIO ();
			return 0;
			}	

		if (!strncmp(argv[1], "nand", 3))
			{
			if (Product_ID != EDDY_S4M) nand_flash ();
			return 0;
			}	
			
		if (!strncmp(argv[1], "reset", 3))
			{
			reset ();
			return 0;
			}				
 
 		if (!strncmp(argv[1], "all", 3))
			{
			if (Product_ID == EDDY_S4M)
				S4M_gpio_pin ();
			else
				CPU_gpio_pin ();
					
			default_gpio_set ();

			if (Product_ID == EDDY_S4M)
				S4M_gpio_led ();
			else
				CPU_gpio_led ();
			default_gpio_set ();

			if (Product_ID != EDDY_S4M)
				{
				keypad ();
				default_gpio_set ();
				}

			sd_card ();
			if (Product_ID != EDDY_S4M)	nand_flash ();
			rtc ();
			adc ();
			if (Product_ID != EDDY_S4M)	eeprom ();
			if (Product_ID != EDDY_S4M)	DIO ();
			reset ();
			return 0;
			}
		}    	
	if (Product_ID == EDDY_S4M)
   		printf ("Usage:  testdk  <all, gpio/led/mmc/rtc/adc/reset>\n");
   	else
   		printf ("Usage:  testdk  <all, gpio/led/key/mmc/nand/rtc/adc/eeprom/dio/reset>\n");



   	return 0;
}
//=============================================================================
void reset ()
{
int fd, i;
	
	printf ("\n\nRESET Key Test\n"); 
	printf ("Press Any key Continue...\n");
	getchar ();	
	printf ("Press Reset Key\n\n");	
		
	fd = open (SB_GPIO_DEVICE, O_RDWR);
	for (i=0; i<1000; i++)
		{
		SB_msleep (10);	
		if (ioctl (fd, RESET_READ) != SB_ENABLE)
			{
			printf ("Check OK.\n");
			break;
			}
		}
	close (fd);
}

#define EEPROM	"/sys/devices/platform/atmel_spi.1/spi1.0/eeprom"
//=============================================================================
void eeprom ()
{
int fd, i, ret, cnt, err=0;
char buf [40];


	printf ("\n\nEEPROM Test\n"); 
	printf ("Press Any key Continue...\n");
	getchar ();

	fd = open(EEPROM, O_RDWR);
	if (fd < 0)
		{
		system ("insmod  /etc/drivers/at25.ko");	
		sleep (1);	
		fd = open(EEPROM, O_RDWR);
		if (fd < 0)
			{
			printf("EEPROM Not Found\n");
			return;
			}
		}

	lseek (fd, 0, 0);
	for (cnt=0,i=0; i<100; i++)
		{
		sprintf (buf, "%019d ", ++cnt);
		ret = write (fd, buf, 20);
		if (ret < 0) 
			{
			printf ("EEPROM  Write Error\n");
			return;
			}
		}
	close (fd);
	
//	printf ("EEPROM Write OK!\n");
	
	fd = open(EEPROM, O_RDWR);
	if (fd < 0)
		{
		printf("EEPROM Open Error\n");
		return;
		}
	lseek (fd, 0, 0);
	for (cnt=0,i=0; i<100; i++)
		{
		memset (buf, 0x00, 30);
		ret = read (fd, buf, 20);
		if (ret < 0) 
			{
			printf ("EEPROM Read Error!\n");
			return;
			}
		else
			{
			if (atoi (buf) != ++cnt)  err = 1;
//			printf ("%d ", atoi (buf));	
			}
		}
	close (fd);
		

	if (err == 0) 
		printf ("EEPROM  Read/Write OK\n");
	else
		printf ("EEPROM Read/Write Error\n");
	return;
}


#define  DIO_DIR_0		"/sys/devices/platform/at91_i2c/i2c-0/0-0074/direction0"	
#define  DIO_DIR_1		"/sys/devices/platform/at91_i2c/i2c-0/0-0074/direction1"
#define  DIO_IN_0		"/sys/devices/platform/at91_i2c/i2c-0/0-0074/input0"
#define  DIO_IN_1		"/sys/devices/platform/at91_i2c/i2c-0/0-0074/input1"
#define  DIO_OUT_0		"/sys/devices/platform/at91_i2c/i2c-0/0-0074/output0"
#define  DIO_OUT_1		"/sys/devices/platform/at91_i2c/i2c-0/0-0074/output1"
int Check_DIO (char *DIR_PATH, char *OUT_PATH, char *IN_PATH);
//=============================================================================
void DIO ()
{
int ret;

	printf ("\n\nDIO Test\n"); 
	printf ("Connect DIO_loopback_Connetor to the Jumber J1(BUS GPIO)\n");
	printf ("Press Any key Continue...\n");
	getchar ();
	
	ret = Check_DIO (DIO_DIR_0, DIO_OUT_0, DIO_IN_0);
	if (ret == 0) 
		printf ("Test Direction-0 OK\n");
	else
		printf ("Test Direction-0 Error\n");

	ret = Check_DIO (DIO_DIR_1, DIO_OUT_1, DIO_IN_1);
	if (ret == 0) 
		printf ("Test Direction-1 OK\n");
	else
		printf ("Test Direction-1 Error\n");

}		
#define		DIRECTION_MODE_OUT_0246_IN_1357		0xaa	// 10101010(b)
#define		DIRECTION_MODE_OUT_1357_IN_0246		0x55	// 01010101(b)
#define		OUTPUT_HIGH_0246	0x55		
#define		OUTPUT_HIGH_1357	0xaa		
int Check_DIO (char *DIR_PATH, char *OUT_PATH, char *IN_PATH)
{
char buf[10];
unsigned char value_1, value_2, value_3, value_4;
int DIR, OUT, IN;

	DIR = open (DIR_PATH, O_WRONLY);
	if (DIR < 0)  {
		system ("insmod /etc/drivers/pca9539.ko");			// DIO and LCD 
		DIR = open (DIR_PATH, O_WRONLY);
		if (DIR < 0)  {
			printf ("DIO Direction Open error !\n"); 
			return -1;
		}
	}

	IN = open (IN_PATH, O_RDONLY);
	if (IN < 0)  {
		printf ("DIO Input Open error !\n"); 
		return -1;
	}

	OUT = open (OUT_PATH, O_WRONLY);
	if (OUT < 0)  {
		printf ("DIO Output Open error !\n"); 
		return -1;
	}

	// Test,  Set output(0,2,4,6) High --> get input(1,3,5,7) High 
	sprintf (buf, "%d\n", DIRECTION_MODE_OUT_0246_IN_1357); 
	write (DIR, buf, strlen(buf));
	sprintf (buf, "%d\n", OUTPUT_HIGH_0246);
	write (OUT, buf, strlen(buf));
	read (IN, buf, 10);	
	value_1 = atoi (buf);
	// Test,  Set output(0,2,4,6) Low --> get input(1,3,5,7) Low
	sprintf (buf, "%d\n", 0x00);
	write (OUT, buf, strlen(buf));
	read (IN, buf, 10);	
	value_2 = atoi (buf);


	close (IN);
	IN  = open (IN_PATH,  O_RDONLY);

	// Test,  Set output(1,3,5,7) High --> get input(0,2,4,6) High 
	sprintf (buf, "%d\n", DIRECTION_MODE_OUT_1357_IN_0246); 
	write (DIR, buf, strlen(buf));
	sprintf (buf, "%d\n", OUTPUT_HIGH_1357);
	write (OUT, buf, strlen(buf));
	read (IN, buf, 10);	
	value_3 = atoi (buf);
	// Test,  Set output(1,3,5,7) Low --> get input(0,2,4,6) Low
	sprintf (buf, "%d\n", 0x00);
	write (OUT, buf, strlen(buf));
	read (IN, buf, 10);	
	value_4 = atoi (buf);

	close (DIR);
	close (OUT);
	close (IN);


	if (value_1 == 0xff && value_2 == 0x00 && 
		value_3 == 0xff && value_4 == 0x00) 
		return 0;
	else
		return -1;
}







//========================================================================
void adc ()
{
struct at91_adc_value 
{
	int ch1_value;
	int ch2_value;
	int ch3_value;
	int ch4_value;
};
int fd;
struct at91_adc_value CHANNEL;
int channel, ret;
int cnt=0, i;

	printf ("\n\nADC Test\n"); 
	if (Product_ID != EDDY_S4M)
		printf ("Switch S5 to Off\n");
	else
		printf ("Now remove the Loopback_Connector\n");
	printf ("Press any key continue...\n");
	getchar ();

	fd = open("/dev/adc", O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0) 
		{
		system ("insmod  /etc/drivers/eddy_adc.ko");		// /dev/adc		
		sleep (2);
		fd = open("/dev/adc", O_RDWR | O_NOCTTY | O_NDELAY);
		if (fd < 0) 
			{
			printf("ADC open error,   Check DK v2.1 board (S5:Off), WEB(Peripheral Settings->ADC:Eddy)\n");
			return;
			}
		}

	// ADC 4 channed   x x x x x x x x (bits)
	//						   | | | |-- channel  1 (temperature sensor)
	//						   | | |---- channel  2 (illumination sensor)
	//						   | |------ channel  3 
	//						   |-------- channel  4
	// if you want to use all channel, you should set 0x0f
	// if you want to use channel0 ~channel3, you should set 0x7.


	channel = 0x03;								// select 1,2 channel 
	ioctl (fd, ADCSETCHANNEL, &channel);		// set channel
	ret = ioctl (fd, ADCGETCHANNEL);			// get channel status
//	printf("Channel Status = 0x%x \n",ret);

	for (i=0; i<10; i++)
		{
		SB_msleep (500);
		ioctl (fd, ADCGETVALUE, &CHANNEL);
		printf("[%02d]  (1:temperature)=%4d,  (2:illumination)=%4d,  3=%d,  4=%d\n", 
			++cnt,
			CHANNEL.ch1_value,
			CHANNEL.ch2_value,
			CHANNEL.ch3_value,
			CHANNEL.ch4_value);
		}

	close (fd);
	return;
}



//=============================================================================
void rtc ()
{
	printf ("\n\nRTC Test\n"); 
	printf ("Press any key continue...\n");
	getchar ();

	system ("test_rtc -s 2009 01 02 03 04 05");
	SB_msleep (200);
	system ("test_rtc -g ");
}


//=============================================================================
void nand_flash ()
{
int fd, i, ret;
char buf [] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+=-?;:',.~`";


	printf ("\n\nNand Flash Test\n"); 
	printf ("Press any key continue...\n");
	getchar ();


	if (access("/dev/mtdblock6", F_OK) != 0)
		{
		system ("insmod  /etc/drivers/at91_nand.ko");		// /dev/mtdblock6,   mount -t yffs2 /dev/mtdblock6  /tmp/nand
		mkdir ("/tmp/nand", 0x777);
		sleep (1);		
		mount ("/dev/mtdblock6", "/tmp/nand", "yaffs2", 16, "");
		chmod ("/tmp/nand", S_IWUSR|S_IRUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
		} 
/*
	if (access("/dev/mtdblock6", F_OK) != 0)
		{
		system ("insmod  /etc/drivers/at91_nand.ko");		// /dev/mtdblock6,   mount -t jffs2 /dev/mtdblock6  /tmp/nand
		mkdir ("/tmp/nand", 0x777);
		system ("mount -t yaffs2 /dev/mtdblock6  /tmp/nand");
		sleep (1);		
		if (access("/dev/mtdblock6", F_OK) != 0)
			{
			printf ("NAND Flash not found!\n");
			return;
			}
		}
*/
   	fd = open("/tmp/nand/test.txt", O_RDWR|O_CREAT, 0644);
	if (fd <= 0) 
		{
		printf ("NAND Flash  open error!\n");
		return;
		}

	for (i=0; i<10; i++)
		{
		ret = write (fd, buf, strlen(buf));
		if (ret < 0) 
			{
			printf ("NAND Flash write error!\n");
			return;
			}
		}
	close (fd);
	
	if (access("/tmp/nand/test.txt", F_OK) == 0)
		{
		unlink ("/tmp/nand/test.txt");
		printf ("NAND Flash  read/write OK\n");
		}
	else
		printf ("NAND Flash  read/write Error\n");	

	return;
}


//=============================================================================
void sd_card ()
{
int fd, i, ret;
char buf [] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+=-?;:',.~`";

	printf ("\n\nSD Card Test\n"); 
	printf ("Press any key continue...\n");
	getchar ();

	if (access("/dev/sda1", F_OK) == 0)
		{
		mkdir ("/tmp/mmc", 0x777);
		mount ("/dev/sda1", "/tmp/mmc", "vfat", 16, "");
		chmod ("/tmp/mmc", S_IWUSR|S_IRUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
		}	
	else
		{
		printf ("SD/MMC not Found !\n");
		return;
		}

   	fd = open("/tmp/mmc/test.txt", O_RDWR|O_CREAT, 0644);
	if (fd <= 0) 
		{
		printf ("SD card Open error !\n");
		return;
		}

	for (i=0; i<10; i++)
		{
		ret = write (fd, buf, strlen(buf));
		if (ret < 0) 
			{
			printf ("SD write error !\n");
			return;
			}
		}
	close (fd);
	
	if (access("/tmp/mmc/test.txt", F_OK) == 0)
		{
		unlink ("/tmp/mmc/test.txt");
		printf ("SD card  read/write OK.\n");
		}
	else
		printf ("SD card read/write error.!\n");	

	return;
}


//========================================================================
void CPU_gpio_led ()
{
int value[3];		// Port A,B,C 
int fd, no;
struct eddy_gpio_struct
{
    unsigned int value[3];
    unsigned int mode[3];
    unsigned int pullup[3];
    unsigned int enable[3];
};
struct eddy_gpio_struct gpio;


	printf ("\nGPIO LED Test\n"); 
	printf ("Now remove the Loopback_Connector\n");
	printf ("Set Switch S7 to On-On-On-On-On-Off\n");
	printf ("Press any key continue...\n");
	getchar ();
	
	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) return;
	
	gpio.enable[0]	= 0x00000030 & GPIO_MASK_PORT_A;	// Port A, Serial 2=2					Total 2  Gpio Ports
	gpio.enable[1]	= 0x3fc00ff0 & GPIO_MASK_PORT_B;	// Port B, Serial 0=8, 1=4, 2=2, 3=2	Total 16 Gpio Ports
	gpio.enable[2]	= 0x00000500 & GPIO_MASK_PORT_C;	// Port C, Serial 3=2					Total 2  Gpio Ports

	gpio.mode[0]	= 0x00000030;	// Output Mode
	gpio.mode[1]	= 0x3fc00ff0;	// Output Mode
	gpio.mode[2]	= 0x00000500;	// Output Mode

	gpio.value[0]	= 0x00000030;	// Value High
	gpio.value[1]	= 0x3fc00ff0;	// Value High
	gpio.value[2]	= 0x00000500;	// Value High

	gpio.pullup[0]	= 0x00000030;	// pullup High
	gpio.pullup[1]	= 0x3fc00ff0;	// pullup High
	gpio.pullup[2]	= 0x00000500;	// pullup High
	
	ioctl (fd, SETGPIOINIT, &gpio);	// Init GPIO	
	close (fd);
	

	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) return;
			
	for (no=0; no<5; no++)
		{
		value[0] = 0x00000030;
		value[1] = 0x3fc00ff0;
		value[2] = 0x00000500;
		ioctl (fd, SETGPIOVAL_LM, &value[0]);;	// Port A,B,C, 	value High

		SB_msleep (300);
		value[0] = 0x00000000;
		value[1] = 0x00000000;
		value[2] = 0x00000000;
		ioctl (fd, SETGPIOVAL_LM, &value[0]);	// Port A,B,C, 	value Low
		SB_msleep (300);
		}
	close (fd);
}
//========================================================================
void S4M_gpio_led ()
{
int value[3];		// Port A,B,C 
int fd, no;
struct eddy_gpio_struct
{
    unsigned int value[3];
    unsigned int mode[3];
    unsigned int pullup[3];
    unsigned int enable[3];
};
struct eddy_gpio_struct gpio;


	printf ("\nGPIO LED Test\n"); 
	printf ("Now remove the Loopback_Connector\n");
	printf ("Press any key continue...\n");
	getchar ();
	
	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) return;
	
	gpio.enable[0]	= S4M_GPIO_MASK_PORT_A;	
	gpio.enable[1]	= S4M_GPIO_MASK_PORT_B;	
	gpio.enable[2]	= S4M_GPIO_MASK_PORT_C | 0x00000003;		// Common (ADC and LED)

	gpio.mode[0]	= S4M_GPIO_MASK_PORT_A;						// Output Mode
	gpio.mode[1]	= S4M_GPIO_MASK_PORT_B;						// Output Mode
	gpio.mode[2]	= S4M_GPIO_MASK_PORT_C | 0x00000003;		// Output Mode  Common (ADC and LED)

	gpio.value[0]	= S4M_GPIO_MASK_PORT_A;						// Value High
	gpio.value[1]	= S4M_GPIO_MASK_PORT_B;						// Value High
	gpio.value[2]	= S4M_GPIO_MASK_PORT_C;						// Value High

	gpio.pullup[0]	= S4M_GPIO_MASK_PORT_A;						// pullup High
	gpio.pullup[1]	= S4M_GPIO_MASK_PORT_B;						// pullup High
	gpio.pullup[2]	= S4M_GPIO_MASK_PORT_C | 0x00000003;		// pullup High
	
	ioctl (fd, SETGPIOINIT, &gpio);	// Init GPIO	
	close (fd);
 

	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) return;
			
	for (no=0; no<5; no++)
		{
		value[0] = S4M_GPIO_MASK_PORT_A;
		value[1] = S4M_GPIO_MASK_PORT_B;
		value[2] = S4M_GPIO_MASK_PORT_C | 0x00000003;

		ioctl (fd, SETGPIOVAL_LM, &value[0]);	// Port A,B,C, 	value High

		SB_msleep (300);
		value[0] = 0x00000000;
		value[1] = 0x00000000;
		value[2] = 0x00000000;
		ioctl (fd, SETGPIOVAL_LM, &value[0]);	// Port A,B,C, 	value Low
		SB_msleep (300);
		}
	close (fd);
}

//================================================
void keypad ()
{
int MODE[3], VALUE[3], MD[3];
int fd, value, no=0;
unsigned char key;


	printf ("\n\nKeypad Test\n"); 
	printf ("close examination of the SW16 is pressed \n");
	
	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) return;
			
	ioctl (fd, GETGPIOMOD_LM, &MODE[0]);	// Get Mode Port A,B,C
	ioctl (fd, GETGPIOVAL_LM, &VALUE[0]);	// Get Value Port A,B,C

	MD[0] = MODE[0];
	MD[1] = MODE[1] | 0xc0300000;			// Set Port B Output Mode (B31, B30, B21, B20)
	MD[2] = MODE[2] & 0xff0fffff;			// Set Port C Input  Mode (C23, C22, C21, C20)
	ioctl (fd, SETGPIOMOD_LM, &MD[0]);
  
	while (1)
		{
		ioctl (fd, GETGPIOVAL_LB, &value);
		if (++no > 3) no=0; 
		value |= 0xc0300000;
		switch (no) 
			{
			case 3 : value &= 0x7fffffff; break;	// Mask Port B31
			case 2 : value &= 0xbfffffff; break;	// Mask Port B30
			case 1 : value &= 0xffdfffff; break;	// Mask Port B21
			case 0 : value &= 0xffefffff; break; 	// Mask Port B20
			}

		ioctl (fd, SETGPIOVAL_LB, &value);
		ioctl (fd, GETGPIOVAL_LC, &value);
		switch (value & 0x00f00000)
			{
			case 0x00700000 :	key=4; break;		// Mask Port C23
			case 0x00b00000 :	key=3; break;		// Mask Port C22
			case 0x00d00000 :	key=2; break;		// Mask Port C21
			case 0x00e00000 :	key=1; break;		// Mask Port C20
			default			:	continue;	
			}

		key += (no*4);
		printf ("Detected Key is SW-%02d\n", key);
		SB_msleep (200);

		if (key == 16) 
			{
			break;
			}
		}
	
	ioctl (fd, SETGPIOMOD_LM, &MODE[0]);	// Restore Mode Port A,B,C
	ioctl (fd, SETGPIOVAL_LM, &VALUE[0]);	// Restore Value Port A,B,C
	close (fd);
	return;
}

//==================================================
void CPU_gpio_pin ()
{
struct eddy_gpio_struct 
	{
    unsigned int value[3];
    unsigned int mode[3];
    unsigned int pullup[3];
    unsigned int enable[3];
	};
struct eddy_gpio_struct gpio;
int fd;

	printf ("\n\nGPIO Pin Test\n"); 
	printf ("Connect Loopback_Connector to the jumper J4(Port A&C)\n");
	printf ("Set Switch S7 to Off-Off-Off-Off-Off-ON, and  Swirch S5 to ON\n");
	printf ("Press any key continue...\n");
	getchar ();

	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) 
		{
		printf ("GPIO open error \n");
		return;
		}

	gpio.enable[0]	= 0x00400030 & GPIO_MASK_PORT_A;
	gpio.enable[1]	= 0xffffffff & GPIO_MASK_PORT_B;
	gpio.enable[2]	= 0x04ffd73f & GPIO_MASK_PORT_C;

	gpio.mode[0]	= 0x00000000;
	gpio.mode[1]	= 0x00000000;
	gpio.mode[2]	= 0x00000000;
	gpio.value[0]	= 0x00000000;
	gpio.value[1]	= 0x00000000;
	gpio.value[2]	= 0x00000000;	
	gpio.pullup[0]	= 0x00000000;	
	gpio.pullup[1]	= 0x00000000;	
	gpio.pullup[2]	= 0x00000000;	
	ioctl (fd, SETGPIOINIT, &gpio);	// Init GPIO	
	close (fd);

	gpio.enable[0]	= 0x00400000 & GPIO_MASK_PORT_A;	// Port A, Serial 2=2					Total 2  Gpio Ports
	gpio.enable[1]	= 0xc03f300f & GPIO_MASK_PORT_B;	// Port B, Serial 0=8, 1=4, 2=2, 3=2	Total 16 Gpio Ports
	gpio.enable[2]	= 0x04fe422f & GPIO_MASK_PORT_C;	// Port C, Serial 3=2					Total 2  Gpio Ports

//---------------------------------------------------------------------------------------------------------------------------------------
//bytes	    3								2								1								0							
//bits	    31	30	29	28	27	26	25	24	23	22	21	20	19	18	17	16	15	14	13	12	11	10	9	8	7	6	5	4	3	2	1	0
//bit		7	6	5	4	3	2	1	0	7	6	5	4	3	2	1	0	7	6	5	4	3	2	1	0	7	6	5	4	3	2	1	0
//---------------------------------------------------------------------------------------------------------------------------------------
//Port A	-	-	-	-	-	-	-	-	-	*	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	S2	S2	-	-	-	-
//Port B	Key Key	S1	S1	S0	S0	S0	S0	S0	S0	Key Key	*	*	*	*	DE	DE	*	*	S3	S3	S2	S2	S1	S1	S0	S0	EPM	EPM	EPM	EPM
//Port C	-	-	-	-	-	*	-	-	Key Key Key Key	*	*	NAD RST	LAN	NAD -	LAN	-   S3	*	S3	-	-	*   RDY	ADC	ADC	ADC	ADC
//---------------------------------------------------------------------------------------------------------------------------------------
//		S0 ~ S3	= Serial Port (Port 1 ~ 4)
//		DE		= Debug Port
//		RST		= Reset 
//		RDY		= Ready LED
//		ADC		= Analog Digital Converter 
//      LAN     = LAN Port
//		NAD		= Nand Flash
//		ER		= SPI(EEPROM)
//		Key		= Keypad
//		-		= Not Used
//		*		= GPIO & User Port
//---------------------------------------------------------------------------------------------------------------


	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) return;


	if (test_port (fd, 'A', 4, 5)) printf ("Port(A)( 4, 5) -> OK\n"); else printf ("Port(A)( 4, 5) -> Error\n");
	if (test_port (fd, 'A', 4,22)) printf ("Port(A)( 4,22) -> OK\n"); else printf ("Port(A)( 4,22) -> Error\n");	
	if (test_port (fd, 'A', 5,22)) printf ("Port(A)( 5,22) -> OK\n"); else printf ("Port(A)( 5,22) -> Error\n");	
	if (test_port (fd, 'C', 0, 1)) printf ("Port(C)( 0, 1) -> OK\n"); else printf ("Port(C)( 0, 1) -> Error\n");
	if (test_port (fd, 'C', 2, 3)) printf ("Port(C)( 2, 3) -> OK\n"); else printf ("Port(C)( 2, 3) -> Error\n");
	if (test_port (fd, 'C', 4, 5)) printf ("Port(C)( 4, 5) -> OK\n"); else printf ("Port(C)( 4, 5) -> Error\n");
	if (test_port (fd, 'C', 8, 9)) printf ("Port(C)( 8, 9) -> OK\n"); else printf ("Port(C)( 8, 9) -> Error\n");
	if (test_port (fd, 'C',10,12)) printf ("Port(C)(10,12) -> OK\n"); else printf ("Port(C)(10,12) -> Error\n");
	if (test_port (fd, 'C',14,15)) printf ("Port(C)(14,15) -> OK\n"); else printf ("Port(C)(14,15) -> Error\n");
	if (test_port (fd, 'C',16,17)) printf ("Port(C)(16,17) -> OK\n"); else printf ("Port(C)(16,17) -> Error\n");
	if (test_port (fd, 'C',18,19)) printf ("Port(C)(18,19) -> OK\n"); else printf ("Port(C)(18,19) -> Error\n");
	if (test_port (fd, 'C',20,21)) printf ("Port(C)(20,21) -> OK\n"); else printf ("Port(C)(20,21) -> Error\n");
	if (test_port (fd, 'C',22,23)) printf ("Port(C)(22,23) -> OK\n"); else printf ("Port(C)(22,23) -> Error\n");
	if (test_port (fd, 'C',23,26)) printf ("Port(C)(23,26) -> OK\n"); else printf ("Port(C)(23,26) -> Error\n");
	if (test_port (fd, 'B', 0, 1)) printf ("Port(B)( 0, 1) -> OK\n"); else printf ("Port(B)( 0, 1) -> Error\n");
	if (test_port (fd, 'B', 2, 3)) printf ("Port(B)( 2, 3) -> OK\n"); else printf ("Port(B)( 2, 3) -> Error\n");
	if (test_port (fd, 'B', 4, 5)) printf ("Port(B)( 4, 5) -> OK\n"); else printf ("Port(B)( 4, 5) -> Error\n");
	if (test_port (fd, 'B', 6, 7)) printf ("Port(B)( 6, 7) -> OK\n"); else printf ("Port(B)( 6, 7) -> Error\n");
	if (test_port (fd, 'B', 8, 9)) printf ("Port(B)( 8, 9) -> OK\n"); else printf ("Port(B)( 8, 9) -> Error\n");
	if (test_port (fd, 'B',10,11)) printf ("Port(B)(10,11) -> OK\n"); else printf ("Port(B)(10,11) -> Error\n");
	if (test_port (fd, 'B',12,13)) printf ("Port(B)(12,13) -> OK\n"); else printf ("Port(B)(12,13) -> Error\n");
	if (test_port (fd, 'B',14,15)) printf ("Port(B)(14,15) -> OK\n"); else printf ("Port(B)(14,15) -> Error\n");
	if (test_port (fd, 'B',16,17)) printf ("Port(B)(16,17) -> OK\n"); else printf ("Port(B)(16,17) -> Error\n");
	if (test_port (fd, 'B',18,19)) printf ("Port(B)(18,19) -> OK\n"); else printf ("Port(B)(18,19) -> Error\n");
	if (test_port (fd, 'B',20,21)) printf ("Port(B)(20,21) -> OK\n"); else printf ("Port(B)(20,21) -> Error\n");
	if (test_port (fd, 'B',22,23)) printf ("Port(B)(22,23) -> OK\n"); else printf ("Port(B)(22,23) -> Error\n");
	if (test_port (fd, 'B',24,25)) printf ("Port(B)(24,25) -> OK\n"); else printf ("Port(B)(24,25) -> Error\n");
	if (test_port (fd, 'B',26,27)) printf ("Port(B)(26,27) -> OK\n"); else printf ("Port(B)(26,27) -> Error\n");
	if (test_port (fd, 'B',28,29)) printf ("Port(B)(28,29) -> OK\n"); else printf ("Port(B)(28,29) -> Error\n");
	if (test_port (fd, 'B',30,31)) printf ("Port(B)(30,31) -> OK\n"); else printf ("Port(B)(30,31) -> Error\n");

		
	close (fd);
	
	return;
}
//==================================================
void S4M_gpio_pin ()
{
	
struct eddy_gpio_struct 
	{
    unsigned int value[3];
    unsigned int mode[3];
    unsigned int pullup[3];
    unsigned int enable[3];
	};
struct eddy_gpio_struct gpio;
int fd;

	printf ("\n\nGPIO Pin Test\n"); 
	printf ("Connect Loopback_Connector to the Jumper J1(BUS GPIO)\n");
	printf ("Press any key continue...\n");
	getchar ();

	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) 
		{
		printf ("GPIO open error \n");
		return;
		}

	gpio.enable[0]	= S4M_GPIO_MASK_PORT_A;
	gpio.enable[1]	= S4M_GPIO_MASK_PORT_B;
	gpio.enable[2]	= S4M_GPIO_MASK_PORT_C;

	gpio.mode[0]	= 0x00000000;
	gpio.mode[1]	= 0x00000000;
	gpio.mode[2]	= 0x00000000;
	gpio.value[0]	= 0x00000000;
	gpio.value[1]	= 0x00000000;
	gpio.value[2]	= 0x00000000;	
	gpio.pullup[0]	= 0x00000000;	
	gpio.pullup[1]	= 0x00000000;	
	gpio.pullup[2]	= 0x00000000;	
	ioctl (fd, SETGPIOINIT, &gpio);	// Init GPIO	
	close (fd);

	gpio.enable[0]	= 0x00400000 & S4M_GPIO_MASK_PORT_A;	
	gpio.enable[1]	= 0xc03f300f & S4M_GPIO_MASK_PORT_B;	
	gpio.enable[2]	= 0x04fe422f & S4M_GPIO_MASK_PORT_C;	

//---------------------------------------------------------------------------------------------------------------------------------------
//bytes	    3								2								1								0							
//bits	    31	30	29	28	27	26	25	24	23	22	21	20	19	18	17	16	15	14	13	12	11	10	9	8	7	6	5	4	3	2	1	0
//bit		7	6	5	4	3	2	1	0	7	6	5	4	3	2	1	0	7	6	5	4	3	2	1	0	7	6	5	4	3	2	1	0
//---------------------------------------------------------------------------------------------------------------------------------------
//Port A	-   *   -   -   -   -   -   -   -   *   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   *   -   -   -   -   -
//Port B	*   *   -   -   -   -   -   -   -   -   *   *   *   *   *   *   -   -   *   *   -   -   -   -   -   -   -   -   *   *   *   *
//Port C	-   -   -   -   -   -   *   *   -   -   -   *   *   *   *   -   *   *   *   *   -   *   *   -   -   -   *   -   *   * ADC ADC   
//---------------------------------------------------------------------------------------------------------------------------------------
//		-		= Reserved			
//		*		= GPIO 
// 		If ADC disable --> GPIO(PC00, PC01) enable
// 		If ADC enable  --> GPIO(PC00, PC01) disable
//---------------------------------------------------------------------------------------------------------------
	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) return;

	if (test_port (fd, 'A', 5,22)) printf ("Port(A)( 5,22)) -> OK\n"); else printf ("Port(A)( 5,22)) -> Error\n");
	if (test_port (fd, 'A', 5,30)) printf ("Port(A)( 5,30)) -> OK\n"); else printf ("Port(A)( 5,30)) -> Error\n");	
	if (test_port (fd, 'A',22,30)) printf ("Port(A)(22,30)) -> OK\n"); else printf ("Port(A)(22,30)) -> Error\n");	
	if (test_port (fd, 'B', 0, 1)) printf ("Port(B)( 0, 1)) -> OK\n"); else printf ("Port(B)( 0, 1)) -> Error\n");
	if (test_port (fd, 'B', 2, 3)) printf ("Port(B)( 2, 3)) -> OK\n"); else printf ("Port(B)( 2, 3)) -> Error\n");
	if (test_port (fd, 'B',12,13)) printf ("Port(B)(12,13)) -> OK\n"); else printf ("Port(B)(12,13)) -> Error\n");
	if (test_port (fd, 'B',16,17)) printf ("Port(B)(16,17)) -> OK\n"); else printf ("Port(B)(16,17)) -> Error\n");
	if (test_port (fd, 'B',18,19)) printf ("Port(B)(18,19)) -> OK\n"); else printf ("Port(B)(18,19)) -> Error\n");
	if (test_port (fd, 'B',20,21)) printf ("Port(B)(20,21)) -> OK\n"); else printf ("Port(B)(20,21)) -> Error\n");
	if (test_port (fd, 'B',30,31)) printf ("Port(B)(30,31)) -> OK\n"); else printf ("Port(B)(30,31)) -> Error\n");
                                                                                                   
//	if (test_port (fd, 'C', 0, 1)) printf ("Port(C)( 0, 1)) -> OK\n"); else printf ("Port(C)( 0, 1)) -> Error\n");
	if (test_port (fd, 'C', 2, 3)) printf ("Port(C)( 2, 3)) -> OK\n"); else printf ("Port(C)( 2, 3)) -> Error\n");
	if (test_port (fd, 'C', 5, 9)) printf ("Port(C)( 5, 9)) -> OK\n"); else printf ("Port(C)( 5, 9)) -> Error\n");
	if (test_port (fd, 'C',10,12)) printf ("Port(C)(10,12)) -> OK\n"); else printf ("Port(C)(10,12)) -> Error\n");
	if (test_port (fd, 'C',13,14)) printf ("Port(C)(13,14)) -> OK\n"); else printf ("Port(C)(13,14)) -> Error\n");
	if (test_port (fd, 'C',15,17)) printf ("Port(C)(15,17)) -> OK\n"); else printf ("Port(C)(15,17)) -> Error\n");
	if (test_port (fd, 'C',18,19)) printf ("Port(C)(18,19)) -> OK\n"); else printf ("Port(C)(18,19)) -> Error\n");
	if (test_port (fd, 'C',20,24)) printf ("Port(C)(20,24)) -> OK\n"); else printf ("Port(C)(20,24)) -> Error\n");
	if (test_port (fd, 'C',20,25)) printf ("Port(C)(20,25)) -> OK\n"); else printf ("Port(C)(20,25)) -> Error\n");
	if (test_port (fd, 'C',24,25)) printf ("Port(C)(24,25)) -> OK\n"); else printf ("Port(C)(24,25)) -> Error\n");
		
	close (fd);
	return;
}
//==================================================================================
int test_port (int fd, char ID, int a, int b)
{
unsigned int SetMode=0, SetValue=0, GetValue=0;
unsigned int mode,value, val, set;

	switch (ID)	{
		case 'A' : 
			SetMode  = SETGPIOMOD_LA;
			SetValue = SETGPIOVAL_LA;
			GetValue = GETGPIOVAL_LA;
			break;
		case 'B' : 
			SetMode  = SETGPIOMOD_LB;
			SetValue = SETGPIOVAL_LB;
			GetValue = GETGPIOVAL_LB;
			break;
		case 'C' : 
			SetMode  = SETGPIOMOD_LC;
			SetValue = SETGPIOVAL_LC;
			GetValue = GETGPIOVAL_LC;
			break;
		}

	mode = set = 0x00000001 << a;
	val = (0x00000001 << b) | set;
	ioctl (fd, SetMode,  &mode);
	ioctl (fd, SetValue, &set);
	ioctl (fd, GetValue, &value);
//	printf ("mode=%x, set=%x, value=%x, val=%x, val&value=%x, \n", mode, set, value, val, value&val);
	if ((value&val) != val) return 0;

	set = 0x00000000;
	ioctl (fd, SetValue, &set);
	ioctl (fd, GetValue, &value);
//	printf ("mode=%x, set=%x, value=%x, val=%x, val&value=%x, \n", mode, set, value, val, value&val);
	if ((value&val) != 0x00000000) return 0;




	mode = set = 0x00000001 << b;
	val = (0x00000001 << a) | set;
	ioctl (fd, SetMode,  &mode);
	ioctl (fd, SetValue, &set);
	ioctl (fd, GetValue, &value);
//	printf ("mode=%x, set=%x, value=%x, val=%x, val&value=%x, \n", mode, set, value, val, value&val);
	if ((value&val) != val) return 0;
		
	set = 0x00000000;
	ioctl (fd, SetValue, &set);
	ioctl (fd, GetValue, &value);
//	printf ("mode=%x, set=%x, value=%x, val=%x, val&value=%x, \n", mode, set, value, val, value&val);	
	if ((value&set) != 0x00000000) return 0;
	return 1;
}

//==================================================================================
void binary_print (char port, int t)
{
int i, ttm;

	for (i=31; i>=0; i--)
		{
		ttm = t >> i;
		ttm &= 0x00000001;
		
		switch (port)	{
			case 'A' :
				if (i==22 || i==5 || i==4) printf ("%d", ttm);	else printf ("-");	break;
			case 'C' :
				if (i==31 || i==30 || i==29 || i==28 || i==27 || i==25 || i==24 || i==13 || i==11 || i==7 || i==6)
					printf ("-"); else printf ("%d", ttm);	break;	
			case 'B' :
				printf ("%d", ttm);	break;	
			}	
		
		if (i==28 || i==24 || i==20 || i==16 || i==12 || i==8 || i==4) printf (" ");
		}		
}			
//==================================================================================
void binary_print_error (char port, int t, int value)
{
int i, ttm, vvv;

	for (i=31; i>=0; i--)
		{
		ttm = t >> i;
		ttm &= 0x00000001;

		vvv = value >> i;
		vvv &= 0x00000001;
		
		switch (port)	
			{
			case 'A' :
				if (i==22 || i==5 || i==4) 
					if (ttm != vvv) 
						printf ("X");	else printf ("O");
				else 
					printf ("-");	break;
											
			case 'C' :
				if (i==31 || i==30 || i==29 || i==28 || i==27 || i==25 || i==24 || i==13 || i==11 || i==7 || i==6)
					if (ttm != vvv) 
						printf ("X");	else printf ("O");
				else 
					printf ("-");	break;

			case 'B' :
					if (ttm != vvv) 
						printf ("X");	else printf ("O");
			}	
		
		if (i==28 || i==24 || i==20 || i==16 || i==12 || i==8 || i==4) printf (" ");
		}
}
//====================================================================================
void default_gpio_set ()
{
struct eddy_gpio_struct
{
    unsigned int value[3];
    unsigned int mode[3];
    unsigned int pullup[3];
    unsigned int enable[3];
};
struct eddy_gpio_struct gpio;
int fd;

	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) 
		{
		printf ("GPIO open error \n");
		return ;
		}

	if (Product_ID == EDDY_S4M)
		{	
		gpio.enable[0]	= 0x00400000 & S4M_GPIO_MASK_PORT_A;
		gpio.enable[1]	= 0xc03f300f & S4M_GPIO_MASK_PORT_B;
		gpio.enable[2]	= 0x04fe422f & S4M_GPIO_MASK_PORT_C;
		}
	else			
		{	
		gpio.enable[0]	= 0x00400000 & GPIO_MASK_PORT_A;	// Port A, Serial 2=2					Total 2  Gpio Ports
		gpio.enable[1]	= 0xc03f300f & GPIO_MASK_PORT_B;	// Port B, Serial 0=8, 1=4, 2=2, 3=2	Total 16 Gpio Ports
		gpio.enable[2]	= 0x04fe422f & GPIO_MASK_PORT_C;	// Port C, Serial 3=2					Total 2  Gpio Ports
		}
	gpio.mode[0]	= 0x00000000;	// Output Mode
	gpio.mode[1]	= 0x00000000;	// Output Mode
	gpio.mode[2]	= 0x00000000;	// Output Mode

	gpio.value[0]	= 0x00000000;	// Value High
	gpio.value[1]	= 0x00000000;	// Value High
	gpio.value[2]	= 0x00000000;	// Value High

	gpio.pullup[0]	= 0x00000000;	// pullup High
	gpio.pullup[1]	= 0x00000000;	// pullup High
	gpio.pullup[2]	= 0x00000000;	// pullup High
	ioctl (fd, SETGPIOINIT, &gpio);	// Init GPIO	
	close (fd);
}
//==========================================================================
void SB_msleep (int msec) 
{
struct timespec interval;

	interval.tv_sec  = msec / 1000;
	interval.tv_nsec = 1000000 * (msec % 1000);
	nanosleep (&interval, NULL);
}
