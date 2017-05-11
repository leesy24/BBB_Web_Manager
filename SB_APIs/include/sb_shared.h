//------- Define PortView ----------------------------------------------------
#define PSM_PORT			PSM->port[port_no]
#define RX_GET   			PSM->rx_getp
#define RX_PUT   			PSM->rx_putp
#define TX_GET  			PSM->tx_getp
#define TX_PUT   			PSM->tx_putp
//------- Define SNMP (System)------------------------------------------------
#define SSM_REBOOT			SSM->admin.reboot				// set only
//------- Define SNMP (Serial)------------------------------------------------
#define SSM_RTS_STAT		SSM->mib[port_no].signals[0].sig_state
#define SSM_RTS_CHANGE		SSM->mib[port_no].signals[0].sig_changes
#define SSM_CTS_STAT		SSM->mib[port_no].signals[1].sig_state
#define SSM_CTS_CHANGE		SSM->mib[port_no].signals[1].sig_changes
#define SSM_DSR_STAT		SSM->mib[port_no].signals[2].sig_state
#define SSM_DSR_CHANGE		SSM->mib[port_no].signals[2].sig_changes
#define SSM_DTR_STAT		SSM->mib[port_no].signals[3].sig_state
#define SSM_DTR_CHANGE		SSM->mib[port_no].signals[3].sig_changes
#define SSM_DCD_STAT		SSM->mib[port_no].signals[4].sig_state
#define SSM_DCD_CHANGE		SSM->mib[port_no].signals[4].sig_changes
#define SSM_PARITY_ERRS		SSM->mib[port_no].parity_errs
#define SSM_OVERRUN_ERRS	SSM->mib[port_no].overrun_errs
#define SSM_FRAMING_ERRS	SSM->mib[port_no].frame_errs
#define SSM_SPEED			SSM->mib[port_no].speed			// get & set
#define SSM_DPS				SSM->mib[port_no].dps			// get & set
#define SSM_FLOW			SSM->mib[port_no].flow			// get & set
#define SSM_NAME			SSM->serial[port_no].name		// get & set
#define SSM_CONNECT_COUNT	SSM->serial[port_no].connection_count
#define SSM_CONNECT_STAT	SSM->serial[port_no].connection_stat
#define SSM_PORTRESET		SSM->serial[port_no].reset		// set only
//------- Define SNMP (Gpio)--------------------------------------------------
#define SSM_GPIO_MODE		SSM->gpio[port_no].mode		// get & set
#define SSM_GPIO_VALUE		SSM->gpio[port_no].value		// get & set


//-------------------------- Portview Shared Memory ----------------------------
struct SB_PORTVIEW_port
{
	char 			status;					// 0=Disable, 1=Connect Wait, 2=Active(connect)
	char 			flag;					// 0=none, 1=counter, 2=counter+scope,
	char 			reset_request;			// 0=none, 1=Reset Request
	char 			interface;
	unsigned int 	tx_bytes;		
	unsigned int 	rx_bytes;
	unsigned int	parity_err;			
	unsigned int	frame_err;				
	unsigned int	overrun_err;
};

struct SB_PORTVIEW_STRUCT
{
	char 			ports;							// Number of Ports 
	struct 			SB_PORTVIEW_port	port[16];	// Max 16 ports
	unsigned int	rx_lastputtime;
	unsigned int	rx_putp;
	unsigned int	rx_getp;
	char			rx_buff [SB_RING_BUFFER_SIZE];
	unsigned int	tx_lastputtime;
	unsigned int	tx_putp;
	unsigned int	tx_getp;
	char			tx_buff [SB_RING_BUFFER_SIZE];
};

//-------------------------- SNMP Shared Memory ----------------------------
struct SB_SNMP_SYSTEM_CONFIG	  
{
	char			mac [6];			// Get only, 	MAC address
	char 			ip	[4];			// Get only,		IP address
	char			mask	[4];		// Get only,		Subnet mask
	char 			gateway	[4];		// Get only,		Gateway
	char			dns     [4];		// Get only,		DNS server   		default=0.0.0.0
	char			ddns    [4];		// Get only,		DDNS server		default=0.0.0.0
	char 			portview[4];		// Get only,		Portview IP			default=0.0.0.0
	char 			ntp[4];				// Get only,		NTP IP			default=0.0.0.0	
	char			line;				// Get only,		Line connection type		I = ip, D=DHCP	
										// staticIP ( 1 ) , dhcp ( 2 )
	char 			device_name[32];	// Get & Set,	Device server's alias name
	char 			username[16];		// Get only,		username for web, FTP, SSH, and telnet
	char 			password[16];		// Get only,		password for web FTP, SSH, and telnet
	unsigned short 	portview_port_no;	// Get only,		Portview Port No		4000
	// Network - service configuration
	char			snmp_agent;			// Get only,		SNMP Agent 		1 = enable/0 = disable	default=disable
	char			telnet_server;		// Get only,		Telnet Server		1 = enable/0 = disable	default=enable
	char			ftp_server;			// Get only,		FTP Server			1 = enable/0 = disable	default=enable	
	char			web_server;			// Get only,		Web Server 		1 = enable/0 = disable	default=enable
	char			ssh_server;			// Get only,		SSH Server		1 = enable/0 = disable	default=disable
	char			target_agent;		// Get only,		LemonIDE monitoring 	1 = enable/0 = disable	default=disable
	char			model;				// Get only,		Eddy model type
	char			ddnsuser [16];		// Get only,		ddns loginuser		http://ddns.nu
	char			ddnspass [16];		// Get only,		ddns loginpass
	char 			website[32];		// Get & Set,	Website information	default=www.sysbas.com
	char 			contact[32];		// Get & Set,	Contact information	default=tech@sysbas.com
	char			v1_readwrite;		// default 0: 0=Read only, 1=R/W
	char			v3_readwrite;		// default 0: 0=Read only, 1=R/W
	char			v3_username[16];	// default "none"
	char			v3_password[16];	// default "none"
};

