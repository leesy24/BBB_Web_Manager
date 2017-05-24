#include "sb_include.h"
#include "sb_define.h"
#include "sb_shared.h"
#include "sb_config.h"
#include "sb_extern.h"
#include "sb_ioctl.h"
#include "sb_misc.h"

struct eddy_gpio_struct
{
    unsigned int value[3];
    unsigned int mode[3];
    unsigned int pullup[3];
    unsigned int enable[3];
};

void Multi_output_ioctl (int repeat);
void Port_output_ioctl (int repeat);
void Multi_input_ioctl ();
void Port_input_ioctl ();


// S4M
//---------------------------------------------------------------------------------------------------------------------------------------
//bytes	    3								2								1								0							
//bits	    31	30	29	28	27	26	25	24	23	22	21	20	19	18	17	16	15	14	13	12	11	10	9	8	7	6	5	4	3	2	1	0
//bit		7	6	5	4	3	2	1	0	7	6	5	4	3	2	1	0	7	6	5	4	3	2	1	0	7	6	5	4	3	2	1	0
//---------------------------------------------------------------------------------------------------------------------------------------
//Port A	-   *   -   -   -   -   -   -   -   *   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   *   S2  -   -   -   -
//Port B	*   *   S1  S1  S0  S0  S0  S0  S0  S0  *   *   *   *   *   *   DE  DE  *   *   S3  S3  S2  S2  S1  S1  S0  S0  *   *   *   *
//Port C	ID  ID  ID  ID  ID  C4  *   *   C4  C3  C3  *   *   *   *   RST *   *   *   *   -   *   *   S3  -   -   *   Rdy ADC	ADC	ADC	ADC
//---------------------------------------------------------------------------------------------------------------------------------------


//CPU
//---------------------------------------------------------------------------------------------------------------------------------------
//bytes	    3								2								1								0							
//bits	    31	30	29	28	27	26	25	24	23	22	21	20	19	18	17	16	15	14	13	12	11	10	9	8	7	6	5	4	3	2	1	0
//bit		7	6	5	4	3	2	1	0	7	6	5	4	3	2	1	0	7	6	5	4	3	2	1	0	7	6	5	4	3	2	1	0
//---------------------------------------------------------------------------------------------------------------------------------------
//Port A	-	-	-	-	-	-	-	-	-	*	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	S2	S2	-	-	-	-
//Port B	Key Key	S1	S1	S0	S0	S0	S0	S0	S0	Key Key	*	*	*	*	DE	DE	*	*	S3	S3	S2	S2	S1	S1	S0	S0	EPM	EPM	EPM	EPM
//Port C	-	-	-	-	-	*	-	-	Key Key Key Key	*	*	NAD RST	LAN	NAD -	LAN	-   S3	*	S3	-	-	*   RDY	ADC	ADC	ADC	ADC
//---------------------------------------------------------------------------------------------------------------------------------------
//		S0 ~ S3	= Serial Port (Port 1 ~ 4)
//		DE		= Debug Port
//		RST		= Reset 
//		RDY		= Ready LED
//		ADC		= Analog Digital Converter 
//      LAN     = LAN Port
//		NAD		= Nand Flash
//		ER		= SPI(EEPROM)
//		Key		= Keypad
//		-		= Not Used
//		*		= GPIO & User Port


