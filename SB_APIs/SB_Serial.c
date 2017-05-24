/*
	http://www.raviyp.com/embedded/189-serial-port-programming-in-linux-using-c-working-code
	https://github.com/longqzh/G-type-485-DCU
	https://github.com/longqzh/daegu-DCU
	https://github.com/longqzh/incheon-dcu
*/

#include "sb_include.h"
#include "sb_ioctl.h"

extern void SB_msleep(int msec);

#define MAX_PORT_NO 10

static struct termios old_port_settings[MAX_PORT_NO];
static int port_fd[MAX_PORT_NO];

//----------------------------------------------------------------------
// Argment 		:  
// 			port_no	=>  Number of port number   (/dev/ttyS0, S1)
// Return Value : 
//			0 <  N	=>  handle no
//			-1		=>  open failed
//----------------------------------------------------------------------
int SB_OpenSerial(int port_no)
{
	int fd;
	char pathname[16];
	int n;

	if( port_no >= MAX_PORT_NO || port_no < 0 )
	{
		printf("illegal comport number\n");
		return -1;
	}

	n = snprintf(pathname, sizeof(pathname), "/dev/ttyS%d", port_no);
	/* Check error code */
	if( n < 0 )
	{
		perror("unable to make comport pathname ");
		return -1;
	}
	/* Check not enough space */
	if( n >= sizeof(pathname) )
	{
		perror("not enough space to make comport pathname ");
		return -1;
	}

	fd = open(pathname, O_RDWR | O_NOCTTY | O_NDELAY);
	if( fd == -1)
	{
		perror("unable to open port_no ");
		return -1;
	}

	port_fd[port_no] = fd;
	return port_no;
}

void SB_CloseSerial(int sfd)
{
	int status;

	if(ioctl(port_fd[sfd], TIOCMGET, &status) == -1)
	{
		perror("unable to get portstatus");
	}

	status &= ~TIOCM_DTR;    /* turn off DTR */
	status &= ~TIOCM_RTS;    /* turn off RTS */

	if(ioctl(port_fd[sfd], TIOCMSET, &status) == -1)
	{
		perror("unable to set portstatus");
	}

	tcsetattr(port_fd[sfd], TCSANOW, old_port_settings + sfd);
	close(port_fd[sfd]);
}
	
//----------------------------------------------------------------------
// Argment 		:  
// 			Serial_fd =>  serial handle
//			bps		  =>  150 BPS(0) ~ 921K BPS(13) 		
//			lcr	      =>  LSR Register Standard 
//			flow	  =>  0:None, 0x20:Xon, 0x80:CTSRTS 			
// Return Value : 
//			None 	
//----------------------------------------------------------------------
void SB_InitSerial(int sfd, char speed, char dps, char flow)
{
	struct termios newtio;
	unsigned long data_bit = CS8;
	unsigned long stop_bit = 0;
	unsigned char parity_from_dps = 0;
	unsigned long parity = 0;
	unsigned long speed_set[]={	B150, B300, B600, B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B921600};
	int error;

	/*
	http://pubs.opengroup.org/onlinepubs/7908799/xsh/termios.h.html

	http://man7.org/linux/man-pages/man3/termios.3.html

	http://xanthium.in/Serial-Port-Programming-on-Linux
	*/
	error = tcgetattr(port_fd[sfd], old_port_settings + sfd);
	if( error == -1)
	{
		perror("unable to read portsettings ");
		return;
	}

	bzero(&newtio, sizeof(newtio));
	switch (dps&0x03) 
	{
		case 0x00: data_bit = CS5; break;
		case 0x01: data_bit = CS6; break;
		case 0x02: data_bit = CS7; break;
		case 0x03: data_bit = CS8; break;
	}
	if (dps&0x04) stop_bit = CSTOPB; 
	
	parity_from_dps = dps&0x18;
	switch (parity_from_dps) 
	{
		case 0x00: parity = 0; break;
		case 0x08: parity |= (PARENB | PARODD); break;
		case 0x10:
		case 0x18: parity |= PARENB; break;
	}

	newtio.c_cflag = speed_set[(int)speed]|data_bit|stop_bit|parity|CLOCAL|CREAD;
	newtio.c_cflag |= HUPCL;

	if (flow == 2)	newtio.c_cflag |=  CRTSCTS;
	if (flow == 1) 	newtio.c_iflag = (IXON|IXOFF);	

	newtio.c_iflag |= (IGNPAR | IGNBRK);
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;
	newtio.c_cc[VMIN] = 0;
	newtio.c_cc[VTIME] = 0;	
	newtio.c_cc[VSTART] = 0x11;
	newtio.c_cc[VSTOP] = 0x13;
/*
	cfsetispeed(&new_port_settings, speed_set[(int)speed]);
	cfsetospeed(&new_port_settings, speed_set[(int)speed]);
*/
	tcflush(port_fd[sfd], TCIFLUSH);
	error = tcsetattr(port_fd[sfd], TCSANOW, &newtio);
	if( error == -1 )
	{
		perror("unable to adjust portsettings ");
		return;
	}

	return;
}

