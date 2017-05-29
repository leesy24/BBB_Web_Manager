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

#define PROCDIR "/proc/"
#define UPLOAD_FILE_NAME	"/tmp/image_file"
#include <errno.h>

typedef int bool;
#define true 1
#define false 0

/* size of read/write buffer */
#define BUFFERSIZE (10 * 1024)

static int dev_fd = -1;
static int fil_fd = -1;


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
#define	HEX_PA22_AND	0xffbfffff	
#define	HEX_PA30_AND	0xbfffffff	
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
#define	HEX_PC24_AND	0xfeffffff  
#define	HEX_PC25_AND	0xfdffffff  
#define	HEX_PC26_AND	0xfbffffff  
                
                
LIST list;      
void read_conf();
void set_login();
void set_logout();
int set_network(char **error_msg);
int set_serial();
void set_gpio();
void set_password();
void set_update();
void set_onlyupdate();
void set_reboot();
void set_test();
void set_snmp();
void set_dio();
void set_wireless();

int Product_ID;
//---------------------------------------------------------------------------
int cgiMain()
{
	char passBuff[12];
	char buff[256];
	char error_msg[256];
	char *msg;
	char target_page[64];
	char type;
	int i;
	int mode=0;	// 0: html, 1: cgi
	struct	SB_SYSTEM_CONFIG	cfg;
	int ret;

   SB_ReadConfig  (CFGFILE_ETC_SYSTEM,(char *)&cfg, sizeof (struct SB_SYSTEM_CONFIG));
    for (i=0; i<4; i++) buff[i] = cgiQueryString[i];
    buff[i] = '\0';
    if (!strcmp(buff, "type"))
	   type = cgiQueryString[5];
    else
	   Error("URL Parse Error");

    list = listCreate();
    strcpy(buff, cfg.device_name);
    listPutf(list, "device_name", buff);
    strcpy(buff, cfg.username);
    listPutf(list, "username", buff);

	Product_ID = SB_GetModel ();

	switch (type)
	{
		case 'l':	// login -> main.html
			strcpy(target_page, "/cgi-bin/getagent.cgi?type=1");
			set_login();
			mode = 1;
			break;
		case 'o':	// logout -> index.html
			strcpy(target_page, "logout.html");
			set_logout();
			break;
		case '1':	// network
			strcpy(target_page, "/cgi-bin/getagent.cgi?type=2");
			ret = set_network(&msg);
			if(ret)
			{
				sprintf(error_msg, "Error: %s", msg);
				Error(error_msg);
			}
			mode = 1;
			break;
#if 0
		case 'a':	// adc
			strcpy(target_page, "/cgi-bin/getagent.cgi?type=3");
			set_adc();
			mode = 1;
			break;
#endif
		case 'd':	// DIO
			strcpy(target_page, "/cgi-bin/getagent.cgi?type=d");
			set_dio();
			mode = 1;
			break;

		case '2':	// serial
			i = set_serial();
			switch (i)
			{	
				case 0 : strcpy(target_page, "/cgi-bin/getagent.cgi?type=A");  break;
				case 1 : strcpy(target_page, "/cgi-bin/getagent.cgi?type=B");  break;
				case 2 : strcpy(target_page, "/cgi-bin/getagent.cgi?type=C");  break;
				case 3 : strcpy(target_page, "/cgi-bin/getagent.cgi?type=D");  break;
				case 4 : strcpy(target_page, "/cgi-bin/getagent.cgi?type=E");  break;
				case 5 : strcpy(target_page, "/cgi-bin/getagent.cgi?type=F");  break;
				case 6 : strcpy(target_page, "/cgi-bin/getagent.cgi?type=G");  break;																
				case 7 : strcpy(target_page, "/cgi-bin/getagent.cgi?type=H");  break;
			}	
			mode = 1;
			break;
		case '3':	// gpio
			strcpy(target_page, "/cgi-bin/getagent.cgi?type=4");
			set_gpio();
			mode = 1;
			break;
		case '4':	// snmp
			strcpy(target_page, "/cgi-bin/getagent.cgi?type=5");
			set_snmp();
			mode = 1;
			break;
		case '5':	// password
			strcpy(target_page, "afterPassword.html");
			set_password();
			break;
		case '6':	// update
			set_update();
			strcpy(target_page, "afterUpdate.html");
			break;
		case '7':	// onlyupdate
			set_onlyupdate();
			strcpy(target_page, "afterComplete.html");
			break;
		case '8':	// factory
			listPutf(list, "ip", "192.168.0.223");
			system("/sbin/def factory");
			strcpy(target_page, "afterFactory.html");
			break;
		case '9':	// save & reboot
			set_reboot();
			strcpy(target_page, "afterReboot.html");
			break;
		case '0':	// onlyreboot
			if (CheckLogin() == 1)
			{
				set_logout();
				system("/usr/bin/sudo /sbin/reboot");
				return;	// return directly
			}
			else
			{
				strcpy(target_page, "index.html");
			}
			break;
		case 'J':	// wireless
			strcpy(target_page, "/cgi-bin/getagent.cgi?type=J");
			set_wireless();
			mode = 1;
			break;
		default:
			//DebugMessage("Page doesn't exist!");
			set_test();
			strcpy(target_page, "/cgi-bin/getagent.cgi?type=1");
			mode = 1;
			break;
	}

	// check login
	if (type != 'o' && type != '0')
	{
		if (CheckLogin() == 0)  Error("Please login first!");
	}

	if (mode == 0)	// HTML
	{
		sprintf(buff, "%s/%s", HTMLPATH, target_page);
		listDisplay(list, buff);
		fflush(0);
		listDelete(list);
		if (type == '7')  sleep (2);
	}
	else		// CGI
	{
		fflush(0);
		fprintf(cgiOut, "\n");
		fprintf(cgiOut, "<html>\n<head>\n");
		fprintf(cgiOut, "<meta http-equiv='refresh' content='0; URL=%s'>\n", target_page);
		fprintf(cgiOut, "</head>\n</html>\n");
		fflush(0);
	}

	return 0;
}
//---------------------------------------------------------------------------
void set_login()
{
    char buff[16];
    char buff2[16];
    char temp[256];
    int value;

    // read from form
    value = cgiFormStringNoNewlines("username", buff, 16);
    if (value == cgiFormEmpty)   Error("Enter username");
    if (CheckAdmin(0, buff) == 0) Error("Invalid username");

    // read from form
    value = cgiFormStringNoNewlines("pw", buff, 16);
    if (value == cgiFormNotFound)
	   Error("Password error");
    else if (value == cgiFormEmpty)
	   Error("Enter password");

    if (CheckAdmin(1, buff) == 0)
	   Error("Invalid password");

    value = open("/var/tmp/login.stat", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (value < 0)   Error("Internal login error");
    write(value, "Y", 1);
    close(value);
}
//---------------------------------------------------------------------------
void set_logout()
{
    unlink("/var/tmp/login.stat");
}
//---------------------------------------------------------------------------
static char ERROR_WAN_LINE_TYPE[] = "Check WAN Line Type!";
static char ERROR_WAN_IP_ADDR[] = "Check WAN IP Address!";
static char WARNNIG_NOTHING_SUBMIT[] = "There is noting to submit.";
int set_network(char **error_msg)
{
    char buff[64];
    char addr[4];
    char cmd[256];
    int new_value;
    int old_value;
	cgiFormResultType ret;
	bool changed;
	struct SB_SYSTEM_CONFIG	cfg;

	SB_ReadConfig(CFGFILE_ETC_SYSTEM, (char *)&cfg, sizeof(struct SB_SYSTEM_CONFIG));			

	changed = false;
    if (cfg.line == 'I')  old_value = 0; else old_value = 1;
    ret = cgiFormInteger("N_LINE", &new_value, old_value);
	if(ret != cgiFormSuccess)
	{
		*error_msg = ERROR_WAN_LINE_TYPE;
		return -1;
	}

	if(new_value != old_value) changed = true;
	if (new_value == 0)		// static
	{
		cfg.line = 'I';
		ret = cgiFormStringNoNewlines("N_IP", buff, 16);
		if (ret == cgiFormSuccess)
		{
			ret = convert_address(buff, addr);
			if(ret)
			{
				*error_msg = ERROR_WAN_IP_ADDR;
				return -1;
			}
			//fprintf(stderr, "sizeof(cfg.ip) = %d\n", sizeof(cfg.ip));
			//fprintf(stderr, "cfg.ip = %d%d%d%d\n", cfg.ip[0], cfg.ip[1], cfg.ip[2], cfg.ip[3]);
			//fprintf(stderr, "addr = %d%d%d%d\n", addr[0], addr[1], addr[2], addr[3]);
			ret = memcmp(addr, cfg.ip, sizeof(cfg.ip));
			//fprintf(stderr, "memcmp ret = %d\n", ret);
			if(ret != 0)
			{
				changed = true;
			}
			memcpy(cfg.ip, addr, sizeof(cfg.ip));
		}
		ret = cgiFormStringNoNewlines("N_MASK", buff, 16);
		if (ret != cgiFormEmpty)  convert_address (buff, cfg.mask);
		ret = cgiFormStringNoNewlines("N_GW", buff, 16);
		if (ret != cgiFormEmpty)  convert_address (buff, cfg.gateway);
		ret = cgiFormStringNoNewlines("N_DNS", buff, 16);
		if (ret != cgiFormEmpty)  convert_address (buff, cfg.dns);
		ret = cgiFormStringNoNewlines("N_DNS", buff, 16);
		if (ret != cgiFormEmpty)  convert_address (buff, cfg.dns);
		ret = cgiFormStringNoNewlines("N_DNS_S", buff, 16);
		if (ret != cgiFormEmpty)  convert_address (buff, cfg.dns_s);
	}
	else
	{
		cfg.line = 'D';
	}

    ret = cgiFormStringNoNewlines("N_NIP", buff, 16);
    if (ret != cgiFormEmpty)  convert_address (buff, cfg.portview);
    ret = cgiFormStringNoNewlines("N_DDNS", buff, 16);
    if (ret != cgiFormEmpty)  convert_address (buff, cfg.ddns);
    ret = cgiFormStringNoNewlines("N_DDNSUSER", buff, 16);
    if (ret != cgiFormEmpty) sprintf(cfg.ddnsuser, "%s", buff);
    ret = cgiFormStringNoNewlines("N_DDNSPASS", buff, 16);
    if (ret != cgiFormEmpty) sprintf(cfg.ddnspass, "%s", buff);

    if (cfg.dhcpenable == 0)  old_value = 0; else old_value = 1;
    cgiFormInteger("N_DHCPENABLE", &new_value, old_value);
    if (new_value == 1)		// dhcpd
		cfg.dhcpenable = 1;
    else
		cfg.dhcpenable = 0;

	ret = cgiFormStringNoNewlines("N_STARTIP", buff, 16);	   
	if (ret != cgiFormEmpty)  convert_address (buff, cfg.dhcpstartIP);
	ret = cgiFormStringNoNewlines("N_ENDIP", buff, 16);
	if (ret != cgiFormEmpty)  convert_address (buff, cfg.dhcpendIP);

	ret = cgiFormStringNoNewlines("N_LANIP", buff, 16);
	if (ret != cgiFormEmpty)  convert_address (buff, cfg.lanip);

	ret = cgiFormStringNoNewlines("N_LANMASK", buff, 16);
	if (ret != cgiFormEmpty)  convert_address (buff, cfg.lanmask);

	ret = cgiFormStringNoNewlines("N_LEASETIME", buff, 6);
	if (ret != cgiFormEmpty) cfg.lease_time = atoi (buff);
	
    cgiFormInteger("N_IDE", &new_value, cfg.target_agent);
    cfg.target_agent = new_value;

    cgiFormInteger("N_TELNET", &new_value, cfg.telnet_server);
    cfg.telnet_server = new_value;

    cgiFormInteger("N_FTP", &new_value, cfg.ftp_server);
	cfg.ftp_server = new_value;
    
    cgiFormInteger("N_WEB", &new_value, cfg.web_server);
	cfg.web_server = new_value;

    cgiFormInteger("N_SSH", &new_value, cfg.ssh_server);
	cfg.ssh_server = new_value;

    ret = cgiFormStringNoNewlines("N_NPORT", buff, 6);
    if (ret != cgiFormEmpty) cfg.portview_port_no = atoi (buff);

    ret = cgiFormStringNoNewlines("N_DNAME", buff, 32);
    if (ret != cgiFormEmpty) sprintf(cfg.device_name, "%s", buff);
    
    SB_WriteConfig(CFGFILE_ETC_SYSTEM, (char *)&cfg, sizeof(struct SB_SYSTEM_CONFIG));

	if(changed != true)
	{
		*error_msg = WARNNIG_NOTHING_SUBMIT;
		return -2;
	}
	return 0;
}
//---------------------------------------------------------------------------
void set_snmp()
{
    char buff[64], tmp, flag=0;
    char cmd[256];
    int value;
struct SB_SNMP_CONFIG	cfg;

	SB_ReadConfig  (CFGFILE_ETC_SNMP,     (char *)&cfg,		sizeof(struct SB_SNMP_CONFIG));	
	
    cgiFormInteger("N_SNMP", &value, cfg.enable);
    if (cfg.enable == 1 && value == 1) flag = 1;
    cfg.enable = value;

    if (flag) {
    cgiFormInteger("N_V1_ATTRIB", &value, cfg.v1_readwrite);
    cfg.v1_readwrite = value;

    cgiFormInteger("N_V3_ATTRIB", &value, cfg.v3_readwrite);
    cfg.v3_readwrite = value;

    value = cgiFormStringNoNewlines("N_V3USER", buff, 16);
    if (value != cgiFormEmpty) sprintf(cfg.v3_username, "%s", buff);

    value = cgiFormStringNoNewlines("N_V3PASS", buff, 16);
    if (value != cgiFormEmpty)  sprintf(cfg.v3_password, "%s", buff);

    value = cgiFormStringNoNewlines("N_TRAPIP", buff, 16);
    if (value != cgiFormEmpty)  convert_address (buff, cfg.trap_ip);

    value = cgiFormStringNoNewlines("N_TRAPPORT", buff, 6);
    if (value != cgiFormEmpty)  cfg.trap_port = atoi (buff);

	tmp = cfg.trap_option & 0x01;
    cgiFormInteger("N_RESET_ALARM", &value, tmp);
    if (value == 1) 
    	cfg.trap_option |= 0x01;
    else	
		cfg.trap_option &= 0xfe;
		
	tmp = cfg.trap_option & 0x02;
	if (tmp == 0x02) tmp = 1; else tmp = 0;
    cgiFormInteger("N_CON_ALARM", &value, tmp);
    if (value == 1) 
    	cfg.trap_option |= 0x02;
    else	
		cfg.trap_option &= 0xfd;

	tmp = cfg.trap_option & 0x04;
	if (tmp == 0x04) tmp = 1; else tmp = 0;	
    cgiFormInteger("N_DIS_ALARM", &value, tmp);
    if (value == 1) 
    	cfg.trap_option |= 0x04;
    else	
		cfg.trap_option &= 0xfb;
    }
    SB_WriteConfig  (CFGFILE_ETC_SNMP,     (char *)&cfg,		sizeof(struct SB_SNMP_CONFIG));	
}
//=====================================================================
int convert_address (char *src, char *dest)
{
struct sockaddr_in sock;		// who to ping
int k,j,i,len,cnt;
char tmp[10];
	
	if ((sock.sin_addr.s_addr = inet_addr(src)) == INADDR_NONE)	return -1;
	len = strlen (src);
	for (cnt=0,i=0; i<len; i++) if (src[i] == '.') cnt++;
	if (cnt != 3) return -1;
		for (k=0,j=0,i=0; i<=len; i++,src++)
		{
		tmp[j++] = *src;
		if (*src == 0x00 || *src == '.')
			{
			tmp[--j] = 0; j=0;
			*dest++ = atoi (tmp);
			}
		}
	return 0;
}
//---------------------------------------------------------------------------
int set_serial()
{
    int value;
    int value2;
    int mode;
    int iftype;
    char buff[64];
    char cmd[256];
    char charval;
    int portno;
	struct SB_SIO_CONFIG			cfg [SB_MAX_SIO_PORT];

	SB_ReadConfig  (CFGFILE_ETC_SIO, (char *)&cfg[0],	sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);	   
    if ( cgiFormInteger("PAGENUM", &portno, 0) != cgiFormSuccess )
        Error("Where are your from?");

    portno -=1;
    
  	if (portno > 1)
		{	
	    cgiFormInteger("IFTYPE", &value, cfg[portno].interface);
    	cfg[portno].interface = value;
    	}    
    
    cgiFormInteger("OPMODE", &value, cfg[portno].protocol);
    cfg[portno].protocol = value;

	cgiFormInteger("LOCAL_PORT", &value, cfg[portno].socket_no);
    cfg[portno].socket_no = value;

    value = cgiFormStringNoNewlines("ALIAS", buff, 16); 
    if (value != cgiFormEmpty) sprintf(cfg[portno].name, "%s", buff);

    cgiFormInteger("BAUDRATE", &value, cfg[portno].speed);
    cfg[portno].speed = value;
 
    value2 = cfg[portno].dps & 0x03;
    cgiFormInteger("DATABIT", &value, value2);
	cfg[portno].dps &= 0xfc;
	cfg[portno].dps |= value;

    charval = cfg[portno].dps & 0x04;
    if (charval == 0x04) value2 = 1; else value2 = 0;
    cgiFormInteger("STOPBIT", &value, value2);
    cfg[portno].dps &= 0xfb;
    if (value == 1) cfg[portno].dps |= 0x04;    


    charval = cfg[portno].dps & 0x18;
    if (charval == 0x00)	value2 = 0;
    if (charval == 0x08)	value2 = 1;
    if (charval == 0x10 || charval == 0x18)	value2 = 2;    	    	
    cgiFormInteger("PARITY", &value, value2);
	cfg[portno].dps &= 0xe7;
	switch (value)	
		{
		case 0 :  break;
		case 1 :  cfg[portno].dps |= 0x08;    break;
		case 2 :  cfg[portno].dps |= 0x10;    break;	
		}

    cgiFormInteger("FLOW", &value, cfg[portno].flow);
    cfg[portno].flow = value;
	
    cgiFormInteger("DEVICETYPE", &value, cfg[portno].device);
    cfg[portno].device = value;

    value = cgiFormStringNoNewlines("SERVER_IP", buff, 16);
	if (value != cgiFormEmpty) convert_address (buff, cfg[portno].remote_ip);

	cgiFormInteger("SERVER_PORT", &value, cfg[portno].remote_socket_no);
    cfg[portno].remote_socket_no = value;
    
	cgiFormInteger("ALIVE_TIME", &value, cfg[portno].keepalive);
    cfg[portno].keepalive = value;

    cgiFormInteger("LATENCY_TIME", &value, cfg[portno].packet_latency_time);
    cfg[portno].packet_latency_time = value;
    
    cgiFormInteger("PASSIVELOGIN", &value, cfg[portno].login);
    cfg[portno].login = value;

    value = cgiFormStringNoNewlines("PASSIVE_USER", buff, 16); 
	if (cfg[portno].login == 1)
   		if (value != cgiFormEmpty) sprintf(cfg[portno].login_name, "%s", buff);

    value = cgiFormStringNoNewlines("PASSIVE_PASS", buff, 16); 
    if (cfg[portno].login == 1)
   		if (value != cgiFormEmpty) sprintf(cfg[portno].login_password, "%s", buff); 

save_flash:
    SB_WriteConfig  (CFGFILE_ETC_SIO, (char *)&cfg[0],	sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);	   

    return portno;
}
void set_wireless()
{
	int ret;
	int value;
	char cmd[256];
	char buff[256];
	struct SB_WIFI_CONFIG cfg;

	SB_ReadConfig  (CFGFILE_ETC_WIFI, (char *)&cfg,	sizeof(struct SB_WIFI_CONFIG));
	
	cgiFormInteger("W_USBWIFI", &value, cfg.wireless);
	cfg.wireless = value;

	cgiFormInteger("W_MODE", &value, cfg.wifi_mode);
	cfg.wifi_mode = value;

	cgiFormInteger("W_NWMODE", &value, cfg.b_g_mode);
	cfg.b_g_mode = value;

	memset (buff, 0, 256);
	if (cgiFormStringNoNewlines("W_SSID", buff, 32) != cgiFormNotFound)
		{
		if (!strlen(buff))
			sprintf(cfg.ssid, "\"\"");
		else
			sprintf(cfg.ssid, "%s", buff);
		}



	cgiFormInteger("W_CHANNEL", &value, cfg.channel);
	cfg.channel = value;
	
	cgiFormInteger("W_BITRATE", &value, cfg.bit_rate);
	cfg.bit_rate = value;

	cgiFormInteger("W_RTSTHR", &value, cfg.rts_thr);
	if (value < 1 || value > 2347) value = 2347;
	cfg.rts_thr = value;	
	

	cgiFormInteger("W_FRAGTHR", &value, cfg.frag_thr);
	if (value < 256 || value > 2346) value = 2346;
	cfg.frag_thr = value;	
	    
	cgiFormInteger("W_IEEE80211H", &value, cfg.ieee80211h);
	cfg.ieee80211h = value;


	cgiFormInteger("W_ROAMING", &value, cfg.roaming_mode);
	cfg.roaming_mode = value;



	cgiFormInteger("W_ROAMTHR", &value, cfg.roaming_threshold);
	if (value < 61 || value > 89) value = 75;
	cfg.roaming_threshold = value;	
		
	cgiFormInteger("W_AUTHMODE", &value, cfg.auth_mode);
	cfg.auth_mode = value;

	cgiFormInteger("W_ENCRYPTYPE", &value, cfg.encryp_type);
	cfg.encryp_type = value;
	ret = value;
	
	
	
	switch(ret)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			cgiFormInteger("W_KEYINDEX", &value, cfg.key_index);
			cfg.key_index = value;

			memset (buff, 0, 256);
			if (cgiFormStringNoNewlines("W_KEY1", buff, 32) != cgiFormNotFound)
			{
				if (!strlen(buff))
					sprintf(cfg.key, "\"\"");
				else
					sprintf(cfg.key, "%s", buff);
			}
			break;

			/*
			memset (buff, 0, 256);
			if (cgiFormStringNoNewlines("W_PASSPHRASE", buff, 65) != cgiFormNotFound)
			{
				if (!strlen(buff))
					sprintf(cfg.passphrase, "\"\"");
				else
					sprintf(cfg.passphrase, "%s", buff);
			}
			break;*/

		default:
			break;
	}

	//Wireless Network Address
	cgiFormInteger("W_LINE", &value, cfg.line);
	cfg.line = value;
	if (value > 0)		// Static IP
	{
		memset (buff, 0, 256);
		value = cgiFormStringNoNewlines("W_IP", buff, 16);
		if (value != cgiFormEmpty)  convert_address (buff, cfg.ip);

		memset (buff, 0, 256);
		value = cgiFormStringNoNewlines("W_MASK", buff, 16);
		if (value != cgiFormEmpty)  convert_address (buff, cfg.mask);

		memset (buff, 0, 256);
		value = cgiFormStringNoNewlines("W_GW", buff, 16);
		if (value != cgiFormEmpty)  convert_address (buff, cfg.gateway);

		memset (buff, 0, 256);
		value = cgiFormStringNoNewlines("W_DNS", buff, 16);
		if (value != cgiFormEmpty)  convert_address (buff, cfg.dns);

		memset (buff, 0, 256);
		value = cgiFormStringNoNewlines("W_DNS_S", buff, 16);
		if (value != cgiFormEmpty)  convert_address (buff, cfg.dns_s);
	}

	SB_WriteConfig (CFGFILE_ETC_WIFI, (char *)&cfg, sizeof(struct SB_WIFI_CONFIG));
}

