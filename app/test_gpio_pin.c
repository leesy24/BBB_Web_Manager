#include "sb_include.h"
#include "sb_define.h"
#include "sb_shared.h"
#include "sb_config.h"
#include "sb_extern.h"
#include "sb_ioctl.h"
#include "sb_misc.h"

void default_gpio_set ();
void binary_print (char port, int t);
void binary_print_error (char port, int t, int value);
int test_port (int fd, char ID, int a, int b);

//========================================================================
int main (int argc, char *argv[])
{	
struct eddy_gpio_struct 
	{
    unsigned int value[3];
    unsigned int mode[3];
    unsigned int pullup[3];
    unsigned int enable[3];
	};
struct eddy_gpio_struct gpio;
int fd;


	system ("kt pinetd com detect udhcpd");

	printf ("\n--------------------------------------------\n");
	printf ("< GPIO Pin Test (S7=F:F:F:F:F:ON, S5=ON)  Press key continue..> ");
	getchar ();

	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0)
		{
		printf ("GPIO open error \n");
		return 0;
		}

	gpio.enable[0]	= 0x00400030 & GPIO_MASK_PORT_A;
	gpio.enable[1]	= 0xffffffff & GPIO_MASK_PORT_B;
	gpio.enable[2]	= 0x04ffd73f & GPIO_MASK_PORT_C;

	gpio.mode[0]	= 0x00000000;
	gpio.mode[1]	= 0x00000000;
	gpio.mode[2]	= 0x00000000;
	gpio.value[0]	= 0x00000000;
	gpio.value[1]	= 0x00000000;
	gpio.value[2]	= 0x00000000;	
	gpio.pullup[0]	= 0x00000000;	
	gpio.pullup[1]	= 0x00000000;	
	gpio.pullup[2]	= 0x00000000;	
	ioctl (fd, SETGPIOINIT, &gpio);	// Init GPIO	
	close (fd);

	gpio.enable[0]	= 0x00400000 & GPIO_MASK_PORT_A;	// Port A, Serial 2=2					Total 2  Gpio Ports
	gpio.enable[1]	= 0xc03f300f & GPIO_MASK_PORT_B;	// Port B, Serial 0=8, 1=4, 2=2, 3=2	Total 16 Gpio Ports
	gpio.enable[2]	= 0x04fe422f & GPIO_MASK_PORT_C;	// Port C, Serial 3=2					Total 2  Gpio Ports

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
//---------------------------------------------------------------------------------------------------------------


	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) return 0;


	if (test_port (fd, 'A', 4, 5)) printf ("Port(A)( 4, 5) -> OK\n"); else printf ("Port(A)( 4, 5) -> Error\n");
	if (test_port (fd, 'A', 4,22)) printf ("Port(A)( 4,22) -> OK\n"); else printf ("Port(A)( 4,22) -> Error\n");	
	if (test_port (fd, 'A', 5,22)) printf ("Port(A)( 5,22) -> OK\n"); else printf ("Port(A)( 5,22) -> Error\n");	
	if (test_port (fd, 'C', 0, 1)) printf ("Port(C)( 0, 1) -> OK\n"); else printf ("Port(C)( 0, 1) -> Error\n");
	if (test_port (fd, 'C', 2, 3)) printf ("Port(C)( 2, 3) -> OK\n"); else printf ("Port(C)( 2, 3) -> Error\n");
	if (test_port (fd, 'C', 4, 5)) printf ("Port(C)( 4, 5) -> OK\n"); else printf ("Port(C)( 4, 5) -> Error\n");
	if (test_port (fd, 'C', 8, 9)) printf ("Port(C)( 8, 9) -> OK\n"); else printf ("Port(C)( 8, 9) -> Error\n");
	if (test_port (fd, 'C',10,12)) printf ("Port(C)(10,12) -> OK\n"); else printf ("Port(C)(10,12) -> Error\n");
	if (test_port (fd, 'C',14,15)) printf ("Port(C)(14,15) -> OK\n"); else printf ("Port(C)(14,15) -> Error\n");
	if (test_port (fd, 'C',16,17)) printf ("Port(C)(16,17) -> OK\n"); else printf ("Port(C)(16,17) -> Error\n");
	if (test_port (fd, 'C',18,19)) printf ("Port(C)(18,19) -> OK\n"); else printf ("Port(C)(18,19) -> Error\n");
	if (test_port (fd, 'C',20,21)) printf ("Port(C)(20,21) -> OK\n"); else printf ("Port(C)(20,21) -> Error\n");
	if (test_port (fd, 'C',22,23)) printf ("Port(C)(22,23) -> OK\n"); else printf ("Port(C)(22,23) -> Error\n");
	if (test_port (fd, 'C',23,26)) printf ("Port(C)(23,26) -> OK\n"); else printf ("Port(C)(23,26) -> Error\n");
	if (test_port (fd, 'B', 0, 1)) printf ("Port(B)( 0, 1) -> OK\n"); else printf ("Port(B)( 0, 1) -> Error\n");
	if (test_port (fd, 'B', 2, 3)) printf ("Port(B)( 2, 3) -> OK\n"); else printf ("Port(B)( 2, 3) -> Error\n");
	if (test_port (fd, 'B', 4, 5)) printf ("Port(B)( 4, 5) -> OK\n"); else printf ("Port(B)( 4, 5) -> Error\n");
	if (test_port (fd, 'B', 6, 7)) printf ("Port(B)( 6, 7) -> OK\n"); else printf ("Port(B)( 6, 7) -> Error\n");
	if (test_port (fd, 'B', 8, 9)) printf ("Port(B)( 8, 9) -> OK\n"); else printf ("Port(B)( 8, 9) -> Error\n");
	if (test_port (fd, 'B',10,11)) printf ("Port(B)(10,11) -> OK\n"); else printf ("Port(B)(10,11) -> Error\n");
	if (test_port (fd, 'B',12,13)) printf ("Port(B)(12,13) -> OK\n"); else printf ("Port(B)(12,13) -> Error\n");
	if (test_port (fd, 'B',14,15)) printf ("Port(B)(14,15) -> OK\n"); else printf ("Port(B)(14,15) -> Error\n");
	if (test_port (fd, 'B',16,17)) printf ("Port(B)(16,17) -> OK\n"); else printf ("Port(B)(16,17) -> Error\n");
	if (test_port (fd, 'B',18,19)) printf ("Port(B)(18,19) -> OK\n"); else printf ("Port(B)(18,19) -> Error\n");
	if (test_port (fd, 'B',20,21)) printf ("Port(B)(20,21) -> OK\n"); else printf ("Port(B)(20,21) -> Error\n");
	if (test_port (fd, 'B',22,23)) printf ("Port(B)(22,23) -> OK\n"); else printf ("Port(B)(22,23) -> Error\n");
	if (test_port (fd, 'B',24,25)) printf ("Port(B)(24,25) -> OK\n"); else printf ("Port(B)(24,25) -> Error\n");
	if (test_port (fd, 'B',26,27)) printf ("Port(B)(26,27) -> OK\n"); else printf ("Port(B)(26,27) -> Error\n");
	if (test_port (fd, 'B',28,29)) printf ("Port(B)(28,29) -> OK\n"); else printf ("Port(B)(28,29) -> Error\n");
	if (test_port (fd, 'B',30,31)) printf ("Port(B)(30,31) -> OK\n"); else printf ("Port(B)(30,31) -> Error\n");

	close (fd);
	default_gpio_set ();
	return 0;
}
//==================================================================================
int test_port (int fd, char ID, int a, int b)
{
unsigned int SetMode=0, SetValue=0, GetValue=0;
unsigned int mode,value, val, set;

	switch (ID)	{
		case 'A' : 
			SetMode  = SETGPIOMOD_LA;
			SetValue = SETGPIOVAL_LA;
			GetValue = GETGPIOVAL_LA;
			break;
		case 'B' : 
			SetMode  = SETGPIOMOD_LB;
			SetValue = SETGPIOVAL_LB;
			GetValue = GETGPIOVAL_LB;
			break;
		case 'C' : 
			SetMode  = SETGPIOMOD_LC;
			SetValue = SETGPIOVAL_LC;
			GetValue = GETGPIOVAL_LC;
			break;
		}

	mode = set = 0x00000001 << a;
	val = (0x00000001 << b) | set;
	ioctl (fd, SetMode,  &mode);
	ioctl (fd, SetValue, &set);
	ioctl (fd, GetValue, &value);
//	printf ("mode=%x, set=%x, value=%x, val=%x, val&value=%x, \n", mode, set, value, val, value&val);
	if ((value&val) != val) return 0;

	set = 0x00000000;
	ioctl (fd, SetValue, &set);
	ioctl (fd, GetValue, &value);
//	printf ("mode=%x, set=%x, value=%x, val=%x, val&value=%x, \n", mode, set, value, val, value&val);
	if ((value&val) != 0x00000000) return 0;




	mode = set = 0x00000001 << b;
	val = (0x00000001 << a) | set;
	ioctl (fd, SetMode,  &mode);
	ioctl (fd, SetValue, &set);
	ioctl (fd, GetValue, &value);
//	printf ("mode=%x, set=%x, value=%x, val=%x, val&value=%x, \n", mode, set, value, val, value&val);
	if ((value&val) != val) return 0;
		
	set = 0x00000000;
	ioctl (fd, SetValue, &set);
	ioctl (fd, GetValue, &value);
//	printf ("mode=%x, set=%x, value=%x, val=%x, val&value=%x, \n", mode, set, value, val, value&val);	
	if ((value&set) != 0x00000000) return 0;
	return 1;
}

