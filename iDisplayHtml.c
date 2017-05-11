/*
    icelove's Display Html

    1999/3/9


    Link with cgic.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "iDisplayHtml.h"
#include "cgic205/cgic.h"

/* Read Status Definition */
#define RSNORMAL        0
#define RSSTX           1
#define RSETX           2
#define RSCHECKTAG      3
#define RSLOOP          4
#define RSREPLACE       5

/* List Get Definition */
#define LGNOT           0
#define LGOK            1
#define LGENDLOOP       2
#define LGOKFILE        3
#define LGENDLOOPFILE   4


char BigBuff[20480];

void ClearSpace(char *data, char *buff);
void ErrorMsg(char * fmt, ... );

void printFile(char *fname);

int   _listPutRaw(LIST list, char *pname, char *val, int type);
pDATA CreateData(char *data, int type);
pNODE CreateNode(char *name, char *val, int type);
pNODE listSearch(LIST list, char *name);

int  ProcessLoop(LIST list, char *hbuff, long size);
void ProcessParse(LIST list, char *hbuff, long size);

// mcpanic adopted from old/cgic.h
// by icelove addition in 1998.5.20
void txt2html(char *data, int max);
void Puts(char * fmt, ... );
void hPuts(char *fmt);

/* 에러 메시지를 출력한다. */
void ErrorMsg(char * fmt, ... )
{
	va_list args;
	char buff[256];

	va_start(args, fmt);
	vsprintf(buff, fmt, args);

    printf("<html><body><center>");
    printf("<font color=#ff0000 size=4>[ Error ]</font>&nbsp;&nbsp;&nbsp;&nbsp;");
    printf("<font color=#5555aa>%s</font>\n",buff);
    printf("</center></body></html>\n");

    exit(0);
}

/* 전달받은 화일의 내용을 화면에 뿌려준다. */
void printFile(char *fname)
{
    FILE *fd;
    char readBuff[2510];
    int  readSize;

    fd = fopen(fname, "r");
    if ( fd == NULL ) { printf("Error : [%s]", fname); return; }
    while(1)
    {
        if ( fgets(readBuff, 2000, fd) == NULL ) break;
        txt2html(readBuff, 2500);
        printf("%s\n", readBuff);
    }
    fclose(fd);
}

/*
void printFile(char *fname)
{
    int fd;
    char readBuff[2048];
    int  readSize;

    fd = open(fname, O_RDONLY);
    if ( fd == -1 ) { printf("Error : [%s]", fname); return; }
    while(1)
    {
        readSize = read(fd, readBuff, 1024);
        if ( readSize <= 0 ) break;
        readBuff[readSize] = 0;
        txt2html(readBuff, 2000);
        printf("%s\n", readBuff);
    }
    close(fd);
}
*/

/* DATA객체 하나를 생성한다. */
/* 성공하면 포인터를, 아니면 NULL을 돌려준다. */
pDATA CreateData(char *data, int type)
{
    pDATA node;

    node = (pDATA)malloc(sizeof(DATA));
    if ( node == NULL ) return NULL;
    node->Next = NULL;
    node->Data = (char *)malloc(strlen(data)+1);
    if ( node->Data == NULL )
    {
        free(node);
        return NULL;
    }
    node->Type = type;
    strcpy(node->Data, data);

    return node;
}

/* data 문자열에서 빈칸을 없애, buff 에 담아준다. */
void ClearSpace(char *data, char *buff)
{
    while( *data )
    {
        if ( *data != ' ' )
        {
            *buff = *data;
            buff++;
        }
        data++;
    }

    *buff = 0;
}

/* 리스트를 하나 생성한다. */
LIST listCreate(void)
{
    LIST list;

    list = (LIST)malloc(sizeof(struct _list));
    if ( list == NULL ) return NULL;
    list->NodeCount = 0;
    list->DataCount = 0;
    list->Node = NULL;

    return list;
}

/* 생성된 리스트와 데이타들을 모두 삭제한다. */
void listDelete(LIST list)
{
    pNODE tempNode;
    pNODE Node;
    pDATA Sll;
    pDATA tempSll;

	
    Node = list->Node;
    while(1)
    {
        if ( Node == NULL ) break;

        Sll = Node->Header;
        while(1)
        {
            if ( Sll == NULL ) break;

            tempSll = Sll->Next;
			free(Sll->Data);
            free(Sll);
            Sll = tempSll;
        }
        tempNode = Node->Next;
//        free(Node->Header);
        free(Node->Name);
        free(Node);

        Node = tempNode;
    }
    free(list);

}

