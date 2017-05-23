
#define FUNC_COMPILE 1
#include "func.h"

int S_MAX_PORT;
char * a_signal	[2]	= { "Data ", "Modem" };
char * a_protocol[8] = {"Disable", "Com_Redirect", "TCP_Server", "TCP_Client", "TCP_Broadcast", "TCP_Multiplex", "UDP_Server", "UDP_Client"};
char * a_speed	[14] = { "150", "300", "600", "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200", "230400", "460800", "921600" };
char * a_data		[4]	= { "5", "6", "7", "8" };
char * a_stop		[2]	= { "1", "2" };
char * a_parity		[3]	= { "None", "Odd", "Even" };
char * a_flow		[3]	= { "None", "Xon", "Rts" };
char * a_interface 	[5] = { "RS232", "RS422-PTOP", "RS232-Multidrop","RS485-None_Echo","RS485-Echo" };

int get_interface(int port_no)
{
	int fd;
	int interface;
	int ret;
   	char comName[20];
	sprintf(comName, "%s%d", SER_DEVICE_NAME, port_no);
	fd = open(comName, O_RDWR);	
	if (fd > 0)
	{
		interface = ioctl(fd, TIOTSINTERFACE);
//		printf("ret: %x\n", ret);
//		interface = ret & 0x30;
//		printf("interface: %d\n", interface);
		close(fd);
	}
	if ((interface != RS232_TYPE) && (interface != RS422_TYPE) && (interface != RS485_TYPE))
	{
	    printf("get_interface error %d\n", interface);
	}
	
	return interface;
}

int set_interface(int port_no, int if_type) //shlee
{

    int ret = 0;
    int fd;
    char comName[20];

    sprintf(comName, "%s%d",SER_DEVICE_NAME, port_no);
    fd = open(comName, O_RDWR);
    
    if ((if_type != RS232_TYPE) && (if_type != RS422_TYPE) && (if_type != RS485_TYPE))
		printf("put_interface error %d\n", if_type);
	
    if (fd > 0)
    {
	   ret = ioctl(fd,TIOTSINTERFACEPUT,&if_type);
	   close(fd);
    }
    return ret;
}

//=============================================================
unsigned char get_char_from_dps( unsigned char char_dps )
{
	unsigned char bit_mask = 3;
	char_dps &= bit_mask;
	return a_data[char_dps][0];
}

