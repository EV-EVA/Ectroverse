#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>


#include "config.h"
#include "artefact.h"
#include "db.h"
#include "sv.h"
#include "io.h"
#include "cmd.h"


svConnectionPtr svConnectionList = 0;
fd_set svSelectRead;
fd_set svSelectWrite;
fd_set svSelectError;



#define SV_INTERFACES 2

int svListenPort[SV_INTERFACES] = { 9122, 9135};
int svListenIO[SV_INTERFACES] = { 0, 1 };
int svListenSocket[SV_INTERFACES];

svConnectionPtr svDebugConnection;


#include "svban.c"


int svTime()
{
  struct timeval lntime;
  gettimeofday( &lntime, 0 );
  return ( lntime.tv_sec * 1000 ) + ( lntime.tv_usec / 1000 );
/*
  return time( 0 );
*/
}



int svInit()
{
  int a, b;
  struct sockaddr_in sinInterface;
  //u_long nInterfaceAddr;

  for( b = 0 ; b < SV_INTERFACES ; b++ )
  {
    svListenSocket[b] = socket( AF_INET, SOCK_STREAM, 0 );
    if( svListenSocket[b] == -1 )
    {
      printf( "Error %03d, creating listening socket\n", errno );
      continue;
    }
    a = 1;
/*
getsockopt
Send : 16384
Recv : 87380
*/
    if( setsockopt( svListenSocket[b], SOL_SOCKET, SO_REUSEADDR, (char *)&a, sizeof(int) ) == -1 )
    {
      printf( "Error %03d, setsockopt\n", errno );
      close( svListenSocket[b] );
      svListenSocket[b] = -1;
      continue;
    }
    sinInterface.sin_family = AF_INET;
    //sinInterface.sin_addr.s_addr = inet_addr("212.79.239.2");
    //Should work on all server now
    sinInterface.sin_addr.s_addr = htonl(INADDR_ANY);
    sinInterface.sin_port = htons( svListenPort[b] );
    if( bind( svListenSocket[b], (struct sockaddr *)&sinInterface, sizeof(struct sockaddr_in) ) == -1 )
    {
      printf( "Error %03d, binding listening socket to port %d\n", errno, svListenPort[b] );
      close( svListenSocket[b] );
      svListenSocket[b] = -1;
      continue;
    }
    if( listen( svListenSocket[b], SOMAXCONN ) == -1 )
    {
      printf( "Error %03d, listen\n", errno );
      close( svListenSocket[b] );
      svListenSocket[b] = -1;
      continue;
    }
    if( fcntl( svListenSocket[b], F_SETFL, O_NONBLOCK ) == -1 )
    {
      printf( "Error %03d, setting the listening socket to non-blocking\n", errno );
      close( svListenSocket[b] );
      svListenSocket[b] = -1;
      continue;
    }
    printf( "Server awaiting connections on port %d...\n", svListenPort[b] );
  }

  return 1;
}


void svEnd()
{
  int a;
  svConnectionPtr cnt, next;
  for( cnt = svConnectionList ; cnt ; cnt = next )
  {
    next = cnt->next;
    svFree( cnt );
  }
  for( a = 0 ; a < SV_INTERFACES ; a++ )
  {
    if( svListenSocket[a] == -1 )
      continue;
    if( shutdown( svListenSocket[a], 2 ) == -1 )
      printf( "Error %03d, unable to shutdown listening socket\n", errno );
    if( close( svListenSocket[a] ) == -1 )
      printf( "Error %03d, closing socket\n", errno );
  }
  printf( "Server shut down\n" );
  return;
}

int svListen ()

