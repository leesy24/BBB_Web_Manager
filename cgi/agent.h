#ifndef _AGENT_H_
#define _AGENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "iDisplayHtml.h"
//#include "include/define.h"
//#include "include/config.h"
//#include "include/default.h"

#include "cgic205/cgic.h"
//#include "cgic.h"

#define HTMLPATH        "/usr/local/www"
#define IMAGEPATH       "/usr/local/www/images"
#define CGIPATH         "/usr/local/www/cgi"

void Error(char *);
void DebugMessage(char *);
int CheckLogin ();
int  CheckAdmin(int type, char *pass);
int SetAdminPW(char *pstr);
void Message(char *);

#endif // _AGENT_H_
