#include "agent.h"
#include "../../include/sb_include.h"
#include "../../src/include/sb_define.h"
#include "../../src/include/sb_extern.h"
#include "../../src/include/sb_config.h"
/////////////////////////////////////////
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/mtd/mtd.h>
#include <errno.h>

typedef int bool;
#define true 1
#define false 0

#define KB(x) ((x) / 1024)
#define PERCENTAGE(x,total) (((x) * 100) / (total))
#define BUFSIZE (10 * 1024)
#define FLAG_NONE		0x00
#define FLAG_VERBOSE	0x01
#define FLAG_HELP		0x02
#define FLAG_FILENAME	0x04
#define FLAG_DEVICE		0x08

static int dev_fd = -1,fil_fd = -1;

static void safe_read (int fd,const char *filename,void *buf,size_t count,bool verbose)
{
   ssize_t result;

   result = mcpanic_read (fd,buf,count);
   if (count != result)
   		Error("safe read error!\n");
}

ssize_t mcpanic_read(int fd, void *buf, size_t count)
{
	ssize_t n;
	do {
		n = read (fd, buf, count);
	} while (n<0 && errno==EINTR);
	return n;
}

int update (void)
{
	
// TSUM
	static char buff_head[500];
	static char tmp1[128];
	static char tmp2[128];
	static char *ptr;
	static int start_p;

   const char *progname;
   char filename[128], device[128];
   // const char *progname,*filename = NULL,*device = NULL;
   int i,flags = FLAG_NONE;
   size_t result,size,written;
   struct mtd_info_user mtd;
   struct erase_info_user erase;
   struct stat filestat;
   unsigned char src[BUFSIZE],dest[BUFSIZE];

   sprintf(filename, "/var/tmp/Image.bin");
   fil_fd = open (filename,O_RDONLY);
   if (fstat (fil_fd,&filestat) < 0)
		Error ("Upload Fail!!");
   
   // FIND MTD
    memset(buff_head,0,sizeof(buff_head));
    lseek(fil_fd,filestat.st_size-200,SEEK_SET);
	read(fil_fd,buff_head,sizeof(buff_head));
	for(i=0;i<200;i++)
	{
		if(buff_head[i]=='m' && buff_head[i+1]=='t' && buff_head[i+2]=='d')
			break;
	}
	if (i>=200)
	{
		unlink("/var/tmp/Image.bin");
		Error("File Format Failed!!");
	}
	sprintf(device,"/dev/mtd%c",buff_head[i+3]);

   // TSUM >>>>>>>>>>>>>>>.
	start_p = 0;
	lseek(fil_fd,0,SEEK_SET);
	read(fil_fd,buff_head,sizeof(buff_head));
	ptr = strstr(buff_head,"Content-Type:");
	if(ptr)
	{
        sscanf(&buff_head[ptr-buff_head],"%s\n%s\n",tmp1,tmp2);
        start_p=ptr-buff_head+strlen(tmp1)+strlen(tmp2)+5;
	}
   lseek(fil_fd,start_p,SEEK_SET); // TSUM
   filestat.st_size -= start_p;//TSUM
   dev_fd = open (device,O_SYNC | O_RDWR);
   if (ioctl (dev_fd,MEMGETINFO,&mtd) < 0)
		Error ("MTD open Error!");
   if (filestat.st_size > mtd.size)
		Error ("MTD size Error!");

   erase.start = 0;
   erase.length = filestat.st_size & ~(mtd.erasesize - 1);
   if (filestat.st_size % mtd.erasesize) 
	   erase.length += mtd.erasesize;
   if (flags & FLAG_VERBOSE)
	 {
		int blocks = erase.length / mtd.erasesize;
		erase.length = mtd.erasesize;
		for (i = 1; i <= blocks; i++)
		  {
			 if (ioctl (dev_fd,MEMERASE,&erase) < 0)
				  Error ("MTD erase error!");
			 erase.start += mtd.erasesize;
		  }
	 }
   else
	 {
		if (ioctl (dev_fd,MEMERASE,&erase) < 0)
			  Error ("MTD erase error!!");
		  
	 }

   sleep(1); // TSUM
   size = filestat.st_size;
//mcpanic BUFSIZE->1024
   i = 1024;
   written = 0;
   while (size)
	 {
//mcpanic BUFSIZE->1024 
		if (size < 1024) 
			i = size;
		safe_read (fil_fd,filename,src,i,flags & FLAG_VERBOSE);
		// write to device 
		result = write (dev_fd,src,i);
		if (i != result)
		  {
			 if (result < 0)
				  Error ("MTD write error!");
			   
			 Error ("MTD write size error!");
		  }
		written += i;
		size -= i;
	 }

   unlink("/var/tmp/boa-temp.XXXXXX");
   sleep(5);
   return 1;
}

////////////////////////////////////////

int cgiMain()
{
    LIST list;
	char buff[128];
	
			
        cgiHeaderContentType("text/html");
		update();

		list = listCreate();
		sprintf(buff, "%safterComplete.html", HTMLPATH);
        listDisplay(list, buff);
		fflush(0);
		listDelete(list);

  return 0;
}
