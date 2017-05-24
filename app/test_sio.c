#include "sb_include.h"
#include "sb_define.h"
#include "sb_shared.h"
#include "sb_config.h"
#include "sb_extern.h"
#include "sb_ioctl.h"
#include "sb_misc.h"

void Check_dtr_dcd (int fd);
void Check_rts_dcd (int fd);
void Check_rts_cts (int fd);
void Check_dtr_dsr (int fd);
char Test_232_422_loopback (int sfd, int rfd);
unsigned int get_CRC16(unsigned char data, unsigned int crc16) ;
void EDDY_CPU_TEST  ();
void EDDY_S4M_TEST  ();
//========================================================================
int main (int argc, char *argv[])
{
int Product_ID;

	Product_ID = SB_GetModel ();
	if (Product_ID == EDDY_S4M)
		EDDY_S4M_TEST ();
	else
		EDDY_CPU_TEST ();

return (0);
}
//=============================================================================
void EDDY_CPU_TEST  ()
{
int SFD, RFD;
char WORK[2048];
char r1, r2;

	printf ("\nRS232 (1)   ");
	SFD = SB_OpenSerial (0);
	SB_InitSerial (SFD, 10, 3, 0);		/* 115k */
	if (Test_232_422_loopback (SFD, SFD) == 'X') 
		printf ("Loopback Error  ");
	else
		printf ("Loopback OK,    ");
	Check_rts_cts (SFD);
	Check_dtr_dsr (SFD);
	Check_dtr_dcd (SFD);
	close (SFD);

	printf ("\nRS232 (2)   ");
	SFD = SB_OpenSerial (1);
	SB_InitSerial (SFD, 10, 3, 0);		/* 115k */
	if (Test_232_422_loopback (SFD, SFD) == 'X') 
		printf ("Loopback Error, ");
	else
		printf ("Loopback OK,    ");
	Check_rts_cts (SFD);
	close (SFD);

	printf ("\nRS422 (3,4) DIP-SW (On-Off-On-On)    Press any key continue .... ");		
	getchar ();
	SFD = SB_OpenSerial (2);
	SB_InitSerial (SFD, 10, 3, 0);		/* 115k */
	read (SFD, WORK, 1024);
	RFD = SB_OpenSerial (3);
	SB_InitSerial (RFD, 10, 3, 0);		/* 115k */
	read (RFD, WORK, 1024);
	printf ("RS422 (3<->4) : ");
	r1 = Test_232_422_loopback (SFD, RFD);
	r2 = Test_232_422_loopback (RFD, SFD);
	if (r1 == 'X' || r2 == 'X')
		printf ("Loopback Error ");
	else
		printf ("Loopback OK    ");
			
	printf ("\nRS485 (3,4) DIP-SW (Off-On-Off-Off)  Press any key continue .... ");
	getchar ();
	printf ("RS485 (3<->4) : ");	
	r1 = Test_232_422_loopback (SFD, RFD);
	r2 = Test_232_422_loopback (RFD, SFD);
	if (r1 == 'X' || r2 == 'X')
		printf ("Loopback Error \n");
	else
		printf ("Loopback OK    \n");

	close (SFD);
	close (RFD);
	return;
}
//=============================================================================
void EDDY_S4M_TEST  ()
{
int SFD, RFD;
char WORK[2048];
int interface;
char r1, r2;

	printf ("\nRS232 (1)   ");
	SFD = SB_OpenSerial (0);
	SB_InitSerial (SFD, 10, 3, 0);		// 115k 
	if (Test_232_422_loopback (SFD, SFD) == 'X') 
		printf ("Loopback Error  ");
	else
		printf ("Loopback OK,    ");
	Check_rts_cts (SFD);
	Check_dtr_dsr (SFD);
	Check_dtr_dcd (SFD);
	close (SFD);

	printf ("\nRS232 (2)   ");
	SFD = SB_OpenSerial (1);
	SB_InitSerial (SFD, 10, 3, 0);		// 115k 
	if (Test_232_422_loopback (SFD, SFD) == 'X') 
		printf ("Loopback Error, ");
	else
		printf ("Loopback OK,    ");
	Check_rts_cts (SFD);
	close (SFD);

	printf ("\nRS422 (3,4) Test,   Press any key continue .... ");		
	getchar ();
	SFD = SB_OpenSerial (2);
	interface = SB_RS422_PTOP;
	ioctl (SFD, INTERFACESEL, &interface);
	SB_InitSerial (SFD, 10, 3, 0);		// 115k 
	read (SFD, WORK, 1024);
	
	RFD = SB_OpenSerial (3);
	interface = SB_RS422_PTOP;
	ioctl (RFD, INTERFACESEL, &interface);
	SB_InitSerial (RFD, 10, 3, 0);		// 115k 
	read (RFD, WORK, 1024);

	printf ("RS422 (3<->4) : ");
	r1 = Test_232_422_loopback (SFD, RFD);
	r2 = Test_232_422_loopback (RFD, SFD);
	if (r1 == 'X' || r2 == 'X')
		printf ("Loopback Error ");
	else
		printf ("Loopback OK    ");
	close (SFD);
	close (RFD);


	SFD = SB_OpenSerial (2);
	interface = SB_RS485_NONE_ECHO;
	ioctl (SFD, INTERFACESEL, &interface);
	SB_InitSerial (SFD, 10, 3, 0);		/* 115k */
	read (SFD, WORK, 1024);
	
	RFD = SB_OpenSerial (3);
	interface = SB_RS485_NONE_ECHO;
	ioctl (RFD, INTERFACESEL, &interface);
	SB_InitSerial (RFD, 10, 3, 0);		/* 115k */
	read (RFD, WORK, 1024);
	
	printf ("\nRS485 (3,4) Test,   Press any key continue .... ");
	getchar ();

	printf ("RS485 (3<->4) : ");	
	r1 = Test_232_422_loopback (SFD, RFD);
	r2 = Test_232_422_loopback (RFD, SFD);
	if (r1 == 'X' || r2 == 'X')
		printf ("Loopback Error \n");
	else
		printf ("Loopback OK    \n");

	close (SFD);
	close (RFD);
	return;
}
//=============================================================================
#define TEST_LENGTH	2560
char Test_232_422_loopback (int sfd, int rfd)
{
int i, ret, len=0;
char SBUF[3000], RBUF[3000];	
unsigned int crc16A=0xffff;
unsigned int crc16B=0xffff;

	for (i=0; i<TEST_LENGTH; i++) SBUF[i] = i;
    for (i=0;i<TEST_LENGTH;i++) crc16A = get_CRC16 (SBUF[i], crc16A); 
	read  (rfd, &RBUF[len], TEST_LENGTH);		// buffer clear
	for (i=0; i<10; i++)
		{
		write (sfd, &SBUF[i*(TEST_LENGTH/10)], (TEST_LENGTH/10));
		SB_msleep (50);
		ret = read  (rfd, &RBUF[len], 1024);
		if (ret > 0) len += ret;
		}		

	if (len != TEST_LENGTH) return 'X';	
    for (i=0;i<TEST_LENGTH;i++) crc16B = get_CRC16 (RBUF[i], crc16B); 
	if (crc16A == crc16B) return '-';
	return 'X';
}	
//=============================================================================
unsigned int get_CRC16(unsigned char data, unsigned int crc16) 
{ 
        int i; 
        unsigned int temp_crc16; 
        unsigned char temp_data; 
 
        temp_crc16 = crc16; 
        temp_data = (unsigned char) data; 
 
        temp_data <<=1; 
        for ( i = 8; i > 0; i--){ 
 
                temp_data >>= 1; 
                if ((temp_data ^ temp_crc16) & 0x0001) 
                        temp_crc16 = (temp_crc16 >> 1) ^ 0x8408; 
                else 
                        temp_crc16 >>= 1; 
        } 
        return(temp_crc16); 
} 
//=============================================================================
void Check_rts_cts (int fd)
{
int CTS1, CTS2;

	SB_SetRts (fd, 1);
	if (SB_GetMsr(fd) & 0x10) CTS1=1; else CTS1=0;

	SB_SetRts (fd, 0);
	if (SB_GetMsr(fd) & 0x10) CTS2=0; else CTS2=1;
	if (CTS1 && CTS2) 
		printf ("RTS-CTS OK,    ");
	else
		printf ("RTS-CTS Error  ");
}
//=============================================================================
void Check_dtr_dsr (int fd)
{
int DSR1, DSR2;
			
	SB_SetDtr (fd, 1);
	if (SB_GetMsr(fd) & 0x20) DSR1=1; else DSR1=0;

	SB_SetDtr (fd, 0);
	if (SB_GetMsr(fd) & 0x20) DSR2=0; else DSR2=1;		
	if (DSR1 && DSR2) 
		printf ("DTR-DSR OK,    ");
	else
		printf ("DTR-DSR Error  ");
}
//=============================================================================
void Check_dtr_dcd (int fd)
{
int DCD1, DCD2;
	
	SB_SetDtr (fd, 1);
	if (SB_GetMsr(fd) & 0x80) DCD1=1; else DCD1=0;

	SB_SetDtr (fd, 0);
	if (SB_GetMsr(fd) & 0x80) DCD2=0; else DCD2=1;		
	if (DCD1 && DCD2) 
		printf ("DTR-DCD OK,    ");
	else
		printf ("DTR-DCD Error  ");
}
