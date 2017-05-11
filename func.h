#ifndef _FUNC_H_
#define _FUNC_H_

#include "../../include/sb_define.h"
#include "../../include/sb_config.h"
#include "../../include/sb_extern.h"
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <ctype.h>					// for "tolower" func by MiRhee, 060324

// #include "../include/mirhee.h"		// by MiRhee

#ifdef MIRHEE_FIX_DHCP_CLIENT
#include <net/if.h>					// for "ifreq" structure
#endif // MIRHEE_FIX_DHCP_CLIENT


char get_string [15];	// for search_id_to_string

extern char * a_status	[2];
extern char * a_signal	[2];
extern char * a_protocol[14];
extern char * a_speed	[14];
extern char * a_data		[4];
extern char * a_stop		[2];
extern char * a_parity		[3];
extern char * a_flow		[3];
extern char * a_interface 	[5]; 

//=============================================================
//               Porter Manager Function                     //
//=============================================================
int get_interface(int port_no);
int set_interface(int port_no, int if_type);
unsigned char get_char_from_dps( unsigned char char_dps );
unsigned char make_char_n_dps( unsigned char get_dps, unsigned char get_char );
unsigned char get_parity_from_dps( unsigned char parity_dps );
unsigned char make_parity_n_dps( unsigned char get_dps, unsigned char get_parity );
unsigned char get_stop_from_dps( unsigned char stop_dps );
unsigned char make_stop_n_dps( unsigned char get_dps, unsigned char get_stop );

void convert_net (unsigned char *d, char *s);
int confirm_digit ( char * str );
int ushort_digit_to_string ( unsigned short * digit_saver, char * set_value );
unsigned char search_id_to_array (int flag);
void convert_mac (unsigned char *d, char *s);

#ifdef MIRHEE_FIX_DHCP_CLIENT
void GetDefaultGateway(char* szNetMask);		// Default Gateway를 얻는 함수
#endif // MIRHEE_FIX_DHCP_CLIENT

#endif // _FUNC_H_
