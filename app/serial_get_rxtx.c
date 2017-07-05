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

char ser2net_cmd[] = "showdevrxtx";

void negotiate(int sock, int index, unsigned char *buf, int len)
{
/**/
{
	int i;
	fprintf(stderr, "len=%d:", len);
	for(i = 0; i < len; i ++)
	{
		fprintf(stderr, "buf[%d]=0x%02x(%d),", i, buf[i], buf[i]);
	}
	fprintf(stderr, "\n");
}
/**/

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
	else if (buf[1] == CLIENT_DONT)
	{
		return;
	}

/**/
{
	int i;
	fprintf(stderr, "=>len=%d:", len);
	for(i = 0; i < len; i ++)
	{
		fprintf(stderr, "buf[%d]=0x%02x(%d),", i, buf[i], buf[i]);
	}
	fprintf(stderr, "\n");
}
/**/
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
	char log_file_path[] = "/tmp/";
	char log_file_name[] = "serial_get_rxtx_";
	char log_file_ext[] = ".txt";
	char log_file_full[sizeof(log_file_path) - 1 + sizeof(log_file_name) - 1 + sizeof(log_file_ext) - 1 + 2];
	FILE *fp_log;

	if (argc < 2 || argc > 2)
	{
		fprintf(stderr, "Usage: %s port_num\n", argv[0]);
		return 1;
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
		return 0;
	}

	host = gethostbyname("localhost");
	if(host == NULL)
	{
		perror("Could find host. Error");
		return 1;
	}
	fprintf(stderr, "ser2net control IP = localhost\n");

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
	fprintf(stderr, "ser2net control port = %d\n", port);

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
	fprintf(stderr, "Connected to controller net ...\n");

	sprintf(log_file_full, "%s%s%1d%s", log_file_path, log_file_name, portnum, log_file_ext);
	fprintf(stderr, "Log file name = %s\n", log_file_full);
	fp_log = fopen(log_file_full, "w");
	if( fp_log == NULL )
	{
		close(sock_net);
		perror("Create file failed. Error");
		return 1;
	}

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
			fclose(fp_log);
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
			if( cmd_ECHO_status[0] != CMD_STAT_ACK ||
			    cmd_ECHO_status[0] != CMD_STAT_ACK)
			{
				// start by reading a single byte
				int rv;
				if ((rv = recv(sock_net , buf , 1 , 0)) < 0)
				{
					fclose(fp_log);
					close(sock_net);
					return 1;
				}
				else if (rv == 0)
				{
					fclose(fp_log);
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
						fclose(fp_log);
						close(sock_net);
						perror("Could not recv. Error");
						return 1;
					}
					else if (len == 0)
					{
						fclose(fp_log);
						close(sock_net);
						fprintf(stderr, "Connection closed by the remote end\n\r");
						return 0;
					}
					negotiate(sock_net, 0, buf, 3);
					if( cmd_ECHO_status[0] == CMD_STAT_ACK &&
					    cmd_ECHO_status[0] == CMD_STAT_ACK)
					{
						char cmd[sizeof(ser2net_cmd) + 1];
						fprintf(stderr, "Send ser2net_cmd = %s\n", ser2net_cmd);
						
						sprintf(cmd, "%s\r", ser2net_cmd);
						send(sock_net, cmd, strlen(cmd) , 0);
					}
				}
			}
			else
			{
				len = recv(sock_net , buf , BUFLEN - 1 , 0);
				if(len < 0)
				{
					fclose(fp_log);
					close(sock_net);
					perror("Could not recv. Error");
					return -1;
				}
				else if (len == 0)
				{
					fclose(fp_log);
					close(sock_net);
					perror("Connection closed by the remote end");
					return 0;
				}

#if 1
				buf[len] = 0;
				char *str_s = strstr((char *)buf, ser2net_cmd);
				if( str_s != NULL )
				{
					str_s += strlen(ser2net_cmd) + 2;
					char *str_e = strstr(str_s, "\r\n");
					if( str_e != NULL )
					{
						*str_e = '\0';
						fprintf(fp_log, "%s", str_s);
						fprintf(stderr, "%s\n", str_s);
						break;
					}
				}
#endif
#if 1
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
				buf[len] = 0;
				fprintf(stderr, "%s", buf);
				break;
				//fflush(0);
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
	fclose(fp_log);
	close(sock_net);
	return 0;
}

