#include "agent.h"
//#include "../../src/include/p_define.h"
#include "../../src/include/p_default.h"
//#include "../../src/include/p_flash.h"
#include "../../src/set/func.h"
#include "include/default.h"
#define HEADER_FROM_CGI 1
#include "../../src/set/set.h"

#ifdef MIRHEE_FIX_DHCP_CLIENT
#include <net/if.h>					// for "ifreq" structure
#endif // MIRHEE_FIX_DHCP_CLIENT
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <linux/mtd/mtd.h>

// update
#define PROCDIR "/proc/"

// fcp
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/mtd/mtd.h>
// mcpanic 051012
#include <errno.h>

typedef int bool;
#define true 1
#define false 0

/* size of read/write buffer */
#define BUFFERSIZE (10 * 1024)

//static int dev_fd = -1;
//static int fil_fd = -1;


// common
struct CONFIG cfg;
LIST list;
void read_conf();
void set_login();
void set_logout();
void set_network();
int set_serial();
void set_admin();
void set_password();
void set_update();
void set_onlyupdate();
void set_reboot();


unsigned char get_char_from_dps( unsigned char char_dps );
unsigned char get_parity_from_dps( unsigned char parity_dps );
unsigned char get_stop_from_dps( unsigned char stop_dps );


int cgiMain()
{
    //    char passBuff[12];
    char buff[256];
    char target_page[64];
    char type=0;
    int i;
    int mode;	// 0: html, 1: cgi
    int ret;

    mode = 0;
    read_conf();

    //	DebugMessage(cgiPathInfo);	-- none
    //	DebugMessage(cgiPathTranslated);	-- none	
    //	DebugMessage(cgiQueryString);	
    for (i=0; i<4; i++)
    {
	   buff[i] = cgiQueryString[i];
    }
    buff[i] = '\0';
    if (!strcmp(buff, "type"))
    {
	   type = cgiQueryString[5];
    }
    else
	   Error("URL Parse Error");

    // TSUM for reboot
    if (type != 'l')
    {
	   if (CheckLogin() == 0)
		  Error("Please login first !");
    }


    //	if (type == '6')
    //		set_update();

    cgiHeaderContentType("text/html");
    // for firefox fix
    //	fflush(cgiOut);

    // create list
    list = listCreate();
    // get common values
    // device_name and username
    strcpy(buff, cfg.svr.name);
    listPutf(list, "device_name", buff);
    strcpy(buff, cfg.svr.username);
    listPutf(list, "username", buff);

    if (cfg.svr.wireless==0)
    {
	   listPutf(list,"hideon","<!--");
	   listPutf(list,"hideoff","-->");
    }
    else
    {
	   listPutf(list,"hideon","");
	   listPutf(list,"hideoff","");
    }


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
		  set_network();
		  mode = 1;
		  break;
	   case '2':	// serial
		  ret= set_serial();
		  if (ret==0)
			 strcpy(target_page, "/cgi-bin/getagent.cgi?type=3");
		  else
			 strcpy(target_page, "/cgi-bin/getagent.cgi?type=b");
		  mode = 1;
		  break;
	   case '3':	// wireless
		  strcpy(target_page, "/cgi-bin/getagent.cgi?type=4");
		  set_wireless();
		  mode = 1;
		  break;
	   case '4':	// admin
		  strcpy(target_page, "/cgi-bin/getagent.cgi?type=5");
		  set_admin();
		  mode = 1;
		  break;
	   case '5':	// password
		  strcpy(target_page, "afterPassword.html");
		  set_password();
		  break;
	   case '6':	// update
		  //			set_update();
		  strcpy(target_page, "afterUpdate.html");
		  break;
	   case '7':	// onlyupdate
		  strcpy(target_page, "afterComplete.html");
		  set_onlyupdate();
		  break;
	   case '8':	// factory
		  listPutf(list, "ip", DEF_IP);
		  system("/sbin/set factory");
		  strcpy(target_page, "afterFactory.html");
		  break;
	   case '9':	// save & reboot
		  set_reboot();
		  strcpy(target_page, "afterReboot.html");
		  break;
	   case '0':	// onlyreboot
		  system("/sbin/set reboot");
		  return 0;	// return directly
	   default:
		  //			DebugMessage("Page doesn't exist!");
		  //set_test();
		  strcpy(target_page, "/cgi-bin/getagent.cgi?type=1");
		  mode = 1;
		  break;
    }
    // check login
    if (type != 'o')
    {
	   if (CheckLogin() == 0)
		  Error("Please login first!");
    }

    if (mode == 0)	// HTML
    {
	   sprintf(buff, "%s%s", HTMLPATH, target_page);
	   //	sprintf(buff, "/%s", target_page);
	   listDisplay(list, buff);
	   fflush(0);
	   listDelete(list);

	   if (type == '6')
		  set_update();
    }
    else		// CGI
    {
	   fflush(0);
	   fprintf(cgiOut, "<html>\n<head>\n");
	   fprintf(cgiOut, "<meta http-equiv='refresh' content='0; URL=%s'>\n", target_page);
	   fprintf(cgiOut, "</head>\n</html>\n");
	   fflush(0);
    }
    return 0;
}

