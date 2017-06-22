#include "sb_include.h"
#include "sb_define.h"
#include "sb_config.h"
#include "sb_extern.h"


//---------------------------------------------------------------
int main(void)
{
	char cmd[1000];	
	int ret;
	FILE *fp;
	struct SB_SYSTEM_CONFIG		CFG;

	ret = SB_ReadConfig(CFGFILE_ETC_SYSTEM, (char *)&CFG, sizeof (struct SB_SYSTEM_CONFIG));
	if (ret < 0 || strncmp(SB_DEVICE_ID, CFG.id, 4))	// CFG not found or ID mismatch 
	{
		fprintf(stderr, "CFG not found or ID mismatch! %d, %4s !\n", ret, CFG.id);
		return -1;
	}

	sprintf (cmd, "/sbin/ifdown %s", SB_WAN_ETH_NAME);	// stop WAN network 
	fprintf(stderr, "system:%s\n", cmd);
	system (cmd);

	if ((fp = fopen("/etc/network/interfaces", "w")) != NULL)
	{
		fprintf(fp, "auto lo\n");
		fprintf(fp, "iface lo inet loopback\n");
		fprintf(fp, "\n");
		fprintf(fp, "auto %s\n", SB_WAN_ETH_NAME);
		if(CFG.line == 'I') // Static IP
		{
			fprintf(fp, "iface %s inet static\n", SB_WAN_ETH_NAME);
			fprintf(fp, "\taddress %d.%d.%d.%d\n", CFG.ip[0], CFG.ip[1],CFG.ip[2],CFG.ip[3]);
			fprintf(fp, "\tnetmask %d.%d.%d.%d\n", CFG.mask[0], CFG.mask[1],CFG.mask[2],CFG.mask[3]);
			fprintf(fp, "\tgateway %d.%d.%d.%d\n", CFG.gateway[0], CFG.gateway[1],CFG.gateway[2],CFG.gateway[3]);
			fprintf(fp, "\n");
			fprintf(fp, "iface %s:0 inet static\n", SB_WAN_ETH_NAME);
			fprintf(fp, "\taddress %d.%d.%d.%d\n", 10, 0, 10, 0);
			fprintf(fp, "\tnetmask %d.%d.%d.%d\n", 255, 255, 0, 0);
		}
		else // if(CFG.line == 'D') // DHCP
		{
			fprintf(fp, "iface %s inet dhcp\n", SB_WAN_ETH_NAME);
			fprintf(fp, "\n");
			fprintf(fp, "iface %s:0 inet static\n", SB_WAN_ETH_NAME);
			fprintf(fp, "\taddress %d.%d.%d.%d\n", 10, 0, 10, 0);
			fprintf(fp, "\tnetmask %d.%d.%d.%d\n", 255, 255, 0, 0);
		}
		fprintf(fp, "\n");
		fflush(fp);
		fclose(fp);
	}
	else
	{
		fprintf(stderr, "fopen error /etc/network/interfaces\n");
	}

	if ((fp = fopen("/etc/resolv.conf", "w")) != NULL)
	{
		if(CFG.line == 'I') // Static IP
		{
			if(CFG.dns[0] != 0 || CFG.dns[1] != 0 || CFG.dns[2] != 0 || CFG.dns[3] != 0)
			{
				fprintf(fp, "nameserver %d.%d.%d.%d\n", CFG.dns[0], CFG.dns[1], CFG.dns[2], CFG.dns[3]);
			}
			if(CFG.dns_s[0] != 0 || CFG.dns_s[1] != 0 || CFG.dns_s[2] != 0 || CFG.dns_s[3] != 0)
			{
				fprintf(fp, "nameserver %d.%d.%d.%d\n", CFG.dns_s[0], CFG.dns_s[1], CFG.dns_s[2], CFG.dns_s[3]);
			}
		}
		else // DHCP
		{
			// Nothing to do, because dhcp client will create this file.
		}
		fflush(fp);
		fclose(fp);
	}
	else
	{
		fprintf(stderr, "fopen error /etc/resolv.conf\n");
	}

	sprintf (cmd, "/sbin/ifup %s", SB_WAN_ETH_NAME);	// start WAN network 
	fprintf(stderr, "system:%s\n", cmd);
	system (cmd);

	return 0;
}