int Product_ID;
//........................................................................................................................................
int main (int argc, char *argv[])
{
struct eddy_gpio_struct gpio;
int repeat, fd;

	Product_ID = SB_GetModel ();


	printf ("Serial related GPIO Testing ...\n");
	
	if (Product_ID != EDDY_S4M)
		printf ("Please  Set UART-Select switch on the board  (On-On-On-On-On-Off)\n\n");
		

	if (argc < 2)
		repeat = 10;
	else
		repeat = atoi (argv[1]);

	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) return 0;
	
	
	if (Product_ID == EDDY_S4M)
		{
		gpio.enable[0]	= S4M_GPIO_MASK_PORT_A;					// Port A, Serial 2=2					Total 2  Gpio Ports
		gpio.enable[1]	= S4M_GPIO_MASK_PORT_B;					// Port B, Serial 0=8, 1=4, 2=2, 3=2	Total 16 Gpio Ports
		gpio.enable[2]	= S4M_GPIO_MASK_PORT_C  | 0x00000003;	// Port C, Serial 3=2					Total 2  Gpio Ports

		gpio.mode[0]	= S4M_GPIO_MASK_PORT_A;					// Output Mode
		gpio.mode[1]	= S4M_GPIO_MASK_PORT_B;					// Output Mode
		gpio.mode[2]	= S4M_GPIO_MASK_PORT_C  | 0x00000003;	// Output Mode

		gpio.value[0]	= S4M_GPIO_MASK_PORT_A;					// Value High
		gpio.value[1]	= S4M_GPIO_MASK_PORT_B;					// Value High
		gpio.value[2]	= S4M_GPIO_MASK_PORT_C;					// Value High

		gpio.pullup[0]	= S4M_GPIO_MASK_PORT_A;					// pullup High
		gpio.pullup[1]	= S4M_GPIO_MASK_PORT_B;					// pullup High
		gpio.pullup[2]	= S4M_GPIO_MASK_PORT_C | 0x00000003;	// pullup High
		}
	else
		{
		gpio.enable[0]	= 0x00000030 & GPIO_MASK_PORT_A;	// Port A, Serial 2=2					Total 2  Gpio Ports
		gpio.enable[1]	= 0x3fc00ff0 & GPIO_MASK_PORT_B;	// Port B, Serial 0=8, 1=4, 2=2, 3=2	Total 16 Gpio Ports
		gpio.enable[2]	= 0x00000500 & GPIO_MASK_PORT_C;	// Port C, Serial 3=2					Total 2  Gpio Ports

		gpio.mode[0]	= 0x00000030;	// Output Mode
		gpio.mode[1]	= 0x3fc00ff0;	// Output Mode
		gpio.mode[2]	= 0x00000500;	// Output Mode

		gpio.value[0]	= 0x00000030;	// Value High
		gpio.value[1]	= 0x3fc00ff0;	// Value High
		gpio.value[2]	= 0x00000500;	// Value High
	
		gpio.pullup[0]	= 0x00000030;	// pullup High
		gpio.pullup[1]	= 0x3fc00ff0;	// pullup High
		gpio.pullup[2]	= 0x00000500;	// pullup High	
		}
		
	ioctl (fd, SETGPIOINIT, &gpio);	// Init GPIO	
	close (fd);
	
	Multi_output_ioctl (repeat);
	Port_output_ioctl  (repeat);

	Multi_input_ioctl ();
	Port_input_ioctl ();

	return 0;
}	
//---------------------------------------------------------------------------------
void Multi_output_ioctl (int repeat)
{
int value[3];		// Port A,B,C 
int fd, no;
struct timeval tval;
unsigned int s_timer, e_timer;

	printf ("GPIO Multi output ioctl Function,  (Repeat=%d)\n", repeat);
	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) return;
			
	gettimeofday(&tval, NULL);
	s_timer = tval.tv_sec*1000000 + tval.tv_usec;

	for (no=0; no<repeat; no++)
		{
		if (Product_ID == EDDY_S4M)
			{
			value[0] = S4M_GPIO_MASK_PORT_A;
			value[1] = S4M_GPIO_MASK_PORT_B;
			value[2] = S4M_GPIO_MASK_PORT_C  | 0x00000003;
			}
		else
			{
			value[0] = GPIO_MASK_PORT_A;
			value[1] = GPIO_MASK_PORT_B;
			value[2] = GPIO_MASK_PORT_C;
			}	
		ioctl (fd, SETGPIOVAL_LM, &value[0]);;	// Port A,B,C, 	value High
		usleep (100000);

		value[0] = 0x00000000;
		value[1] = 0x00000000;
		value[2] = 0x00000000;
		ioctl (fd, SETGPIOVAL_LM, &value[0]);	// Port A,B,C, 	value Low
		usleep (100000);
		}
	close (fd);

	gettimeofday(&tval, NULL);
	e_timer = tval.tv_sec*1000000 + tval.tv_usec;	
	e_timer -= s_timer;
	printf ("\tProcess time = %d usec,   %d all/usec\n\n", e_timer, e_timer/repeat);
}
//---------------------------------------------------------------------------------
void Port_output_ioctl (int repeat)
{
int fd, no;
struct timeval tval;
unsigned int s_timer, e_timer;
int value;
	
	printf ("GPIO Port  output ioctl Function,  (Repeat=%d)\n", repeat);
	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) return;

	gettimeofday(&tval, NULL);
	s_timer = tval.tv_sec*1000000 + tval.tv_usec;

	for (no=0; no<repeat; no++)
		{
		value = 0x00000030;
		ioctl (fd, SETGPIOVAL_LA, &value);		// Port A.  value high
		value = 0x3fc00ff0;
		ioctl (fd, SETGPIOVAL_LB, &value);		// Port B,  value high
		value = 0x00000500;
		ioctl (fd, SETGPIOVAL_LC, &value);		// Port C,  value high
		
		value = 0x00000000;
		ioctl (fd, SETGPIOVAL_LA, &value);		// Port A,  value low
		value = 0x00000000;
		ioctl (fd, SETGPIOVAL_LB, &value);		// Port B,  value low
		value = 0x00000000;
		ioctl (fd, SETGPIOVAL_LC, &value);		// Port C,  value low
		}
	close (fd);
	
	gettimeofday(&tval, NULL);
	e_timer = tval.tv_sec*1000000 + tval.tv_usec;	
	e_timer -= s_timer;
	printf ("\tProcess time = %d usec,   %d port/usec\n\n", e_timer, e_timer/repeat);
}	
//---------------------------------------------------------------------------------
void Multi_input_ioctl ()
{
int value[3];		// Port A,B,C 
int fd;

	printf ("GPIO Multi Input ioctl Function\n");
	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) return;

	value[0] = 0x00000000;
	value[1] = 0x00000000;
	value[2] = 0x00000000;
	ioctl (fd, SETGPIOMOD_LM, &value[0]);	// Port A,B,C, 	Mode Input

	ioctl (fd, GETGPIOVAL_LM, &value[0]);	// Port A,B,C, 	Get Mode
	printf ("Get Value  A=%08x, B=%08x, C=%08x\n", value[0], value[1], value[2]);
	close (fd);
}
//---------------------------------------------------------------------------------
void Port_input_ioctl ()
{
int fd;
int value;
int A, B, C;
	
	printf ("\nGPIO Port Input ioctl Function\n");
	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) return;

	value = 0x00000000;
	ioctl (fd, SETGPIOMOD_LA, &value);	// Port A Mode Input
	value = 0x00000000;
	ioctl (fd, SETGPIOMOD_LB, &value);	// Port B Mode Input
	value = 0x00000000;
	ioctl (fd, SETGPIOMOD_LC, &value);	// Port C Mode Input

	ioctl (fd, GETGPIOVAL_LA, &A);		// Port A,  
	ioctl (fd, GETGPIOVAL_LB, &B);		// Port B,  
	ioctl (fd, GETGPIOVAL_LC, &C);		// Port C,  
	printf ("Get Value  A=%08x, B=%08x, C=%08x\n", A, B, C);
	close (fd);
}	