{
  int a, b, socket;
  struct sockaddr_in sockaddr;
  svConnectionPtr cnt;
  ioInterfacePtr io;
/*
  struct linger linger;
*/

  for( b = 0 ; b < SV_INTERFACES ; b++ )
  {
    if( svListenSocket[b] == -1 )
      continue;

    a = sizeof( struct sockaddr_in );
    socket = accept( svListenSocket[b], (struct sockaddr *)(&sockaddr), &a );
    if( socket == -1 )
    {
      if( errno == EWOULDBLOCK )
        continue;
      
      printf( "Error %03d, failed to accept a connection %s\n", errno, inet_ntoa(sockaddr.sin_addr) );
      continue;
      printf("if you see this there a big problem\n");
    }
    if( socket >= FD_SETSIZE )
    {
      printf( "Error, socket >= FD_SETSIZE, %d\n", socket );
      if( close( socket ) == -1 )
        printf( "Error %03d, unable to close socket\n", errno );
      continue;
    }
#if SERVER_REPORT_CONNECT == 1
    else
      printf( "Accepting connection from %s:%d>%d\n", inet_ntoa( sockaddr.sin_addr ), ntohs( sockaddr.sin_port ), socket );
#endif

    if( !( cnt = malloc( sizeof(svConnectionDef) ) ) )
    {
      printf( "ERROR, not enough memory to create a connection structure\n" );
      if( close( socket ) == -1 )
        printf( "Error %03d, unable to close socket\n", errno );
      continue;
    }
    cnt->socket = 0;
    cnt->next = 0;
    cnt->previous = 0;
    cnt->time = 0;
    cnt->flags = 0;
    cnt->sendbuf = 0;
    cnt->sendbufpos = 0;
    cnt->sendpos = 0;
    cnt->sendsize = 0;
    cnt->sendflushbuf = 0;
    cnt->sendflushpos = 0;
    cnt->sendstatic = 0;
    cnt->sendstaticsize = 0;
    cnt->io = 0;
    cnt->iodata = 0;
    cnt->dbuser = 0;

    cnt->recv_buf[SERVER_RECV_BUFSIZE] = 0;
    cnt->recv_pos = 0;
    cnt->recv = cnt->recv_buf;
    cnt->recv_max = SERVER_RECV_BUFSIZE;


#if SERVER_NAGLE_BUFFERING == 0
    a = 1;
    if( setsockopt( socket, IPPROTO_TCP, TCP_NODELAY, (char *)&a, sizeof(int) ) == -1 )
      printf( "Error %03d, setsockopt\n", errno );
#endif

/*
   linger.l_onoff  = 1;
   linger.l_linger = 2;
   if( setsockopt( socket, SOL_SOCKET, SO_LINGER, (char *)&linger, sizeof(linger) ) == -1 )
      printf( "Error %03d, setsockopt\n", errno );
*/

    if( fcntl( socket, F_SETFL, O_NONBLOCK ) == -1 )
      printf( "Error %03d, setting a socket to non-blocking\n", errno );

    cnt->socket = socket;
    memcpy( &(cnt->sockaddr), &sockaddr, sizeof(struct sockaddr_in) );
    cnt->time = svTime();

    cnt->previous = (void **)&(svConnectionList);
    cnt->next = svConnectionList;
    if( svConnectionList )
      svConnectionList->previous = &(cnt->next);
    svConnectionList = cnt;

    io = cnt->io = &ioInterface[svListenIO[b]];
    svSendInit( cnt, io->outputsize );
    io->inNew( cnt );
  }

  return 1;
}


void svSelect()
{
  int a, rmax;
  svConnectionPtr cnt;
  struct timeval to;
  FD_ZERO( &svSelectRead );
  FD_ZERO( &svSelectWrite );
  FD_ZERO( &svSelectError );
  rmax = 0;
  for( a = 0 ; a < SV_INTERFACES ; a++ )
  {
    if( svListenSocket[a] == -1 )
      continue;
    FD_SET( svListenSocket[a], &svSelectRead );
    if( svListenSocket[a] > rmax )
      rmax = svListenSocket[a];
  }

  for( cnt = svConnectionList ; cnt ; cnt = cnt->next )
  {
    if( cnt->socket > rmax )
      rmax = cnt->socket;
    FD_SET( cnt->socket, &svSelectError );
    if( cnt->flags & ( SV_FLAGS_NEED_WRITE | SV_FLAGS_WRITE_BUFFERED | SV_FLAGS_WRITE_STATIC | SV_FLAGS_TO_CLOSE ) )
      FD_SET( cnt->socket, &svSelectWrite );
    else
      FD_SET( cnt->socket, &svSelectRead );
  }

  to.tv_usec = ( SERVER_SELECT_MSEC % 1000 ) * 1000;
  to.tv_sec = SERVER_SELECT_MSEC / 1000;
  if( select( rmax+1, &svSelectRead, &svSelectWrite, &svSelectError, &to ) < 0 )
  {
    printf( "Error %03d, select()\n", errno );
    return;
  }
  return;
}


