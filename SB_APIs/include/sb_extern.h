//---------------- Serial APIs -----------------------------------------------------------
int  SB_OpenSerial  (int port_no);
void SB_InitSerial  (int sfd, char speed, char dps, char flow);
void SB_SendSerial    (int fd, char *buff, int len);
int  SB_ReadSerial  (int fd, char *buff, int buffer_limit, int wait_msec);
void SB_SetInterface(int interface_type);
void SB_SetDtr      (int fd, int flag);
void SB_SetRts      (int fd, int flag);
int  SB_GetPorts    ();
int  SB_GetModel    ();
int  SB_GetDelaySerial     (int bps);
void SB_SetSerialInterface (int port_no,int interface);
unsigned char SB_GetMsr (int fd);
//---------------- Socket APIs -----------------------------------------------------------
int SB_ConnectTcp (char *IP_address, unsigned int socket_no, int wait_sec, int Tx_Size, int Rx_Size);
int SB_CheckConnect (int fd, struct sockaddr *saptr, socklen_t salen, long wait_sec);
int SB_ListenTcp (unsigned int socket_no,  int Tx_Size, int Rx_Size);
int SB_AcceptTcp (int fd, int msec);
int SB_AcceptTcpMulti (int fd, int msec);
int SB_ReadTcp (int fd, char *buff, int buff_size);
void SB_CloseTcp (int fd); 
int SB_BindUdp (unsigned int socket_no) ;
int SB_ReadUdp (int fd, char *buff, int buffer_size);
void SB_SendUdpServer (int fd, char *buff, int len);
void SB_SendUdpClient (int fd, char *buff, int len, char *ServerIP, int ServerPort);
unsigned int SB_GetIp (char *eth_name);
unsigned int SB_GetMask (char *eth_name);
unsigned int SB_GetGateway ();
int SB_SendTcp    (int fd, char *buff, int len);
//---------------- Log APIs -----------------------------------------------------------
void SB_LogMsgSave   (const char *fmt, ...);
void SB_LogMsgPrint  (const char *fmt, ...);
void SB_LogDataPrint (char *RTx, char *buff, int len);
//---------------- System Tick APIs -----------------------------------------------------------
void SB_InitEddy    ();
void SB_GetVersion  (int type, char *ver);
unsigned long SB_GetTick ();
void SB_msleep (int msec) ;
void SB_SetReadyLed (int onoff);
void SB_SetPriority (int priority);
//---------------- Flash APIs -----------------------------------------------------------
//---------------- Eddy Application APIs -----------------------------------
void *SB_GetSharedMemory (int Key_ID, int Shared_Size);
void *SB_SetSharedMemory (int Key_ID, int Shared_Size);
int SB_ReadConfig (char *FileName, char *Dest, int Get_Size);
int SB_WriteConfig (char *FileName, char *Source, int Put_Size);
int  SB_Ip2Hex (char *ip, char *hex);
void SB_Hex2Ip (char *hex, char *ip);
void SB_AliveTime (int *day, int *hour, int *min, int *sec);
void SB_getstring (char *source, char *findstring, int get_size, char *Dest);
//---------------- Eddy Application APIs -----------------------------------
void SB_SetGpioMode (int port, int mode);
void SB_SetGpioValue (int port, int value);
int  SB_GetGpioMode  (int port);
int  SB_GetGpioValue (int port);


