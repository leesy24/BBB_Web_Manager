/*****************************************************************************
Title		: Read from Flash Configuration file sample code

Description	: Configuration files that set in the WEB, Telnet is read / written.
              The details about configuration file is referred in include/sb_config.h.
Argments  1	: None
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
struct SB_SYSTEM_CONFIG			CFG_SYS;						// /flash/eddy_system.cfg
struct SB_SIO_CONFIG			CFG_SIO [SB_MAX_SIO_PORT];		// /flash/eddy_sio.cfg
struct SB_SNMP_CONFIG			CFG_SNMP;						// /flash/eddy_snmp.cfg
struct SB_GPIO_CONFIG			CFG_GPIO;						// /flash/eddy_gpio.cfg
struct SB_DIO_CONFIG			CFG_DIO;						// /flash/eddy_dio.cfg
struct SB_WIFI_CONFIG			CFG_WIFI;						// /flash/eddy_wifi.cfg


#if 0
	//------------------------- Read /etc Config Data
	SB_ReadConfig  (CFGFILE_ETC_SYSTEM,  (char *)&CFG_SYS, 		sizeof (struct SB_SYSTEM_CONFIG));
	SB_ReadConfig  (CFGFILE_ETC_SIO,     (char *)&CFG_SIO[0],	sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);	
	SB_ReadConfig  (CFGFILE_ETC_SNMP,    (char *)&CFG_SNMP,		sizeof(struct SB_SNMP_CONFIG));	
	SB_ReadConfig  (CFGFILE_ETC_GPIO,    (char *)&CFG_GPIO,		sizeof(struct SB_GPIO_CONFIG));	
	SB_ReadConfig  (CFGFILE_ETC_DIO,     (char *)&CFG_DIO,		sizeof(struct SB_DIO_CONFIG));	
	SB_ReadConfig  (CFGFILE_ETC_WIFI,    (char *)&CFG_WIFI,		sizeof(struct SB_WIFI_CONFIG));
#endif

	//------------------------- Read Flash Memory Config Data
	SB_ReadConfig  (CFGFILE_FLASH_SYSTEM,  (char *)&CFG_SYS, 	sizeof (struct SB_SYSTEM_CONFIG));
	SB_ReadConfig  (CFGFILE_FLASH_SIO,     (char *)&CFG_SIO[0],	sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);	
	SB_ReadConfig  (CFGFILE_FLASH_SNMP,    (char *)&CFG_SNMP,	sizeof(struct SB_SNMP_CONFIG));	
	SB_ReadConfig  (CFGFILE_FLASH_GPIO,    (char *)&CFG_GPIO,	sizeof(struct SB_GPIO_CONFIG));	
	SB_ReadConfig  (CFGFILE_FLASH_DIO,     (char *)&CFG_DIO,	sizeof(struct SB_DIO_CONFIG));	
	SB_ReadConfig  (CFGFILE_FLASH_WIFI,    (char *)&CFG_WIFI,	sizeof(struct SB_WIFI_CONFIG));



	printf ("(%s)\n", CFGFILE_FLASH_SYSTEM);
	printf ("mac        :   %02x:%02x:%02x:%02x:%02x:%02x\n", CFG_SYS.mac[0],CFG_SYS.mac[1],CFG_SYS.mac[2],CFG_SYS.mac[3],CFG_SYS.mac[4],CFG_SYS.mac[5]);
	printf ("ip         :   %d.%d.%d.%d\n", CFG_SYS.ip[0], CFG_SYS.ip[1], CFG_SYS.ip[2], CFG_SYS.ip[3]);
	printf ("gateway    :   %d.%d.%d.%d\n", CFG_SYS.gateway[0], CFG_SYS.gateway[1], CFG_SYS.gateway[2], CFG_SYS.gateway[3]);
	printf ("mask       :   %d.%d.%d.%d\n", CFG_SYS.mask[0], CFG_SYS.mask[1], CFG_SYS.mask[2], CFG_SYS.mask[3]);
	printf ("primary dns:   %d.%d.%d.%d\n", CFG_SYS.dns[0], CFG_SYS.dns[1], CFG_SYS.dns[2], CFG_SYS.dns[3]);
	printf ("second dns :   %d.%d.%d.%d\n", CFG_SYS.dns_s[0], CFG_SYS.dns_s[1], CFG_SYS.dns_s[2], CFG_SYS.dns_s[3]);
	printf ("line       :   %s\n.\n.\n", (CFG_SYS.line == 'I') ? "Static IP" : "DHCP");
	
	printf ("(%s)\n", CFGFILE_FLASH_SIO);
	printf ("name       :   (1)%8s,  (2)%8s\n", CFG_SIO[0].name, CFG_SIO[1].name);
	printf ("socket_no  :   (1)%8d,  (2)%8d\n", CFG_SIO[0].socket_no, CFG_SIO[1].socket_no);
	printf ("speed      :   (1)%8d,  (2)%8d\n.\n.\n", CFG_SIO[0].speed, CFG_SIO[1].speed);

	printf ("(%s)\n", CFGFILE_FLASH_WIFI);
	printf ("Wireless   :   %s\n", (CFG_WIFI.wireless == 0) ? "Disable" : "Enable");
	printf ("ssid       :   %s\n", CFG_WIFI.ssid);
	printf ("wifi_mode  :   %s\n.\n.\n", (CFG_WIFI.wifi_mode == 0) ? "Infrastructure" : "AD-Hoc");
		

#if 0
	//-------------------------- Write to Flash Memory
	SB_WriteConfig (CFGFILE_FLASH_SYSTEM,  (char *)&CFG_SYS, 		sizeof (struct SB_SYSTEM_CONFIG));
	SB_WriteConfig (CFGFILE_FLASH_SIO,     (char *)&CFG_SIO[0],	sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);	
	SB_WriteConfig (CFGFILE_FLASH_SNMP,    (char *)&CFG_SNMP,		sizeof(struct SB_SNMP_CONFIG));	
	SB_WriteConfig (CFGFILE_FLASH_GPIO,    (char *)&CFG_GPIO,		sizeof(struct SB_GPIO_CONFIG));	
	SB_WriteConfig (CFGFILE_FLASH_DIO,     (char *)&CFG_DIO,		sizeof(struct SB_DIO_CONFIG));	
	SB_WriteConfig (CFGFILE_FLASH_WIFI,    (char *)&CFG_WIFI,		sizeof(struct SB_WIFI_CONFIG));
#endif
	return 0;
}