void svRecv()
{
  int a, b, time;
  ioInterfacePtr io;
  svConnectionPtr cnt, next;
  time = svTime();

  for( cnt = svConnectionList ; cnt ; cnt = next )
  {
    svDebugConnection = cnt;

    next = cnt->next;
    io = cnt->io;
    if( ( time - cnt->time >= io->timeout ) && !( cnt->flags & SV_FLAGS_TIMEOUT ) )
    {
#if SERVER_REPORT_ERROR == 1
      printf( "%s>%d Timeout : %d\n", inet_ntoa( (cnt->sockaddr).sin_addr ), cnt->socket, errno );
#endif
      io->inError( cnt, 0 );
      cnt->flags |= SV_FLAGS_TIMEOUT;
    }
    if( time - cnt->time >= io->hardtimeout )
    {
#if SERVER_REPORT_ERROR == 1
      printf( "%s>%d Hard timeout : %d\n", inet_ntoa( (cnt->sockaddr).sin_addr ), cnt->socket, errno );
#endif
      svFree( cnt );
      continue;
    }

    if( cnt->flags & SV_FLAGS_WAIT_CLOSE )
    {
      a = recv( cnt->socket, &b, 4, 0 );
      if( ( a == 0 ) || ( ( a == -1 ) && ( errno != EWOULDBLOCK ) ) )
      {
        io->inClosed( cnt );
        svFree( cnt );
      }
      continue;
    }

    if( ( cnt->flags & ( SV_FLAGS_NEED_WRITE | SV_FLAGS_WRITE_BUFFERED | SV_FLAGS_WRITE_STATIC ) ) && ( FD_ISSET( cnt->socket, &svSelectWrite ) ) )
    {
      if( cnt->flags & SV_FLAGS_NEED_WRITE )
      {
        io->outSendReply( cnt );
        cnt->flags &= 0xFFFFFFFF - SV_FLAGS_NEED_WRITE;
        cnt->flags |= SV_FLAGS_WRITE_BUFFERED;
      }
      if( cnt->flags & SV_FLAGS_WRITE_BUFFERED )
      {
        if( svSendFlush( cnt ) )
          cnt->flags &= 0xFFFFFFFF - SV_FLAGS_WRITE_BUFFERED;
        cnt->time = time;
      }
      if( ( cnt->flags & ( SV_FLAGS_WRITE_STATIC | SV_FLAGS_WRITE_BUFFERED ) ) == SV_FLAGS_WRITE_STATIC )
      {
        svSendStaticFlush( cnt );
        if( !( cnt->sendstaticsize ) )
          cnt->flags &= 0xFFFFFFFF - SV_FLAGS_WRITE_STATIC;
        cnt->time = time;
      }
      if( !( cnt->flags & ( SV_FLAGS_WRITE_STATIC | SV_FLAGS_WRITE_BUFFERED ) ) )
        io->inSendComplete( cnt );
    }

    if( cnt->flags & SV_FLAGS_TO_CLOSE )
    {
      svShutdown( cnt );
      cnt->flags |= SV_FLAGS_WAIT_CLOSE;
      continue;
    }

    if( !( FD_ISSET( cnt->socket, &svSelectRead ) ) && !( FD_ISSET( cnt->socket, &svSelectError ) ) )
      continue;

    if( cnt->recv_pos == cnt->recv_max )
      io->inError( cnt, 1 );

    a = recv( cnt->socket, &((cnt->recv)[cnt->recv_pos]), cnt->recv_max - cnt->recv_pos, 0 );
    if( a <= 0 )
    {
      if( ( a == -1 ) && ( errno == EWOULDBLOCK ) )
        continue;
#if SERVER_REPORT_ERROR == 1
      printf( "Connection to %s>%d died : %d\n", inet_ntoa( (cnt->sockaddr).sin_addr ), cnt->socket, errno );
#endif
      io->inClosed( cnt );
      svFree( cnt );
      continue;
    }
    cnt->recv_pos += a;
    cnt->time = time;
    cnt->flags &= 0xFFFFFFFF - SV_FLAGS_TIMEOUT;

    io->inNewData( cnt );

  }
  return;
}