/*
//---------------------------------------------------------------------------
void set_adc()
{
    int value;
    int value2;
    int mode;
    char buff[64];
struct SB_ADC_CONFIG	cfg;

	SB_ReadConfig  (CFGFILE_ETC_ADC,     (char *)&cfg,		sizeof(struct SB_ADC_CONFIG));		    

    cgiFormInteger("N_ENABLE", &value, cfg.enable);
    cfg.enable = value;

	if (cfg.enable == 1) 
		{
	    value = cgiFormStringNoNewlines("LOCALPORT", buff, 6);
    	if (value != cgiFormEmpty) cfg.socket_no = atoi(buff);

    	value = cgiFormStringNoNewlines("INTERVAL", buff, 6);
    	if (value != cgiFormEmpty) cfg.interval = atoi(buff);

    	value = cgiFormStringNoNewlines("KEEPALIVE", buff, 6);
    	if (value != cgiFormEmpty) cfg.keepalive = atoi(buff);	

	    if (cfg.channel & 0x01) mode = 0; else mode = 1;
    	cgiFormInteger("CHANNEL1", &value, mode);
	    if (value == 0)
    		cfg.channel &=  0x0e;
	    else	
    		cfg.channel |=  0x01;

	    if (cfg.channel & 0x02) mode = 0; else mode = 1;
    	cgiFormInteger("CHANNEL2", &value, mode);
	    if (value == 0)
    		cfg.channel &=  0x0d;
	    else	
    		cfg.channel |=  0x02;

    	if (cfg.channel & 0x04) mode = 0; else mode = 1;
    	cgiFormInteger("CHANNEL3", &value, mode);
    	if (value == 0)
	    	cfg.channel &=  0x0b;
    	else	
    		cfg.channel |=  0x04;

    	if (cfg.channel & 0x02) mode = 0; else mode = 1;
	    cgiFormInteger("CHANNEL4", &value, mode);
    	if (value == 0)
    		cfg.channel &=  0x07;
    	else	
    		cfg.channel |=  0x08;
		}    	
    SB_WriteConfig  (CFGFILE_ETC_ADC,     (char *)&cfg,		sizeof(struct SB_ADC_CONFIG));		    	
}
*/
//---------------------------------------------------------------------------
void set_gpio()                                                   
{																	
int value, value2, gg, gv, gm;
struct SB_GPIO_CONFIG	cfg;
                                                                                                              
	SB_ReadConfig  (CFGFILE_ETC_GPIO,     (char *)&cfg,		sizeof(struct SB_GPIO_CONFIG));		              
 	
	if (Product_ID != EDDY_S4M)
		{
		cgiFormInteger("SIO1",     &value, cfg.serial_1);					
		cfg.serial_1 = value;
		if (value == 0)	{				// GPIO
			cfg.enable[GPORT_B] |= HEX_PB04;		cfg.enable[GPORT_B] |= HEX_PB05;
			cfg.enable[GPORT_B] |= HEX_PB22;		cfg.enable[GPORT_B] |= HEX_PB23;
			cfg.enable[GPORT_B] |= HEX_PB24;		cfg.enable[GPORT_B] |= HEX_PB25;
			cfg.enable[GPORT_B] |= HEX_PB26;		cfg.enable[GPORT_B] |= HEX_PB27;
		} else {
			cfg.enable[GPORT_B] &= HEX_PB04_AND;	cfg.enable[GPORT_B] &= HEX_PB05_AND;
			cfg.enable[GPORT_B] &= HEX_PB22_AND;	cfg.enable[GPORT_B] &= HEX_PB23_AND;
			cfg.enable[GPORT_B] &= HEX_PB24_AND;	cfg.enable[GPORT_B] &= HEX_PB25_AND;
			cfg.enable[GPORT_B] &= HEX_PB26_AND;	cfg.enable[GPORT_B] &= HEX_PB27_AND;	}
					
		cgiFormInteger("SIO2",     &value, cfg.serial_2);	
		cfg.serial_2 = value;
		if (value == 0)	{				// GPIO
			cfg.enable[GPORT_B] |= HEX_PB06;		cfg.enable[GPORT_B] |= HEX_PB07;
			cfg.enable[GPORT_B] |= HEX_PB28;		cfg.enable[GPORT_B] |= HEX_PB29;
		} else {
			cfg.enable[GPORT_B] &= HEX_PB06_AND;	cfg.enable[GPORT_B] &= HEX_PB07_AND;
			cfg.enable[GPORT_B] &= HEX_PB28_AND;	cfg.enable[GPORT_B] &= HEX_PB29_AND;	}
	
		cgiFormInteger("SIO3",     &value, cfg.serial_3);	
		cfg.serial_3 = value;
		if (value == 0)	{				// GPIO
			cfg.enable[GPORT_A] |= HEX_PA04;		cfg.enable[GPORT_A] |= HEX_PA05;
			cfg.enable[GPORT_B] |= HEX_PB08;		cfg.enable[GPORT_B] |= HEX_PB09;
		} else {
			cfg.enable[GPORT_A] &= HEX_PA04_AND;	cfg.enable[GPORT_A] &= HEX_PA05_AND;
			cfg.enable[GPORT_B] &= HEX_PB08_AND;	cfg.enable[GPORT_B] &= HEX_PB09_AND;	}
	
		cgiFormInteger("SIO4",     &value, cfg.serial_4);	
		cfg.serial_4 = value;
		if (value == 0)	{				// GPIO
			cfg.enable[GPORT_B] |= HEX_PB10;		cfg.enable[GPORT_B] |= HEX_PB11;
			cfg.enable[GPORT_C] |= HEX_PC08;		cfg.enable[GPORT_C] |= HEX_PC10;
		} else {
			cfg.enable[GPORT_B] &= HEX_PB10_AND;	cfg.enable[GPORT_B] &= HEX_PB11_AND;
			cfg.enable[GPORT_C] &= HEX_PC08_AND;	cfg.enable[GPORT_C] &= HEX_PC10_AND;	}
	
		cgiFormInteger("DEBUGPORT",     &value, cfg.debugport);					
		cfg.debugport = value;
		if (value == 0)	{				// GPIO
			cfg.enable[GPORT_B] |= HEX_PB14;		cfg.enable[GPORT_B] |= HEX_PB15;
		} else {
			cfg.enable[GPORT_B] &= HEX_PB14_AND;	cfg.enable[GPORT_B] &= HEX_PB15_AND;	}
	
		cgiFormInteger("ADC",     &value, cfg.adc);
		cfg.adc = value;
		if (value == 0)	{				// GPIO
			cfg.enable[GPORT_C] |= HEX_PC00;		cfg.enable[GPORT_C] |= HEX_PC01;
			cfg.enable[GPORT_C] |= HEX_PC02;		cfg.enable[GPORT_C] |= HEX_PC03;		
		} else {
			cfg.enable[GPORT_C] &= HEX_PC00_AND;	cfg.enable[GPORT_C] &= HEX_PC01_AND;	
			cfg.enable[GPORT_C] &= HEX_PC02_AND;	cfg.enable[GPORT_C] &= HEX_PC03_AND;	}
	
		cgiFormInteger("EEPROM",     &value, cfg.spi_eeprom);
		cfg.spi_eeprom = value;
		if (value == 0)	{				// GPIO
			cfg.enable[GPORT_B] |= HEX_PB00;		cfg.enable[GPORT_B] |= HEX_PB01;
			cfg.enable[GPORT_B] |= HEX_PB02;		cfg.enable[GPORT_B] |= HEX_PB03;		
		} else {
			cfg.enable[GPORT_B] &= HEX_PB00_AND;	cfg.enable[GPORT_B] &= HEX_PB01_AND;	
			cfg.enable[GPORT_B] &= HEX_PB02_AND;	cfg.enable[GPORT_B] &= HEX_PB03_AND;	}
	
	
		cgiFormInteger("LAN",     &value, cfg.lan);
		cfg.lan = value;
		if (value == 0)	{				// GPIO
			cfg.enable[GPORT_C] |= HEX_PC12;		cfg.enable[GPORT_C] |= HEX_PC15;
		} else {
			cfg.enable[GPORT_C] &= HEX_PC12_AND;	cfg.enable[GPORT_C] &= HEX_PC15_AND;	}
	
		cgiFormInteger("RESET",     &value, cfg.reset);
		cfg.reset = value;
		if (value == 0)	{				// GPIO
			cfg.enable[GPORT_C] |= HEX_PC16;
		} else {
			cfg.enable[GPORT_C] &= HEX_PC16_AND;	}
	
		cgiFormInteger("NAND",     &value, cfg.nandflash);
		cfg.nandflash = value;
		if (value == 0)	{				// GPIO
			cfg.enable[GPORT_C] |= HEX_PC14;	cfg.enable[GPORT_C] |= HEX_PC17;
		} else {
			cfg.enable[GPORT_C] &= HEX_PC14_AND;	cfg.enable[GPORT_C] &= HEX_PC17_AND;  }
	
	
		cgiFormInteger("RDYLED",     &value, cfg.rdy_led);
		cfg.rdy_led = value;
		if (value == 0)	{				// GPIO
			cfg.enable[GPORT_C] |= HEX_PC04;
		} else {
		cfg.enable[GPORT_C] &= HEX_PC04_AND;	}
	
	
		cgiFormInteger("KEYPAD",     &value, cfg.keypad);
		cfg.keypad = value;
		if (value != 2)	{				// GPIO, Keypad
			cfg.enable[GPORT_B] |= HEX_PB20; cfg.enable[GPORT_B] |= HEX_PB21; 
			cfg.enable[GPORT_B] |= HEX_PB30; cfg.enable[GPORT_B] |= HEX_PB31;
			cfg.enable[GPORT_C] |= HEX_PC20; cfg.enable[GPORT_C] |= HEX_PC21;
			cfg.enable[GPORT_C] |= HEX_PC22; cfg.enable[GPORT_C] |= HEX_PC23;
			
			if (value == 1)  {	// eddy (keypad)
				cfg.mode[GPORT_B] &= HEX_PB20_AND; cfg.mode[GPORT_B] &= HEX_PB21_AND; 
				cfg.mode[GPORT_B] &= HEX_PB30_AND; cfg.mode[GPORT_B] &= HEX_PB31_AND;
				cfg.mode[GPORT_C] &= HEX_PC20_AND; cfg.mode[GPORT_C] &= HEX_PC21_AND;
				cfg.mode[GPORT_C] &= HEX_PC22_AND; cfg.mode[GPORT_C] &= HEX_PC23_AND;	}
		} else {
			cfg.enable[GPORT_B] &= HEX_PB20_AND; cfg.enable[GPORT_B] &= HEX_PB21_AND; 
			cfg.enable[GPORT_B] &= HEX_PB30_AND; cfg.enable[GPORT_B] &= HEX_PB31_AND;
			cfg.enable[GPORT_C] &= HEX_PC20_AND; cfg.enable[GPORT_C] &= HEX_PC21_AND;
			cfg.enable[GPORT_C] &= HEX_PC22_AND; cfg.enable[GPORT_C] &= HEX_PC23_AND;	}
		
		if (cfg.enable[GPORT_A] & HEX_PA22) gg = 1; else gg = 0;
		cgiFormInteger("PA22EN",  &value, gg);
		if (value)	cfg.enable[GPORT_A] |= HEX_PA22; else cfg.enable[GPORT_A] &= HEX_PA22_AND;
	
		if (cfg.enable[GPORT_C] & HEX_PC09) gg = 1; else gg = 0;
		cgiFormInteger("PC09EN",  &value, gg);
		if (value)	cfg.enable[GPORT_C] |= HEX_PC09; else cfg.enable[GPORT_C] &= HEX_PC09_AND;
		
		if (cfg.enable[GPORT_B] & HEX_PB12) gg = 1; else gg = 0;
		cgiFormInteger("PB12EN",  &value, gg);
		if (value)	cfg.enable[GPORT_B] |= HEX_PB12; else cfg.enable[GPORT_B] &= HEX_PB12_AND;
	
		if (cfg.enable[GPORT_B] & HEX_PB13) gg = 1; else gg = 0;
		cgiFormInteger("PB13EN",  &value, gg);
		if (value)	cfg.enable[GPORT_B] |= HEX_PB13; else cfg.enable[GPORT_B] &= HEX_PB13_AND;
	
		if (cfg.enable[GPORT_B] & HEX_PB16) gg = 1; else gg = 0;
		cgiFormInteger("PB16EN",  &value, gg);
		if (value)	cfg.enable[GPORT_B] |= HEX_PB16; else cfg.enable[GPORT_B] &= HEX_PB16_AND;
	
		if (cfg.enable[GPORT_B] & HEX_PB17) gg = 1; else gg = 0;
		cgiFormInteger("PB17EN",  &value, gg);
		if (value)	cfg.enable[GPORT_B] |= HEX_PB17; else cfg.enable[GPORT_B] &= HEX_PB17_AND;
	
		if (cfg.enable[GPORT_B] & HEX_PB18) gg = 1; else gg = 0;
		cgiFormInteger("PB18EN",  &value, gg);
		if (value)	cfg.enable[GPORT_B] |= HEX_PB18; else cfg.enable[GPORT_B] &= HEX_PB18_AND;
	
		if (cfg.enable[GPORT_B] & HEX_PB19) gg = 1; else gg = 0;
		cgiFormInteger("PB19EN",  &value, gg);
		if (value)	cfg.enable[GPORT_B] |= HEX_PB19; else cfg.enable[GPORT_B] &= HEX_PB19_AND;
	
		if (cfg.enable[GPORT_C] & HEX_PC05) gg = 1; else gg = 0;
		cgiFormInteger("PC05EN",  &value, gg);
		if (value)	cfg.enable[GPORT_C] |= HEX_PC05; else cfg.enable[GPORT_C] &= HEX_PC05_AND;
	
		if (cfg.enable[GPORT_C] & HEX_PC18) gg = 1; else gg = 0;
		cgiFormInteger("PC18EN",  &value, gg);
		if (value)	cfg.enable[GPORT_C] |= HEX_PC18; else cfg.enable[GPORT_C] &= HEX_PC18_AND;
	
		if (cfg.enable[GPORT_C] & HEX_PC19) gg = 1; else gg = 0;
		cgiFormInteger("PC19EN",  &value, gg);
		if (value)	cfg.enable[GPORT_C] |= HEX_PC19; else cfg.enable[GPORT_C] &= HEX_PC19_AND;
	
		if (cfg.enable[GPORT_C] & HEX_PC26) gg = 1; else gg = 0;
		cgiFormInteger("PC26EN",  &value, gg);
		if (value)	cfg.enable[GPORT_C] |= HEX_PC26; else cfg.enable[GPORT_C] &= HEX_PC26_AND;
	
		if (cfg.mode[GPORT_A] & HEX_PA04)
			if (cfg.value[GPORT_A] & HEX_PA04) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_A] & HEX_PA04) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PA04",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_A] &= HEX_PA04_AND; cfg.pullup[GPORT_A] |= HEX_PA04;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_A] &= HEX_PA04_AND; cfg.pullup[GPORT_A] &= HEX_PA04_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_A] |= HEX_PA04;     cfg.value [GPORT_A] &= HEX_PA04_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_A] |= HEX_PA04;     cfg.value [GPORT_A] |= HEX_PA04;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_A] & HEX_PA05)
			if (cfg.value[GPORT_A] & HEX_PA05) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_A] & HEX_PA05) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PA05",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_A] &= HEX_PA05_AND; cfg.pullup[GPORT_A] |= HEX_PA05;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_A] &= HEX_PA05_AND; cfg.pullup[GPORT_A] &= HEX_PA05_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_A] |= HEX_PA05;     cfg.value [GPORT_A] &= HEX_PA05_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_A] |= HEX_PA05;     cfg.value [GPORT_A] |= HEX_PA05;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_A] & HEX_PA22)
			if (cfg.value[GPORT_A] & HEX_PA22) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_A] & HEX_PA22) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PA22",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_A] &= HEX_PA22_AND; cfg.pullup[GPORT_A] |= HEX_PA22;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_A] &= HEX_PA22_AND; cfg.pullup[GPORT_A] &= HEX_PA22_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_A] |= HEX_PA22;     cfg.value [GPORT_A] &= HEX_PA22_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_A] |= HEX_PA22;     cfg.value [GPORT_A] |= HEX_PA22;		break;}	// mode out	 value high
	
	
		if (cfg.mode[GPORT_B] & HEX_PB00)
			if (cfg.value[GPORT_B] & HEX_PB00) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB00) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB00",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB00_AND; cfg.pullup[GPORT_B] |= HEX_PB00;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB00_AND; cfg.pullup[GPORT_B] &= HEX_PB00_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB00;     cfg.value [GPORT_B] &= HEX_PB00_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB00;     cfg.value [GPORT_B] |= HEX_PB00;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB01)
			if (cfg.value[GPORT_B] & HEX_PB01) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB01) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB01",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB01_AND; cfg.pullup[GPORT_B] |= HEX_PB01;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB01_AND; cfg.pullup[GPORT_B] &= HEX_PB01_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB01;     cfg.value [GPORT_B] &= HEX_PB01_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB01;     cfg.value [GPORT_B] |= HEX_PB01;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB02)
			if (cfg.value[GPORT_B] & HEX_PB02) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB02) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB02",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB02_AND; cfg.pullup[GPORT_B] |= HEX_PB02;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB02_AND; cfg.pullup[GPORT_B] &= HEX_PB02_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB02;     cfg.value [GPORT_B] &= HEX_PB02_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB02;     cfg.value [GPORT_B] |= HEX_PB02;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB03)
			if (cfg.value[GPORT_B] & HEX_PB03) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB03) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB03",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB03_AND; cfg.pullup[GPORT_B] |= HEX_PB03;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB03_AND; cfg.pullup[GPORT_B] &= HEX_PB03_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB03;     cfg.value [GPORT_B] &= HEX_PB03_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB03;     cfg.value [GPORT_B] |= HEX_PB03;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB04)
			if (cfg.value[GPORT_B] & HEX_PB04) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB04) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB04",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB04_AND; cfg.pullup[GPORT_B] |= HEX_PB04;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB04_AND; cfg.pullup[GPORT_B] &= HEX_PB04_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB04;     cfg.value [GPORT_B] &= HEX_PB04_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB04;     cfg.value [GPORT_B] |= HEX_PB04;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB05)
			if (cfg.value[GPORT_B] & HEX_PB05) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB05) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB05",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB05_AND; cfg.pullup[GPORT_B] |= HEX_PB05;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB05_AND; cfg.pullup[GPORT_B] &= HEX_PB05_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB05;     cfg.value [GPORT_B] &= HEX_PB05_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB05;     cfg.value [GPORT_B] |= HEX_PB05;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB06)
			if (cfg.value[GPORT_B] & HEX_PB06) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB06) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB06",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB06_AND; cfg.pullup[GPORT_B] |= HEX_PB06;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB06_AND; cfg.pullup[GPORT_B] &= HEX_PB06_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB06;     cfg.value [GPORT_B] &= HEX_PB06_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB06;     cfg.value [GPORT_B] |= HEX_PB06;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB07)
			if (cfg.value[GPORT_B] & HEX_PB07) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB07) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB07",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB07_AND; cfg.pullup[GPORT_B] |= HEX_PB07;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB07_AND; cfg.pullup[GPORT_B] &= HEX_PB07_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB07;     cfg.value [GPORT_B] &= HEX_PB07_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB07;     cfg.value [GPORT_B] |= HEX_PB07;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB08)
			if (cfg.value[GPORT_B] & HEX_PB08) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB08) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB08",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB08_AND; cfg.pullup[GPORT_B] |= HEX_PB08;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB08_AND; cfg.pullup[GPORT_B] &= HEX_PB08_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB08;     cfg.value [GPORT_B] &= HEX_PB08_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB08;     cfg.value [GPORT_B] |= HEX_PB08;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB09)
			if (cfg.value[GPORT_B] & HEX_PB09) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB09) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB09",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB09_AND; cfg.pullup[GPORT_B] |= HEX_PB09;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB09_AND; cfg.pullup[GPORT_B] &= HEX_PB09_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB09;     cfg.value [GPORT_B] &= HEX_PB09_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB09;     cfg.value [GPORT_B] |= HEX_PB09;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB10)
			if (cfg.value[GPORT_B] & HEX_PB10) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB10) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB10",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB10_AND; cfg.pullup[GPORT_B] |= HEX_PB10;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB10_AND; cfg.pullup[GPORT_B] &= HEX_PB10_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB10;     cfg.value [GPORT_B] &= HEX_PB10_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB10;     cfg.value [GPORT_B] |= HEX_PB10;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB11)
			if (cfg.value[GPORT_B] & HEX_PB11) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB11) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB11",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB11_AND; cfg.pullup[GPORT_B] |= HEX_PB11;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB11_AND; cfg.pullup[GPORT_B] &= HEX_PB11_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB11;     cfg.value [GPORT_B] &= HEX_PB11_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB11;     cfg.value [GPORT_B] |= HEX_PB11;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB12)
			if (cfg.value[GPORT_B] & HEX_PB12) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB12) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB12",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB12_AND; cfg.pullup[GPORT_B] |= HEX_PB12;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB12_AND; cfg.pullup[GPORT_B] &= HEX_PB12_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB12;     cfg.value [GPORT_B] &= HEX_PB12_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB12;     cfg.value [GPORT_B] |= HEX_PB12;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB13)
			if (cfg.value[GPORT_B] & HEX_PB13) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB13) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB13",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB13_AND; cfg.pullup[GPORT_B] |= HEX_PB13;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB13_AND; cfg.pullup[GPORT_B] &= HEX_PB13_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB13;     cfg.value [GPORT_B] &= HEX_PB13_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB13;     cfg.value [GPORT_B] |= HEX_PB13;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB14)
			if (cfg.value[GPORT_B] & HEX_PB14) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB14) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB14",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB14_AND; cfg.pullup[GPORT_B] |= HEX_PB14;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB14_AND; cfg.pullup[GPORT_B] &= HEX_PB14_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB14;     cfg.value [GPORT_B] &= HEX_PB14_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB14;     cfg.value [GPORT_B] |= HEX_PB14;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB15)
			if (cfg.value[GPORT_B] & HEX_PB15) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB15) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB15",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB15_AND; cfg.pullup[GPORT_B] |= HEX_PB15;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB15_AND; cfg.pullup[GPORT_B] &= HEX_PB15_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB15;     cfg.value [GPORT_B] &= HEX_PB15_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB15;     cfg.value [GPORT_B] |= HEX_PB15;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB16)
			if (cfg.value[GPORT_B] & HEX_PB16) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB16) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB16",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB16_AND; cfg.pullup[GPORT_B] |= HEX_PB16;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB16_AND; cfg.pullup[GPORT_B] &= HEX_PB16_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB16;     cfg.value [GPORT_B] &= HEX_PB16_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB16;     cfg.value [GPORT_B] |= HEX_PB16;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB17)
			if (cfg.value[GPORT_B] & HEX_PB17) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB17) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB17",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB17_AND; cfg.pullup[GPORT_B] |= HEX_PB17;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB17_AND; cfg.pullup[GPORT_B] &= HEX_PB17_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB17;     cfg.value [GPORT_B] &= HEX_PB17_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB17;     cfg.value [GPORT_B] |= HEX_PB17;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB18)
			if (cfg.value[GPORT_B] & HEX_PB18) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB18) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB18",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB18_AND; cfg.pullup[GPORT_B] |= HEX_PB18;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB18_AND; cfg.pullup[GPORT_B] &= HEX_PB18_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB18;     cfg.value [GPORT_B] &= HEX_PB18_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB18;     cfg.value [GPORT_B] |= HEX_PB18;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB19)
			if (cfg.value[GPORT_B] & HEX_PB19) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB19) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB19",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB19_AND; cfg.pullup[GPORT_B] |= HEX_PB19;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB19_AND; cfg.pullup[GPORT_B] &= HEX_PB19_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB19;     cfg.value [GPORT_B] &= HEX_PB19_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB19;     cfg.value [GPORT_B] |= HEX_PB19;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB20)
			if (cfg.value[GPORT_B] & HEX_PB20) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB20) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB20",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB20_AND; cfg.pullup[GPORT_B] |= HEX_PB20;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB20_AND; cfg.pullup[GPORT_B] &= HEX_PB20_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB20;     cfg.value [GPORT_B] &= HEX_PB20_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB20;     cfg.value [GPORT_B] |= HEX_PB20;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB21)
			if (cfg.value[GPORT_B] & HEX_PB21) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB21) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB21",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB21_AND; cfg.pullup[GPORT_B] |= HEX_PB21;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB21_AND; cfg.pullup[GPORT_B] &= HEX_PB21_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB21;     cfg.value [GPORT_B] &= HEX_PB21_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB21;     cfg.value [GPORT_B] |= HEX_PB21;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB22)
			if (cfg.value[GPORT_B] & HEX_PB22) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB22) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB22",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB22_AND; cfg.pullup[GPORT_B] |= HEX_PB22;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB22_AND; cfg.pullup[GPORT_B] &= HEX_PB22_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB22;     cfg.value [GPORT_B] &= HEX_PB22_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB22;     cfg.value [GPORT_B] |= HEX_PB22;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB23)
			if (cfg.value[GPORT_B] & HEX_PB23) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB23) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB23",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB23_AND; cfg.pullup[GPORT_B] |= HEX_PB23;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB23_AND; cfg.pullup[GPORT_B] &= HEX_PB23_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB23;     cfg.value [GPORT_B] &= HEX_PB23_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB23;     cfg.value [GPORT_B] |= HEX_PB23;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB24)
			if (cfg.value[GPORT_B] & HEX_PB24) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB24) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB24",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB24_AND; cfg.pullup[GPORT_B] |= HEX_PB24;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB24_AND; cfg.pullup[GPORT_B] &= HEX_PB24_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB24;     cfg.value [GPORT_B] &= HEX_PB24_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB24;     cfg.value [GPORT_B] |= HEX_PB24;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB25)
			if (cfg.value[GPORT_B] & HEX_PB25) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB25) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB25",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB25_AND; cfg.pullup[GPORT_B] |= HEX_PB25;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB25_AND; cfg.pullup[GPORT_B] &= HEX_PB25_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB25;     cfg.value [GPORT_B] &= HEX_PB25_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB25;     cfg.value [GPORT_B] |= HEX_PB25;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB26)
			if (cfg.value[GPORT_B] & HEX_PB26) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB26) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB26",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB26_AND; cfg.pullup[GPORT_B] |= HEX_PB26;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB26_AND; cfg.pullup[GPORT_B] &= HEX_PB26_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB26;     cfg.value [GPORT_B] &= HEX_PB26_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB26;     cfg.value [GPORT_B] |= HEX_PB26;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB27)
			if (cfg.value[GPORT_B] & HEX_PB27) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB27) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB27",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB27_AND; cfg.pullup[GPORT_B] |= HEX_PB27;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB27_AND; cfg.pullup[GPORT_B] &= HEX_PB27_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB27;     cfg.value [GPORT_B] &= HEX_PB27_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB27;     cfg.value [GPORT_B] |= HEX_PB27;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB28)
			if (cfg.value[GPORT_B] & HEX_PB28) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB28) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB28",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB28_AND; cfg.pullup[GPORT_B] |= HEX_PB28;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB28_AND; cfg.pullup[GPORT_B] &= HEX_PB28_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB28;     cfg.value [GPORT_B] &= HEX_PB28_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB28;     cfg.value [GPORT_B] |= HEX_PB28;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB29)
			if (cfg.value[GPORT_B] & HEX_PB29) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB29) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB29",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB29_AND; cfg.pullup[GPORT_B] |= HEX_PB29;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB29_AND; cfg.pullup[GPORT_B] &= HEX_PB29_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB29;     cfg.value [GPORT_B] &= HEX_PB29_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB29;     cfg.value [GPORT_B] |= HEX_PB29;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB30)
			if (cfg.value[GPORT_B] & HEX_PB30) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB30) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB30",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB30_AND; cfg.pullup[GPORT_B] |= HEX_PB30;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB30_AND; cfg.pullup[GPORT_B] &= HEX_PB30_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB30;     cfg.value [GPORT_B] &= HEX_PB30_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB30;     cfg.value [GPORT_B] |= HEX_PB30;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB31)
			if (cfg.value[GPORT_B] & HEX_PB31) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB31) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB31",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB31_AND; cfg.pullup[GPORT_B] |= HEX_PB31;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB31_AND; cfg.pullup[GPORT_B] &= HEX_PB31_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB31;     cfg.value [GPORT_B] &= HEX_PB31_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB31;     cfg.value [GPORT_B] |= HEX_PB31;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC00)
			if (cfg.value[GPORT_C] & HEX_PC00) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC00) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC00",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC00_AND; cfg.pullup[GPORT_C] |= HEX_PC00;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC00_AND; cfg.pullup[GPORT_C] &= HEX_PC00_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC00;     cfg.value [GPORT_C] &= HEX_PC00_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC00;     cfg.value [GPORT_C] |= HEX_PC00;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC01)
			if (cfg.value[GPORT_C] & HEX_PC01) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC01) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC01",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC01_AND; cfg.pullup[GPORT_C] |= HEX_PC01;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC01_AND; cfg.pullup[GPORT_C] &= HEX_PC01_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC01;     cfg.value [GPORT_C] &= HEX_PC01_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC01;     cfg.value [GPORT_C] |= HEX_PC01;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC02)
			if (cfg.value[GPORT_C] & HEX_PC02) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC02) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC02",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC02_AND; cfg.pullup[GPORT_C] |= HEX_PC02;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC02_AND; cfg.pullup[GPORT_C] &= HEX_PC02_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC02;     cfg.value [GPORT_C] &= HEX_PC02_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC02;     cfg.value [GPORT_C] |= HEX_PC02;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC03)
			if (cfg.value[GPORT_C] & HEX_PC03) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC03) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC03",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC03_AND; cfg.pullup[GPORT_C] |= HEX_PC03;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC03_AND; cfg.pullup[GPORT_C] &= HEX_PC03_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC03;     cfg.value [GPORT_C] &= HEX_PC03_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC03;     cfg.value [GPORT_C] |= HEX_PC03;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC04)
			if (cfg.value[GPORT_C] & HEX_PC04) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC04) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC04",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC04_AND; cfg.pullup[GPORT_C] |= HEX_PC04;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC04_AND; cfg.pullup[GPORT_C] &= HEX_PC04_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC04;     cfg.value [GPORT_C] &= HEX_PC04_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC04;     cfg.value [GPORT_C] |= HEX_PC04;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC05)
			if (cfg.value[GPORT_C] & HEX_PC05) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC05) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC05",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC05_AND; cfg.pullup[GPORT_C] |= HEX_PC05;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC05_AND; cfg.pullup[GPORT_C] &= HEX_PC05_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC05;     cfg.value [GPORT_C] &= HEX_PC05_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC05;     cfg.value [GPORT_C] |= HEX_PC05;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC08)
			if (cfg.value[GPORT_C] & HEX_PC08) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC08) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC08",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC08_AND; cfg.pullup[GPORT_C] |= HEX_PC08;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC08_AND; cfg.pullup[GPORT_C] &= HEX_PC08_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC08;     cfg.value [GPORT_C] &= HEX_PC08_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC08;     cfg.value [GPORT_C] |= HEX_PC08;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC09)
			if (cfg.value[GPORT_C] & HEX_PC09) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC09) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC09",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC09_AND; cfg.pullup[GPORT_C] |= HEX_PC09;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC09_AND; cfg.pullup[GPORT_C] &= HEX_PC09_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC09;     cfg.value [GPORT_C] &= HEX_PC09_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC09;     cfg.value [GPORT_C] |= HEX_PC09;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC10)
			if (cfg.value[GPORT_C] & HEX_PC10) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC10) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC10",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC10_AND; cfg.pullup[GPORT_C] |= HEX_PC10;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC10_AND; cfg.pullup[GPORT_C] &= HEX_PC10_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC10;     cfg.value [GPORT_C] &= HEX_PC10_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC10;     cfg.value [GPORT_C] |= HEX_PC10;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC12)
			if (cfg.value[GPORT_C] & HEX_PC12) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC12) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC12",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC12_AND; cfg.pullup[GPORT_C] |= HEX_PC12;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC12_AND; cfg.pullup[GPORT_C] &= HEX_PC12_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC12;     cfg.value [GPORT_C] &= HEX_PC12_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC12;     cfg.value [GPORT_C] |= HEX_PC12;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC14)
			if (cfg.value[GPORT_C] & HEX_PC14) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC14) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC14",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC14_AND; cfg.pullup[GPORT_C] |= HEX_PC14;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC14_AND; cfg.pullup[GPORT_C] &= HEX_PC14_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC14;     cfg.value [GPORT_C] &= HEX_PC14_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC14;     cfg.value [GPORT_C] |= HEX_PC14;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC15)
			if (cfg.value[GPORT_C] & HEX_PC15) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC15) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC15",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC15_AND; cfg.pullup[GPORT_C] |= HEX_PC15;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC15_AND; cfg.pullup[GPORT_C] &= HEX_PC15_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC15;     cfg.value [GPORT_C] &= HEX_PC15_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC15;     cfg.value [GPORT_C] |= HEX_PC15;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC16)
			if (cfg.value[GPORT_C] & HEX_PC16) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC16) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC16",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC16_AND; cfg.pullup[GPORT_C] |= HEX_PC16;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC16_AND; cfg.pullup[GPORT_C] &= HEX_PC16_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC16;     cfg.value [GPORT_C] &= HEX_PC16_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC16;     cfg.value [GPORT_C] |= HEX_PC16;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC17)
			if (cfg.value[GPORT_C] & HEX_PC17) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC17) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC17",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC17_AND; cfg.pullup[GPORT_C] |= HEX_PC17;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC17_AND; cfg.pullup[GPORT_C] &= HEX_PC17_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC17;     cfg.value [GPORT_C] &= HEX_PC17_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC17;     cfg.value [GPORT_C] |= HEX_PC17;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC18)
			if (cfg.value[GPORT_C] & HEX_PC18) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC18) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC18",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC18_AND; cfg.pullup[GPORT_C] |= HEX_PC18;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC18_AND; cfg.pullup[GPORT_C] &= HEX_PC18_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC18;     cfg.value [GPORT_C] &= HEX_PC18_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC18;     cfg.value [GPORT_C] |= HEX_PC18;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC19)
			if (cfg.value[GPORT_C] & HEX_PC19) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC19) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC19",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC19_AND; cfg.pullup[GPORT_C] |= HEX_PC19;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC19_AND; cfg.pullup[GPORT_C] &= HEX_PC19_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC19;     cfg.value [GPORT_C] &= HEX_PC19_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC19;     cfg.value [GPORT_C] |= HEX_PC19;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC20)
			if (cfg.value[GPORT_C] & HEX_PC20) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC20) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC20",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC20_AND; cfg.pullup[GPORT_C] |= HEX_PC20;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC20_AND; cfg.pullup[GPORT_C] &= HEX_PC20_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC20;     cfg.value [GPORT_C] &= HEX_PC20_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC20;     cfg.value [GPORT_C] |= HEX_PC20;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC21)
			if (cfg.value[GPORT_C] & HEX_PC21) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC21) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC21",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC21_AND; cfg.pullup[GPORT_C] |= HEX_PC21;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC21_AND; cfg.pullup[GPORT_C] &= HEX_PC21_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC21;     cfg.value [GPORT_C] &= HEX_PC21_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC21;     cfg.value [GPORT_C] |= HEX_PC21;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC22)
			if (cfg.value[GPORT_C] & HEX_PC22) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC22) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC22",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC22_AND; cfg.pullup[GPORT_C] |= HEX_PC22;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC22_AND; cfg.pullup[GPORT_C] &= HEX_PC22_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC22;     cfg.value [GPORT_C] &= HEX_PC22_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC22;     cfg.value [GPORT_C] |= HEX_PC22;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC23)
			if (cfg.value[GPORT_C] & HEX_PC23) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC23) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC23",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC23_AND; cfg.pullup[GPORT_C] |= HEX_PC23;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC23_AND; cfg.pullup[GPORT_C] &= HEX_PC23_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC23;     cfg.value [GPORT_C] &= HEX_PC23_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC23;     cfg.value [GPORT_C] |= HEX_PC23;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC26)
			if (cfg.value[GPORT_C] & HEX_PC26) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC26) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC26",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC26_AND; cfg.pullup[GPORT_C] |= HEX_PC26;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC26_AND; cfg.pullup[GPORT_C] &= HEX_PC26_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC26;     cfg.value [GPORT_C] &= HEX_PC26_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC26;     cfg.value [GPORT_C] |= HEX_PC26;		break;}	// mode out	 value high
		}
	else	// S4M
		{
		cfg.serial_1 = SB_ENABLE;
		cfg.serial_2 = SB_ENABLE;				
		cfg.serial_3 = SB_ENABLE;
		cfg.serial_4 = SB_ENABLE;				
		cfg.debugport = SB_ENABLE;				
		cfg.lan			= 	SB_DISABLE;
		cfg.reset		=	SB_ENABLE;
		cfg.rdy_led		=	SB_ENABLE;
		cfg.spi_eeprom	=	SB_DISABLE;
		cfg.nandflash	=	SB_DISABLE;
					
		cgiFormInteger("ADC",     &value, cfg.adc);
		cfg.adc = value;
		if (value == 0)	{				// GPIO
			cfg.enable[GPORT_C] |= HEX_PC00;		cfg.enable[GPORT_C] |= HEX_PC01;
			cfg.enable[GPORT_C] |= HEX_PC02;		cfg.enable[GPORT_C] |= HEX_PC03;		
		} else {
			cfg.enable[GPORT_C] &= HEX_PC00_AND;	cfg.enable[GPORT_C] &= HEX_PC01_AND;	
			cfg.enable[GPORT_C] &= HEX_PC02_AND;	cfg.enable[GPORT_C] &= HEX_PC03_AND;	}
	
		if (cfg.enable[GPORT_A] & HEX_PA05) gg = 1; else gg = 0;
		cgiFormInteger("PA05EN",  &value, gg);
		if (value)	cfg.enable[GPORT_A] |= HEX_PA05; else cfg.enable[GPORT_A] &= HEX_PA05_AND;

		if (cfg.enable[GPORT_A] & HEX_PA22) gg = 1; else gg = 0;
		cgiFormInteger("PA22EN",  &value, gg);
		if (value)	cfg.enable[GPORT_A] |= HEX_PA22; else cfg.enable[GPORT_A] &= HEX_PA22_AND;

		if (cfg.enable[GPORT_A] & HEX_PA30) gg = 1; else gg = 0;
		cgiFormInteger("PA30EN",  &value, gg);
		if (value)	cfg.enable[GPORT_A] |= HEX_PA30; else cfg.enable[GPORT_A] &= HEX_PA30_AND;
	
		if (cfg.enable[GPORT_B] & HEX_PB00) gg = 1; else gg = 0;
		cgiFormInteger("PB00EN",  &value, gg);
		if (value)	cfg.enable[GPORT_B] |= HEX_PB00; else cfg.enable[GPORT_B] &= HEX_PB00_AND;

		if (cfg.enable[GPORT_B] & HEX_PB01) gg = 1; else gg = 0;
		cgiFormInteger("PB01EN",  &value, gg);
		if (value)	cfg.enable[GPORT_B] |= HEX_PB01; else cfg.enable[GPORT_B] &= HEX_PB01_AND;

		if (cfg.enable[GPORT_B] & HEX_PB02) gg = 1; else gg = 0;
		cgiFormInteger("PB02EN",  &value, gg);
		if (value)	cfg.enable[GPORT_B] |= HEX_PB02; else cfg.enable[GPORT_B] &= HEX_PB02_AND;

		if (cfg.enable[GPORT_B] & HEX_PB03) gg = 1; else gg = 0;
		cgiFormInteger("PB03EN",  &value, gg);
		if (value)	cfg.enable[GPORT_B] |= HEX_PB03; else cfg.enable[GPORT_B] &= HEX_PB03_AND;

		if (cfg.enable[GPORT_B] & HEX_PB12) gg = 1; else gg = 0;
		cgiFormInteger("PB12EN",  &value, gg);
		if (value)	cfg.enable[GPORT_B] |= HEX_PB12; else cfg.enable[GPORT_B] &= HEX_PB12_AND;

		if (cfg.enable[GPORT_B] & HEX_PB13) gg = 1; else gg = 0;
		cgiFormInteger("PB13EN",  &value, gg);
		if (value)	cfg.enable[GPORT_B] |= HEX_PB13; else cfg.enable[GPORT_B] &= HEX_PB13_AND;
	
		if (cfg.enable[GPORT_B] & HEX_PB16) gg = 1; else gg = 0;
		cgiFormInteger("PB16EN",  &value, gg);
		if (value)	cfg.enable[GPORT_B] |= HEX_PB16; else cfg.enable[GPORT_B] &= HEX_PB16_AND;
	
		if (cfg.enable[GPORT_B] & HEX_PB17) gg = 1; else gg = 0;
		cgiFormInteger("PB17EN",  &value, gg);
		if (value)	cfg.enable[GPORT_B] |= HEX_PB17; else cfg.enable[GPORT_B] &= HEX_PB17_AND;
	
		if (cfg.enable[GPORT_B] & HEX_PB18) gg = 1; else gg = 0;
		cgiFormInteger("PB18EN",  &value, gg);
		if (value)	cfg.enable[GPORT_B] |= HEX_PB18; else cfg.enable[GPORT_B] &= HEX_PB18_AND;
	
		if (cfg.enable[GPORT_B] & HEX_PB19) gg = 1; else gg = 0;
		cgiFormInteger("PB19EN",  &value, gg);
		if (value)	cfg.enable[GPORT_B] |= HEX_PB19; else cfg.enable[GPORT_B] &= HEX_PB19_AND;

		if (cfg.enable[GPORT_B] & HEX_PB20) gg = 1; else gg = 0;
		cgiFormInteger("PB20EN",  &value, gg);
		if (value)	cfg.enable[GPORT_B] |= HEX_PB20; else cfg.enable[GPORT_B] &= HEX_PB20_AND;

		if (cfg.enable[GPORT_B] & HEX_PB21) gg = 1; else gg = 0;
		cgiFormInteger("PB21EN",  &value, gg);
		if (value)	cfg.enable[GPORT_B] |= HEX_PB21; else cfg.enable[GPORT_B] &= HEX_PB21_AND;

		if (cfg.enable[GPORT_B] & HEX_PB30) gg = 1; else gg = 0;
		cgiFormInteger("PB30EN",  &value, gg);
		if (value)	cfg.enable[GPORT_B] |= HEX_PB30; else cfg.enable[GPORT_B] &= HEX_PB30_AND;

		if (cfg.enable[GPORT_B] & HEX_PB31) gg = 1; else gg = 0;
		cgiFormInteger("PB31EN",  &value, gg);
		if (value)	cfg.enable[GPORT_B] |= HEX_PB31; else cfg.enable[GPORT_B] &= HEX_PB31_AND;

		if (cfg.enable[GPORT_C] & HEX_PC05) gg = 1; else gg = 0;
		cgiFormInteger("PC05EN",  &value, gg);
		if (value)	cfg.enable[GPORT_C] |= HEX_PC05; else cfg.enable[GPORT_C] &= HEX_PC05_AND;
	
		if (cfg.enable[GPORT_C] & HEX_PC09) gg = 1; else gg = 0;
		cgiFormInteger("PC09EN",  &value, gg);
		if (value)	cfg.enable[GPORT_C] |= HEX_PC09; else cfg.enable[GPORT_C] &= HEX_PC09_AND;

		if (cfg.enable[GPORT_C] & HEX_PC10) gg = 1; else gg = 0;
		cgiFormInteger("PC10EN",  &value, gg);
		if (value)	cfg.enable[GPORT_C] |= HEX_PC10; else cfg.enable[GPORT_C] &= HEX_PC10_AND;

		if (cfg.enable[GPORT_C] & HEX_PC12) gg = 1; else gg = 0;
		cgiFormInteger("PC12EN",  &value, gg);
		if (value)	cfg.enable[GPORT_C] |= HEX_PC12; else cfg.enable[GPORT_C] &= HEX_PC12_AND;

		if (cfg.enable[GPORT_C] & HEX_PC13) gg = 1; else gg = 0;
		cgiFormInteger("PC13EN",  &value, gg);
		if (value)	cfg.enable[GPORT_C] |= HEX_PC13; else cfg.enable[GPORT_C] &= HEX_PC13_AND;

		if (cfg.enable[GPORT_C] & HEX_PC14) gg = 1; else gg = 0;
		cgiFormInteger("PC14EN",  &value, gg);
		if (value)	cfg.enable[GPORT_C] |= HEX_PC14; else cfg.enable[GPORT_C] &= HEX_PC14_AND;

		if (cfg.enable[GPORT_C] & HEX_PC15) gg = 1; else gg = 0;
		cgiFormInteger("PC15EN",  &value, gg);
		if (value)	cfg.enable[GPORT_C] |= HEX_PC15; else cfg.enable[GPORT_C] &= HEX_PC15_AND;

		if (cfg.enable[GPORT_C] & HEX_PC17) gg = 1; else gg = 0;
		cgiFormInteger("PC17EN",  &value, gg);
		if (value)	cfg.enable[GPORT_C] |= HEX_PC17; else cfg.enable[GPORT_C] &= HEX_PC17_AND;

		if (cfg.enable[GPORT_C] & HEX_PC18) gg = 1; else gg = 0;
		cgiFormInteger("PC18EN",  &value, gg);
		if (value)	cfg.enable[GPORT_C] |= HEX_PC18; else cfg.enable[GPORT_C] &= HEX_PC18_AND;
	
		if (cfg.enable[GPORT_C] & HEX_PC19) gg = 1; else gg = 0;
		cgiFormInteger("PC19EN",  &value, gg);
		if (value)	cfg.enable[GPORT_C] |= HEX_PC19; else cfg.enable[GPORT_C] &= HEX_PC19_AND;
	
		if (cfg.enable[GPORT_C] & HEX_PC20) gg = 1; else gg = 0;
		cgiFormInteger("PC20EN",  &value, gg);
		if (value)	cfg.enable[GPORT_C] |= HEX_PC20; else cfg.enable[GPORT_C] &= HEX_PC20_AND;
	
		if (cfg.enable[GPORT_C] & HEX_PC24) gg = 1; else gg = 0;
		cgiFormInteger("PC24EN",  &value, gg);
		if (value)	cfg.enable[GPORT_C] |= HEX_PC24; else cfg.enable[GPORT_C] &= HEX_PC24_AND;
	
		if (cfg.enable[GPORT_C] & HEX_PC25) gg = 1; else gg = 0;
		cgiFormInteger("PC25EN",  &value, gg);
		if (value)	cfg.enable[GPORT_C] |= HEX_PC25; else cfg.enable[GPORT_C] &= HEX_PC25_AND;
		


		if (cfg.mode[GPORT_A] & HEX_PA05)
			if (cfg.value[GPORT_A] & HEX_PA05) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_A] & HEX_PA05) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PA05",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_A] &= HEX_PA05_AND; cfg.pullup[GPORT_A] |= HEX_PA05;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_A] &= HEX_PA05_AND; cfg.pullup[GPORT_A] &= HEX_PA05_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_A] |= HEX_PA05;     cfg.value [GPORT_A] &= HEX_PA05_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_A] |= HEX_PA05;     cfg.value [GPORT_A] |= HEX_PA05;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_A] & HEX_PA22)
			if (cfg.value[GPORT_A] & HEX_PA22) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_A] & HEX_PA22) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PA22",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_A] &= HEX_PA22_AND; cfg.pullup[GPORT_A] |= HEX_PA22;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_A] &= HEX_PA22_AND; cfg.pullup[GPORT_A] &= HEX_PA22_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_A] |= HEX_PA22;     cfg.value [GPORT_A] &= HEX_PA22_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_A] |= HEX_PA22;     cfg.value [GPORT_A] |= HEX_PA22;		break;}	// mode out	 value high

		if (cfg.mode[GPORT_A] & HEX_PA30)
			if (cfg.value[GPORT_A] & HEX_PA30) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_A] & HEX_PA30) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PA30",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_A] &= HEX_PA30_AND; cfg.pullup[GPORT_A] |= HEX_PA30;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_A] &= HEX_PA30_AND; cfg.pullup[GPORT_A] &= HEX_PA30_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_A] |= HEX_PA30;     cfg.value [GPORT_A] &= HEX_PA30_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_A] |= HEX_PA30;     cfg.value [GPORT_A] |= HEX_PA30;		break;}	// mode out	 value high	
	
		if (cfg.mode[GPORT_B] & HEX_PB00)
			if (cfg.value[GPORT_B] & HEX_PB00) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB00) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB00",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB00_AND; cfg.pullup[GPORT_B] |= HEX_PB00;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB00_AND; cfg.pullup[GPORT_B] &= HEX_PB00_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB00;     cfg.value [GPORT_B] &= HEX_PB00_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB00;     cfg.value [GPORT_B] |= HEX_PB00;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB01)
			if (cfg.value[GPORT_B] & HEX_PB01) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB01) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB01",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB01_AND; cfg.pullup[GPORT_B] |= HEX_PB01;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB01_AND; cfg.pullup[GPORT_B] &= HEX_PB01_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB01;     cfg.value [GPORT_B] &= HEX_PB01_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB01;     cfg.value [GPORT_B] |= HEX_PB01;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB02)
			if (cfg.value[GPORT_B] & HEX_PB02) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB02) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB02",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB02_AND; cfg.pullup[GPORT_B] |= HEX_PB02;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB02_AND; cfg.pullup[GPORT_B] &= HEX_PB02_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB02;     cfg.value [GPORT_B] &= HEX_PB02_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB02;     cfg.value [GPORT_B] |= HEX_PB02;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB03)
			if (cfg.value[GPORT_B] & HEX_PB03) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB03) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB03",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB03_AND; cfg.pullup[GPORT_B] |= HEX_PB03;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB03_AND; cfg.pullup[GPORT_B] &= HEX_PB03_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB03;     cfg.value [GPORT_B] &= HEX_PB03_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB03;     cfg.value [GPORT_B] |= HEX_PB03;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB12)
			if (cfg.value[GPORT_B] & HEX_PB12) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB12) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB12",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB12_AND; cfg.pullup[GPORT_B] |= HEX_PB12;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB12_AND; cfg.pullup[GPORT_B] &= HEX_PB12_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB12;     cfg.value [GPORT_B] &= HEX_PB12_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB12;     cfg.value [GPORT_B] |= HEX_PB12;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB13)
			if (cfg.value[GPORT_B] & HEX_PB13) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB13) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB13",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB13_AND; cfg.pullup[GPORT_B] |= HEX_PB13;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB13_AND; cfg.pullup[GPORT_B] &= HEX_PB13_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB13;     cfg.value [GPORT_B] &= HEX_PB13_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB13;     cfg.value [GPORT_B] |= HEX_PB13;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB16)
			if (cfg.value[GPORT_B] & HEX_PB16) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB16) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB16",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB16_AND; cfg.pullup[GPORT_B] |= HEX_PB16;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB16_AND; cfg.pullup[GPORT_B] &= HEX_PB16_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB16;     cfg.value [GPORT_B] &= HEX_PB16_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB16;     cfg.value [GPORT_B] |= HEX_PB16;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB17)
			if (cfg.value[GPORT_B] & HEX_PB17) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB17) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB17",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB17_AND; cfg.pullup[GPORT_B] |= HEX_PB17;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB17_AND; cfg.pullup[GPORT_B] &= HEX_PB17_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB17;     cfg.value [GPORT_B] &= HEX_PB17_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB17;     cfg.value [GPORT_B] |= HEX_PB17;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB18)
			if (cfg.value[GPORT_B] & HEX_PB18) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB18) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB18",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB18_AND; cfg.pullup[GPORT_B] |= HEX_PB18;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB18_AND; cfg.pullup[GPORT_B] &= HEX_PB18_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB18;     cfg.value [GPORT_B] &= HEX_PB18_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB18;     cfg.value [GPORT_B] |= HEX_PB18;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB19)
			if (cfg.value[GPORT_B] & HEX_PB19) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB19) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB19",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB19_AND; cfg.pullup[GPORT_B] |= HEX_PB19;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB19_AND; cfg.pullup[GPORT_B] &= HEX_PB19_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB19;     cfg.value [GPORT_B] &= HEX_PB19_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB19;     cfg.value [GPORT_B] |= HEX_PB19;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB20)
			if (cfg.value[GPORT_B] & HEX_PB20) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB20) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB20",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB20_AND; cfg.pullup[GPORT_B] |= HEX_PB20;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB20_AND; cfg.pullup[GPORT_B] &= HEX_PB20_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB20;     cfg.value [GPORT_B] &= HEX_PB20_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB20;     cfg.value [GPORT_B] |= HEX_PB20;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB21)
			if (cfg.value[GPORT_B] & HEX_PB21) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB21) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB21",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB21_AND; cfg.pullup[GPORT_B] |= HEX_PB21;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB21_AND; cfg.pullup[GPORT_B] &= HEX_PB21_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB21;     cfg.value [GPORT_B] &= HEX_PB21_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB21;     cfg.value [GPORT_B] |= HEX_PB21;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB30)
			if (cfg.value[GPORT_B] & HEX_PB30) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB30) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB30",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB30_AND; cfg.pullup[GPORT_B] |= HEX_PB30;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB30_AND; cfg.pullup[GPORT_B] &= HEX_PB30_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB30;     cfg.value [GPORT_B] &= HEX_PB30_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB30;     cfg.value [GPORT_B] |= HEX_PB30;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_B] & HEX_PB31)
			if (cfg.value[GPORT_B] & HEX_PB31) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_B] & HEX_PB31) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PB31",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_B] &= HEX_PB31_AND; cfg.pullup[GPORT_B] |= HEX_PB31;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_B] &= HEX_PB31_AND; cfg.pullup[GPORT_B] &= HEX_PB31_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_B] |= HEX_PB31;     cfg.value [GPORT_B] &= HEX_PB31_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_B] |= HEX_PB31;     cfg.value [GPORT_B] |= HEX_PB31;		break;}	// mode out	 value high

		if (cfg.mode[GPORT_C] & HEX_PC00)
			if (cfg.value[GPORT_C] & HEX_PC00) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC00) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC00",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC00_AND; cfg.pullup[GPORT_C] |= HEX_PC00;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC00_AND; cfg.pullup[GPORT_C] &= HEX_PC00_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC00;     cfg.value [GPORT_C] &= HEX_PC00_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC00;     cfg.value [GPORT_C] |= HEX_PC00;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC01)
			if (cfg.value[GPORT_C] & HEX_PC01) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC01) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC01",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC01_AND; cfg.pullup[GPORT_C] |= HEX_PC01;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC01_AND; cfg.pullup[GPORT_C] &= HEX_PC01_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC01;     cfg.value [GPORT_C] &= HEX_PC01_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC01;     cfg.value [GPORT_C] |= HEX_PC01;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC02)
			if (cfg.value[GPORT_C] & HEX_PC02) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC02) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC02",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC02_AND; cfg.pullup[GPORT_C] |= HEX_PC02;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC02_AND; cfg.pullup[GPORT_C] &= HEX_PC02_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC02;     cfg.value [GPORT_C] &= HEX_PC02_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC02;     cfg.value [GPORT_C] |= HEX_PC02;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC03)
			if (cfg.value[GPORT_C] & HEX_PC03) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC03) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC03",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC03_AND; cfg.pullup[GPORT_C] |= HEX_PC03;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC03_AND; cfg.pullup[GPORT_C] &= HEX_PC03_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC03;     cfg.value [GPORT_C] &= HEX_PC03_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC03;     cfg.value [GPORT_C] |= HEX_PC03;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC05)
			if (cfg.value[GPORT_C] & HEX_PC05) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC05) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC05",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC05_AND; cfg.pullup[GPORT_C] |= HEX_PC05;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC05_AND; cfg.pullup[GPORT_C] &= HEX_PC05_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC05;     cfg.value [GPORT_C] &= HEX_PC05_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC05;     cfg.value [GPORT_C] |= HEX_PC05;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC09)
			if (cfg.value[GPORT_C] & HEX_PC09) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC09) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC09",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC09_AND; cfg.pullup[GPORT_C] |= HEX_PC09;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC09_AND; cfg.pullup[GPORT_C] &= HEX_PC09_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC09;     cfg.value [GPORT_C] &= HEX_PC09_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC09;     cfg.value [GPORT_C] |= HEX_PC09;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC10)
			if (cfg.value[GPORT_C] & HEX_PC10) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC10) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC10",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC10_AND; cfg.pullup[GPORT_C] |= HEX_PC10;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC10_AND; cfg.pullup[GPORT_C] &= HEX_PC10_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC10;     cfg.value [GPORT_C] &= HEX_PC10_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC10;     cfg.value [GPORT_C] |= HEX_PC10;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC12)
			if (cfg.value[GPORT_C] & HEX_PC12) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC12) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC12",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC12_AND; cfg.pullup[GPORT_C] |= HEX_PC12;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC12_AND; cfg.pullup[GPORT_C] &= HEX_PC12_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC12;     cfg.value [GPORT_C] &= HEX_PC12_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC12;     cfg.value [GPORT_C] |= HEX_PC12;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC13)
			if (cfg.value[GPORT_C] & HEX_PC13) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC13) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC13",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC13_AND; cfg.pullup[GPORT_C] |= HEX_PC13;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC13_AND; cfg.pullup[GPORT_C] &= HEX_PC13_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC13;     cfg.value [GPORT_C] &= HEX_PC13_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC13;     cfg.value [GPORT_C] |= HEX_PC13;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC14)
			if (cfg.value[GPORT_C] & HEX_PC14) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC14) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC14",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC14_AND; cfg.pullup[GPORT_C] |= HEX_PC14;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC14_AND; cfg.pullup[GPORT_C] &= HEX_PC14_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC14;     cfg.value [GPORT_C] &= HEX_PC14_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC14;     cfg.value [GPORT_C] |= HEX_PC14;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC15)
			if (cfg.value[GPORT_C] & HEX_PC15) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC15) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC15",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC15_AND; cfg.pullup[GPORT_C] |= HEX_PC15;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC15_AND; cfg.pullup[GPORT_C] &= HEX_PC15_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC15;     cfg.value [GPORT_C] &= HEX_PC15_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC15;     cfg.value [GPORT_C] |= HEX_PC15;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC17)
			if (cfg.value[GPORT_C] & HEX_PC17) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC17) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC17",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC17_AND; cfg.pullup[GPORT_C] |= HEX_PC17;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC17_AND; cfg.pullup[GPORT_C] &= HEX_PC17_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC17;     cfg.value [GPORT_C] &= HEX_PC17_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC17;     cfg.value [GPORT_C] |= HEX_PC17;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC18)
			if (cfg.value[GPORT_C] & HEX_PC18) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC18) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC18",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC18_AND; cfg.pullup[GPORT_C] |= HEX_PC18;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC18_AND; cfg.pullup[GPORT_C] &= HEX_PC18_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC18;     cfg.value [GPORT_C] &= HEX_PC18_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC18;     cfg.value [GPORT_C] |= HEX_PC18;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC19)
			if (cfg.value[GPORT_C] & HEX_PC19) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC19) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC19",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC19_AND; cfg.pullup[GPORT_C] |= HEX_PC19;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC19_AND; cfg.pullup[GPORT_C] &= HEX_PC19_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC19;     cfg.value [GPORT_C] &= HEX_PC19_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC19;     cfg.value [GPORT_C] |= HEX_PC19;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC20)
			if (cfg.value[GPORT_C] & HEX_PC20) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC20) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC20",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC20_AND; cfg.pullup[GPORT_C] |= HEX_PC20;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC20_AND; cfg.pullup[GPORT_C] &= HEX_PC20_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC20;     cfg.value [GPORT_C] &= HEX_PC20_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC20;     cfg.value [GPORT_C] |= HEX_PC20;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC21)
			if (cfg.value[GPORT_C] & HEX_PC24) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC24) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC24",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC24_AND; cfg.pullup[GPORT_C] |= HEX_PC24;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC24_AND; cfg.pullup[GPORT_C] &= HEX_PC24_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC24;     cfg.value [GPORT_C] &= HEX_PC24_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC24;     cfg.value [GPORT_C] |= HEX_PC24;		break;}	// mode out	 value high
	
		if (cfg.mode[GPORT_C] & HEX_PC25)
			if (cfg.value[GPORT_C] & HEX_PC25) gg = 3; else gg = 2;	// output
		else 
			if (cfg.pullup[GPORT_C] & HEX_PC25) gg = 0; else gg = 1;	// input
	    cgiFormInteger("PC25",  &value, gg);	    
	    switch (value)	{
	    	case 0 : cfg.mode[GPORT_C] &= HEX_PC25_AND; cfg.pullup[GPORT_C] |= HEX_PC25;		break;	// mode in,  pullup
	    	case 1 : cfg.mode[GPORT_C] &= HEX_PC25_AND; cfg.pullup[GPORT_C] &= HEX_PC25_AND;	break;	// mode in	 pulldown
	    	case 2 : cfg.mode[GPORT_C] |= HEX_PC25;     cfg.value [GPORT_C] &= HEX_PC25_AND;	break;	// mode out	 value low	
	    	case 3 : cfg.mode[GPORT_C] |= HEX_PC25;     cfg.value [GPORT_C] |= HEX_PC25;		break;}	// mode out	 value high
		}
						
	SB_WriteConfig  (CFGFILE_ETC_GPIO,     (char *)&cfg,	sizeof(struct SB_GPIO_CONFIG));
}
//---------------------------------------------------------------------------
void set_dio()                                                   
{																	
int value, value2, gg, gv, gm;
struct SB_DIO_CONFIG	cfg;
                                                                                                              
	SB_ReadConfig  (CFGFILE_ETC_DIO,     (char *)&cfg,		sizeof(struct SB_DIO_CONFIG));		              

	if (cfg.lcd) gg = 1; else gg = 0;
	cgiFormInteger("LCD",  &value, gg);
	if (value)	cfg.lcd = 1; else cfg.lcd = 0;
 	
	//--------------------------------------------------------------------------------- 	
	if (cfg.mode & 0x0001) gg = 0; else	if (cfg.value & 0x0001) gg = 2; else gg = 1;
    cgiFormInteger("DIO00",  &value, gg);	    
    switch (value)	{
    	case 0 : cfg.mode |= 0x0001; break;
    	case 1 : cfg.mode &= 0xfffe; cfg.value &= 0xfffe;	break;
    	case 2 : cfg.mode &= 0xfffe; cfg.value |= 0x0001;	break;}
	//--------------------------------------------------------------------------------- 	
	if (cfg.mode & 0x0002) gg = 0; else	if (cfg.value & 0x0002) gg = 2; else gg = 1;
    cgiFormInteger("DIO01",  &value, gg);	    
    switch (value)	{
    	case 0 : cfg.mode |= 0x0002; break;
    	case 1 : cfg.mode &= 0xfffd; cfg.value &= 0xfffd;	break;
    	case 2 : cfg.mode &= 0xfffd; cfg.value |= 0x0002;	break;}
	//--------------------------------------------------------------------------------- 	
	if (cfg.mode & 0x0004) gg = 0; else	if (cfg.value & 0x0004) gg = 2; else gg = 1;
    cgiFormInteger("DIO02",  &value, gg);	    
    switch (value)	{
    	case 0 : cfg.mode |= 0x0004; break;
    	case 1 : cfg.mode &= 0xfffb; cfg.value &= 0xfffb;	break;
    	case 2 : cfg.mode &= 0xfffb; cfg.value |= 0x0004;	break;}
	//--------------------------------------------------------------------------------- 	
	if (cfg.mode & 0x0008) gg = 0; else	if (cfg.value & 0x0008) gg = 2; else gg = 1;
    cgiFormInteger("DIO03",  &value, gg);	    
    switch (value)	{
    	case 0 : cfg.mode |= 0x0008; break;
    	case 1 : cfg.mode &= 0xfff7; cfg.value &= 0xfff7;	break;
    	case 2 : cfg.mode &= 0xfff7; cfg.value |= 0x0008;	break;}
	//--------------------------------------------------------------------------------- 	
	if (cfg.mode & 0x0010) gg = 0; else	if (cfg.value & 0x0010) gg = 2; else gg = 1;
    cgiFormInteger("DIO04",  &value, gg);	    
    switch (value)	{
    	case 0 : cfg.mode |= 0x0010; break;
    	case 1 : cfg.mode &= 0xffef; cfg.value &= 0xffef;	break;
    	case 2 : cfg.mode &= 0xffef; cfg.value |= 0x0010;	break;}
	//--------------------------------------------------------------------------------- 	
	if (cfg.mode & 0x0020) gg = 0; else	if (cfg.value & 0x0020) gg = 2; else gg = 1;
    cgiFormInteger("DIO05",  &value, gg);	    
    switch (value)	{
    	case 0 : cfg.mode |= 0x0020; break;
    	case 1 : cfg.mode &= 0xffdf; cfg.value &= 0xffdf;	break;
    	case 2 : cfg.mode &= 0xffdf; cfg.value |= 0x0020;	break;}
	//--------------------------------------------------------------------------------- 	
	if (cfg.mode & 0x0040) gg = 0; else	if (cfg.value & 0x0040) gg = 2; else gg = 1;
    cgiFormInteger("DIO06",  &value, gg);	    
    switch (value)	{
    	case 0 : cfg.mode |= 0x0040; break;
    	case 1 : cfg.mode &= 0xffbf; cfg.value &= 0xffbf;	break;
    	case 2 : cfg.mode &= 0xffbf; cfg.value |= 0x0040;	break;}
	//--------------------------------------------------------------------------------- 	
	if (cfg.mode & 0x0080) gg = 0; else	if (cfg.value & 0x0080) gg = 2; else gg = 1;
    cgiFormInteger("DIO07",  &value, gg);	    
    switch (value)	{
    	case 0 : cfg.mode |= 0x0080; break;
    	case 1 : cfg.mode &= 0xff7f; cfg.value &= 0xff7f;	break;
    	case 2 : cfg.mode &= 0xff7f; cfg.value |= 0x0080;	break;}
	//--------------------------------------------------------------------------------- 	
	if (cfg.mode & 0x0100) gg = 0; else	if (cfg.value & 0x0100) gg = 2; else gg = 1;
    cgiFormInteger("DIO08",  &value, gg);	    
    switch (value)	{
    	case 0 : cfg.mode |= 0x0100; break;
    	case 1 : cfg.mode &= 0xfeff; cfg.value &= 0xfeff;	break;
    	case 2 : cfg.mode &= 0xfeff; cfg.value |= 0x0100;	break;}
	//--------------------------------------------------------------------------------- 	
	if (cfg.mode & 0x0200) gg = 0; else	if (cfg.value & 0x0200) gg = 2; else gg = 1;
    cgiFormInteger("DIO09",  &value, gg);	    
    switch (value)	{
    	case 0 : cfg.mode |= 0x0200; break;
    	case 1 : cfg.mode &= 0xfdff; cfg.value &= 0xfdff;	break;
    	case 2 : cfg.mode &= 0xfdff; cfg.value |= 0x0200;	break;}
	//--------------------------------------------------------------------------------- 	
	if (cfg.mode & 0x0400) gg = 0; else	if (cfg.value & 0x0400) gg = 2; else gg = 1;
    cgiFormInteger("DIO10",  &value, gg);	    
    switch (value)	{
    	case 0 : cfg.mode |= 0x0400; break;
    	case 1 : cfg.mode &= 0xfbff; cfg.value &= 0xfbff;	break;
    	case 2 : cfg.mode &= 0xfbff; cfg.value |= 0x0400;	break;}
	//--------------------------------------------------------------------------------- 	
	if (cfg.mode & 0x0800) gg = 0; else	if (cfg.value & 0x0800) gg = 2; else gg = 1;
    cgiFormInteger("DIO11",  &value, gg);	    
    switch (value)	{
    	case 0 : cfg.mode |= 0x0800; break;
    	case 1 : cfg.mode &= 0xf7ff; cfg.value &= 0xf7ff;	break;
    	case 2 : cfg.mode &= 0xf7ff; cfg.value |= 0x0800;	break;}
	//--------------------------------------------------------------------------------- 	
	if (cfg.mode & 0x1000) gg = 0; else	if (cfg.value & 0x1000) gg = 2; else gg = 1;
    cgiFormInteger("DIO12",  &value, gg);	    
    switch (value)	{
    	case 0 : cfg.mode |= 0x1000; break;
    	case 1 : cfg.mode &= 0xefff; cfg.value &= 0xefff;	break;
    	case 2 : cfg.mode &= 0xefff; cfg.value |= 0x1000;	break;}
	//--------------------------------------------------------------------------------- 	
	if (cfg.mode & 0x2000) gg = 0; else	if (cfg.value & 0x2000) gg = 2; else gg = 1;
    cgiFormInteger("DIO13",  &value, gg);	    
    switch (value)	{
    	case 0 : cfg.mode |= 0x2000; break;
    	case 1 : cfg.mode &= 0xdfff; cfg.value &= 0xdfff;	break;
    	case 2 : cfg.mode &= 0xdfff; cfg.value |= 0x2000;	break;}
	//--------------------------------------------------------------------------------- 	
	if (cfg.mode & 0x4000) gg = 0; else	if (cfg.value & 0x4000) gg = 2; else gg = 1;
    cgiFormInteger("DIO14",  &value, gg);	    
    switch (value)	{
    	case 0 : cfg.mode |= 0x4000; break;
    	case 1 : cfg.mode &= 0xbfff; cfg.value &= 0xbfff;	break;
    	case 2 : cfg.mode &= 0xbfff; cfg.value |= 0x4000;	break;}
	//--------------------------------------------------------------------------------- 	
	if (cfg.mode & 0x8000) gg = 0; else	if (cfg.value & 0x8000) gg = 2; else gg = 1;
    cgiFormInteger("DIO15",  &value, gg);	    
    switch (value)	{
    	case 0 : cfg.mode |= 0x8000; break;
    	case 1 : cfg.mode &= 0x7fff; cfg.value &= 0x7fff;	break;
    	case 2 : cfg.mode &= 0x7fff; cfg.value |= 0x8000;	break;}

	SB_WriteConfig  (CFGFILE_ETC_DIO,     (char *)&cfg,		sizeof(struct SB_DIO_CONFIG));	
}
//---------------------------------------------------------------------------
void set_password()
{
    char buff[64];
    char buff2[64];
    char cmd[256];
    int result;
    int fd;
    struct SB_SYSTEM_CONFIG	cfg;

	SB_ReadConfig  (CFGFILE_ETC_SYSTEM,     (char *)&cfg,		sizeof(struct SB_SYSTEM_CONFIG));
    result = cgiFormStringNoNewlines("NEW_ID", buff, 16);
    if (result == cgiFormEmpty) // ID will not be changed.
    	{
	   listPutf(list, "newid", cfg.username);	
    	}
    else
    	{
	   	listPutf(list, "newid", buff);
	   	memcpy (cfg.username, buff, 16);
    	}

    result = cgiFormStringNoNewlines("OLD_PW", buff, 16);
    if (result == cgiFormEmpty)
    	{
	   if (cgiFormStringNoNewlines("NEW_PW", buff, 16) != cgiFormEmpty)
		  Error("Enter the current password");
	   if (cgiFormStringNoNewlines("NEW_PW2", buff, 16) != cgiFormEmpty)
		  Error("Enter the current password");
    
	    listPutf(list, "newpw", cfg.password);	
		}	
    else
    	{
	   // check password
	   if (strcmp(buff, cfg.password) != 0)
		  Error("Current password entered is wrong");

	   // verify new password
	   result = cgiFormStringNoNewlines("NEW_PW", buff, 16);
	   if (result == cgiFormNotFound)
		  Error("Wrong password input");
	   else if (result == cgiFormEmpty)
		  Error("Enter the new password");

	   result = cgiFormStringNoNewlines("NEW_PW2", buff2, 16);
	   if (result == cgiFormNotFound)
		  Error("Wrong password input");
	   else if (result == cgiFormEmpty)
		  Error("Enter the new password");

	   if (strcmp(buff, buff2) != 0)
		  Error("Two passwords do not match!");

	   listPutf(list, "newpw", buff);
		memcpy (cfg.password, buff, 16);
    	}
	
	SB_WriteConfig  (CFGFILE_ETC_SYSTEM,     (char *)&cfg,		sizeof(struct SB_SYSTEM_CONFIG));
}
//---------------------------------------------------------------------------
void set_update()
{
char name[1024];
char buffer[1024];

if (cgiFormtFileName("filename", name, sizeof(name)) != cgiFormSuccess) 
Error("Component Not Found!");

sprintf(buffer,"mv %s %s", name, UPLOAD_FILE_NAME);
system(buffer);
}
//---------------------------------------------------------------------------
void set_onlyupdate()
{
char buff[255];

	sprintf (buff, "/sbin/upgrade  %s;sleep 3;/sbin/reboot", UPLOAD_FILE_NAME);
//	sprintf (buff, "/sbin/upgrade  %s", UPLOAD_FILE_NAME);	
	system (buff);
}

