#include "sb_include.h"
#include "sb_define.h"
#include "sb_config.h"
#include "sb_extern.h"

struct SB_SYSTEM_CONFIG CFG_SYS;
struct SB_SIO_CONFIG CFG_SERIAL[SB_MAX_SIO_PORT];

static int ser2net_get_baudrate(int portnum)
{
	switch(CFG_SERIAL[portnum-1].speed)
	{
		case SB_BAUDRATE_150:		return 150;
		case SB_BAUDRATE_300:		return 300;
		case SB_BAUDRATE_600:		return 600;
		case SB_BAUDRATE_1200:		return 1200;
		case SB_BAUDRATE_2400:		return 2400;
		case SB_BAUDRATE_4800:		return 4800;
		case SB_BAUDRATE_9600:		return 9600;
		case SB_BAUDRATE_19200:		return 19200;
		case SB_BAUDRATE_38400:		return 38400;
		case SB_BAUDRATE_57600:		return 57600;
		case SB_BAUDRATE_115200:	return 115200;
		case SB_BAUDRATE_230400:	return 230400;
		case SB_BAUDRATE_460800:	return 460800;
		case SB_BAUDRATE_921600:	return 921600;
		default:					break;;
	}
	return 115200;
}

static char *ser2net_get_parity(int portnum)
{
	switch((CFG_SERIAL[portnum-1].dps & SB_PARITY_MASK) >> SB_PARITY_SHIFT)
	{
		case SB_PARITY_ODD:
			return "ODD";
		case SB_PARITY_EVEN:
			return "EVEN";
		case SB_PARITY_NONE:
			return "NONE";
		default:
			break;
	}
	return "NONE";
}

static char *ser2net_get_stopbits(int portnum)
{
	switch((CFG_SERIAL[portnum-1].dps & SB_STOPBITS_MASK) >> SB_STOPBITS_SHIFT)
	{
		case SB_STOPBITS_2:
			return "2STOPBITS";
		case SB_STOPBITS_1:
			return "1STOPBIT";
		default:
			break;
	}
	return "1STOPBIT";
}

static char *ser2net_get_databits(int portnum)
{
	switch((CFG_SERIAL[portnum-1].dps & SB_DATABITS_MASK) >> SB_DATABITS_SHIFT)
	{
		case SB_DATABITS_5:
			return "5DATABITS";
		case SB_DATABITS_6:
			return "6DATABITS";
		case SB_DATABITS_7:
			return "7DATABITS";
		case SB_DATABITS_8:
			return "8DATABITS";
		default:
			break;
	}
	return "8DATABITS";
}

static void disable(int portnum)
{
	char cmd[1000];	
	char sysd_filename[256];
	char sysd_filepath[256];

	sprintf(sysd_filename, "ttyO%d.service", portnum);
	sprintf(sysd_filepath, "/lib/systemd/system/%s", sysd_filename);

	sprintf (cmd, "/bin/systemctl stop %s", sysd_filename);
	fprintf(stderr, "system:%s\n", cmd);
	system (cmd);

	sprintf (cmd, "/bin/systemctl disable %s", sysd_filename);
	fprintf(stderr, "system:%s\n", cmd);
	system (cmd);
}

