#include "sb_include.h"
#include "sb_define.h"
#include "sb_shared.h"
#include "sb_config.h"
#include "sb_extern.h"
#include "sb_ioctl.h"


#include <sys/mount.h>



// reference to Eddy_APPs/pinet.c


#define DEST_FILE		"/tmp/mmc/test.txt"

//=============================================================================
int main (int argc, char *argv[])
{
int fd, i, ret;
char buf [] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+=-?;:',.~`";

	if (access("/dev/sda1", F_OK) == 0)
		{
		mkdir ("/tmp/mmc", 0x777);
		mount ("/dev/sda1", "/tmp/mmc", "vfat", 16, "");
		chmod ("/tmp/mmc", S_IWUSR|S_IRUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
		}	
	else
		{
		printf ("SD/MMC not Found !\n");
		return 0;
		}
		
   	fd = open(DEST_FILE, O_RDWR|O_CREAT, 0644);
	if (fd <= 0) 
		{
		printf ("MMC Card --> Open/Create error !\n");
		return 0;
		}

	for (i=0; i<10; i++)
		{
		ret = write (fd, buf, strlen(buf));
		if (ret < 0) 
			{
			printf ("MMC Card --> Write error !\n");
			return 0;
			}
		}
	close (fd);
	
	if (access(DEST_FILE, F_OK) == 0)
		{
		unlink (DEST_FILE);
		printf ("MMC Flash --> Test OK!\n");
		}
	else
		printf ("MMC Flash --> Test Failed!\n");	

	return 0;
}





