#include "sb_include.h"
#include "sb_define.h"
#include "sb_shared.h"
#include "sb_config.h"
#include "sb_extern.h"
#include "sb_ioctl.h"
#include "sb_misc.h"

struct at91_adc_value 
{
	int ch1_value;
	int ch2_value;
	int ch3_value;
	int ch4_value;
};

//========================================================================
int main (int argc, char *argv[])
{
int fd;
struct at91_adc_value CHANNEL;
int channel, ret;
int cnt=0;

	printf ("ADC Testing   (Break = Ctrl+C)\n");


	fd = open("/dev/adc", O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0) 
		{
		system ("insmod  /etc/drivers/eddy_adc.ko");		// /dev/adc		
		sleep (2);
		fd = open("/dev/adc", O_RDWR | O_NOCTTY | O_NDELAY);
		if (fd < 0) 
			{
			printf("ADC open error,   Check DK v2.1 board (S5:Off), WEB(Peripheral Settings->ADC:Eddy)\n");
			return (0);
			}
		}

	// ADC 4 channed   x x x x x x x x (bits)
	//						   | | | |-- channel  1 (temperature sensor)
	//						   | | |---- channel  2 (illumination sensor)
	//						   | |------ channel  3 
	//						   |-------- channel  4
	// if you want to use all channel, you should set 0x0f
	// if you want to use channel0 ~channel3, you should set 0x7.


	channel = 0x03;								// select 1,2 channel 
	ioctl (fd, ADCSETCHANNEL, &channel);		// set channel
	ret = ioctl (fd, ADCGETCHANNEL);			// get channel status
	printf("Channel Status = 0x%x \n",ret);

	while (1)
		{
		SB_msleep (100);
		ioctl (fd, ADCGETVALUE, &CHANNEL);
		printf("[%u]  (1:temperature)=%4d,  (2:illumination)=%4d,  3=%d,  4=%d\n", 
			++cnt,
			CHANNEL.ch1_value,
			CHANNEL.ch2_value,
			CHANNEL.ch3_value,
			CHANNEL.ch4_value);
		}

	close (fd);
	return (0);
}
