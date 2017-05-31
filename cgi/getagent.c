#include "agent.h"
#include "sb_include.h"
#include "sb_define.h"
#include "sb_config.h"
#include "sb_extern.h"
#include "sb_ioctl.h"
#include "sb_misc.h"

#include <net/if.h>					// for "ifreq" structure
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>



#define	GPORT_A		0
#define	GPORT_B		1
#define	GPORT_C		2
#define	HEX_PA04	0x00000010
#define	HEX_PA05	0x00000020
#define	HEX_PA22	0x00400000
#define	HEX_PA30	0x40000000
#define	HEX_PB00	0x00000001
#define	HEX_PB01	0x00000002
#define	HEX_PB02	0x00000004
#define	HEX_PB03	0x00000008
#define	HEX_PB04	0x00000010
#define	HEX_PB05	0x00000020
#define	HEX_PB06	0x00000040
#define	HEX_PB07	0x00000080
#define	HEX_PB08	0x00000100
#define	HEX_PB09	0x00000200
#define	HEX_PB10	0x00000400
#define	HEX_PB11	0x00000800
#define	HEX_PB12	0x00001000
#define	HEX_PB13	0x00002000
#define	HEX_PB14	0x00004000
#define	HEX_PB15	0x00008000
#define	HEX_PB16	0x00010000
#define	HEX_PB17	0x00020000
#define	HEX_PB18	0x00040000
#define	HEX_PB19	0x00080000
#define	HEX_PB20	0x00100000
#define	HEX_PB21	0x00200000
#define	HEX_PB22	0x00400000
#define	HEX_PB23	0x00800000
#define	HEX_PB24	0x01000000
#define	HEX_PB25	0x02000000
#define	HEX_PB26	0x04000000
#define	HEX_PB27	0x08000000
#define	HEX_PB28	0x10000000
#define	HEX_PB29	0x20000000
#define	HEX_PB30	0x40000000
#define	HEX_PB31	0x80000000
#define	HEX_PC00	0x00000001
#define	HEX_PC01	0x00000002
#define	HEX_PC02	0x00000004
#define	HEX_PC03	0x00000008
#define	HEX_PC04	0x00000010
#define	HEX_PC05	0x00000020
#define	HEX_PC08	0x00000100
#define	HEX_PC09	0x00000200
#define	HEX_PC10	0x00000400
#define	HEX_PC12	0x00001000
#define	HEX_PC13	0x00002000
#define	HEX_PC14	0x00004000
#define	HEX_PC15	0x00008000
#define	HEX_PC16	0x00010000
#define	HEX_PC17	0x00020000
#define	HEX_PC18	0x00040000
#define	HEX_PC19	0x00080000
#define	HEX_PC20	0x00100000
#define	HEX_PC21	0x00200000
#define	HEX_PC22	0x00400000
#define	HEX_PC23	0x00800000
#define	HEX_PC24	0x01000000
#define	HEX_PC25	0x02000000
#define	HEX_PC26	0x04000000


LIST list;
void get_wireless();
void get_main();
void get_network();
void get_serial();
void get_gpio();
void get_admin();
void get_snmp();
void get_dio ();


