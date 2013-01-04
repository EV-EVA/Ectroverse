
typedef struct
{
  void (*Init)();
  void (*End)();
  void (*inNew)( svConnectionPtr cnt );
  void (*inNewData)( svConnectionPtr cnt );
  void (*outSendReply)( svConnectionPtr cnt );
  void (*inSendComplete)( svConnectionPtr cnt );
  void (*inClosed)( svConnectionPtr cnt );
  void (*inError)( svConnectionPtr cnt, int type );
  void (*TickStart)();
  void (*TickEnd)();
  int outputsize;
  int timeout;
  int hardtimeout;
} ioInterfaceDef, *ioInterfacePtr;

extern ioInterfaceDef ioInterface[];


enum
{
IO_INTERFACE_HTTP=0,
IO_INTERFACE_EVM,
IO_INTERFACE_NUM,
};




unsigned char *ioCompareWords( unsigned char *string, unsigned char *word );
unsigned char *ioCompareFindWords( unsigned char *string, unsigned char *word );
unsigned char *ioCompareFindBinwords( unsigned char *string, unsigned char *word, int size );
int ioCompareExact( unsigned char *s1, unsigned char *s2 );
int iohttpCompareExt( unsigned char *string, unsigned char *ext );
int iohttpCaseLow( unsigned char *dest, unsigned char *string );



typedef struct
{
  unsigned char path[SERVER_PATH_BUFSIZE];
  unsigned char fileread[SERVER_PATH_BUFSIZE];
  int type; // 0:file, 1:function call

  void *data;
  int size;
  int mime;
  time_t scurtime;
  void (*function)( svConnectionPtr cnt );

  void *next;
  void **prev;
} iohttpFileDef, *iohttpFilePtr;

iohttpFilePtr iohttpFileList;


typedef struct
{
  int flags;

  // http variables
  unsigned char path[SERVER_PATH_BUFSIZE];
  int method;
  int content_length;
  unsigned char *content;
  unsigned char *query_string;
  unsigned char *content_type;
  unsigned char *cookie;
  unsigned char *accept;
  unsigned char *accept_language;
  unsigned char *host;
  unsigned char *referer;
  unsigned char *user_agent;

  // reply
  int code;
  iohttpFilePtr file;

} iohttpDataDef, *iohttpDataPtr;






typedef struct
{
  int status;
} ioevmDataDef, *ioevmDataPtr;






