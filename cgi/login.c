#include "icat.h"
#include "iDisplayHtml.h"

struct ABOUT about;

int getInfo()
{
	int fd;
	fd = open ("/var/config/about.conf", O_RDONLY);
	if (fd < 0)
	{
		Error("Can't open information file for this page");
		close (fd);
		return 0;
	}

	if (read (fd, &about, sizeof(about)) < 0)
	{
		Error("Can't read from informaiton file for this page");
		close(fd);
		return 0;
	}

	close (fd);
	return 1;
}
int cgiMain()
{
/* 
	// Load a previously saved CGI scenario if that button has been pressed.
//  if (cgiFormSubmitClicked("loadenvironment") == cgiFormSuccess) {
//	      LoadEnvironment();
//		    }
    // Set any new cookie requested. Must be done *before*
	//       outputting the content type. 
//    CookieSet();
	  // Send the content type, letting the browser know this is HTML 
	  cgiHeaderContentType("text/html");
	    // Top of the page 
	    fprintf(cgiOut, "<HTML><HEAD>\n");
		  fprintf(cgiOut, "<TITLE>cgic test</TITLE></HEAD>\n");
		    fprintf(cgiOut, "<BODY><H1>cgic test</H1>\n");
			  // If a submit button has already been clicked, act on the 
				     submission of the form. 
//			  if ((cgiFormSubmitClicked("testcgic") == cgiFormSuccess) ||
//					      cgiFormSubmitClicked("saveenvironment") == cgiFormSuccess)
//				    {
//						    HandleSubmit();
//							    fprintf(cgiOut, "<hr>\n");
//								  }
			    // Now show the form 
//			    ShowForm();
				  // Finish up the page 
				  fprintf(cgiOut, "</BODY></HTML>\n");
				    return 0;
*/					
	char userBuff[12];
    char passBuff[12];
    LIST list;
    char buff[256];
   	int fd;
	
	char username[12];
	int idfd;

	memset(userBuff,0,sizeof(userBuff));

	if ( cgiFormStringNoNewlines("user_name",userBuff,12) == cgiFormNotFound )
		Error("Your user name contains invalid characters.");

	if ( cgiFormStringNoNewlines("pw",passBuff,12) == cgiFormNotFound )
        Error("Your password is invalid");


	idfd=open("/var/config/id.conf",O_RDONLY);
	read(idfd,username,sizeof(username));
	close(idfd);
	if (strlen(userBuff)==0)
		Error("Invalid username!");
/*
	sprintf(buff,"%s:%s",username,userBuff);
	Error(buff);
*/
	if (strcmp(username,userBuff)!=0)
		Error("Invalid username!!");
  
    if ( CheckAdmin(passBuff) == 0 ) 
    {
        Error("Wrong password, please try again.");
    }
    else
    {
// mcpanic 5b15 cgic version up
		fd = open("/var/tmp/login.stat", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
		if (fd < 0)
		{
		Error("Internal Login Error");
		close (fd);
		}
		write(fd, "Y", 1);
		close(fd);
//		cgiFromString ("cookie_admin_pw", passBuff, sizeof(passBuff));
		
//        if ( cgiSetCookie("cookie_admin_pw", passBuff, "/") != cgiCookieSuccess)
//            Error("Can't Set cookie");
      
		getInfo();

		list = listCreate();
		listPutf(list, "device_name", "%s", about.device_name);
        sprintf(buff, "%s/main.htm", HTMLPATH);
        
        cgiHeaderContentType("text/html");
        fflush(0);
        listDisplay(list, buff);
        fflush(0);
        listDelete(list);
	}
	
}