int Product_ID;
//===========================================================================
int cgiMain()
{
struct	SB_SYSTEM_CONFIG	cfg;
	char passBuff[12];
	char buff[256];
	char target_page[20];
	char type;
	int i, fd;
	struct in_addr addr;

	SB_ReadConfig  (CFGFILE_ETC_SYSTEM,(char *)&cfg, sizeof (struct SB_SYSTEM_CONFIG));
	for (i=0; i<4; i++)
		buff[i] = cgiQueryString[i];
	buff[i] = '\0';

	Product_ID = SB_GetModel();

	if (!strcmp(buff, "type"))
		type = cgiQueryString[5];
	else
		Error("URL Parse Error");

	if (type != 's')
	{
		if (CheckLogin() == 0)	Error("Please login first!");
	}

	list = listCreate();

	strcpy(buff, cfg.device_name);
	listPutf(list, "device_name", buff);
	strcpy(buff, cfg.username);
	listPutf(list, "username", buff);

	if (Product_ID != EDDY_S4M)
		{
		listPutf(list,"S4M_START", "");
		listPutf(list,"S4M_END", "");
		}
	else
		{
		listPutf(list,"S4M_START", "<!--");
		listPutf(list,"S4M_END", "-->");
		}

#if 0
	if (cfg.mac[3] == 0x00 && cfg.mac[4] == 0x20 && cfg.mac[5] == 0x57)
		{
		listPutf(list,"WARNING_START", "");
		listPutf(list,"WARNING_END", "");
		} 
	else	
		{
		listPutf(list,"WARNING_START", "<!--");
		listPutf(list,"WARNING_END", "-->");
		}
#else
	listPutf(list,"WARNING_START", "<!--");
	listPutf(list,"WARNING_END", "-->");
#endif

	switch (type)
		{
		case 's':	// index.html -> start.html
			{
				char kver[20],fver[20],bver[20];
				SB_GetVersion ('F', fver);
				listPutf(list, "fw_version", fver);
				SB_GetVersion ('K', kver);
				listPutf(list, "kr_version", kver);
				SB_GetVersion ('B', bver);
				listPutf(list, "bl_version", bver);				
				strcpy(target_page, "start.html");
			} 
			break;

		case '1':	// main.html
			strcpy(target_page, "main.html");
			get_main();
			break;
		case '2':	// network.html
			strcpy(target_page, "network.html");
			get_network();
			break;
//		case '3':	// adc.html
//			strcpy(target_page, "adc.html");
//			get_adc();
//			break;
		case '4':	// gpio.html
			if (Product_ID == EDDY_S4M)
				strcpy(target_page, "s4m_gpio.html");
			else
				strcpy(target_page, "gpio.html");
			get_gpio();
			break;
		case '5':	// snmp.html
			strcpy(target_page, "snmp.html");
			get_snmp();
			break;
		case '6':	//  
			break;
		case '7':	// password.html
			strcpy(target_page, "password.html");
			break;
		case '8':	// update.html
			strcpy(target_page, "update.html");
			break;
		case '9':	// factory.html
			strcpy(target_page, "factory.html");
			break;
		case 'd':	// DIO.html
			strcpy(target_page, "dio.html");
			get_dio();
			break;
		case '0':	// reboot.html
			strcpy(target_page, "reboot.html");
			break;
		case 'A':	// serial.html
			strcpy(target_page, "serial.html");
			get_serial(0);
			break;
		case 'B':	// serial.html
			strcpy(target_page, "serial.html");
			get_serial(1);
			break;
		case 'C':	// serial.html
			strcpy(target_page, "serial.html");
			get_serial(2);
			break;
		case 'D':	// serial.html
			strcpy(target_page, "serial.html");
			get_serial(3);
			break;
		case 'E':	// serial.html
			strcpy(target_page, "serial.html");
			get_serial(4);
			break;
		case 'F':	// serial.html
			strcpy(target_page, "serial.html");
			get_serial(5);
			break;
		case 'G':	// serial.html
			strcpy(target_page, "serial.html");
			get_serial(6);
			break;
		case 'H':	// serial.html
			strcpy(target_page, "serial.html");
			get_serial(7);
			break;
		case 'J':	// wireless.html
			strcpy(target_page, "wireless.html");
			get_wireless();
			break;
		default:
			DebugMessage("Page doesn't exist!");
			break;
	}

	sprintf(buff, "%s/%s", HTMLPATH, target_page);
	listDisplay(list, buff);
	fflush(0);
	listDelete(list);
	return 0;
}
//---------------------------------------------------------------------------
void get_main()
{
	char buff[1024], Get_str[50];
	int fd;
	struct ifreq ifr;						
	struct sockaddr_in *pAddr;
	struct sockaddr maddr;
	char kver[20],fver[20],hver[20];
	struct in_addr addr;
	int day, hour, min, sec;
	struct SB_SYSTEM_CONFIG	cfg;
	char *point;
	char *temppoint;
	struct SB_WIFI_CONFIG	wifi_cfg;
	


	SB_ReadConfig(CFGFILE_ETC_WIFI, (char *)&wifi_cfg, sizeof(struct SB_WIFI_CONFIG));
	
	if (wifi_cfg.wireless)
		{
		int fd;
		
		listPutf(list, "WIRELESS_ON" , "");
		listPutf(list, "WIRELESS_OFF", "");
		
		// mcpanic 7613 eth2 -> ETH_NAME
		strcpy(ifr.ifr_name, "wlan0");
		fd = socket(AF_INET, SOCK_DGRAM,0);
	   	ioctl(fd, SIOCGIFHWADDR, &ifr);
	   	memcpy(&maddr, (struct sockaddr *)&ifr.ifr_hwaddr, sizeof(struct sockaddr));
			sprintf(buff, "%02X:%02X:%02X:%02X:%02X:%02X", 
			maddr.sa_data[0]&0xff, 
			maddr.sa_data[1]&0xff, 
			maddr.sa_data[2]&0xff, 
			maddr.sa_data[3]&0xff, 
			maddr.sa_data[4]&0xff, 
			maddr.sa_data[5]&0xff);
		listPutf(list, "wifi_macaddr", buff);

		system ("/sbin/iwlist wlan0 channel | grep Current > /var/tmp/iwlist");
		SB_ReadConfig ("/var/tmp/iwlist", buff, 1000);
		SB_getstring (buff, "Frequency:", 22, Get_str);
		listPutf(list, "wifi_frequency", Get_str);
		
		system ("/sbin/iwconfig wlan0 > /var/tmp/iwconfig");
		SB_ReadConfig ("/var/tmp/iwconfig", buff, 1000);

		SB_getstring (buff, "Quality=", 7, Get_str);
		listPutf(list, "wifi_linkquality", Get_str);

		SB_getstring (buff, "Rate:", 6, Get_str);
		listPutf(list, "wifi_bitrate", Get_str);

		SB_getstring (buff, "Signal level=", 7, Get_str);
		listPutf(list, "wifi_signallevel", Get_str);

		SB_getstring (buff, "Noise level=", 8, Get_str);
		listPutf(list, "wifi_noiselevel", Get_str);

		SB_getstring (buff, "Point: ", 17, Get_str);
		listPutf(list, "wifi_accesspoint", Get_str);

		system("/sbin/ifconfig wlan0 | grep addr: > /var/tmp/ifconfig");//rausb0
		fd = open("/var/tmp/ifconfig", O_RDONLY);
		read(fd, buff,sizeof(buff));
		close(fd);
		point = strstr(buff, "inet addr:");
		
		if (point != NULL && strcmp(Get_str, "Not-Associated   ") != 0)
			listPutf(list, "wifi_state", "Connection");
		else	
			{
			if (wifi_cfg.wifi_mode == 1)		// ad-hoc	
				listPutf(list, "wifi_state", "Connection");
			else	
				listPutf(list, "wifi_state", "No Connection");
			}
			
		if (wifi_cfg.line == 0)		// DHCP
			{
			listPutf(list, "wifi_line", "DHCP");
	
			if (point == NULL)
				{
				listPutf(list, "wifi_ip", "None");
				listPutf(list, "wifi_subnet", "None");
				listPutf(list, "wifi_gw", "None");
				}
			else
				{
				memset(buff,0,sizeof(buff));
				addr.s_addr = SB_GetIp ("wlan0");
				strcpy(buff, inet_ntoa(addr));
				listPutf(list, "wifi_ip", buff);
			
				memset(buff,0,sizeof(buff));
				addr.s_addr = SB_GetMask ("wlan0");
				strcpy(buff, inet_ntoa(addr));
				listPutf(list, "wifi_subnet", buff);
	
				memset(buff,0,sizeof(buff));
				addr.s_addr = SB_GetGateway ();
				strcpy(buff, inet_ntoa(addr));
				listPutf(list, "wifi_gw", buff);
				}
			}
		else
			{
			listPutf(list, "wifi_line", "Static IP");	
				
			memset(buff,0,sizeof(buff));
			sprintf(buff, "%d.%d.%d.%d",wifi_cfg.ip[0], wifi_cfg.ip[1],wifi_cfg.ip[2],wifi_cfg.ip[3]);
			listPutf(list, "wifi_ip", buff);
		
			memset(buff,0,sizeof(buff));
			sprintf(buff, "%d.%d.%d.%d",wifi_cfg.mask[0], wifi_cfg.mask[1],wifi_cfg.mask[2],wifi_cfg.mask[3]);
			listPutf(list, "wifi_subnet", buff);
		
			memset(buff,0,sizeof(buff));
			sprintf(buff, "%d.%d.%d.%d",wifi_cfg.gateway[0],wifi_cfg.gateway[1],wifi_cfg.gateway[2],wifi_cfg.gateway[3]);
			listPutf(list, "wifi_gw", buff);
			}


		}
	else
		{
		listPutf(list, "WIRELESS_ON" , "<!--");
		listPutf(list, "WIRELESS_OFF", "-->");
		}


	SB_ReadConfig  (CFGFILE_ETC_SYSTEM,     (char *)&cfg,		sizeof(struct SB_SYSTEM_CONFIG));			

	SB_GetVersion ('B', hver);
	SB_GetVersion ('K', kver);
	SB_GetVersion ('F', fver);
	listPutf(list, "bl_version", hver);
	listPutf(list, "kr_version", kver);
	listPutf(list, "fw_version", fver);

	sprintf(buff, "%02x:%02x:%02x:%02x:%02x:%02x", 
			cfg.mac[0], cfg.mac[1], cfg.mac[2],
			cfg.mac[3], cfg.mac[4], cfg.mac[5]);

	listPutf(list, "mac_addr", buff);

	if (cfg.line == 'I')
	{
		strcpy(buff, "Static IP");
		listPutf(list, "nw_line", buff);
		sprintf(buff, "%d.%d.%d.%d", cfg.ip[0], cfg.ip[1],cfg.ip[2],cfg.ip[3]);
		listPutf(list, "nw_ip", buff);
		sprintf(buff, "%d.%d.%d.%d", cfg.mask[0], cfg.mask[1],cfg.mask[2],cfg.mask[3]);
		listPutf(list, "nw_subnet", buff);
		sprintf(buff, "%d.%d.%d.%d", cfg.gateway[0], cfg.gateway[1],cfg.gateway[2],cfg.gateway[3]);
		listPutf(list, "nw_gw", buff);
	}
	else 
	{
		strcpy(buff, "DHCP");
		listPutf(list, "nw_line", buff);

		addr.s_addr = SB_GetIp (SB_WAN_ETH_NAME);
		strcpy(buff, inet_ntoa(addr));
		listPutf(list, "nw_ip", buff);

		addr.s_addr = SB_GetMask (SB_WAN_ETH_NAME);
		strcpy(buff, inet_ntoa(addr));
		listPutf(list, "nw_subnet", buff);

		addr.s_addr = SB_GetGateway ();
		strcpy(buff, inet_ntoa(addr));
		listPutf(list, "nw_gw", buff);
	}


	if (Product_ID != EDDY_S4M)
		sprintf (buff, "Eddy-DK");
	else
		sprintf (buff, "Eddy-S4M");
	
	listPutf(list, "device_type", buff);	
	strcpy(buff, cfg.website);
	listPutf(list, "website", buff);
	strcpy(buff, cfg.contact);
	listPutf(list, "contact", buff);
	SB_AliveTime (&day, &hour, &min, &sec);
	sprintf (buff, "(%d Days) %02d:%02d:%02d", day, hour, min, sec);
	listPutf(list, "alivetime", buff);
}
void get_wireless()
{
	int mode, value;
	char buff[255];
	int i;
	struct in_addr addr;
	struct SB_WIFI_CONFIG	wifi_cfg;

	SB_ReadConfig (CFGFILE_ETC_WIFI, (char *)&wifi_cfg, sizeof(struct SB_WIFI_CONFIG));

	cgiFormInteger("W_USBWIFI", &value, wifi_cfg.wireless);
	if (value == 1)
		listPutf(list, "w_usbwifi1", "selected");
	else
		{
		listPutf(list, "w_usbwifi0", "selected");
		listPutf(list,"wifi_all","disabled");
		}


	// Wireless Mode
	if ( cgiFormInteger("W_MODE", &mode, 0) != cgiFormSuccess )
		mode = wifi_cfg.wifi_mode;
	sprintf(buff,"w_mode%d",mode);
	listPutf(list, buff, "selected");
	
	// Wireless Network Mode
	if ( cgiFormInteger("W_NWMODE", &mode, 0) != cgiFormSuccess )
		mode = wifi_cfg.b_g_mode;
	sprintf(buff,"w_nwmode%d",mode);
	listPutf(list, buff, "selected");
	
	// SSID
	if ( cgiFormStringNoNewlines("W_SSID", buff, 32) == cgiFormNotFound )
		listPutf(list, "w_ssid", "%s", wifi_cfg.ssid);
	else
		listPutf(list, "w_ssid", "%s", buff);
	
	// Channel
	if ( cgiFormInteger("W_CHANNEL", &mode, 0) != cgiFormSuccess )
		mode = wifi_cfg.channel;
	sprintf(buff,"w_channel%d",mode);
	listPutf(list, buff, "selected");
	
	// Bitrate
	if ( cgiFormInteger("W_BITRATE", &mode, 0) != cgiFormSuccess )
		mode = wifi_cfg.bit_rate;
	sprintf(buff,"w_bitrate%d",mode);
	listPutf(list, buff, "selected");

	// RTS Threshold 
	if (cgiFormStringNoNewlines("W_RTSTHR", buff, 16) == cgiFormNotFound)
		listPutf(list, "w_rtsthr", "%d", wifi_cfg.rts_thr);
	else
		listPutf(list, "w_rtsthr", "%d", atoi(buff));
	
	// Fragment Threshold
	if (cgiFormStringNoNewlines("W_FRAGTHR", buff, 16) == cgiFormNotFound)
		listPutf(list, "w_fragthr", "%d", wifi_cfg.frag_thr);
	else
		listPutf(list, "w_fragthr", "%d", atoi(buff));
	
	// IEEE 802.11h
	if ( cgiFormInteger("W_IEEE80211H", &mode, 0) != cgiFormSuccess )
		mode = wifi_cfg.ieee80211h;
	sprintf(buff,"w_ieee80211h%d",mode);
	listPutf(list, buff, "selected");
	
	/**
	* shlee change for Roaming 20080827
	*/
	if (cgiFormInteger("W_ROAMING", &mode, 0) != cgiFormSuccess)
		mode = wifi_cfg.roaming_mode;
	
	sprintf(buff,"w_roaming%d",mode);
	listPutf(list, buff, "selected");

	switch(mode)
	{
	case 0:
		listPutf(list,"roamviewon","<!--");
		listPutf(list,"roamviewoff","-->");
		break;
	case 1:
		listPutf(list,"roamviewon","");
		listPutf(list,"roamviewoff","");
		break;
	}

	// Roaming Threshold 
	if (cgiFormStringNoNewlines("W_ROAMTHR", buff, 16) == cgiFormNotFound)
		listPutf(list, "w_roamthr", "%d", wifi_cfg.roaming_threshold);
	else
		listPutf(list, "w_roamthr", "%d", atoi(buff));

	// Wireless Security
	cgiFormInteger("W_AUTHMODE", &mode, wifi_cfg.auth_mode);
	sprintf(buff,"w_authmode%d",mode);
	listPutf(list, buff, "selected");

	cgiFormInteger("W_ENCRYPTYPE", &value, wifi_cfg.encryp_type);
			
	switch(mode)
	{
	case 0:	// AUTO
	case 1:	// OPEN
		listPutf(list,"OPENMODE_START"	,	"");
		listPutf(list,"SHAREMODE_START"	,	"");
		listPutf(list,"SHAREMODE_END"	,	"");
		listPutf(list,"OPENMODE_END"	,	"");
		listPutf(list,"WPAPSK_START"	,	"<!--");
		listPutf(list,"WPAPSK_END"		,	"-->");		
		if(value>2) value = 0;
		break;
	case 2:	// SHARE
		listPutf(list,"OPENMODE_START"	,	"");
		listPutf(list,"SHAREMODE_START"	,	"<!--");
		listPutf(list,"SHAREMODE_END"	,	"-->");
		listPutf(list,"OPENMODE_END"	,	"");
		listPutf(list,"WPAPSK_START"	,	"<!--");
		listPutf(list,"WPAPSK_END"		,	"-->");
		if(value==0 || value>2) value = 1;
		break;
	case 3:	// WPAPSK
	case 4:	// WPA2PSK
		listPutf(list,"OPENMODE_START"	,	"<!--");
		listPutf(list,"OPENMODE_END"	,	"-->");
		listPutf(list,"WPAPSK_START"	,	"");
		listPutf(list,"WPAPSK_END"		,	"");
		if(value<3) value=3;
		break;
	}

	sprintf(buff,"w_encryptype%d",value);
	listPutf(list, buff, "selected");
	
	switch(value)
		{
		case 0:				// none
			listPutf(list,"NETWORKKEYon"	,"<!--");
			listPutf(list,"NETWORKKEYoff"	,"-->");		
			break;
		case 1:				// wep40
			listPutf(list,"NETWORKKEYon"	,"");
			listPutf(list,"KEYLENGTH"		,"10");
			listPutf(list,"NETWORKKEYoff"	,"");		
			break;
		case 2:				// wep104
			listPutf(list,"NETWORKKEYon"	,"");
			listPutf(list,"KEYLENGTH"		,"26");
			listPutf(list,"NETWORKKEYoff"	,"");		
			break;
		case 3:				// aes
		case 4:
		case 5:
			listPutf(list,"NETWORKKEYon"	,"");
			listPutf(list,"KEYLENGTH"		,"38");
			listPutf(list,"NETWORKKEYoff"	,"");		
			break;
		}
		
	memset(buff,0,sizeof(buff));
	if (cgiFormStringNoNewlines("W_KEY1", buff, 38) == cgiFormNotFound)
		listPutf(list, "w_key1", wifi_cfg.key);
		else
		listPutf(list, "w_key1", buff);
	
	
	//Wireless Network Address
	cgiFormInteger("W_LINE", &mode, wifi_cfg.line);
	sprintf(buff,"w_line%d",mode);
	listPutf(list, buff, "selected");
	
	if (mode == 0)		// DHCP
	{
		memset(buff,0,sizeof(buff));
		listPutf(list, "w_option1", "disabled");
		addr.s_addr = SB_GetIp ("wlan0");
		strcpy(buff, inet_ntoa(addr));
		listPutf(list, "w_ip", buff);
		
		memset(buff,0,sizeof(buff));
		addr.s_addr = SB_GetMask ("wlan0");
		strcpy(buff, inet_ntoa(addr));
		listPutf(list, "w_mask", buff);

		memset(buff,0,sizeof(buff));
		addr.s_addr = SB_GetGateway ();
		strcpy(buff, inet_ntoa(addr));
		listPutf(list, "w_gw", buff);

		memset(buff,0,sizeof(buff));
		addr.s_addr = SB_GetPrimaryDNS();
		strcpy(buff, inet_ntoa(addr));
		listPutf(list, "w_dns", buff);

		memset(buff,0,sizeof(buff));
		addr.s_addr = SB_GetSecondaryDNS();
		strcpy(buff, inet_ntoa(addr));
		listPutf(list, "w_dns_s", buff);
	}
	else
	{
		memset(buff,0,sizeof(buff));
		if (cgiFormStringNoNewlines("W_IP", buff, 16) == cgiFormNotFound)
			sprintf(buff, "%d.%d.%d.%d",wifi_cfg.ip[0], wifi_cfg.ip[1],wifi_cfg.ip[2],wifi_cfg.ip[3]);
		listPutf(list, "w_ip", buff);
	
		memset(buff,0,sizeof(buff));
		if (cgiFormStringNoNewlines("W_MASK", buff, 16) == cgiFormNotFound)
		{	
			sprintf(buff, "%d.%d.%d.%d",wifi_cfg.mask[0], wifi_cfg.mask[1],wifi_cfg.mask[2],wifi_cfg.mask[3]);
			listPutf(list, "w_mask", buff);
		}
		else
			listPutf(list, "w_mask", buff);
	
		memset(buff,0,sizeof(buff));
		if (cgiFormStringNoNewlines("W_GW", buff, 16) == cgiFormNotFound)
		{	
			sprintf(buff, "%d.%d.%d.%d",wifi_cfg.gateway[0],wifi_cfg.gateway[1],wifi_cfg.gateway[2],wifi_cfg.gateway[3]);
			listPutf(list, "w_gw", buff);
		}
		else
			listPutf(list, "w_gw", buff);

		memset(buff,0,sizeof(buff));
		if (cgiFormStringNoNewlines("W_DNS", buff, 16) == cgiFormNotFound)
		{	
			sprintf(buff, "%d.%d.%d.%d",wifi_cfg.dns[0],wifi_cfg.dns[1],wifi_cfg.dns[2],wifi_cfg.dns[3]);
			listPutf(list, "w_dns", buff);
		}
		else
			listPutf(list, "w_dns", buff);

		memset(buff,0,sizeof(buff));
		if (cgiFormStringNoNewlines("W_DNS_S", buff, 16) == cgiFormNotFound)
		{	
			sprintf(buff, "%d.%d.%d.%d",wifi_cfg.dns_s[0],wifi_cfg.dns_s[1],wifi_cfg.dns_s[2],wifi_cfg.dns_s[3]);
			listPutf(list, "w_dns_s", buff);
		}
		else
			listPutf(list, "w_dns_s", buff);
	}
	
}
//---------------------------------------------------------------------------
void get_network()
{
	char buff[256];
	int value;
	int value2;
	struct in_addr addr;
struct SB_SYSTEM_CONFIG	cfg;
struct SB_GPIO_CONFIG	gpio;

	SB_ReadConfig  (CFGFILE_ETC_SYSTEM,     (char *)&cfg,		sizeof(struct SB_SYSTEM_CONFIG));			
	SB_ReadConfig  (CFGFILE_ETC_GPIO,       (char *)&gpio,		sizeof(struct SB_GPIO_CONFIG));				

#ifdef MIRHEE_FIX_DHCP_CLIENT	
	int fd;
	//		char szGateway[20];
	struct ifreq ifr;						
	struct sockaddr_in *pAddr;
#endif // MIRHEE_FIX_DHCP_CLIENT	

//=================WAN ========================
	if (cfg.line == 'I')
		value2 = 0;
	else
		value2 = 1;
	cgiFormInteger("N_LINE", &value, value2);
	if (value == 0)		// static
	{
		listPutf(list, "n_lines", "selected");

		if (cgiFormStringNoNewlines("N_IP", buff, 16) == cgiFormNotFound)
			sprintf(buff, "%d.%d.%d.%d",cfg.ip[0], cfg.ip[1],cfg.ip[2],cfg.ip[3]);

		listPutf(list, "n_ip", buff);

		if (cgiFormStringNoNewlines("N_MASK", buff, 16) == cgiFormNotFound)
		{	
			sprintf(buff, "%d.%d.%d.%d",cfg.mask[0], cfg.mask[1],cfg.mask[2],cfg.mask[3]);
			listPutf(list, "n_mask", buff);
		}
		else
			listPutf(list, "n_mask", buff);

		if (cgiFormStringNoNewlines("N_GW", buff, 16) == cgiFormNotFound)
		{	
			sprintf(buff, "%d.%d.%d.%d",cfg.gateway[0], cfg.gateway[1],cfg.gateway[2],cfg.gateway[3]);
			listPutf(list, "n_gw", buff);
		}
		else
			listPutf(list, "n_gw", buff);

		if (cgiFormStringNoNewlines("N_DNS", buff, 16) == cgiFormNotFound)
		{	
			sprintf(buff, "%d.%d.%d.%d",cfg.dns[0], cfg.dns[1],cfg.dns[2],cfg.dns[3]);
			listPutf(list, "n_dns", buff);
		}
		else	
			listPutf(list, "n_dns", buff);

		if (cgiFormStringNoNewlines("N_DNS_S", buff, 16) == cgiFormNotFound)
		{	
			sprintf(buff, "%d.%d.%d.%d",cfg.dns_s[0], cfg.dns_s[1],cfg.dns_s[2],cfg.dns_s[3]);
			listPutf(list, "n_dns_s", buff);
		}
		else	
			listPutf(list, "n_dns_s", buff);
	}
	else	// DHCP
	{
		listPutf(list, "n_lined", "selected");
		listPutf(list, "n_option1", "disabled");

		addr.s_addr = SB_GetIp (SB_WAN_ETH_NAME);
		strcpy(buff, inet_ntoa(addr));
		listPutf(list, "n_ip", buff);

		addr.s_addr = SB_GetMask (SB_WAN_ETH_NAME);
		strcpy(buff, inet_ntoa(addr));
		listPutf(list, "n_mask", buff);

		addr.s_addr = SB_GetGateway ();
		strcpy(buff, inet_ntoa(addr));
		listPutf(list, "n_gw", buff);

		addr.s_addr = SB_GetPrimaryDNS();
		strcpy(buff, inet_ntoa(addr));
		listPutf(list, "n_dns", buff);

		addr.s_addr = SB_GetSecondaryDNS();
		strcpy(buff, inet_ntoa(addr));
		listPutf(list, "n_dns_s", buff);
	}

//=================LAN ======================
	
	if (gpio.lan != 1) listPutf(list, "n_langpio", "disabled");	
	
	
	if (cfg.dhcpenable == 1)		// dhcpd
		value2 = 1;
	else
		value2 = 0;
		
	cgiFormInteger("N_DHCPENABLE", &value, value2);
	if (value == 0)		// static ip
		{
		listPutf(list, "n_dhcpdisable", "selected");
		listPutf(list, "n_option2", "disabled");		
		}
	else
		listPutf(list, "n_dhcpenable", "selected");

	if (cgiFormStringNoNewlines("N_STARTIP", buff, 16) == cgiFormNotFound)
		{	
		sprintf(buff, "%d.%d.%d.%d",cfg.dhcpstartIP[0], cfg.dhcpstartIP[1],cfg.dhcpstartIP[2],cfg.dhcpstartIP[3]);
		listPutf(list, "n_startip", buff);
		}
	else
		listPutf(list, "n_startip", buff);

	if (cgiFormStringNoNewlines("N_ENDIP", buff, 16) == cgiFormNotFound)
		{	
		sprintf(buff, "%d.%d.%d.%d",cfg.dhcpendIP[0], cfg.dhcpendIP[1],cfg.dhcpendIP[2],cfg.dhcpendIP[3]);
		listPutf(list, "n_endip", buff);
		}
	else
		listPutf(list, "n_endip", buff);

	if (cgiFormStringNoNewlines("N_LEASETIME", buff, 6) == cgiFormNotFound)
		listPutf(list, "n_leasetime", "%u", cfg.lease_time);
	else		
		listPutf(list, "n_leasetime", buff);

	if (cgiFormStringNoNewlines("N_LANIP", buff, 16) == cgiFormNotFound)
		{	
		sprintf(buff, "%d.%d.%d.%d", cfg.lanip[0], cfg.lanip[1],cfg.lanip[2],cfg.lanip[3]);
		listPutf(list, "n_lanip", buff);
		}
	else
		listPutf(list, "n_lanip", buff);

	if (cgiFormStringNoNewlines("N_LANMASK", buff, 16) == cgiFormNotFound)
		{	
		sprintf(buff, "%d.%d.%d.%d",cfg.lanmask[0], cfg.lanmask[1],cfg.lanmask[2],cfg.lanmask[3]);
		listPutf(list, "n_lanmask", buff);
		}
	else
		listPutf(list, "n_lanmask", buff);

//=================== service ===========================
	cgiFormInteger("N_TELNET", &value, cfg.telnet_server);
	if (value == 1)
		listPutf(list, "n_telnet_en", "selected");
	else
		listPutf(list, "n_telnet_di", "selected");
	cgiFormInteger("N_FTP", &value, cfg.ftp_server);
	if (value == 1)
		listPutf(list, "n_ftp_en", "selected");
	else
		listPutf(list, "n_ftp_di", "selected");

	cgiFormInteger("N_WEB", &value, cfg.web_server);
	if (value == 1)
		listPutf(list, "n_web_en", "selected");
	else
		listPutf(list, "n_web_di", "selected");

	cgiFormInteger("N_SSH", &value, cfg.ssh_server);
	if (value == 1)
		listPutf(list, "n_ssh_en", "selected");
	else
		listPutf(list, "n_ssh_di", "selected");

	cgiFormInteger("N_IDE", &value, cfg.target_agent);
	if (value == 1)
		listPutf(list, "n_ide_en", "selected");
	else
		listPutf(list, "n_ide_di", "selected");

	if (cgiFormStringNoNewlines("N_DDNS", buff, 16) == cgiFormNotFound)
	{	
		sprintf(buff, "%d.%d.%d.%d", cfg.ddns[0], cfg.ddns[1],cfg.ddns[2],cfg.ddns[3]);
		listPutf(list, "n_ddns", buff);
	}
	else	
		listPutf(list, "n_ddns", buff);

	if (cgiFormStringNoNewlines("N_DDNSUSER", buff, 16) == cgiFormNotFound)
	{	
		listPutf(list, "n_ddnsuser", cfg.ddnsuser);
	}
	else	
		listPutf(list, "n_ddnsuser", buff);

	if (cgiFormStringNoNewlines("N_DDNSPASS", buff, 16) == cgiFormNotFound)
	{	
		listPutf(list, "n_ddnspass", cfg.ddnspass);
	}
	else	
		listPutf(list, "n_ddnspass", buff);


	if (cgiFormStringNoNewlines("N_NIP", buff, 16) == cgiFormNotFound)
	{	
		sprintf(buff, "%d.%d.%d.%d",cfg.portview[0], cfg.portview[1],cfg.portview[2],cfg.portview[3]);
		listPutf(list, "n_nip", buff);
	}
	else
		listPutf(list, "n_nip", buff);

	// NMS Port
	if (cgiFormStringNoNewlines("N_NPORT", buff, 6) == cgiFormNotFound)
		listPutf(list, "n_nport", "%u", cfg.portview_port_no);
	else		
		listPutf(list, "n_nport", buff);

	// Device Name
	if (cgiFormStringNoNewlines("N_DNAME", buff, 32) == cgiFormNotFound)
		listPutf(list, "n_dname", cfg.device_name);
	else		
		listPutf(list, "n_dname", buff);

	/*	
	// OEM Functionality: edit device name
#ifdef EDDY_OEM_SUPPORT
listPutf(list, "n_oem", "");
#else
listPutf(list, "n_oem", "disabled");
#endif // EDDY_OEM_SUPPORT
	 */	
}
//---------------------------------------------------------------------------
void get_serial(int portno)
{
	struct SB_SIO_CONFIG			cfg  [SB_MAX_SIO_PORT];
	struct SB_GPIO_CONFIG			gpio;
	int value, value2, mode;
	char buff[256];
	char charval;
	int p_no;

	fprintf(stderr, "get_serial(portno = %d)\n", portno);

	SB_ReadConfig  (CFGFILE_ETC_SIO, (char *)&cfg[0],	sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);	
	SB_ReadConfig  (CFGFILE_ETC_GPIO,(char *)&gpio,		sizeof(struct SB_GPIO_CONFIG));	

	if(cgiFormInteger("PORTNUM",&p_no, 0) == cgiFormSuccess )
		portno = p_no - 1;
	fprintf(stderr, "portno = %d\n", portno);

	listPutf(list, "port_no", "%d", portno+1);

	if (portno < SB_MAX_SIO_PORT) 
	{
		listPutf(list,"RS232_START", "");
		listPutf(list,"RS232_END", "");
		listPutf(list,"RS422485_START", "<!--");
		listPutf(list,"RS422485_END", "-->");
		listPutf(list, "s_type_rs232","selected");
	}
	else	
	{
		listPutf(list,"RS232_START", "<!--");
		listPutf(list,"RS232_END", "-->");
		listPutf(list,"RS422485_START", "");
		listPutf(list,"RS422485_END", "");
		switch(cfg[portno].interface)
		{
			case SB_RS422_PTOP:
				listPutf(list, "s_type_rs422","selected");	break;
			case SB_RS485_NONE_ECHO:
				listPutf(list, "s_type_rs485n","selected");	break;
			case SB_RS485_ECHO:
				listPutf(list, "s_type_rs485e","selected");	break;	
		}	
	}	
	cgiFormInteger("IFTYPE", &mode, cfg[portno].interface);
	
	cgiFormInteger("OPMODE", &mode, cfg[portno].protocol);

	switch (portno)
	{
		case 0: if(gpio.serial_1 == SB_DISABLE) listPutf(list,"s_option", "disabled"); break;
		case 1: if(gpio.serial_2 == SB_DISABLE) listPutf(list,"s_option", "disabled"); break;
		case 2: if(gpio.serial_3 == SB_DISABLE) listPutf(list,"s_option", "disabled"); break;
		case 3: if(gpio.serial_4 == SB_DISABLE) listPutf(list,"s_option", "disabled"); break;
	}

	// Switch for Operation Mode
	switch(mode)
	{
		case SB_DISABLE_MODE:	// Disable
			listPutf(list,"s_op_dis","selected");
			listPutf(list,"s_option", "disabled");	
			break;
		case SB_COM_REDIRECT_MODE: // COM Redirect
			listPutf(list,"s_op_com","selected");
			listPutf(list, "s_remote_option", "disabled");
			break;
		case SB_TCP_SERVER_MODE: // TCP Server
			listPutf(list,"s_op_tcps","selected");
			listPutf(list, "s_remote_option", "disabled");
			break;
		case SB_TCP_CLIENT_MODE: // TCP Client
			listPutf(list,"s_op_tcpc","selected");
			break;
		case SB_TCP_BROADCAST_MODE: // TCP Broadcast
			listPutf(list,"s_op_tcpb","selected");
			listPutf(list, "s_remote_option", "disabled");
			break;
		case SB_TCP_MULTIPLEX_MODE: // TCP Multiplex
			listPutf(list,"s_op_tcpm","selected");
			listPutf(list, "s_remote_option", "disabled");
			break;
		case SB_UDP_SERVER_MODE: // UDP Server
			listPutf(list,"s_op_udps","selected");
			listPutf(list, "s_remote_option", "disabled");
			break;
		case SB_UDP_CLIENT_MODE: // UDP Client
			listPutf(list,"s_op_udpc","selected");
			break;
		default: // Default SB_DISABLE_MODE
			listPutf(list,"s_op_dis","selected");
			listPutf(list,"s_option", "disabled");	
			break;
	}


	// Local socket port
	if (cgiFormStringNoNewlines("LOCAL_PORT", buff, 6) == cgiFormNotFound)
		listPutf(list, "s_port", "%u", cfg[portno].socket_no);
	else
		listPutf(list, "s_port", "%u", atoi(buff));

	// Port Alias
	if (cgiFormStringNoNewlines("ALIAS", buff, 16) == cgiFormNotFound)
		listPutf(list, "s_alias", cfg[portno].name);
	else
		listPutf(list, "s_alias", buff);

	// baud rate            
	cgiFormInteger("BAUDRATE", &value, cfg[portno].speed);
	switch (value)
	{
		case 0:
			listPutf(list, "s_b150", "selected");
			break;
		case 1:
			listPutf(list, "s_b300", "selected");
			break;
		case 2:
			listPutf(list, "s_b600", "selected");
			break;
		case 3:
			listPutf(list, "s_b1200", "selected");
			break;
		case 4:
			listPutf(list, "s_b2400", "selected");
			break;
		case 5:
			listPutf(list, "s_b4800", "selected");
			break;
		case 6:
			listPutf(list, "s_b9600", "selected");
			break;
		case 7:
			listPutf(list, "s_b19200", "selected");
			break;
		case 8:
			listPutf(list, "s_b38400", "selected");
			break;
		case 9:
			listPutf(list, "s_b57600", "selected");
			break;
		case 10:
			listPutf(list, "s_b115200", "selected");
			break;
		case 11:
			listPutf(list, "s_b230400", "selected");
			break;
		case 12:
			listPutf(list, "s_b460800", "selected");
			break;
		case 13:
			listPutf(list, "s_b921600", "selected");
			break;
		default:
			listPutf(list, "s_b9600", "selected");
	}

	// data bits
	charval = cfg[portno].dps & 0x03;
	switch (charval)
	{
		case 0 :		value2 = 0;  break;
		case 1 :		value2 = 1;  break;
		case 2 :		value2 = 2;  break;
		case 3 :		value2 = 3;  break;
	}																

	cgiFormInteger("DATABIT", &value, value2);
	if (value == 0)
		listPutf(list, "s_d5", "selected");
	else 
		if (value == 1)
			listPutf(list, "s_d6", "selected");
		else 
			if (value == 2)
				listPutf(list, "s_d7", "selected");
			else
				listPutf(list, "s_d8", "selected");

	// stop bits			
	charval = cfg[portno].dps & 0x04;
	if (charval != 0x04)
		value2 = 0;
	else
		value2 = 1;

	cgiFormInteger("STOPBIT", &value, value2);
	if (value == 0)
		listPutf(list, "s_s1", "selected");
	else
		listPutf(list, "s_s2", "selected");
	// parity			
	charval = cfg[portno].dps & 0x18;
	if (charval == 0x00) value2 = 0;
	if (charval == 0x08) value2 = 1;
	if (charval == 0x10 || charval == 0x18)  value2 = 2;

	cgiFormInteger("PARITY", &value, value2);
	if (value == 0)
		listPutf(list, "s_pnone", "selected");
	else 
		if (value == 1)
			listPutf(list, "s_podd", "selected");
		else
			listPutf(list, "s_peven", "selected");

	// flow control
	value2 = cfg[portno].flow;
	cgiFormInteger("FLOW", &value, value2);
	if (value == 0)
		listPutf(list, "s_fnone", "selected");
	else 
		if (value == 1)
			listPutf(list, "s_fhw", "selected");
		else
			listPutf(list, "s_fsw", "selected");

	// signal (device type)	

	value2 = cfg[portno].device;
	cgiFormInteger("DEVICETYPE", &value, value2);
	if (value == 0)
		listPutf(list, "s_ddata", "selected");
	else
		listPutf(list, "s_dmodem", "selected");

	// Remote IP Address / Port	
	if (cgiFormStringNoNewlines("REMOTE_IP", buff, 16) == cgiFormNotFound)
	{	
		sprintf(buff, "%d.%d.%d.%d",cfg[portno].remote_ip[0], cfg[portno].remote_ip[1],cfg[portno].remote_ip[2],cfg[portno].remote_ip[3]);
		listPutf(list, "s_rip", buff);
	}
	else
		listPutf(list, "s_rip", buff);

	if ( cgiFormStringNoNewlines("REMOTE_PORT", buff, 6) == cgiFormNotFound )
		listPutf(list, "s_rport", "%u", cfg[portno].remote_socket_no);
	else
		listPutf(list, "s_rport", buff);	

	// Keepalive
	if ( cgiFormStringNoNewlines("ALIVE_TIME", buff, 6) == cgiFormNotFound )
		listPutf(list, "s_alive", "%u", cfg[portno].keepalive);
	else
		listPutf(list, "s_alive", buff);	

	// Latency
	if ( cgiFormStringNoNewlines("LATENCY_TIME", buff, 6) == cgiFormNotFound )
		listPutf(list, "s_latency", "%u", cfg[portno].packet_latency_time);
	else
		listPutf(list, "s_latency", buff);	


	// Port Login	
	value2 = cfg[portno].login;
	cgiFormInteger("PASSIVELOGIN", &value, value2);
	if (value == 0)
		{
		listPutf(list, "s_pdisable", "selected");
		listPutf(list, "s_passive_option", "disabled");
		}
	else
		{
		listPutf(list, "s_penable", "selected");
		}
		
	if ( cgiFormStringNoNewlines("PASSIVE_USER", buff, 16) == cgiFormNotFound )
		listPutf(list, "s_puser", "%s", cfg[portno].login_name);
	else
		listPutf(list, "s_puser", "%s", buff);

	memset(buff,0,sizeof(buff));
	if ( cgiFormStringNoNewlines("PASSIVE_PASS", buff, 16) == cgiFormNotFound )
		listPutf(list, "s_ppass", "%s", cfg[portno].login_password);
	else
		listPutf(list, "s_ppass", "%s", buff);
}
/*
//---------------------------------------------------------------------------
void get_adc ()
{
int value, value2;
char buff[256];
struct SB_ADC_CONFIG	cfg;
struct SB_GPIO_CONFIG	gpio;

	SB_ReadConfig  (CFGFILE_ETC_ADC,     (char *)&cfg,		sizeof(struct SB_ADC_CONFIG));		
	SB_ReadConfig  (CFGFILE_ETC_GPIO,    (char *)&gpio,		sizeof(struct SB_GPIO_CONFIG));		
	

	if (gpio.adc != 1) listPutf(list, "s_option", "disabled");

	cgiFormInteger("N_ENABLE", &value, cfg.enable);
	if (value == 0)
		{
		listPutf(list, "adcdis", "selected");
		listPutf(list, "s_option", "disabled");
		}
	else
		listPutf(list, "adcena", "selected");

	// Local socket port
	if (cgiFormStringNoNewlines("LOCALPORT", buff, 6) == cgiFormNotFound)
		listPutf(list, "localport", "%u", cfg.socket_no);
	else
		listPutf(list, "localport", "%u", atoi(buff));

	// interval
	if ( cgiFormStringNoNewlines("INTERVAL", buff, 6) == cgiFormNotFound)
		listPutf(list, "interval", "%u", cfg.interval);
	else
		listPutf(list, "interval", buff);	

	// Keepalive
	if ( cgiFormStringNoNewlines("KEEPALIVE", buff, 6) == cgiFormNotFound)
		listPutf(list, "keepalive", "%u", cfg.keepalive);
	else
		listPutf(list, "keepalive", buff);	


	// channel 1	
	value2 = cfg.channel & 0x01;
	cgiFormInteger("CHANNEL1", &value, value2);
	if (value == 0)
		listPutf(list, "ch1dis", "selected");
	else
		listPutf(list, "ch1ena", "selected");

	// channel 2	
	value2 = cfg.channel & 0x02;
	cgiFormInteger("CHANNEL2", &value, value2);
	if (value == 0)
		listPutf(list, "ch2dis", "selected");
	else
		listPutf(list, "ch2ena", "selected");
		
	// channel 3
	value2 = cfg.channel & 0x04;
	cgiFormInteger("CHANNEL3", &value, value2);
	if (value == 0)
		listPutf(list, "ch3dis", "selected");
	else
		listPutf(list, "ch3ena", "selected");
	
	// channel 4
	value2 = cfg.channel & 0x08;
	cgiFormInteger("CHANNEL4", &value, value2);
	if (value == 0)
		listPutf(list, "ch4dis", "selected");
	else
		listPutf(list, "ch4ena", "selected");
}
*/
//---------------------------------------------------------------------------
void get_gpio()
{
int value, cpu=0, value2, gv, gm, gg;
struct SB_GPIO_CONFIG	cfg;

	SB_ReadConfig  (CFGFILE_ETC_GPIO,     (char *)&cfg,		sizeof(struct SB_GPIO_CONFIG));		
	

	if (Product_ID != EDDY_S4M)
		{
		cgiFormInteger("SIO1", &value, cfg.serial_1);
		switch (value)	{
			case 0 :			// GPIO
				listPutf(list, "sio1_0", "selected");  	break;
			case 1 : 			// eddy
				listPutf(list, "sio1_1", "selected");	listPutf(list, "hide_sio1", "disabled"); break;		
			case 2 :			// User
				listPutf(list, "sio1_2", "selected");	listPutf(list, "hide_sio1", "disabled"); break;	}
		cgiFormInteger("SIO2", &value, cfg.serial_2);
		switch (value)	{
			case 0 :			// GPIO
				listPutf(list, "sio2_0", "selected");  	break;
			case 1 : 			// eddy
				listPutf(list, "sio2_1", "selected");	listPutf(list, "hide_sio2", "disabled"); break;		
			case 2 :			// User
				listPutf(list, "sio2_2", "selected");	listPutf(list, "hide_sio2", "disabled"); break;	}
		cgiFormInteger("SIO3", &value, cfg.serial_3);
		switch (value)	{
			case 0 :			// GPIO
				listPutf(list, "sio3_0", "selected");  	break;
			case 1 : 			// eddy
				listPutf(list, "sio3_1", "selected");	listPutf(list, "hide_sio3", "disabled"); break;		
			case 2 :			// User
				listPutf(list, "sio3_2", "selected");	listPutf(list, "hide_sio3", "disabled"); break;	}
		cgiFormInteger("SIO4", &value, cfg.serial_4);
		switch (value)	{
			case 0 :			// GPIO
				listPutf(list, "sio4_0", "selected");  	break;
			case 1 : 			// eddy
				listPutf(list, "sio4_1", "selected");	listPutf(list, "hide_sio4", "disabled"); break;		
			case 2 :			// User
				listPutf(list, "sio4_2", "selected");	listPutf(list, "hide_sio4", "disabled"); break;	}
	
		cgiFormInteger("DEBUGPORT", &value, cfg.debugport);
		switch (value)	{
			case 0 :			// GPIO
				listPutf(list, "debugport_0", "selected");  	break;
			case 1 :			// eddy
				listPutf(list, "debugport_1", "selected");	listPutf(list, "hide_debugport", "disabled"); break;					
			case 2 : 			// user
				listPutf(list, "debugport_2", "selected");	listPutf(list, "hide_debugport", "disabled"); break;	}
	
		cgiFormInteger("ADC", &value, cfg.adc);
		switch (value)	{
			case 0 :			// GPIO
				listPutf(list, "adc_0", "selected");  	break;
			case 1 : 			// eddy
				listPutf(list, "adc_1", "selected");	listPutf(list, "hide_adc", "disabled"); break;		
			case 2 :			// User
				listPutf(list, "adc_2", "selected");	listPutf(list, "hide_adc", "disabled"); break;	}
	
		cgiFormInteger("LAN", &value, cfg.lan);
		switch (value)	{
			case 0 :			// GPIO
				listPutf(list, "lan_0", "selected");  	break;
			case 1 : 			// eddy
				listPutf(list, "lan_1", "selected");	listPutf(list, "hide_lan", "disabled"); break;		
			case 2 :			// User
				listPutf(list, "lan_2", "selected");	listPutf(list, "hide_lan", "disabled"); break;	}
		
		cgiFormInteger("RESET", &value, cfg.reset);
		switch (value)	{
			case 0 :			// GPIO
				listPutf(list, "reset_0", "selected");  	break;
			case 1 : 			// eddy
				listPutf(list, "reset_1", "selected");	listPutf(list, "hide_reset", "disabled"); break;		
			case 2 :			// User
				listPutf(list, "reset_2", "selected");	listPutf(list, "hide_reset", "disabled"); break;	}
	
		cgiFormInteger("RDYLED", &value, cfg.rdy_led);
		switch (value)	{
			case 0 :			// GPIO
				listPutf(list, "rdyled_0", "selected");  	break;
			case 1 : 			// eddy
				listPutf(list, "rdyled_1", "selected");	listPutf(list, "hide_rdyled", "disabled"); break;		
			case 2 :			// User
				listPutf(list, "rdyled_2", "selected");	listPutf(list, "hide_rdyled", "disabled"); break;	}
	
		cgiFormInteger("EEPROM", &value, cfg.spi_eeprom);
		switch (value)	{
			case 0 :			// GPIO
				listPutf(list, "eeprom_0", "selected");  	break;
			case 1 : 			// eddy
				listPutf(list, "eeprom_1", "selected");	listPutf(list, "hide_eeprom", "disabled"); break;		
			case 2 :			// User
				listPutf(list, "eeprom_2", "selected");	listPutf(list, "hide_eeprom", "disabled"); break;	}
	
		cgiFormInteger("NAND", &value, cfg.nandflash);
		switch (value)	{
			case 0 :			// GPIO
				listPutf(list, "nand_0", "selected");  	break;
			case 1 : 			// eddy
				listPutf(list, "nand_1", "selected");	listPutf(list, "hide_nand", "disabled"); break;		
			case 2 :			// User
				listPutf(list, "nand_2", "selected");	listPutf(list, "hide_nand", "disabled"); break;	}
	
		cgiFormInteger("KEYPAD", &value, cfg.keypad);
		switch (value)	{
			case 0 :			// GPIO
				listPutf(list, "keypad_0", "selected");  	break;
			case 1 : 			// eddy
				listPutf(list, "keypad_1", "selected");	listPutf(list, "hide_keypad", "disabled"); break;		
			case 2 :			// User
				listPutf(list, "keypad_2", "selected");	listPutf(list, "hide_keypad", "disabled"); break;	}
	
		if (cfg.enable[GPORT_A] & HEX_PA22) gg = 1; else gg = 0;
		cgiFormInteger("PA22EN", &value, gg);
		if (value==1)	listPutf(list, "PA22EN_1", "selected");	else 
			{ listPutf(list, "PA22EN_0", "selected");	listPutf(list, "hide_PA22EN", "disabled"); }	
		if (cfg.enable[GPORT_C] & HEX_PC09) gg = 1; else gg = 0;
		cgiFormInteger("PC09EN", &value, gg);
		if (value==1)	listPutf(list, "PC09EN_1", "selected");	else 
			{ listPutf(list, "PC09EN_0", "selected");	listPutf(list, "hide_PC09EN", "disabled"); }
		if (cfg.enable[GPORT_B] & HEX_PB12) gg = 1; else gg = 0;
		cgiFormInteger("PB12EN", &value, gg);
		if (value==1)	listPutf(list, "PB12EN_1", "selected");	else 
			{ listPutf(list, "PB12EN_0", "selected");	listPutf(list, "hide_PB12EN", "disabled"); }
		if (cfg.enable[GPORT_B] & HEX_PB13) gg = 1; else gg = 0;
		cgiFormInteger("PB13EN", &value, gg);
		if (value==1)	listPutf(list, "PB13EN_1", "selected");	else 
			{ listPutf(list, "PB13EN_0", "selected");	listPutf(list, "hide_PB13EN", "disabled"); }
	
		if (cfg.enable[GPORT_B] & HEX_PB16) gg = 1; else gg = 0;
		cgiFormInteger("PB16EN", &value, gg);
		if (value==1)	listPutf(list, "PB16EN_1", "selected");	else 
			{ listPutf(list, "PB16EN_0", "selected");	listPutf(list, "hide_PB16EN", "disabled"); }
	
		if (cfg.enable[GPORT_B] & HEX_PB17) gg = 1; else gg = 0;
		cgiFormInteger("PB17EN", &value, gg);
		if (value==1)	listPutf(list, "PB17EN_1", "selected");	else 
			{ listPutf(list, "PB17EN_0", "selected");	listPutf(list, "hide_PB17EN", "disabled"); }
	
		if (cfg.enable[GPORT_B] & HEX_PB18) gg = 1; else gg = 0;
		cgiFormInteger("PB18EN", &value, gg);
		if (value==1)	listPutf(list, "PB18EN_1", "selected");	else 
			{ listPutf(list, "PB18EN_0", "selected");	listPutf(list, "hide_PB18EN", "disabled"); }
		if (cfg.enable[GPORT_B] & HEX_PB19) gg = 1; else gg = 0;
		cgiFormInteger("PB19EN", &value, gg);
		if (value==1)	listPutf(list, "PB19EN_1", "selected");	else 
			{ listPutf(list, "PB19EN_0", "selected");	listPutf(list, "hide_PB19EN", "disabled"); }
	
		if (cfg.enable[GPORT_C] & HEX_PC05) gg = 1; else gg = 0;
		cgiFormInteger("PC05EN", &value, gg);
		if (value==1)	listPutf(list, "PC05EN_1", "selected");	else 
			{ listPutf(list, "PC05EN_0", "selected");	listPutf(list, "hide_PC05EN", "disabled"); }
	
		if (cfg.enable[GPORT_C] & HEX_PC18) gg = 1; else gg = 0;
		cgiFormInteger("PC18EN", &value, gg);
		if (value==1)	listPutf(list, "PC18EN_1", "selected");	else 
			{ listPutf(list, "PC18EN_0", "selected");	listPutf(list, "hide_PC18EN", "disabled"); }
	
		if (cfg.enable[GPORT_C] & HEX_PC19) gg = 1; else gg = 0;
		cgiFormInteger("PC19EN", &value, gg);
		if (value==1)	listPutf(list, "PC19EN_1", "selected");	else 
			{ listPutf(list, "PC19EN_0", "selected");	listPutf(list, "hide_PC19EN", "disabled"); }
	
		if (cfg.enable[GPORT_C] & HEX_PC26) gg = 1; else gg = 0;
		cgiFormInteger("PC26EN", &value, gg);
		if (value==1)	listPutf(list, "PC26EN_1", "selected");	else 
			{ listPutf(list, "PC26EN_0", "selected");	listPutf(list, "hide_PC26EN", "disabled"); }
	
		//...................................................................
		if (cfg.mode[GPORT_A] & HEX_PA04)
			if (cfg.value[GPORT_A] & HEX_PA04) gg = 3; else gg = 2;	// output			
		else 
			if (cfg.pullup[GPORT_A] & HEX_PA04) gg = 0; else gg = 1;	// input
		cgiFormInteger("PA04", &value, gg);  
		if (value == 0)	listPutf(list, "PA04_0", "selected"); else  if (value == 1) listPutf(list, "PA04_1", "selected"); else if (value == 2) listPutf(list, "PA04_2", "selected"); else listPutf(list, "PA04_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_A] & HEX_PA05)
			if (cfg.value[GPORT_A] & HEX_PA05) gg = 3; else gg = 2;	// output			
		else 
			if (cfg.pullup[GPORT_A] & HEX_PA05) gg = 0; else gg = 1;	// input
		cgiFormInteger("PA05", &value, gg);  
		if (value == 0)	listPutf(list, "PA05_0", "selected"); else  if (value == 1) listPutf(list, "PA05_1", "selected"); else if (value == 2) listPutf(list, "PA05_2", "selected"); else listPutf(list, "PA05_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_A] & HEX_PA22)
			if (cfg.value[GPORT_A] & HEX_PA22) gg = 3; else gg = 2;	// output			
		else 
			if (cfg.pullup[GPORT_A] & HEX_PA22) gg = 0; else gg = 1;	// input
		cgiFormInteger("PA22", &value, gg);  
		if (value == 0)	listPutf(list, "PA22_0", "selected"); else  if (value == 1) listPutf(list, "PA22_1", "selected"); else if (value == 2) listPutf(list, "PA22_2", "selected"); else listPutf(list, "PA22_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB00)
			if (cfg.value[GPORT_B] & HEX_PB00) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB00) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB00", &value, gg);  
		if (value == 0)	listPutf(list, "PB00_0", "selected"); else  if (value == 1) listPutf(list, "PB00_1", "selected"); else if (value == 2) listPutf(list, "PB00_2", "selected"); else listPutf(list, "PB00_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB01)
			if (cfg.value[GPORT_B] & HEX_PB01) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB01) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB01", &value, gg);  
		if (value == 0)	listPutf(list, "PB01_0", "selected"); else  if (value == 1) listPutf(list, "PB01_1", "selected"); else if (value == 2) listPutf(list, "PB01_2", "selected"); else listPutf(list, "PB01_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB02)
			if (cfg.value[GPORT_B] & HEX_PB02) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB02) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB02", &value, gg);  
		if (value == 0)	listPutf(list, "PB02_0", "selected"); else  if (value == 1) listPutf(list, "PB02_1", "selected"); else if (value == 2) listPutf(list, "PB02_2", "selected"); else listPutf(list, "PB02_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB03)
			if (cfg.value[GPORT_B] & HEX_PB03) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB03) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB03", &value, gg);  
		if (value == 0)	listPutf(list, "PB03_0", "selected"); else  if (value == 1) listPutf(list, "PB03_1", "selected"); else if (value == 2) listPutf(list, "PB03_2", "selected"); else listPutf(list, "PB03_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB04)
			if (cfg.value[GPORT_B] & HEX_PB04) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB04) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB04", &value, gg);  
		if (value == 0)	listPutf(list, "PB04_0", "selected"); else  if (value == 1) listPutf(list, "PB04_1", "selected"); else if (value == 2) listPutf(list, "PB04_2", "selected"); else listPutf(list, "PB04_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB05)
			if (cfg.value[GPORT_B] & HEX_PB05) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB05) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB05", &value, gg);  
		if (value == 0)	listPutf(list, "PB05_0", "selected"); else  if (value == 1) listPutf(list, "PB05_1", "selected"); else if (value == 2) listPutf(list, "PB05_2", "selected"); else listPutf(list, "PB05_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB06)
			if (cfg.value[GPORT_B] & HEX_PB06) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB06) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB06", &value, gg);  
		if (value == 0)	listPutf(list, "PB06_0", "selected"); else  if (value == 1) listPutf(list, "PB06_1", "selected"); else if (value == 2) listPutf(list, "PB06_2", "selected"); else listPutf(list, "PB06_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB07)
			if (cfg.value[GPORT_B] & HEX_PB07) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB07) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB07", &value, gg);  
		if (value == 0)	listPutf(list, "PB07_0", "selected"); else  if (value == 1) listPutf(list, "PB07_1", "selected"); else if (value == 2) listPutf(list, "PB07_2", "selected"); else listPutf(list, "PB07_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB08)
			if (cfg.value[GPORT_B] & HEX_PB08) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB08) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB08", &value, gg);  
		if (value == 0)	listPutf(list, "PB08_0", "selected"); else  if (value == 1) listPutf(list, "PB08_1", "selected"); else if (value == 2) listPutf(list, "PB08_2", "selected"); else listPutf(list, "PB08_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB09)
			if (cfg.value[GPORT_B] & HEX_PB09) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB09) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB09", &value, gg);  
		if (value == 0)	listPutf(list, "PB09_0", "selected"); else  if (value == 1) listPutf(list, "PB09_1", "selected"); else if (value == 2) listPutf(list, "PB09_2", "selected"); else listPutf(list, "PB09_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB10)
			if (cfg.value[GPORT_B] & HEX_PB10) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB10) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB10", &value, gg);  
		if (value == 0)	listPutf(list, "PB10_0", "selected"); else  if (value == 1) listPutf(list, "PB10_1", "selected"); else if (value == 2) listPutf(list, "PB10_2", "selected"); else listPutf(list, "PB10_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB11)
			if (cfg.value[GPORT_B] & HEX_PB11) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB11) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB11", &value, gg);  
		if (value == 0)	listPutf(list, "PB11_0", "selected"); else  if (value == 1) listPutf(list, "PB11_1", "selected"); else if (value == 2) listPutf(list, "PB11_2", "selected"); else listPutf(list, "PB11_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB12)
			if (cfg.value[GPORT_B] & HEX_PB12) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB12) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB12", &value, gg);  
		if (value == 0)	listPutf(list, "PB12_0", "selected"); else  if (value == 1) listPutf(list, "PB12_1", "selected"); else if (value == 2) listPutf(list, "PB12_2", "selected"); else listPutf(list, "PB12_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB13)
			if (cfg.value[GPORT_B] & HEX_PB13) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB13) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB13", &value, gg);  
		if (value == 0)	listPutf(list, "PB13_0", "selected"); else  if (value == 1) listPutf(list, "PB13_1", "selected"); else if (value == 2) listPutf(list, "PB13_2", "selected"); else listPutf(list, "PB13_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB14)
			if (cfg.value[GPORT_B] & HEX_PB14) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB14) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB14", &value, gg);  
		if (value == 0)	listPutf(list, "PB14_0", "selected"); else  if (value == 1) listPutf(list, "PB14_1", "selected"); else if (value == 2) listPutf(list, "PB14_2", "selected"); else listPutf(list, "PB14_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB15)
			if (cfg.value[GPORT_B] & HEX_PB15) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB15) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB15", &value, gg);  
		if (value == 0)	listPutf(list, "PB15_0", "selected"); else  if (value == 1) listPutf(list, "PB15_1", "selected"); else if (value == 2) listPutf(list, "PB15_2", "selected"); else listPutf(list, "PB15_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB16)
			if (cfg.value[GPORT_B] & HEX_PB16) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB16) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB16", &value, gg);  
		if (value == 0)	listPutf(list, "PB16_0", "selected"); else  if (value == 1) listPutf(list, "PB16_1", "selected"); else if (value == 2) listPutf(list, "PB16_2", "selected"); else listPutf(list, "PB16_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB17)
			if (cfg.value[GPORT_B] & HEX_PB17) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB17) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB17", &value, gg);  
		if (value == 0)	listPutf(list, "PB17_0", "selected"); else  if (value == 1) listPutf(list, "PB17_1", "selected"); else if (value == 2) listPutf(list, "PB17_2", "selected"); else listPutf(list, "PB17_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB18)
			if (cfg.value[GPORT_B] & HEX_PB18) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB18) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB18", &value, gg);  
		if (value == 0)	listPutf(list, "PB18_0", "selected"); else  if (value == 1) listPutf(list, "PB18_1", "selected"); else if (value == 2) listPutf(list, "PB18_2", "selected"); else listPutf(list, "PB18_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB19)
			if (cfg.value[GPORT_B] & HEX_PB19) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB19) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB19", &value, gg);  
		if (value == 0)	listPutf(list, "PB19_0", "selected"); else  if (value == 1) listPutf(list, "PB19_1", "selected"); else if (value == 2) listPutf(list, "PB19_2", "selected"); else listPutf(list, "PB19_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB20)
			if (cfg.value[GPORT_B] & HEX_PB20) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB20) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB20", &value, gg);  
		if (value == 0)	listPutf(list, "PB20_0", "selected"); else  if (value == 1) listPutf(list, "PB20_1", "selected"); else if (value == 2) listPutf(list, "PB20_2", "selected"); else listPutf(list, "PB20_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB21)
			if (cfg.value[GPORT_B] & HEX_PB21) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB21) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB21", &value, gg);  
		if (value == 0)	listPutf(list, "PB21_0", "selected"); else  if (value == 1) listPutf(list, "PB21_1", "selected"); else if (value == 2) listPutf(list, "PB21_2", "selected"); else listPutf(list, "PB21_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB22)
			if (cfg.value[GPORT_B] & HEX_PB22) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB22) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB22", &value, gg);  
		if (value == 0)	listPutf(list, "PB22_0", "selected"); else  if (value == 1) listPutf(list, "PB22_1", "selected"); else if (value == 2) listPutf(list, "PB22_2", "selected"); else listPutf(list, "PB22_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB23)
			if (cfg.value[GPORT_B] & HEX_PB23) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB23) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB23", &value, gg);  
		if (value == 0)	listPutf(list, "PB23_0", "selected"); else  if (value == 1) listPutf(list, "PB23_1", "selected"); else if (value == 2) listPutf(list, "PB23_2", "selected"); else listPutf(list, "PB23_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB24)
			if (cfg.value[GPORT_B] & HEX_PB24) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB24) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB24", &value, gg);  
		if (value == 0)	listPutf(list, "PB24_0", "selected"); else  if (value == 1) listPutf(list, "PB24_1", "selected"); else if (value == 2) listPutf(list, "PB24_2", "selected"); else listPutf(list, "PB24_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB25)
			if (cfg.value[GPORT_B] & HEX_PB25) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB25) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB25", &value, gg);  
		if (value == 0)	listPutf(list, "PB25_0", "selected"); else  if (value == 1) listPutf(list, "PB25_1", "selected"); else if (value == 2) listPutf(list, "PB25_2", "selected"); else listPutf(list, "PB25_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB26)
			if (cfg.value[GPORT_B] & HEX_PB26) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB26) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB26", &value, gg);  
		if (value == 0)	listPutf(list, "PB26_0", "selected"); else  if (value == 1) listPutf(list, "PB26_1", "selected"); else if (value == 2) listPutf(list, "PB26_2", "selected"); else listPutf(list, "PB26_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB27)
			if (cfg.value[GPORT_B] & HEX_PB27) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB27) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB27", &value, gg);  
		if (value == 0)	listPutf(list, "PB27_0", "selected"); else  if (value == 1) listPutf(list, "PB27_1", "selected"); else if (value == 2) listPutf(list, "PB27_2", "selected"); else listPutf(list, "PB27_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB28)
			if (cfg.value[GPORT_B] & HEX_PB28) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB28) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB28", &value, gg);  
		if (value == 0)	listPutf(list, "PB28_0", "selected"); else  if (value == 1) listPutf(list, "PB28_1", "selected"); else if (value == 2) listPutf(list, "PB28_2", "selected"); else listPutf(list, "PB28_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB29)
			if (cfg.value[GPORT_B] & HEX_PB29) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB29) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB29", &value, gg);  
		if (value == 0)	listPutf(list, "PB29_0", "selected"); else  if (value == 1) listPutf(list, "PB29_1", "selected"); else if (value == 2) listPutf(list, "PB29_2", "selected"); else listPutf(list, "PB29_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB30)
			if (cfg.value[GPORT_B] & HEX_PB30) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB30) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB30", &value, gg);  
		if (value == 0)	listPutf(list, "PB30_0", "selected"); else  if (value == 1) listPutf(list, "PB30_1", "selected"); else if (value == 2) listPutf(list, "PB30_2", "selected"); else listPutf(list, "PB30_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB31)
			if (cfg.value[GPORT_B] & HEX_PB31) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB31) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB31", &value, gg);  
		if (value == 0)	listPutf(list, "PB31_0", "selected"); else  if (value == 1) listPutf(list, "PB31_1", "selected"); else if (value == 2) listPutf(list, "PB31_2", "selected"); else listPutf(list, "PB31_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC00)
			if (cfg.value[GPORT_C] & HEX_PC00) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC00) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC00", &value, gg);  
		if (value == 0)	listPutf(list, "PC00_0", "selected"); else  if (value == 1) listPutf(list, "PC00_1", "selected"); else if (value == 2) listPutf(list, "PC00_2", "selected"); else listPutf(list, "PC00_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC01)
			if (cfg.value[GPORT_C] & HEX_PC01) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC01) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC01", &value, gg);  
		if (value == 0)	listPutf(list, "PC01_0", "selected"); else  if (value == 1) listPutf(list, "PC01_1", "selected"); else if (value == 2) listPutf(list, "PC01_2", "selected"); else listPutf(list, "PC01_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC02)
			if (cfg.value[GPORT_C] & HEX_PC02) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC02) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC02", &value, gg);  
		if (value == 0)	listPutf(list, "PC02_0", "selected"); else  if (value == 1) listPutf(list, "PC02_1", "selected"); else if (value == 2) listPutf(list, "PC02_2", "selected"); else listPutf(list, "PC02_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC03)
			if (cfg.value[GPORT_C] & HEX_PC03) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC03) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC03", &value, gg);  
		if (value == 0)	listPutf(list, "PC03_0", "selected"); else  if (value == 1) listPutf(list, "PC03_1", "selected"); else if (value == 2) listPutf(list, "PC03_2", "selected"); else listPutf(list, "PC03_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC04)
			if (cfg.value[GPORT_C] & HEX_PC04) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC04) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC04", &value, gg);  
		if (value == 0)	listPutf(list, "PC04_0", "selected"); else  if (value == 1) listPutf(list, "PC04_1", "selected"); else if (value == 2) listPutf(list, "PC04_2", "selected"); else listPutf(list, "PC04_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC05)
			if (cfg.value[GPORT_C] & HEX_PC05) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC05) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC05", &value, gg);  
		if (value == 0)	listPutf(list, "PC05_0", "selected"); else  if (value == 1) listPutf(list, "PC05_1", "selected"); else if (value == 2) listPutf(list, "PC05_2", "selected"); else listPutf(list, "PC05_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC08)
			if (cfg.value[GPORT_C] & HEX_PC08) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC08) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC08", &value, gg);  
		if (value == 0)	listPutf(list, "PC08_0", "selected"); else  if (value == 1) listPutf(list, "PC08_1", "selected"); else if (value == 2) listPutf(list, "PC08_2", "selected"); else listPutf(list, "PC08_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC09)
			if (cfg.value[GPORT_C] & HEX_PC09) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC09) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC09", &value, gg);  
		if (value == 0)	listPutf(list, "PC09_0", "selected"); else  if (value == 1) listPutf(list, "PC09_1", "selected"); else if (value == 2) listPutf(list, "PC09_2", "selected"); else listPutf(list, "PC09_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC10)
			if (cfg.value[GPORT_C] & HEX_PC10) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC10) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC10", &value, gg);  
		if (value == 0)	listPutf(list, "PC10_0", "selected"); else  if (value == 1) listPutf(list, "PC10_1", "selected"); else if (value == 2) listPutf(list, "PC10_2", "selected"); else listPutf(list, "PC10_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC12)
			if (cfg.value[GPORT_C] & HEX_PC12) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC12) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC12", &value, gg);  
		if (value == 0)	listPutf(list, "PC12_0", "selected"); else  if (value == 1) listPutf(list, "PC12_1", "selected"); else if (value == 2) listPutf(list, "PC12_2", "selected"); else listPutf(list, "PC12_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC14)
			if (cfg.value[GPORT_C] & HEX_PC14) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC14) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC14", &value, gg);  
		if (value == 0)	listPutf(list, "PC14_0", "selected"); else  if (value == 1) listPutf(list, "PC14_1", "selected"); else if (value == 2) listPutf(list, "PC14_2", "selected"); else listPutf(list, "PC14_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC15)
			if (cfg.value[GPORT_C] & HEX_PC15) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC15) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC15", &value, gg);  
		if (value == 0)	listPutf(list, "PC15_0", "selected"); else  if (value == 1) listPutf(list, "PC15_1", "selected"); else if (value == 2) listPutf(list, "PC15_2", "selected"); else listPutf(list, "PC15_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC16)
			if (cfg.value[GPORT_C] & HEX_PC16) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC16) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC16", &value, gg);  
		if (value == 0)	listPutf(list, "PC16_0", "selected"); else  if (value == 1) listPutf(list, "PC16_1", "selected"); else if (value == 2) listPutf(list, "PC16_2", "selected"); else listPutf(list, "PC16_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC17)
			if (cfg.value[GPORT_C] & HEX_PC17) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC17) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC17", &value, gg);  
		if (value == 0)	listPutf(list, "PC17_0", "selected"); else  if (value == 1) listPutf(list, "PC17_1", "selected"); else if (value == 2) listPutf(list, "PC17_2", "selected"); else listPutf(list, "PC17_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC18)
			if (cfg.value[GPORT_C] & HEX_PC18) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC18) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC18", &value, gg);  
		if (value == 0)	listPutf(list, "PC18_0", "selected"); else  if (value == 1) listPutf(list, "PC18_1", "selected"); else if (value == 2) listPutf(list, "PC18_2", "selected"); else listPutf(list, "PC18_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC19)
			if (cfg.value[GPORT_C] & HEX_PC19) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC19) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC19", &value, gg);  
		if (value == 0)	listPutf(list, "PC19_0", "selected"); else  if (value == 1) listPutf(list, "PC19_1", "selected"); else if (value == 2) listPutf(list, "PC19_2", "selected"); else listPutf(list, "PC19_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC20)
			if (cfg.value[GPORT_C] & HEX_PC20) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC20) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC20", &value, gg);  
		if (value == 0)	listPutf(list, "PC20_0", "selected"); else  if (value == 1) listPutf(list, "PC20_1", "selected"); else if (value == 2) listPutf(list, "PC20_2", "selected"); else listPutf(list, "PC20_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC21)
			if (cfg.value[GPORT_C] & HEX_PC21) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC21) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC21", &value, gg);  
		if (value == 0)	listPutf(list, "PC21_0", "selected"); else  if (value == 1) listPutf(list, "PC21_1", "selected"); else if (value == 2) listPutf(list, "PC21_2", "selected"); else listPutf(list, "PC21_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC22)
			if (cfg.value[GPORT_C] & HEX_PC22) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC22) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC22", &value, gg);  
		if (value == 0)	listPutf(list, "PC22_0", "selected"); else  if (value == 1) listPutf(list, "PC22_1", "selected"); else if (value == 2) listPutf(list, "PC22_2", "selected"); else listPutf(list, "PC22_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC23)
			if (cfg.value[GPORT_C] & HEX_PC23) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC23) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC23", &value, gg);  
		if (value == 0)	listPutf(list, "PC23_0", "selected"); else  if (value == 1) listPutf(list, "PC23_1", "selected"); else if (value == 2) listPutf(list, "PC23_2", "selected"); else listPutf(list, "PC23_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC26)
			if (cfg.value[GPORT_C] & HEX_PC26) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC26) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC26", &value, gg);  
		if (value == 0)	listPutf(list, "PC26_0", "selected"); else  if (value == 1) listPutf(list, "PC26_1", "selected"); else if (value == 2) listPutf(list, "PC26_2", "selected"); else listPutf(list, "PC26_3", "selected");
		}
	else	// S4M
		{
		cgiFormInteger("ADC", &value, cfg.adc);
		switch (value)	{
			case 0 :			// GPIO
				listPutf(list, "adc_0", "selected");  	break;
			case 1 : 			// eddy
				listPutf(list, "adc_1", "selected");	listPutf(list, "hide_adc", "disabled"); break;		
			case 2 :			// User
				listPutf(list, "adc_2", "selected");	listPutf(list, "hide_adc", "disabled"); break;	}
	
		if (cfg.enable[GPORT_A] & HEX_PA05) gg = 1; else gg = 0;
		cgiFormInteger("PA05EN", &value, gg);
		if (value==1)	listPutf(list, "PA05EN_1", "selected");	else 
			{ listPutf(list, "PA05EN_0", "selected");	listPutf(list, "hide_PA05EN", "disabled"); }	

		if (cfg.enable[GPORT_A] & HEX_PA22) gg = 1; else gg = 0;
		cgiFormInteger("PA22EN", &value, gg);
		if (value==1)	listPutf(list, "PA22EN_1", "selected");	else 
			{ listPutf(list, "PA22EN_0", "selected");	listPutf(list, "hide_PA22EN", "disabled"); }	

		if (cfg.enable[GPORT_A] & HEX_PA30) gg = 1; else gg = 0;
		cgiFormInteger("PA30EN", &value, gg);
		if (value==1)	listPutf(list, "PA30EN_1", "selected");	else 
			{ listPutf(list, "PA30EN_0", "selected");	listPutf(list, "hide_PA30EN", "disabled"); }	

		if (cfg.enable[GPORT_B] & HEX_PB00) gg = 1; else gg = 0;
		cgiFormInteger("PB00EN", &value, gg);
		if (value==1)	listPutf(list, "PB00EN_1", "selected");	else 
			{ listPutf(list, "PB00EN_0", "selected");	listPutf(list, "hide_PB00EN", "disabled"); }

		if (cfg.enable[GPORT_B] & HEX_PB01) gg = 1; else gg = 0;
		cgiFormInteger("PB01EN", &value, gg);
		if (value==1)	listPutf(list, "PB01EN_1", "selected");	else 
			{ listPutf(list, "PB01EN_0", "selected");	listPutf(list, "hide_PB01EN", "disabled"); }

		if (cfg.enable[GPORT_B] & HEX_PB02) gg = 1; else gg = 0;
		cgiFormInteger("PB02EN", &value, gg);
		if (value==1)	listPutf(list, "PB02EN_1", "selected");	else 
			{ listPutf(list, "PB02EN_0", "selected");	listPutf(list, "hide_PB02EN", "disabled"); }

		if (cfg.enable[GPORT_B] & HEX_PB03) gg = 1; else gg = 0;
		cgiFormInteger("PB03EN", &value, gg);
		if (value==1)	listPutf(list, "PB03EN_1", "selected");	else 
			{ listPutf(list, "PB03EN_0", "selected");	listPutf(list, "hide_PB03EN", "disabled"); }

		if (cfg.enable[GPORT_B] & HEX_PB12) gg = 1; else gg = 0;
		cgiFormInteger("PB12EN", &value, gg);
		if (value==1)	listPutf(list, "PB12EN_1", "selected");	else 
			{ listPutf(list, "PB12EN_0", "selected");	listPutf(list, "hide_PB12EN", "disabled"); }
		if (cfg.enable[GPORT_B] & HEX_PB13) gg = 1; else gg = 0;
		cgiFormInteger("PB13EN", &value, gg);
		if (value==1)	listPutf(list, "PB13EN_1", "selected");	else 
			{ listPutf(list, "PB13EN_0", "selected");	listPutf(list, "hide_PB13EN", "disabled"); }
	
		if (cfg.enable[GPORT_B] & HEX_PB16) gg = 1; else gg = 0;
		cgiFormInteger("PB16EN", &value, gg);
		if (value==1)	listPutf(list, "PB16EN_1", "selected");	else 
			{ listPutf(list, "PB16EN_0", "selected");	listPutf(list, "hide_PB16EN", "disabled"); }
	
		if (cfg.enable[GPORT_B] & HEX_PB17) gg = 1; else gg = 0;
		cgiFormInteger("PB17EN", &value, gg);
		if (value==1)	listPutf(list, "PB17EN_1", "selected");	else 
			{ listPutf(list, "PB17EN_0", "selected");	listPutf(list, "hide_PB17EN", "disabled"); }
	
		if (cfg.enable[GPORT_B] & HEX_PB18) gg = 1; else gg = 0;
		cgiFormInteger("PB18EN", &value, gg);
		if (value==1)	listPutf(list, "PB18EN_1", "selected");	else 
			{ listPutf(list, "PB18EN_0", "selected");	listPutf(list, "hide_PB18EN", "disabled"); }
		if (cfg.enable[GPORT_B] & HEX_PB19) gg = 1; else gg = 0;
		cgiFormInteger("PB19EN", &value, gg);
		if (value==1)	listPutf(list, "PB19EN_1", "selected");	else 
			{ listPutf(list, "PB19EN_0", "selected");	listPutf(list, "hide_PB19EN", "disabled"); }

		if (cfg.enable[GPORT_B] & HEX_PB20) gg = 1; else gg = 0;
		cgiFormInteger("PB20EN", &value, gg);
		if (value==1)	listPutf(list, "PB20EN_1", "selected");	else 
				{ listPutf(list, "PB20EN_0", "selected");	listPutf(list, "hide_PB20EN", "disabled"); }
		if (cfg.enable[GPORT_B] & HEX_PB21) gg = 1; else gg = 0;
		cgiFormInteger("PB21EN", &value, gg);
		if (value==1)	listPutf(list, "PB21EN_1", "selected");	else 
			{ listPutf(list, "PB21EN_0", "selected");	listPutf(list, "hide_PB21EN", "disabled"); }
		if (cfg.enable[GPORT_B] & HEX_PB30) gg = 1; else gg = 0;
		cgiFormInteger("PB30EN", &value, gg);
		if (value==1)	listPutf(list, "PB30EN_1", "selected");	else 
			{ listPutf(list, "PB30EN_0", "selected");	listPutf(list, "hide_PB30EN", "disabled"); }
		if (cfg.enable[GPORT_B] & HEX_PB31) gg = 1; else gg = 0;
		cgiFormInteger("PB31EN", &value, gg);
		if (value==1)	listPutf(list, "PB31EN_1", "selected");	else 
			{ listPutf(list, "PB31EN_0", "selected");	listPutf(list, "hide_PB31EN", "disabled"); }
	
		if (cfg.enable[GPORT_C] & HEX_PC05) gg = 1; else gg = 0;
		cgiFormInteger("PC05EN", &value, gg);
		if (value==1)	listPutf(list, "PC05EN_1", "selected");	else 
			{ listPutf(list, "PC05EN_0", "selected");	listPutf(list, "hide_PC05EN", "disabled"); }
	
		if (cfg.enable[GPORT_C] & HEX_PC09) gg = 1; else gg = 0;
		cgiFormInteger("PC09EN", &value, gg);
		if (value==1)	listPutf(list, "PC09EN_1", "selected");	else 
			{ listPutf(list, "PC09EN_0", "selected");	listPutf(list, "hide_PC09EN", "disabled"); }

		if (cfg.enable[GPORT_C] & HEX_PC10) gg = 1; else gg = 0;
		cgiFormInteger("PC10EN", &value, gg);
		if (value==1)	listPutf(list, "PC10EN_1", "selected");	else 
			{ listPutf(list, "PC10EN_0", "selected");	listPutf(list, "hide_PC10EN", "disabled"); }

		if (cfg.enable[GPORT_C] & HEX_PC12) gg = 1; else gg = 0;
		cgiFormInteger("PC12EN", &value, gg);
		if (value==1)	listPutf(list, "PC12EN_1", "selected");	else 
			{ listPutf(list, "PC12EN_0", "selected");	listPutf(list, "hide_PC12EN", "disabled"); }

		if (cfg.enable[GPORT_C] & HEX_PC13) gg = 1; else gg = 0;
		cgiFormInteger("PC13EN", &value, gg);
		if (value==1)	listPutf(list, "PC13EN_1", "selected");	else 
			{ listPutf(list, "PC13EN_0", "selected");	listPutf(list, "hide_PC13EN", "disabled"); }

		if (cfg.enable[GPORT_C] & HEX_PC14) gg = 1; else gg = 0;
		cgiFormInteger("PC14EN", &value, gg);
		if (value==1)	listPutf(list, "PC14EN_1", "selected");	else 
			{ listPutf(list, "PC14EN_0", "selected");	listPutf(list, "hide_PC14EN", "disabled"); }

		if (cfg.enable[GPORT_C] & HEX_PC15) gg = 1; else gg = 0;
		cgiFormInteger("PC15EN", &value, gg);
		if (value==1)	listPutf(list, "PC15EN_1", "selected");	else 
			{ listPutf(list, "PC15EN_0", "selected");	listPutf(list, "hide_PC15EN", "disabled"); }

		if (cfg.enable[GPORT_C] & HEX_PC17) gg = 1; else gg = 0;
		cgiFormInteger("PC17EN", &value, gg);
		if (value==1)	listPutf(list, "PC17EN_1", "selected");	else 
			{ listPutf(list, "PC17EN_0", "selected");	listPutf(list, "hide_PC17EN", "disabled"); }
	
		if (cfg.enable[GPORT_C] & HEX_PC18) gg = 1; else gg = 0;
		cgiFormInteger("PC18EN", &value, gg);
		if (value==1)	listPutf(list, "PC18EN_1", "selected");	else 
			{ listPutf(list, "PC18EN_0", "selected");	listPutf(list, "hide_PC18EN", "disabled"); }

		if (cfg.enable[GPORT_C] & HEX_PC19) gg = 1; else gg = 0;
		cgiFormInteger("PC19EN", &value, gg);
		if (value==1)	listPutf(list, "PC19EN_1", "selected");	else 
			{ listPutf(list, "PC19EN_0", "selected");	listPutf(list, "hide_PC19EN", "disabled"); }
	
		if (cfg.enable[GPORT_C] & HEX_PC20) gg = 1; else gg = 0;
		cgiFormInteger("PC20EN", &value, gg);
		if (value==1)	listPutf(list, "PC20EN_1", "selected");	else 
			{ listPutf(list, "PC20EN_0", "selected");	listPutf(list, "hide_PC20EN", "disabled"); }

		if (cfg.enable[GPORT_C] & HEX_PC24) gg = 1; else gg = 0;
		cgiFormInteger("PC24EN", &value, gg);
		if (value==1)	listPutf(list, "PC24EN_1", "selected");	else 
			{ listPutf(list, "PC24EN_0", "selected");	listPutf(list, "hide_PC24EN", "disabled"); }

		if (cfg.enable[GPORT_C] & HEX_PC25) gg = 1; else gg = 0;
		cgiFormInteger("PC25EN", &value, gg);
		if (value==1)	listPutf(list, "PC25EN_1", "selected");	else 
			{ listPutf(list, "PC25EN_0", "selected");	listPutf(list, "hide_PC25EN", "disabled"); }
	
		//...................................................................
		if (cfg.mode[GPORT_A] & HEX_PA05)
			if (cfg.value[GPORT_A] & HEX_PA05) gg = 3; else gg = 2;	// output			
		else 
			if (cfg.pullup[GPORT_A] & HEX_PA05) gg = 0; else gg = 1;	// input
		cgiFormInteger("PA05", &value, gg);  
		if (value == 0)	listPutf(list, "PA05_0", "selected"); else  if (value == 1) listPutf(list, "PA05_1", "selected"); else if (value == 2) listPutf(list, "PA05_2", "selected"); else listPutf(list, "PA05_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_A] & HEX_PA22)
			if (cfg.value[GPORT_A] & HEX_PA22) gg = 3; else gg = 2;	// output			
		else 
			if (cfg.pullup[GPORT_A] & HEX_PA22) gg = 0; else gg = 1;	// input
		cgiFormInteger("PA22", &value, gg);  
		if (value == 0)	listPutf(list, "PA22_0", "selected"); else  if (value == 1) listPutf(list, "PA22_1", "selected"); else if (value == 2) listPutf(list, "PA22_2", "selected"); else listPutf(list, "PA22_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_A] & HEX_PA30)
			if (cfg.value[GPORT_A] & HEX_PA30) gg = 3; else gg = 2;	// output			
		else 
			if (cfg.pullup[GPORT_A] & HEX_PA30) gg = 0; else gg = 1;	// input
		cgiFormInteger("PA30", &value, gg);  
		if (value == 0)	listPutf(list, "PA30_0", "selected"); else  if (value == 1) listPutf(list, "PA30_1", "selected"); else if (value == 2) listPutf(list, "PA30_2", "selected"); else listPutf(list, "PA30_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB00)
			if (cfg.value[GPORT_B] & HEX_PB00) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB00) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB00", &value, gg);  
		if (value == 0)	listPutf(list, "PB00_0", "selected"); else  if (value == 1) listPutf(list, "PB00_1", "selected"); else if (value == 2) listPutf(list, "PB00_2", "selected"); else listPutf(list, "PB00_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB01)
			if (cfg.value[GPORT_B] & HEX_PB01) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB01) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB01", &value, gg);  
		if (value == 0)	listPutf(list, "PB01_0", "selected"); else  if (value == 1) listPutf(list, "PB01_1", "selected"); else if (value == 2) listPutf(list, "PB01_2", "selected"); else listPutf(list, "PB01_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB02)
			if (cfg.value[GPORT_B] & HEX_PB02) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB02) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB02", &value, gg);  
		if (value == 0)	listPutf(list, "PB02_0", "selected"); else  if (value == 1) listPutf(list, "PB02_1", "selected"); else if (value == 2) listPutf(list, "PB02_2", "selected"); else listPutf(list, "PB02_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB03)
			if (cfg.value[GPORT_B] & HEX_PB03) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB03) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB03", &value, gg);  
		if (value == 0)	listPutf(list, "PB03_0", "selected"); else  if (value == 1) listPutf(list, "PB03_1", "selected"); else if (value == 2) listPutf(list, "PB03_2", "selected"); else listPutf(list, "PB03_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB12)
			if (cfg.value[GPORT_B] & HEX_PB12) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB12) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB12", &value, gg);  
		if (value == 0)	listPutf(list, "PB12_0", "selected"); else  if (value == 1) listPutf(list, "PB12_1", "selected"); else if (value == 2) listPutf(list, "PB12_2", "selected"); else listPutf(list, "PB12_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB13)
			if (cfg.value[GPORT_B] & HEX_PB13) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB13) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB13", &value, gg);  
		if (value == 0)	listPutf(list, "PB13_0", "selected"); else  if (value == 1) listPutf(list, "PB13_1", "selected"); else if (value == 2) listPutf(list, "PB13_2", "selected"); else listPutf(list, "PB13_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB16)
			if (cfg.value[GPORT_B] & HEX_PB16) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB16) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB16", &value, gg);  
		if (value == 0)	listPutf(list, "PB16_0", "selected"); else  if (value == 1) listPutf(list, "PB16_1", "selected"); else if (value == 2) listPutf(list, "PB16_2", "selected"); else listPutf(list, "PB16_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB17)
			if (cfg.value[GPORT_B] & HEX_PB17) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB17) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB17", &value, gg);  
		if (value == 0)	listPutf(list, "PB17_0", "selected"); else  if (value == 1) listPutf(list, "PB17_1", "selected"); else if (value == 2) listPutf(list, "PB17_2", "selected"); else listPutf(list, "PB17_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB18)
			if (cfg.value[GPORT_B] & HEX_PB18) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB18) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB18", &value, gg);  
		if (value == 0)	listPutf(list, "PB18_0", "selected"); else  if (value == 1) listPutf(list, "PB18_1", "selected"); else if (value == 2) listPutf(list, "PB18_2", "selected"); else listPutf(list, "PB18_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB19)
			if (cfg.value[GPORT_B] & HEX_PB19) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB19) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB19", &value, gg);  
		if (value == 0)	listPutf(list, "PB19_0", "selected"); else  if (value == 1) listPutf(list, "PB19_1", "selected"); else if (value == 2) listPutf(list, "PB19_2", "selected"); else listPutf(list, "PB19_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB20)
			if (cfg.value[GPORT_B] & HEX_PB20) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB20) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB20", &value, gg);  
		if (value == 0)	listPutf(list, "PB20_0", "selected"); else  if (value == 1) listPutf(list, "PB20_1", "selected"); else if (value == 2) listPutf(list, "PB20_2", "selected"); else listPutf(list, "PB20_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB21)
			if (cfg.value[GPORT_B] & HEX_PB21) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB21) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB21", &value, gg);  
		if (value == 0)	listPutf(list, "PB21_0", "selected"); else  if (value == 1) listPutf(list, "PB21_1", "selected"); else if (value == 2) listPutf(list, "PB21_2", "selected"); else listPutf(list, "PB21_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB30)
			if (cfg.value[GPORT_B] & HEX_PB30) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB30) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB30", &value, gg);  
		if (value == 0)	listPutf(list, "PB30_0", "selected"); else  if (value == 1) listPutf(list, "PB30_1", "selected"); else if (value == 2) listPutf(list, "PB30_2", "selected"); else listPutf(list, "PB30_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_B] & HEX_PB31)
			if (cfg.value[GPORT_B] & HEX_PB31) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_B] & HEX_PB31) gg = 0; else gg = 1;	// input
		cgiFormInteger("PB31", &value, gg);  
		if (value == 0)	listPutf(list, "PB31_0", "selected"); else  if (value == 1) listPutf(list, "PB31_1", "selected"); else if (value == 2) listPutf(list, "PB31_2", "selected"); else listPutf(list, "PB31_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC00)
			if (cfg.value[GPORT_C] & HEX_PC00) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC00) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC00", &value, gg);  
		if (value == 0)	listPutf(list, "PC00_0", "selected"); else  if (value == 1) listPutf(list, "PC00_1", "selected"); else if (value == 2) listPutf(list, "PC00_2", "selected"); else listPutf(list, "PC00_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC01)
			if (cfg.value[GPORT_C] & HEX_PC01) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC01) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC01", &value, gg);  
		if (value == 0)	listPutf(list, "PC01_0", "selected"); else  if (value == 1) listPutf(list, "PC01_1", "selected"); else if (value == 2) listPutf(list, "PC01_2", "selected"); else listPutf(list, "PC01_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC02)
			if (cfg.value[GPORT_C] & HEX_PC02) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC02) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC02", &value, gg);  
		if (value == 0)	listPutf(list, "PC02_0", "selected"); else  if (value == 1) listPutf(list, "PC02_1", "selected"); else if (value == 2) listPutf(list, "PC02_2", "selected"); else listPutf(list, "PC02_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC03)
			if (cfg.value[GPORT_C] & HEX_PC03) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC03) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC03", &value, gg);  
		if (value == 0)	listPutf(list, "PC03_0", "selected"); else  if (value == 1) listPutf(list, "PC03_1", "selected"); else if (value == 2) listPutf(list, "PC03_2", "selected"); else listPutf(list, "PC03_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC05)
			if (cfg.value[GPORT_C] & HEX_PC05) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC05) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC05", &value, gg);  
		if (value == 0)	listPutf(list, "PC05_0", "selected"); else  if (value == 1) listPutf(list, "PC05_1", "selected"); else if (value == 2) listPutf(list, "PC05_2", "selected"); else listPutf(list, "PC05_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC09)
			if (cfg.value[GPORT_C] & HEX_PC09) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC09) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC09", &value, gg);  
		if (value == 0)	listPutf(list, "PC09_0", "selected"); else  if (value == 1) listPutf(list, "PC09_1", "selected"); else if (value == 2) listPutf(list, "PC09_2", "selected"); else listPutf(list, "PC09_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC10)
			if (cfg.value[GPORT_C] & HEX_PC10) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC10) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC10", &value, gg);  
		if (value == 0)	listPutf(list, "PC10_0", "selected"); else  if (value == 1) listPutf(list, "PC10_1", "selected"); else if (value == 2) listPutf(list, "PC10_2", "selected"); else listPutf(list, "PC10_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC12)
			if (cfg.value[GPORT_C] & HEX_PC12) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC12) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC12", &value, gg);  
		if (value == 0)	listPutf(list, "PC12_0", "selected"); else  if (value == 1) listPutf(list, "PC12_1", "selected"); else if (value == 2) listPutf(list, "PC12_2", "selected"); else listPutf(list, "PC12_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC13)
			if (cfg.value[GPORT_C] & HEX_PC13) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC13) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC13", &value, gg);  
		if (value == 0)	listPutf(list, "PC13_0", "selected"); else  if (value == 1) listPutf(list, "PC13_1", "selected"); else if (value == 2) listPutf(list, "PC13_2", "selected"); else listPutf(list, "PC13_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC14)
			if (cfg.value[GPORT_C] & HEX_PC14) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC14) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC14", &value, gg);  
		if (value == 0)	listPutf(list, "PC14_0", "selected"); else  if (value == 1) listPutf(list, "PC14_1", "selected"); else if (value == 2) listPutf(list, "PC14_2", "selected"); else listPutf(list, "PC14_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC15)
			if (cfg.value[GPORT_C] & HEX_PC15) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC15) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC15", &value, gg);  
		if (value == 0)	listPutf(list, "PC15_0", "selected"); else  if (value == 1) listPutf(list, "PC15_1", "selected"); else if (value == 2) listPutf(list, "PC15_2", "selected"); else listPutf(list, "PC15_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC17)
			if (cfg.value[GPORT_C] & HEX_PC17) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC17) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC17", &value, gg);  
		if (value == 0)	listPutf(list, "PC17_0", "selected"); else  if (value == 1) listPutf(list, "PC17_1", "selected"); else if (value == 2) listPutf(list, "PC17_2", "selected"); else listPutf(list, "PC17_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC18)
			if (cfg.value[GPORT_C] & HEX_PC18) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC18) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC18", &value, gg);  
		if (value == 0)	listPutf(list, "PC18_0", "selected"); else  if (value == 1) listPutf(list, "PC18_1", "selected"); else if (value == 2) listPutf(list, "PC18_2", "selected"); else listPutf(list, "PC18_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC19)
			if (cfg.value[GPORT_C] & HEX_PC19) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC19) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC19", &value, gg);  
		if (value == 0)	listPutf(list, "PC19_0", "selected"); else  if (value == 1) listPutf(list, "PC19_1", "selected"); else if (value == 2) listPutf(list, "PC19_2", "selected"); else listPutf(list, "PC19_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC20)
			if (cfg.value[GPORT_C] & HEX_PC20) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC20) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC20", &value, gg);  
		if (value == 0)	listPutf(list, "PC20_0", "selected"); else  if (value == 1) listPutf(list, "PC20_1", "selected"); else if (value == 2) listPutf(list, "PC20_2", "selected"); else listPutf(list, "PC20_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC24)
			if (cfg.value[GPORT_C] & HEX_PC24) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC24) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC24", &value, gg);  
		if (value == 0)	listPutf(list, "PC24_0", "selected"); else  if (value == 1) listPutf(list, "PC24_1", "selected"); else if (value == 2) listPutf(list, "PC24_2", "selected"); else listPutf(list, "PC24_3", "selected");
		//...................................................................
		if (cfg.mode[GPORT_C] & HEX_PC25)
			if (cfg.value[GPORT_C] & HEX_PC25) gg = 3; else gg = 2;		// output
		else
			if (cfg.pullup[GPORT_C] & HEX_PC25) gg = 0; else gg = 1;	// input
		cgiFormInteger("PC25", &value, gg);  
		if (value == 0)	listPutf(list, "PC25_0", "selected"); else  if (value == 1) listPutf(list, "PC25_1", "selected"); else if (value == 2) listPutf(list, "PC25_2", "selected"); else listPutf(list, "PC25_3", "selected");
		}
}
//---------------------------------------------------------------------------
void get_dio()
{
int value, cpu=0, value2, gv, gm, gg;
struct SB_DIO_CONFIG	cfg;

	SB_ReadConfig  (CFGFILE_ETC_DIO,     (char *)&cfg,		sizeof(struct SB_DIO_CONFIG));		

	cgiFormInteger("LCD", &value, cfg.lcd);
	switch (value)	{
		case 0 :			// GPIO
			listPutf(list, "lcd_0", "selected");  	break;
		case 1 :			// User
			listPutf(list, "lcd_1", "selected");	listPutf(list, "hide_lcd", "disabled"); break;	}

	//..............................................................................................
	if (cfg.mode & 0x0001) gg = 0; else if (cfg.value & 0x0001) gg = 2; else gg = 1;		// output
	cgiFormInteger("DIO00", &value, gg);  
	if (value == 0)	listPutf(list, "DIO00_0", "selected"); else  if (value == 1) listPutf(list, "DIO00_1", "selected"); else listPutf(list, "DIO00_2", "selected"); 
	//..............................................................................................
	if (cfg.mode & 0x0002) gg = 0; else if (cfg.value & 0x0002) gg = 2; else gg = 1;		// output
	cgiFormInteger("DIO01", &value, gg);  
	if (value == 0)	listPutf(list, "DIO01_0", "selected"); else  if (value == 1) listPutf(list, "DIO01_1", "selected"); else listPutf(list, "DIO01_2", "selected"); 
	//..............................................................................................
	if (cfg.mode & 0x0004) gg = 0; else if (cfg.value & 0x0004) gg = 2; else gg = 1;		// output
	cgiFormInteger("DIO02", &value, gg);  
	if (value == 0)	listPutf(list, "DIO02_0", "selected"); else  if (value == 1) listPutf(list, "DIO02_1", "selected"); else listPutf(list, "DIO02_2", "selected"); 
	//..............................................................................................
	if (cfg.mode & 0x0008) gg = 0; else if (cfg.value & 0x0008) gg = 2; else gg = 1;		// output
	cgiFormInteger("DIO03", &value, gg);  
	if (value == 0)	listPutf(list, "DIO03_0", "selected"); else  if (value == 1) listPutf(list, "DIO03_1", "selected"); else listPutf(list, "DIO03_2", "selected"); 
	//..............................................................................................
	if (cfg.mode & 0x0010) gg = 0; else if (cfg.value & 0x0010) gg = 2; else gg = 1;		// output
	cgiFormInteger("DIO04", &value, gg);  
	if (value == 0)	listPutf(list, "DIO04_0", "selected"); else  if (value == 1) listPutf(list, "DIO04_1", "selected"); else listPutf(list, "DIO04_2", "selected"); 
	//..............................................................................................
	if (cfg.mode & 0x0020) gg = 0; else if (cfg.value & 0x0020) gg = 2; else gg = 1;		// output
	cgiFormInteger("DIO05", &value, gg);  
	if (value == 0)	listPutf(list, "DIO05_0", "selected"); else  if (value == 1) listPutf(list, "DIO05_1", "selected"); else listPutf(list, "DIO05_2", "selected"); 
	//..............................................................................................
	if (cfg.mode & 0x0040) gg = 0; else if (cfg.value & 0x0040) gg = 2; else gg = 1;		// output
	cgiFormInteger("DIO06", &value, gg);  
	if (value == 0)	listPutf(list, "DIO06_0", "selected"); else  if (value == 1) listPutf(list, "DIO06_1", "selected"); else listPutf(list, "DIO06_2", "selected"); 
	//..............................................................................................
	if (cfg.mode & 0x0080) gg = 0; else if (cfg.value & 0x0080) gg = 2; else gg = 1;		// output
	cgiFormInteger("DIO07", &value, gg);  
	if (value == 0)	listPutf(list, "DIO07_0", "selected"); else  if (value == 1) listPutf(list, "DIO07_1", "selected"); else listPutf(list, "DIO07_2", "selected"); 
	//..............................................................................................
	if (cfg.mode & 0x0100) gg = 0; else if (cfg.value & 0x0100) gg = 2; else gg = 1;		// output
	cgiFormInteger("DIO08", &value, gg);  
	if (value == 0)	listPutf(list, "DIO08_0", "selected"); else  if (value == 1) listPutf(list, "DIO08_1", "selected"); else listPutf(list, "DIO08_2", "selected"); 
	//..............................................................................................
	if (cfg.mode & 0x0200) gg = 0; else if (cfg.value & 0x0200) gg = 2; else gg = 1;		// output
	cgiFormInteger("DIO09", &value, gg);  
	if (value == 0)	listPutf(list, "DIO09_0", "selected"); else  if (value == 1) listPutf(list, "DIO09_1", "selected"); else listPutf(list, "DIO09_2", "selected"); 
	//..............................................................................................
	if (cfg.mode & 0x0400) gg = 0; else if (cfg.value & 0x0400) gg = 2; else gg = 1;		// output
	cgiFormInteger("DIO10", &value, gg);  
	if (value == 0)	listPutf(list, "DIO10_0", "selected"); else  if (value == 1) listPutf(list, "DIO10_1", "selected"); else listPutf(list, "DIO10_2", "selected"); 
	//..............................................................................................
	if (cfg.mode & 0x0800) gg = 0; else if (cfg.value & 0x0800) gg = 2; else gg = 1;		// output
	cgiFormInteger("DIO11", &value, gg);  
	if (value == 0)	listPutf(list, "DIO11_0", "selected"); else  if (value == 1) listPutf(list, "DIO11_1", "selected"); else listPutf(list, "DIO11_2", "selected"); 
	//..............................................................................................
	if (cfg.mode & 0x1000) gg = 0; else if (cfg.value & 0x1000) gg = 2; else gg = 1;		// output
	cgiFormInteger("DIO12", &value, gg);  
	if (value == 0)	listPutf(list, "DIO12_0", "selected"); else  if (value == 1) listPutf(list, "DIO12_1", "selected"); else listPutf(list, "DIO12_2", "selected"); 
	//..............................................................................................
	if (cfg.mode & 0x2000) gg = 0; else if (cfg.value & 0x2000) gg = 2; else gg = 1;		// output
	cgiFormInteger("DIO13", &value, gg);  
	if (value == 0)	listPutf(list, "DIO13_0", "selected"); else  if (value == 1) listPutf(list, "DIO13_1", "selected"); else listPutf(list, "DIO13_2", "selected"); 
	//..............................................................................................
	if (cfg.mode & 0x4000) gg = 0; else if (cfg.value & 0x4000) gg = 2; else gg = 1;		// output
	cgiFormInteger("DIO14", &value, gg);  
	if (value == 0)	listPutf(list, "DIO14_0", "selected"); else  if (value == 1) listPutf(list, "DIO14_1", "selected"); else listPutf(list, "DIO14_2", "selected"); 
	//..............................................................................................
	if (cfg.mode & 0x8000) gg = 0; else if (cfg.value & 0x8000) gg = 2; else gg = 1;		// output
	cgiFormInteger("DIO15", &value, gg);  
	if (value == 0)	listPutf(list, "DIO15_0", "selected"); else  if (value == 1) listPutf(list, "DIO15_1", "selected"); else listPutf(list, "DIO15_2", "selected"); 
}
//---------------------------------------------------------------------------
void get_snmp()
{
char buff[256];
int value, value2;
char tmp;

struct SB_SNMP_CONFIG	cfg;

	SB_ReadConfig  (CFGFILE_ETC_SNMP,     (char *)&cfg,		sizeof(struct SB_SNMP_CONFIG));		

	cgiFormInteger("N_SNMP", &value, cfg.enable);
	if (value == 1)
		listPutf(list, "n_snmp_en", "selected");
	else
		{
		listPutf(list, "n_snmp_di", "selected");
		listPutf(list,"snmp_start","disabled");
		}
		
	cgiFormInteger("N_V1_ATTRIB", &value, cfg.v1_readwrite);
	switch (value)
		{
		case 0 :
		listPutf(list, "n_v1ro", "selected");
		break;
		case 1 : 	
		listPutf(list, "n_v1rw", "selected");
		break;
		case 2 :
		listPutf(list, "n_v1rd", "selected");
		break;
		}
		
	cgiFormInteger("N_V3_ATTRIB", &value, cfg.v3_readwrite);
	if (value == 1)
		listPutf(list, "n_v3rw", "selected");
	else
		listPutf(list, "n_v3ro", "selected");

	if (cgiFormStringNoNewlines("N_V3USER", buff, 16) == cgiFormNotFound)
		listPutf(list, "n_v3user", cfg.v3_username);
	else	
		listPutf(list, "n_v3user", buff);
		
	if (cgiFormStringNoNewlines("N_V3PASS", buff, 16) == cgiFormNotFound)
		listPutf(list, "n_v3pass", cfg.v3_password);
	else	
		listPutf(list, "n_v3pass", buff);
		
	if (cgiFormStringNoNewlines("N_TRAPIP", buff, 16) == cgiFormNotFound)
		{	
		sprintf(buff, "%d.%d.%d.%d", cfg.trap_ip[0], cfg.trap_ip[1],cfg.trap_ip[2],cfg.trap_ip[3]);
		listPutf(list, "n_trapip", buff);
		}
	else	
		listPutf(list, "n_trapip", buff);		
		
	if (cgiFormStringNoNewlines("N_TRAPPORT", buff, 6) == cgiFormNotFound)
		listPutf(list, "n_trapport", "%u", cfg.trap_port);
	else		
		listPutf(list, "n_trapport", buff);		

	tmp = cfg.trap_option & 0x01;
	cgiFormInteger("N_RESET_ALARM", &value, tmp);
	if (value == 1)
		listPutf(list, "n_reset_en", "selected");
	else
		listPutf(list, "n_reset_di", "selected");
		
	tmp = (cfg.trap_option & 0x02);
	if (tmp == 0x02) tmp = 1; else tmp = 0;
	cgiFormInteger("N_CON_ALARM", &value, tmp);
	if (value == 1)
		listPutf(list, "n_con_en", "selected");
	else
		listPutf(list, "n_con_di", "selected");

	tmp = (cfg.trap_option & 0x04);
	if (tmp == 0x04) tmp = 1; else tmp = 0;
	cgiFormInteger("N_DIS_ALARM", &value, tmp);
	if (value == 1)
		listPutf(list, "n_dis_en", "selected");
	else
		listPutf(list, "n_dis_di", "selected");
}
//---------------------------------------------------------------------------
void get_admin()
{
char buff[256];
char kver[10],fver[10],hver[10],date[128];
int fd;
struct SB_SYSTEM_CONFIG	cfg;	
	
	SB_ReadConfig  (CFGFILE_ETC_SYSTEM, (char *)&cfg,	sizeof(struct SB_SYSTEM_CONFIG));	

	fd=open("/etc/versions",O_RDONLY);
	read(fd,buff,sizeof(buff));
	close(fd);
	sscanf(buff,"Date:%s\nKVer:%s\nFVer:%s\nHWVer:%s\n",date,kver,fver,hver);

	// device name
	if (cgiFormStringNoNewlines("DEVICE_NAME", buff, 32) == cgiFormNotFound)
		listPutf(list, "a_device_name", cfg.device_name);
	else
		listPutf(list, "a_device_name", buff);

	listPutf(list, "fw_ver", fver);
	listPutf(list, "hw_ver", hver);
	listPutf(list, "kn_ver", kver);

	// website
	if (cgiFormStringNoNewlines("WEBSITE", buff, 50) == cgiFormNotFound)
		listPutf(list, "a_website", cfg.website);
	else
		listPutf(list, "a_website", buff);
	// contact
	if (cgiFormStringNoNewlines("CONTACT", buff, 50) == cgiFormNotFound)
		listPutf(list, "a_contact", cfg.contact);
	else
		listPutf(list, "a_contact", buff);

	// OEM Functionality: edit device name, contact info
#ifdef EDDY_OEM_SUPPORT
	listPutf(list, "a_oem", "");
#else
	listPutf(list, "a_oem", "disabled");
#endif // EDDY_OEM_SUPPORT
}



