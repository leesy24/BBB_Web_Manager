/*****************************************************************************
Title		: TCP_Server sampme code

Description	: The socket number is designated as an argument. By this situation, 
			  the TCP access is waited. After the access, the received data from 
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

int main (int argc, char *argv[]);
//===============================================================================	
int main (int argc, char *argv[])
{
char WORK [1024];
int  ret, len, listen_socket_no, fd;

    if (argc < 2)
	    {
		printf ("Usage> test_tcp_server  <Listen_Socket_no>\n");
	   	return 0;
    	}

	listen_socket_no = atoi (argv[1]);
	printf ("Listen Socket No = %d  \n", listen_socket_no);

	fd = SB_ListenTcp (listen_socket_no, 8, 8);
	if (fd <= 0) 
		{
		printf ("TCP Socket Bind Error !\n");	
		return (0);
		}

	while (1)
		{
		ret = SB_AcceptTcp (fd, 1);
		if (ret < 0) 
			{
			printf ("Accept Error !\n");
			return (0);
			}
		if (ret > 0) break;	
		printf ("Connect Waiting !\n");
		}
	
	fd = ret;
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
