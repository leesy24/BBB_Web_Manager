/*---------------------------------------------------------------------
Argment 	:  none 
return		:  
		EDDY_DK21			0x21
		EDDY_CPU			0x1f
		EDDY_S1_PIN 		0x00			
		EDDY_S1_PIN_C 		0x01			
		EDDY_S1_DB9 		0x02
		EDDY_S1_DB9_C		0x03
		EDDY_S1_POE			0x04
		EDDY_S1_POE_C		0x05
		EDDY_WS1_TTL		0x08
		EDDY_WS1_TTL_C		0x09
		EDDY_WS1_PIN		0x0a
		EDDY_WS1_PIN_C		0x0b
		EDDY_WS1_DB9		0x0c
		EDDY_WS1_DB9_C		0x0d
-----------------------------------------------------------------------*/
int SB_GetModel ()
{
int fd, eddyID=0;
                                                     
	fd = open (SB_GPIO_DEVICE, O_RDWR);
	if (fd < 0) return 0;
	eddyID = ioctl (fd, GET_PRODUCTID);
	close (fd);

	return eddyID;
}
//=============================================================
void SB_LogDataPrint (char *RTx, char *buff, int len)
{
int i, p=0;
char tmp[1000];
FILE *fp;
struct timeval tval;
unsigned long ttt;

	gettimeofday(&tval, NULL);
	ttt = ((tval.tv_sec*1000) + (tval.tv_usec/1000));

    fp=fopen("/dev/pts/0","w+");
   	if(fp==NULL) return;
   	sprintf (tmp, "[%lu] %s %3d = ",ttt, RTx, len);
	fprintf (fp, tmp);
   	for (i=0; i<len; i++) 
	   	{
	   	if (*buff >= 0x20 && *buff <= 0x7e)
	   		{
   			sprintf (&tmp[p], "%c,", (unsigned char)*buff++);
			p += 2;
   			}
   		else	
   			{
   			sprintf (&tmp[p], "%02x,", (unsigned char)*buff++);
   			p += 3;
   			}
   		if (p > 993) break;
   		}
	strcpy (&tmp[p], "\n");
	fprintf (fp, tmp);
	fclose (fp);
}
//==========================================================================
void SB_LogMsgPrint (const char *fmt, ...)
{
va_list args;
char buff[300], bufff[300];
FILE *fp;
struct timeval tval;
unsigned long ttt;

	gettimeofday(&tval, NULL);
	ttt = ((tval.tv_sec*1000) + (tval.tv_usec/1000));
    va_start(args, fmt);
    vsprintf(buff,fmt,args);
    va_end(args);
	sprintf (bufff, "[%lu] %s%c", ttt, buff, 0);

    fp=fopen("/dev/pts/0","w+");
   	if(fp==NULL) return;
   	fprintf(fp,bufff);
   	fclose(fp);
}
//==========================================================================
void SB_LogMsgSave (const char *fmt, ...)
{
va_list args;
char buff[300], bufff[300];
unsigned int seek;
int fd;
struct timeval tval;
unsigned long ttt;

	gettimeofday(&tval, NULL);
	ttt = ((tval.tv_sec*1000) + (tval.tv_usec/1000));
    va_start(args, fmt);
    vsprintf(buff,fmt,args);
    va_end(args);
	sprintf (bufff, "[%lu] %s\n%c", ttt, buff, 0);

   	fd = open("/var/log", O_RDWR|O_CREAT, 0644);
	if (fd <= 0) return;
	seek = lseek (fd, 0, 2);
	if (seek > 102400) 
		{
		close (fd);
		unlink ("/var/log");
		return;
		}
	write(fd, bufff, strlen(bufff));
	close(fd);
}
//-----------------------------------------------------------------------
int SB_Ip2Hex (char *ip, char *hex)
{
int i, len, j;
int cnt = 0;
char tp[10];
struct sockaddr_in dest;		// who to ping

	if ( (dest.sin_addr.s_addr = inet_addr(ip)) == INADDR_NONE)	return 0;

	len = strlen (ip);
	for (i=0; i<len; i++) if (*(ip+i) == '.') cnt++;
	if (cnt != 3) return 0;

	for (j=0,i=0; i<=len; i++)
		{
		tp[j++] = *(ip+i);
		if (*(ip+i) == 0x00 || *(ip+i) == '.')
			{
			tp[--j] = 0; j=0;
			*hex++ = atoi (tp);
			}
		}
	return 1;	
}
//-----------------------------------------------------------------------
void SB_Hex2Ip (char *hex, char *ip)
{
	sprintf (ip, "%d.%d.%d.%d", *(hex+0), *(hex+1),*(hex+2),*(hex+3));
	return;
}

//===============================================================================
void SB_getstring (char *source, char *findstring, int get_size, char *Dest)
{
char *point;

	memset (Dest, 0x00, 30);
	point = strstr(source, findstring);
	if (point == NULL)
		sprintf (Dest, "Null");
	else
		memcpy (Dest, point + strlen(findstring), get_size);
}