//===============================================================================	
void SB_SendSerial(int sfd, char *buff, int len)
{
	int ret;
	short cnt=0;

	while (1)
	{
		ret = write (port_fd[sfd], buff, len);	
		if (ret > 0) 			
		{
			len -= ret;
			buff += ret;
		}	
		if (len > 0) 
		{
			if (++cnt > 20) return; 			// Max 2 sec Wait
//			printf ("cnt=%d len=%d\n",cnt, len);
			SB_msleep (20);			 			// delay 20 msec 
		}
		else
		{
			break;
		}
	}
}

//----------------------------------------------------------------------
// Argment 		:  
// 			fd			=>  serial Handle
//			buff		=>  Receive Workarea 		
//			buff_limit	=>  Workarea buffer size	
//			wati_msec	=>  next byte receive time
//		
// Return Value :  error code or read length
//			  0  :   no receive data
//		      N  :   read bytes
//----------------------------------------------------------------------
int SB_ReadSerial(int sfd, char *buff, int buffer_limit, int wait_msec)
{
	int ret, RCV_len=0;

	while (1)
	{
		ret = read (port_fd[sfd], buff, buffer_limit-RCV_len);
		if (ret <= 0) break;
		buff += ret;
		RCV_len  += ret;
		if (wait_msec <= 0) break;
		SB_msleep (wait_msec);
	}				
	
	return (RCV_len);
}

 
/*
Constant  Description
TIOCM_LE        DSR (data set ready/line enable)
TIOCM_DTR       DTR (data terminal ready)
TIOCM_RTS       RTS (request to send)
TIOCM_ST        Secondary TXD (transmit)
TIOCM_SR        Secondary RXD (receive)
TIOCM_CTS       CTS (clear to send)
TIOCM_CAR       DCD (data carrier detect)
TIOCM_CD        see TIOCM_CAR
TIOCM_RNG       RNG (ring)
TIOCM_RI        see TIOCM_RNG
TIOCM_DSR       DSR (data set ready)
 
http://man7.org/linux/man-pages/man4/tty_ioctl.4.html
*/

//----------------------------------------------------------------------
// Argment 		:  
// 			fd	=>  serial Handle
//
// Return Value : 
//			x x x x x x x x
// 			| | | +-------- CTS stat 	  			
//			| | +---------- DSR stat 
//			| +------------ RI  stat		
//			+--------------	DCD stat
//===============================================================================
unsigned char SB_GetMsr(int sfd)
{
	return (unsigned char) (ioctl(port_fd[sfd], TIOTSMSR));
}

#if 0
//=============================================================
void SB_SetUart (int sfd, int tx_level, int rx_level, int fifo_size)
{
	ioctl (port_fd[sfd], TX_TRIGGER_SET,  &tx_level);
	ioctl (port_fd[sfd], RX_TRIGGER_SET,  &rx_level);
	ioctl (port_fd[sfd], TX_FIFOSIZE,     &fifo_size);
}
#endif

//=============================================================
void SB_SetDtr (int sfd, int flag)
{
unsigned int *option = (unsigned int *)TIOCM_DTR;

	if (flag)
		ioctl(port_fd[sfd], TIOCMBIS, &option);		// ON
	else
		ioctl(port_fd[sfd], TIOCMBIC, &option);		// OFF
}

//=============================================================
void SB_SetRts (int sfd, int flag)
{
unsigned int *option = (unsigned int *)TIOCM_RTS;

	if (flag)
		ioctl(port_fd[sfd], TIOCMBIS, &option);		// ON
	else	
		ioctl(port_fd[sfd], TIOCMBIC, &option);		// OFF
}

/*===============================================================================
Speed	    1 byte/usec	    Wait bytes      msec
150			66667 			2				133 
300			33333 			2				66 
600			16667 			2				33 
1200		8333 			3				25 
2400		4167 			3				12 
4800		2083 			4				7
9600		1042 			5				5 
19200		521 			10				5 
38400		260 			12				3 
57600		174 			18				3 
115200		87 				25				2 
230400		43 				30				1 
460800		22 				50				1 
921600		11 				100				1 
===============================================================================*/
int SB_GetDelaySerial (int bps)
{
	switch (bps)
		{
		case 0 : return  133;			// 150 bps
		case 1 : return  66;			// 300 bps
		case 2 : return  33;			// 600 bps
		case 3 : return  25;			// 1200 bps
		case 4 : return  12;			// 2400 bps
		case 5 : return  7;				// 4800 bps
		case 6 : return  5;				// 9600 bps
		case 7 : return  5;				// 19200 bps
		case 8 : return  3;				// 38400 bps
		case 9 : return  3;				// 57600 bps
		case 10 : return 2;				// 115k bps
		case 11 : return 1;				// 230k bps
		case 12 : return 1;				// 460k bps			
		case 13 : return 1;				// 920k bps
		default : return 5;
		}
}	

/*---------------------------------------------------------------------
Argment 	:  
	interface	RS422_PTOP			1
				RS422_MULTIDROP		2
				RS485_NONE_ECHO		3
				RS485_ECHO			4
return		:   
	none
-----------------------------------------------------------------------
void SB_SetSerialInterface (int port_no, int interface)
{
int fd;
    fd = SB_OpenSerial  (port_no);

//	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) 
	{
	printf(" SB_SetSerialInterface open error \n");
	
	return;
	}
	ioctl (fd, INTERFACESEL, &interface);
	close (fd);
}

*/