//---------------------------------------------------------------
int main(int argc, char *argv[])
{
	char cmd[1000];	
	char conf_filename[256];
	char conf_filepath[256];
	char sysd_filename[256];
	char sysd_filepath[256];
	char sysd_desc[256];
	char sysd_exec[256];
	int ret;
	FILE *fp;
	int portnum;

	fprintf(stderr, "%s:%s\n", argv[0], argv[1]);
	if(argc < 2)
	{
		fprintf(stderr, "Usage: %s port_num\n", argv[0]);
		return -1;
	}
	portnum = atoi(argv[1]);
	fprintf(stderr, "portnum = %d\n", portnum);

	ret = SB_ReadConfig(CFGFILE_ETC_SYSTEM, (char *)&CFG_SYS, sizeof (struct SB_SYSTEM_CONFIG));
	if (ret < 0 || strncmp(SB_DEVICE_ID, CFG_SYS.id, 4))	// CFG not found or ID mismatch
	{
		fprintf(stderr, "CFG_SYS not found or ID mismatch! %d, %4s !\n", ret, CFG_SYS.id);
		return -1;
	}
	ret = SB_ReadConfig(CFGFILE_ETC_SIO, (char *)&CFG_SERIAL[0], sizeof(struct SB_SIO_CONFIG)*SB_MAX_SIO_PORT);
	if (ret < 0)	// CFG_SERIAL not found
	{
		fprintf(stderr, "CFG_SERIAL not found! %d!\n", ret);
		return -1;
	}

	if( CFG_SERIAL[portnum-1].protocol == SB_DISABLE_MODE )
	{
		disable(portnum);
		return 0;
	}

	if( CFG_SERIAL[portnum-1].protocol == SB_TCP_SERVER_MODE
		||
		CFG_SERIAL[portnum-1].protocol == SB_TCP_BROADCAST_MODE
		||
		CFG_SERIAL[portnum-1].protocol == SB_TCP_MULTIPLEX_MODE
		)
	{
		sprintf(conf_filename, "ser2net_ttyO%d.conf", portnum);
		sprintf(conf_filepath, "/etc/%s", conf_filename);
		if ((fp = fopen(conf_filepath, "w")) == NULL)
		{
			fprintf(stderr, "fopen error %s\n", conf_filepath);
			return -1;
		}
		fprintf(fp, "ipv4");
		fprintf(fp, ",");
		fprintf(fp, "tcp");
		fprintf(fp, ",");
		fprintf(fp, "%d", CFG_SERIAL[portnum-1].local_port);
		fprintf(fp, ":");
		fprintf(fp, "raw");
		fprintf(fp, ":");
		fprintf(fp, "0");
		fprintf(fp, ":");
		fprintf(fp, "/dev/ttyO%d", portnum);
		fprintf(fp, ":");
		fprintf(fp, "%d", ser2net_get_baudrate(portnum));
		fprintf(fp, " ");
		fprintf(fp, ser2net_get_parity(portnum));
		fprintf(fp, " ");
		fprintf(fp, ser2net_get_stopbits(portnum));
		fprintf(fp, " ");
		fprintf(fp, ser2net_get_databits(portnum));
		fprintf(fp, " ");
		fprintf(fp, "max-connections=10");
		fprintf(fp, "\n");
		fflush(fp);
		fclose(fp);
		sprintf(sysd_desc, "Relay Serial /dev/ttyO%d to TCP Server", portnum);
		sprintf(sysd_exec, "/usr/local/sbin/ser2net -n -c %s", conf_filepath);
	}
	else if( CFG_SERIAL[portnum-1].protocol == SB_UDP_SERVER_MODE )
	{
		sprintf(conf_filename, "ser2net_ttyO%d.conf", portnum);
		sprintf(conf_filepath, "/etc/%s", conf_filename);
		if ((fp = fopen(conf_filepath, "w")) == NULL)
		{
			fprintf(stderr, "fopen error %s\n", conf_filepath);
			return -1;
		}
		fprintf(fp, "ipv4");
		fprintf(fp, ",");
		fprintf(fp, "udp");
		fprintf(fp, ",");
		fprintf(fp, "%d", CFG_SERIAL[portnum-1].local_port);
		fprintf(fp, ":");
		fprintf(fp, "raw");
		fprintf(fp, ":");
		fprintf(fp, "0");
		fprintf(fp, ":");
		fprintf(fp, "/dev/ttyO%d", portnum);
		fprintf(fp, ":");
		fprintf(fp, "%d", ser2net_get_baudrate(portnum));
		fprintf(fp, " ");
		fprintf(fp, ser2net_get_parity(portnum));
		fprintf(fp, " ");
		fprintf(fp, ser2net_get_stopbits(portnum));
		fprintf(fp, " ");
		fprintf(fp, ser2net_get_databits(portnum));
		fprintf(fp, " ");
		fprintf(fp, "max-connections=10");
		fprintf(fp, "\n");
		fflush(fp);
		fclose(fp);
		sprintf(sysd_desc, "Relay Serial /dev/ttyO%d to UDP Server", portnum);
		sprintf(sysd_exec, "/usr/local/sbin/ser2net -n -c %s", conf_filepath);
	}
	else if( CFG_SERIAL[portnum-1].protocol == SB_TCP_CLIENT_MODE
		||
		CFG_SERIAL[portnum-1].protocol == SB_UDP_CLIENT_MODE
		)
	{
		int baudrate;
		sprintf(conf_filename, "socat_ttyO%d.sh", portnum);
		sprintf(conf_filepath, "/etc/%s", conf_filename);
//		touch('/file/path/here');
//		chmod('/file/path/here', 0775);
		if ((fp = fopen(conf_filepath, "w")) == NULL)
		{
			fprintf(stderr, "fopen error %s\n", conf_filepath);
			return -1;
		}
		fprintf(fp, "#! /bin/sh\n");
		fprintf(fp, "/usr/bin/socat");
		fprintf(fp, " ");
		fprintf(fp, "/dev/ttyO%d", portnum);
		fprintf(fp, ",");
		switch(CFG_SERIAL[portnum-1].speed)
		{
			case SB_BAUDRATE_150:		baudrate = 150; break;
			case SB_BAUDRATE_300:		baudrate = 300; break;
			case SB_BAUDRATE_600:		baudrate = 600; break;
			case SB_BAUDRATE_1200:		baudrate = 1200; break;
			case SB_BAUDRATE_2400:		baudrate = 2400; break;
			case SB_BAUDRATE_4800:		baudrate = 4800; break;
			case SB_BAUDRATE_9600:		baudrate = 9600; break;
			case SB_BAUDRATE_19200:		baudrate = 19200; break;
			case SB_BAUDRATE_38400:		baudrate = 38400; break;
			case SB_BAUDRATE_57600:		baudrate = 57600; break;
			case SB_BAUDRATE_115200:	baudrate = 115200; break;
			case SB_BAUDRATE_230400:	baudrate = 230400; break;
			case SB_BAUDRATE_460800:	baudrate = 460800; break;
			case SB_BAUDRATE_921600:	baudrate = 921600; break;
			default:					baudrate = 115200; break;
		}
		fprintf(fp, "b%d", baudrate);
		fprintf(fp, ",");
		switch((CFG_SERIAL[portnum-1].dps & SB_DATABITS_MASK) >> SB_DATABITS_SHIFT)
		{
			case SB_DATABITS_5:
				fprintf(fp, "cs5");
				break;
			case SB_DATABITS_6:
				fprintf(fp, "cs6");
				break;
			case SB_DATABITS_7:
				fprintf(fp, "cs7");
				break;
			case SB_DATABITS_8:
			default:
				fprintf(fp, "cs8");
				break;
		}
		fprintf(fp, ",");
		switch((CFG_SERIAL[portnum-1].dps & SB_PARITY_MASK) >> SB_PARITY_SHIFT)
		{
			case SB_PARITY_ODD:
				fprintf(fp, "parenb=1,parodd=1");
				break;
			case SB_PARITY_EVEN:
				fprintf(fp, "parenb=1,parodd=0");
				break;
			case SB_PARITY_NONE:
			default:
				fprintf(fp, "parenb=0");
				break;
		}
		fprintf(fp, ",");
		switch((CFG_SERIAL[portnum-1].dps & SB_STOPBITS_MASK) >> SB_STOPBITS_SHIFT)
		{
			case SB_STOPBITS_2:
				fprintf(fp, "cstopb=1");
				break;
			case SB_STOPBITS_1:
			default:
				fprintf(fp, "cstopb=0");
				break;
		}
		fprintf(fp, ",raw,echo=0,icanon=0");
		fprintf(fp, " ");
		if( CFG_SERIAL[portnum-1].protocol == SB_TCP_CLIENT_MODE )
			fprintf(fp, "TCP");
		else // if( CFG_SERIAL[portnum-1].protocol == SB_UDP_CLIENT_MODE )
			fprintf(fp, "UDP");
		fprintf(fp, ":");
		fprintf(fp, "%d.%d.%d.%d", CFG_SERIAL[portnum-1].remote_ip[0], CFG_SERIAL[portnum-1].remote_ip[1], CFG_SERIAL[portnum-1].remote_ip[2], CFG_SERIAL[portnum-1].remote_ip[3]);
		fprintf(fp, ":");
		fprintf(fp, "%d", CFG_SERIAL[portnum-1].remote_port);
		fprintf(fp, ",");
		if( CFG_SERIAL[portnum-1].protocol == SB_TCP_CLIENT_MODE )
			fprintf(fp, "forever");
		// option "forever" not support with UDP
		/* 
 		else // if( CFG_SERIAL[portnum-1].protocol == SB_UDP_CLIENT_MODE )
			fprintf(fp, "forever");
		*/
		fprintf(fp, "\n");
		fprintf(fp, "exit $?");
		fprintf(fp, "\n");
		fflush(fp);
		fclose(fp);

		sprintf (cmd, "/bin/chmod 744 %s", conf_filepath);
		fprintf(stderr, "system:%s\n", cmd);
		system (cmd);

		if( CFG_SERIAL[portnum-1].protocol == SB_TCP_CLIENT_MODE )
			sprintf(sysd_desc, "Relay Serial /dev/ttyO%d to TCP Client %d.%d.%d.%d(%d)", portnum, CFG_SERIAL[portnum-1].remote_ip[0], CFG_SERIAL[portnum-1].remote_ip[1], CFG_SERIAL[portnum-1].remote_ip[2], CFG_SERIAL[portnum-1].remote_ip[3], CFG_SERIAL[portnum-1].remote_port);
		else // if( CFG_SERIAL[portnum-1].protocol == SB_UDP_CLIENT_MODE )
			sprintf(sysd_desc, "Relay Serial /dev/ttyO%d to UDP Client %d.%d.%d.%d(%d)", portnum, CFG_SERIAL[portnum-1].remote_ip[0], CFG_SERIAL[portnum-1].remote_ip[1], CFG_SERIAL[portnum-1].remote_ip[2], CFG_SERIAL[portnum-1].remote_ip[3], CFG_SERIAL[portnum-1].remote_port);
		sprintf(sysd_exec, "%s", conf_filepath);
	}

	sprintf(sysd_filename, "ttyO%d.service", portnum);
	sprintf(sysd_filepath, "/lib/systemd/system/%s", sysd_filename);

	sprintf (cmd, "/bin/systemctl stop %s", sysd_filename);
	fprintf(stderr, "system:%s\n", cmd);
	system (cmd);

	sprintf (cmd, "/bin/systemctl disable %s", sysd_filename);
	fprintf(stderr, "system:%s\n", cmd);
	system (cmd);

	if ((fp = fopen(sysd_filepath, "w")) != NULL)
	{
		if( CFG_SERIAL[portnum-1].protocol == SB_TCP_SERVER_MODE
			||
			CFG_SERIAL[portnum-1].protocol == SB_TCP_BROADCAST_MODE
			||
			CFG_SERIAL[portnum-1].protocol == SB_TCP_MULTIPLEX_MODE
			||
			CFG_SERIAL[portnum-1].protocol == SB_UDP_SERVER_MODE
			)
		{
			fprintf(fp, "[Unit]\n");
			fprintf(fp, "Description=%s\n", sysd_desc);
			fprintf(fp, "After=syslog.target\n");
			fprintf(fp, "\n");
			fprintf(fp, "[Service]\n");
			fprintf(fp, "ExecStart=%s\n", sysd_exec);
			fprintf(fp, "\n");
			fprintf(fp, "[Install]\n");
			fprintf(fp, "WantedBy=multi-user.target\n");
			fprintf(fp, "Alias=%s\n", sysd_filename);
			fprintf(fp, "\n");
		}
		else if( CFG_SERIAL[portnum-1].protocol == SB_TCP_CLIENT_MODE
			||
			CFG_SERIAL[portnum-1].protocol == SB_UDP_CLIENT_MODE
			)
		{
			fprintf(fp, "[Unit]\n");
			fprintf(fp, "Description=%s\n", sysd_desc);
			fprintf(fp, "After=syslog.target\n");
			fprintf(fp, "StartLimitIntervalSec=0\n");
			fprintf(fp, "\n");
			fprintf(fp, "[Service]\n");
			fprintf(fp, "ExecStart=%s\n", sysd_exec);
			fprintf(fp, "KillMode=mixed\n");
			fprintf(fp, "RestartSec=1\n");
			fprintf(fp, "Restart=always\n");
			fprintf(fp, "\n");
			fprintf(fp, "[Install]\n");
			fprintf(fp, "WantedBy=multi-user.target\n");
			fprintf(fp, "Alias=%s\n", sysd_filename);
			fprintf(fp, "\n");
		}

		fflush(fp);
		fclose(fp);
	}
	else
	{
		fprintf(stderr, "fopen error %s\n", sysd_filepath);
	}

	sprintf (cmd, "/bin/systemctl enable %s", sysd_filename);
	fprintf(stderr, "system:%s\n", cmd);
	system (cmd);

	sprintf (cmd, "/bin/systemctl start %s", sysd_filename);
	fprintf(stderr, "system:%s\n", cmd);
	system (cmd);

	return 0;
}