/* LIST에 데이타가 존재하는지 살펴보고, 존재하면 해당 Node 위치를 돌려준다. */
/* 실패하면 NULL을 돌려준다. */
pNODE listSearch(LIST list, char *name)
{
    pNODE node;

    if ( list == NULL ) return NULL;
    if ( list->Node == NULL ) return NULL;

    node = list->Node;
    while(1)
    {
        if ( node == NULL ) break;
        if ( strcmp(node->Name, name) == 0 ) return node;
        node = node->Next;
    }

    return NULL;
}

/* 노드를 하나 생성한다. */
pNODE CreateNode(char *name, char *val, int type)
{
    pNODE node;

    node = (pNODE)malloc(sizeof(NODE));
    if ( node == NULL ) return NULL;
    node->Name = (char *)malloc(strlen(name)+1);
    if ( node->Name == NULL )
    {
        free(node);
        return NULL;
    }
    strcpy(node->Name, name);
    node->MaxList = 0;
    node->Next = NULL;
    node->Header = CreateData(val, type);
    if ( node->Header == NULL )
    {
        free(node);
        return NULL;
    }
    node->Reader = node->Header;

    return node;
}

/* LIST에 데이타를 하나 추가 시킨다.  이미 있다면 다중으로 만든다. */
/* 성공하면 1을, 아니면 0을 돌려준다. */
int _listPutRaw(LIST list, char *pname, char *val, int type)
{
    pNODE node, node2;
    pDATA data;
    char name[256];

    if ( list == NULL ) list = listCreate();
    if ( list == NULL ) return 0;

    ClearSpace(pname, name);
    node = listSearch(list, name);
    if ( node == NULL )
    {
        node = CreateNode(name, val, type);
        if ( node == NULL ) return 0;

        list->DataCount++;
        list->NodeCount++;
        node->MaxList++;
        if ( list->Node == NULL ) list->Node = node;
        else
        {
            node2 = list->Node;
            while(1)
            {
                if ( node2->Next == NULL ) break;
                node2 = node2->Next;
            }
            node2->Next = node;
        }
    }
    else
    {
        data = node->Header;
        while(1)
        {
            if ( data->Next == NULL ) break;
            data = data->Next;
        }

        data->Next = CreateData(val, type);
        if ( data->Next == NULL ) return 0;
        else
        {
            list->DataCount++;
            node->MaxList++;
        }
    }

    return 1;
}

/* format 형식의 데이타를 집어 넣는다.  사용법은 printf(); 와 같다. */
/* 성공하면 1을, 아니면 0을 돌려준다. */
int listPutf(LIST list, char *name, char * fmt, ... )
{
	va_list args;

	va_start(args, fmt);
	vsprintf(BigBuff, fmt, args);

    return _listPutRaw(list, name, BigBuff, DATATYPESTRING);
}

int listPut(LIST list, char *pname, char *val)
{
    return _listPutRaw(list, pname, val, DATATYPESTRING);
}

int listPutFile(LIST list, char *pname, char *fname)
{
    return _listPutRaw(list, pname, fname, DATATYPEFILE);
}

int listPutfFile(LIST list, char *name, char * fmt, ... )
{
	va_list args;

	va_start(args, fmt);
	vsprintf(BigBuff, fmt, args);

    return _listPutRaw(list, name, BigBuff, DATATYPEFILE);
}

/* 리스트에서 자료를 하나 돌려준다.  루프일 경우 리딩카운터를 하나 증가한다. */
int listGet(LIST list, char *pname, char *buff)
{
    pNODE node;
    char name[256];
    int  type;

    if ( list == NULL ) return LGNOT;

    ClearSpace(pname, name);
    node = listSearch(list, name);

    if ( node == NULL ) return LGNOT;
    else
    {
        if ( node->Reader == NULL ) return LGNOT;
        if ( node->Reader->Data == NULL ) return LGNOT;

        strcpy(buff, node->Reader->Data);
        type = node->Reader->Type;
        if ( node->Reader->Next == NULL )
        {
            node->Reader = node->Header;
            return type==DATATYPEFILE ? LGENDLOOPFILE:LGENDLOOP;
        }
        else node->Reader = node->Reader->Next;
    }

    return type==DATATYPEFILE ? LGOKFILE:LGOK;
}