void svShutdown( svConnectionPtr cnt )
{
#if SERVER_REPORT_CLOSE == 1
  printf( "Shuting down connection to %s:%d>%d\n", inet_ntoa( cnt->sockaddr.sin_addr ), ntohs( cnt->sockaddr.sin_port ), cnt->socket );
#endif
  shutdown( cnt->socket, 1 );
  return;
}

void svClose( svConnectionPtr cnt )
{
#if SERVER_REPORT_CLOSE == 1
  printf( "Closed connection to %s:%d>%d\n", inet_ntoa( cnt->sockaddr.sin_addr ), ntohs( cnt->sockaddr.sin_port ), cnt->socket );
#endif
  if( close( cnt->socket ) == -1 )
    printf( "Error %03d, closing socket\n", errno );
  cnt->socket = -1;
  return;
}

int svFree( svConnectionPtr cnt )
{
  svConnectionPtr next;
  if( cnt->socket != -1 )
    svClose( cnt );
#if SERVER_REPORT_CLOSE == 1
  printf( "Freed connection to %s:%d\n", inet_ntoa( cnt->sockaddr.sin_addr ), ntohs( cnt->sockaddr.sin_port ) );
#endif
  svSendEnd( cnt );
  next = cnt->next;
  *(cnt->previous) = (void *)next;
  if( next )
    next->previous = cnt->previous;
  if( cnt->iodata )
  {
    free( cnt->iodata );
    cnt->iodata = 0;
  }
  free( cnt );
  return 1;
}





int svSendAddBuffer( svBufferPtr *bufferp, int size )
{
  svBufferPtr buffer;
  unsigned char *mem;
  if( !( mem = malloc( sizeof(svBufferDef) + size ) ) )
  {
    printf( "Error %03d, add buffer malloc\n", errno );
    return 0;
  }
  buffer = (svBufferPtr)mem;
  buffer->data = &mem[sizeof(svBufferDef)];

  buffer->prev = (void **)&(bufferp);
  buffer->next = 0;
  *bufferp = buffer;

  return 1;
}

void svSendInit( svConnectionPtr cnt, int size )
{
  svSendEnd( cnt );
  if( !( svSendAddBuffer( &(cnt->sendbuf), size ) ) )
    return;
  cnt->sendsize = size;
  cnt->sendbufpos = cnt->sendbuf;
  cnt->sendpos = 0;
  cnt->sendflushbuf = cnt->sendbuf;
  cnt->sendflushpos = 0;
  return;
}

void svSendEnd( svConnectionPtr cnt )
{
  svBufferPtr buffer, next;
  for( buffer = cnt->sendbuf ; buffer ; buffer = next )
  {
    next = buffer->next;
    free( buffer );
  }
  cnt->sendbuf = 0;
  return;
}

int svSendFlush( svConnectionPtr cnt )
{
  int a, size;
  for( ; cnt->sendflushbuf ; cnt->sendflushbuf = (cnt->sendflushbuf)->next )
  {
    if( (cnt->sendflushbuf)->next )
      size = cnt->sendsize - cnt->sendflushpos;
    else
      size = cnt->sendpos - cnt->sendflushpos;
    a = send( cnt->socket, &(cnt->sendflushbuf)->data[cnt->sendflushpos], size, 0 );
    if( a == -1 )
    {
      if( errno == EWOULDBLOCK )
        return 0;
      printf( "Error %d, send\n", errno );
      return 1;
    }
    if( a != size )
    {
      cnt->sendflushpos += a;
      return 0;
    }
    cnt->sendflushpos = 0;
  }
  cnt->sendbufpos = cnt->sendbuf;
  cnt->sendpos = 0;
  cnt->sendflushbuf = cnt->sendbuf;
  cnt->sendflushpos = 0;
  return 1;
}

