/*****************************************************************************
Title		: UDP_Client sampme code

Description	: As UDP access is tried to server¡¯s IP address and socket number 
			  to be designated to argument, after access, the received data from 
			  the other socket is re-transmitted to the other socket. 
Argments  1	: Destination IP Address (ex: 192.168.1.1)
argments  2	: Socket no (1000 ~ 65535)
******************************************************************************/
#include "sb_include.h"
#include "sb_define.h"
#include "sb_shared.h"
#include "sb_config.h"
#include "sb_extern.h"
#include "sb_ioctl.h"
#include "sb_misc.h"

#define BUFFER_SIZE		1024

//===============================================================================	
int main (int argc, char *argv[])
{
char WORK [1024];
int  len, remote_port, fd;

    if (argc < 3)
	    {
		printf ("Usage> test_udp_client  <Server IP Address>  <Socket_no>\n");
	   	return 0;
    	}

	remote_port = atoi (argv[2]);
	printf ("Destination IP = (%s), Port (%d)\n", argv[1], remote_port);

	fd = SB_BindUdp (5000);
	if (fd <= 0) return 0;


	sprintf (WORK, "Hello World !!\r");
	SB_SendUdpClient (fd, WORK, strlen(WORK), argv[1], remote_port);

	while (1)
		{
		len = SB_ReadUdp (fd, WORK, BUFFER_SIZE);
		if (len)
			{
			SB_SendUdpClient (fd, WORK, len, argv[1], remote_port);
			WORK[len] = 0x00;
			printf ("Read/Write Len = %d, Data = %s\n", len, WORK);
			
			if (WORK[0] == 'Q') 
				{
				printf ("Detect Exit key\n");
				break;
				}			
			}
		}	 

	shutdown(fd, SHUT_RDWR);	
	close (fd);
}	

