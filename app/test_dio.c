#include "sb_include.h"
#include "sb_define.h"
#include "sb_shared.h"
#include "sb_config.h"
#include "sb_extern.h"
#include "sb_ioctl.h"


#define  DIO_DIR_0	"/sys/devices/platform/at91_i2c/i2c-0/0-0074/direction0"	
#define  DIO_DIR_1	"/sys/devices/platform/at91_i2c/i2c-0/0-0074/direction1"
#define  DIO_IN_0		"/sys/devices/platform/at91_i2c/i2c-0/0-0074/input0"
#define  DIO_IN_1		"/sys/devices/platform/at91_i2c/i2c-0/0-0074/input1"
#define  DIO_OUT_0		"/sys/devices/platform/at91_i2c/i2c-0/0-0074/output0"
#define  DIO_OUT_1		"/sys/devices/platform/at91_i2c/i2c-0/0-0074/output1"

int main (int argc, char *argv[]);
int Check_DIO (char *DIR_PATH, char *OUT_PATH, char *IN_PATH);
//=============================================================================
int main (int argc, char *argv[])
{
int ret;

	ret = Check_DIO (DIO_DIR_0, DIO_OUT_0, DIO_IN_0);
	if (ret == 0) 
		printf ("Test Direction-0 OK !\n");
	else
		printf ("Test Direction-0 Failed !\n");

	ret = Check_DIO (DIO_DIR_1, DIO_OUT_1, DIO_IN_1);
	if (ret == 0) 
		printf ("Test Direction-1 OK !\n");
	else
		printf ("Test Direction-1 Failed !\n");

	return 0;
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
