/*****************************************************************************
Title		: TCP_Client (Serial to LAN) sampme code

Description	: After each serial-port¡¯s configuration file to be set in the WEB, 
              Telnet is read, the access is tried to connect configuration file 
              to be designated server in the TCP manner. After the socket is accessed, the data to be read serial-port is transmitted to TCP socket and the data to be read TCP socket is transmitted to serial port..
argments  1	: Port no  (1 ~ 4)
******************************************************************************/
#include "sb_include.h"
#include "sb_define.h"
#include "sb_shared.h"
#include "sb_config.h"
#include "sb_extern.h"
#include "sb_ioctl.h"
#include "sb_misc.h"

#define MAX_BUFFER_SIZE 		2048
#define MAX_SOCKET_READ_SIZE 	512
#define MAX_SERIAL_READ_SIZE 	512

char WORK [MAX_BUFFER_SIZE];
int LFD = 0;
int SFD = 0;
int port_no;

struct SB_SIO_CONFIG			cfg [SB_MAX_SIO_PORT];

int main (int argc, char *argv[]);
void mainloop(void);
int receive_from_lan (int sfd, int lfd);
int receive_from_port(int sfd, int lfd);

//===============================================================================	
int main (int argc, char *argv[])
{
    if (argc < 2)
	    {
		printf ("Usage> test_serial_to_lan-2  <port_no(1 ~ 4)>\n");	
	   	return 0;
    	}

	port_no = atoi (argv[1]) - 1;			// port no 1 ~ 4

	while (1) 
		{									// reference WEB Configuration --> include/sb_config.h
		SB_ReadConfig  (CFGFILE_ETC_SIO,     (char *)&cfg[0], sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);	
		mainloop();
		}
}
//===============================================================================
void mainloop()
{
int  cnt, ret;
char Server_IP [100];
	
	sprintf (Server_IP, "%d.%d.%d.%d", cfg[port_no].remote_ip[0], cfg[port_no].remote_ip[1], cfg[port_no].remote_ip[2], cfg[port_no].remote_ip[3]);

	printf ("Port = %d, Server IP = %s(%d), Baudrate = %d\n", port_no+1, Server_IP, cfg[port_no].remote_socket_no, cfg[port_no].speed);

	for (cnt=0; cnt<10; cnt++)
		{
		ret = SB_ConnectTcp (Server_IP, cfg[port_no].remote_socket_no, 3, 8, 8);
		if (ret > 0) break;
		SB_msleep (2000);
		}
	
	if (ret <= 0) return;
	LFD = ret;		
	printf ("Connection Successfully !\n");
	
	if (SFD <= 0)
		{
		int interface = cfg[port_no].interface;	
		SFD = SB_OpenSerial (port_no);
		if (SFD <= 0) return;
		ioctl (SFD, INTERFACESEL, &interface);
		}
	SB_InitSerial (SFD, cfg[port_no].speed, cfg[port_no].dps, cfg[port_no].flow);
	SB_ReadSerial (SFD, WORK, MAX_BUFFER_SIZE, 0);
	SB_SetDtr (SFD, SB_ENABLE);
	SB_SetRts (SFD, SB_ENABLE);
	
	while (1)
		{	
		if (receive_from_lan (SFD, LFD) < 0) break;
		if (receive_from_port(SFD, LFD) < 0) break;
		}		
	
	if (LFD > 0)
		{
		shutdown(LFD, SHUT_RDWR);
		close (LFD);
		LFD = 0;
		}
	printf ("Close Socket\n");
	SB_msleep (100);
}
//===============================================================================
int receive_from_lan(int sfd, int lfd)
{
int len;

	len = SB_ReadTcp (lfd, WORK, MAX_SOCKET_READ_SIZE);
	if (len <= 0) return len;
	
	SB_SendSerial (sfd, WORK, len);
	WORK[len] = 0;
	printf ("LAN -> Serial  Len = %d, Data = %s\n", len, WORK);
	return 0;
} 
//===============================================================================
int receive_from_port(int sfd, int lfd)
{
int len, ret; 

	len = SB_ReadSerial (sfd, WORK, MAX_SERIAL_READ_SIZE, cfg[port_no].packet_latency_time);
	if (len <= 0) return 0;

	ret = write (lfd, WORK, len);
	WORK[ret] = 0;
	printf ("Serial -> LAN  Len = %d, Data = %s\n", ret, WORK);
	return 0;
}
