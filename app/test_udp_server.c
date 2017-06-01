/*****************************************************************************
Title		: UDP_Server sampme code

Description	: The socket number is designated as an argument. By this situation, 
			  the UDP access is waited. After the access, the received data from 
			  the other socket is re-transmitted to the other socket.
argments  1	: listen Socket no (1000 ~ 65535)
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
int  len, listen_local_port, fd;

    if (argc < 2)
	    {
		printf ("Usage> test_udp_server  <Listen_Socket_no>\n");
	   	return 0;
    	}

	listen_local_port = atoi (argv[1]);
	printf ("Listen Socket No = %d  \n", listen_local_port);

	fd = SB_BindUdp (listen_local_port);
	if (fd <= 0) return 0;

	while (1)
		{
		len = SB_ReadUdp (fd, WORK, BUFFER_SIZE);
		if (len)
			{
			SB_SendUdpServer (fd, WORK, len);
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
