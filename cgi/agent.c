#include "agent.h"

void Error(char *msg)
{
    cgiOut = stdout;
	cgiHeaderContentType("text/html");
	
	fprintf(cgiOut, "<html><head>\n");
	fprintf(cgiOut, "<script>\n");
	fprintf(cgiOut, "function init()\n{\n");
	fprintf(cgiOut, "    alert(\"%s\");\n", msg);
	fprintf(cgiOut, "    history.back(1);\n");
	fprintf(cgiOut, "}\n");
	
	fprintf(cgiOut, "</script></head>\n");
	fprintf(cgiOut, "<body onLoad=\"init()\">\n");
	fprintf(cgiOut, "</body></html>\n");
	
	fflush(0);
	exit(-1);
}

void DebugMessage(char *msg)
{
    cgiOut = stdout;
	cgiHeaderContentType("text/html");
	
	fprintf(cgiOut, "<html><head>\n");
	fprintf(cgiOut, "<script>\n");
	fprintf(cgiOut, "function init()\n{\n");
	fprintf(cgiOut, "    alert(\"%s\");\n", msg);
	fprintf(cgiOut, "    history.back(1);\n");
	fprintf(cgiOut, "}\n");
	
	fprintf(cgiOut, "</script></head>\n");
	fprintf(cgiOut, "<body onLoad=\"init()\">\n");
	fprintf(cgiOut, "</body></html>\n");
	fflush(0);
}

int CheckLogin ()
{
	int fd;
	char sbuff[4];
	
	fd = open("/var/tmp/login.stat", O_RDONLY);
	if (fd < 0)
	{
		close(fd);
		return 0;
	}

	if (read(fd, sbuff, 1) < 0)
	{
		close(fd);
		return 0;
	}

	if (sbuff[0] == 'Y')
	{
		close(fd);
		return 1;
	}
	else
	{
		close(fd);
		return 0;
	}
}

int CheckAdmin(int type, char *pass)
{
    char	sbuff[16];
	int		i, len;
	int fd;
	
	memset(sbuff, 0x00, 16);

	if (type == 0)
		fd = open("/var/tmp/login.id", O_RDONLY);
	else 
		fd = open("/var/tmp/login.pw", O_RDONLY);

	if (fd > 0)
	{
		if ( read(fd, sbuff, 16) < 0) {
			close(fd);
		}
		close(fd);
	}
	else {
		close(fd);
		Error("can't open password file");
	}

	len = strlen(sbuff);
	for (i=0; i<len; i++)
		if (sbuff[i] <= 0x20 || sbuff[i] >= 0x7f) break;
	len = i;		
	if (len != strlen(pass)) return 0;

	for (i = 0; i < len; i++)
		if ( sbuff[i] != *(pass+i))	return 0;
	return 1;
}

int SetAdminPW(char *pstr)
{
	int fd;
	fd = open("/var/config/password.conf", O_WRONLY);
	
	if ( fd > 0) 
	{
		if ( write (fd, pstr, sizeof(pstr)) < 0 ) 
		{
		    Error("Can't write the administrator password info");
	
    		close(fd);
		    return 0;
		}
	}
    else 
    {
        Error("Can't open the administrator password info");

    	close(fd);
	    return 0;
	}

	close(fd);
    return 1;
}

void Message(char *msg)
{
	cgiOut = stdout;
	
	cgiHeaderContentType("text/html");
	
	fprintf(cgiOut, "<html><head>\n");
	fprintf(cgiOut, "<body bgcolor=\"white\">\n");
	fprintf(cgiOut, "<center><br><br>\n");
	fprintf(cgiOut, "<font color=#ff5555 face=±¼¸² size=3>Error : </font>\n");
	
	
	fprintf(cgiOut, "<font color=#5555AA face=±¼¸² size=2>%s</font>\n", msg);
	
	fprintf(cgiOut, "</center></body></html>\n");
	
	fflush(0);
	exit(0);
}