// read from configuration file and store in struct CONFIG
void read_conf()
{
    int fd;
    fd = open(CFG_FILE_PATH, 0);
    if (fd < 0)
    {
	   Error("config file open error\n");
    }
    lseek(fd, 0, 0);
    read (fd, &cfg, sizeof (struct CONFIG));
    close (fd);
}

void set_login()
{
    char buff[16];
    //char buff2[16];
    //char temp[256];
    int value;

    // read from form
    value = cgiFormStringNoNewlines("username", buff, 16);
    if (value == cgiFormNotFound)
	   Error("Username error");
    else if (value == cgiFormEmpty)
	   Error("Enter username");

    if (CheckAdmin(0, buff) == 0)
	   Error("Invalid username");

    // read from form
    value = cgiFormStringNoNewlines("pw", buff, 16);
    if (value == cgiFormNotFound)
	   Error("Password error");
    else if (value == cgiFormEmpty)
	   Error("Enter password");

    if (CheckAdmin(1, buff) == 0)
	   Error("Invalid password");

    value = open("/var/tmp/login.stat", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (value < 0)
    {
	   Error("Internal login error");
	   close (value);
    }
    write(value, "Y", 1);
    close(value);
}

void set_logout()
{
    unlink("/var/tmp/login.stat");
}


void set_network()
{
    char buff[64];
    char cmd[256];
    int value;
    int value2=0;

    if (cfg.svr.line == 'I')
	   value2 = 0;
    else if (cfg.svr.line == 'D')
	   value2 = 1;
    cgiFormInteger("N_LINE", &value, value2);

    // line type, IP, subnet, gateway
    if (value == 0)		// static
    {
	   // Line Type
	   system("/sbin/set nw mode static");

	   // IP
	   value2 = cgiFormStringNoNewlines("N_IP", buff, 16);
	   if (value2 == cgiFormNotFound)
		  Error("Enter the IP address");
	   else if (value2 == cgiFormEmpty)
	   {
		  sprintf(cmd, "/sbin/set nw ip %d.%d.%d.%d",
				cfg.svr.ip[0], cfg.svr.ip[1],cfg.svr.ip[2],cfg.svr.ip[3]);
		  system(cmd);
	   }
	   else
	   {
		  sprintf(cmd, "/sbin/set nw ip %s", buff);
		  system(cmd);
	   }

	   // Subnet
	   value2 = cgiFormStringNoNewlines("N_MASK", buff, 16);
	   if (value2 == cgiFormNotFound)
		  Error("Enter the subnet mask");
	   else if (value2 == cgiFormEmpty)
	   {	
		  sprintf(cmd, "/sbin/set nw subnet %d.%d.%d.%d",
				cfg.svr.mask[0], cfg.svr.mask[1],cfg.svr.mask[2],cfg.svr.mask[3]);
		  system(cmd);
	   }
	   else
	   {
		  sprintf(cmd, "/sbin/set nw subnet %s", buff);
		  system(cmd);
	   }

	   // Gateway
	   value2 = cgiFormStringNoNewlines("N_GW", buff, 16);
	   if (value2 == cgiFormNotFound)
		  Error("Enter the default gateway");
	   else if (value2 == cgiFormEmpty)
	   {	
		  sprintf(cmd, "/sbin/set nw gateway %d.%d.%d.%d",
				cfg.svr.gateway[0], cfg.svr.gateway[1],cfg.svr.gateway[2],cfg.svr.gateway[3]);
		  system(cmd);
	   }
	   else
	   {
		  sprintf(cmd, "/sbin/set nw gateway %s", buff);
		  system(cmd);
	   }
	   // DNS
	   value2 = cgiFormStringNoNewlines("N_DNS", buff, 16);
	   if (value2 == cgiFormNotFound)
		  Error("Enter the DNS");
	   else if (value2 == cgiFormEmpty)
	   {	
		  sprintf(cmd, "/sbin/set nw dns %d.%d.%d.%d",
				cfg.svr.dnsip[0], cfg.svr.dnsip[1],cfg.svr.dnsip[2],cfg.svr.dnsip[3]);
		  system(cmd);
	   }
	   else
	   {
		  sprintf(cmd, "/sbin/set nw dns %s", buff);
		  system(cmd);
	   }

    }
    else if (value == 1)	// DHCP
    {
	   system("/sbin/set nw mode dhcp");
    }
    else 
    {
	   system("/sbin/set nw mode static");
    }



#if 0
    if (cfg.svr.wireless)
    {
	   cgiFormInteger("N_BR", &value, cfg.svr.br);
	   if (value == 1)
		  system("/sbin/set br 1");
	   else	
		  system("/sbin/set br 0");
    }
#endif

    //LAN IP

    if (/*value == 0 && nat set bug*/ cfg.svr.wireless ==1)
    {
	   value2 = cgiFormStringNoNewlines("N_LIP", buff, 16);
	   if (value2 == cgiFormNotFound)
		  Error("Enter the NAT IP address");
	   else if (value2 == cgiFormEmpty)
	   {
		  sprintf(cmd, "/sbin/set nw nat ip %d.%d.%d.%d",
				cfg.svr.lanip[0], cfg.svr.lanip[1],cfg.svr.lanip[2],cfg.svr.lanip[3]);
		  system(cmd);
	   }
	   else
	   {
		  sprintf(cmd, "/sbin/set nw nat ip %s", buff);
		  system(cmd);
	   }

	   // Subnet
	   value2 = cgiFormStringNoNewlines("N_LMASK", buff, 16);
	   if (value2 == cgiFormNotFound)
		  Error("Enter the NAT subnet mask");
	   else if (value2 == cgiFormEmpty)
	   {	
		  sprintf(cmd, "/sbin/set nw nat subnet %d.%d.%d.%d",
				cfg.svr.lanmask[0], cfg.svr.lanmask[1],cfg.svr.lanmask[2],cfg.svr.lanmask[3]);
		  system(cmd);
	   }
	   else
	   {
		  sprintf(cmd, "/sbin/set nw nat subnet %s", buff);
		  system(cmd);
	   }

	   cgiFormInteger("N_DHCPS", &value, cfg.svr.dhcpuse);
	   if (value == 1)
	   {
		  system("/sbin/set nw nat dhcps enable");
		  value2 = cgiFormStringNoNewlines("N_SADDR", buff, 16);
		  if (value2 == cgiFormNotFound)
			 Error("Enter the Start Address");
		  else 
		  {	
			 sprintf(cmd, "/sbin/set nw nat saddr %s", buff);
			 system(cmd);
		  }
		  value2 = cgiFormStringNoNewlines("N_EADDR", buff, 16);
		  if (value2 == cgiFormNotFound)
			 Error("Enter the End Address");
		  else 
		  {	
			 sprintf(cmd, "/sbin/set nw nat eaddr %s", buff);
			 system(cmd);
		  }

	   }
	   else	
		  system("/sbin/set nw nat dhcps disable");
    }

    // SNMP
    cgiFormInteger("N_SNMP", &value, cfg.svr.snmp);
    if (value == 1)
	   system("/sbin/set nw nms snmp enable");
    else	
	   system("/sbin/set nw nms snmp disable");

    // TSUM 6621 PORTVIEW
    if (cfg.svr.nip[0]==0)
	   value2=0;
    else
	   value2 =1;
    cgiFormInteger("N_PORTVIEW", &value, value2);

    if (value==0)
    {
	   sprintf(cmd, "/sbin/set nw nms portview ip 0.0.0.0");
	   system(cmd);
	   return;
    }

    // NMS
    value = cgiFormStringNoNewlines("N_NIP", buff, 16);
    if (value == cgiFormNotFound)
	   Error("Enter Portview's IP");
    else if (value == cgiFormEmpty)
    {	
	   sprintf(cmd, "/sbin/set nw nms portview ip %d.%d.%d.%d",
			 cfg.svr.nip[0], cfg.svr.nip[1],cfg.svr.nip[2],cfg.svr.nip[3]);
	   system(cmd);
    }
    else
    {
	   sprintf(cmd, "/sbin/set nw nms portview ip %s", buff);
	   system(cmd);
    }

    // NMS Port
    value = cgiFormStringNoNewlines("N_NPORT", buff, 6);
    if (value == cgiFormNotFound)
	   Error ("Enter Portview's Port");
    else if (value == cgiFormEmpty)
    {
	   sprintf(cmd, "/sbin/set nw nms portview port %d", cfg.svr.nport);
	   system(cmd);
    }
    else
    {
	   sprintf(cmd, "/sbin/set nw nms portview port %d", atoi(buff));
	   system(cmd);
    }

    // Device Name
    /*
	  value = cgiFormStringNoNewlines("N_DNAME", buff, 32);
	  if (value == cgiFormNotFound)
	  Error ("Enter Device Name");
	  else if (value == cgiFormEmpty)
	  {
	  sprintf(cmd, "/sbin/set admin name %s", cfg.svr.name);
	  system(cmd);
	  }
	  else
	  {
	  sprintf(cmd, "/sbin/set admin name %s", buff);
	  system(cmd);
	  }
	*/	
    // Group
    value = cgiFormStringNoNewlines("N_GROUP", buff, 32);
    if (value == cgiFormNotFound)
	   Error ("Enter Group Name for Portview");
    else if (value == cgiFormEmpty)
    {
	   sprintf(cmd, "/sbin/set nw nms portview group %s", cfg.svr.group);
	   system(cmd);
    }
    else
    {
	   sprintf(cmd, "/sbin/set nw nms portview group %s", buff);
	   system(cmd);
    }

    // Location
    value = cgiFormStringNoNewlines("N_LOC", buff, 32);
    if (value == cgiFormNotFound)
	   Error ("Enter location information for NMS");
    else if (value == cgiFormEmpty)
    {
	   sprintf(cmd, "/sbin/set nw nms portview location %s", cfg.svr.location);
	   system(cmd);
    }
    else
    {
	   sprintf(cmd, "/sbin/set nw nms portview location %s", buff);
	   system(cmd);
    }

}

int set_serial()
{
    int value;
    int value2;
    int mode;
    char buff[64];
    char cmd[256];
    char charval;
    int portno;

    if ( cgiFormInteger("PAGENUM", &portno, 0) != cgiFormSuccess )
	   Error("Where are your from?");

    portno -=1;
    // port status	
    value2 = cfg.port[portno].status;
    cgiFormInteger("STATUS", &value, value2);
    if (value == 0)
	   sprintf(cmd,"/sbin/set serial %d status disable",portno+1);
    else if (value == 1)
	   sprintf(cmd,"/sbin/set serial %d status enable",portno+1);
    system(cmd);

    if (value==0)
	   return portno;

    // serial interface
    cgiFormInteger("INTERFACE", &value, cfg.port[portno].iType);
    sprintf(cmd,"/sbin/set serial %d interface %s",portno+1,char_serial_interface[value]);
    system(cmd);
    cgiFormInteger("TERMINATION", &value, cfg.port[portno].tr);
    sprintf(cmd,"/sbin/set serial %d tr %s",portno+1,char_status[value]);
    system(cmd);
    // cannot set manually (yet)

    // operation mode
    cgiFormInteger("OPMODE", &mode, cfg.port[portno].protocol);
    sprintf(cmd,"/sbin/set serial %d mode %s",portno+1,char_serial_mode[mode]);
    system(cmd);

    // Local socket port
    value = cgiFormStringNoNewlines("LOCAL_PORT", buff, 6);
    if (value == cgiFormNotFound)
	   Error ("Enter local socket port");
    else if (value == cgiFormEmpty)
    {	// set to current value
	   sprintf(cmd, "/sbin/set serial %d port %d",portno+1, cfg.port[portno].sockno);
	   system(cmd);
    }
    else 
    {
	   sprintf(cmd, "/sbin/set serial %d port %d",portno+1, atoi(buff));
	   system(cmd);
    }

    // Port Alias
    value = cgiFormStringNoNewlines("ALIAS", buff, 16); 
    if (value == cgiFormNotFound)
	   Error ("Enter port alias name");
    else if (value == cgiFormEmpty)
    {	// set to current value
	   sprintf(cmd, "/sbin/set serial %d name %s",portno+1, cfg.port[portno].pname);
	   system(cmd);
    }
    else
    {
	   sprintf(cmd, "/sbin/set serial %d name %s",portno+1, buff);
	   system(cmd);
    }

    // baud rate            
    cgiFormInteger("BAUDRATE", &value, cfg.port[portno].speed);
    sprintf(cmd,"/sbin/set serial %d speed %s",portno+1,char_serial_speed[value]);
    system(cmd);
    /////////////////////// TSUM TSUM 

    // data bits
    charval = get_char_from_dps(cfg.port[portno].dps);
    if (charval == '5')
	   value2 = 0;
    else if (charval == '6')
	   value2 = 1;
    else if (charval == '7')
	   value2 = 2;
    else if (charval == '8')
	   value2 = 3;
    cgiFormInteger("DATABIT", &value, value2);
    sprintf(cmd,"/sbin/set serial %d data %s",portno+1,char_serial_data[value]);
    system(cmd);

    // stop bits			
    charval = get_stop_from_dps(cfg.port[portno].dps);
    if (charval == '1')
	   value2 = 0;
    else if (charval == '2')
	   value2 = 1;
    cgiFormInteger("STOPBIT", &value, value2);
    sprintf(cmd,"/sbin/set serial %d stop %s",portno+1,char_serial_stop[value]);
    system(cmd);

    // parity			
    charval = get_parity_from_dps(cfg.port[portno].dps);
    if (charval == 'N')
	   value2 = 0;
    else if (charval == 'O')
	   value2 = 1;
    else if (charval == 'E')
	   value2 = 2;
    cgiFormInteger("PARITY", &value, value2);
    sprintf(cmd,"/sbin/set serial %d parity %s",portno+1,char_serial_parity[value]);
    system(cmd);

    // flow control
    value2 = cfg.port[portno].flow;
    cgiFormInteger("FLOW", &value, value2);
    sprintf(cmd,"/sbin/set serial %d flow %s",portno+1,char_serial_flow[value]);
    system(cmd);

    // signal (device type)	
    value2 = cfg.port[portno].signal;
    cgiFormInteger("DEVICETYPE", &value, value2);
    sprintf(cmd,"/sbin/set serial %d signal %s",portno+1,char_serial_signal[value]);
    system(cmd);

    // Remote IP Address / Port	
    value = cgiFormStringNoNewlines("SERVER_IP", buff, 16);
    if (value == cgiFormNotFound)
	   Error ("Enter remote IP address");
    else if (value == cgiFormEmpty)
    {	
	   sprintf(cmd, "/sbin/set serial %d rip %d.%d.%d.%d",portno+1,
			 cfg.port[portno].rip[0], cfg.port[portno].rip[1],cfg.port[portno].rip[2],cfg.port[portno].rip[3]);
	   system(cmd);
    }
    else
    {	
	   sprintf(cmd, "/sbin/set serial %d rip %s",portno+1, buff);
	   system(cmd);
    }

    value = cgiFormStringNoNewlines("SERVER_PORT", buff, 6);
    if (value == cgiFormNotFound )
	   Error ("Enter remote server port");
    else if (value == cgiFormEmpty)
    {	
	   sprintf(cmd, "/sbin/set serial %d rport %d",portno+1, cfg.port[portno].rport);
	   system(cmd);
    }
    else
    {	
	   sprintf(cmd, "/sbin/set serial %d rport %d",portno+1, atoi(buff));
	   system(cmd);
    }

    // Keepalive
    value = cgiFormStringNoNewlines("ALIVE_TIME", buff, 5);
    if (value == cgiFormNotFound )
	   Error("Enter keepalive time value");
    else if (value == cgiFormEmpty)
    {	
	   sprintf(cmd, "/sbin/set serial %d alive %d",portno+1, cfg.port[portno].alive_min);
	   system(cmd);
    }
    else
    {	
	   sprintf(cmd, "/sbin/set serial %d alive %d",portno+1, atoi(buff));
	   system(cmd);
    }

    // MTU	
    value = cgiFormStringNoNewlines("MTU", buff, 5);
    if (value == cgiFormNotFound )
	   Error("Enter MTU value");
    else if (value == cgiFormEmpty)
    {	
	   sprintf(cmd, "/sbin/set serial %d mtu %d",portno+1, cfg.port[portno].mtu);
	   system(cmd);
    }
    else
    {	
	   sprintf(cmd, "/sbin/set serial %d mtu %d",portno+1, atoi(buff));
	   system(cmd);
    }

    // Port Login	
    value2 = cfg.port[portno].plogin;
    cgiFormInteger("PASSIVELOGIN", &value, value2);


    if (value == 0)
    {
	   sprintf(cmd,"/sbin/set serial %d login disable",portno+1);
	   system(cmd);
    }
    else if (value == 1)
    {
	   sprintf(cmd,"/sbin/set serial %d login enable",portno+1);
	   system(cmd);
	   // Passive Username/Password	
	   value2 = cgiFormStringNoNewlines("PASSIVE_USER", buff, 32);
	   if (value2 == cgiFormNotFound )
		  Error("Enter passive login username");
	   else if (value2 == cgiFormEmpty)
	   {
		  sprintf(cmd, "/sbin/set serial %d username %s",portno+1, cfg.port[portno].username);
		  system(cmd);
	   }
	   else
	   {
		  sprintf(cmd, "/sbin/set serial %d username %s",portno+1, buff);
		  system(cmd);
	   }

	   memset(buff,0,sizeof(buff));

	   value2 = cgiFormStringNoNewlines("PASSIVE_PASS", buff, 32);
	   if (value2 == cgiFormNotFound )
		  Error("Enter passive login password");
	   else if (value2 == cgiFormEmpty)
	   {
		  sprintf(cmd, "/sbin/set serial %d password %s",portno+1, cfg.port[portno].password);
		  system(cmd);
	   }
	   else
	   {
		  sprintf(cmd, "/sbin/set serial %d password %s",portno+1, buff);
		  system(cmd);
	   }
    }

    // RESET PORT
    if ( cgiFormStringNoNewlines("RESETPORT",buff,5) == cgiFormNotFound ) {
    }
    else {
	   sprintf(cmd,"/var/run/resetport%d",portno+1);
	   close(open(cmd,O_RDWR|O_CREAT));
    }

    return portno;

}

void set_admin()
{
    int result;
    char buff[64];
    char cmd[256];

    // device name
    result = cgiFormStringNoNewlines("DEVICE_NAME", buff, 32);
    if (result == cgiFormNotFound)
	   Error("Enter the device name");
    else if (result == cgiFormEmpty)
    {
	   sprintf(cmd, "/sbin/set admin name %s", cfg.svr.name);
	   system (cmd);
    }
    else
    {
	   sprintf(cmd, "/sbin/set admin name %s", buff);
	   system (cmd);
    }


    // website
    result = cgiFormStringNoNewlines("WEBSITE", buff, 50);
    if (result == cgiFormNotFound)
	   Error("Enter the website");
    else if (result == cgiFormEmpty)
    {
	   sprintf(cmd, "/sbin/set admin website %s", cfg.support.website);
	   system (cmd);
    }
    else
    {
	   sprintf(cmd, "/sbin/set admin website %s", buff);
	   system (cmd);
    }


    // contact
    result = cgiFormStringNoNewlines("CONTACT", buff, 50);
    if (result == cgiFormNotFound)
	   Error("Enter the contact information");
    else if (result == cgiFormEmpty)
    {
	   sprintf(cmd, "/sbin/set admin contact %s", cfg.support.contact);
	   system (cmd);
    }
    else
    {
	   sprintf(cmd, "/sbin/set admin contact %s", buff);
	   system (cmd);
    }

}

void set_password()
{
    char buff[64];
    char buff2[64];
    char cmd[256];
    int result;
    //int fd;

#if 0
    // new ID
    result = cgiFormStringNoNewlines("NEW_ID", buff, 16);
    if (result == cgiFormNotFound)
	   Error("Enter the new ID");
    else if (result == cgiFormEmpty) // ID will not be changed.
    {
	   memset(buff2,0,sizeof(buff2));
	   fd = open("/var/tmp/login.id", O_RDONLY);
	   if (fd > 0)
	   {
		  if (read(fd, buff2, 16) < 0)
			 close (fd);
		  close (fd);
	   }
	   else
	   {
		  close (fd);
		  Error("File I/O Error");
	   }
	   listPutf(list, "newid", buff2);	
    }
    else
    {
	   listPutf(list, "newid", buff);
	   sprintf(cmd, "/sbin/set user %s", buff);
	   system (cmd);
    }
#endif
    // password setup
    result = cgiFormStringNoNewlines("OLD_PW", buff, 16);
    if (result == cgiFormNotFound)
	   Error("Enter the current password");
    else if (result == cgiFormEmpty)
    {
	   if (cgiFormStringNoNewlines("NEW_PW", buff, 16) != cgiFormEmpty)
		  Error("Enter the current password");
	   if (cgiFormStringNoNewlines("NEW_PW2", buff, 16) != cgiFormEmpty)
		  Error("Enter the current password");
    }	
    else
    {
	   // check password
	   if (strcmp(buff, cfg.svr.pass) != 0)
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
	   sprintf(cmd, "/sbin/set passwd -systemadmin %s", buff);
	   system (cmd);
    }
}

void set_update()
{
    /*
	  cgiFilePtr file;
	  char name[1024];
	  char contentType[1024];
	  char buffer[1024];
	  int size;
	  int got;
	  int fd;

	  unlink(UPLOAD_FILE_NAME);

	  if (cgiFormFileName("filename", name, sizeof(name)) != cgiFormSuccess) {
	  Error("Component Not Found!");
	  return;
	  }
	  cgiFormFileSize("filename", &size);
	  cgiFormFileContentType("filename", contentType, sizeof(contentType));
	  if (cgiFormFileOpen("filename", &file) != cgiFormSuccess) {
	  Error("File Open Error");
	  return;
	  }
	  fd = open(UPLOAD_FILE_NAME,O_RDWR|O_CREAT,0666);
	  while (cgiFormFileRead(file, buffer, sizeof(buffer), &got) == cgiFormSuccess)
	  {
	  write(fd,buffer,got);
	  }
	  cgiFormFileClose(file);
	  close(fd);
	*/
    char name[1024];
    char buffer[1024];

    if (cgiFormtFileName("filename", name, sizeof(name)) != cgiFormSuccess) 
	   Error("Component Not Found!");

    sprintf(buffer,"mv %s %s", name, UPLOAD_FILE_NAME);
    system(buffer);
}

void set_onlyupdate()
{
    int fd;
    int size;
    char buff[255];

    memset(buff,0,sizeof(buff));

    fd=open(UPLOAD_FILE_NAME, O_RDWR);
    if (fd<0)
	   Error("Upload File Not Found!");

    size = lseek(fd, 0, SEEK_END);
    lseek(fd, -4, SEEK_END);
    read(fd, buff, 4);
    buff[4] = '\0';
    ftruncate (fd, size-4);
    close(fd);

    if (strncmp("mtd2", buff, 4) ==0)
    {
	   sleep(1);
	   system("eraseall /dev/mtd2");
	   sleep(1);
	   system("fcp /var/tmp/Image.bin /dev/mtd2");
	   sleep(1);
    }
    else if (strncmp("mtd3",buff,4)==0)
    {
	   sleep(1);
	   system("eraseall /dev/mtd3");
	   sleep(1);
	   system("fcp /var/tmp/Image.bin /dev/mtd3");
	   sleep(1);
    }
    else if (strncmp("mtd4",buff,4)==0)
    {
	   sleep(1);
	   system("eraseall /dev/mtd4");
	   sleep(1);
	   system("/var/fcp /var/tmp/Image.bin /dev/mtd4");
	   sleep(1);
    }
    else
    {
	   unlink(UPLOAD_FILE_NAME);
	   Error("Uploaded file is not a correct firmware file");
    }

    unlink(UPLOAD_FILE_NAME);
    sleep(5);
}


void set_reboot()
{
    char buff[32];
    if (cgiFormStringNoNewlines("REBOOT", buff, 16) ==  cgiFormNotFound)
	   Error("Choose correct option");
    else
    {
	   if (strncmp(buff, "Save & Reboot", sizeof(buff)) == 0)
		  system("/sbin/set save");
	   else if (strncmp(buff, "Reboot Only", sizeof(buff)) == 0)
		  ;
	   else
		  Error("Option fetch failed");
    }
}



/*
   static void log_printf (int level,const char *fmt, ...)
   {
   FILE *fp = level == LOG_NORMAL ? stdout : stderr;
   va_list ap;
   va_start (ap,fmt);
   vfprintf (fp,fmt,ap);
   va_end (ap);
   fflush (fp);
   }
 */


char * a_data		[MAX_DATA]	= { "5", "6", "7", "8" };
char * a_stop		[MAX_STOP]	= { "1", "2" };
char * a_parity		[MAX_PARITY]	= { "None", "Odd", "Even" };
char * a_flow		[MAX_FLOW]	= { "None", "Xon", "Rts" };

//=============================================================
unsigned char get_char_from_dps( unsigned char char_dps )
{
    unsigned char bit_mask = 3;
    char_dps &= bit_mask;
    return a_data[char_dps][0];
}

//=============================================================
unsigned char get_parity_from_dps( unsigned char parity_dps )
{
    unsigned char bit_mask = 3;
    parity_dps >>= 3;
    parity_dps &= bit_mask;
    return a_parity[ parity_dps ][0] ;
}

//=============================================================
unsigned char get_stop_from_dps( unsigned char stop_dps )
{
    unsigned char bit_mask = 1;
    stop_dps >>= 2;
    stop_dps &= bit_mask;
    return a_stop[ stop_dps ][0];
}