//=============================================================
unsigned char make_char_n_dps( unsigned char get_dps, unsigned char get_char )
{
	unsigned char bit_mask = 252;
	get_dps &= bit_mask; // 초기화
	get_dps |= get_char;
	return get_dps;
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
unsigned char make_parity_n_dps( unsigned char get_dps, unsigned char get_parity )
{
	unsigned char bit_mask = 231;
	get_dps &= bit_mask; // 초기화
	get_parity <<= 3;
	get_dps |= get_parity;
	return get_dps;
}

//=============================================================
unsigned char get_stop_from_dps( unsigned char stop_dps )
{
	unsigned char bit_mask = 1;
	stop_dps >>= 2;
	stop_dps &= bit_mask;
	return a_stop[ stop_dps ][0];
}

//=============================================================
unsigned char make_stop_n_dps( unsigned char get_dps, unsigned char get_stop )
{
	unsigned char bit_mask = 251;
	get_dps &= bit_mask; // 초기화
	get_stop <<= 2;
	get_dps |= get_stop;
	return get_dps;
}

//=============================================================
unsigned char search_id_to_array (int flag)
{
	int i = 0;
	int ci = 0;

	get_string[ci] = toupper( get_string[ci] );
	for ( ci = 1; ci < strlen ( get_string ); ci++ ) 
		get_string[ci] = tolower(get_string[ci]);

	switch( flag ) 
	{
		case STATUS_FLAG:
			for ( i = 0; i < MAX_STATUS ; i++) {
				if (!strncmp (a_status[i], get_string, 2 )  ) return (unsigned char) i;
			}
			break;
		case SIGNAL_FLAG:
			for ( i = 0; i < MAX_SIGNAL ; i++) {
				if (!strncmp (a_signal[i], get_string, 2 )  ) return (unsigned char) i;
			}
			break;
		case PROTOCOL_FLAG:
			for ( i = 0; i < MAX_PROTOCOL ; i++) {
				if (!strncasecmp (a_protocol[i], get_string, 6 )  ) return (unsigned char) i;
			}
			break;
		case SPEED_FLAG:
			if (S_MAX_PORT == 2)
				{
						//--> shlee_0504
#if 0
				for ( i = 0; i < MAX_SPEED-1 ; i++) 
					{
					if (!strncmp (a_speed[i], get_string, strlen(a_speed[i]))  ) return (unsigned char) i;
					}
#endif
				for ( i = 0; i < MAX_SPEED ; i++) 
					{
					if (!strncmp (a_speed[i], get_string, strlen(a_speed[i]))  ) return (unsigned char) i;
					}
				//<-- 
				}
			else
				{
				for ( i = 0; i < MAX_SPEED ; i++) 
					{
					if (!strncmp (a_speed[i], get_string, strlen(a_speed[i]))  ) return (unsigned char) i;
					}
				}
			break;
		case DATA_FLAG:
			for ( i = 0; i < MAX_DATA ; i++) {
				if (!strncmp (a_data[i], get_string, strlen(a_data[i]))  ) return (unsigned char) i;
			}
			break;
		case PARITY_FLAG:
			for ( i = 0; i < MAX_PARITY ; i++) {
				if (!strncmp (a_parity[i], get_string, 3)  ) return (unsigned char) i;
			}
			break;
		case STOP_FLAG:
			for ( i = 0; i < MAX_STOP ; i++) {
				if (!strncmp (a_stop[i], get_string, strlen(a_stop[i]))  ) return (unsigned char) i;
			}
			break;
		case FLOW_FLAG:
			for ( i = 0; i < MAX_FLOW ; i++) {
				if (!strncmp (a_flow[i], get_string, 3)  ) return (unsigned char) i;
			}
			break;
		case INTERFACE: // shlee
			for ( i = 0; i < MAX_INTERFACE ; i++) {
			    if (!strncmp (a_interface[i], get_string,7))
			    {
				   //printf("interface = ( %s )\n",a_interface[i]);
				   //printf("interface num = ( %d ) \n",i);
				   return (unsigned char) i;
			    }
			}
			break;
	}

	return INPUT_ERROR;
}


//=============================================================
void convert_net (unsigned char *d, char *s)
{
	int i, len, j;
	int cnt = 0;
	char tp[10];
	struct sockaddr_in dest;		// who to ping

	if ( (dest.sin_addr.s_addr = inet_addr(s)) == INADDR_NONE)
	{
		printf("Error> Syntax error: Input the correct address\n");
		exit(0);
	}

	len = strlen (s);
	for (i=0; i<len; i++) if (*(s+i) == '.') cnt++;
	if (cnt != 3)
	{
		printf ("Error> Syntax Argument\n");
		exit(0);
	}

	j = 0;
	for (i=0; i<=len; i++)
	{
		tp[j++] = *(s+i);
		if (*(s+i) == 0x00 || *(s+i) == '.')
		{
			tp[--j] = 0; j=0;
			*d++ = atoi (tp);
		}
	}
}

// TWJUNG
void convert_mac (unsigned char *d, char *s)
{
	int i, len, j;
	int cnt = 0;
	char tp[10];
//	struct sockaddr_in dest;		// who to ping

	len = strlen (s);
	if (len != 17) {
		printf("Error> Syntex Argument\n");
		exit(0);
	}
	
	for (i=0; i<len; i++) if (*(s+i) == ':') cnt++;
	if (cnt != 5)
	{
		printf ("Error> Syntex Argment\n");
		exit(0);
	}

	j = 0;
	for (i=0; i<=len; i++)
	{
		tp[j++] = *(s+i);
		if (*(s+i) == 0x00 || *(s+i) == ':')
		{
			tp[--j] = 0; j=0;
			*d++ = strtol(tp, (char **)NULL, 16);//atoi (tp);
		}
	}
}

//=============================================================
int confirm_digit ( char * str )
{
	int str_len = 0;
	int i;

	str_len = strlen(str);
	for ( i =0; i < str_len; i++) if( ! isdigit( *(str+i) ) ) return INPUT_ERROR;
	return 1;
}

//=============================================================
int ushort_digit_to_string ( unsigned short * digit_saver, char * set_value )
{
//	int temp_set_value = 0;

	if( confirm_digit ( set_value ) == 1 ) * digit_saver = atoi(set_value);
	else
	{
		printf("Error> Syntex Argment error: Input digit\n");
		exit(0);
	}
	return 1;
}


#ifdef MIRHEE_FIX_DHCP_CLIENT
void GetDefaultGateway(char* szGateway)
{

    char buff[256];
    int  nl = 0 ;
    struct in_addr dest;
    struct in_addr gw;
    int flgs, ref, use, metric;
    unsigned long int d,g,m;

    FILE *fp = fopen("/proc/net/route", "r");

    while( fgets(buff, sizeof(buff), fp) != NULL ) 
    {
        if(nl) 
        {
            int ifl = 0;
            while(buff[ifl]!=' ' && buff[ifl]!='\t' && buff[ifl]!='\0')
                ifl++;
            buff[ifl]=0;    /* interface */
            if(sscanf(buff+ifl+1, "%lx%lx%X%d%d%d%lx",
               &d, &g, &flgs, &ref, &use, &metric, &m)!=7) 
            {
	            szGateway[0] = 0;
	            return;
            }
            
            dest.s_addr = d;
            gw.s_addr   = g;

            if (dest.s_addr == 0)
            {	// default gateway
            	strcpy(szGateway, inet_ntoa(gw));
              	return;
            }
                
        }
        nl++;
    }
}                

#endif // MIRHEE_FIX_DHCP_CLIENT
