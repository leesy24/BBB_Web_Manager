#include "sb_include.h"
#include "sb_define.h"
#include "sb_shared.h"
#include "sb_config.h"
#include "sb_extern.h"
#include "sb_ioctl.h"

#include <time.h>
#include <linux/rtc.h>
//=============================================================================
int main (int argc, char *argv[])
{
struct tm	rtctime;
int fd,ret;

	if(argc == 1)
		{
		printf("usage>\nSet RTC Time : test_rtc -s YYYY MM DD hh mm ss\n");
		printf("Get RTC Time : test_rtc -g\n");
		return 0;
		}
		
	if(!strcmp(argv[1], "-s"))
		{
		rtctime.tm_year	=	atoi(argv[2]) - 1900;
		rtctime.tm_mon	=	atoi(argv[3]) - 1;
		rtctime.tm_mday	=	atoi(argv[4]);
		rtctime.tm_hour	=	atoi(argv[5]);
		rtctime.tm_min	=	atoi(argv[6]);
		rtctime.tm_sec	=	atoi(argv[7]);
		
		fd = open("/dev/rtc0", O_RDONLY);
		if (fd < 0) 
			{
			printf ("RTC --> Open Error\n");
			return 0;
			}
		ioctl(fd, RTC_SET_TIME, &rtctime);
		close (fd);

		printf("Set ioctl RTC Time = %d-%d-%d, %02d:%02d:%02d\n",
				rtctime.tm_year + 1900, rtctime.tm_mon + 1, rtctime.tm_mday, 
				rtctime.tm_hour, rtctime.tm_min, rtctime.tm_sec);
		}
	
	if(!strcmp(argv[1], "-g"))
		{
		fd = open("/dev/rtc0", O_RDONLY);
		if (fd < 0) 
			{
			printf ("RTC --> Open Error\n");
			return 0;
			}
		ret = ioctl(fd, RTC_RD_TIME, &rtctime);
		close (fd);

		if (ret < 0) 
			{
			printf ("RTC --> ioctl Error !\n");
			return 0;
			}

		printf("Get ioctl RTC Time = %d-%d-%d, %02d:%02d:%02d\n",
				rtctime.tm_year + 1900, rtctime.tm_mon + 1, rtctime.tm_mday, 
				rtctime.tm_hour, rtctime.tm_min, rtctime.tm_sec);
		}

return 0;
}

/********************************************************************************************
Test Sample 
*********************************************************************************************

# date -s 010203042009.00						// date commamd    set time
Fri Jan  2 03:04:00 UTC 2009

# date	-u										// date command    get time
Fri Jan  2 03:04:00 UTC 2009

# test_rtc -g									// RTC test program   get time 
Get ioctl RTC Time = 2009-1-2, 03:04:00




# test_rtc -s 2009 3 4 5 6 7					// RTC test program   set time
Set ioctl RTC Time = 2009-3-4, 05:06:07

# date -u
Wed Mar  4 05:06:07 UTC 2009

# test_rtc -g
Get ioctl RTC Time = 2009-3-4, 05:06:07





# rdate -s time.bora.net						// Get time from remote Host    set time

# rdate -p time.bora.net
Thu Feb  5 06:05:25 2009

# date -u
Thu Feb  5 06:05:25 UTC 2009

# test_rtc -g
Get ioctl RTC Time = 2009-2-5, 06:05:25
# 

*******************************************************************************************/