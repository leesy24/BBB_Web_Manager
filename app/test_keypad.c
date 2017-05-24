#include "sb_include.h"
#include "sb_define.h"
#include "sb_shared.h"
#include "sb_config.h"
#include "sb_extern.h"
#include "sb_ioctl.h"

//---------------------------------------------------------------------------------------------------------------------------------------
//bytes	    3								2								1								0							
//bits	    31	30	29	28	27	26	25	24	23	22	21	20	19	18	17	16	15	14	13	12	11	10	9	8	7	6	5	4	3	2	1	0
//bit		7	6	5	4	3	2	1	0	7	6	5	4	3	2	1	0	7	6	5	4	3	2	1	0	7	6	5	4	3	2	1	0
//---------------------------------------------------------------------------------------------------------------------------------------
//Port A	-	-	-	-	-	-	-	-	-	*	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	S2	S2	-	-	-	-
//Port B	Key Key	S1	S1	S0	S0	S0	S0	S0	S0	Key Key	*	*	*	*	DE	DE	*	*	S3	S3	S2	S2	S1	S1	S0	S0	EPM	EPM	EPM	EPM
//Port C	-	-	-	-	-	*	-	-	Key Key Key Key	*	*	NAD RST	LAN	NAD *	LAN	-   S3	*	S3	-	-	*   RDY	ADC	ADC	ADC	ADC
//---------------------------------------------------------------------------------------------------------------------------------------
void msec_sleep (int msec);
int main (int argc, char *argv[]);

int main (int argc, char *argv[])
{
int MODE[3], VALUE[3], MD[3];
int fd, value, no=0;
unsigned char key;

	printf ("Keypad Test Program     (Press SW16 key = Exit)\n");
	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) return 0;
			
	ioctl (fd, GETGPIOMOD_LM, &MODE[0]);	// Get Mode Port A,B,C
	ioctl (fd, GETGPIOVAL_LM, &VALUE[0]);	// Get Value Port A,B,C

	MD[0] = MODE[0];
	MD[1] = MODE[1] | 0xc0300000;			// Set Port B Output Mode (B31, B30, B21, B20)
	MD[2] = MODE[2] & 0xff0fffff;			// Set Port C Input  Mode (C23, C22, C21, C20)
	ioctl (fd, SETGPIOMOD_LM, &MD[0]);
  
	while (1)
		{
		ioctl (fd, GETGPIOVAL_LB, &value);
		if (++no > 3) no=0; 
		value |= 0xc0300000;
		switch (no) 
			{
			case 3 : value &= 0x7fffffff; break;	// Mask Port B31
			case 2 : value &= 0xbfffffff; break;	// Mask Port B30
			case 1 : value &= 0xffdfffff; break;	// Mask Port B21
			case 0 : value &= 0xffefffff; break; 	// Mask Port B20
			}

		ioctl (fd, SETGPIOVAL_LB, &value);
		ioctl (fd, GETGPIOVAL_LC, &value);
		switch (value & 0x00f00000)
			{
			case 0x00700000 :	key=4; break;		// Mask Port C23
			case 0x00b00000 :	key=3; break;		// Mask Port C22
			case 0x00d00000 :	key=2; break;		// Mask Port C21
			case 0x00e00000 :	key=1; break;		// Mask Port C20
			default			:	continue;	
			}

		key += (no*4);
		printf ("Key = SW-%02d\n", key);
		msec_sleep (200);

		if (key == 16) 
			{
			printf ("Get SW16 key,   End of Program...\n");
			break;
			}
		}
	
	ioctl (fd, SETGPIOMOD_LM, &MODE[0]);	// Restore Mode Port A,B,C
	ioctl (fd, SETGPIOVAL_LM, &VALUE[0]);	// Restore Value Port A,B,C
	close (fd);
	return 0;
}
//==========================================================================
void msec_sleep (int msec) 
{
struct timespec interval;

	interval.tv_sec  = msec / 1000;
	interval.tv_nsec = 1000000 * (msec % 1000);
	nanosleep (&interval, NULL);
}
