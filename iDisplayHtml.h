/*
    icelove's Display Html

    1999/3/5
*/

#ifndef _iDISPLAYHTML_H_
#define _iDISPLAYHTML_H_


#define DATATYPESTRING  0
#define DATATYPEFILE    1

/* Single Linked List */
struct _data {
    char *Data;
    int  Type;
    struct _data *Next;
};

/* Multi Linked List */
struct _node {
    long MaxList;       /* 가지고 있는 데이타의 갯수 */

    char *Name;
    struct _data *Header;
    struct _data *Reader;
    struct _node *Next;
};

struct _list {
    long NodeCount;
    long DataCount;

    struct _node *Node;
};

typedef struct _data  DATA;
typedef struct _data* pDATA;
typedef struct _node  NODE;
typedef struct _node* pNODE;
typedef struct _list* LIST;


LIST listCreate(void);
void listDelete(LIST list);

int  listPut(LIST list, char *name, char *val);
int  listPutf(LIST list, char *name, char *fmt, ... );
int  listPutFile(LIST list, char *name, char *fname);
int  listPutfFile(LIST list, char *name, char *fmt, ... );
int  listGet(LIST list, char *name, char *buff);
void listReverse(LIST list, char *qName);

void listDisplay(LIST list, char *HtmlName);
void listShowInfo(LIST list, char *fname);

#endif
