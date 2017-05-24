/*****************************************************************************
Copyright (c) 2007 SystemBase Co., LTD  All Rights Reserved.

Kill processor 		09, 31, 2007. yhlee
******************************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <asm/ioctl.h>
#include <dirent.h>

#define FLASHERASE                _IOW('F', 3, erase_info_t)

typedef struct erase_info_t {
		u_long offset;
		u_long size;
} erase_info_t;



#define  RW_SIZE      128

int firm_v;
unsigned char dat[1000];
int flash_fd;

void kill_process (char *pross, int len);
int main(int argc, char *argv[]);

int main(int argc, char *argv[])
{
int i;

	for (i=1; i<argc; i++)
		{
		kill_process (argv[i], strlen(argv[i]));
		}
}
/*==================================================================*/
void kill_process (char *pross, int len)
{
DIR *dp;
struct dirent *entry;
char ch;
int ret, fd, pid_no, i;
char name [30], work[100];
		
		if ((dp = opendir ("/proc/")) == NULL)
			{
			printf ("proc dir not fount !\n");
			return;
			}

		chdir ("/proc/");
		while ((entry = readdir (dp)) != NULL)
			{
			if (strcmp (".",  entry->d_name) == 0) continue;
			if (strcmp ("..", entry->d_name) == 0) continue;
			ch = entry->d_name[0];
			if (!(ch >= '0' && ch <= '9')) continue;
			pid_no = atoi (entry->d_name);
			sprintf (name, "/proc/%d/cmdline", pid_no);
			fd = open (name, 0);
			if (fd < 0) continue;
				
			for (i=0; i<90; i++) work[i] = 0x00;
			ret = read (fd, work, 90);
			close (fd);		
			
			if (ret <= 0) continue;
	
			ret = strlen(work);
			for (i=0; i<ret; i++) 
				{
				if (!strncmp (pross, &work[i], len)) 
					{
					printf ("Task name=%-20s, PID=%d\n", work, pid_no);
					sprintf (work, "kill -9 %d", pid_no);
					system (work);
					i=ret;
					}
				}
			}
		closedir (dp);
		return;
}
