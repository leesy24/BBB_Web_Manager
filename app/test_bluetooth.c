#include "sb_include.h"
#include "sb_define.h"
#include "sb_shared.h"
#include "sb_config.h"
#include "sb_extern.h"
#include "sb_ioctl.h"
#include "sb_misc.h"

#define		CMD_CONNECT_ESCAPE		"+++"
#define		CMD_CONNECT_RESUME		"ATO"
#define		CMD_DISCONNECT			"ATH"
#define		CMD_BTINFO				"AT+BTINFO?"
#define		CMD_SET_MODE			"AT+BTMODE,"
#define		CMD_SET_SERIAL			"AT+UARTCONFIG,"

int main (int argc, char *argv[]);
int BT_Connect_Status ();
void BT_SendData (int fd, const char *fmt, ...);
void Read_from_Bluetooth (int fd, int wait_msec);
//========================================================================
int main (int argc, char *argv[])
{
int SFD, cnt = 0;;
char buff[100];

    if (argc < 2)
	    {
		printf ("Usage> test_bluetooth  <Dest BT Address(6 Byte Hexa)>\n");
	   	return 0;
    	}


	system ("kt  pinetd  com_  tcp_  udp");			// Disable  Damon & Serial Application



	SFD = SB_OpenSerial (3);					// Serial 4
	SB_InitSerial (SFD, 6, 3, 2);			// 9600, Parity none, 8 data bits, 1 stop bits, HW FlowControl

	BT_SendData (SFD, "%s\r", CMD_CONNECT_ESCAPE);		// +++
	SB_msleep (100);

	BT_SendData (SFD, "%s\r", CMD_DISCONNECT);			// ATH  

	BT_SendData (SFD, "%s\r", CMD_BTINFO);				// Get Info
	Read_from_Bluetooth (SFD, 1000);
	
	BT_SendData (SFD, "%s%s\r", CMD_SET_SERIAL, "9600,n,1,1");	// 9600 bps, none parity, 1 stop bit, RTS/CTS Flow enable 
	BT_SendData (SFD, "%s%s\r", CMD_SET_MODE, "0");				// Mode 0 (AT command Mode)

	BT_SendData (SFD, "%s\r", CMD_BTINFO);
	Read_from_Bluetooth (SFD, 1000);							// Clear Buffer


	BT_SendData (SFD, "ATD%s\r", argv[1]);						// Connect Destination BT Address

	while (1)
		{
		if (BT_Connect_Status()) break;
		SB_msleep (300);
		printf ("Connect Wait\n");
		} 

	printf ("Connect OK\n");


	while (1)
		{
		sprintf (buff, "[%03d] Hello Eddy Bluetooth 2.0\n", ++cnt);
		BT_SendData (SFD, buff);
		Read_from_Bluetooth (SFD, 300);				// timeout 300 msec
		}
	close (SFD);
}
//========================================================================
int BT_Connect_Status ()
{	
int fd, value;

	fd = open (SB_GPIO_DEVICE, O_RDWR);					// dev/gpio                  
	ioctl (fd, GETGPIOVAL_LC, &value);
	close (fd);
	
	if (value & 0x02000000)		// PC25
		return 0;				// Disconnect stat
	else
		return 1;				// Connect stat
}
//==========================================================================
void BT_SendData (int fd, const char *fmt, ...)
{
va_list args;
char buff[100];

    va_start(args, fmt);
    vsprintf(buff,fmt,args);
    va_end(args);
	
	write (fd, buff, strlen(buff));
	printf ("Send = %s", buff);
}
//===============================================================================
void Read_from_Bluetooth (int fd, int wait_msec)
{
char WORK[2000];
int len; 
unsigned long Timer;


	Timer = SB_GetTick () + wait_msec;
	
	while (1)
		{
		if (Timer < SB_GetTick ()) 	return;
		len = SB_ReadSerial (fd, WORK, 1000, 50);
		if (len <= 0) continue;
		WORK[len] = 0x00;
		printf ("Read = %s\n", WORK);
		}
}










