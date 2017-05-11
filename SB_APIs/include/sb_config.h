//==================================================================================
// Copyright (c) 2008 SystemBase Co., LTD  All Rights Reserved.   		
//
// Eddy Configuration 							18.Dec,2008 by yhlee
//==================================================================================



//---------------------------------------------------------------------------------
//	Network Configuration Data (WAN, LAN, Service, Username) 
//---------------------------------------------------------------------------------
struct SB_SYSTEM_CONFIG
{
	char 			mac     [6];
	char 			ip		[4];
	char			mask	[4];
	char 			gateway	[4];
	char			dns     [4];			// dns server   0.0.0.0
	char			line;					// I = ip, D=DHCP	
	char			User_Config_Area_1 [64];// User Define Config Area
	char			dhcpenable;
	char			lanip		[4];
	char			lanmask		[4];
	char			dhcpstartIP	[4];
	char			dhcpendIP  	[4];
	unsigned short	lease_time;				// sec
	char			User_Config_Area_2 [64];// User Define Config Area
	char 			device_name[32];		// device server's alias name
	char 			username[16];			// username for web,telnet
	char 			password[16];			// password for web,telnet
	char 			portview[4];			// Portview IP	0.0.0.0
	unsigned short 	portview_port_no;		// Portview Port No	4000
	char			telnet_server;			// enable/disable	default=enable
	char			ftp_server;				// enable/disable		default=enable	
	char			web_server;				// enable/disable		default=enable
	char			ssh_server;				// enable/disable		default=disable
	char			target_agent;			// LemonIDE monitoring 	default=disable
	char			ddns    [4];			// ddns server	0.0.0.0
	char			ddnsuser [16];			// ddns loginuser	http://ddns.nu
	char			ddnspass [16];			// ddns loginpass
	char 			website[32];
	char 			contact[32];
	char			id [4];		
//	char			User_Config_Area_3 [64]; // User Define Config Area
//-----------------------------(ORG)!
	

//----------------------
// GDM_EC_ for Eddy v2.5
//----------------------
	char			GDM_XXzero[16];			//16	// "GDM_EC:: zero_position!
	char			GDM_YYzero[16];			//16	// "GDM_EC:: zero_position!
	char			GDM_DDzero[16];			//16	// "GDM_EC:: zero_position!
	//...++[48]byte...
	char			GDM_USEmode;			//		// "GDM_EC:: USE-GPS/Encoder! {-1:none 1:ON 0:OFF(encoder)}
	//...++[49]byte...
	
	//char			dummy[16];  //<---(64-48)=468
	char			dummy[15];  //<---#GDM_USEmode
	
	
};

//---------------------------------------------------------------------------------
//	Serial Configuration Data (COM Spec, Operation mode, ODD) 
//---------------------------------------------------------------------------------
struct SB_SIO_CONFIG
{
	char			name	[16];			// Port-01
	unsigned short	socket_no;				// 4001 ~ 4016
	char			protocol;				// 0=disable, 1=Com_Redirect,2=TCP_Server,3=TCP_Client,4=TCP_Broadcast,5=TCP_multiplex,6=UDP_Server,7=UDP_Client
	char			device;					// 0=Data, 1=Modem, Default : 0
	char			speed;					// 0=150,1=300,2=600,3=1200,4=2400,5-4800,6=9600,7=19.2k,8=38.4k,9=57.6k,10=115k,11=230k,12=460k,13=960k,   Default=6
	char			dps;					// 0x03, N/8/1
	char			flow;					// 0=none, 1=Xon, 2=RTS
	char			remote_ip [4];			// 0.0.0.0
	unsigned short	remote_socket_no;		// 4000
	unsigned short	packet_latency_time;	// latency time for completion packet   0 ~ 65535 msec
	unsigned short  keepalive;				// 0 socket Timeout (sec)	limited=0~65535 sec
	char			login;					// 0=Disable, 1=Enable, default=0
	char			login_name[16];			// none
	char			login_password[16];		// none	
	char    		interface;				// 0=232, 1=422ptp, , 3=485nc, 5=485e
	char			User_Config_Area [127];	// User Define Config Area
	};


//---------------------------------------------------------------------------------------------------------------------------------------
//bytes	    3								2								1								0							
//bits	    31	30	29	28	27	26	25	24	23	22	21	20	19	18	17	16	15	14	13	12	11	10	9	8	7	6	5	4	3	2	1	0
//bit		7	6	5	4	3	2	1	0	7	6	5	4	3	2	1	0	7	6	5	4	3	2	1	0	7	6	5	4	3	2	1	0
//---------------------------------------------------------------------------------------------------------------------------------------
//Port A	-	-	-	-	-	-	-	-	-	*	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	S2	S2	-	-	-	-
//Port B	Key Key	S1	S1	S0	S0	S0	S0	S0	S0	Key Key	*	*	*	*   DE	DE	*	*	S3	S3	S2	S2	S1	S1	S0	S0	EPM	EPM	EPM	EPM
//Port C	-	-	-	-	-	*	-	-	Key Key Key Key	*	*	NAD RST	LAN	NAD -	LAN	-   S3	*	S3	-	-	*   RDY	ADC	ADC	ADC	ADC
//---------------------------------------------------------------------------------------------------------------------------------------
//		S0 ~ S3	= Serial Port (Port 1 ~ 4)
//		DE		= Debug Port
//		RST		= Reset 
//		RDY		= Ready LED
//		ADC		= Analog Digital Converter 
//      LAN     = LAN Port
//		NAD		= Nand Flash
//		ER		= SPI(EEPROM)
//		Key		= Keypad
//		-		= Not Used
//		*		= GPIO & User Port
//---------------------------------------------------------------------------------------------------------------
#define GPIO_MASK_PORT_A			0x00400030
#define GPIO_MASK_PORT_B			0xffffffff
#define GPIO_MASK_PORT_C			0x06ffd73f			// PC25 Bluetooth Enable