/* 리스트의 순서를 거꾸로 바꾼다. */
void listReverse(LIST list, char *qName)
{
    pNODE node;
    pDATA temp;
    pDATA pretemp;
    pDATA head;
    char name[256];

    if ( list == NULL ) return;

    ClearSpace(qName, name);
    node = listSearch(list, name);

    if ( node == NULL ) return;
    head = NULL;

    while(1)
    {
        temp = node->Header;
        pretemp = NULL;
        while(1)
        {
            if ( temp->Next == NULL ) break;
            pretemp = temp;
            temp = temp->Next;
        }
        if ( pretemp != NULL ) pretemp->Next = NULL;

        if ( head == NULL )
        {
            head = temp;
            head->Next = NULL;
        }
        else
        {
            pretemp = head;
            while(1)
            {
                if ( pretemp->Next == NULL ) break;
                pretemp = pretemp->Next;
            }
            pretemp->Next = temp;
        }

        if ( temp == node->Header ) break;
    }

    node->Header = head;
    node->Reader = head;
}


/* 생성된 리스트의 데이타의 구조를 파일로 저장한다. */
void listShowInfo(LIST list, char *fname)
{
    pDATA data;
    pNODE node;
    FILE *fd;
    long i;

    if ( list == NULL ) return;
    node = list->Node;

    fd = fopen(fname, "w+");
    if ( fd == NULL ) return;

    fprintf(fd, "Total Name: %lu\nTotal Data : %lu\n\n", list->NodeCount, list->DataCount);
    while(1)
    {
        if ( node == NULL ) break;
        fprintf(fd, "> Name : %s  [Count:%lu]\n", node->Name, node->MaxList);
        data = node->Header;

        for(i=0; i<node->MaxList; i++)
        {
            if ( data == NULL ) break;
            fprintf(fd, "\t- Value : %s\n", data->Data);
            data = data->Next;
        }

        node = node->Next;
    }
    fclose(fd);
}

/* 제작된 리스트를 화면으로 보낸다. */
void listDisplay(LIST list, char *HtmlName)
{
    int fd;
    char *hbuff;
    char *readBuff;
    long FileSize;
    int  readSize;

    fd = open(HtmlName, O_RDONLY);
    if ( fd == -1 ) ErrorMsg("File Open Error : %s", HtmlName);
    FileSize = lseek(fd, 0, SEEK_END);
    hbuff = (char *)malloc(FileSize);
    if ( hbuff == NULL )
	ErrorMsg("listDisplay() : Html 읽을 버퍼를 마련하지 못했습니다.");
    lseek(fd, 0, SEEK_SET);
    readBuff = hbuff;
    while(1)
    {
        readSize = read(fd, readBuff, 1024);
        if ( readSize <= 0 ) break;
        readBuff += readSize;
    }
    close(fd);
    ProcessParse(list, hbuff, FileSize);
    free(hbuff);
}

/*
    구문분석을 처리한다.
    Html 화일이 담긴 버퍼와 크기를 넘겨주면 분석을 한다.
*/
void ProcessParse(LIST list, char *hbuff, long size)
{
    long i, j, LoopSize;
    char *pStartParse;
    char *pStartLoop;
    char *p;
    int  readStatus, namePos, Divider;
    char NameBuff[256];

    readStatus = RSNORMAL;

    p = hbuff;
    for(i=0; i<size; i++,p++)
    {
        switch( readStatus )
        {
        case RSNORMAL:          /* 태그의 시작인지 확인한다. */
            if ( *p == '[' )
            {
                readStatus = RSCHECKTAG;
                pStartParse = p;
            }
            else putchar(*p);
            break;

        case RSCHECKTAG:        /* 올바른 태그인지 체크 */
            if ( *p=='r' || *p=='R' || *p=='v' || *p=='V' )
            {
                readStatus = RSREPLACE;
                Divider = 0;
                namePos = 0;
            }
            else if ( (*p=='l' || *p=='L') && *(p+1)==']' )
            {
                readStatus = RSLOOP;
                LoopSize = 0;
                pStartLoop = p+2;
                p++;
                i++;
            }
            else
            {
                for(j=0; j<size; j++)   /* 태그가 올바르지 않으면 그대로 화면에 출력한다. */
                {
                    putchar( *pStartParse );
                    if ( pStartParse == p ) break;
                    pStartParse++;
                }
                readStatus = RSNORMAL;
            }
            break;

        case RSLOOP:    /* LOOP 태그의 처리 */
            if ( *p == '[' )
            {
                if ( *(p+1)=='/' && (*(p+2)=='l' || *(p+2)=='L') && *(p+3)==']' )
                {
		    int cnt ;
                    cnt = ProcessLoop(list, pStartLoop, LoopSize);
//		    printf("Loop Count :%d ", cnt);
                    p += 3;
                    i += 3;
                    readStatus = RSNORMAL;
                }
                else LoopSize++;
            }
            else
            {
                LoopSize++;
            }
            break;

        case RSREPLACE:     /* 치환 태그 처리 */
            if ( Divider )
            {
                switch( *p )
                {
                case ']':
                    NameBuff[namePos] = 0;
                    switch( listGet(list, NameBuff, BigBuff) )
                    {
                    case LGNOT:
                        for(j=0; j<size; j++)
                        {
                            putchar( *pStartParse );
                            if ( pStartParse == p ) break;
                            pStartParse++;
                        }
                        break;

                    case LGOKFILE:
                    case LGENDLOOPFILE:
                        printFile(BigBuff);
                        break;

                    default:
                        printf("%s",BigBuff);
                        break;
                    }
                    readStatus = RSNORMAL;
                    break;

                case ' ':   case '\"':  case '\'':      /* 빈칸, ', " 등은 건너뜀 */
                    break;

                default:
                    NameBuff[namePos++] = *p;
                    if ( namePos >= 254 )   /* 버퍼넘침 방지 */
                    {
                        for(j=0; j<size; j++)
                        {
                            putchar( *pStartParse );
                            if ( pStartParse == p ) break;
                            pStartParse++;
                        }
                        readStatus = RSNORMAL;
                    }
                    break;
                }
            }
            else
            {
                if ( *p==' ' || *p==',' || *p=='.' || *p==';' || *p==':' ) Divider = 1;
                else
                {
                    for(j=0; j<size; j++)
                    {
                        putchar( *pStartParse );
                        if ( pStartParse == p ) break;
                        pStartParse++;
                    }
                    readStatus = RSNORMAL;
                }
            }
            break;
        }
    }

    if ( readStatus != RSNORMAL )   /* 처리되지 못한 태그를 출력한다. */
    {
        for(j=0; j<size; j++)
        {
            putchar( *pStartParse );
            if ( pStartParse == p ) break;
            pStartParse++;
        }
    }
}