void svSend( svConnectionPtr cnt, void *data, int size )
{
  int mem;
  svBufferPtr buffer;
  for( buffer = cnt->sendbufpos ; ; )
  {
    mem = cnt->sendsize - cnt->sendpos;
    if( size <= mem )
    {
      memcpy( &buffer->data[cnt->sendpos], data, size );
      cnt->sendpos += size;
      return;
    }
    else
    {
      memcpy( &buffer->data[cnt->sendpos], data, mem );
      cnt->sendpos += mem;
      size -= mem;
      data += mem;
      if( !( svSendAddBuffer( (svBufferPtr *)&(buffer->next), cnt->sendsize ) ) )
        return;
      buffer = cnt->sendbufpos = buffer->next;
      cnt->sendpos = 0;
    }
  }
  return;
}


void svSendString( svConnectionPtr cnt, char *string )
{
  svSend( cnt, string, strlen(string) );
  return;
}

void svSendPrintf( svConnectionPtr cnt, char *string, ... )
{
  int len;
  char text[4096];
  va_list ap;
  va_start( ap, string );
  len = vsnprintf( text, 4096, string, ap );
  va_end( ap );
  svSend( cnt, text, len );
  return;
}



void svSendStaticFlush( svConnectionPtr cnt )
{
  int a;
  a = send( cnt->socket, cnt->sendstatic, cnt->sendstaticsize, 0 );
  if( a == -1 )
   return;
  cnt->sendstatic += a;
  cnt->sendstaticsize -= a;
  return;
}

void svSendStatic( svConnectionPtr cnt, void *data, int size )
{
  cnt->sendstatic = data;
  cnt->sendstaticsize = size;
  cnt->flags |= SV_FLAGS_WRITE_STATIC;
  return;
}




int svDebugTickPass;
int svDebugTickId;

void svSignal( int signal )
{
  int a, size;
  FILE *fFile;  
    
    
  if(signal == SIGUSR1)
  {
  	svRoundEnd = 1;
		if( ( fFile = fopen( SV_TICK_FILE, "r+" ) ) )
	  {
	   	fscanf( fFile, "%d", &a );
	   //	fprintf(fFile, " %d", svRoundEnd);
	   	fclose( fFile );
	  }
  	return;
  }
  if(signal == SIGUSR2)
  {
  	//Free memory db and reload it to have a new member in :P
  	printf("Ask a dbinit\n");
  	dbEnd();
  	dbInit();
  	return;
  }
  
  iohttpDataPtr iohttp;
  printf( "ERROR, signal %d\n", signal );
  fflush( stdout );
  printf( "cnt : %d\n", (int)svDebugConnection);
  fflush( stdout );
  printf( "tick pass : %d\n", svDebugTickPass );
  fflush( stdout );
  printf( "tick id : %d\n", svDebugTickId );
  fflush( stdout );
  if( svDebugConnection )
  {
    printf( "OK\n" );
    fflush( stdout );

    iohttp = svDebugConnection->iodata;

    printf( "OK\n" );
    fflush( stdout );

    printf( "iohttp : %d\n", (int)iohttp );
    fflush( stdout );
    printf( "iohttp->path : %s\n", iohttp->path );
    fflush( stdout );
    printf("iottp content lenth: %d\n", iohttp->content_length);
    fflush(stdout);
    printf( "iohttp->content : " );
    fwrite( iohttp->content, 1, iohttp->content_length, stdout );
    fflush( stdout );
    printf( "\niohttp->query_string : %s\n", iohttp->query_string );
    fflush( stdout );
    printf( "iohttp->cookie : %s\n", iohttp->cookie );
    fflush( stdout );
    printf( "iohttp->referer : %s\n", iohttp->referer );
    fflush( stdout );
    printf( "iohttp->user_agent : %s\n", iohttp->user_agent );
    fflush( stdout );

    for( ; svDebugConnection->sendflushbuf ; svDebugConnection->sendflushbuf = (svDebugConnection->sendflushbuf)->next )
    {
      if( (svDebugConnection->sendflushbuf)->next )
        size = svDebugConnection->sendsize - svDebugConnection->sendflushpos;
      else
        size = svDebugConnection->sendpos - svDebugConnection->sendflushpos;
      a = size;
      fwrite( &(svDebugConnection->sendflushbuf)->data[svDebugConnection->sendflushpos], 1, size, stdout );
      if( a == -1 )
      {
        if( errno == EWOULDBLOCK )
          return;
        printf( "Error %d, send\n", errno );
        return;
      }
      if( a != size )
      {
        svDebugConnection->sendflushpos += a;
        break;
      }
      svDebugConnection->sendflushpos = 0;
    }
  }
  exit( 1 );
}


