#include <stdio.h>

#include <sys/sysinfo.h>

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
	
  printf("Uptime: %ddays, %dhours, %dminutes, %ldseconds\n",
	                *day, *hour, *min, *sec);
}



