/*===========================================================================================
Title 		: Serial  sample  code

Description	: The received data from device accessed each serial-port re-transmits to device.
argments  1	: Port no  (1 ~ 4)
argments  2	: BaudRate (150 ~ 921600)
============================================================================================*/
#include "sb_include.h"
#include "sb_define.h"
#include "sb_shared.h"
#include "sb_config.h"
#include "sb_extern.h"
#include "sb_ioctl.h"
#include "sb_misc.h"

#define BUFFER_SIZE		1024
//========================================================================
int main (int argc, char *argv[])
{
int fd, len, val, Port_No, speed, interface;
char WORK [BUFFER_SIZE];

    if (argc < 3)
	    {
		printf ("Usage> test_serial  <Port No (1 ~ 4)>  <Baudrate (150 ~ 921600)>\n");
	   	return 0;
    	}
	
	Port_No = atoi (argv[1]);
	if (Port_No < 1 || Port_No > 4) 
		{
		printf ("Error Port number!\n");
		return 0;
		}
		
	speed = atoi (argv[2]);
	switch (speed)
		{
		case 150	:	val = 0;	break;
		case 300	:	val = 1;	break;
		case 600	:	val = 2;	break;
		case 1200	:	val = 3;	break;
		case 2400	:	val = 4;	break;
		case 4800	:	val = 5;	break;
		case 9600	:	val = 6;	break;
		case 19200	:	val = 7;	break;
		case 38400	:	val = 8;	break;
		case 57600	:	val = 9;	break;
		case 115200	:	val = 10;	break;
		case 230400	:	val = 11;	break;
		case 460800	:	val = 12;	break;
		case 921600	:	val = 13;	break;
		default :		val = 10;	break;
		}
		
	fd = SB_OpenSerial (Port_No - 1);
	if (fd <= 0) 
		{
		printf ("Serial Port Open Erroe!\n");
		return 0;
		}

	switch (Port_No)
		{
		case 1	:	interface = SB_RS232;		break;		// Only RS232 
		case 2	:	interface = SB_RS232;		break;		// Only RS232 
		case 3  :	interface = SB_RS422_PTOP;	break;		// Select (SB_RS422_PTOP, SB_RS485_NONE_ECHO, SB_RS485_ECHO)
		case 4  :	interface = SB_RS422_PTOP;	break;		// Select (SB_RS422_PTOP, SB_RS485_NONE_ECHO, SB_RS485_ECHO)
		}
	ioctl (fd, INTERFACESEL, &interface);

	SB_InitSerial (fd, val, 3, 0);		// Speed, Parity None, 8 Data Bits, 1 stop Bits, Flow None 

	printf ("Open Serial Port (%d), Baudrate=%d,  Interface = %d\n", Port_No, speed, interface);
	
	while (1)
		{
		len = SB_ReadSerial (fd, WORK, BUFFER_SIZE, 0);
		if (len)
			{
			SB_SendSerial (fd, WORK, len);
			WORK[len] = 0x00;
			printf ("Read/Write Len = %d, Data = %s\n", len, WORK);

			if (WORK[0] == 'Q') 
				{
				printf ("Detect Exit key\n");
				break;
				}
			}
		}

	close (fd);
	return 0;
}


