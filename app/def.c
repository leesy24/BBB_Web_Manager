/*****************************************************************************
Copyright (c) 2007 SystemBase Co., LTD  All Rights Reserved.

setup utility 		09, 31, 2007. yhlee
******************************************************************************/
#include "sb_include.h"
#include "sb_define.h"
#include "sb_shared.h"
#include "sb_config.h"
#include "sb_extern.h"
#include "sb_ioctl.h"
#include "sb_misc.h"

struct SB_SYSTEM_CONFIG			CFG_SYS;
struct SB_SIO_CONFIG			CFG_SIO [SB_MAX_SIO_PORT];
struct SB_SNMP_CONFIG			CFG_SNMP;
struct SB_GPIO_CONFIG			CFG_GPIO;
struct SB_DIO_CONFIG			CFG_DIO;
struct SB_WIFI_CONFIG			CFG_WIFI;

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

#define	HEX_PA04_AND	0xffffffef
#define	HEX_PA05_AND	0xffffffdf	

#define	HEX_PB00_AND	0xfffffffe
#define	HEX_PB01_AND	0xfffffffd
#define	HEX_PB02_AND	0xfffffffb
#define	HEX_PB03_AND	0xfffffff7
#define	HEX_PB04_AND	0xffffffef
#define	HEX_PB05_AND	0xffffffdf
#define	HEX_PB06_AND	0xffffffbf
#define	HEX_PB07_AND	0xffffff7f
#define	HEX_PB08_AND	0xfffffeff
#define	HEX_PB09_AND	0xfffffdff
#define	HEX_PB10_AND	0xfffffbff
#define	HEX_PB11_AND	0xfffff7ff
#define	HEX_PB12_AND	0xffffefff
#define	HEX_PB13_AND	0xffffdfff
#define	HEX_PB14_AND	0xffffbfff
#define	HEX_PB15_AND	0xffff7fff
#define	HEX_PB16_AND	0xfffeffff
#define	HEX_PB17_AND	0xfffdffff
#define	HEX_PB18_AND	0xfffbffff
#define	HEX_PB19_AND	0xfff7ffff
#define	HEX_PB20_AND	0xffefffff
#define	HEX_PB21_AND	0xffdfffff
#define	HEX_PB22_AND	0xffbfffff
#define	HEX_PB23_AND	0xff7fffff
#define	HEX_PB24_AND	0xfeffffff
#define	HEX_PB25_AND	0xfdffffff
#define	HEX_PB26_AND	0xfbffffff
#define	HEX_PB27_AND	0xf7ffffff 
#define	HEX_PB28_AND	0xefffffff 
#define	HEX_PB29_AND	0xdfffffff 
#define	HEX_PB30_AND	0xbfffffff 
#define	HEX_PB31_AND	0x7fffffff 

#define	HEX_PC00_AND	0xfffffffe 
#define	HEX_PC01_AND	0xfffffffd 
#define	HEX_PC02_AND	0xfffffffb 
#define	HEX_PC03_AND	0xfffffff7 
#define	HEX_PC04_AND	0xffffffef 
#define	HEX_PC05_AND	0xffffffdf 
#define	HEX_PC08_AND	0xfffffeff 
#define	HEX_PC09_AND	0xfffffdff 
#define	HEX_PC10_AND	0xfffffbff 
#define	HEX_PC12_AND	0xffffefff 
#define	HEX_PC13_AND	0xffffdfff 
#define	HEX_PC14_AND	0xffffbfff 
#define	HEX_PC15_AND	0xffff7fff 
#define	HEX_PC16_AND	0xfffeffff 
#define	HEX_PC17_AND	0xfffdffff 
#define	HEX_PC18_AND	0xfffbffff 
#define	HEX_PC19_AND	0xfff7ffff 
#define	HEX_PC20_AND	0xffefffff 
#define	HEX_PC21_AND	0xffdfffff 
#define	HEX_PC22_AND	0xffbfffff
#define	HEX_PC23_AND	0xff7fffff 
#define	HEX_PC26_AND	0xfbffffff  


void help();
void view_management ();
void view_port ();
void view_gpio();
void view_snmp();
void view_wan();
void view_lan();
void make_cfg ();
int set_wan (int gc, char *gv1, char *gv2);
int set_wifi (int gc, char *gv1, char *gv2);
int set_lan (int gc, char *gv1, char *gv2);
int set_port (char *gv1, char *gv2, char *gv3);
int set_snmp (int gc, char *gv1, char *gv2);
int set_gpio (char *gv1, char *gv2, char *gv3);
int set_management (char *gv1, char *gv2, char *gv3);
void write_web_login ();
int main (int argc, char *argv[]);
void factory_default (char *MAC);
void view_dio();
void view_wifi();