int svTickNum = 0;
int svTickTime;
int svTickStatus = 0;
int svRoundEnd = 0;

int svTickAutoStart = 1;


int main()
{
  int a, curtime;
  FILE *file;
  ioInterfacePtr io;
  time_t date;

  svTickTime = time(0) + SV_TICK_TIME;
	
	if( ( file = fopen( SV_TICK_FILE, "r" ) ) )
	  {
	    fscanf( file, "%d", &a );
	  //  fscanf( file, "%d", &svRoundEnd );
	    fclose( file );
	  }
	

  signal( SIGPIPE, SIG_IGN );
  signal( SIGHUP, SIG_IGN );

  signal( SIGFPE, &svSignal );
  signal( SIGUSR2, &svSignal);
  signal( SIGBUS, &svSignal );
  signal( SIGILL, &svSignal );
  signal( SIGINT, &svSignal );
  signal( SIGKILL, &svSignal );
  signal( SIGPOLL, &svSignal );
  signal( SIGPROF, &svSignal );
  signal( SIGQUIT, &svSignal );
  signal( SIGSEGV, &svSignal );
  signal( SIGSYS, &svSignal );
  signal( SIGTERM, &svSignal );
  signal( SIGUSR1, &svSignal );
  signal( SIGTRAP, &svSignal );
	
	srand( time(NULL) ); //Random Init
	
  if( !( svInit() ) )
  {
    printf( "Server initialisation failed, exiting\n" );
    return 0;
  }
  if( !( dbInit() ) )
    return 0;
  for( a = 0 ; a < IO_INTERFACE_NUM ; a++ )
  {
  	io = &ioInterface[a];
    io->Init();
  }
    if( !( cmdInit() ) )
    	return 0;
    if( chdir( DB_DIRECTORY ) == -1 )
    return 0;
    if( ( file = fopen( SV_TICK_FILE, "r" ) ) )
  {
    fscanf( file, "%d", &svTickNum );
    fclose( file );
  }
	
	if( ( svTickAutoStart == 1 ) && ( !svTickStatus ) && (svTickNum))
		svTickStatus = 1;
		
	//printf("Main Loop start\n");
  for( ; ; )
  {
  				
    svSelect();
    svListen();
    svRecv();

    svDebugConnection = 0;
    
		
    curtime = time( 0 );
    if( curtime < svTickTime )
      continue;
      
    //printf("date = %s, config= %s === %d\n", ctime(&curtime), STOP_TIME, strstr(ctime(&curtime),STOP_TIME));
		if(strstr(ctime(&curtime), START_TIME))
			svTickStatus = 1;
		
		if(strstr(ctime(&curtime), STOP_TIME))
			svTickStatus = 0;
			
    svTickTime += SV_TICK_TIME;
		
    for( a = 0 ; a < IO_INTERFACE_NUM ; a++ )
    {
      io = &ioInterface[a];
      io->TickStart();
    }
		
		//printf("Tick init\n");
    cmdTickInit();
    if( svTickStatus )
    {
			cmdTick();
      svTickNum++;
    }
    //printf("Tick end\n");
    cmdTickEnd();

    for( a = 0 ; a < IO_INTERFACE_NUM ; a++ )
    {
      io = &ioInterface[a];
      io->TickEnd();
    }

    file = fopen( SV_TICK_FILE, "r+" );
    if(!file)
    	file = fopen( SV_TICK_FILE, "w" );
    if(file)
    {
      fprintf( file, "%d", svTickNum );
      fclose( file );
    }

    cmdExecuteFlush();

  }
  cmdEnd();
  dbEnd();
  for( a = 0 ; a < IO_INTERFACE_NUM ; a++ )
  {
    io = &ioInterface[a];
    io->End();
  }
  svEnd();
  return 1;
}



