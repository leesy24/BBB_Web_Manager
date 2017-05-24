/*****************************************************************************
Copyright (c) 2007 SystemBase Co., LTD  All Rights Reserved.

DDNS Manager 		09, 31, 2007. yhlee


DDNS Service Site :  http://ddns.nu
					 1. resister (username, password),  ex) SystemBase Eddy Account (eddy, 99999999)
					 2. maintain (register your devices)

******************************************************************************/
#include "sb_include.h"
#include "sb_define.h"
#include "sb_shared.h"
#include "sb_config.h"
#include "sb_extern.h"
#include "sb_ioctl.h"
#include "sb_misc.h"
//=============================================================================
int main(int argc, char *argv[])
{
struct SB_SYSTEM_CONFIG			cfg;
struct in_addr addr;
struct in_addr bak;
int cnt=0;
char tmp [200];

	SB_ReadConfig  (CFGFILE_ETC_SYSTEM,   (char *)&cfg, sizeof(struct SB_SYSTEM_CONFIG));		
	bak.s_addr = 0;
	
	while (1) 
		{
		sleep (10);
		if (++cnt > 10) 
			{
			bak.s_addr = 0l;
			cnt = 0;
			}	
		if (cfg.ddns[0] == 0x00) return 0;
		addr.s_addr = SB_GetIp (SB_WAN_ETH_NAME);
		if (addr.s_addr == bak.s_addr) continue;
		bak.s_addr = addr.s_addr;	
		sprintf (tmp, "/usr/sbin/ddns3 --host %d.%d.%d.%d  --user %s --pass %s  set eddy-%02x%02x%02x%02x %s", 
		cfg.ddns[0], cfg.ddns[1], cfg.ddns[2], cfg.ddns[3], 
		cfg.ddnsuser, cfg.ddnspass, 
		cfg.mac[2], cfg.mac[3], cfg.mac[4], cfg.mac[5],
		inet_ntoa(addr) );
		system (tmp);
//		printf ("%s\n", tmp);
		}
}

