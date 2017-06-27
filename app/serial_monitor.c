#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
 
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

void negotiate(int sock, int index, unsigned char *buf, int len)
{
/*
	fprintf(stderr, "len=%d:", len);
	for(int i = 0; i < len; i ++)
	{
		fprintf(stderr, "buf[%d]=0x%02x(%d),", i, buf[i], buf[i]);
	}
	fprintf(stderr, "\n");
*/

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

/*
	fprintf(stderr, "=>len=%d:", len);
	for(int i = 0; i < len; i ++)
	{
		fprintf(stderr, "buf[%d]=0x%02x(%d),", i, buf[i], buf[i]);
	}
	fprintf(stderr, "\n");
*/
	if (send(sock, buf, len , 0) < 0)
		exit(1);
}

#define BUFLEN 1024

int main(int argc , char *argv[])
{
	int sock_net;
	int sock_term;
	struct sockaddr_in server;
	struct hostent *host;
	unsigned char buf[BUFLEN];
	int len;

	if (argc < 2 || argc > 3)
	{
		fprintf(stderr, "Usage: %s address [port]\n", argv[0]);
		return 1;
	}
	int port = 23;
	if (argc == 3)
	port = atoi(argv[2]);

	host = gethostbyname(argv[1]);
	if(host == NULL)
	{
		perror("Could find host. Error");
		return 1;
	}

	// copy the network address to sockaddr_in structure
	//memcpy(&server.sin_addr, host->h_addr, host->h_length);
	//server.sin_addr.s_addr = *(struct in_addr*)(host->h_addr);
	server.sin_addr = *(struct in_addr *)host->h_addr;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

#if 1
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
		perror("Connect sock_net failed. Error");
		return 1;
	}
	fprintf(stderr, "Connected to controller net ...\n");
#endif
#if 0
	//Create socket
	sock_term = socket(AF_INET , SOCK_STREAM , 0);
	if (sock_net == -1)
	{
		perror("Could not create sock_term socket. Error");
		return 1;
	}

	//Connect to remote server
	if (connect(sock_term , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("Connect sock_term failed. Error");
		return 1;
	}
	fprintf(stderr, "Connected to controller term ...\n");
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
			if(	cmd_ECHO_status[0] != CMD_STAT_ACK ||
				cmd_ECHO_status[0] != CMD_STAT_ACK)
			{
				// start by reading a single byte
				int rv;
				if ((rv = recv(sock_net , buf , 1 , 0)) < 0)
					return 1;
				else if (rv == 0)
				{
					fprintf(stderr, "Connection closed by the remote end\n\r");
					return 0;
				}

				if (buf[0] == IAC_CMD)
				{
					// read 2 more bytes
					len = recv(sock_net , buf + 1 , 2 , 0);
					if (len  < 0)
					{
						perror("Could not recv. Error");
						return 1;
					}
					else if (len == 0)
					{
						fprintf(stderr, "Connection closed by the remote end\n\r");
						return 0;
					}
					negotiate(sock_net, 0, buf, 3);
					if(	cmd_ECHO_status[0] == CMD_STAT_ACK &&
						cmd_ECHO_status[0] == CMD_STAT_ACK)
					{
						char cmd[] = "monitor both ipv4,tcp,4001\r";
						//char cmd[] = "help\r";
						/*
						fprintf(stderr, ">>>len=%d:", strlen(cmd));
						for(int i = 0; i < strlen(cmd); i ++)
						{
							fprintf(stderr, "cmd[%d]=0x%02x(%d),", i, cmd[i], cmd[i]);
						}
						fprintf(stderr, "\n");
						*/
						send(sock_net, cmd, strlen(cmd) , 0);
					}
				}
			}
			else
			{
				int offset;
				len = recv(sock_net , buf , BUFLEN - 1 , 0);
				if(len < 0)
				{
					perror("Could not recv. Error");
					return -1;
				}
				else if (len == 0)
				{
					perror("Connection closed by the remote end");
					return 0;
				}
				buf[len] = 0;
				//fprintf(stderr, "%s\r\n", buf);
				//fflush(0);
				//printf("len=%d,buf=0x%x\n", len, (int)buf);
				offset = 0;
				do
				{
					unsigned char *ret = (unsigned char *)strchr((char *)buf + offset, '\r');
					//printf("ret=0x%x\n", (int)ret);
					if(ret == NULL)
					{
						//printf("len=%d - offset=%d = %d\n", len, offset, len - offset);
						fprintf(stderr, "%.*s", len - offset, buf + offset);
						break;
					}
					//printf("len=%d\n", (int)(ret - buf) - offset + 1);
					fprintf(stderr, "%.*s\n", (int)(ret - buf) - offset + 1, buf + offset);
					offset = (int)(ret - buf) - offset + 1;
				} while(1);
				//fflush(0);
			}
		}
#if 0
		// select setup
		fd_set fds_term;
		FD_ZERO(&fds_term);
		if (sock_term != 0)
			FD_SET(sock_term, &fds_term);
		FD_SET(0, &fds_term);

		// wait for data
		nready = select(sock_term + 1, &fds_term, (fd_set *) 0, (fd_set *) 0, &ts);
		if (nready < 0)
		{
			perror("select. Error");
			return 1;
		}
		else if (nready == 0)
		{
			ts.tv_sec = 1; // 1 second
			ts.tv_usec = 0;
		}
		else if (sock_term != 0 && FD_ISSET(sock_term, &fds_term))
		{
			if(	cmd_ECHO_status[1] != CMD_STAT_ACK ||
				cmd_ECHO_status[1] != CMD_STAT_ACK)
			{
				// start by reading a single byte
				int rv;
				if ((rv = recv(sock_term , buf , 1 , 0)) < 0)
					return 1;
				else if (rv == 0)
				{
					fprintf(stderr, "Connection closed by the remote end\n\r");
					return 0;
				}

				if (buf[0] == IAC_CMD)
				{
					// read 2 more bytes
					len = recv(sock_term , buf + 1 , 2 , 0);
					if (len  < 0)
					{
						perror("Could not recv. Error");
						return 1;
					}
					else if (len == 0)
					{
						fprintf(stderr, "Connection closed by the remote end\n\r");
						return 0;
					}
					negotiate(sock_term, 1, buf, 3);
					if(	cmd_ECHO_status[1] == CMD_STAT_ACK &&
						cmd_ECHO_status[1] == CMD_STAT_ACK)
					{
						char cmd[] = "monitor term ipv4,tcp,4001\r";
						//char cmd[] = "help\r";
						/*
						fprintf(stderr, ">>>len=%d:", strlen(cmd));
						for(i = 0; i < strlen(cmd); i ++)
						{
							fprintf(stderr, "cmd[%d]=0x%02x(%d),", i, cmd[i], cmd[i]);
						}
						fprintf(stderr, "\n");
						*/
						send(sock_term, cmd, strlen(cmd) , 0);
					}
				}
			}
			else
			{
				len = recv(sock_term , buf , BUFLEN - 1 , 0);
				if(len < 0)
				{
					perror("Could not recv. Error");
					return 1;
				}
				else if (len == 0)
				{
					perror("Connection closed by the remote end");
					return 0;
				}
				buf[len] = 0;
				fprintf(stderr, "%s", buf);
				fflush(0);
			}
		}
#endif
	}
	close(sock_net);
	close(sock_term);
	return 0;
}
