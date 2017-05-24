#include <stdio.h>
#include <string.h>
#include <linux/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h> 

#define EEPROM	"/sys/devices/platform/atmel_spi.1/spi1.0/eeprom"

//=============================================================================
int main (int argc, char *argv[])
{
int fd, i, ret, cnt, j;
char buf [] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+=-?;:',.~`";


	fd = open(EEPROM, O_RDWR);
	if (fd < 0)
		{
		system ("insmod  /etc/drivers/at25.ko");	
		sleep (1);	
		fd = open(EEPROM, O_RDWR);
		if (fd < 0)
			{
			printf("Error on opening the device file\n");
			return 0;
			}
		}

	lseek (fd, 0, 0);
	for (cnt=0,i=0; i<100; i++)
		{
		sprintf (buf, "%019d ", ++cnt);
		ret = write (fd, buf, 20);
		if (ret < 0) 
			{
			printf ("EEPROM, Write error return Code = %d!\n", ret);
			return 0;
			}
		}
	close (fd);
	
	printf ("EEPROM Write OK!\n");

	
	fd = open(EEPROM, O_RDWR);
	if (fd < 0)
		{
		printf("Error on opening the device file\n");
		return 0;
		}
	lseek (fd, 0, 0);
	for (cnt=0,i=0; i<100; i++)
		{
		ret = read (fd, buf, 20);
		if (ret < 0) 
			{
			printf ("EEPROM, read error return Code = %d!\n", ret);
			return 0;
			}
		else
			for (j=0; j<ret;j++) printf ("%c", buf[j]); printf ("\n");
		}
	close (fd);
		
	printf ("EEPROM Read OK!\n");
	return 0;
}


