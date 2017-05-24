/*****************************************************************************
Title		: WiFi Operation code

Description	: After Wifi configuration files that set in the WEB, Telnet is read, 
			  Wifi module can be operated.
Argments  1	: None
 ******************************************************************************/
#include "sb_include.h"
#include "sb_define.h"
#include "sb_shared.h"
#include "sb_config.h"
#include "sb_extern.h"
#include "sb_ioctl.h"
#include "sb_misc.h"

#define SB_WLAN_W8712_NAME			"wlan0"
//=============================================================================
int main(int argc, char *argv[])
{
struct SB_SYSTEM_CONFIG			CFG_SYS;
struct SB_WIFI_CONFIG			CFG_WIFI;
char cmd[256];
int fd;

	SB_msleep (4500);
	SB_ReadConfig  (CFGFILE_ETC_SYSTEM,  (char *)&CFG_SYS, 		sizeof (struct SB_SYSTEM_CONFIG));
	SB_ReadConfig  (CFGFILE_ETC_WIFI,    (char *)&CFG_WIFI,		sizeof(struct SB_WIFI_CONFIG));

	system("insmod  /etc/drivers/8712u.ko");	
	SB_msleep (2000);
	sprintf(cmd, "ifconfig %s up", SB_WLAN_W8712_NAME);
	system (cmd);
	SB_msleep(500);
	
	
	memset (cmd, 0x00, sizeof(cmd));
	sprintf(cmd, "/sbin/ifconfig | grep %s > /var/tmp/ifconfig", SB_WLAN_W8712_NAME);
	system (cmd);
	fd = open("/var/tmp/ifconfig", O_RDONLY);
	read(fd, cmd, sizeof(cmd));
	close(fd);
	if (strstr(cmd, SB_WLAN_W8712_NAME) == NULL) 
		{
		printf ("WiFi module not found!\n");	
		return 0;
		}
	
	if(CFG_WIFI.channel > 0)	// 0 = Auto
		{
		SB_msleep (10);
		//sprintf (cmd, "iwconfig %s channel %d", SB_WLAN_W8712_NAME, CFG_WIFI.channel);
		sprintf (cmd, "iwconfig %s channel %d", SB_WLAN_W8712_NAME, CFG_WIFI.channel);
		system (cmd);		
		}

	if(CFG_WIFI.bit_rate > 0)	// 0 = Auto
		{
		SB_msleep (10);
		//sprintf (cmd, "iwpriv %s set TxRate=%d", SB_WLAN_RA_NAME, CFG_WIFI.bit_rate);
		sprintf (cmd, "iwconfig %s rate %dM ", SB_WLAN_W8712_NAME, CFG_WIFI.bit_rate);		
		system (cmd);
		SB_LogMsgPrint("%s\n",cmd);
		}
	
	//frag
	if(CFG_WIFI.frag_thr > 256 && CFG_WIFI.frag_thr < 2347)
		{
		SB_msleep (10);
		sprintf (cmd, "iwconfig %s frag %d", SB_WLAN_W8712_NAME, CFG_WIFI.frag_thr);
		system (cmd);
		}
		
		
	//Start wpa_supplicant in the backgroud.
	sprintf(cmd,"wpa_supplicant -Dwext -i%s -c /etc/wpa.conf -B", SB_WLAN_W8712_NAME);
	system (cmd);
	
	
	system ("wpa_cli -p/var/run/wpa_supplicant remove_network 0");	//초기화
	if(CFG_WIFI.wifi_mode == SB_DISABLE)
		{
		system ("wpa_cli -p/var/run/wpa_supplicant ap_scan 1");
		}
		else
		{
		system ("wpa_cli -p/var/run/wpa_supplicant ap_scan 2");	
		}
	system ("wpa_cli -p/var/run/wpa_supplicant add_network");		//저장공간생성
	if(CFG_WIFI.wifi_mode == SB_ENABLE)	//AD_hoc
		{
		system ("wpa_cli -p/var/run/wpa_supplicant set_network 0 mode 1");
		}	
	sprintf(cmd,"wpa_cli -p/var/run/wpa_supplicant set_network 0 ssid \'\"%s\"\' ",CFG_WIFI.ssid);	//ssid
	system (cmd);

	if(CFG_WIFI.auth_mode > 2)
		{
		system ("wpa_cli -p/var/run/wpa_supplicant set_network 0 key_mgmt WPA-PSK");//WPAPSK	WPA2PSK
		if(CFG_WIFI.auth_mode == 4) system ("wpa_cli -p/var/run/wpa_supplicant set_network 0 proto WPA2");		//WPA2PSK를 지원하기 위해서 씀
		}
		else
		{
		system ("wpa_cli -p/var/run/wpa_supplicant set_network 0 key_mgmt NONE");	//AUTO	OPEN	SHARE
		switch (CFG_WIFI.auth_mode)
			{
			case 1 :
				system ("wpa_cli -p/var/run/wpa_supplicant set_network 0 auth_alg OPEN");
				break;
			case 2 :
				system ("wpa_cli -p/var/run/wpa_supplicant set_network 0 auth_alg SHARED");
				break;
			}
		}
	
	switch (CFG_WIFI.encryp_type)
		{
		case 1 : 
		case 2 :
			sprintf (cmd,"wpa_cli -p/var/run/wpa_supplicant set_network 0 wep_key0 \'\"%s\"\' ",CFG_WIFI.key);
			system(cmd);	
			sprintf (cmd,"wpa_cli -p/var/run/wpa_supplicant set_network 0 wep_key0 %s ",CFG_WIFI.key);			
			system(cmd);
			system ("wpa_cli -p/var/run/wpa_supplicant set_network 0 wep_tx_keyidx 0");			
			break;
		case 3 :
			system ("wpa_cli -p/var/run/wpa_supplicant set_network 0 pairwise TKIP");
			
		case 4 :
			if (CFG_WIFI.encryp_type == 4) 
				system ("wpa_cli -p/var/run/wpa_supplicant set_network 0 pairwise CCMP");
			
		case 5 :				
			sprintf (cmd, "wpa_cli -p/var/run/wpa_supplicant set_network 0 psk \'\"%s\"\' ",CFG_WIFI.key);
			system(cmd);
			break;			
		}
		
	


	system ("wpa_cli -p/var/run/wpa_supplicant select_network 0");//Selected
	
	//DEFAULT ROUTE정보를 삭제한다.
	//sprintf (cmd, "/sbin/route del default gw %d.%d.%d.%d dev %s",
	//	CFG_SYS.gateway[0], CFG_SYS.gateway[1],CFG_SYS.gateway[2],CFG_SYS.gateway[3],SB_WAN_ETH_NAME);
	//system (cmd);

	SB_msleep (3000);
	if(CFG_WIFI.line == 0)	//dhcp를 사용하면
		{
		if(CFG_WIFI.wifi_mode == SB_DISABLE)
			{
			sprintf (cmd, "/sbin/udhcpc -i %s &", SB_WLAN_W8712_NAME);
			system (cmd);
			}
		}
	else					//사용하지 않으면 
		{
		FILE *fp;
		sprintf (cmd, "ifconfig %s %d.%d.%d.%d netmask %d.%d.%d.%d", 
			SB_WLAN_W8712_NAME,
			CFG_WIFI.ip[0], 
			CFG_WIFI.ip[1], 
			CFG_WIFI.ip[2], 
			CFG_WIFI.ip[3], 
			CFG_WIFI.mask[0], 
			CFG_WIFI.mask[1], 
			CFG_WIFI.mask[2], 
			CFG_WIFI.mask[3]);
		system (cmd);
		
		if ((fp = fopen("/etc/resolv.conf", "a+")) != NULL)
			{
			fprintf(fp, "nameserver\t%d.%d.%d.%d\n",
			CFG_WIFI.dns[0],CFG_WIFI.dns[1],CFG_WIFI.dns[2],CFG_WIFI.dns[3]);
			fclose(fp);
			}	
		}
	
	//DEFAULT ROUTE정보를 입력한다.
	sprintf (cmd, "/sbin/route add default gw %d.%d.%d.%d dev %s",
			CFG_WIFI.gateway[0],CFG_WIFI.gateway[1],CFG_WIFI.gateway[2],CFG_WIFI.gateway[3],SB_WLAN_W8712_NAME);
	system (cmd);

	system ("echo  1 > /proc/sys/net/ipv4/ip_forward");				
	
	if(CFG_WIFI.wifi_mode == SB_ENABLE)
		{
		sprintf (cmd, "/usr/local/sbin/iptables -t nat -A POSTROUTING -s %d.%d.%d.0/%d.%d.%d.%d -j MASQUERADE", 
			CFG_SYS.ip[0], CFG_SYS.ip[1], CFG_SYS.ip[2],
			CFG_SYS.mask[0], CFG_SYS.mask[1], CFG_SYS.mask[2], CFG_SYS.mask[3]);
		system (cmd);			
		SB_msleep (2000);
		}
		
	return 0;
}