int Product_ID;
//=============================================================================
int main (int argc, char *argv[])
{
int i;
	printf ("%s:%s\n", argv[0], argv[1]);

	Product_ID = SB_GetModel ();
    if (argc < 2)
	    {
		help ();
	   	return 0;
    	}

    if (argc > 1) for (i=0; i < strlen( argv[1]) ; i++) argv[1][i] = tolower( argv[1][i] );
    if (argc > 2) for (i=0; i < strlen( argv[2]) ; i++) argv[2][i] = tolower( argv[2][i] );
    if (argc > 3) for (i=0; i < strlen( argv[3]) ; i++) argv[3][i] = tolower( argv[3][i] );    
    if (argc > 4) for (i=0; i < strlen( argv[4]) ; i++) argv[4][i] = tolower( argv[4][i] );    


    if (!strncmp("help", argv[1], 2)) 
    	{
		help ();
	   	return 0;
		}

    if (!strncmp("save", argv[1], 3)) 
		{
		SB_ReadConfig  (CFGFILE_ETC_SYSTEM,  (char *)&CFG_SYS, sizeof(struct SB_SYSTEM_CONFIG));	
		SB_WriteConfig (CFGFILE_FLASH_SYSTEM,(char *)&CFG_SYS, sizeof(struct SB_SYSTEM_CONFIG));

		SB_ReadConfig  (CFGFILE_ETC_SIO,     (char *)&CFG_SIO[0], sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);	
		SB_WriteConfig (CFGFILE_FLASH_SIO,   (char *)&CFG_SIO[0], sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);

		SB_ReadConfig  (CFGFILE_ETC_SNMP,    (char *)&CFG_SNMP, sizeof(struct SB_SNMP_CONFIG));	
		SB_WriteConfig (CFGFILE_FLASH_SNMP,  (char *)&CFG_SNMP, sizeof(struct SB_SNMP_CONFIG));

		SB_ReadConfig  (CFGFILE_ETC_GPIO,    (char *)&CFG_GPIO, sizeof(struct SB_GPIO_CONFIG));	
		SB_WriteConfig (CFGFILE_FLASH_GPIO,  (char *)&CFG_GPIO, sizeof(struct SB_GPIO_CONFIG));

		SB_ReadConfig  (CFGFILE_ETC_DIO,     (char *)&CFG_DIO, sizeof(struct SB_DIO_CONFIG));	
		SB_WriteConfig (CFGFILE_FLASH_DIO,   (char *)&CFG_DIO, sizeof(struct SB_DIO_CONFIG));

		SB_ReadConfig  (CFGFILE_ETC_WIFI, (char *)&CFG_WIFI, sizeof(struct SB_WIFI_CONFIG));
		SB_WriteConfig (CFGFILE_FLASH_WIFI, (char *)&CFG_WIFI, sizeof(struct SB_WIFI_CONFIG));
		
		printf ("Flash Write Successfully\n");
		write_web_login ();
		return 0;
		}

    if (!strncmp("factory", argv[1], 4)) 
		{
		char MAC[10] = { 0x00, 0x05, 0xf4, 0x00, 0x20, 0x57 };	 // Factory MAC address
			
		if (SB_ReadConfig  (CFGFILE_ETC_SYSTEM,  (char *)&CFG_SYS, sizeof(struct SB_SYSTEM_CONFIG)) > 0)
			memcpy (MAC, CFG_SYS.mac, 6);

		factory_default (MAC);
		printf ("Factory Default Successfully\n");
	   	return 0;
    	}

    if (!strncmp("view", argv[1], 2)) 
    	{
		SB_ReadConfig  (CFGFILE_ETC_SYSTEM,  (char *)&CFG_SYS, sizeof(struct SB_SYSTEM_CONFIG));
	   	if (argc == 2) 
			{
			view_wan();
			if (Product_ID != EDDY_S4M) view_lan();
			view_wifi ();
			view_management ();
			view_snmp();
			view_port ();
			view_gpio();
			if (Product_ID != EDDY_S4M) view_dio ();
			return 0;
			}	
		if (!strncmp ("management", argv[2], 2))  view_management ();
		if (!strncmp ("wan", 		argv[2], 2))  view_wan ();
		if (!strncmp ("wifi", 		argv[2], 2))  view_wifi ();	
		if (!strncmp ("lan", 		argv[2], 2) && Product_ID != EDDY_S4M)  view_lan ();		
		if (!strncmp ("serial",   	argv[2], 2))  view_port ();	
		if (!strncmp ("gpio",   	argv[2], 2))  view_gpio ();		
		if (!strncmp ("dio",   		argv[2], 2) && Product_ID != EDDY_S4M)  view_dio ();
		if (!strncmp ("snmp",   	argv[2], 2))  view_snmp ();
		return 0; 
		}


    if (!strncmp("port", argv[1], 2) && argc == 5)
		{
		SB_ReadConfig  (CFGFILE_ETC_SIO,  (char *)&CFG_SIO[0], sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);
		if (set_port (argv[2], argv[3], argv[4]) == 1) 	
			{
			SB_WriteConfig  (CFGFILE_ETC_SIO,  (char *)&CFG_SIO[0], sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);
			}
		else
			{
		  	printf("def port <port number> <command> <input data>\n");
		  	printf("If you need help, execute 'def help' or 'help'\n");
			}
	  	return 0;
    	}

    if (!strncmp("wifi", argv[1], 2) && argc == 4)
		{
	    SB_ReadConfig  (CFGFILE_ETC_WIFI, (char *)&CFG_WIFI, sizeof(struct SB_WIFI_CONFIG));
        if ( set_wifi (argc, argv[2], argv[3]) == 1 ) 
	   	    {
		    SB_WriteConfig (CFGFILE_ETC_WIFI, (char *)&CFG_WIFI, sizeof(struct SB_WIFI_CONFIG));
    	    }
	   else
			{
			printf("def <command> <input data>\n");
			printf("If you need help, execute 'def help' or 'help'\n");
			}
		return 0;    	   
    	}

   SB_ReadConfig  (CFGFILE_ETC_SYSTEM,  (char *)&CFG_SYS,  sizeof(struct SB_SYSTEM_CONFIG));	
   SB_ReadConfig  (CFGFILE_ETC_SNMP,    (char *)&CFG_SNMP, sizeof(struct SB_SNMP_CONFIG));	
   if ( set_wan (argc, argv[1], argv[2]) == 1 ) 
	   	{
		SB_WriteConfig  (CFGFILE_ETC_SYSTEM,  (char *)&CFG_SYS, sizeof(struct SB_SYSTEM_CONFIG));
		SB_WriteConfig  (CFGFILE_ETC_SNMP,    (char *)&CFG_SNMP, sizeof(struct SB_SNMP_CONFIG));	
	   	}
   else
		{
		printf("def <command> <input data>\n");
		printf("If you need help, execute 'def help' or 'help'\n");
		}
	return 0;
}
//=============================================================
void write_web_login ()
{
int fd;	

	unlink ("/var/tmp/login.id");
	unlink ("/var/tmp/login.pw");
	fd = open("/var/tmp/login.id", O_WRONLY | O_CREAT);
	if (fd > 0)
		{
		write(fd, CFG_SYS.username, strlen(CFG_SYS.username));
		close(fd);
		}
	fd = open("/var/tmp/login.pw", O_WRONLY | O_CREAT);
	if (fd > 0)
		{
		write(fd, CFG_SYS.password, strlen(CFG_SYS.password));
		close(fd);
		}
}			
//=============================================================
void factory_default (char *MAC)
{
int no;

	memcpy  (CFG_SYS.mac,		MAC, 			6);
/*
	sprintf (CFG_SYS.ip,		"%c%c%c%c",     192,168,0,223);
	sprintf (CFG_SYS.mask,		"%c%c%c%c",     255,255,255,0);
	sprintf (CFG_SYS.gateway,	"%c%c%c%c",     192,168,0,254);
	sprintf (CFG_SYS.dns,		"%c%c%c%c",     168,126,63,1);
	CFG_SYS.line 				= 'I'; // Static IP
*/
	CFG_SYS.line 				= 'D'; // DHCP

	CFG_SYS.dhcpenable			= 0;
	sprintf (CFG_SYS.lanip,			"%c%c%c%c",     10,10,1,1);
	sprintf (CFG_SYS.lanmask,		"%c%c%c%c",     255,255,255,0);
	sprintf (CFG_SYS.dhcpstartIP,	"%c%c%c%c",     10,10,1,2);
	sprintf (CFG_SYS.dhcpendIP,		"%c%c%c%c",     10,10,1,30);		
	CFG_SYS.lease_time			= 180;
		
/*
	sprintf (CFG_SYS.ddns,		"%c%c%c%c",     203,32,117,1);
	sprintf (CFG_SYS.portview,	"%c%c%c%c",     0,0,0,0);
*/
	sprintf (CFG_SYS.device_name,"Eddy");
	sprintf (CFG_SYS.username,	"eddy");
	sprintf (CFG_SYS.password,	"99999999");
	CFG_SYS.portview_port_no		= 4000;
	CFG_SYS.telnet_server		= SB_DISABLE;
	CFG_SYS.ftp_server			= SB_DISABLE;
	CFG_SYS.web_server			= SB_DISABLE;
	CFG_SYS.ssh_server			= SB_DISABLE;
	CFG_SYS.target_agent			= SB_DISABLE;
	sprintf (CFG_SYS.website,	"http://www.dsintek.com");
	sprintf (CFG_SYS.contact,	"jsLee@DSI");
	sprintf (CFG_SYS.ddnsuser,	"eddy");
	sprintf (CFG_SYS.ddnspass,	"99999999");
	sprintf (CFG_SYS.id,		"%s",			SB_DEVICE_ID);		
	SB_WriteConfig (CFGFILE_FLASH_SYSTEM, (char *)&CFG_SYS,  sizeof(struct SB_SYSTEM_CONFIG));
	SB_WriteConfig (CFGFILE_ETC_SYSTEM,   (char *)&CFG_SYS,  sizeof(struct SB_SYSTEM_CONFIG));	
	
	
	for (no=0; no<SB_MAX_SIO_PORT; no++)
		{
		sprintf (CFG_SIO[no].name,		"Port-%02d",	no+1);
		CFG_SIO[no].socket_no		=	4001 + no;
		CFG_SIO[no].protocol		=	SB_COM_REDIRECT_MODE;		
		CFG_SIO[no].device			=   SB_DATA_TYPE;
		CFG_SIO[no].speed			=	6;						// 9600
		CFG_SIO[no].dps				= 	0x03;					// n/8/1
		CFG_SIO[no].flow			=	SB_FLOW_NONE;
		sprintf (CFG_SIO[no].remote_ip, "%c%c%c%c",	0,0,0,0);
		CFG_SIO[no].remote_socket_no=	4000;
		CFG_SIO[no].packet_latency_time	= 0;
		CFG_SIO[no].keepalive		=	0;
		CFG_SIO[no].interface		=	SB_RS232;
		CFG_SIO[no].login			=	SB_DISABLE;
		sprintf (CFG_SIO[no].login_name,		"none");
		sprintf (CFG_SIO[no].login_password,	"none");
		}		
	CFG_SIO[2].interface		=	SB_RS422_PTOP;
	CFG_SIO[3].interface		=	SB_RS422_PTOP;	
		
	SB_WriteConfig (CFGFILE_FLASH_SIO, (char *)&CFG_SIO[0],  sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);
	SB_WriteConfig (CFGFILE_ETC_SIO,   (char *)&CFG_SIO[0],  sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);

	CFG_GPIO.serial_1			=	SB_DISABLE;
	CFG_GPIO.serial_2			=	SB_DISABLE;
	CFG_GPIO.serial_3			=	SB_DISABLE;
	CFG_GPIO.serial_4			=	SB_DISABLE;
	CFG_GPIO.debugport			=	SB_ENABLE;
	CFG_GPIO.lan				=	SB_DISABLE;
	CFG_GPIO.adc				=	SB_DISABLE;
	CFG_GPIO.reset				=	SB_DISABLE;
	CFG_GPIO.rdy_led			=	SB_ENABLE;
	CFG_GPIO.spi_eeprom			=	SB_DISABLE;
	CFG_GPIO.nandflash			=	SB_DISABLE;

	if (Product_ID != EDDY_S4M)
		{
		CFG_GPIO.keypad			=	SB_ENABLE;

		CFG_GPIO.enable[0]		=	0x00400000;	// GPIO Port A,	 (serial 3)
		CFG_GPIO.enable[1]		=	0xc03f300f;	// GPIO Port B,  (serial 1 ~ 4)
//		CFG_GPIO.enable[2]		=	0x04fed22f;	// GPIO Port C,	 (serial 4, debugport, Rdy, reset)
		CFG_GPIO.enable[2]		=	0x06fed22f;	// GPIO Port C,	 (serial 4, debugport, Rdy, reset) + PC25 (Bluetooth)
		}
	else
		{
		CFG_GPIO.keypad			=	SB_DISABLE;
			
		CFG_GPIO.enable[0]		=	S4M_GPIO_MASK_PORT_A;	// GPIO Port A,	 (serial 3)
		CFG_GPIO.enable[1]		=	S4M_GPIO_MASK_PORT_B;	// GPIO Port B,  (serial 1 ~ 4)
		CFG_GPIO.enable[2]		=	S4M_GPIO_MASK_PORT_C;	// GPIO Port C,	 (serial 4, debugport, Rdy, reset)
		}
		
	CFG_GPIO.mode[0]			=	0x00000000;
	CFG_GPIO.mode[1]			=	0x00000000;
	CFG_GPIO.mode[2]			=	0x00000000;
	CFG_GPIO.value[0]			=	0x00000000;
	CFG_GPIO.value[1]			=	0x00000000;
	CFG_GPIO.value[2]			=	0x00000000;
	CFG_GPIO.pullup[0]			=	0x00000000;
	CFG_GPIO.pullup[1]			=	0x00000000;
	CFG_GPIO.pullup[2]			=	0x00000000;
	SB_WriteConfig (CFGFILE_FLASH_GPIO, (char *)&CFG_GPIO,  sizeof(struct SB_GPIO_CONFIG));
	SB_WriteConfig (CFGFILE_ETC_GPIO,   (char *)&CFG_GPIO,  sizeof(struct SB_GPIO_CONFIG));

	CFG_DIO.mode	= 0x0000;		// output mode
	CFG_DIO.value   = 0x0000;		// low
	CFG_DIO.lcd		= SB_ENABLE;	// use to LCD
	SB_WriteConfig (CFGFILE_FLASH_DIO, (char *)&CFG_DIO,  sizeof(struct SB_DIO_CONFIG));
	SB_WriteConfig (CFGFILE_ETC_DIO,   (char *)&CFG_DIO,  sizeof(struct SB_DIO_CONFIG));

	CFG_SNMP.enable						= SB_DISABLE;
	sprintf (CFG_SNMP.trap_ip,	"%c%c%c%c",     0,0,0,0);
	CFG_SNMP.trap_port			= 162;
	CFG_SNMP.trap_option			= 1;
	CFG_SNMP.v1_readwrite			= 0;
	CFG_SNMP.v3_readwrite			= 0;
	sprintf (CFG_SNMP.v3_username,	"eddy");
	sprintf (CFG_SNMP.v3_password,	"administrator");
	SB_WriteConfig (CFGFILE_FLASH_SNMP, (char *)&CFG_SNMP,  sizeof(struct SB_SNMP_CONFIG));
	SB_WriteConfig (CFGFILE_ETC_SNMP,   (char *)&CFG_SNMP,  sizeof(struct SB_SNMP_CONFIG));


	CFG_WIFI.wireless = SB_DISABLE;			//Don't represent on the main
	CFG_WIFI.wifi_mode = SB_DISABLE;			//Infra
	sprintf (CFG_WIFI.ssid, "none");
	CFG_WIFI.channel = SB_DISABLE;			//Auto
	CFG_WIFI.auth_mode = 3;					//WPA2-PSK
	CFG_WIFI.encryp_type = 2;				//TKIP
	CFG_WIFI.key_index = SB_DISABLE;			//1
	sprintf (CFG_WIFI.key, "none");
	sprintf (CFG_WIFI.passphrase, "none");
	CFG_WIFI.roaming_mode = SB_DISABLE;		//Disable
	CFG_WIFI.roaming_threshold = 75;
	CFG_WIFI.scanning_period = 60;

	CFG_WIFI.b_g_mode = 0;			//Wireless NW mode   0=b/g, 1=B only, 2=A only, 3= A/B/G, 4=G only
	CFG_WIFI.bit_rate = 0;			// Bit rate, 0 = auto
	//int	sens_thr;	// Sensitivity threshold
	CFG_WIFI.rts_thr = 2347;		// RTS threshold, default: 2347
	CFG_WIFI.frag_thr = 2346;		// Fragment threshold, default: 2346
	CFG_WIFI.ieee80211h = 0;		// 802.11h spectrum management (Disable)

	CFG_WIFI.line = 0;			// DHCP
	sprintf (CFG_WIFI.ip,		"%c%c%c%c",     192,168,1,72);
	sprintf (CFG_WIFI.mask,		"%c%c%c%c",     255,255,255,0);
	sprintf (CFG_WIFI.gateway,	"%c%c%c%c",     192,168,1,1);
	sprintf (CFG_WIFI.dns,		"%c%c%c%c",     168,126,63,1);

	SB_WriteConfig (CFGFILE_FLASH_WIFI, (char *)&CFG_WIFI, sizeof(struct SB_WIFI_CONFIG));
	SB_WriteConfig (CFGFILE_ETC_WIFI, (char *)&CFG_WIFI, sizeof(struct SB_WIFI_CONFIG));
}
//=============================================================
int set_wan (int gc, char *gv1, char *gv2)
{
int i,j,len, ret;
char mac[30],x1,x2;


    if( gc < 3 ) return 0;

    if (!strncmp("line", gv1, 2 ))
		{
		if (!strncmp ("dhcp", gv2, 2 )) 
			CFG_SYS.line = 'D';
	    else
	    	if (!strncmp ("ip",   gv2, 2 )) 
	    		CFG_SYS.line = 'I';			
	    	else
	    		return 0;	
	  	return 1;
		}
    
    if (!strncmp("name", gv1, 2 ))
		{
		len = strlen (gv2);
		gv2[31] = 0x00;
		memset (CFG_SYS.device_name, 0x00, 32);
		strcpy (CFG_SYS.device_name, gv2);
		return 1;
		}

    if (!strncmp("mac", gv1, 3 ))
		{
		len = strlen (gv2);
		if (len < 12) return 0;
		for (j=0,i=0; i<len; i++)
			{
			if (gv2[i] == ':' || gv2[i] == '-') continue;
			mac [j++] = gv2[i];
			}
		mac [j] = 0x00;	
		if (strlen(mac) != 12) return 0;
		for (i=0; i<6; i++)
			{
			x1 = mac[i*2];
			if (x1 >= '0' && x1 <= '9') x1 = x1 - '0';
			if (x1 >= 'a' && x1 <= 'f') x1 = (x1 - 'a') + 10;
			x2 = mac[i*2+1];
			if (x2 >= '0' && x2 <= '9') x2 = x2 - '0';
			if (x2 >= 'a' && x2 <= 'f') x2 = (x2 - 'a') + 10;
			CFG_SYS.mac [i] = (x1 << 4) | x2;
			}
		return 1;
		}

    if (!strncmp("portviewport", gv1, 9 ))
		{
		CFG_SYS.portview_port_no = atoi (gv2);
		return 1;
		}

    if (!strncmp("ddnsuser", gv1, 6))
		{
		gv2[15] = 0x00;
		memset (CFG_SYS.ddnsuser, 0x00, 16);
		strcpy (CFG_SYS.ddnsuser, gv2);
		return 1;
		}

    if (!strncmp("ddnspass", gv1, 6 ))
		{
		gv2[15] = 0x00;
		memset (CFG_SYS.ddnspass, 0x00, 16);
		strcpy (CFG_SYS.ddnspass, gv2);
		return 1;
		}

    if (!strncmp("ip", gv1, 2 ))		return (SB_Ip2Hex (gv2, CFG_SYS.ip));
    if (!strncmp("mask", gv1, 3 ))		return (SB_Ip2Hex (gv2, CFG_SYS.mask));
    if (!strncmp("gateway", gv1, 3)) 	return (SB_Ip2Hex (gv2, CFG_SYS.gateway));
    if (!strncmp("dns", gv1, 3 ))		return (SB_Ip2Hex (gv2, CFG_SYS.dns));
    if (!strncmp("ddns", gv1, 3 ))		return (SB_Ip2Hex (gv2, CFG_SYS.ddns));
    if (!strncmp("portview", gv1, 5 ))	return (SB_Ip2Hex (gv2, CFG_SYS.portview));
	if (!strncmp("trapip", gv1, 5 ))	return (SB_Ip2Hex (gv2, CFG_SNMP.trap_ip));

	if (!strncmp("lanip", gv1, 5 ))	    return (SB_Ip2Hex (gv2, CFG_SYS.lanip));
	if (!strncmp("lanmask", gv1, 5 ))	return (SB_Ip2Hex (gv2, CFG_SYS.lanmask));
	if (!strncmp("lanstart", gv1, 5 ))	return (SB_Ip2Hex (gv2, CFG_SYS.dhcpstartIP));
	if (!strncmp("lanend", gv1, 5 ))	return (SB_Ip2Hex (gv2, CFG_SYS.dhcpendIP));	
	

     if (!strncmp("lanleasetime", gv1, 6))
		{
		ret = atoi (gv2);
		if (ret < 0 || ret > 65535) return 0;
		CFG_SYS.lease_time = ret;
		return 1;
		}	

    if (!strncmp("landhcp", gv1, 3 ))
		{
		if (!strncmp("enable", gv2, 2 )) 
			CFG_SYS.dhcpenable = SB_ENABLE;
		else
			if (!strncmp("disable", gv2, 2 )) 
				CFG_SYS.dhcpenable = SB_DISABLE;
			else
				return 0;			
		return 1;
		}		

    if (!strncmp("username", gv1, 4 ))
		{
		gv2[15] = 0x00;
		memset (CFG_SYS.username, 0x00, 16);
		strcpy (CFG_SYS.username, gv2);
		return 1;
		}

    if (!strncmp("password", gv1, 4 ))
		{
		gv2[15] = 0x00;
		memset (CFG_SYS.password, 0x00, 16);
		strcpy (CFG_SYS.password, gv2);
		return 1;
		}

    if (!strncmp("snmp", gv1, 4 ))
		{
		if (!strncmp("enable", gv2, 2 )) 
			CFG_SNMP.enable = SB_ENABLE;
		else
			if (!strncmp("disable", gv2, 2 )) 
				CFG_SNMP.enable = SB_DISABLE;
			else
				return 0;			
		return 1;
		}

    if (!strncmp("ftp", gv1, 3 ))
		{
		if (!strncmp("enable", gv2, 2 )) 
			CFG_SYS.ftp_server = SB_ENABLE;
		else
			if (!strncmp("disable", gv2, 2 )) 
				CFG_SYS.ftp_server = SB_DISABLE;
			else
				return 0;			
		return 1;
		}

    if (!strncmp("telnet", gv1, 3 ))
		{
		if (!strncmp("enable", gv2, 2 )) 
			CFG_SYS.telnet_server = SB_ENABLE;
		else
			if (!strncmp("disable", gv2, 2 )) 
				CFG_SYS.telnet_server = SB_DISABLE;
			else
				return 0;			
		return 1;
		}

    if (!strncmp("ssh", gv1, 3 ))
		{
		if (!strncmp("enable", gv2, 2 )) 
			CFG_SYS.ssh_server = SB_ENABLE;
		else
			if (!strncmp("disable", gv2, 2 )) 
				CFG_SYS.ssh_server = SB_DISABLE;
			else
				return 0;			
		return 1;
		}

    if (!strncmp("target_agent", gv1, 3 ))
		{
		if (!strncmp("enable", gv2, 2 )) 
			CFG_SYS.target_agent = SB_ENABLE;
		else
			if (!strncmp("disable", gv2, 2 )) 
				CFG_SYS.target_agent = SB_DISABLE;
			else
				return 0;			
		return 1;
		}

    if (!strncmp("web", gv1, 3 ))
		{
		if (!strncmp("enable", gv2, 2 )) 
			CFG_SYS.web_server = SB_ENABLE;
		else
			if (!strncmp("disable", gv2, 2 )) 
				CFG_SYS.web_server = SB_DISABLE;
			else
				return 0;			
		return 1;
		}

    if (!strncmp("v1readwrite", gv1, 3 ))
		{
		if (!strncmp("readwrite", gv2, 5 )) 
			CFG_SNMP.v1_readwrite = SB_ENABLE;
		else
			if (!strncmp("readonly", gv2, 5 )) 
				CFG_SNMP.v1_readwrite = SB_DISABLE;
			else
				if (!strncmp("disable", gv2, 3)) 
					CFG_SNMP.v1_readwrite = 2;		// disable
				else
					return 0;	
		return 1;
		}

    if (!strncmp("v3readwrite", gv1, 3 ))
		{
		if (!strncmp("readwrite", gv2, 5 )) 
			CFG_SNMP.v3_readwrite = SB_ENABLE;
		else
			if (!strncmp("readonly", gv2, 5 )) 
				CFG_SNMP.v3_readwrite = SB_DISABLE;
			else
				return 0;			
		return 1;
		}

    if (!strncmp("trapport", gv1, 5 ))
		{
		CFG_SNMP.trap_port = atoi (gv2);
		return 1;
		}

    if (!strncmp("v3username", gv1, 4 ))
		{
		gv2[15] = 0x00;
		memset (CFG_SNMP.v3_username, 0x00, 16);
		strcpy (CFG_SNMP.v3_username, gv2);
		return 1;
		}

    if (!strncmp("v3password", gv1, 4 ))
		{
		gv2[15] = 0x00;
		if (strlen(gv2) < 8) 
			{
			printf ("Password is at least 8 character !\n");
			return 0;
			}
		memset (CFG_SNMP.v3_password, 0x00, 16);
		strcpy (CFG_SNMP.v3_password, gv2);
		return 1;
		}


    if (!strncmp("trap_reset", gv1, 6 ))
		{
		if (!strncmp("enable", gv2, 2 )) 
			CFG_SNMP.trap_option |= 0x01;
		else
			if (!strncmp("disable", gv2, 2 )) 
				CFG_SNMP.trap_option &= 0xfe;
			else
				return 0;			
		return 1;
		}

    if (!strncmp("trap_connect", gv1, 6 ))
		{
		if (!strncmp("enable", gv2, 2 )) 
			CFG_SNMP.trap_option |= 0x02;
		else
			if (!strncmp("disable", gv2, 2 )) 
				CFG_SNMP.trap_option &= 0xfd;
			else
				return 0;			
		return 1;
		}

    if (!strncmp("trap_disconnect", gv1, 6 ))
		{
		if (!strncmp("enable", gv2, 2 )) 
			CFG_SNMP.trap_option |= 0x04;
		else
			if (!strncmp("disable", gv2, 2 )) 
				CFG_SNMP.trap_option &= 0xfb;
			else
				return 0;			
		return 1;
		}

	return 0;
} 
//=============================================================
int set_wifi (int gc, char *gv1, char *gv2)
{
int i,len;

    if( gc < 4 ) return 0;

    if (!strncmp("wireless", gv1, 2 ))
		{
		if (!strncmp ("enable", gv2, 2 )) 
			CFG_WIFI.wireless = SB_ENABLE;
	    else
	    	if (!strncmp ("disable", gv2, 2 )) 
	    		CFG_WIFI.wireless = SB_DISABLE;
	    	else
	    		return 0;	
	  	return 1;
		}

    if (!strncmp("mode", gv1, 2 ))
		{
		if (!strncmp ("infrastructure", gv2, 2 )) 
			CFG_WIFI.wifi_mode = SB_DISABLE;
	    else
	    	if (!strncmp ("ad-hoc", gv2, 2 )) 
	    		CFG_WIFI.wifi_mode = SB_ENABLE;
	    	else
	    		return 0;	
	  	return 1;
		}

    if (!strncmp("network", gv1, 2 ))
		{
		if (!strncmp ("802.11b/g mixed", gv2, 9)) 
			CFG_WIFI.b_g_mode = 0;
	    else
	    	if (!strncmp ("802.11b only", gv2, 7 )) 
	    		CFG_WIFI.b_g_mode = 1;
	    	else
//	    		if (!strncmp ("802.11a/b/g only", gv2, 9)) 
//	    			CFG_WIFI.b_g_mode = 3;
//	    		else
//	    			if (!strncmp ("802.11a/b/g mixed", gv2, 7)) 
//	    				CFG_WIFI.b_g_mode = 2;	
//	    			else
	    				if (!strncmp ("802.11g only", gv2, 7 )) 
	    					CFG_WIFI.b_g_mode = 4;	
	    				else
	    					return 0;	
	  	return 1;
		}

    if (!strncmp("ssid", gv1, 3))
		{
		len = strlen (gv2);
		gv2[31] = 0x00;
		memset (CFG_WIFI.ssid, 0x00, 32);
		strcpy (CFG_WIFI.ssid, gv2);
		return 1;
		}

    if (!strncmp("channel", gv1, 3))
		{
		if (!strncmp ("auto", gv2, 2 )) 
			CFG_WIFI.channel = 0;
	    else
	    	{
	    	i = atoi (gv2);
	    	if (i >= 0 && i < 14) 
	    		CFG_WIFI.channel = i;
	    	else
	    		return 0;	
			}
	  	return 1;
		}

    if (!strncmp("bitrate", gv1, 3))
		{
		if (!strncmp ("auto", gv2, 2 )) 
			CFG_WIFI.bit_rate = 0;
	    else
			{
			i = atoi (gv2);
			if (i==1) 	
				CFG_WIFI.bit_rate = 1;
			else
				if (i==2) 
					CFG_WIFI.bit_rate = 2;	
				else
					if (i==5) 
						CFG_WIFI.bit_rate = 3;	
					else
						if (i==11) 
							CFG_WIFI.bit_rate = 4;	
						else
							if (i==6)
								CFG_WIFI.bit_rate = 5;	
							else
								if (i==9)
									CFG_WIFI.bit_rate = 6;	
								else
									if (i==12)
										CFG_WIFI.bit_rate = 7;		
									else
										if (i==18)
											CFG_WIFI.bit_rate = 8;	
										else
											if (i==24)
												CFG_WIFI.bit_rate = 9;		
											else
												if (i==36)
													CFG_WIFI.bit_rate = 10;
												else
													if (i==48)
														CFG_WIFI.bit_rate = 11;
													else
														if (i==54)
															CFG_WIFI.bit_rate = 12;
														else
															return 0;
			}
	  	return 1;
		}

    if (!strncmp("rts", gv1, 3 ))
		{
		i = atoi (gv2);
		if (i > 0 && i < 2348)
			CFG_WIFI.rts_thr = i;
		else
			return 0;	
		return 1;
		}
    
    if (!strncmp("fragment", gv1, 3 ))
		{
		i = atoi (gv2);
		if (i > 255 && i < 2347)
			CFG_WIFI.frag_thr = i;
		else
			return 0;	
		return 1;
		}    
    
    /*if (!strncmp("802.11h", gv1, 2 ))
		{
		if (!strncmp ("enable", gv2, 2 )) 
			CFG_WIFI.ieee80211h = SB_ENABLE;
	    else
	    	if (!strncmp ("disable", gv2, 2 )) 
	    		CFG_WIFI.ieee80211h = SB_DISABLE;
	    	else
	    		return 0;	
	  	return 1;
		}    
        
    if (!strncmp("roamming", gv1, 3))
		{
		if (!strncmp ("enable", gv2, 2 )) 
			CFG_WIFI.roaming_mode = SB_ENABLE;
	    else
	    	if (!strncmp ("disable", gv2, 2 )) 
	    		CFG_WIFI.roaming_mode = SB_DISABLE;
	    	else
	    		return 0;	
	  	return 1;
		}    */
        
    if (!strncmp("authentication", gv1, 3 ))
		{
		if (!strncmp ("auto", gv2, 4)) 
			CFG_WIFI.auth_mode = 0;
	    else
	    	if (!strncmp ("open", gv2, 4 )) 
	    		CFG_WIFI.auth_mode = 1;
	    	else
	    		if (!strncmp ("share", gv2, 5 )) 
	    			CFG_WIFI.auth_mode = 2;
	    		else
	    			if (!strncmp ("wpapsk", gv2, 5)) 
	    				CFG_WIFI.auth_mode = 3;	
	    			else
	    				if (!strncmp ("wpa2psk", gv2, 5 )) 
	    					CFG_WIFI.auth_mode = 4;	
	    				else
	    					return 0;	
	  	return 1;
		}    
    
    if (!strncmp("encryption", gv1, 3 ))
		{
		if (!strncmp ("none", gv2, 4)) 
			CFG_WIFI.encryp_type = 0;
	    else
	    	if (!strncmp ("wep64", gv2, 4 )) 
	    		CFG_WIFI.encryp_type = 1;
	    	else
	    		if (!strncmp ("wep128", gv2, 4 )) 
	    			CFG_WIFI.encryp_type = 2;
	    		else
	    			if (!strncmp ("tkip", gv2, 4)) 
	    				CFG_WIFI.encryp_type = 3;	
	    				else
						if (!strncmp ("aes", gv2, 3)) 
							CFG_WIFI.encryp_type = 4;	
							else
							if (!strncmp ("tkip/aes", gv2, 5)) 
								CFG_WIFI.encryp_type = 5;	
								else							
	    					return 0;	
	  	return 1;
		}      
    
    if (!strncmp("password", gv1, 3))
		{
		len = strlen (gv2);
		gv2[len] = 0x00;
		memset (CFG_WIFI.passphrase, 0x00, 60);
		strcpy (CFG_WIFI.passphrase, gv2);
		return 1;
		}    
    
    if (!strncmp("keyindex", gv1, 4))
		{
		i = atoi (gv2);
		if (i > 0 && i < 5)
			CFG_WIFI.key_index = i-1;
		else
			return 0;	
		return 1;
		}       
    
    if (!strncmp("key", gv1, 3))
		{
		len = strlen (gv2);
		gv2[len] = 0x00;
		memset (CFG_WIFI.key, 0x00, 32);
		strcpy (CFG_WIFI.key, gv2);
		return 1;
		}      
    
    if (!strncmp("line", gv1, 3 ))
		{
		if (!strncmp ("dhcp", gv2, 4)) 
			CFG_WIFI.line = 0;
	    else
	    	if (!strncmp ("static", gv2, 4 )) 
	    		CFG_WIFI.line = 1;
			else
				return 0;	
	  	return 1;
		} 

    if (!strncmp("ip", gv1, 2 ))		return (SB_Ip2Hex (gv2, CFG_WIFI.ip));
    if (!strncmp("mask", gv1, 3 ))		return (SB_Ip2Hex (gv2, CFG_WIFI.mask));
    if (!strncmp("gateway", gv1, 3)) 	return (SB_Ip2Hex (gv2, CFG_WIFI.gateway));
    if (!strncmp("dns", gv1, 3 ))		return (SB_Ip2Hex (gv2, CFG_WIFI.dns));

	return 0;
} 
//=============================================================
int set_port (char *gv1, char *gv2, char *gv3)
{
int Ports, Fport, Lport, no, ret, fd;
char cmd[100], tmp1[5], tmp2[5];
int tmp_cnt1, tmp_cnt2, i;

	Ports = SB_MAX_SIO_PORT;

	if (!strncmp("all", gv1, 2)) 
		{ Fport = 0; Lport = Ports-1; }
	else
		{	// 1-4, 8-10, 11-14, x
		if (gv1[1] == '-' || gv1[2] == '-')
			{
			memset (tmp1, 0x00, 5);
			memset (tmp2, 0x00, 5);
			tmp_cnt1 = 0;
			tmp_cnt2 = 0;
			for (i=0; i<strlen(gv1); i++)
				{
				if (gv1[i] == '-') break;	
				if (gv1[i] >= '0' && gv1[i] <= '9') tmp1[tmp_cnt1++] = gv1[i];
				}
			
			for (i+=1; i<strlen(gv1); i++)
				{
				if (gv1[i] == '-') break;	
				if (gv1[i] >= '0' && gv1[i] <= '9') tmp2[tmp_cnt2++] = gv1[i];
				}
				
			Fport = atoi (tmp1) - 1;
			Lport = atoi (tmp2) - 1;
			if 	(Fport >= Ports || Fport < 0) return 0;
			if 	(Lport >= Ports || Lport < 0) return 0;	
			}
		else
			{
			Lport = Fport = atoi (gv1) - 1;
			if 	(Fport >= Ports || Fport < 0) return 0;
			if 	(Lport >= Ports || Lport < 0) return 0;	
			}
		}

	
    if (!strncmp("name", gv2, 2))
		{
		gv3[15] = 0x00;
		for (no=Fport; no<=Lport; no++)	
			{
			memset (CFG_SIO[no].name, 0x00, 16);
			strcpy (CFG_SIO[no].name, gv3);
			}
		return 1;
		}

    if (!strncmp("protocol", gv2, 3))
		{
		if (!strncmp("com_redir", gv3, 3)) 
			ret = SB_COM_REDIRECT_MODE;
		else
			if (!strncmp("disable", gv3, 3))
				ret = SB_DISABLE_MODE;
			else
				if (!strncmp("tcp_server", gv3, 5))
					ret = SB_TCP_SERVER_MODE;
				else
					if (!strncmp("tcp_client", gv3, 5))
						ret = SB_TCP_CLIENT_MODE;
					else
						if (!strncmp("tcp_broadcast", gv3, 5))
							ret = SB_TCP_BROADCAST_MODE;
						else
							if (!strncmp("tcp_multiplex", gv3, 5))
								ret = SB_TCP_MULTIPLEX_MODE;
							else
								if (!strncmp("udp_server", gv3, 5))
									ret = SB_UDP_SERVER_MODE;	
								else
									if (!strncmp("udp_client", gv3, 5))
										ret = SB_UDP_CLIENT_MODE;	
									else
										return 0;	
		for (no=Fport; no<=Lport; no++)	CFG_SIO[no].protocol = ret;
		return 1;
		}		

    if (!strncmp("socket", gv2, 3 ))
		{
		ret = atoi (gv3);
		if (ret < 1 || ret > 65535) return 0;
		for (no=Fport; no<=Lport; no++)	CFG_SIO[no].socket_no = ret;
		return 1;
		}	

    if (!strncmp("speed", gv2, 3))
		{
		switch (atoi(gv3))
			{
			case 150	: ret = 0; 	break;
			case 300	: ret = 1;	break;
			case 600	: ret = 2; 	break;
			case 1200	: ret = 3;	break;
			case 2400	: ret = 4; 	break;
			case 4800	: ret = 5; 	break;
			case 9600	: ret = 6; 	break;
			case 19200	: ret = 7; 	break;
			case 38400	: ret = 8; 	break;
			case 57600	: ret = 9; 	break;
			case 115200	: ret = 10; break;
			case 230400	: ret = 11; break;
			case 460800	: ret = 12; break;	
			case 921600	: ret = 13; break;													
			default 	: return 0;	
			}
		for (no=Fport; no<=Lport; no++)	CFG_SIO[no].speed = ret;
		return 1;
		}	

    if (!strncmp("flow", gv2, 3))
		{
		if (!strncmp("none", gv3, 2)) 
			ret = SB_FLOW_NONE;
		else
			if (!strncmp("xon", gv3, 2))
				ret = SB_FLOW_XON;
			else
				if (!strncmp("rts", gv3, 2))
					ret = SB_FLOW_RTS;
				else
					return 0;	
		for (no=Fport; no<=Lport; no++)	CFG_SIO[no].flow = ret;
		return 1;
		}

    if (!strncmp("device", gv2, 3))
		{
		if (!strncmp("data", gv3, 2)) 
			ret = SB_DATA_TYPE;
		else
			if (!strncmp("modem", gv3, 2))
				ret = SB_MODEM_TYPE;
			else
				return 0;	
		for (no=Fport; no<=Lport; no++)	CFG_SIO[no].device = ret;
		return 1;
		}

     if (!strncmp("remoteport", gv2, 7 ))
		{
		ret = atoi (gv3);
		if (ret < 0 || ret > 65000) return 0;
		for (no=Fport; no<=Lport; no++)	CFG_SIO[no].remote_socket_no = ret;
		return 1;
		}	
		
    if (!strncmp("remote", gv2, 3))	
    	{
    	for (no=Fport; no<=Lport; no++)	
    		{
    		ret =SB_Ip2Hex (gv3, CFG_SIO[no].remote_ip);
    		if (ret == 0) return 0;
    		}
    	return 1;
    	}	

    if (!strncmp("latency", gv2, 3 ))
		{
		ret = atoi (gv3);
		if (ret < 0 || ret > 65535) return 0;
		for (no=Fport; no<=Lport; no++)	CFG_SIO[no].packet_latency_time = ret;
		return 1;
		}

     if (!strncmp("keepalive", gv2, 4))
		{
		ret = atoi (gv3);
		if (ret < 0 || ret > 65535) return 0;
		for (no=Fport; no<=Lport; no++)	CFG_SIO[no].keepalive = ret;
		return 1;
		}			

    if (!strncmp("loginname", gv2, 6 ))
		{
		gv3[7] = 0x00;
		for (no=Fport; no<=Lport; no++)	
			{
			memset (CFG_SIO[no].login_name, 0x00, 8);
			strcpy (CFG_SIO[no].login_name, gv3);
			}
		return 1;
		}

    if (!strncmp("loginpass", gv2, 6 ))
		{
		gv3[7] = 0x00;
		for (no=Fport; no<=Lport; no++)	
			{
			memset (CFG_SIO[no].login_password, 0x00, 8);
			strcpy (CFG_SIO[no].login_password, gv3);
			}
		return 1;
		}

    if (!strncmp("login", gv2, 3 ))
		{
		if (!strncmp("enable", gv3, 2 )) 
			ret = SB_ENABLE;
		else
			if (!strncmp("disable", gv3, 2 )) 
				ret = SB_DISABLE;
			else
				return 0;	
		for (no=Fport; no<=Lport; no++)	CFG_SIO[no].login = ret;				
		return 1;
		}

     if (!strncmp("char", gv2, 3))
		{
		ret = atoi (gv3);
		if (ret < 5 || ret > 8) return 0;
		ret -= 5;
		for (no=Fport; no<=Lport; no++)	
			{
			CFG_SIO[no].dps &= 0xfc;
			CFG_SIO[no].dps |= (unsigned char)(ret);
			}
		return 1;
		}	

     if (!strncmp("stop", gv2, 3))
		{
		ret = atoi (gv3);
		if (ret < 1 || ret > 2) return 0;
		if (ret == 1) ret = 0x00; else ret = 0x04;
		for (no=Fport; no<=Lport; no++)	
			{
			CFG_SIO[no].dps &= 0xfb;	
			CFG_SIO[no].dps |= (unsigned char)(ret);
			}
		return 1;
		}	

    if (!strncmp("parity", gv2, 3 ))
		{
		if (!strncmp("none", gv3, 2 )) 
			ret = 0x00;
		else
			if (!strncmp("odd", gv3, 2 )) 
				ret = 0x08;
			else
				if (!strncmp("even", gv3, 2 )) 
					ret = 0x10;
				else
					return 0;			

		for (no=Fport; no<=Lport; no++)	
			{
			CFG_SIO[no].dps &= 0xe7;	
			CFG_SIO[no].dps |= (unsigned char)(ret);
			}
		return 1;
		}


   	if (!strncmp("interface", gv2, 3))
		{
		if (!(Fport >= 2 && Lport >= 2)) return 0;
		
		if (!strncmp("rs422", gv3, 5)) 
			ret = SB_RS422_PTOP;
		else
			if (!strncmp("rs485ne", gv3, 6))
				ret = SB_RS485_NONE_ECHO;
			else
				if (!strncmp("rs485e", gv3, 6))
					ret = SB_RS485_ECHO;
				else
					return 0;	
		for (no=Fport; no<=Lport; no++)	CFG_SIO[no].interface = ret;
		return 1;
		}


    if (!strncmp("debug", gv2, 4 ))
		{
		if (!strncmp("on", gv3, 2 )) 
			ret = 1;
		else
			if (!strncmp("off", gv3, 2 )) 
				ret = 0;
			else
				return 0;

		for (no=Fport; no<=Lport; no++)	
			{
			sprintf (cmd, "/var/run/debug-%d", no);
			if (ret==1)
				{
				fd = open(cmd, O_RDWR|O_CREAT, 0644);
				close (fd);	
				}
			else
				unlink (cmd);
			}
		return 1;
		}
	return 0;
}
void help()
{
	printf("---<View Command>---------------------------------------------------------\n");	
	printf("def view           ;  Print all Configuration\n");
	printf("def view wan       ;  Print WAN Configuration\n");
	printf("def view wifi      ;  Print WiFi Configuration\n");
	if (Product_ID != EDDY_S4M)
		printf("def view lan       ;  Print LAN Configuration\n");
	printf("def view management;  Print Management Configuration\n");
	printf("def view serial    ;  Print Serial Configuration\n");	
	printf("def view gpio      ;  Print GPIO Configuration\n");		
	if (Product_ID != EDDY_S4M)
		printf("def view dio       ;  Print DIO Configuration\n");		
	printf("---<Save & Factory>-------------------------------------------------------\n");	
	printf("def factory        ;  Factory Default\n");
	printf("def save           ;  Save to Flash (Configuration Data)\n");
	printf ("Press any key continue...\n");	getchar ();
	
	printf("---< WAN Config >---------------------------------------------------------\n");
	printf("def mac           <MAC Address>       ; WAN MAC Address\n");
	printf("def name          <string>            ; Device name\n");
	printf("def username      <string>            ; Telnet, WEB Login username\n");
	printf("def password      <string>            ; Telnet, WEB Login Password\n");	
	printf("def line          <ip, dhcp>          ; IP, DHCP\n");
	printf("def ip	          <address>           ; WAN IP address\n");
	printf("def mask          <submask>           ; WAN Subnet Mask\n");
	printf("def gateway       <address>           ; WAN GateWay address\n");
	printf("def dns           <address>           ; WAN DNS address\n");
	printf ("Press any key continue...\n");	getchar ();
	
	if (Product_ID != EDDY_S4M)
		{
		printf("---< LAN Config >---------------------------------------------------------\n");
		printf("def landhcp       <enable, disable>   ; LAN DHCP Server Execution\n");	
		printf("def lanip         <address>           ; LAN IP address\n");
		printf("def lanmask       <address>           ; LAN subnet Mask\n");
		printf("def lanstart      <address>           ; LAN DHCP Lease Start IP address\n");
		printf("def lanend        <address>           ; LAN DHCP Lease End   IP address\n");
		printf("def lanleasetime  <time>              ; LAN DHCP Lease Time\n");
		printf ("Press any key continue...\n");	getchar ();
		}
	
	printf("---< Management Config >---------------------------------------------------\n");
	printf("def ftp           <enable, disable>   ; FTP Server Execution\n");
	printf("def telnet        <enable, disable>   ; Telnet Server Execution\n");	
	printf("def ssh           <enable, disable>   ; SSH Server Execution\n");
	printf("def web           <enable, disable>   ; WEB Server Execution\n");	
	printf("def ddns          <address>           ; DDNS address\n");
	printf("def ddnsuser      <address>           ; DDNS Loginname (http://ddns.nu)\n");
	printf("def ddnspass      <address>           ; DDNS Loginpass (http://ddns.nu)\n");		
	printf("def portview      <address>           ; PortView address\n");
	printf("def portviewport  <Socket No.>        ; PortView Socket Number\n");
	printf("def target_agent  <enable, disable>   ; LemonIDE Remote Debugger Execution\n");		
	printf("---< GPIO Config >--------------------------------------------------------\n");
	printf("                               <Set Browser only>\n");	

	printf ("Press any key continue...\n");	getchar ();
	printf("---< SNMP Config >--------------------------------------------------------\n");
	printf("def snmp          <enable, disable>   ; SNMP MIB V1/2/3 agent Execution\n");
	printf("def v1readwrite   <readonly, readwrite, disable>\n");
	printf("                  ; SNMP V1/2 Attribute Readonly or Read/Write or Disable\n");		
	printf("def v3readwrite   <readonly, readwrite> \n");
	printf("                  ; SNMP V3 Attribute Readonly or Read/Write\n");		
	printf("def v3username    <string>            ; SNMP V3 username\n");		
	printf("def v3password    <string>            ; SNMP V3 password\n");		
	printf("def trapip        <address>           ; SNMP trap IP address\n");
	printf("def trapoprt      <Socket No.>        ; SNMP trap Socket Number\n");
	printf("def trap_reset    <enable, disable>   ; SNMP trap System Reset\n");		
	printf("def trap_connect  <enable, disable>   ; SNMP trap port connected\n");		
	printf("def trap_disconnect <enable, disable> ; SNMP trap port disconnected\n");		
	
	printf ("Press any key continue...\n");	getchar ();
	printf("---< Port Config >--------------------------------------------------------\n");
	printf("def port <x, x-x, all> name         <Port alias name>\n" );
	printf("def port <x, x-x, all> protocol     <disable, com_redirect, tcp_server,\n");
	printf("                                     tcp_client, tcp_broadcast, tcp_multiplex,\n"); 
	printf("                                     udp_server, udp_client>\n");
	printf("def port <x, x-x, all> interface    <rs422, rs485ne, rs485e>\n" );
	printf("def port <x, x-x, all> socket       <1 ~ 65535>\n" );
	printf("def port <x, x-x, all> speed        <150, 300, 600, 1200, 2400, 4800, 9600,\n"); 
	printf("                                     19200, 38400, 57600, 115200, 230400,  \n"); 
	printf("                                     460800, 921600>\n"); 
	printf("def port <x, x-x, all> char         <5, 6, 7, 8>\n");
	printf("def port <x, x-x, all> parity       <none, odd, even>\n");
	printf("def port <x, x-x, all> stop         <1, 2>\n" );
	printf("def port <x, x-x, all> flow         <none, xon, rts>\n");
	printf("def port <x, x-x, all> device       <data, modem>\n");
	printf("def port <x, x-x, all> remote       <Destination IP Address>\n" );
	printf("def port <x, x-x, all> remoterport  <Destination Socket Number>\n" );	
	printf("def port <x, x-x, all> latency_time <0 ~ 65535 sec>\n" );
	printf("def port <x, x-x, all> keepalive    <0 ~ 65535 sec>\n" );
	printf("def port <x, x-x, all> login        <enable, disable>\n" );
	printf("def port <x, x-x, all> loginname    <tcp_server login username>\n");
	printf("def port <x, x-x, all> loginpass    <tcp_server login password>\n");
	printf ("Press any key continue...\n");	getchar ();
	
	printf("---< WiFi Config >--------------------------------------------------------\n");	
	printf("def wifi wireless       <Enable, Disable>\n" );
	printf("def wifi mode           <infrastructure, ad-hoc>\n" );
//	printf("def wifi network        <802.11b/g mixed, 802.11b only, 802.11a only\n");
//	printf("                         802.11a/b/g mixed, 802.11g only>\n");
	printf("def wifi network        <802.11b/g mixed, 802.11b only, 802.11g only>\n");
	printf("def wifi ssid           <network name in a wireless network>\n"); 
	printf("def wifi channel        <auto, 0 ~ 13>\n");
	printf("def wifi bitrate        <auto, 1, 2, 5, 6, 9, 11, 12, 18, 24, 36, 48, 54>\n");
	printf("def wifi rts            <1 ~ 2347>\n" );
	printf("def wifi fragment       <256 ~ 2346>\n" );
	//printf("def wifi 802.11h        <enable, disable>\n");
	//printf("def wifi roamming       <enable, disable>\n");
	printf("def wifi authentication <auto, open, share, wpapsk, wpa2psk>\n");
	printf("def wifi encryption     <none, wep64, wep128, tkip, aes, tkip/aes>\n");
	printf("def wifi keyindex       <1 ~ 4>\n");
	printf("def wifi key            <wep key string>\n");
	printf("def wifi password       <wpa password string>\n");
	printf("def wifi line           <dhcp, static IP>\n" );
	printf("def wifi ip             <IP address>\n" );	
	printf("def wifi gateway        <Gateway IP address>\n" );	
	printf("def wifi mask           <Subnet Mask address>\n" );	
	printf("def wifi dns            <DNS IP address>\n" );	
	printf("--------------------------------------------------------------------------\n");	
}

