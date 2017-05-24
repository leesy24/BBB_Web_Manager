/*****************************************************************************
  Copyright (c) 2007 SystemBase Co., LTD  All Rights Reserved.

  TFTP Flash Updater (BootLoader or Kernel or FileSystem)	09, 31, 2007. yhlee
 ******************************************************************************/
#include "sb_include.h"
#include "sb_define.h"
//===============================================================================	
int main (int argc, char *argv[])
{
char tftp_ip[50], filename[5][100], cmd[90];
int i;

	if (argc < 3)
		{
		printf("Usage: upgradetftp  <TFTP IP Address>  <file_1, file_2, file_3, ...>\n");
		return 0;
		}

	strcpy (tftp_ip, argv[1]);
	printf ("TFTP IP Address = %s\n", tftp_ip);
	for (i=0; i<argc-2; i++)
		{
		strcpy (filename[i], argv[i+2]);
		printf ("Firmware Image Name (%d) = %s\n", i+1, filename[i]);
		}		

	printf ("\n\n");
	
	for (i=0; i<argc-2; i++)
		{
		sprintf (cmd, "/usr/bin/tftp -g -l %s -r %s %s", filename[i], filename[i], tftp_ip);
		system (cmd);

		sprintf (cmd, "/sbin/upgrade  %s", filename[i]);
		system (cmd);
		}	
	return 0;
}
