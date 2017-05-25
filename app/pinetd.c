/*****************************************************************************
  Copyright (c) 2007 SystemBase Co., LTD  All Rights Reserved.
 ******************************************************************************/
#include "sb_include.h"
#include "sb_define.h"
#include "sb_shared.h"
#include "sb_config.h"
#include "sb_extern.h"
#include "sb_ioctl.h"
#include "sb_misc.h"
#include <time.h>
#include <linux/rtc.h>
#include <sys/mount.h>

#ifndef isblank
#define isblank(ch) (((ch) == ' ') || ((ch) == '\t'))
#endif

struct SB_SYSTEM_CONFIG			CFG_SYS;
struct SB_SIO_CONFIG			CFG_SIO [SB_MAX_SIO_PORT];
struct SB_SNMP_CONFIG			CFG_SNMP;
struct SB_GPIO_CONFIG			CFG_GPIO;
struct SB_DIO_CONFIG			CFG_DIO;
struct SB_WIFI_CONFIG			CFG_WIFI;

struct SB_SNMP_STRUCT	  *SSM;

struct eddy_gpio_struct
{
    unsigned int value[3];
    unsigned int mode[3];
    unsigned int pullup[3];
    unsigned int enable[3];
};

int PORT_PID [SB_MAX_SIO_PORT];
int DETECT_PID=0;
int PORTVIEW_PID=0;
int	SNMPD_PID=0;
int Number_Of_Ports;

int SNMP_INIT ();
pid_t Task_Launch  (char *tname, int no);
int check_proc (char *task_name);
void Fork_Processor (int no);
void sig_chld (int signo);
int get_network_address(unsigned char type);
void freechunks(void);
void Check_reset_switch (int fd);
void Make_system_time ();
void reset_or_factory (int flag);
void make_dhcpd_conf ();
#ifdef LCD_SUPPORT
void lcd ();
#endif

