#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <ctype.h>
#include "sb_include.h"
#include "sb_define.h"
#include "sb_config.h"
#include "sb_extern.h"

//#define DEBUG

//#define ENABLE_FILE_LOG
#define ENABLE_EM_LOG

#define IAC_CMD		0xff	// 255: Interpret as command

#define SERVER_DONT	0xfe	// 254: Indicates the demand that the other
							//		party stop performing, or confirmation that you
							//		are no longer expecting the other party to
							//		perform, the indicated option.
#define SERVER_DO	0xfd	// 253: Indicates the request that the other
							//		party perform, or confirmation that you are
							//		expecting the other party to
							//		perform, the indicated option.
#define SERVER_WONT 0xfc	// 252: Indicates the refusal to perform,
							//      or continue performing, the indicated option.
#define SERVER_WILL 0xfb	// 251: Indicates the desire to begin performing,
							//      or confirmation that you are now performing,
							//      the indicated option.

#define CLIENT_DONT 0xfe	// 254: Sender wants the other not to do something.
#define CLIENT_DO	0xfd	// 253: Sender wants the other end to do something.
#define CLIENT_WONT 0xfc	// 252: Sender doesn't want to do something.
#define CLIENT_WILL	0xfb	// 251: Sender wants to do something.

#define SB			0xfa 	// 250: Subnegotiation of the indicated option follows.
#define SE			0xf0	// 240: End of subnegotiation parameters.

#define CMD_ECHO	1	// Echo
#define CMD_SGA		3	// Suppress Go Ahead
#define CMD_NAWS	31	// Negotiate About Window Size

#define CMD_STAT_NONE	0
#define CMD_STAT_REPLY	1
#define CMD_STAT_ACK	2

int cmd_ECHO_status[2] = {CMD_STAT_NONE, CMD_STAT_NONE};
int cmd_SGA_status[2] = {CMD_STAT_NONE, CMD_STAT_NONE};

struct SB_SYSTEM_CONFIG CFG_SYS;
struct SB_SIO_CONFIG CFG_SERIAL[SB_MAX_SIO_PORT];

char ser2net_cmd[] = "monitor both";

void negotiate(int sock, int index, unsigned char *buf, int len)
{
#ifdef DEBUG
{
	int i;
	fprintf(stderr, "len=%d:", len);
	for(i = 0; i < len; i ++)
	{
		fprintf(stderr, "buf[%d]=0x%02x(%d),", i, buf[i], buf[i]);
	}
	fprintf(stderr, "\n");
}
#endif

	if (buf[1] == SERVER_WILL)
	{
		if(buf[2] == CMD_ECHO)
		{
			if(cmd_ECHO_status[index] != CMD_STAT_NONE)
			{
				cmd_ECHO_status[index] = CMD_STAT_ACK;
				return;
			}
			buf[1] = CLIENT_DO;
			cmd_ECHO_status[index] = CMD_STAT_REPLY;
		}
		else if(buf[2] == CMD_SGA)
		{
			if(cmd_SGA_status[index] != CMD_STAT_NONE)
			{
				cmd_SGA_status[index] = CMD_STAT_ACK;
				return;
			}
			buf[1] = CLIENT_DO;
			cmd_SGA_status[index] = CMD_STAT_REPLY;
		}
		else
		{
			buf[1] = CLIENT_DO;
		}
	}
	else if (buf[1] == SERVER_DO)
	{
		if(buf[2] == CMD_NAWS)
		{
			unsigned char tmp1[3] = {IAC_CMD, CLIENT_WILL, CMD_NAWS};
			if (send(sock, tmp1, 3 , 0) < 0)
				exit(1);

			unsigned char tmp2[9] = {IAC_CMD, SB, CMD_NAWS, 0, 80, 0, 24, IAC_CMD, SE};
			if (send(sock, tmp2, 9, 0) < 0)
				exit(1);
			return;
		}
		else
		{
			buf[1] = CLIENT_WONT;
		}
	}
	else if (buf[1] == SERVER_DONT)
	{
		return;
	}

#ifdef DEBUG
{
	int i;
	fprintf(stderr, "=>len=%d:", len);
	for(i = 0; i < len; i ++)
	{
		fprintf(stderr, "buf[%d]=0x%02x(%d),", i, buf[i], buf[i]);
	}
	fprintf(stderr, "\n");
}
#endif

	if (send(sock, buf, len , 0) < 0)
		exit(1);
}