/* 루프태그를 처리한다. 최대 10000 번까지만 루프를 돈다. */
/* 루프를 돈 횟수를 돌려준다. */
int ProcessLoop(LIST list, char *hbuff, long size)
{
    long i, j;
    char *pStartParse;
    char *p;
    int  readStatus, namePos, LoopCount, rTagCounter, Divider, EndLoop;
    char NameBuff[256];

    EndLoop = 0;
    for(LoopCount=0; LoopCount<10000; LoopCount++)
    {
        readStatus = RSNORMAL;
        p = hbuff;
        rTagCounter = 0;
        for(i=0; i<size; i++,p++)
        {
            switch( readStatus )
            {
            case RSNORMAL:
                if ( *p == '[' )
                {
                    readStatus = RSCHECKTAG;
                    pStartParse = p;
                }
                else putchar( *p );
                break;

            case RSCHECKTAG:
                if ( *p=='r' || *p=='R' || *p=='v' || *p=='V' )
                {
                    readStatus = RSREPLACE;
                    Divider = 0;
                    namePos = 0;
                }
                else
                {
                    for(j=0; j<size; j++)   /* 태그가 올바르지 않으면 그대로 화면에 출력한다. */
                    {
                        putchar( *pStartParse );
                        if ( pStartParse == p ) break;
                        pStartParse++;
                    }
                    readStatus = RSNORMAL;
                }
                break;

            case RSREPLACE:
                if ( Divider )
                {
                    switch( *p )
                    {
                    case ']':
                        NameBuff[namePos] = 0;
                        switch( listGet(list, NameBuff, BigBuff) )
                        {
                        case LGNOT:
                            for(j=0; j<size; j++)
                            {
                                putchar( *pStartParse );
                                if ( pStartParse == p ) break;
                                pStartParse++;
                            }
                            break;

                        case LGENDLOOP:
                            rTagCounter++;
                            printf("%s",BigBuff);
                            EndLoop = 1;
                            break;;

                        case LGOK:
                            rTagCounter++;
                            printf("%s",BigBuff);
                            break;

                        case LGENDLOOPFILE:
                            rTagCounter++;
                            printFile(BigBuff);
                            EndLoop = 1;
                            break;;

                        case LGOKFILE:
                            rTagCounter++;
                            printFile(BigBuff);
                            break;
                        }
                        readStatus = RSNORMAL;
                        break;

                    case ' ':   case '\"':  case '\'':      /* 빈칸, ', " 등은 건너뜀 */
                        break;

                    default:
                        NameBuff[namePos++] = *p;
                        if ( namePos >= 254 )   /* 버퍼넘침 방지 */
                        {
                            for(j=0; j<size; j++)
                            {
                                putchar( *pStartParse );
                                if ( pStartParse == p ) break;
                                pStartParse++;
                            }
                            readStatus = RSNORMAL;
                        }
                        break;
                    }
                }
                else
                {
                    if ( *p==' ' || *p==',' || *p=='.' || *p==';' || *p==':' ) Divider = 1;
                    else
                    {
                        for(j=0; j<size; j++)
                        {
                            putchar( *pStartParse );
                            if ( pStartParse == p ) break;
                            pStartParse++;
                        }
                        readStatus = RSNORMAL;
                    }
                }
                break;
            }
        }
        if ( readStatus != RSNORMAL )
        {
            for(j=0; j<size; j++)
            {
                putchar( *pStartParse );
                if ( pStartParse == p ) break;
                pStartParse++;
            }
        }
        if ( rTagCounter == 0 ) break;      /* 루프내에 V 태그가 없을때 */
        if ( EndLoop ) break;
    }

    return LoopCount;
}