//=============================================================================
int main(int argc, char *argv[])
{
struct eddy_gpio_struct  gpio;
int port_no, no, fd, pflag=0, gflag=0,rdy_flag=0, gpio_fd, ProductID = EDDY_CPU;
short RDY_cnt=15, SNMP_cnt=0;
#ifdef LCD_SUPPORT
short LCD_cnt=0;
#endif
short MMC_cnt=0, PROC_cnt=0;
#if 0 // TODO:
char  MMC_Flag=1;
#else
char  MMC_Flag=0;
#endif
#ifdef LCD_SUPPORT
char LCD_Flag=1;
#endif
char cmd[30];
unsigned int timer;

	SB_msleep (1);
	//------------------------- Read Flash Memory Config Data
	SB_ReadConfig  (CFGFILE_FLASH_SYSTEM,  (char *)&CFG_SYS, 		sizeof (struct SB_SYSTEM_CONFIG));
	SB_ReadConfig  (CFGFILE_FLASH_SIO,     (char *)&CFG_SIO[0],	sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);	
	SB_ReadConfig  (CFGFILE_FLASH_SNMP,    (char *)&CFG_SNMP,		sizeof(struct SB_SNMP_CONFIG));	
	SB_ReadConfig  (CFGFILE_FLASH_GPIO,    (char *)&CFG_GPIO,		sizeof(struct SB_GPIO_CONFIG));	
	SB_ReadConfig  (CFGFILE_FLASH_DIO,     (char *)&CFG_DIO,		sizeof(struct SB_DIO_CONFIG));	
	SB_ReadConfig  (CFGFILE_FLASH_WIFI,    (char *)&CFG_WIFI,		sizeof(struct SB_WIFI_CONFIG));
	//-------------------------- Write to /etc
	SB_WriteConfig (CFGFILE_ETC_SYSTEM,  (char *)&CFG_SYS, 		sizeof (struct SB_SYSTEM_CONFIG));
	SB_WriteConfig (CFGFILE_ETC_SIO,     (char *)&CFG_SIO[0],	sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);	
	SB_WriteConfig (CFGFILE_ETC_SNMP,    (char *)&CFG_SNMP,		sizeof(struct SB_SNMP_CONFIG));	
	SB_WriteConfig (CFGFILE_ETC_GPIO,    (char *)&CFG_GPIO,		sizeof(struct SB_GPIO_CONFIG));	
	SB_WriteConfig (CFGFILE_ETC_DIO,     (char *)&CFG_DIO,		sizeof(struct SB_DIO_CONFIG));	
	SB_WriteConfig (CFGFILE_ETC_WIFI,    (char *)&CFG_WIFI,		sizeof(struct SB_WIFI_CONFIG));


	//--------------------------------------------------------------< SNMP launch
	if (CFG_SNMP.enable == SB_ENABLE) SNMP_INIT();

#if 0	// TODO:
	//--------------------------------------------------------------< Detect Applition launch
	DETECT_PID = Task_Launch ("/sbin/detect", 0);		
#endif

	//--------------------------------------------------------------< DDNS Applition launch
	if (CFG_SYS.ddns[0] != 0x00) Task_Launch ("/sbin/ddns_agent", 0);
		
	//--------------------------------------------------------------< PortView Applition launch
	if (CFG_SYS.portview[0] != 0x00) PORTVIEW_PID = Task_Launch ("/sbin/portview", 0);

	//--------------------------------------------------------------< Serial Application launch
	for (no=0; no<SB_MAX_SIO_PORT; no++)
		{	
		Fork_Processor (no);
		}

	//---------------------------------------------------------------------- LAN ---------
	if (CFG_GPIO.lan == SB_ENABLE)	
		{
		system ("insmod /etc/drivers/dm9000.ko");
		sprintf(cmd, "ifconfig %s hw ether %02x:%02x:%02x:%02x:%02x:%02x",
				SB_LAN_ETH_NAME,
				CFG_SYS.mac[0],
				CFG_SYS.mac[1],
				CFG_SYS.mac[2],
				CFG_SYS.mac[3] | 0x88,
				CFG_SYS.mac[4],
				CFG_SYS.mac[5]);
		system(cmd);

		sprintf (cmd, "ifconfig %s %d.%d.%d.%d netmask %d.%d.%d.%d up", 
			SB_LAN_ETH_NAME,
			CFG_SYS.lanip[0], 
			CFG_SYS.lanip[1], 
			CFG_SYS.lanip[2], 
			CFG_SYS.lanip[3], 
			CFG_SYS.lanmask[0], 
			CFG_SYS.lanmask[1], 
			CFG_SYS.lanmask[2], 
			CFG_SYS.lanmask[3]);
		system (cmd);	

		sprintf (cmd, "/usr/local/sbin/iptables -t nat -A POSTROUTING -s %d.%d.%d.0/%d.%d.%d.%d -j MASQUERADE", 
			CFG_SYS.lanip[0], 
			CFG_SYS.lanip[1], 
			CFG_SYS.lanip[2], 
			CFG_SYS.lanmask[0], 
			CFG_SYS.lanmask[1], 
			CFG_SYS.lanmask[2], 
			CFG_SYS.lanmask[3]);
		system (cmd);	
		system ("echo  1 > /proc/sys/net/ipv4/ip_forward");
		
		if (CFG_SYS.dhcpenable == SB_ENABLE)
			{
			make_dhcpd_conf ();
			system ("/sbin/udhcpd");
			}
		}

	signal(SIGCHLD, sig_chld);		// When SIGCHLD happen, go sig_chld

#if 0 // TODO:
	//--------------------------------------------------------------< Init RTC
	system ("insmod  /etc/drivers/rtc-ds1307.ko");		// /dev/rtc0
#endif

	//--------------------------------------------------------------< Init NAND Flash
	if (CFG_GPIO.nandflash == SB_ENABLE)	
		{
		system ("insmod  /etc/drivers/at91_nand.ko");		// /dev/mtdblock6,   mount -t jffs2 /dev/mtdblock6  /tmp/nand
		mkdir ("/tmp/nand", 0x777);
		sleep (1);
		mount ("/dev/mtdblock6", "/tmp/nand", "yaffs2", 16, "");
//		system ("mount -t yaffs2 /dev/mtdblock6  /tmp/nand");
		}

	//--------------------------------------------------------------< Init ADC
	if (CFG_GPIO.adc == SB_ENABLE)							// If ADC disable --> GPIO(PC00, PC01) enable	
		{													// IF ADC Enable  --> GPIO(PC00, PC01) disable
		system ("insmod  /etc/drivers/eddy_adc.ko");		// /dev/adc		
		}

	//--------------------------------------------------------------< Init SPI(EEPROM)
	if (CFG_GPIO.spi_eeprom == SB_ENABLE)	
		{
		system ("insmod  /etc/drivers/at25.ko");		// /sys/class/spi_master/spi1/device/spi1.0/eeprom
		}

#if 0 // TODO:
	//--------------------------------------------------------------< Init GPIO
	system ("insmod  /etc/drivers/eddy_gpio.ko");		
	SB_msleep (100);

	gpio_fd = open (SB_GPIO_DEVICE, O_RDWR);					// dev/gpio                  
	if (gpio_fd > 0)	                  
		{                  
		ioctl (gpio_fd, INIT_PRODUCT);	                  
		ProductID = ioctl (gpio_fd, GET_PRODUCTID);	                  
		printf ("Product ID = %s\n", (ProductID == EDDY_S4M) ? "Eddy_S4M" : "Eddy-CPU");	              
		}

	for (no=0; no<3; no++)
		{
		gpio.enable[no]	= CFG_GPIO.enable[no];
		gpio.mode[no]	= CFG_GPIO.mode[no];
		gpio.value[no]	= CFG_GPIO.value[no];
		gpio.pullup[no]	= CFG_GPIO.pullup[no];
		}

	if (ProductID == EDDY_S4M)
		{
		gpio.enable[0] &= S4M_GPIO_MASK_PORT_A;
		gpio.enable[1] &= S4M_GPIO_MASK_PORT_B;
		gpio.enable[2] &= S4M_GPIO_MASK_PORT_C;
		CFG_DIO.lcd = SB_DISABLE;				// Disable LCD
		}
	else	
		{
		gpio.enable[0] &= GPIO_MASK_PORT_A;
		gpio.enable[1] &= GPIO_MASK_PORT_B;
		gpio.enable[2] &= GPIO_MASK_PORT_C;
		if (CFG_GPIO.keypad == SB_ENABLE)
			{
			gpio.mode[1] &= 0x3fcfffff;				// Mask Keypad input  Port B.
			gpio.mode[2] |= 0x00f00000;				// Mask Keypad output Port C.
			}	
		}

	if (gpio_fd > 0)	                  
		{                  
		ioctl (gpio_fd, SETGPIOINIT, &gpio);
		close (gpio_fd);
		}
#endif
		
	//<===================================================================================
	//  Here User Application Launching !!	
	// -----------------------------------------------------------------------------------
	//
	//  ex) Task_Launch ("/sbin/hello", argument); 
	//                        |          |
	//                        |          +---- Integer argument
	//                        +--------------- Application name with path
	//
	//====================================================================================>

	//--------------------------------------- WEB Server       
    if (CFG_SYS.web_server == SB_ENABLE)                                                                     
        system ("/usr/local/sbin/thttpd -C  /etc/thttpd.conf");

#if 0 // TODO:
	if (ProductID != EDDY_S4M) 
		{
		system ("insmod /etc/drivers/pca9539.ko");			// DIO and LCD 
		if (CFG_DIO.lcd == SB_ENABLE)
			{
			system ("insmod /etc/drivers/cfbcopyarea.ko");
			system ("insmod /etc/drivers/cfbfillrect.ko");
			system ("insmod /etc/drivers/cfbimgblt.ko");
			system ("insmod /etc/drivers/eddy_lcd.ko");
			system ("insmod /etc/drivers/eddy_lcdfb.ko");
			}
		}
#endif

	Make_system_time ();
	
	//--------------------------------------------------------------< Init SPI(EEPROM)
	if (CFG_WIFI.wireless == SB_ENABLE) 
		{
		Task_Launch ("/sbin/wifi", 0); 	
		}	

	fd = open (SB_GPIO_DEVICE, O_RDWR);
	//-------------------------------------------------------------< Ready LED and Reset button 
	while (1) 
		{
		SB_msleep(30);

#ifdef LCD_SUPPORT
		if (LCD_Flag)
			{
			if (++LCD_cnt > 70) 								// After 1 sec
				{
				LCD_Flag = 0;
				if (CFG_DIO.lcd == SB_ENABLE) lcd ();
				}	
			}
#endif

		if (MMC_Flag)
			{
			if (++MMC_cnt > 130) 								// After 2 sec
				{
				MMC_Flag = 0;
				if (access("/dev/sda1", F_OK) == 0)
					{	
					mkdir ("/tmp/mmc", 0x777);
//					system ("mount -t vfat /dev/sda1  /tmp/mmc ");
					mount ("/dev/sda1", "/tmp/mmc", "vfat", 16, "");
					chmod ("/tmp/mmc", S_IWUSR|S_IRUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
					}
				}
			}

		if (++PROC_cnt > 300)  // 5 분마다 검사
			{
			PROC_cnt = 0;
			if (CFG_SYS.web_server == SB_ENABLE)
				{
				no = check_proc ("/usr/local/sbin/thttp");
				if (no == 0) 
					{
					system ("/usr/local/sbin/thttpd -C /etc/thttpd.conf");
					}
				}
			}

		
		if (CFG_GPIO.rdy_led == SB_ENABLE)
			{
			if (++RDY_cnt > 15)			// 450 msec
				{
				if (rdy_flag == 0)
					{
					ioctl (fd, RDY_LED_OFF);
					rdy_flag = 1;
					}
				else
					{
					ioctl (fd, RDY_LED_ON);	
					rdy_flag = 0;
					}				
				RDY_cnt = 0;
				}
			}

		if (CFG_GPIO.reset == SB_ENABLE)
			{
			if (ioctl (fd, RESET_READ) != SB_ENABLE)
				{
				timer = SB_GetTick () + 3000;		// 3 sec
				while (1)
					{
					SB_msleep (20);
					if (timer < SB_GetTick ()) reset_or_factory (1);
					if (ioctl (fd, RESET_READ) == SB_ENABLE) reset_or_factory (0);
					}
				}
			}


		if (SNMPD_PID  == 0) continue;
		if (++SNMP_cnt > 15) continue;
		SNMP_cnt = 0;	
		if (SSM_REBOOT) 		// snmp set (reboot)
			{
			system ("/sbin/reboot");	
			SB_msleep (1000);
			}
#if 0
		for (port_no=0; port_no<gpio; port_no++)
			{
			if (SSM_GPIO_MODE != CFG_GPIO.mode[port_no])
				{
				CFG_GPIO.mode [port_no] = SSM_GPIO_MODE;
				SB_SetGpioMode (port_no, CFG_GPIO.mode [port_no]);
				gflag = 1;
				}
			if (SSM_GPIO_VALUE != CFG_GPIO.value [port_no])
				{
				CFG_GPIO.value [port_no] = SSM_GPIO_VALUE;
				if (CFG_GPIO.mode [port_no]) SB_SetGpioValue (port_no, CFG_GPIO.value [port_no]);
				gflag = 1;
				}
			}
#endif
		for (port_no=0; port_no<SB_MAX_SIO_PORT; port_no++)
			{
			if (SSM_DPS != CFG_SIO[port_no].dps)		// parity,databits,stopbits
				{
				CFG_SIO[port_no].dps = SSM_DPS;
				pflag = 1;
				}
			if (SSM_SPEED != CFG_SIO[port_no].speed)
				{
				CFG_SIO[port_no].speed = SSM_SPEED;
				pflag = 1;
				}
			if (SSM_FLOW != CFG_SIO[port_no].flow)
				{
				CFG_SIO[port_no].flow = SSM_FLOW;
				pflag = 1;
				}
			}
		if (gflag) 
			{
			SB_WriteConfig  (CFGFILE_ETC_GPIO,     (char *)&CFG_GPIO, sizeof(struct SB_GPIO_CONFIG));	
			gflag = 0;
			}

		if (pflag) 
			{
			SB_WriteConfig  (CFGFILE_ETC_SIO,     (char *)&CFG_SIO[0], sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);	
			pflag = 0;
			}
		}
}
//=============================================================
void reset_or_factory (int flag)
{
	
	if (flag)
		{
		printf ("Factory Default\n");
		system ("/sbin/def factory");
		SB_msleep (1000);
		}

	printf ("Warm Rebooting...\n");
	system ("/sbin/reboot");	
	SB_msleep (1000);
	system ("/sbin/reboot");	
	SB_msleep (10000);
	return;
}
//=============================================================
void Make_system_time ()
{
int fd;
char buf[30];
time_t tt;
time_t now;
struct tm t;

	fd = open("/dev/rtc0", O_RDONLY);
	if (fd > 0)
		{
		ioctl(fd, RTC_RD_TIME, &t);
		printf("RTC = %4d-%02d-%02d %02d:%02d:%02d\n", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);	
		now = mktime(&t);
		stime (&now);	    	// Set Time
		close (fd);
		}

	usleep (1);
    time(&tt);
	fd = open("/var/run/timesec", O_WRONLY | O_CREAT);
	if (fd > 0)
		{
		lseek (fd, 0, 0);
		sprintf (buf, "%d", (int)tt);
		write(fd, buf, 20);
		close(fd);
		chmod ("/var/run/timesec", S_IWUSR|S_IRUSR);
		}		
}
/*======================================================================*/
void Fork_Processor (int no)
{

	switch (no)
		{
		case 0 : if (CFG_GPIO.serial_1 == SB_ENABLE)	break;	else	return;
		case 1 : if (CFG_GPIO.serial_2 == SB_ENABLE)	break;	else	return;
		case 2 : if (CFG_GPIO.serial_3 == SB_ENABLE)	break;	else	return;
		case 3 : if (CFG_GPIO.serial_4 == SB_ENABLE)	break;	else	return;
		}			

	switch (CFG_SIO[no].protocol)
	{
		case SB_COM_REDIRECT_MODE	: PORT_PID[no] = Task_Launch ("/sbin/com_redirect", no); 	break;
		case SB_TCP_SERVER_MODE	    : PORT_PID[no] = Task_Launch ("/sbin/tcp_server", no); 	break;
		case SB_TCP_CLIENT_MODE	    : PORT_PID[no] = Task_Launch ("/sbin/tcp_client", no); 	break;
		case SB_TCP_BROADCAST_MODE  : PORT_PID[no] = Task_Launch ("/sbin/tcp_broadcast", no); break;
		case SB_TCP_MULTIPLEX_MODE  : PORT_PID[no] = Task_Launch ("/sbin/tcp_multiplex", no); break;
		case SB_UDP_SERVER_MODE     : PORT_PID[no] = Task_Launch ("/sbin/udp", no); 		break;
		case SB_UDP_CLIENT_MODE     : PORT_PID[no] = Task_Launch ("/sbin/udp", no); 		break;
	}
}	
//==========================================================
pid_t Task_Launch  (char *tname, int no)
{
char arg[40];
pid_t pid;

	sprintf(arg, "%d", no+1);
	pid = vfork ();
	if (pid < 0) return 0;

	if (pid==0) 		// child process
		{
		execl(tname, tname, arg, (char *)0);
		exit (0);
		}
	else
		{
		return pid;
		}
}	
//***************************************************************************
void sig_chld (int signo)
{
	pid_t pid;
	int stat, no;

	for (no=0; no<Number_Of_Ports; no++)
	{
		if (CFG_SIO[no].protocol == SB_DISABLE_MODE) continue;

		if ( (pid = waitpid(PORT_PID[no], &stat, WNOHANG)) > 0 ) 
		{
			SB_msleep (1000);
			Fork_Processor (no);
		}
	}

	if ( (pid = waitpid(DETECT_PID, &stat, WNOHANG)) > 0 ) 
	{
		printf ("Sytem Event pid=%d, DTD pid=%d\n", pid, DETECT_PID);
		SB_msleep (1000);
		DETECT_PID = Task_Launch ("/sbin/detect", 0);
	}

	if (CFG_SYS.portview[0] != 0)
	{
		if ( (pid = waitpid(PORTVIEW_PID, &stat, WNOHANG)) > 0 ) 
		{
			printf ("Sytem Event pid=%d, PORTVIEW pid=%d\n", pid, PORTVIEW_PID);
			SB_msleep (1000);
			PORTVIEW_PID = Task_Launch ("/sbin/portview", 0);
		}
	}
}

//===============================================================================
int check_proc (char *task_name)
{
DIR *dp;
struct dirent *entry;
char ch;
int ret, fd, pid_no, i, ret_code=0;
char work[100];
		
		if ((dp = opendir ("/proc/")) == NULL)
			{
			printf ("proc dir not fount !\n");
			return ret_code;;
			}

		chdir ("/proc/");
		while ((entry = readdir (dp)) != NULL)
			{
			if (strcmp (".",  entry->d_name) == 0) continue;
			if (strcmp ("..", entry->d_name) == 0) continue;
			ch = entry->d_name[0];
			if (!(ch >= '0' && ch <= '9')) continue;
			pid_no = atoi (entry->d_name);
			sprintf (work, "/proc/%d/cmdline", pid_no);
			fd = open (work, 0);
			if (fd < 0) continue;
				
			for (i=0; i<90; i++) work[i] = 0x00;
			ret = read (fd, work, 90);
			close (fd);		
			
			if (ret <= 0) continue;
			if (!strncmp (task_name, work, strlen(task_name))) 
				{
//				printf ("Task name=%-20s, PID=%d\n", work, pid_no);
				ret_code = 1;
				break;
				}
			}
		closedir (dp);

	return ret_code;
}

//===============================================================================
int SNMP_INIT ()
{
int wfd, i, no, cnt=0;
void *shared_memory = (void *)0;
FILE *fd;
char tmp[100][90];

	shared_memory = SB_SetSharedMemory (SB_SNMP_KEY, sizeof (struct SB_SNMP_STRUCT));
	if (shared_memory == (void *)-1)
		{
		printf ("Didn't initialize Shared Memory\n");			
		return -1;
		}
	SSM = (struct SB_SNMP_STRUCT *)shared_memory;

	memcpy(SSM->system.mac,		CFG_SYS.mac, 6);
	memcpy(SSM->system.ip,		CFG_SYS.ip, 		sizeof(CFG_SYS.ip));
	memcpy(SSM->system.mask,	CFG_SYS.mask,	sizeof(CFG_SYS.mask));
	memcpy(SSM->system.gateway,	CFG_SYS.gateway,	sizeof(CFG_SYS.gateway));
	memcpy(SSM->system.dns,		CFG_SYS.dns,		sizeof(CFG_SYS.dns));
	memcpy(SSM->system.ddns,	CFG_SYS.ddns,	sizeof(CFG_SYS.ddns));
	memcpy(SSM->system.portview,CFG_SYS.portview,sizeof(CFG_SYS.portview));
	SSM->system.line = CFG_SYS.line; 
	memcpy(SSM->system.device_name, CFG_SYS.device_name,	sizeof(CFG_SYS.device_name));
	memcpy(SSM->system.username,	CFG_SYS.username,	sizeof(CFG_SYS.username));
	memcpy(SSM->system.password,	CFG_SYS.password,	sizeof(CFG_SYS.password));
	SSM->system.portview_port_no =	CFG_SYS.portview_port_no;
	SSM->system.snmp_agent = 	CFG_SNMP.enable;
	SSM->system.telnet_server = CFG_SYS.telnet_server;
	SSM->system.ftp_server =	CFG_SYS.ftp_server;
	SSM->system.web_server =	CFG_SYS.web_server;
	SSM->system.ssh_server =	CFG_SYS.ssh_server;
	SSM->system.target_agent =	CFG_SYS.target_agent;
//	SSM->system.model =			CFG.model;
	
	memcpy(SSM->system.ddnsuser, CFG_SYS.ddnsuser, sizeof(CFG_SYS.ddnsuser));
	memcpy(SSM->system.ddnspass, CFG_SYS.ddnspass, sizeof(CFG_SYS.ddnspass));
	memcpy(SSM->system.website, CFG_SYS.website, sizeof(CFG_SYS.website));
	memcpy(SSM->system.contact, CFG_SYS.contact, sizeof(CFG_SYS.contact));
	
	SSM->system.v1_readwrite = CFG_SNMP.v1_readwrite;
	SSM->system.v3_readwrite = CFG_SNMP.v3_readwrite;
	memcpy(SSM->system.v3_username, CFG_SNMP.v3_username, sizeof(CFG_SNMP.v3_username));
	memcpy(SSM->system.v3_password, CFG_SNMP.v3_password, sizeof(CFG_SNMP.v3_password));
	
	memcpy(SSM->trap.trap_ip,	CFG_SNMP.trap_ip,		sizeof(CFG_SNMP.trap_ip));
	SSM->trap.trap_port = CFG_SNMP.trap_port;
	SSM->trap.trap_option = CFG_SNMP.trap_option;

	SB_GetVersion ('B', SSM->admin.bl_ver);
	SB_GetVersion ('K', SSM->admin.os_ver);
	SB_GetVersion ('F', SSM->admin.fs_ver);
	SSM->admin.reboot = 0;

	for (no=0; no<SB_MAX_SIO_PORT; no++)
		{
		memcpy(SSM->serial[no].name, CFG_SIO[no].name, sizeof(CFG_SIO[no].name));
		SSM->serial[no].socket_no = CFG_SIO[no].socket_no;		
		SSM->serial[no].protocol = CFG_SIO[no].protocol;		
		SSM->serial[no].device = CFG_SIO[no].device;		
		memcpy(SSM->serial[no].remote_ip, CFG_SIO[no].remote_ip, sizeof(CFG_SIO[no].remote_ip));
		SSM->serial[no].remote_socket_no = CFG_SIO[no].remote_socket_no;		
		SSM->serial[no].packet_latency_time = CFG_SIO[no].packet_latency_time;		
		SSM->serial[no].keepalive = CFG_SIO[no].keepalive;		
		SSM->serial[no].login = CFG_SIO[no].login;		
		memcpy(SSM->serial[no].login_name, CFG_SIO[no].login_name, 8);
		memcpy(SSM->serial[no].login_password, CFG_SIO[no].login_password, 8);
		
		SSM->serial[no].connection_count = 0;	// task reconnect count 	
		SSM->serial[no].connection_stat  = 0;	// task state 0:idle, 1=connection	
		SSM->serial[no].reset = 0;				// set command (port reset )

		SSM->mib[no].port_index = no;
		SSM->mib[no].speed = CFG_SIO[no].speed;	
		SSM->mib[no].dps = CFG_SIO[no].dps;		
		SSM->mib[no].flow = CFG_SIO[no].flow;		
		SSM->mib[no].interface = SB_RS232;
		//--------------------------------------------------RTS	
		SSM->mib[no].signals[0].port_index	= 0;
		SSM->mib[no].signals[0].name		= 1;	// rts
		SSM->mib[no].signals[0].sig_state	= 1;	// 1=on,0=off
		SSM->mib[no].signals[0].sig_changes = 1;
		//--------------------------------------------------CTS
		SSM->mib[no].signals[1].port_index	= 1;
		SSM->mib[no].signals[1].name		= 2;	// cts
		SSM->mib[no].signals[1].sig_state	= 0;	// 1=on,0=off
		SSM->mib[no].signals[1].sig_changes = 0;
		//--------------------------------------------------DSR
		SSM->mib[no].signals[2].port_index	= 2;
		SSM->mib[no].signals[2].name		= 3;	// dsr
		SSM->mib[no].signals[2].sig_state	= 0;	// 1=on,0=off
		SSM->mib[no].signals[2].sig_changes = 0;
		//--------------------------------------------------DTR
		SSM->mib[no].signals[3].port_index	= 3;
		SSM->mib[no].signals[3].name		= 4;	// dtr
		SSM->mib[no].signals[3].sig_state	= 1;	// 1=on,0=off
		SSM->mib[no].signals[3].sig_changes = 1;
		//--------------------------------------------------DCD
		SSM->mib[no].signals[4].port_index	= 4;
		SSM->mib[no].signals[4].name		= 6;	// dcd
		SSM->mib[no].signals[4].sig_state	= 0;	// 1=on,0=off
		SSM->mib[no].signals[4].sig_changes = 0;

		SSM->mib[no].parity_errs = 0;
		SSM->mib[no].frame_errs	 = 0;
		SSM->mib[no].overrun_errs = 0;
		}
		
	for (no=0; no<3/*SB_MAX_GPIO_PORT*/; no++)
		{
		SSM->gpio[no].socket_no	= 0;
		SSM->gpio[no].mode		= CFG_GPIO.mode[no]; 
		SSM->gpio[no].value		= CFG_GPIO.value[no];
		}

	//-------------------- snmpd.conf --------------------------------
	if ((fd = fopen(SNMPD_CONF, "r")) == NULL) return -1;
	while (1) 
		{
		if (fgets(tmp[cnt], 90, fd) == NULL)	break;
		cnt++;	
		}
	fclose(fd);

	for (i=0; i<cnt; i++)
		{	
		if (strncmp (tmp[i], "trapsink",   8) == 0)
			sprintf (tmp[i], "trapsink   %d.%d.%d.%d%c%c%c", CFG_SNMP.trap_ip[0],CFG_SNMP.trap_ip[1],CFG_SNMP.trap_ip[2],CFG_SNMP.trap_ip[3],0x0d,0x0a,0);
		if (strncmp (tmp[i], "trap2sink",  9) == 0)	
			sprintf (tmp[i], "trap2sink   %d.%d.%d.%d%c%c%c", CFG_SNMP.trap_ip[0],CFG_SNMP.trap_ip[1],CFG_SNMP.trap_ip[2],CFG_SNMP.trap_ip[3],0x0d,0x0a,0);
		if (strncmp (tmp[i], "informsink", 10) == 0)	
			sprintf (tmp[i], "informsink   %d.%d.%d.%d%c%c%c", CFG_SNMP.trap_ip[0],CFG_SNMP.trap_ip[1],CFG_SNMP.trap_ip[2],CFG_SNMP.trap_ip[3],0x0d,0x0a,0);
		}

	switch (CFG_SNMP.v1_readwrite)
		{
		case 0 :	
			sprintf (tmp[i], "rocommunity	public%c%c%c", 0x0d, 0x0a, 0);
			break;
		case 1 :	
			sprintf (tmp[i], "rwcommunity	public%c%c%c", 0x0d, 0x0a, 0);
			break;
		default :
			sprintf (tmp[i], "rocommunity	disable%c%c%c", 0x0d, 0x0a, 0);
			break;
		}

	sprintf (tmp[i+1], "createUser	%s	MD5	%s%c%c%c", CFG_SNMP.v3_username, CFG_SNMP.v3_password, 0x0d, 0x0a, 0);
	
	if (CFG_SNMP.v3_readwrite == 0)
		sprintf (tmp[i+2], "rouser %s auth%c%c%c", CFG_SNMP.v3_username, 0x0d, 0x0a, 0);
	else	
		sprintf (tmp[i+2], "rwuser %s auth%c%c%c", CFG_SNMP.v3_username, 0x0d, 0x0a, 0);
	
	wfd = open (SNMPD_CONF, O_WRONLY|O_CREAT, 0644);
	if (wfd <= 0) return -1;
	lseek (wfd, 0, 0);		
	for (i=0; i<cnt+3; i++) write (wfd, tmp[i], strlen(tmp[i]));
	close (wfd);

	system ("/usr/snmp/snmpd -c /usr/snmp/snmpd.conf");
	SNMPD_PID = 1;
	return 1;
}

//===============================================================================
void make_dhcpd_conf ()
{
FILE *fd;
char tmp[80][90];
int i, wfd, cnt=0;

	if ((fd = fopen("/etc/udhcpd.conf", "r")) == NULL) return;
	while (1) 
		{
		if (fgets(tmp[cnt], 90, fd) == NULL)	break;
		cnt++;	
		}
	fclose(fd);

	sprintf (tmp[1], "start	%d.%d.%d.%d%c%c%c", CFG_SYS.dhcpstartIP[0], CFG_SYS.dhcpstartIP[1], CFG_SYS.dhcpstartIP[2], CFG_SYS.dhcpstartIP[3],0x0d,0x0a,0);
	sprintf (tmp[2], "end	%d.%d.%d.%d%c%c%c", CFG_SYS.dhcpendIP[0], CFG_SYS.dhcpendIP[1], CFG_SYS.dhcpendIP[2], CFG_SYS.dhcpendIP[3],0x0d,0x0a,0);
	sprintf (tmp[cnt+0], "opt		dns	%d.%d.%d.%d%c%c%c", CFG_SYS.dns[0], CFG_SYS.dns[1], CFG_SYS.dns[2], CFG_SYS.dns[3], 0x0d, 0x0a, 0);
	sprintf (tmp[cnt+1], "option		subnet	%d.%d.%d.%d%c%c%c", CFG_SYS.lanmask[0], CFG_SYS.lanmask[1], CFG_SYS.lanmask[2], CFG_SYS.lanmask[3], 0x0d, 0x0a, 0);
	sprintf (tmp[cnt+2], "opt		router	%d.%d.%d.%d%c%c%c", CFG_SYS.lanip[0], CFG_SYS.lanip[1], CFG_SYS.lanip[2], CFG_SYS.lanip[3], 0x0d, 0x0a, 0);
	sprintf (tmp[cnt+3], "option		domain	local%c%c%c", 0x0d, 0x0a, 0);
	sprintf (tmp[cnt+4], "option		lease	%u%c%c%c", CFG_SYS.lease_time*60, 0x0d, 0x0a, 0);

	wfd = open ("/etc/udhcpd.conf", O_WRONLY|O_CREAT, 0644);
	if (wfd <= 0) return;
	lseek (wfd, 0, 0);		
	for (i=0; i<cnt+5; i++) write (wfd, tmp[i], strlen(tmp[i]));
	close (wfd);

	system ("echo 1 > /var/lib/misc/udhcpd.leases");
}

#ifdef LCD_SUPPORT
/*==================================================================================*/
#include "lcd_font/font_8x8.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdarg.h>

#define CFAG12864B_WIDTH		(128)
#define CFAG12864B_HEIGHT		(64)
#define CFAG12864B_SIZE			(128 * 64 / 8)
#define CFAG12864B_BPB			(8)
#define CFAG12864B_ADDRESS(x, y)	((y) * CFAG12864B_WIDTH / \
		CFAG12864B_BPB + (x) / CFAG12864B_BPB)