struct SB_SNMP_SERIAL_CONFIG	
{
	char			name	[16];		// Get & Set,	Port-01
	unsigned short	socket_no;			// Get only,		4001 ~ 4016
	char			protocol;			// Get only,		disabled ( 0 ) , comRedirect ( 1 ) , tcpServer ( 2 ) , tcpClient ( 3 ) , tcpBroadcast ( 4 ) ,
										// tcpMultiplex ( 5 ) , udpServer ( 6 ) , udpClient ( 7 )  
	char			device;				// Get only,		0=Data, 1=Modem, default= 0
	char			remote_ip [4];		// Get only,		default=0.0.0.0
	unsigned short	remote_socket_no;	// Get only,		default=4000
	unsigned short	packet_latency_time;// Get only,		latency time for completion packet    limited=0~65535 msec
	unsigned short	keepalive;			// Get only,		0 socket Timeout (sec)	limited=0~65535 sec
	char			login;				// Get only,		0=Disable, 1=Enable, default=0
	char			login_name[8];		// Get only,		none
	char			login_password[8];	// Get only,		none	
	unsigned short	connection_count;	// Get only,		Sock Connection Count			
	char			connection_stat;	// Get only,		Connection Status 	idle ( 0 ) , connected ( 1) 
	char			reset;				// set only
};	

struct SB_SNMP_GPIO						// all GET/SET support
{
	unsigned short	socket_no;			// Get only,		listen socket	5001~
	char			mode;			// Get & Set,	0:input, 1:output, default 0
	char    		value;			// Get & Set,	0 or 1, only works for output, default=0
};
struct SB_SNMP_TRAP	  
{
	char			trap_ip [4];		// Get only,		Trap receiver's IP address
	unsigned short	trap_port;			// Get only,		Port for trap service
	char			trap_option; 		// Get only,		If set to 0, no trap is enabled.
										//	If set to 0x01, system restart notification is enabled.
										//	If set to 0x02, port connectino notification is enabled.
										//	If set to 0x04, both notifications are enabled.
};

struct SB_SNMP_ADMIN
{
	char 			bl_ver	[10];		// Get only,		bootloader version	
	char 			os_ver	[10];		// Get only,		kernel version	
	char			fs_ver	[10];		// Get only,		filesystem version	
	char			reboot;	
};

struct SB_SNMP_SIGNAL
{
	char 			port_index;
	char 			name; 				// rts ( 1 ) , cts ( 2 ) , dsr ( 3 ) , dtr ( 4 ) , ri ( 5 ) , dcd ( 6 ) , sq ( 7 ) , srs ( 8 ) , srts ( 9 ) , scts ( 10 ) , sdcd ( 11 )
	char 			sig_state;			// signal state:  on ( 1 ) , off ( 0 )
	int 			sig_changes;
};

struct SB_SNMP_SERIAL_MIB	
{
	short			port_index;			// Get only,		port number 
	char			speed;				// Get & Set,	0=150,1=300,2=600,3=1200,4=2400,5-4800,6=9600,7=19.2k,8=38.4k,
										// 			9=57.6k,10=115k,11=230k,12=460k,13=960k,   Default=6
	char			dps;				// Get & Set,	0x03, N/8/1
										// Data bits 5~8
										// Stop bits one ( 1 ) , two ( 2 ) , oneAndHalf ( 3 ) , dynamic ( 4 )
										// Parity bits none ( 1 ) , odd ( 2 ) , even ( 3 ) , mark ( 4 ) , space ( 5 )
	char			flow;				// Get & Set,	0=none, 1=Xon, 2=RTS	
										// none ( 1 ) , ctsRts ( 2 ) , dsrDtr ( 3 )
	char    		interface;			// Get only,		0=232, 1=422ptp, 2=422mp, 3=485nc, 4=485e
										// other-rs485 ( 1 ) , rs232 ( 2 ) , rs422 ( 3 ) , rs423 ( 4 ) , v35 ( 5 ) , x21 ( 6 )
	// signal MIB
	struct SB_SNMP_SIGNAL signals[5];
	unsigned int  	parity_errs;		// Get only,		Parity Errors count		
    unsigned int  	frame_errs;			// Get only,		Framing Errors count	
    unsigned int  	overrun_errs;		// Get only,		Overrun Errors count	
};

struct SB_SNMP_STRUCT
{
	struct SB_SNMP_SYSTEM_CONFIG	system;						// network basic infomation
	struct SB_SNMP_SERIAL_CONFIG 	serial [SB_MAX_SIO_PORT];	// RFC-1317 RS-232 outside infomation 
	struct SB_SNMP_SERIAL_MIB		mib [SB_MAX_SIO_PORT];		// RS-1317 RS-232  inside infomation
	struct SB_SNMP_GPIO				gpio[SB_MAX_GPIO_PORT];		// GPIO 
	struct SB_SNMP_TRAP				trap;						// TRAP 
	struct SB_SNMP_ADMIN			admin;						// Eddy Info
};

