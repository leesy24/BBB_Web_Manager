//========================================< Shared Memory >
#define SB_PORTVIEW_KEY		777			// PortView Shared Memory
#define SB_SNMP_KEY			999			// SNMP Shared Memory
//========================================< Odd Defile >



#define CFGFILE_FLASH_SYSTEM	"/flash/eddy_system.cfg"
#define CFGFILE_FLASH_SIO		"/flash/eddy_sio.cfg"
#define CFGFILE_FLASH_SNMP		"/flash/eddy_snmp.cfg"
#define CFGFILE_FLASH_GPIO		"/flash/eddy_gpio.cfg"
#define CFGFILE_FLASH_DIO		"/flash/eddy_dio.cfg"
#define CFGFILE_FLASH_WIFI		"/flash/eddy_wifi.cfg"

#define CFGFILE_ETC_SYSTEM		"/etc/eddy_system.cfg"
#define CFGFILE_ETC_SIO			"/etc/eddy_sio.cfg"
#define CFGFILE_ETC_SNMP		"/etc/eddy_snmp.cfg"
#define CFGFILE_ETC_GPIO		"/etc/eddy_gpio.cfg"
#define CFGFILE_ETC_DIO			"/etc/eddy_dio.cfg"
#define CFGFILE_ETC_WIFI		"/etc/eddy_wifi.cfg"

#define SB_SIO_NAME				"/dev/ttyS"
#define SB_WAN_ETH_NAME			"eth0"	
#define SB_LAN_ETH_NAME			"eth1"	
#define SB_WLAN_RA_NAME			"rausb0"
#define SB_DEVICE_ID			"EDDY"
#define	SB_ADC_DEVICE			"/dev/adc"			
#define	SB_GPIO_DEVICE			"/dev/gpio"			
#define  SNMPD_CONF 			"/usr/snmp/snmpd.conf"
#define SB_MAX_SIO_PORT			4	
#define SB_MAX_GPIO_PORT		56	
#define SB_MAX_DIO_PORT			16	
//========================================< Port Type >
#define SB_RS232				0
#define SB_RS422_PTOP			1
#define SB_RS422_MULTIDROP		2
#define SB_RS485_NONE_ECHO		3
#define SB_RS485_ECHO			5		// yhlee

#define SB_RS422_PTOP1			5
#define SB_RS422_MULTIDROP1		6
#define SB_RS485_NONE_ECHO1		7
#define SB_RS485_ECHO1			8
//=========================================< STATUS >
#define SB_DISABLE				0
#define SB_ENABLE				1
#define SB_ACTIVE				2
//========================================< Serial Flow Option >
#define SB_FLOW_NONE			0
#define SB_FLOW_XON				1
#define SB_FLOW_RTS				2
//========================================< Serial Parity Option >
#define	SB_PARITY_NONE			0
#define	SB_PARITY_ODD			1
#define	SB_PARITY_EVEN			2
//========================================< Serial Operation Mode >
#define SB_DISABLE_MODE			0 
#define SB_COM_REDIRECT_MODE	1 
#define SB_TCP_SERVER_MODE	  	2  
#define SB_TCP_CLIENT_MODE	   	3  
#define SB_TCP_BROADCAST_MODE   4
#define SB_TCP_MULTIPLEX_MODE   5
#define SB_UDP_SERVER_MODE    	6
#define SB_UDP_CLIENT_MODE    	7
//=========================================< Serial Device Type >
#define	SB_DATA_TYPE		0
#define	SB_MODEM_TYPE		1
//========================================< Eddy Model List >
#define EDDY_DK21			0x21
#define EDDY_CPU			0x1f
#define EDDY_S1_PIN 		0x00			// RS-232
#define EDDY_S1_PIN_C 		0x01			// 422/485
#define EDDY_S4M	 		0x02
/*
#define EDDY_S1_DB9_C		0x03
#define EDDY_S1_POE			0x04
#define EDDY_S1_POE_C		0x05
#define EDDY_S2_M			0x06
#define EDDY_S2_M_C			0x07
#define EDDY_WS1_TTL		0x08
#define EDDY_WS1_TTL_C		0x09
#define EDDY_WS1_PIN		0x0a
#define EDDY_WS1_PIN_C		0x0b
#define EDDY_WS1_DB9		0x0c
#define EDDY_WS1_DB9_C		0x0d
*/
#define EDDY_DK				0x1e
//=========================================< Detector >
#define SB_DETECT_LISTEN_PORT	2057
#define SB_RING_BUFFER_SIZE		10240