//=============================================================
void view_wan()
{
struct in_addr addr;
char ver[20];
union { char c[4]; unsigned int i; } q;

    printf("=< Welcome to Eddy Configuration Manager >=========================\n");
    SB_GetVersion ('B', ver);
    printf("BootLoader Version : %s\n", ver);
    SB_GetVersion ('K', ver);
    printf("Kernel     Version : %s\n", ver);
    SB_GetVersion ('F', ver);
    printf("Firmware   Version : %s\n", ver);

	printf("-< WAN >------------------------------------------------------------\n");
	printf("MAC Address        : %02x:%02x:%02x:%02x:%02x:%02x\n", CFG_SYS.mac[0], CFG_SYS.mac[1], CFG_SYS.mac[2], CFG_SYS.mac[3], CFG_SYS.mac[4], CFG_SYS.mac[5]);
	printf("Network Line       : %s\n", (CFG_SYS.line == 'I') ? "Static IP" : "DHCP");

	addr.s_addr = SB_GetIp (SB_WAN_ETH_NAME);
	printf("IP Address         : %s  ", inet_ntoa(addr));
	memcpy (q.c, CFG_SYS.ip, 4);	addr.s_addr   = q.i;
	printf("(Config = %s)\n", inet_ntoa (addr));

	addr.s_addr = SB_GetMask (SB_WAN_ETH_NAME);
	printf("Subnet Mask        : %s  ", inet_ntoa(addr));
	memcpy (q.c, CFG_SYS.mask, 4);	addr.s_addr   = q.i;
	printf("(Config = %s)\n", inet_ntoa (addr));

   	addr.s_addr = SB_GetGateway ();
    printf("Gateway            : %s  ", inet_ntoa(addr));
	memcpy (q.c, CFG_SYS.gateway, 4);	addr.s_addr   = q.i;
	printf("(Config = %s)\n", inet_ntoa (addr));

	memcpy (q.c, CFG_SYS.dns, 4);	addr.s_addr   = q.i;
	printf("DNS                : %s\n", inet_ntoa (addr));
}
//=============================================================
void view_wifi()
{
struct in_addr addr;
char buf[40];
union { char c[4]; unsigned int i; } q;


	SB_ReadConfig  (CFGFILE_ETC_WIFI, (char *)&CFG_WIFI, sizeof(struct SB_WIFI_CONFIG));

	printf("-< WiFi >-----------------------------------------------------------\n");
	printf("Wireless           : %s\n", (CFG_WIFI.wireless == 0) ? "Disable" : "Enable");
	printf("WiFi Mode          : %s\n", (CFG_WIFI.wifi_mode == 0) ? "Infrastructure" : "AD-Hoc");
	switch (CFG_WIFI.b_g_mode)	{
		case 0 : strcpy (buf, "802.11b/g mixed"); break;
		case 1 : strcpy (buf, "802.11b only"); break;
//		case 2 : strcpy (buf, "802.11a only"); break;
//		case 3 : strcpy (buf, "802.11a/b/g mixed"); break; 
		case 4 : strcpy (buf, "802.11g only"); break;	}			
	printf("Network Mode       : %s\n", buf);
	
	printf("SSID               : %s\n", CFG_WIFI.ssid);
	if (CFG_WIFI.channel == 0) 	
		printf("Channel            : Auto\n");
	else	
		printf("Channel            : %d\n", CFG_WIFI.channel);
	
	switch (CFG_WIFI.bit_rate)	{
		case 0 : strcpy (buf, "Auto"); break;
		case 1 : strcpy (buf, "1 Mbps"); break;	
		case 2 : strcpy (buf, "2 Mbps"); break;	
		case 3 : strcpy (buf, "5.5 Mbps"); break;	
		case 4 : strcpy (buf, "11 Mbps"); break;	
		case 5 : strcpy (buf, "6 Mbps"); break;	
		case 6 : strcpy (buf, "9 Mbps"); break;	
		case 7 : strcpy (buf, "12 Mbps"); break;	
		case 8 : strcpy (buf, "18 Mbps"); break;	
		case 9 : strcpy (buf, "24 Mbps"); break;	
		case 10: strcpy (buf, "36 Mbps"); break;	
		case 11: strcpy (buf, "48 Mbps"); break;	
		case 12: strcpy (buf, "54 Mbps"); break;		}										
	printf("Bit Rate           : %s\n", buf);
	printf("RTS Threshold      : %d\n", CFG_WIFI.rts_thr);
	printf("Fragment Threshold : %d\n", CFG_WIFI.frag_thr);
	//printf("802.11h Support    : %s\n", (CFG_WIFI.ieee80211h == 0) ? "Disable" : "Enable");
	//printf("Roaming            : %s\n", (CFG_WIFI.roaming_mode == 0) ? "Disable" : "Enable");	
	//printf("Roaming Threshold  : %d\n", CFG_WIFI.roaming_threshold);
	
	switch (CFG_WIFI.auth_mode)	{
		case 0 : strcpy (buf, "Auto"); break;
		case 1 : strcpy (buf, "Open"); break;	
		case 2 : strcpy (buf, "Share"); break;	
		case 3 : strcpy (buf, "WPAPSK"); break;	
		case 4 : strcpy (buf, "WPA2PSK"); break;	}
	printf("Authentication     : %s\n", buf);	
	
	switch (CFG_WIFI.encryp_type)	{
		case 0 : strcpy (buf, "None"); break;
		case 1 : strcpy (buf, "WEP64"); break;	
		case 2 : strcpy (buf, "WEP128"); break;	
		case 3 : strcpy (buf, "TKIP"); break;	
		case 4 : strcpy (buf, "AES"); break;
		case 5 : strcpy (buf, "TKIP/AES"); break;}
	printf("Encryption         : %s\n", buf);	
	printf("NETWORK Key    : %s\n", CFG_WIFI.key);	
	
	printf("Connection Type    : %s\n", (CFG_WIFI.line == 0) ? "DHCP" : "Static IP");
			
	addr.s_addr = SB_GetIp (SB_WLAN_RA_NAME);
	printf("IP Address         : %s  ", inet_ntoa(addr));
	memcpy (q.c, CFG_WIFI.ip, 4);	addr.s_addr   = q.i;
	printf("(Config = %s)\n", inet_ntoa (addr));

	addr.s_addr = SB_GetMask (SB_WLAN_RA_NAME);
	printf("Subnet Mask        : %s  ", inet_ntoa(addr));
	memcpy (q.c, CFG_WIFI.mask, 4);	addr.s_addr   = q.i;
	printf("(Config = %s)\n", inet_ntoa (addr));

   	addr.s_addr = SB_GetGateway ();
    printf("Gateway            : %s  ", inet_ntoa(addr));
	memcpy (q.c, CFG_WIFI.gateway, 4);	addr.s_addr   = q.i;
	printf("(Config = %s)\n", inet_ntoa (addr));

	memcpy (q.c, CFG_WIFI.dns, 4);	addr.s_addr   = q.i;
	printf("DNS                : %s\n", inet_ntoa (addr));
}
//=============================================================
void view_lan()
{
struct in_addr addr;
union { char c[4]; unsigned int i; } q;

	printf("-< LAN >-----------------------------------------------------------\n");
	printf("DHCPD Enable       : %s\n", (CFG_SYS.dhcpenable == 1) ? "Enable" : "Disable");

	memcpy (q.c, CFG_SYS.lanip, 4);	
	addr.s_addr   = q.i;
	printf("IP Address         : %s\n", inet_ntoa(addr));
	
	memcpy (q.c, CFG_SYS.lanmask, 4);	
	addr.s_addr   = q.i;
	printf("Mask Address       : %s\n", inet_ntoa(addr));
	
	memcpy (q.c, CFG_SYS.dhcpstartIP, 4);	
	addr.s_addr   = q.i;
	printf("Lease Start IP     : %s\n", inet_ntoa(addr));

	memcpy (q.c, CFG_SYS.dhcpendIP, 4);	
	addr.s_addr   = q.i;
	printf("Lease End IP       : %s\n", inet_ntoa(addr));		
	printf("Lease Time         : %d min\n", CFG_SYS.lease_time);
}
//=============================================================
void view_management()
{
struct in_addr addr;
union { char c[4]; unsigned int i; } q;
					  
	CFG_SYS.device_name[31] = 0x00;
	printf("-< Management >----------------------------------------------------\n");	
//	printf("Device Type        : %s\n", buff);	
	printf("Device Name        : %s\n", CFG_SYS.device_name);
	printf("Username           : %s\n", CFG_SYS.username);
	printf("Password           : %s\n", CFG_SYS.password);

	memcpy (q.c, CFG_SYS.ddns, 4); addr.s_addr   = q.i;
	printf("DDNS               : %s (%s/%s)\n", inet_ntoa (addr), CFG_SYS.ddnsuser, CFG_SYS.ddnspass);
	memcpy (q.c, CFG_SYS.portview, 4); addr.s_addr   = q.i;
	printf("PortView           : %s (%d)\n", inet_ntoa (addr), CFG_SYS.portview_port_no);
	printf("WEB Server         : %s\n", (CFG_SYS.web_server == 0) ? "Disable" : "Enable");
	printf("TELNET Server      : %s\n", (CFG_SYS.telnet_server == 0) ? "Disable" : "Enable");
	printf("FTP Server         : %s\n", (CFG_SYS.ftp_server == 0) ? "Disable" : "Enable");
	printf("SSH Server         : %s\n", (CFG_SYS.ssh_server == 0) ? "Disable" : "Enable");				
	printf("Target_Agent       : %s\n", (CFG_SYS.target_agent == 0) ? "Disable" : "Enable");				
}
//=============================================================
void view_snmp()
{
char ver[30];
union { char c[4]; unsigned int i; } q;
struct in_addr addr;
	

	SB_ReadConfig  (CFGFILE_ETC_SNMP,  (char *)&CFG_SNMP, sizeof(struct SB_SNMP_CONFIG));
	printf("-< SNMP >----------------------------------------------------------\n");	
	printf("SNMP Agent         : %s\n", (CFG_SNMP.enable == 0) ? "Disable" : "Enable");
	switch (CFG_SNMP.v1_readwrite)
		{
		case 0 :  sprintf (ver, "Readonly"); break;	
		case 1 :  sprintf (ver, "Read/Write"); break;
		case 2 :  sprintf (ver, "Disable"); break;
		}		
	printf("SNMP V1 read/write : %s\n", ver);
	printf("SNMP V3 read/write : %s\n", (CFG_SNMP.v3_readwrite == 0) ? "Readonly" : "Read/Write");	
	printf("SNMP V3 user/pass  : %s / %s\n", CFG_SNMP.v3_username, CFG_SNMP.v3_password);	
	memcpy (q.c, CFG_SNMP.trap_ip, 4);	addr.s_addr   = q.i;
	printf("SNMP Trap IP/Port  : %s / %d\n", inet_ntoa (addr), CFG_SNMP.trap_port);
	printf("reset trap         : %s\n", ((CFG_SNMP.trap_option & 0x01) == 1) ? "Enable" : "Disable");				
	printf("connected trap     : %s\n", ((CFG_SNMP.trap_option & 0x02) == 2) ? "Enable" : "Disable");				
	printf("disconnect trap    : %s\n", ((CFG_SNMP.trap_option & 0x04) == 4) ? "Enable" : "Disable");				
}
//=============================================================
void view_port()
{
int speed [14]={150, 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600 };
int no, stop;
char Parity[40], protocol[20], flow[10], temp[100];

	
	SB_ReadConfig  (CFGFILE_ETC_SIO,  (char *)&CFG_SIO[0], sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);
	printf("-< Serial>---------------------------------------------------------\n");		
	for (no=0; no<SB_MAX_SIO_PORT; no++)
		{
		switch (CFG_SIO[no].protocol)
			{
			case SB_DISABLE_MODE 	  : strcpy (protocol, "Disable"); break;
			case SB_COM_REDIRECT_MODE  : strcpy (protocol, "Com_Redirect"); break;
			case SB_TCP_SERVER_MODE   : strcpy (protocol, "TCP_Server"); break;
			case SB_TCP_CLIENT_MODE   : strcpy (protocol, "TCP_Client"); break;
			case SB_TCP_BROADCAST_MODE: strcpy (protocol, "TCP_Broadcast"); break;
			case SB_TCP_MULTIPLEX_MODE: strcpy (protocol, "TCP_Multiplex"); break;
			case SB_UDP_SERVER_MODE   : strcpy (protocol, "UDP_Server"); break;
			case SB_UDP_CLIENT_MODE   : strcpy (protocol, "UDP_Client"); break;							
			}
		switch ((CFG_SIO[no].dps & 0x18))
			{
			case  0x00 : strcpy (Parity, "None"); break;
			case  0x08 : strcpy (Parity, "Odd "); break;
			case  0x10 : 
			case  0x18 : strcpy (Parity, "Even"); break;	
			}		
		switch (CFG_SIO[no].flow)
			{
			case  0 : strcpy (flow, "None"); break;
			case  1 : strcpy (flow, "XON "); break;
			case  2 : strcpy (flow, "RTS "); break;
			}

		if (CFG_SIO[no].dps&0x04) stop = 2; else stop = 1;
		printf("[%2d] Name     : %-16s       Protocol : %s\n",  no+1,  CFG_SIO[no].name, protocol);
		printf("     Speed    : %-6d/%-4s/%d/%d        Flow     : %s\n", speed[(int)CFG_SIO[no].speed], Parity, (CFG_SIO[no].dps&0x03)+5, stop, flow );
		printf("     Soket No.: %-4d                   Latency  : %-5d msec\n", CFG_SIO[no].socket_no, CFG_SIO[no].packet_latency_time);
		printf("     Keepalive: %-5d sec              Device   : %s\n", CFG_SIO[no].keepalive, (CFG_SIO[no].device == 0) ? "Data" : "Modem");        
		sprintf(temp, "     Remote   : %d.%d.%d.%d (%d)",	CFG_SIO[no].remote_ip[0], CFG_SIO[no].remote_ip[1],CFG_SIO[no].remote_ip[2],CFG_SIO[no].remote_ip[3], CFG_SIO[no].remote_socket_no);
		switch (CFG_SIO[no].interface)
			{
			case  SB_RS232     		: strcpy (Parity, "RS-232"); break;
			case  SB_RS422_PTOP		: strcpy (Parity, "RS-422"); break;
			case  SB_RS485_NONE_ECHO: strcpy (Parity, "RS-485-N0-ECHO"); break;
			case  SB_RS485_ECHO		: strcpy (Parity, "RS485-ECHO"); break;	
			}
		printf("%-39sInterface: %s\n", temp, Parity);
		printf("     Login    : %-7s                Username : %s/%s\n", (CFG_SIO[no].login == 0) ? "Disable" : "Enable",  CFG_SIO[no].login_name, CFG_SIO[no].login_password);        
		}
}
//=============================================================
void view_gpio()
{
int no;
int cnt;
char u[20], v[30];
unsigned int hexa = 0x00000001, hex;


	SB_ReadConfig  (CFGFILE_ETC_GPIO,  (char *)&CFG_GPIO, sizeof(struct SB_GPIO_CONFIG));

	printf("-< GPIO >-----------------------------------------------------------\n");
	printf("No. Status  Value               No. Status  Value     \n");
	printf("--------------------------------------------------------------------\n");	


	if (Product_ID != EDDY_S4M)
		{
		if (CFG_GPIO.enable   [GPORT_A] & HEX_PA04) strcpy (u, "Enable "); else strcpy (u, "Disable");
		if (CFG_GPIO.mode  [GPORT_A] & HEX_PA04) 
			if (CFG_GPIO.value [GPORT_A] & HEX_PA04) strcpy (v, "Output(High)    ");  else strcpy (v, "Output(Low)     ");
		else		
			if (CFG_GPIO.pullup[GPORT_A] & HEX_PA04) strcpy (v, "Input (PullUp)  ");  else strcpy (v, "Input (PullDown)");
		printf ("A04 %s %s    ",  u, v);
	
		if (CFG_GPIO.enable   [GPORT_A] & HEX_PA05) strcpy (u, "Enable "); else strcpy (u, "Disable");
		if (CFG_GPIO.mode  [GPORT_A] & HEX_PA05) 
			if (CFG_GPIO.value [GPORT_A] & HEX_PA05) strcpy (v, "Output(High)    ");  else strcpy (v, "Output(Low)     ");
		else		
			if (CFG_GPIO.pullup[GPORT_A] & HEX_PA05) strcpy (v, "Input (PullUp)  ");  else strcpy (v, "Input (PullDown)");
		printf ("A05 %s %s\n",  u, v);
	
		for (no=0; no<32; no++)
			{
			hex = hexa << no;
			if (CFG_GPIO.enable   [GPORT_B] & hex) strcpy (u, "Enable "); else strcpy (u, "Disable");
			if (CFG_GPIO.mode  [GPORT_B] & hex) 
				if (CFG_GPIO.value [GPORT_B] & hex) strcpy (v, "Output(High)    ");  else strcpy (v, "Output(Low)     ");
			else		
				if (CFG_GPIO.pullup[GPORT_B] & hex) strcpy (v, "Input (PullUp)  ");  else strcpy (v, "Input (PullDown)");
			if (no % 2 == 1)
				printf ("B%02d %s %s\n",  no, u, v);
			else
				printf ("B%02d %s %s    ", no, u, v);
			}
	
		cnt = 0;
		for (no=0; no<27; no++)
			{
			switch (no)	{
				case 6 :
				case 7 :
				case 11 :
				case 24 :
				case 25 :  continue;
				}
			cnt ++;
			hex = hexa << no;
			if (CFG_GPIO.enable   [GPORT_C] & hex) strcpy (u, "Enable "); else strcpy (u, "Disable");
			if (CFG_GPIO.mode  [GPORT_C] & hex) 
				if (CFG_GPIO.value [GPORT_C] & hex) strcpy (v, "Output(High)    ");  else strcpy (v, "Output(Low)     ");
			else		
				if (CFG_GPIO.pullup[GPORT_C] & hex) strcpy (v, "Input (PullUp)  ");  else strcpy (v, "Input (PullDown)");
			if (cnt % 2 == 0)
				printf ("C%02d %s %s\n",  no, u, v);
			else
				printf ("C%02d %s %s    ", no, u, v);
			}
		}
	else		// S4M
		{
		if (CFG_GPIO.enable   [GPORT_A] & HEX_PA05) strcpy (u, "Enable "); else strcpy (u, "Disable");
		if (CFG_GPIO.mode  [GPORT_A] & HEX_PA05) 
			if (CFG_GPIO.value [GPORT_A] & HEX_PA05) strcpy (v, "Output(High)    ");  else strcpy (v, "Output(Low)     ");
		else		
			if (CFG_GPIO.pullup[GPORT_A] & HEX_PA05) strcpy (v, "Input (PullUp)  ");  else strcpy (v, "Input (PullDown)");
		printf ("A05 %s %s    ",  u, v);
	
		if (CFG_GPIO.enable   [GPORT_A] & HEX_PA22) strcpy (u, "Enable "); else strcpy (u, "Disable");
		if (CFG_GPIO.mode  [GPORT_A] & HEX_PA22) 
			if (CFG_GPIO.value [GPORT_A] & HEX_PA22) strcpy (v, "Output(High)    ");  else strcpy (v, "Output(Low)     ");
		else		
			if (CFG_GPIO.pullup[GPORT_A] & HEX_PA22) strcpy (v, "Input (PullUp)  ");  else strcpy (v, "Input (PullDown)");
		printf ("A22 %s %s\n",  u, v);
		
		if (CFG_GPIO.enable   [GPORT_A] & HEX_PA30) strcpy (u, "Enable "); else strcpy (u, "Disable");
		if (CFG_GPIO.mode  [GPORT_A] & HEX_PA22) 
			if (CFG_GPIO.value [GPORT_A] & HEX_PA30) strcpy (v, "Output(High)    ");  else strcpy (v, "Output(Low)     ");
		else		
			if (CFG_GPIO.pullup[GPORT_A] & HEX_PA30) strcpy (v, "Input (PullUp)  ");  else strcpy (v, "Input (PullDown)");
		printf ("A30 %s %s\n",  u, v);		
	
		for (cnt=0, no=0; no<=31; no++)
			{
			if (!(no==0 || no==1 || no==2 || no==3 || no==12 || no==13 || no==16 || no==17 || no==18 || no==19 || no==20 || no==21 || no==30 || no==31)) continue;
			cnt ++;
			hex = hexa << no;
			if (CFG_GPIO.enable   [GPORT_B] & hex) strcpy (u, "Enable "); else strcpy (u, "Disable");
			if (CFG_GPIO.mode  [GPORT_B] & hex) 
				if (CFG_GPIO.value [GPORT_B] & hex) strcpy (v, "Output(High)    ");  else strcpy (v, "Output(Low)     ");
			else		
				if (CFG_GPIO.pullup[GPORT_B] & hex) strcpy (v, "Input (PullUp)  ");  else strcpy (v, "Input (PullDown)");
			if (cnt % 2 == 0)
				printf ("B%02d %s %s\n",  no, u, v);
			else
				printf ("B%02d %s %s    ", no, u, v);
			}

		for (cnt=0, no=0; no<=25; no++)
			{
			if (no==4 || no==6 || no==7 || no==8 || no==11 || no==16 || no==21 || no==22 || no==23) continue;
			cnt ++;
			hex = hexa << no;
			if (CFG_GPIO.enable   [GPORT_C] & hex) strcpy (u, "Enable "); else strcpy (u, "Disable");
			if (CFG_GPIO.mode  [GPORT_C] & hex) 
				if (CFG_GPIO.value [GPORT_C] & hex) strcpy (v, "Output(High)    ");  else strcpy (v, "Output(Low)     ");
			else		
				if (CFG_GPIO.pullup[GPORT_C] & hex) strcpy (v, "Input (PullUp)  ");  else strcpy (v, "Input (PullDown)");
			if (cnt % 2 == 0)
				printf ("C%02d %s %s\n",  no, u, v);
			else
				printf ("C%02d %s %s    ", no, u, v);
			}			
		printf ("\n");	
		}
}

//=============================================================
void view_dio()
{
int no;
char v[30];
unsigned short hexa = 0x0001, hex;

	SB_ReadConfig  (CFGFILE_ETC_DIO,  (char *)&CFG_DIO, sizeof(struct SB_DIO_CONFIG));

	printf("-< DIO >------------------------------------------------------------\n");
	printf("No Value          No Value          No Value          No Value\n");
	printf("--------------------------------------------------------------------\n");	

	for (no=0; no<16; no++)
		{
		hex = hexa << no;
		if (CFG_DIO.mode & hex) 
            strcpy (v, "Input          ");
        else
			if (CFG_DIO.value  & hex) strcpy (v, "Output(High)   ");  else strcpy (v, "Output(Low)    ");

		if (no % 4 == 3)
			printf ("%02d %s\n",  no, v);
		else
			printf ("%02d %s", no, v);
		}
}


