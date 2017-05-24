#include <stdio.h>

#include <sys/time.h> // for gettimeofday()
#include <time.h> // for nanosleep()
#include <sys/sysinfo.h> // for sysinfo()

unsigned long SB_GetTick (Void)
{
	struct timeval tval;

	gettimeofday(&tval, NULL);
	return tval.tv_sec*1000 + tval.tv_usec/1000;
}

void SB_msleep (int msec) 
{
	struct timespec interval;

	interval.tv_sec  = msec / 1000;
	interval.tv_nsec = 1000000 * (msec % 1000);
	nanosleep (&interval, NULL);
}

void SB_AliveTime (int *day, int *hour, int *min, int *sec)
{
	struct sysinfo sys_info;

	if(sysinfo(&sys_info) != 0) {
		perror("sysinfo");
		return;
	}

	*day = sys_info.uptime / 86400;
	*hour = (sys_info.uptime / 3600) - (*day * 24);
	*min = (sys_info.uptime / 60) - (*day * 1440) - (*hour * 60);
	*sec = sys_info.uptime % 60;

	//printf("Uptime: %ddays, %dhours, %dminutes, %ldseconds\n", *day, *hour, *min, *sec);
}