//==================================================================================
void binary_print (char port, int t)
{
int i, ttm;

	for (i=31; i>=0; i--)
		{
		ttm = t >> i;
		ttm &= 0x00000001;
		
		switch (port)	{
			case 'A' :
				if (i==22 || i==5 || i==4) printf ("%d", ttm);	else printf ("-");	break;
			case 'C' :
				if (i==31 || i==30 || i==29 || i==28 || i==27 || i==25 || i==24 || i==13 || i==11 || i==7 || i==6)
					printf ("-"); else printf ("%d", ttm);	break;	
			case 'B' :
				printf ("%d", ttm);	break;	
			}	
		
		if (i==28 || i==24 || i==20 || i==16 || i==12 || i==8 || i==4) printf (" ");
		}		
}			
//==================================================================================
void binary_print_error (char port, int t, int value)
{
int i, ttm, vvv;

	for (i=31; i>=0; i--)
		{
		ttm = t >> i;
		ttm &= 0x00000001;

		vvv = value >> i;
		vvv &= 0x00000001;
		
		switch (port)	
			{
			case 'A' :
				if (i==22 || i==5 || i==4) 
					if (ttm != vvv) 
						printf ("X");	else printf ("O");
				else 
					printf ("-");	break;
											
			case 'C' :
				if (i==31 || i==30 || i==29 || i==28 || i==27 || i==25 || i==24 || i==13 || i==11 || i==7 || i==6)
					if (ttm != vvv) 
						printf ("X");	else printf ("O");
				else 
					printf ("-");	break;

			case 'B' :
					if (ttm != vvv) 
						printf ("X");	else printf ("O");
			}	
		
		if (i==28 || i==24 || i==20 || i==16 || i==12 || i==8 || i==4) printf (" ");
		}
}
//====================================================================================
void default_gpio_set ()
{
struct eddy_gpio_struct
{
    unsigned int value[3];
    unsigned int mode[3];
    unsigned int pullup[3];
    unsigned int enable[3];
};
struct eddy_gpio_struct gpio;
int fd;

	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) 
		{
		printf ("GPIO open error \n");
		return ;
		}
		
	gpio.enable[0]	= 0x00400000 & GPIO_MASK_PORT_A;	// Port A, Serial 2=2					Total 2  Gpio Ports
	gpio.enable[1]	= 0xc03f300f & GPIO_MASK_PORT_B;	// Port B, Serial 0=8, 1=4, 2=2, 3=2	Total 16 Gpio Ports
	gpio.enable[2]	= 0x04fe422f & GPIO_MASK_PORT_C;	// Port C, Serial 3=2					Total 2  Gpio Ports

	gpio.mode[0]	= 0x00000000;	// Output Mode
	gpio.mode[1]	= 0x00000000;	// Output Mode
	gpio.mode[2]	= 0x00000000;	// Output Mode

	gpio.value[0]	= 0x00000000;	// Value High
	gpio.value[1]	= 0x00000000;	// Value High
	gpio.value[2]	= 0x00000000;	// Value High

	gpio.pullup[0]	= 0x00000000;	// pullup High
	gpio.pullup[1]	= 0x00000000;	// pullup High
	gpio.pullup[2]	= 0x00000000;	// pullup High
	ioctl (fd, SETGPIOINIT, &gpio);	// Init GPIO	
	close (fd);
}
