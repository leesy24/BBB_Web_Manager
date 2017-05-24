/*****************************************************************************
Title		: TCP_Client sampme code

Description	: As TCP access is tried to server¡¯s IP address and socket number to be 
			  designated to argument, after access, the received data from the other 
			  socket is re-transmitted to the other socket. 
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

int main (int argc, char *argv[]);
//===============================================================================	
int main (int argc, char *argv[])
{
char WORK [1024];
int  len, remote_socket_no, fd, cnt;

    if (argc < 3)
	    {
		printf ("Usage> test_tcp_client  <Server IP Address>  <Socket_no>\n");
	   	return 0;
    	}

	remote_socket_no = atoi (argv[2]);
	printf ("Destination IP = (%s), Port (%d)\n", argv[1], remote_socket_no);

	for (cnt=0; cnt<10; cnt++)
		{
		fd = SB_ConnectTcp (argv[1], remote_socket_no, 1, 8, 8);
		if (fd > 0) break;
		printf ("Connect Waiting !\n");
		}
	
	if (fd <= 0)
		{
		printf ("Connect Failed !\n");	
		return (0);
		}
		
	printf ("Connect Successfully \n");
	
	while (1)
		{	
		len = SB_ReadTcp (fd, WORK, 1000);
		if (len < 0) 
			{
			printf ("Socket Receive Error !\n");
			break;
			}

		if (len > 0)
			{
			write (fd, WORK, len);
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
	return (0);
}
