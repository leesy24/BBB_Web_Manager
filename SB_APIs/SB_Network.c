#include <stdio.h>

#include <string.h> /* for strncpy */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <limits.h>
#include <errno.h>
#include <resolv.h>

/* http://www.geekpage.jp/en/programming/linux-network/get-ipaddr.php */
unsigned int SB_GetIp (char *eth_name)
{
	int fd;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	/* I want to get an IPv4 IP address */
	ifr.ifr_addr.sa_family = AF_INET;

	/* I want IP address attached to eth_name */
	strncpy(ifr.ifr_name, eth_name, IFNAMSIZ-1);

	ioctl(fd, SIOCGIFADDR, &ifr);

	close(fd);

	/* return result */
	return (unsigned int)(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr);
}

/* http://www.geekpage.jp/en/programming/linux-network/get-netmask.php */
unsigned int SB_GetMask (char *eth_name)
{
	int fd;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	/* I want an IPv4 netmask */
	ifr.ifr_addr.sa_family = AF_INET;

	/* I want netmask attached to eth_name */
	strncpy(ifr.ifr_name, eth_name, IFNAMSIZ-1);

	ioctl(fd, SIOCGIFNETMASK, &ifr);

	close(fd);

	/* return result */
	return (unsigned int)(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr);
}

/* http://www.binarytides.com/get-local-ip-c-linux/
	 http://stackoverflow.com/questions/548105/default-gateway-in-c-on-linux */
unsigned int SB_GetGateway ()
{
	FILE *f;
	char line[100], *p, *c, *g, *saveptr;

	f = fopen("/proc/net/route", "r");

	while(fgets(line, sizeof(line), f))
	{
		p = strtok_r(line, " \t", &saveptr);
		c = strtok_r(NULL, " \t", &saveptr);
		g = strtok_r(NULL, " \t", &saveptr);

		if(p!=NULL && c!=NULL)
		{
			if(strcmp(c, "00000000") == 0)
			{
				//printf("Default interface is : %s \n" , p);
				if (g)
				{
					unsigned int ngw;
					char *endptr;
					
				  errno = 0;    /* To distinguish success/failure after call */
					ngw = strtol(g, &endptr, 16);
					
					/* Check for various possible errors */

					if ((errno == ERANGE && (ngw == LONG_MAX || ngw == LONG_MIN))
							|| (errno != 0 && ngw == 0)) {
						//perror("strtol");
						continue;
					}

					if (endptr == g) {
						//fprintf(stderr, "No digits were found\n");
						continue;
					}

					/* If we got here, strtol() successfully parsed a number */
					//printf("strtol() returned %ld\n", val);

#if 0
					if (*endptr != '\0')        /* Not necessarily an error... */
						printf("Further characters after number: %s\n", endptr);
#endif

					return ngw;
				}
			}
		}
	}
	return 0;
}

/*
	https://docstore.mik.ua/orelly/networking_2ndEd/dns/ch15_02.htm
	https://www.cyberciti.biz/faq/how-to-find-out-what-my-dns-servers-address-is/
	https://linux.die.net/man/3/resolver
*/
unsigned int SB_GetPrimaryDNS ()
{
	int ret;

	ret = res_init();
	if(ret < 0)
		return 0;

	//printf("_res.nscount = %d\n",_res.nscount);
	if(_res.nscount < 1)
		return 0;
	
	//printf("_res.nsaddr_list[0] = %08x, %s\n", _res.nsaddr_list[0].sin_addr, inet_ntoa(_res.nsaddr_list[0].sin_addr));
	return _res.nsaddr_list[0].sin_addr.s_addr;
}

unsigned int SB_GetSecondaryDNS ()
{
	int ret;

	ret = res_init();
	if(ret < 0)
		return 0;

	//printf("_res.nscount = %d\n",_res.nscount);
	if(_res.nscount < 2)
		return 0;
	
	//printf("_res.nsaddr_list[1] = %08x, %s\n", _res.nsaddr_list[1].sin_addr, inet_ntoa(_res.nsaddr_list[1].sin_addr));
	return _res.nsaddr_list[1].sin_addr.s_addr;
}