#define S4M_GPIO_MASK_PORT_A		0x40400020  
#define S4M_GPIO_MASK_PORT_B		0xc03f300f  
#define S4M_GPIO_MASK_PORT_C		0x031ef62c 	

//---------------------------------------------------------------------------------
//	GPIO Configuration Data 
//---------------------------------------------------------------------------------
struct SB_GPIO_CONFIG
	{
	unsigned int	enable[3];				// 0:disable, 1:use to Gpio,,,		Port A=0, B=1, C=2
	unsigned int	mode  [3];				// 0:inout, 1:output
	unsigned int	value [3];				// 0:low, 1=high
	unsigned int	pullup[3];				// 0:pulldown, 1=pullup
	char			serial_1;				// 0:gpio, 1=serial, 2=User Device
	char			serial_2;				// 0:gpio, 1=serial, 2=User Device
	char			serial_3;				// 0:gpio, 1=serial, 2=User Device
	char			serial_4;				// 0:gpio, 1=serial, 2=User Device
	char			debugport;				// 0:gpio, 1=debug,  2=User Device
	char			adc;					// 0:gpio, 1=ADC,    2=User Device
	char			lan;					// 0:gpio, 1=LAN,    2=User Device
	char			reset;					// 0:gpio, 1=Reset,  2=User Device
	char			rdy_led;				// 0:gpio, 1=RDY,    2=User Device
	char			spi_eeprom;				// 0:gpio, 1=EEPROM(SPI),    2=User Device
	char			nandflash;				// 0:gpio, 1=Nand Flash,    2=User Device
	char			keypad;					// 0:gpio, 1=Key Pad,    2=User Device	
	char			User_Config_Area [32];	// User Define Config Area
};

//---------------------------------------------------------------------------------
//	DIO Configuration Data 
//---------------------------------------------------------------------------------
struct SB_DIO_CONFIG
	{
	unsigned char	lcd;					// use to LCD is Enable
	unsigned short	mode;					// 16 bit  0:input, 1:output
	unsigned short	value;					// 16 bit  0:Output(low), 1=Output(high)
	char			User_Config_Area [32];	// User Define Config Area
};

//---------------------------------------------------------------------------------
//	SNMP Configuration Data
//---------------------------------------------------------------------------------
struct SB_SNMP_CONFIG
{
	char			enable;					// enable/disable		default=disable
	char			trap_ip [4];			// Get only,		Trap receiver's IP address
	unsigned short	trap_port;				// Get only,		Port for trap service
	char			trap_option; 			// Get only,		If set to 0, no trap is enabled.
											//If set to 0-01, system restart notification is enabled.
											//If set to 0x02, port connectino notification is enabled.
											//If set to 0x04, both notifications are enabled.
	char			v1_readwrite;			// default 0: 0=Read only, 1=R/W
	char			v3_readwrite;			// default 0: 0=Read only, 1=R/W
	char			v3_username[16];		// default "none"
	char			v3_password[16];		// default "none"
	char			User_Config_Area [8];	// User Define Config Area
};		
		
//---------------------------------------------------------------------------------
//	WiFi Configuration Data
//---------------------------------------------------------------------------------
struct SB_WIFI_CONFIG
{
	int				wireless;				// 0 disable, 1=enable
	int				wifi_mode;				// infra, ad-hoc	
	char			ssid[32];
	int				channel;
	int				auth_mode;
	int				encryp_type;
	int				key_index;
	char			passphrase[68];
	char			key[32];
	int				wpa_algo;				// not used
	char			wpakey[32];				// not used
	char			eapusername[32];		// not used
	char			eappassword[32];		// not used
	
	int				roaming_mode;
	int				scanning_period;		// not used
	int				roaming_threshold;

	int				b_g_mode;				// Wireless NW mode (b, g, mixed)
	int				bit_rate;				// Bit rate, 0 = auto
	int				sens_thr;				// Sensitivity threshold
	int				rts_thr;				// RTS threshold, 0 = off
	int				frag_thr;				// Fragment threshold, 0 = off
	int				ieee80211h;				// 802.11h spectrum management

	int				line;					//dhcp or static	0=DHCP, 1=Static IP
	char 			ip		[4];	
	char			mask	[4];
	char 			gateway	[4];
	char			dns     [4];			// dns server   0.0.0.0
	unsigned char	reserve[20];
};