#define BUFLEN 1024

int main(int argc , char *argv[])
{
	int sock_net;
	struct sockaddr_in server;
	struct hostent *host;
	int port;
	unsigned char buf[BUFLEN];
	int len;
	int portnum;
	int ret;
#if defined(ENABLE_FILE_LOG) || defined(ENABLE_EM_LOG)
	char log_file_path[] = "/tmp/";
	char log_file_name[] = "serial_monitor_";
#endif
#ifdef ENABLE_FILE_LOG
	char log_file_ext[] = ".txt";
	char log_file_full[sizeof(log_file_path) - 1 + sizeof(log_file_name) - 1 + sizeof(log_file_ext) - 1 + 2];
	FILE *fp_log;
#endif
#ifdef ENABLE_EM_LOG
	char log_em_full[sizeof(log_file_path) - 1 + sizeof(log_file_name) - 1 + 2];
	int fd_log;
#endif

	if (argc < 2 || argc > 2)
	{
		fprintf(stderr, "Usage: %s port_num\n", argv[0]);
		return 1;
	}
	portnum = atoi(argv[1]);
#ifdef DEBUG
	fprintf(stderr, "portnum = %d\n", portnum);
#endif

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
		fprintf(stderr, "Serial port %d is disabled !\n", portnum);
		return 0;
	}

	host = gethostbyname("localhost");
	if(host == NULL)
	{
		perror("Could find host. Error");
		return 1;
	}
#ifdef DEBUG
	fprintf(stderr, "ser2net control IP = localhost\n");
#endif

	if( CFG_SERIAL[portnum-1].protocol == SB_TCP_SERVER_MODE
		||
		CFG_SERIAL[portnum-1].protocol == SB_TCP_BROADCAST_MODE
		||
		CFG_SERIAL[portnum-1].protocol == SB_TCP_MULTIPLEX_MODE
		||
		CFG_SERIAL[portnum-1].protocol == SB_UDP_SERVER_MODE
		)
	{
		port = CFG_SERIAL[portnum-1].local_port + 10000;
	}
	else if(
		// For UDP Client, Needs TCP Server first.
		CFG_SERIAL[portnum-1].protocol == SB_UDP_CLIENT_MODE
		||
		// For TCP Client, Needs UDP Server first.
		CFG_SERIAL[portnum-1].protocol == SB_TCP_CLIENT_MODE
		)
	{
		port = CFG_SERIAL[portnum-1].remote_port + 10000;
	}
#ifdef DEBUG
	fprintf(stderr, "ser2net control port = %d\n", port);