/***by icelove addition in 1998.5.20***********************************/

#include <stdarg.h>

/*
	문자열을 html 에서 볼수 있도록 태깅 코드들을 일반화 시킨다.

	char buff[256] = "<font size=5>",
	txt2html(buff,256); 

	이렇게 하면 buff 는 다음과 같이 변환이 된다.

	buff : "&#60font size=5&#62;"

	이렇게 변환이 됨으로써 html 에서는 원래의 문자열인

	<font size=5>  로 보이게 된다.

	int max;  는 문자열 버퍼의 크기이다.
	변환하다가 크기를 넘어가면 변환을 중지하고 되돌아 간다.
*/
/*
void txt2html(char *data, int max)
{
	int i, j, wpos=0, prepos=0, tpos=0;
	char wbuff[2048], prechar;

	wpos=0;
	for(i=0; i<strlen(data); i++)
	{
		prepos = wpos;
		wbuff[wpos] = 0;
		switch( data[i] )
		{
		case '&':
			strcat(wbuff, "&amp;");
			wpos += 5;
			tpos++;
			break;
		case ' ':
			if ( prechar == ' ' )
			{
				strcat(wbuff, "&nbsp;");
				wpos += 6;
			}
			else wbuff[wpos++] = ' ';
			tpos++;
			break;
		case '<':
			strcat(wbuff, "&#60;");
			wpos += 5;
			tpos++;
			break;
		case '>':
			strcat(wbuff, "&#62;");
			wpos += 5;
			tpos++;
			break;
		case '\t':
			for(j=0; j<8-(tpos%8); j++)
			{
				strcat(wbuff, "&nbsp;");
				wpos += 6;
			}
			tpos += j;
			prechar = ' ';
			break;
		case '\n':
			strcat(wbuff, "<br>");
			wpos += 4;
			tpos=0;
			break;
		default:
			prechar = data[i];
			wbuff[wpos++] = data[i];
			tpos++;
			break;
		}
		
		if ( wpos>=max-1 || wpos>=2040 )
		{
			wpos = prepos;
			break;
		}
	}
	wbuff[wpos] = 0;
	strcpy(data, wbuff);
}
*/
void txt2html(char *data, int max)
{
	int i, j, wpos=0;
	char wbuff[2048];

	wpos=0;
	for(i=0; i<strlen(data); i++)
	{
		wbuff[wpos] = 0;
		switch( data[i] )
		{
		case '&':
			strcat(wbuff, "&amp;");
			wpos += 5;
			break;
		case '<':
			strcat(wbuff, "&lt;");
			wpos += 4;
			break;
		case '>':
			strcat(wbuff, "&#62;");
			wpos += 5;
			break;
		case '\n':
			strcat(wbuff, "<br>");
			wpos += 4;
			break;
		default:
			wbuff[wpos++] = data[i];
			break;
		}
		
		if ( wpos>=max-1 || wpos>=2040 ) break;
	}
	wbuff[wpos] = 0;
	strcpy(data, wbuff);
}


/*
	fprintf(cgiOut, format) 을 대치하여 만든것이다.
*/
void Puts(char * fmt, ... )
{
	va_list args;
	char buff[1024];

	va_start(args, fmt);
	vsprintf(buff, fmt, args);

	fputs(buff, cgiOut);
}

/*
	html 형식이 들어간 문자열도 있는 그대로 보여준다.
	문자열 내에 태깅 코드가 들어가더라도 html 에서 그대로 보여줄 수 있도록 변환한다.
*/
void hPuts(char *fmt)
{
	char buff[2048];

	strcpy(buff, fmt);
	txt2html(buff, 2040);
	fputs(buff, cgiOut);
}

/**********end addition 1998.5.20******************************************/