#define CFAG12864B_BIT(n)		(((unsigned char) 1) << (n))

#undef CFAG12864B_DOCHECK
#ifdef CFAG12864B_DOCHECK
#define CFAG12864B_CHECK(x, y)		((x) < CFAG12864B_WIDTH && \
		(y) < CFAG12864B_HEIGHT)
#else
#define CFAG12864B_CHECK(x, y)		(1)
#endif

unsigned char * cfag12864b_mem;
unsigned char cfag12864b_buffer[CFAG12864B_SIZE];

int cfag12864b_init(char *path);
void cfag12864b_exit(int fd);
void SB_LCDclear(void);
int SB_LCDputchar(int loc_x, int loc_y, char ascii );
int SB_LCDprintf(int x, int y, const char *fmt, ...);
void SB_LCDwrite(void);
//------------------------------------------------------------------------------------
void lcd ()
{
int lcdfd;
char bl[20], os[20], fs[20];
struct in_addr addr;

/*
	system ("insmod /etc/drivers/cfbcopyarea.ko");
	system ("insmod /etc/drivers/cfbfillrect.ko");
	system ("insmod /etc/drivers/cfbimgblt.ko");
	system ("insmod /etc/drivers/eddy_lcd.ko");
	system ("insmod /etc/drivers/eddy_lcdfb.ko");
	sleep (2);
*/	
	lcdfd = cfag12864b_init("/dev/fb0");
	if (lcdfd < 0) return;

	SB_LCDclear ();
	SB_GetVersion ('B', bl);
	SB_GetVersion ('K', os);
	SB_GetVersion ('F', fs);
	addr.s_addr = SB_GetIp (SB_WAN_ETH_NAME);

	SB_LCDprintf(0, 0, "Welcome to Eddy\n\n");
	SB_LCDprintf(0, 2, "B/L Ver:%s", bl);
	SB_LCDprintf(0, 3, "O/S Ver:%s", os);
	SB_LCDprintf(0, 4, "F/S Ver:%s", fs);
	SB_LCDprintf(0, 6, "(%s)", inet_ntoa(addr));
	SB_LCDprintf(0, 7, "MAC:%02x%02x%02x%02x%02x%02x", CFG_SYS.mac[0],CFG_SYS.mac[1],CFG_SYS.mac[2],CFG_SYS.mac[3],CFG_SYS.mac[4],CFG_SYS.mac[5]);
	SB_LCDwrite ();
	cfag12864b_exit(lcdfd);
	
	CFG_DIO.lcd = SB_DISABLE;
}