#endif

	// copy the network address to sockaddr_in structure
	//memcpy(&server.sin_addr, host->h_addr, host->h_length);
	//server.sin_addr.s_addr = *(struct in_addr*)(host->h_addr);
	server.sin_addr = *(struct in_addr *)host->h_addr;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	//Create socket
	sock_net = socket(AF_INET , SOCK_STREAM , 0);
	if (sock_net == -1)
	{
		perror("Could not create sock_net socket. Error");
		return 1;
	}

	//Connect to remote server
	if (connect(sock_net , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		close(sock_net);
		perror("Connect sock_net failed. Error");
		return 1;
	}
#ifdef DEBUG
	fprintf(stderr, "Connected to controller net ...\n");
#endif

#ifdef ENABLE_FILE_LOG
	sprintf(log_file_full, "%s%s%1d%s", log_file_path, log_file_name, portnum, log_file_ext);
#ifdef DEBUG
	fprintf(stderr, "Log file name = %s\n", log_file_full);
#endif
#endif
#ifdef ENABLE_EM_LOG
	sprintf(log_em_full, "%s%s%1d", log_file_path, log_file_name, portnum);
#ifdef DEBUG
	fprintf(stderr, "Log em name = %s\n", log_em_full);
#endif
	sprintf((char *)buf, "/bin/rm -f %s", log_em_full);
	system((char *)buf);
	sprintf((char *)buf, "/usr/bin/mkemlog %s 64 0666", log_em_full);
	system((char *)buf);
	sprintf((char *)buf, "/bin/chmod +r %s", log_em_full);
	system((char *)buf);
#endif

	struct timeval ts;
	ts.tv_sec = 1; // 1 second
	ts.tv_usec = 0;
 
	while (1)
	{
		int nready;
		// select setup
		fd_set fds_net;
		FD_ZERO(&fds_net);
		if (sock_net != 0)
			FD_SET(sock_net, &fds_net);
		FD_SET(0, &fds_net);

		// wait for data
		nready = select(sock_net + 1, &fds_net, (fd_set *) 0, (fd_set *) 0, &ts);
		if (nready < 0)
		{
			close(sock_net);
			perror("select. Error");
			return 1;
		}
		else if (nready == 0)
		{
			ts.tv_sec = 1; // 1 second
			ts.tv_usec = 0;
		}
		else if (sock_net != 0 && FD_ISSET(sock_net, &fds_net))
		{
			//if( cmd_ECHO_status[0] < CMD_STAT_ACK ||
			//    cmd_SGA_status[0] < CMD_STAT_ACK)
			if( cmd_ECHO_status[0] < CMD_STAT_REPLY ||
			    cmd_SGA_status[0] < CMD_STAT_REPLY)
			{
				// start by reading a single byte
				int rv;
				if ((rv = recv(sock_net , buf , 1 , 0)) < 0)
				{
					close(sock_net);
					return 1;
				}
				else if (rv == 0)
				{
					close(sock_net);
					fprintf(stderr, "Connection closed by the remote end\n\r");
					return 0;
				}

				if (buf[0] == IAC_CMD)
				{
					// read 2 more bytes
					len = recv(sock_net , buf + 1 , 2 , 0);
					if (len  < 0)
					{
						close(sock_net);
						perror("Could not recv. Error");
						return 1;
					}
					else if (len == 0)
					{
						close(sock_net);
						fprintf(stderr, "Connection closed by the remote end\n\r");
						return 0;
					}
					negotiate(sock_net, 0, buf, 3);
					//if( cmd_ECHO_status[0] >= CMD_STAT_ACK &&
					//    cmd_SGA_status[0] >= CMD_STAT_ACK)
					if( cmd_ECHO_status[0] >= CMD_STAT_REPLY &&
					    cmd_SGA_status[0] >= CMD_STAT_REPLY)
					{
						char cmd[sizeof(ser2net_cmd) + 1];
#ifdef DEBUG
						fprintf(stderr, "#1 Send ser2net_cmd = %s\n", ser2net_cmd);
#endif

						sprintf(cmd, "%s\r", ser2net_cmd);
						send(sock_net, cmd, strlen(cmd) , 0);
#if 0
						len = recv(sock_net , buf , BUFLEN - 1 , 0);
						if(len < 0)
						{
							close(sock_net);
							perror("Could not recv. Error");
							return -1;
						}
						else if(len == 0)
						{
							close(sock_net);
							perror("Connection closed by the remote end");
							return 0;
						}

						buf[len] = 0;
						char *str_s = strstr((char *)buf, ser2net_cmd);
						if(str_s != NULL)
						{
							str_s += strlen(ser2net_cmd) + 2;
							char *str_e = strstr(str_s, "\r\n");
							if(str_e != NULL)
							{
							}
						}
#endif
					}
				}
				else
				{
#ifdef DEBUG
					fprintf(stderr, "skip:0x%02x(%d)[%c]\n", buf[0], buf[0], isprint(buf[0])?buf[0]:' ');
#endif
/*
					// read more bytes
					len = recv(sock_net , buf + 1 , BUFLEN - 2 , 0);
					buf[len] = 0;
					fprintf(stderr, "%s", buf);

					cmd_ECHO_status[0] = cmd_SGA_status[0] = CMD_STAT_ACK;

					char cmd[sizeof(ser2net_cmd) + 1];
					fprintf(stderr, "#2 Send ser2net_cmd = %s\n", ser2net_cmd);
					
					sprintf(cmd, "%s\r", ser2net_cmd);
						send(sock_net, cmd, strlen(cmd) , 0);
*/
				}
			}
			else
			{
				len = recv(sock_net , buf , BUFLEN - 1 , 0);
				if(len < 0)
				{
					close(sock_net);
					perror("Could not recv. Error");
					return -1;
				}
				else if (len == 0)
				{
					close(sock_net);
					perror("Connection closed by the remote end");
					return 0;
				}

#if 0
				buf[len] = 0;
				char *str_s = strstr((char *)buf, ser2net_cmd);
				if( str_s != NULL )
				{
					str_s += strlen(ser2net_cmd) + 2;
					char *str_e = strstr(str_s, "\r\n");
					if( str_e != NULL )
					{
#ifdef ENABLE_FILE_LOG
						*str_e = '\0';
						fp_log = fopen(log_file_full, "w");
						if( fp_log == NULL )
						{
							perror("Create file failed. Error");
							return 1;
						}
						fprintf(fp_log, "%s", str_s);
						fflush(fp_log);
						fclose(fp_log);
#endif
#ifdef ENABLE_EM_LOG
						*str_e++ = '\n';
						*str_e = '\0';
						fd_log = open(log_em_full, O_WRONLY);
						if( fd_log == -1 )
						{
							perror("Could not open of emlog device");
							return 1;
						}
						write(fd_log, str_s, strlen(str_s));
						close(fd_log);
#endif
						//fprintf(stderr, "%s\n", str_s);
						sleep(3); // sleep 3secs

						char cmd[sizeof(ser2net_cmd) + 1];
						//fprintf(stderr, "#3 Send ser2net_cmd = %s\n", ser2net_cmd);

						sprintf(cmd, "%s\r", ser2net_cmd);
						send(sock_net, cmd, strlen(cmd) , 0);
						
					}
				}
#endif
#if 0
				/**/
				int i;
				fprintf(stderr, "recv len=%d:", len);
				for(i = 0; i < len; i ++)
				{
					fprintf(stderr, "0x%02x(%d)[%c],", buf[i], buf[i], isprint(buf[i])?buf[i]:' ');
				}
				fprintf(stderr, "\n\n");
				/**/
#endif
#if 1
#ifdef ENABLE_EM_LOG
				fd_log = open(log_em_full, O_WRONLY);
				if( fd_log == -1 )
				{
					perror("Could not open of emlog device");
					return 1;
				}
				write(fd_log, buf, len);
				close(fd_log);
#endif
#ifdef DEBUG
				buf[len] = 0;
				fprintf(stderr, "%s", buf);
				//break;
				//fflush(0);
#endif
#endif
#if 0
				buf[len] = 0;
				printf("len=%d,buf=0x%x\n", len, (int)buf);
				int offset = 0;
				do
				{
					unsigned char *ret = (unsigned char *)strchr((char *)buf + offset, '\r');
					//printf("ret=0x%x\n", (int)ret);
					if(ret == NULL)
					{
						printf("len=%d - offset=%d = %d\n", len, offset, len - offset);
						fprintf(stderr, "%.*s", len - offset, buf + offset);
						break;
					}
					printf("len=%d\n", (int)(ret - buf) - offset + 1);
					fprintf(stderr, "%.*s\n", (int)(ret - buf) - offset + 1, buf + offset);
					offset = (int)(ret - buf) - offset + 1;
				} while(1);
				//fflush(0);
#endif
			}
		}
	}
	close(sock_net);
	return 0;
}