void set_reboot()
{
    char buff[32];
    if (cgiFormStringNoNewlines("REBOOT", buff, 16) ==  cgiFormNotFound)
	   Error("Choose correct option");
    else
    	{
    	if (strncmp(buff, "Save & Reboot", sizeof(buff)) == 0)
			{
			system("/sbin/def save");
			}
		else 
			if (strncmp(buff, "Reboot Only", sizeof(buff)) == 0)
				{
				;
				}
	   		else
		  		Error("Option fetch failed");
		}
}
//---------------------------------------------------------------------------
void set_test()
{
    sleep(2);
    fcp(4);
    sleep(5);
}
//---------------------------------------------------------------------------
static int safe_open (const char *pathname,int flags)
{
    int fd;

    fd = open (pathname,flags);
    if (fd < 0)
	   Error("Open error!");
    return (fd);
}
//---------------------------------------------------------------------------
static void safe_read (int fd,const char *filename,void *buf,size_t count)
{
    ssize_t result;
    result = mcpanic_read (fd,buf,count);

    if (count != result)
    {
	   if (result < 0)
		  Error("Read error!");
	   Error("Read error!");
    }
}
//---------------------------------------------------------------------------
static void safe_rewind (int fd,const char *filename)
{
    if (lseek (fd,0L,SEEK_SET) < 0)
	   Error("File seek error!");
}
/******************************************************************************/
static void cleanup (void)
{
    if (dev_fd > 0)
	   close (dev_fd);
    if (fil_fd > 0) 
	   close (fil_fd);
}
//---------------------------------------------------------------------------
ssize_t mcpanic_read(int fd, void *buf, size_t count)
{
    ssize_t n;
    do {
	   n = read (fd, buf, count);
    } while (n<0 && errno==EINTR);
    return n;
}
//---------------------------------------------------------------------------
int fcp (int mtd_no)
{
	return 1;
}