int cfag12864b_init(char *path)
{
int cfag12864b_fd;
	
	cfag12864b_fd = open(path, O_RDWR);
	if (cfag12864b_fd == -1)
		return -1;

	cfag12864b_mem = mmap(0, CFAG12864B_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, cfag12864b_fd, 0);
	if (cfag12864b_mem == MAP_FAILED) 
		{
		close(cfag12864b_fd);
		return -2;
		}

	return cfag12864b_fd;
}

void cfag12864b_exit(int fd)
{
	munmap(cfag12864b_mem, CFAG12864B_SIZE);
	close(fd);
}
/*
 * clear (unset all pixels)
 */
void SB_LCDclear(void)
{
	unsigned short i;

	for (i = 0; i < CFAG12864B_SIZE; i++)
		cfag12864b_buffer[i] = 0;
	SB_LCDwrite ();	
}

/*
 * deformat
 */ 

int SB_LCDputchar(int loc_x, int loc_y, char ascii )
{
	int font_x, font_y;
	int i;
	char tmp_buffer[8][8][16];
	
	memcpy(tmp_buffer,cfag12864b_buffer,CFAG12864B_SIZE);	
	
	font_x = ((int)ascii) % 16;
	font_y = (int) (ascii / 16);
		
//	printf("(%d,%d \n",loc_x,loc_y);
		
	for(i = 0; i < 8; i++)
	{
		tmp_buffer[loc_y][i][loc_x] = fontdata_8x8[font_y][i][font_x];
//		printf("(%d,%d,(%02x)(%02x)\n",font_x,font_y,fontdata_8x8[font_y][i][font_x],(int)ascii);
	}
	
	memcpy(cfag12864b_buffer,tmp_buffer,CFAG12864B_SIZE);	
		
	return 0;
}

//==========================================================================
int SB_LCDprintf(int x, int y, const char *fmt, ...)  // x=0~15, y=0~7
{	
int i;
va_list args;
char buf[300];
	
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

	if (x > 15 || y > 7) return -1;
	for(i=0; i<strlen(buf); i++)
		{
		if(x > 15)
			{
			y++;
			x = 0;
			}
		if(y > 7) break;
		if (buf[i] == 0x0a)
			{
			x = 0;  
			y ++;
			}
		else		
			{
			SB_LCDputchar(x++, y, buf[i] );
			}
		}

	return (i);
}


/*
 * blit buffer to lcd
 */

void SB_LCDwrite(void)
{
	memcpy(cfag12864b_mem, cfag12864b_buffer, CFAG12864B_SIZE);
}
#endif
