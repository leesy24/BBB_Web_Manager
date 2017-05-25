#ifndef _AGENT_H_
#define _AGENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "iDisplayHtml.h"
//#include "define.h"
//#include "config.h"
//#include "default.h"

#include "cgic.h"

#define HTMLPATH        cgiDocumentRoot
#define IMAGEPATH       "images"
#define CGIPATH         "cgi-bin"

void Error(char *);
void DebugMessage(char *);
int CheckLogin ();
int  CheckAdmin(int type, char *pass);
int SetAdminPW(char *pstr);
void Message(char *);

#endif // _AGENT_H_
