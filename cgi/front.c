/*flash 파일의 데이터를 가져와 임시파일에 저장한다..*/
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#include "icat.h"
#include "include/default.h"

struct ABOUT about;
struct WAN wan;
unsigned char device_name[32];
unsigned char device_version[16];


int getConfigData()
{
	int fd;
	int i;

	memset((char *)&about, 0, sizeof(about));
	memset((char *)&wan, 0, sizeof(wan));
	
	fd = open("/var/config/about.conf", O_RDONLY);
	
	if (fd<0)
	{
		Error("Can't open information file");
		close(fd);
		return 0;
	}
	else
	{
		if (read(fd, &about, sizeof(about)) < 0)
		{
			Error("Can't read from information file");
			close(fd);
			return 0;
		}
	}
	close(fd);
	
	fd = open("/var/config/wan.conf", O_RDONLY);
	
	if (fd<0)
	{
		Error("Can't open information file");
		close(fd);
		return 0;
	}
	else
	{
		if (read(fd, &wan, sizeof(wan)) < 0)
		{
			Error("Can't read from information file");
			close(fd);
			return 0;
		}
	}
	close(fd);
	return 1;
}

unsigned int get_myip(char ty)
{
		struct ifreq ifr;
		int fd;
		struct sockaddr_in *had, ha;

		if (ty == 'p')
			strcpy(ifr.ifr_name, "ppp0");
		else
			strcpy(ifr.ifr_name, "eth0");

		fd = socket(AF_INET, SOCK_DGRAM, 0);
		ioctl(fd, SIOCGIFADDR, &ifr);
		close(fd);

		had = &ha;
		memcpy(had, (struct sockaddr_in *)&ifr.ifr_addr, sizeof(struct sockaddr_in));

		return (unsigned int)(had->sin_addr.s_addr);
}

int cgiMain()
{
	char buff[256];
	LIST list;

	union { unsigned char c[4];  int i; } q;


	if (getConfigData() != 1)
		Error("Failure in getting configuration data");
	
	cgiHeaderContentType("text/html");
	list = listCreate();
	
	if (wan.mode == PPPOE)
		q.i = get_myip('p');
	else
		q.i = get_myip('e');

	listPutf(list, "front_name", about.device_name);
	listPutf(list, "front_version", about.device_version);
	listPutf(list, "front_ip","%d.%d.%d.%d", q.c[0],q.c[1],q.c[2],q.c[3]);

	sprintf(buff, "%s/start.htm", HTMLPATH);
	listDisplay(list, buff);
	fflush(0);
	listDelete(list);	
	
	return 0;
}
