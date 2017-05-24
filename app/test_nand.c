#include "sb_include.h"
#include "sb_define.h"
#include "sb_shared.h"
#include "sb_config.h"
#include "sb_extern.h"
#include "sb_ioctl.h"

#include <sys/mount.h>

// reference to Eddy_APPs/pinet.c


#define DEST_FILE		"/tmp/nand/test.txt"

//=============================================================================
int main (int argc, char *argv[])
{
int fd, i, ret;
char buf [] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+=-?;:',.~`";

	if (access("/dev/mtdblock6", F_OK) != 0)
		{
		system ("insmod  /etc/drivers/at91_nand.ko");		// /dev/mtdblock6,   mount -t yffs2 /dev/mtdblock6  /tmp/nand
		mkdir ("/tmp/nand", 0x777);
		sleep (1);		
		mount ("/dev/mtdblock6", "/tmp/nand", "yaffs2", 16, "");
		chmod ("/tmp/nand", S_IWUSR|S_IRUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
		} 
		
   	fd = open(DEST_FILE, O_RDWR|O_CREAT, 0644);
	if (fd <= 0) 
		{
		printf ("NAND Flash --> Open/Create error !\n");
		return 0;
		}

	for (i=0; i<10; i++)
		{
		ret = write (fd, buf, strlen(buf));
		if (ret < 0) 
			{
			printf ("NAND Flash --> Write error !\n");
			return 0;
			}
		}
	close (fd);
	
	if (access(DEST_FILE, F_OK) == 0)
		{
		unlink (DEST_FILE);
		printf ("NAND Flash --> Test OK!\n");
		}
	else
		printf ("NAND Flash --> Test Failed!\n");	

	return 0;
}





