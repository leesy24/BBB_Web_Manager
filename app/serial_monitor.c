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

int CMD_ECHO_STAT = CMD_STAT_NONE;
int CMD_SGA_STAT = CMD_STAT_NONE;

void negotiate(int sock, unsigned char *buf, int len)
{
	int i;

/**/
	printf("len=%d:", len);
	for(i = 0; i < len; i ++)
	{
		printf("buf[%d]=0x%02x(%d),", i, buf[i], buf[i]);
	}
	printf("\n");
/**/

	if (buf[1] == SERVER_WILL)
	{
		if(buf[2] == CMD_ECHO)
		{
			if(CMD_ECHO_STAT != CMD_STAT_NONE)
			{
				CMD_ECHO_STAT = CMD_STAT_ACK;
				return;
			}
			buf[1] = CLIENT_DO;
			CMD_ECHO_STAT = CMD_STAT_REPLY;
		}
		else if(buf[2] == CMD_SGA)
		{
			if(CMD_SGA_STAT != CMD_STAT_NONE)
			{
				CMD_SGA_STAT = CMD_STAT_ACK;
				return;
			}
			buf[1] = CLIENT_DO;
			CMD_SGA_STAT = CMD_STAT_REPLY;
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
	printf("=>len=%d:", len);
	for(i = 0; i < len; i ++)
	{
		printf("buf[%d]=0x%02x(%d),", i, buf[i], buf[i]);
	}
	printf("\n");
/**/
	if (send(sock, buf, len , 0) < 0)
		exit(1);
}
 
#define BUFLEN 1024
int main(int argc , char *argv[]) {
	int sock;
	struct sockaddr_in server;
	struct hostent *host;
	unsigned char buf[BUFLEN];
	int len;
	int i;

	if (argc < 2 || argc > 3)
	{
		printf("Usage: %s address [port]\n", argv[0]);
		return 1;
	}
	int port = 23;
	if (argc == 3)
	port = atoi(argv[2]);

	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		perror("Could not create socket. Error");
		return 1;
	}

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

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	puts("Connected...\n");

	struct timeval ts;
	ts.tv_sec = 1; // 1 second
	ts.tv_usec = 0;
 
	while (1)
	{
		// select setup
		fd_set fds;
		FD_ZERO(&fds);
		if (sock != 0)
			FD_SET(sock, &fds);
		FD_SET(0, &fds);

		// wait for data
		int nready = select(sock + 1, &fds, (fd_set *) 0, (fd_set *) 0, &ts);
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
		else if (sock != 0 && FD_ISSET(sock, &fds))
		{
			if(	CMD_ECHO_STAT != CMD_STAT_ACK ||
				CMD_ECHO_STAT != CMD_STAT_ACK)
			{
				// start by reading a single byte
				int rv;
				if ((rv = recv(sock , buf , 1 , 0)) < 0)
					return 1;
				else if (rv == 0)
				{
					printf("Connection closed by the remote end\n\r");
					return 0;
				}

				if (buf[0] == IAC_CMD)
				{
					// read 2 more bytes
					len = recv(sock , buf + 1 , 2 , 0);
					if (len  < 0)
					{
						perror("Could not recv. Error");
						return 1;
					}
					else if (len == 0)
					{
						printf("Connection closed by the remote end\n\r");
						return 0;
					}
					negotiate(sock, buf, 3);
					if(	CMD_ECHO_STAT == CMD_STAT_ACK &&
						CMD_ECHO_STAT == CMD_STAT_ACK)
					{
						char cmd[] = "monitor tcp ipv4,tcp,4001\r";
						//char cmd[] = "help\r";
						/**/
						printf(">>>len=%d:", strlen(cmd));
						for(i = 0; i < strlen(cmd); i ++)
						{
							printf("cmd[%d]=0x%02x(%d),", i, cmd[i], cmd[i]);
						}
						printf("\n");
						/**/
						send(sock, cmd, strlen(cmd) , 0);
					}
				}
			}
			else
			{
				len = recv(sock , buf , BUFLEN - 1 , 0);
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
				printf("%s", buf);
				fflush(0);
			}
		}
	}
	close(sock);
	return 0;
}
