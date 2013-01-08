#include <stdio.h>
#include <stdlib.h>
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


unsigned char dbFileUsersName[] = "%s/users";
unsigned char dbFileMapName[] = "map";
unsigned char dbFileMarketName[] = "market";

#define DB_FILE_NUMBER 3
#define DB_FILE_USERS 0
#define DB_FILE_MAP 1
#define DB_FILE_MARKET 2

unsigned char *dbFileList[DB_FILE_NUMBER] = { dbFileUsersName, dbFileMapName, dbFileMarketName };
FILE *dbFilePtr[DB_FILE_NUMBER];



unsigned char dbFileUserUserName[] = "%s/user%d/user";
unsigned char dbFileUserMainName[] = "%s/user%d/main";
unsigned char dbFileUserBuildName[] = "%s/user%d/build";
unsigned char dbFileUserPlanetsName[] = "%s/user%d/planets";
unsigned char dbFileUserFleetsName[] = "%s/user%d/fleets";
unsigned char dbFileUserNewsName[] = "%s/user%d/news";
unsigned char dbFileUserMarketName[] = "%s/user%d/market";
unsigned char dbFileUserMailInName[] = "%s/user%d/mailin";
unsigned char dbFileUserMailOutName[] = "%s/user%d/mailout";
unsigned char dbFileUserSpecOpsName[] = "%s/user%d/specops";
unsigned char dbFileUserRecordName[] = "%s/user%d/record";
unsigned char dbFileUserFlags[] = "%s/user%d/user";


#define DB_FILE_USER_NUMBER 12

#define DB_FILE_USER_USER 0
#define DB_FILE_USER_MAIN 1
#define DB_FILE_USER_BUILD 2
#define DB_FILE_USER_PLANETS 3
#define DB_FILE_USER_FLEETS 4
#define DB_FILE_USER_NEWS 5
#define DB_FILE_USER_MARKET 6
#define DB_FILE_USER_MAILIN 7
#define DB_FILE_USER_MAILOUT 8
#define DB_FILE_USER_SPECOPS 9
#define DB_FILE_USER_RECORD 10
#define DB_FILE_USER_USER_FLAGS 11

unsigned char *dbFileUserList[DB_FILE_USER_NUMBER] = { dbFileUserUserName, dbFileUserMainName, dbFileUserBuildName, dbFileUserPlanetsName, dbFileUserFleetsName, dbFileUserNewsName, dbFileUserMarketName, dbFileUserMailInName, dbFileUserMailOutName, dbFileUserSpecOpsName, dbFileUserRecordName, dbFileUserFlags };

long long int dbFileUserListDat0[] = { 0, -1, -1, 0, 0 };
int dbFileUserListDat1[] = { 0, 8 };

int dbFileUserListBase[DB_FILE_USER_NUMBER] = { 0, 0, 4, 4, 4, 40, 8, 8, 8, 4, 4, 0 };
long long int *dbFileUserListData[DB_FILE_USER_NUMBER] = { 0, 0, dbFileUserListDat0, dbFileUserListDat0, dbFileUserListDat0, dbFileUserListDat0, dbFileUserListDat0, dbFileUserListDat1, dbFileUserListDat1, dbFileUserListDat0, dbFileUserListDat0, 0 };


dbMainSystemPtr dbMapSystems;
int dbMapBInfoStatic[7];



int dbArtefactPos[ARTEFACT_NUMUSED];

int dbArtefactMax;



/*
Database format

users
  4:next user ID
  4:number of free IDs
4*X:list of free IDs

map
  4:size X
  4:size Y
  4:number of systems
  4:number of planets
  4:number of families
  4:number of players per empire
  4:reserved
 32:reserved
struct ( 20 )
  4:position ( y << 16 ) + x
  4:index first planet
  4:number of planets
  4:empire home system, -1:none
  4:reserved
struct ( 184 )
  4:system indice
  4:position, ( y << 20 ) + ( x << 8 ) + planet
  4:planet owner ID, none if < 0
  4:size
  4:flags
  4:population
  4:maxpopulation
3*4:special
 64:number of buildings
 64:number of units
  4:construction
  4:protection
  4:surrender
  4:reserved
struct ( 336 )
  4:number of players
128:players ID, 32 fixed maximum
  4:home system ID
  4:home system position ( y << 16 ) + x
 64:empire name
  4:leader ID
 32:vote index in players IDs
  4:picture mime
  4:picture time
  4:planets
  4:networth
  4:artefacts
  4:rank
 72:reserved

market
   4:num max bids
   4:list free
3 * 2 * 80 * 12 ; res*action*prices*struct
struct( 12 )
   4:total of bids
   4:first list ID
   4:last list ID
struct( 16 )
   4:previous id
   4:next id
   4:quantity
   4:owner ID


user%d/user
  4:user ID
  4:level
  4:flags
  4:reserved
 32:name
 32:password

user%d/main
 nn:dbUserMainDef

user%d/build
  4:number of builds
struct ( 20 )
  4:type, |0x10000 if units
  4:quantity
  4:time remaining
  4:planet ID
  4:planet position, ( y << 20 ) + ( x << 8 ) + planet
  4*8:Cost

user%d/planets
  4:number of planets
struct ( 20 )
  4:planet index
  4:system index
  4:location, ( y << 20 ) + ( x << 8 ) + planet
  4:flags, &1:portal
  4:reserved

user%d/fleets
  4:number of fleets
struct ( 96 )
 64:units
  4:order
  4:destination, ( y << 20 ) + ( x << 8 ) + planet
  4:destination planet ID
  4:destination system ID
  4:source, ( y << 20 ) + ( x << 8 )
  4:flags
  4:time
  8:reserved

user%d/news
  8:num
  8:list used
  8:list free
  8:news flags
  8:new num
struct ( 128 )
  8:previous news id
  8:next news id
  8:time
  8:flags
224:news data

user%d/market
  4:number of bids
  4:flags
struct ( 20 )
  4:action
  4:resource
  4:price
  4:quantity
  4:bid ID

user%d/specops
  4:number of specops
struct ( nn )
 nn:dbUserSpecOpDef


fam%d/news
  4:num
  4:list used
  4:list free
  4:news flags
  4:new num
struct ( 128 )
  4:previous news id
  4:next news id
  4:time
  4:flags
112:news data

And Mal forgot the mail file
....


*/


FILE *dbFileGenOpen( int num )
{
	char szSource[500];
	
	if(num == DB_FILE_USERS)
		sprintf(szSource, dbFileList[num], USER_DIRECTORY);
	else
		sprintf(szSource, dbFileList[num]);
	if( dbFilePtr[num] )
    return dbFilePtr[num];
    
  if( !( dbFilePtr[num] = fopen( szSource, "rb+" ) ) )
  {
    printf( "Error %02d, could not open %s\n", errno, szSource );
    return 0;
  }
  return dbFilePtr[num];
}

void dbFileGenClose( int num )
{
  if( dbFilePtr[num] )
  {
    fclose( dbFilePtr[num] );
    dbFilePtr[num] = 0;
  }
  return;
}

void dbFlush()
{
  int a;
  for( a = 0 ; a < DB_FILE_NUMBER ; a++ )
    dbFileGenClose( a );
  return;
}


FILE *dbFileUserOpen( int id, int num )
{
  unsigned char fname[532];
  FILE *file;
  
  if((num&0xFFFF) == DB_FILE_USER_USER)
  	sprintf( fname, dbFileUserList[num&0xFFFF], USER_DIRECTORY, id );
  else
  	sprintf( fname, dbFileUserList[num&0xFFFF], DB_DIRECTORY, id );
  
  if( !( file = fopen( fname, "rb+" ) ) )
  {
		// mooooooo
    if( ( file = fopen( fname, "wb" ) ) )
    {
      fwrite( dbFileUserListData[num&0xFFFF], 1, dbFileUserListBase[num&0xFFFF], file );
      fclose( file );
      return fopen( fname, "rb+" );
    }

    if( num < 0x10000 )
      printf( "Error %02d, fopen %s\n", errno, fname );
    return 0;
  }
  return file;
}

FILE *dbFileFamOpen( int id, int num )
{
  int a, b;
  unsigned char fname[32];
  FILE *file;
  sprintf( fname, "fam%02dn%02d", id, num );
  if( !( file = fopen( fname, "rb+" ) ) )
  {
    if( ( file = fopen( fname, "wb+" ) ) )
    {
      if( num == 0 )
      {
        a = 0;
        fwrite( &a, 1, sizeof(int), file );
      }
      else
      {
        a = 0;
        for( b = 0 ; b < 4096*2/4 ; b++ )
          fwrite( &a, 1, sizeof(int), file );
      }
      fseek( file, 0, SEEK_SET );
      return file;
    }
    if( num < 0x10000 )
      printf( "Error %02d, fopen %s\n", errno, fname );
    return 0;
  }
  return file;
}




dbUserPtr dbUserList;
dbUserPtr dbUserTable[16384];



dbUserPtr dbUserAllocate( int id )
{
  unsigned char pass[32];
  dbUserPtr user;
  if( !( user = malloc( sizeof(dbUserDef) ) ) )
  {
    printf( "Error, malloc dbuser failed\n" );
    return 0;
  }
  memset( user, 0, sizeof(dbUserDef) );
  user->prev = (void **)&(dbUserList);
  user->next = dbUserList;
  if( dbUserList )
    dbUserList->prev = &(user->next);
  dbUserList = user;
  user->id = id;
  user->lasttime = 0;
  dbUserRetrievePassword( id, pass );
  srand( time(0) + pass[1] + 3*pass[3] + 5*pass[5] + 7*pass[7] );
  user->session[0] = rand();
  user->session[1] = rand();
  user->session[2] = rand();
  user->session[3] = rand();
  dbUserTable[id] = user;
  return user;
}

void dbUserFree( dbUserPtr user )
{
  dbUserPtr next;
  next = user->next;
  *(user->prev) = (void *)next;
  if( next )
    next->prev = user->prev;
  free( user );
  if( (unsigned int)user->id >= 16384 )
    return;
  dbUserTable[user->id] = 0;
  return;
}


/*
users
  4:next user ID
  4:number of free IDs
4*X:list of free IDs
*/
int dbInitUsersReset()
{
  int a, last, freenum;
  //dbUserMainDef maind;
  FILE *file;

  if( !( dbFileGenOpen( DB_FILE_USERS ) ) )
    return -3;
  last = 0;
  for( a = 0 ; a < 16384 ; a++ )
  {
    if( !( file = dbFileUserOpen( a, 0x10000 | DB_FILE_USER_MAIN ) ) )
      continue;
    last = a;
    fclose( file );
  }

  fseek( dbFilePtr[DB_FILE_USERS], 8, SEEK_SET );
  freenum = 0;
  for( a = 0 ; a <= last ; a++ )
  {
    if( ( file = dbFileUserOpen( a, 0x10000 | DB_FILE_USER_MAIN ) ) )
    {
      fclose( file );
      continue;
    }
    fwrite( &a, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );
    freenum++;
  }

  fseek( dbFilePtr[DB_FILE_USERS], 0, SEEK_SET );
  last++;
  fwrite( &last, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );

  fseek( dbFilePtr[DB_FILE_USERS], 4, SEEK_SET );
  fwrite( &freenum, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );

  return 1;
}



int dbInit()
{
  int a, b, c;
  int array[4];
  dbUserPtr user;
  dbUserMainDef maind;
  dbMainPlanetDef planetd;
  FILE *file;
  char szUsersFile[500];
  
  if( chdir( DB_DIRECTORY ) == -1 )
  {
    printf( "Error %02d, db chdir, Dir: %s\n", errno, DB_DIRECTORY );
    return 0;
  }

  if( ( dbMapRetrieveMain( dbMapBInfoStatic ) < 0 ) )
    return 0;
  dbMapBInfoStatic[2] = dbMapBInfoStatic[2];
  if( !( dbMapSystems = malloc( dbMapBInfoStatic[2] * sizeof(dbMainSystemDef) ) ) )
    return 0;
  for( a = 0 ; a < dbMapBInfoStatic[2] ; a++ )
    dbMapRetrieveSystem( 0x10000000 | a, &dbMapSystems[a] );

  if( !( dbFileGenOpen( DB_FILE_MARKET ) ) )
  {
    printf( "Market database not found, creating\n" );
    if( !( dbFilePtr[DB_FILE_MARKET] = fopen( dbFileList[DB_FILE_MARKET], "wb+" ) ) )
    {
      printf( "Error, could not create market database!\n" );
      return 0;
    }

    fseek( dbFilePtr[DB_FILE_MARKET], 0, SEEK_SET );
    array[0] = 0;
    array[1] = -1;
    fwrite( array, 1, 2*sizeof(int), dbFilePtr[DB_FILE_MARKET] );

    array[0] = 0;
    array[1] = -1;
    array[2] = -1;
    for( a = 0 ; a < 6*DB_MARKET_RANGE ; a++ )
      fwrite( array, 1, 3*sizeof(int), dbFilePtr[DB_FILE_MARKET] );

    dbFileGenClose( DB_FILE_MARKET );
  }


  if( !( file = fopen( "forums", "rb+" ) ) )
  {
    printf( "No forum database, creating\n" );
    if( !( file = fopen( "forums", "wb+" ) ) )
    {
      printf( "Error, could not create forum database!\n" );
      return 0;
    }
    a = 0;
    fwrite( &a, 1, sizeof(int), file );
  }
  fclose( file );


  if( !( dbFileGenOpen( DB_FILE_USERS ) ) )
  {
    printf( "Users database not found, creating\n" );
    // Create a path to the users file in the same way as dbFileGenOpen
		sprintf( szUsersFile, dbFileList[DB_FILE_USERS], USER_DIRECTORY );
    if( !( dbFilePtr[DB_FILE_USERS] = fopen( szUsersFile, "wb+" ) ) )
    {
      printf( "Error, could not create users database!\n" );
      return 0;
    }
    fseek( dbFilePtr[DB_FILE_USERS], 0, SEEK_SET );
    a = 0;
    fwrite( &a, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );
    fwrite( &a, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );
    dbFileGenClose( DB_FILE_USERS );
    return 1;
  }
  else
  {

dbInitUsersReset();


  }


  fseek( dbFilePtr[DB_FILE_USERS], 0, SEEK_SET );
  fread( &b, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );

  for( a = 0 ; a < b ; a++ )
  {
    if( !( file = dbFileUserOpen( a, 0x10000 | DB_FILE_USER_USER ) ) )
      continue;
    fread( &c, 1, sizeof(int), file );

/*
    printf( "%d, %d\n", a, c );
*/

//    if( !( user = dbUserAllocate( c ) ) )
    if( !( user = dbUserAllocate( a ) ) )
    {
      fclose( file );
      continue;
    }
    
    fread( &user->level, 1, sizeof(int), file );
    fread( &user->flags, 1, sizeof(int), file );
    fread( &user->reserved, 1, sizeof(int), file );
    fread( &user->name, 1, 32, file );
    fclose( file );
    
    if( !( file = dbFileUserOpen( a, 0x10000 | DB_FILE_USER_USER_FLAGS ) ) )
      continue;
    
    fseek( file, sizeof(int)*2, SEEK_SET );
    fread( &user->flags, 1, sizeof(int), file );
    fclose(file);
    dbUserMainRetrieve( a, &maind );
    sprintf( user->faction, maind.faction );
    sprintf( user->forumtag, maind.forumtag );
    user->lasttime = maind.lasttime;
  }

  dbFlush();

  // Find artefacts
  for( a = 0 ; a < dbMapBInfoStatic[3] ; a++ )
  {
    dbMapRetrievePlanet( a, &planetd );
    if( ( b = (int)artefactPrecense( &planetd ) ) < 0 )
      continue;
    dbArtefactPos[b] = planetd.position;
  }
	
  return 1;
}


void dbEnd()
{
  dbUserPtr user, next;
  for( user = dbUserList ; user ; user = next )
  {
    next = user->next;
    dbUserFree( user );
  }
  if( dbMapSystems )
    free( dbMapSystems );
  return;
}




// Quick map search

int dbMapFindSystem( int x, int y )
{
  int a, position = ( y << 16 ) + x;
  for( a = 0 ; a < dbMapBInfoStatic[2] ; a++ )
  {
    if( dbMapSystems[a].position == position )
      return a;
  }
  return -1;
}

int dbMapFindValid( int x, int y )
{
  int binfo[7];
  dbMapRetrieveMain( binfo );
  if( (unsigned int)x >= binfo[0] )
    return 0;
  if( (unsigned int)y >= binfo[1] )
    return 0;
  return 1;
}




// Users functions

int dbUserSearch( unsigned char *name )
{
  dbUserPtr user;
  for( user = dbUserList ; user ; user = user->next )
  {
  	//printf("%s, %s, %d\n", name, user->name, user->id);
  	
  	if( !( ioCompareExact( name, user->name ) ) )
      continue;

    return user->id;
  }
  return -1;
}

int dbUserSearchFaction( unsigned char *name )
{
  dbUserPtr user;
  for( user = dbUserList ; user ; user = user->next )
  {
    if( !( ioCompareExact( name, user->faction ) ) )
      continue;
    //printf("%d this is it\n", user->id);
    return user->id;
  }
  return -1;
}

dbUserPtr dbUserLinkID( int id )
{
/*
  dbUserPtr user;
  for( user = dbUserList ; ; user = user->next )
  {
    if( !( user ) )
      return 0;
    if( id == user->id )
      break;
  }
  return user;
*/
  if( (unsigned int)id >= 16384 )
    return 0;
  return dbUserTable[id];
}


int dbUserAdd( unsigned char *name, unsigned char *faction, unsigned char *forumtag )
{
  int a, id, freenum;
  dbUserPtr user;
  unsigned char dname[532], fname[532], uname[532];
  dbUserDescDef descd;
  FILE *file;

  if( !( dbFileGenOpen( DB_FILE_USERS ) ) )
    return -3;

  fseek( dbFilePtr[DB_FILE_USERS], 4, SEEK_SET );
  fread( &freenum, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );
  if( !( freenum ) )
  {
    fseek( dbFilePtr[DB_FILE_USERS], 0, SEEK_SET );
    fread( &id, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );
  }
  else
  {
    a = freenum - 1;
    fseek( dbFilePtr[DB_FILE_USERS], 8 + ( a << 2 ), SEEK_SET );
    fread( &id, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );
  }
	
  user = dbUserAllocate( id );
  
  //create both folder for player
  sprintf( dname, "%s/user%d", DB_DIRECTORY, id );
  sprintf( uname, "%s/user%d", USER_DIRECTORY, id );
  
  mkdir( dname, S_IRWXU );
  mkdir( uname, S_IRWXU );
  
  printf("both dir created\n");
  
 	//Create a db Database in the db other server
  for( a = DB_FILE_USER_NUMBER-2 ;  ; a-- )
  {
  	sprintf( fname, dbFileUserList[a], DB_DIRECTORY, id );
    
    if( !( file = fopen( fname, "wb+" ) ))
    {
      dbUserFree( user );
      rmdir( dname );
      rmdir( uname );
      printf( "Error %02d, fopen dbuseradd\n", errno );
      return -3;
    }
    if( a == 0 )
      break;
    if( dbFileUserListBase[a] )
      fwrite( dbFileUserListData[a], 1, dbFileUserListBase[a], file );
    fclose( file );
  }

  fwrite( &id, 1, sizeof(int), file );
  a = 0;
  fwrite( &a, 1, sizeof(int), file );
  fwrite( &a, 1, sizeof(int), file );
  fwrite( &a, 1, sizeof(int), file );
  memset( fname, 0, 32 );
  sprintf( fname, name );
  fwrite( fname, 1, 32, file );
  fclose( file );
 
  
  //Create a user Database in the db 10Min server
  for( a = DB_FILE_USER_NUMBER-2 ;  ; a-- )
  {
  	sprintf( fname, dbFileUserList[a], USER_DIRECTORY, id );
    if( !( file = fopen( fname, "wb+" ) ))
    {
      dbUserFree( user );
      rmdir( dname );
      rmdir( uname );
      printf( "Error %02d, fopen dbuseradd\n", errno );
      return -3;
    }
    if( a == 0 )
      break;
    if( dbFileUserListBase[a] )
    {
      //printf("write base of %s", fname);
      fwrite( dbFileUserListData[a], 1, dbFileUserListBase[a], file );
    }
    fclose( file );
  }
	
  fwrite( &id, 1, sizeof(int), file );
  a = 0;
  fwrite( &a, 1, sizeof(int), file );
  fwrite( &a, 1, sizeof(int), file );
  fwrite( &a, 1, sizeof(int), file );
  memset( fname, 0, 32 );
  sprintf( fname, name );
  fwrite( fname, 1, 32, file );
  fclose( file );
	
	if( !( freenum ) )
  {
    fseek( dbFilePtr[DB_FILE_USERS], 0, SEEK_SET );
    a = id + 1;
    fwrite( &a, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );
  }
  else
  {
    a = freenum - 1;
    fseek( dbFilePtr[DB_FILE_USERS], 4, SEEK_SET );
    fwrite( &a, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );
  }

  descd.desc[0] = 0;
  dbUserDescSet( id, &descd );
  //dbEnd();
  //dbInit();
  printf("system kill -n 12 $(pidof sv)\n");
  system("kill -n 12 $(pidof sv)");
  return id;
}

int dbUserRemove( int id )
{
  int a;
  dbUserPtr user;
  unsigned char dname[532], fname[532];

  if( !( user = dbUserLinkID( id ) ) )
    return 0;
  if( !( dbFileGenOpen( DB_FILE_USERS ) ) )
    return 0;
  dbUserFree( user );

  fseek( dbFilePtr[DB_FILE_USERS], 4, SEEK_SET );
  fread( &a, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );
  a++;
  fseek( dbFilePtr[DB_FILE_USERS], 4, SEEK_SET );
  fwrite( &a, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );
  fseek( dbFilePtr[DB_FILE_USERS], ( a + 1 ) << 2, SEEK_SET );
  fwrite( &id, 1, sizeof(int), dbFilePtr[DB_FILE_USERS] );

  for( a = 0 ; a < DB_FILE_USER_NUMBER-1 ; a++ )
  {
    sprintf( fname, dbFileUserList[a], USER_DIRECTORY, id );
    unlink( fname );
  }
  for( a = 0 ; a < DB_FILE_USER_NUMBER-1 ; a++ )
  {
    sprintf( fname, dbFileUserList[a], DB_DIRECTORY, id );
    unlink( fname );
  }
  sprintf( dname, "%s/user%d", USER_DIRECTORY, id );
  rmdir( dname );
  sprintf( dname, "%s/user%d", DB_DIRECTORY, id );
  rmdir( dname );
	printf("system kill -n 12 $(pidof sv)\n");
  system("kill -n 12 $(pidof sv)");

  return 1;
}


int dbUserSave( int id, dbUserPtr user )
{
  FILE *file;
  
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_USER ) ) )
  {
    printf( "Error %02d, fopen dbsetname\n", errno );
    return -3;
  }
  fwrite( &user->id, 1, sizeof(int), file );
  fwrite( &user->level, 1, sizeof(int), file );
  fseek(file, sizeof(int), SEEK_CUR);
  
  fwrite( &user->reserved, 1, sizeof(int), file );
  fwrite( user->name, 1, 32, file );
  fclose( file );
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_USER_FLAGS ) ) )
  {
    printf( "Error %02d, fopen dbsetname\n", errno );
    return -3;
  }
  fseek(file, 2*sizeof(int), SEEK_CUR);
  fwrite( &user->flags, 1, sizeof(int), file );
  fclose(file);
  return 1;
}

int dbUserSetPassword( int id, unsigned char *pass )
{
  unsigned char fname[532];
  FILE *file;
  
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_USER ) ) )
  {
    printf( "Error %02d, fopen dbsetpassword\n", errno );
    return -3;
  }
  fseek( file, 16+32, SEEK_SET );
  memset( fname, 0, 32 );
  sprintf( fname, pass );
  fwrite( fname, 1, 32, file );
  fclose( file );
  return 1;
}

int dbUserRetrievePassword( int id, unsigned char *pass )
{
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_USER ) ) )
  {
    printf( "Error %02d, fopen dbretrievepassword\n", errno );
    return -3;
  }
  fseek( file, 16+32, SEEK_SET );
  fread( pass, 1, 32, file );
  fclose( file );
  return 1;
}

int dbUserLinkDatabase( void *cnt, int id )
{
  dbUserPtr user;
  svConnectionPtr cnt2 = cnt;
  if( id < 0 )
  {
    cnt2->dbuser = 0;
    return 1;
  }
  if( !( user = dbUserLinkID( id ) ) )
    return -2;
  cnt2->dbuser = user;
  user->lasttime = time( 0 );
  return 1;
}




// session functions

int dbSessionSet( dbUserPtr user, char *hash, int *session )
{
  int a, b;
  if( !( user ) )
    return -3;
  b = rand() & 0xFFFF;
  if( hash )
    srand( time(0) + b + 3*hash[0] + 7*hash[2] + 11*hash[4] + 13*hash[6] + 17*hash[8] );
  else
    srand( time(0) + b );
  for( a = 0 ; a < 4 ; a++ )
    user->session[a] = rand() & 0xFFFF;
  memcpy( session, user->session, 4*sizeof(int) );
  return 1;
}

int dbSessionRetrieve( dbUserPtr user, int *session )
{
  if( !( user ) )
    return -3;
  memcpy( session, user->session, 4*sizeof(int) );
  return 1;
}




// user main data functions

int dbUserMainSet( int id, dbUserMainPtr maind )
{
  dbUserPtr user;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_MAIN ) ) )
    return -3;
  fwrite( maind, 1, sizeof(dbUserMainDef), file );
  fclose( file );
  if( !( user = dbUserLinkID( id ) ) )
    return -3;
  sprintf( user->faction, maind->faction );
  sprintf( user->forumtag, maind->forumtag );
  return 1;
}

int dbUserMainRetrieve( int id, dbUserMainPtr maind )
{
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_MAIN ) ) )
    return -3;


// CHANGING ST SIZE
  memset( maind, 0, sizeof(dbUserMainDef) );


  fread( maind, 1, sizeof(dbUserMainDef), file );
  fclose( file );
  return 1;
};





// user build functions

int dbUserBuildAdd( int id, int type, long long int *cost, int quantity, int time, int plnid, int plnloc )
{
  int pos, i;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_BUILD ) ) )
    return -3;  
  fread( &pos, 1, sizeof(int), file );
  fseek( file, 4+(pos*52), SEEK_SET );
  fwrite( &type, 1, sizeof(int), file );
  fwrite( &quantity, 1, sizeof(int), file );
  fwrite( &time, 1, sizeof(int), file );
  fwrite( &plnid, 1, sizeof(int), file );
  fwrite( &plnloc, 1, sizeof(int), file );
  for(i=0;i<4;i++)
  	fwrite(&cost[i], 1, sizeof(long long int), file);
  fseek( file, 0, SEEK_SET );
  pos++;
  fwrite( &pos, 1, sizeof(int), file );
  fclose( file );
  return pos;
}

int dbUserBuildRemove( int id, int bldid )
{
  int a, num, data[13];
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_BUILD ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( (unsigned int)bldid >= num )
  {
    fclose( file );
    return -2;
  }
  if( bldid+1 < num )
  {
    fseek( file, 4+(num*52)-52, SEEK_SET );
    fread( data, 1, 52, file );
    fseek( file, 4+(bldid*52), SEEK_SET );
    fwrite( data, 1, 52, file );
  }

  fseek( file, 0, SEEK_SET );
  a = num-1;
  fwrite( &a, 1, sizeof(int), file );
  fclose( file );
  return 1;
}

int dbUserBuildList( int id, dbUserBuildPtr *build )
{
  int a, num, i;
  dbUserBuildPtr buildp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_BUILD ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( !( buildp = malloc( num*sizeof(dbUserBuildDef) ) ) )
  {
    fclose( file );
    return -1;
  }
  for( a = 0 ; a < num ; a++ )
  {
    fread( &buildp[a].type, 1, sizeof(int), file );
    fread( &buildp[a].quantity, 1, sizeof(int), file );
    fread( &buildp[a].time, 1, sizeof(int), file );
    fread( &buildp[a].plnid, 1, sizeof(int), file );
    fread( &buildp[a].plnpos, 1, sizeof(int), file );
  	for(i=0;i<4;i++)
  		fread(&buildp[a].cost[i], 1, sizeof(long long int), file);
  }
  fclose( file );
  *build = buildp;
  return num;
}


void sortlist ( int num, int *list1, int *list2 )
{
  int b, c, smaller;
  int *list3;
  
  if( !( list3 = malloc( (num)*sizeof(int) ) ) )
    return;
  
  for( b = 0 ; b < num ; b++ )
    {
      smaller = 0;
//      list3[0] = list1[0];
      for ( c = 0 ; c < num; c++ )
	{
	  if ( b != c )
	    {
	      if ( ( list2[b] > list2[c] ) || ( ( list2[b] == list2[c]) && ( list1[b] > list1[c] ) ) )
		smaller++;
	    }
	}
      list3[smaller] = list1[b];
    }
  
  for( b = 0 ; b < num ; b++ )
    list1[b] = list3[b];

 // printf("Dit is void sortlist\n");
  free (list3);
  return;

}

void sortlist2 ( int num, int *list1, int *list2, int *list3 )
{
  int b, c, smaller;
  int *list4;
  
  if( !( list4 = malloc( (num)*sizeof(int) ) ) )
    return;
  
  for( b = 0 ; b < num ; b++ )
    {
      smaller = 0;
      //list4[0] = list1[0];
      for ( c = 0 ; c < num; c++ )
	{
	  if ( b != c )
	    {
	      if ( ( list2[b] > list2[c] ) || ( ( list2[b] == list2[c]) && ( list3[b] > list3[c] ) ) || ( ( list2[b] == list2[c]) && ( list3[b] == list3[c] ) && ( list1[b] > list1[c] ) ) )
		smaller++;
	    }
	}
      list4[smaller] = list1[b];
    }
  
  for( b = 0 ; b < num ; b++ )
    list1[b] = list4[b];
  
 // printf("dees is void sortlist2 \n");
  free (list4);
  return;
}

int dbUserBuildListReduceTime( int id, dbUserBuildPtr *build )
{
  int a, num, i;
  dbUserBuildPtr buildp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_BUILD ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( !( buildp = malloc( num*sizeof(dbUserBuildDef) ) ) )
  {
    fclose( file );
    return -1;
  }
  for( a = 0 ; a < num ; a++ )
  {
    fread( &buildp[a].type, 1, sizeof(int), file );
    fread( &buildp[a].quantity, 1, sizeof(int), file );
    fread( &buildp[a].time, 1, sizeof(int), file );
    buildp[a].time--;
    fseek( file, -4, SEEK_CUR );
    fwrite( &buildp[a].time, 1, sizeof(int), file );
    fread( &buildp[a].plnid, 1, sizeof(int), file );
    fread( &buildp[a].plnpos, 1, sizeof(int), file );
    for(i=0;i<4;i++)
    	fread(&buildp[a].cost[i], 1, sizeof(long long int), file);
  }
  fclose( file );
  *build = buildp;
  return num;
}

int dbUserBuildEmpty( int id )
{
  int a;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_BUILD ) ) )
    return -3;
  a = 0;
  fwrite( &a, 1, sizeof(int), file );
  fclose( file );
  return 1;
}






// user planets functions

int dbUserPlanetNumber( int id )
{
  int pos;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  fread( &pos, 1, sizeof(int), file );
  fclose( file );
  return pos;
}

int dbUserPlanetAdd( int id, int plnid, int sysid, int plnloc, int flags )
{
  int a, pos;
  dbUserMainDef maind;
  dbMainEmpireDef empired;
  dbMainPlanetDef Planetd;
  FILE *file;
  
  
  //Arti Code so if someone get it, it reset all their count return to 0
	dbMapRetrievePlanet(plnid, &Planetd);
	if(artefactPrecense(&Planetd) == 6)
  {
  	dbMapRetrieveEmpire( id, &empired );
  	for( a = 0 ; a < empired.numplayers ; a++ )
    {
  		if( dbUserMainRetrieve( empired.player[a], &maind ) < 0 )
        continue;
      maind.articount = 0; //Reset their count to zero so it become inefective
      dbUserMainSet( empired.player[a], &maind );
    }
	}
	
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  fread( &pos, 1, sizeof(int), file );
  fseek( file, 4+(pos*20), SEEK_SET );
  fwrite( &plnid, 1, sizeof(int), file );
  fwrite( &sysid, 1, sizeof(int), file );
  fwrite( &plnloc, 1, sizeof(int), file );
  fwrite( &flags, 1, sizeof(int), file );
  a = 0;
  fwrite( &a, 1, sizeof(int), file );
  fseek( file, 0, SEEK_SET );
  pos++;
  fwrite( &pos, 1, sizeof(int), file );
  fclose( file );
  return pos;
}

int dbUserPlanetRemove( int id, int plnid )
{
  int a, b, num, data[5];
  dbUserMainDef maind;
  dbMainEmpireDef empired;
  dbMainPlanetDef Planetd;
  FILE *file;
  
  
  //Arti Code so if someone lose it all their count return to 0
	dbMapRetrievePlanet(plnid, &Planetd);
	if(artefactPrecense(&Planetd) == 6)
  {
  	dbMapRetrieveEmpire( id, &empired );
  	for( a = 0 ; a < empired.numplayers ; a++ )
    {
  		if( dbUserMainRetrieve( empired.player[a], &maind ) < 0 )
        continue;
      maind.articount = 0; //Reset their count to zero so it become inefective
      dbUserMainSet( empired.player[a], &maind );
    }
	}
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( num >= 2 )
  {
    fseek( file, 4+(num*20)-20, SEEK_SET );
    fread( data, 1, 20, file );
  }
  for( a = 0 ; a < num ; a++ )
  {
    fseek( file, 4+(a*20), SEEK_SET );
    fread( &b, 1, sizeof(int), file );
    if( b != plnid )
      continue;
    if( ( num >= 2 ) && ( a+1 < num ) )
    {
      fseek( file, 4+(a*20), SEEK_SET );
      fwrite( data, 1, 20, file );
    }
    fseek( file, 0, SEEK_SET );
    num--;
    fwrite( &num, 1, sizeof(int), file );
    fclose( file );
    return 1;
  }
  fclose( file );
  return -3;
}

int dbUserPlanetSetFlags( int id, int plnid, int flags )
{
  int num, pos;
  int *list;
  FILE *file;
  if( ( num = dbUserPlanetListIndices( id, &list ) ) < 0 )
    return -3;
  for( pos = 0 ; pos < num ; pos++ )
  {
    if( pos == num )
    {
      free( list );
      return -3;
    }
    if( list[pos] == plnid )
      break;
  }
  free( list );
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  fseek( file, 4+(pos*20)+12, SEEK_SET );
  fwrite( &flags, 1, sizeof(int), file );
  fclose( file );
  return 1;
}

int dbUserPlanetListCoords( int id, int **list )
{
  int a, num;
  int *listp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( !( listp = malloc( num*sizeof(int) ) ) )
  {
    fclose( file );
    return -1;
  }
  for( a = 0 ; a < num ; a++ )
  {
    fseek( file, 4+(a*20)+8, SEEK_SET );
    fread( &listp[a], 1, sizeof(int), file );
  }
  fclose( file );
  *list = listp;
  return num;
}

int dbUserPlanetListIndices( int id, int **list )
{
  int a, num;
  int *listp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( !( listp = malloc( num*sizeof(int) ) ) )
  {
    fclose( file );
    return -1;
  }
  for( a = 0 ; a < num ; a++ )
  {
    fseek( file, 4+(a*20), SEEK_SET );
    fread( &listp[a], 1, sizeof(int), file );
  }
  fclose( file );
  *list = listp;
  return num;
}

int dbUserPlanetListSystems( int id, int **list )
{
  int a, num;
  int *listp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( !( listp = malloc( num*sizeof(int) ) ) )
  {
    fclose( file );
    return -1;
  }
  for( a = 0 ; a < num ; a++ )
  {
    fseek( file, 4+(a*20)+4, SEEK_SET );
    fread( &listp[a], 1, sizeof(int), file );
  }
  fclose( file );
  *list = listp;
  return num;
}



int dbUserPlanetListFull ( int id, int **list )
{
  int a, num;
  int *listp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( !( listp = malloc( num*4*sizeof(int) ) ) )
  {
    fclose( file );
    return -1;
  }
  for( a = 0 ; a < num ; a++ )
  {
    fseek( file, 4+(a*20), SEEK_SET );
    fread( &listp[a*4], 1, 4*sizeof(int), file );
  }
  fclose( file );
  *list = listp;
  return num;
}


int dbUserPlanetListIndicesSorted( int id, int **list, int sort )
{
  int a, num, b, d;
  int *listp, *list2p, *list3p;
  dbMainPlanetDef planetd;
  FILE *file;
// printf("we beginnen te sortere\n");
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( !( listp = malloc( num*sizeof(int) ) ) )
    {
      fclose( file );
      return -1;
    }
  
  if( !( num ) )
    {
      free (listp);
      fclose( file );
      return -1;
    }
 // printf("database geopend\n");
  for( a = 0 ; a < num ; a++ )
    {
      fseek( file, 4+(a*20), SEEK_SET );
      fread( &listp[a], 1, sizeof(int), file );
    }
  fclose( file );
// printf("eerste malloc begint\n");  
  if( !( list2p = malloc( (num)*sizeof(int) ) ) )
    {
      free (listp);
      return -1;
    }
// printf("tweede malloc begint\n");  
  if( !( list3p = malloc( (num)*sizeof(int) ) ) )
    {
      free (listp);
      free (list2p);
      return -1;
    }
// printf("sorteren begonnen sort=%d\n", sort);
  if (sort == 1)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  list2p[b] = planetd.size;
	}
      sortlist( num, listp, list2p );
    }
  else if (sort == 2)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  for( a = d = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
	    d += planetd.building[a];
	  list2p[b]= d + planetd.construction;
	}
      sortlist( num, listp, list2p );
    }
  else if (sort == 3)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  for( a = d = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
	    d += planetd.building[a];
	  list2p[b]= (int)(100 * ( cmdGetBuildOvercost( planetd.size, d + planetd.construction ) -1 ) );
	}
      sortlist( num, listp, list2p );
    }
  else if (sort == 4)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  list2p[b] = planetd.population;
	}
      sortlist( num, listp, list2p );
    }
  else if (sort == 5)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  list2p[b] = planetd.protection;
	}
      sortlist( num, listp, list2p );
    }
  else if (sort == 6)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  list2p[b] = planetd.special[0];
	  list3p[b] = planetd.special[1];	  
	}
      sortlist2( num, listp, list2p, list3p );
    }
  else if (sort == 10)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  list2p[b] = -( ( planetd.position >> 8 ) & 0xFFF );
	  list3p[b] = -( planetd.position >> 20 );
	}
      sortlist2( num, listp, list2p, list3p );
    }
  else if (sort == 11)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  list2p[b] = -planetd.size;
	}
      sortlist( num, listp, list2p );
    }
  else if (sort == 12)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  for( a = d = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
	    d += planetd.building[a];
	  list2p[b]= -d - planetd.construction;
	}
      sortlist( num, listp, list2p );
    }
  else if (sort == 13)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  for( a = d = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
	    d += planetd.building[a];
	  list2p[b]= -(int)(100 * ( cmdGetBuildOvercost( planetd.size, d + planetd.construction ) -1 ) );
	}
      sortlist( num, listp, list2p );
    }
  else if (sort == 14)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  list2p[b] = -planetd.population;
	}
      sortlist( num, listp, list2p );
    }
  else if (sort == 15)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  list2p[b] = -planetd.protection;
	}
      sortlist( num, listp, list2p );
    }
  else if (sort == 16)
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  list2p[b] = -planetd.special[0];
	  list3p[b] = -planetd.special[1];	  
	}
      sortlist2( num, listp, list2p, list3p );
    }
  else
    {
      for( b = 0; b < num; b++)
	{
	  if( dbMapRetrievePlanet( listp[b], &planetd ) < 0 )
	    continue;
	  list2p[b] = ( ( planetd.position >> 8 ) & 0xFFF );
	  list3p[b] = ( planetd.position >> 20 );
	}
      sortlist2( num, listp, list2p, list3p );
    }
  
  // printf("sorteren is gedaan nu nog free \n");

  *list = listp;
  free (list2p);
  free (list3p);
  return num;
}



// user planets/portals functions

int dbUserPortalsList( int id, int **list )
{
  int a, b, num, flags;
  int *listp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( !( listp = malloc( 3*num*sizeof(int) ) ) )
    return -1;
  for( a = b = 0 ; a < num ; a++ )
  {
    fseek( file, 4+(a*20), SEEK_SET );
    fread( &listp[b], 1, 3*sizeof(int), file );
    fread( &flags, 1, sizeof(int), file );
    if( flags & CMD_PLANET_FLAGS_PORTAL )
      b += 3;
  }
  fclose( file );
  *list = listp;
  return (b/3);
}

int dbUserPortalsListCoords( int id, int **list )
{
  int a, b, num, flags;
  int *listp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( !( listp = malloc( num*sizeof(int) ) ) )
    return -1;
  for( a = b = 0 ; a < num ; a++ )
  {
    fseek( file, 4+(a*20)+8, SEEK_SET );
    fread( &listp[b], 1, sizeof(int), file );
    fread( &flags, 1, sizeof(int), file );
    if( flags & CMD_PLANET_FLAGS_PORTAL )
      b++;
  }
  fclose( file );
  *list = listp;
  return b;
}

int dbUserPortalsListIndices( int id, int **list )
{
  int a, b, num, flags;
  int *listp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_PLANETS ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( !( listp = malloc( num*sizeof(int) ) ) )
    return -1;
  for( a = b = 0 ; a < num ; a++ )
  {
    fseek( file, 4+(a*20), SEEK_SET );
    fread( &listp[b], 1, sizeof(int), file );
    fread( &flags, 1, sizeof(int), file );
    if( flags & CMD_PLANET_FLAGS_PORTAL )
      b++;
  }
  fclose( file );
  *list = listp;
  return b;
}






// user fleets functions

int dbUserFleetAdd( int id, dbUserFleetPtr fleetd )
{
  int pos;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_FLEETS ) ) )
    return -3;
  fread( &pos, 1, sizeof(int), file );
  fseek( file, 4+(pos*96), SEEK_SET );
  fwrite( fleetd->unit, 1, 16*sizeof(int), file );
  fwrite( &fleetd->order, 1, sizeof(int), file );
  fwrite( &fleetd->destination, 1, sizeof(int), file );
  fwrite( &fleetd->destid, 1, sizeof(int), file );
  fwrite( &fleetd->sysid, 1, sizeof(int), file );
  fwrite( &fleetd->source, 1, sizeof(int), file );
  fwrite( &fleetd->flags, 1, sizeof(int), file );
  fwrite( &fleetd->time, 1, sizeof(int), file );
  fwrite( &fleetd->basetime, 1, sizeof(int), file );
  fseek( file, 0, SEEK_SET );
  pos++;
  fwrite( &pos, 1, sizeof(int), file );
  fclose( file );
  return (pos-1);
}

int dbUserFleetRemove( int id, int fltid )
{
  int a, num, data[24];
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_FLEETS ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( (unsigned int)fltid >= num )
  {
    fclose( file );
    return -2;
  }
  if( fltid+1 < num )
  {
    fseek( file, 4+(num*96)-96, SEEK_SET );
    fread( data, 1, 96, file );
    fseek( file, 4+(fltid*96), SEEK_SET );
    fwrite( data, 1, 96, file );
  }
  fseek( file, 0, SEEK_SET );
  a = num-1;
  fwrite( &a, 1, sizeof(int), file );
  fclose( file );
  return 1;
}

int dbUserFleetList( int id, dbUserFleetPtr *fleetd )
{
  int a, num;
  dbUserFleetPtr fleetp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_FLEETS ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( !( fleetp = malloc( num*sizeof(dbUserFleetDef) ) ) )
  {
    fclose( file );
    return -1;
  }
  for( a = 0 ; a < num ; a++ )
  {
    fseek( file, 4+(a*96), SEEK_SET );
    fread( fleetp[a].unit, 1, 16*sizeof(int), file );
    fread( &fleetp[a].order, 1, sizeof(int), file );
    fread( &fleetp[a].destination, 1, sizeof(int), file );
    fread( &fleetp[a].destid, 1, sizeof(int), file );
    fread( &fleetp[a].sysid, 1, sizeof(int), file );
    fread( &fleetp[a].source, 1, sizeof(int), file );
    fread( &fleetp[a].flags, 1, sizeof(int), file );
    fread( &fleetp[a].time, 1, sizeof(int), file );
    fread( &fleetp[a].basetime, 1, sizeof(int), file );
  }
  fclose( file );
  *fleetd = fleetp;
  return num;
}

int dbUserFleetSet( int id, int fltid, dbUserFleetPtr fleetd )
{
  int num;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_FLEETS ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( (unsigned int)fltid >= num )
  {
    fclose( file );
    return -2;
  }
  fseek( file, 4+(fltid*96), SEEK_SET );
  fwrite( fleetd->unit, 1, 16*sizeof(int), file );
  fwrite( &fleetd->order, 1, sizeof(int), file );
  fwrite( &fleetd->destination, 1, sizeof(int), file );
  fwrite( &fleetd->destid, 1, sizeof(int), file );
  fwrite( &fleetd->sysid, 1, sizeof(int), file );
  fwrite( &fleetd->source, 1, sizeof(int), file );
  fwrite( &fleetd->flags, 1, sizeof(int), file );
  fwrite( &fleetd->time, 1, sizeof(int), file );
  fwrite( &fleetd->basetime, 1, sizeof(int), file );
  fclose( file );
  return num;
}

int dbUserFleetRetrieve( int id, int fltid, dbUserFleetPtr fleetd )
{
  int num;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_FLEETS ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( (unsigned int)fltid >= num )
  {
    fclose( file );
    return -2;
  }
  fseek( file, 4+(fltid*96), SEEK_SET );
  fread( fleetd->unit, 1, 16*sizeof(int), file );
  fread( &fleetd->order, 1, sizeof(int), file );
  fread( &fleetd->destination, 1, sizeof(int), file );
  fread( &fleetd->destid, 1, sizeof(int), file );
  fread( &fleetd->sysid, 1, sizeof(int), file );
  fread( &fleetd->source, 1, sizeof(int), file );
  fread( &fleetd->flags, 1, sizeof(int), file );
  fread( &fleetd->time, 1, sizeof(int), file );
  fread( &fleetd->basetime, 1, sizeof(int), file );
  fclose( file );
  return num;
}





// user news functions

int dbUserNewsAdd( int id, long long int *data, long long int flags )
{
  long long int a, num, lused, lfree, numnext, lcur, lnext, cflags;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_NEWS ) ) )
  {
    printf( "Error %02d, fopen dbusernewsadd\n", errno );
    return -3;
  }
  
  fread( &num, 1, sizeof(long long int), file );
  fread( &lused, 1, sizeof(long long int), file );
  fread( &lfree, 1, sizeof(long long int), file );

  fread( &cflags, 1, sizeof(long long int), file );
  cflags |= flags;
  fseek( file, 24, SEEK_SET );
  fwrite( &cflags, 1, sizeof(long long int), file );

  fread( &numnext, 1, sizeof(long long int), file );

  if( lfree != -1 )
  {
    fseek( file, 40+lfree*DB_USER_NEWS_SIZE+8, SEEK_SET );
    fread( &lnext, 1, sizeof(long long int), file );
    fseek( file, 16, SEEK_SET );
    fwrite( &lnext, 1, sizeof(long long int), file );
    lcur = lfree;
  }
  else
  {
    lcur = numnext;
    numnext++;
    fseek( file, 32, SEEK_SET );
    fwrite( &numnext, 1, sizeof(long long int), file );
  }

  fseek( file, 40+lcur*DB_USER_NEWS_SIZE, SEEK_SET );
  a = -1;
  fwrite( &a, 1, sizeof(long long int), file );

  fwrite( &lused, 1, sizeof(long long int), file );

  fwrite( data, 1, DB_USER_NEWS_BASE*sizeof(long long int), file );
  if( lused != -1 )
  {
    fseek( file, 40+lused*DB_USER_NEWS_SIZE, SEEK_SET );
    fwrite( &lcur, 1, sizeof(long long int), file );
  }
  fseek( file, 8, SEEK_SET );
  fwrite( &lcur, 1, sizeof(long long int), file );

  num++;
  fseek( file, 0, SEEK_SET );
  fwrite( &num, 1, sizeof(long long int), file );

  fclose( file );
  return 1;
}

long long int dbUserNewsGetFlags( int id )
{
  long long int flags;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_NEWS ) ) )
  {
    printf( "Error %02d, fopen dbusernewsadd\n", errno );
    return -3;
  }
  fseek( file, 24, SEEK_SET );
  fread( &flags, 1, sizeof(long long int), file );
  fclose( file );
  return flags;
}

int dbUserNewsList( int id, long long int **data )
{
  long long int a, b, c, num, lused, lfree;
  FILE *file;
  long long int *datap;
  *data = 0;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_NEWS ) ) )
  {
    printf( "Error %02d, fopen dbusernewslist\n", errno );
    return -3;
  }
  fread( &num, 1, sizeof(long long int), file );
  fread( &lused, 1, sizeof(long long int), file );
  fread( &lfree, 1, sizeof(long long int), file );

  if( !( datap = malloc( num*DB_USER_NEWS_BASE*sizeof(long long int) ) ) )
  {
    fclose( file );
    return -3;
  }
  for( a = lused, b = 0, c = num ; c ; c--, b += (DB_USER_NEWS_BASE) )
  {
    fseek( file, 40+a*DB_USER_NEWS_SIZE+8, SEEK_SET );
    fread( &a, 1, sizeof(long long int), file );
    fread( &datap[b], 1, DB_USER_NEWS_BASE*sizeof(long long int), file );
  }
  *data = datap;

  fclose( file );
  return num;
}

// I hate doing linked lists with files!! The one who fully understand this function on the first read gets a cookie.
long long int dbUserNewsListUpdate( int id, long long int **data, long long int time )
{
  long long int a, b, c, d, num, numnew, lused, lfree, lprev, lnext;
  FILE *file;
  long long int *datap;
  *data = 0;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_NEWS ) ) )
  {
    printf( "Error %02d, fopen dbusernewslist\n", errno );
    return -3;
  }
  fread( &num, 1, sizeof(long long int), file );
  fread( &lused, 1, sizeof(long long int), file );
  fread( &lfree, 1, sizeof(long long int), file );
  a = 0;
  fwrite( &a, 1, sizeof(long long int), file );
  numnew = num;

  if( !( datap = malloc( num*DB_USER_NEWS_BASE*sizeof(long long int) ) ) )
  {
    fclose( file );
    return -3;
  }
  for( a = lused, b = 0, c = num ; c ; c--, a = lnext )
  {
    fseek( file, 40+a*DB_USER_NEWS_SIZE, SEEK_SET );
    fread( &lprev, 1, sizeof(long long int), file );
    fread( &lnext, 1, sizeof(long long int), file );
    fread( &datap[b], 1, 2*sizeof(long long int), file );
    if( !( datap[b+1] & CMD_NEWS_FLAGS_NEW ) )
    {
      numnew--;
      if( datap[b+0]+CMD_NEWS_EXPIRE_TIME >= time )
        continue;
      num--;

      if( lprev != -1 )
        fseek( file, 40+lprev*DB_USER_NEWS_SIZE+8, SEEK_SET );
      else
        fseek( file, 8, SEEK_SET );
      fwrite( &lnext, 1, sizeof(long long int), file );
      if( lnext != -1 )
      {
        fseek( file, 40+lnext*DB_USER_NEWS_SIZE, SEEK_SET );
        fwrite( &lprev, 1, sizeof(long long int), file );
      }

      fseek( file, 40+a*DB_USER_NEWS_SIZE, SEEK_SET );
      d = -1;
      fwrite( &d, 1, sizeof(long long int), file );
      fwrite( &lfree, 1, sizeof(long long int), file );
      if( lfree != -1 )
      {
        fseek( file, 40+lfree*DB_USER_NEWS_SIZE, SEEK_SET );
        fwrite( &a, 1, sizeof(long long int), file );
      }
      fseek( file, 16, SEEK_SET );
      lfree = a;
      fwrite( &lfree, 1, sizeof(long long int), file );

      fseek( file, 0, SEEK_SET );
      fwrite( &num, 1, sizeof(long long int), file );
    }
    fseek( file, 40+a*DB_USER_NEWS_SIZE+16+8, SEEK_SET );
    datap[b+1] &= 0xFFFFFFFF - CMD_NEWS_FLAGS_NEW;
    fwrite( &datap[b+1], 1, sizeof(long long int), file );
    fread( &datap[b+2], 1, (DB_USER_NEWS_BASE-2)*sizeof(long long int), file );
    b += DB_USER_NEWS_BASE;
  }
  *data = datap;

  fclose( file );
  return numnew;
}

int dbUserNewsEmpty( int id )
{
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_NEWS ) ) )
  {
    printf( "Error %02d, fopen dbusernewsempty\n", errno );
    return -3;
  }
  fwrite( dbFileUserListData[DB_FILE_USER_NEWS], 1, dbFileUserListBase[DB_FILE_USER_NEWS], file );
  fclose( file );
  return 1;
}





// fam news functions

int dbFamNewsAdd( int id, long long int *data )
{
  long long int a, num, lused, lfree, numnext, lcur, lnext;
  FILE *file;
  unsigned char fname[32];
  sprintf( fname, "fam%dnews", id );
  if( !( file = fopen( fname, "rb+" ) ) )
  {
		printf( "Error %02d, fopen dbusernewsadd\n", errno );
  	return -3;
  }
  
  fread( &num, 1, sizeof(long long int), file );
  fread( &lused, 1, sizeof(long long int), file );
  fread( &lfree, 1, sizeof(long long int), file );
  fseek( file, 32, SEEK_SET );
  fread( &numnext, 1, sizeof(long long int), file );

  if( lfree != -1 )
  {
    fseek( file, 40+lfree*DB_USER_NEWS_SIZE+8, SEEK_SET );
    fread( &lnext, 1, sizeof(long long int), file );
    fseek( file, 16, SEEK_SET );
    fwrite( &lnext, 1, sizeof(long long int), file );
    lcur = lfree;
  }
  else
  {
    lcur = numnext;
    numnext++;
    fseek( file, 32, SEEK_SET );
    fwrite( &numnext, 1, sizeof(long long int), file );
  }

  fseek( file, 40+lcur*DB_USER_NEWS_SIZE, SEEK_SET );
  a = -1;
  fwrite( &a, 1, sizeof(long long int), file );

  fwrite( &lused, 1, sizeof(long long int), file );

  fwrite( data, 1, DB_USER_NEWS_BASE*sizeof(long long int), file );
  if( lused != -1 )
  {
    fseek( file, 40+lused*DB_USER_NEWS_SIZE, SEEK_SET );
    fwrite( &lcur, 1, sizeof(long long int), file );
  }
  fseek( file, 8, SEEK_SET );
  fwrite( &lcur, 1, sizeof(long long int), file );

  num++;
  fseek( file, 0, SEEK_SET );
  fwrite( &num, 1, sizeof(long long int), file );

  fclose( file );
  return 1;
}

int dbFamNewsList( int id, long long int **data, int time )
{
  long long int a, b, c, d, num, lused, lfree, lprev, lnext;
  FILE *file;
  long long int *datap;
  unsigned char fname[32];
  *data = 0;
  sprintf( fname, "fam%dnews", id );
  if( !( file = fopen( fname, "rb+" ) ) )
  {
    printf( "Error %02d, fopen dbusernewslist\n", errno );
    return -3;
  }
  fread( &num, 1, sizeof(long long int), file );
  fread( &lused, 1, sizeof(long long int), file );
  fread( &lfree, 1, sizeof(long long int), file );
  a = 0;
  fwrite( &a, 1, sizeof(int), file );
  if( !( datap = malloc( num*DB_USER_NEWS_BASE*sizeof(long long int) ) ) )
  {
    fclose( file );
    return -3;
  }
  for( a = lused, b = 0, c = num ; c ; c--, a = lnext )
  {
    fseek( file, 40+a*DB_USER_NEWS_SIZE, SEEK_SET );
    fread( &lprev, 1, sizeof(long long int), file );
    fread( &lnext, 1, sizeof(long long int), file );
    fread( &datap[b], 1, 1*sizeof(long long int), file );
    if( datap[b+0]+CMD_NEWS_EXPIRE_TIME < time )
    {
      num--;
      if( lprev != -1 )
        fseek( file, 40+lprev*DB_USER_NEWS_SIZE+8, SEEK_SET );
      else
        fseek( file, 8, SEEK_SET );
      fwrite( &lnext, 1, sizeof(long long int), file );
      if( lnext != -1 )
      {
        fseek( file, 40+lnext*DB_USER_NEWS_SIZE, SEEK_SET );
        fwrite( &lprev, 1, sizeof(long long int), file );
      }

      fseek( file, 40+a*DB_USER_NEWS_SIZE, SEEK_SET );
      d = -1;
      fwrite( &d, 1, sizeof(long long int), file );
      fwrite( &lfree, 1, sizeof(long long int), file );
      if( lfree != -1 )
      {
        fseek( file, 40+lfree*DB_USER_NEWS_SIZE, SEEK_SET );
        fwrite( &a, 1, sizeof(long long int), file );
      }
      fseek( file, 16, SEEK_SET );
      lfree = a;
      fwrite( &lfree, 1, sizeof(long long int), file );

      fseek( file, 0, SEEK_SET );
      fwrite( &num, 1, sizeof(long long int), file );
    }
    fseek( file, 40+a*DB_USER_NEWS_SIZE+16+8, SEEK_SET );
    fread( &datap[b+1], 1, (DB_USER_NEWS_BASE-1)*sizeof(long long int), file );
    b += DB_USER_NEWS_BASE;
  }
  *data = datap;

  fclose( file );
  return num;
}






// map functions

int dbMapRetrieveMain( int *binfo )
{
  FILE *file;
  if( !( file = dbFileGenOpen( DB_FILE_MAP ) ) )
    return -3;
  fseek( file, 0, SEEK_SET );
  fread( binfo, 1, 7*sizeof(int), file );
  return 1;
}

int dbMapSetSystem( int sysid, dbMainSystemPtr systemd )
{
  FILE *file;
  if( !( file = dbFileGenOpen( DB_FILE_MAP ) ) )
    return -3;
  if( (unsigned int)sysid >= dbMapBInfoStatic[2] )
    return -3;
  fseek( file, 28+32+(sysid*20), SEEK_SET );
  fwrite( &systemd->position, 1, sizeof(int), file );
  fwrite( &systemd->indexplanet, 1, sizeof(int), file );
  fwrite( &systemd->numplanets, 1, sizeof(int), file );
  fwrite( &systemd->empire, 1, sizeof(int), file );
  fwrite( &systemd->unexplored, 1, sizeof(int), file );
  memcpy( &dbMapSystems[sysid], systemd, sizeof(dbMainSystemDef) );
  return 1;
}

int dbMapRetrieveSystem( int sysid, dbMainSystemPtr systemd )
{
  FILE *file;
  if( ( sysid & 0x10000000 ) )
  {
    sysid &= 0xFFFFFF;
    if( !( file = dbFileGenOpen( DB_FILE_MAP ) ) )
      return -3;
    if( (unsigned int)sysid >= dbMapBInfoStatic[2] )
      return -3;
    fseek( file, 28+32+(sysid*20), SEEK_SET );
    fread( &systemd->position, 1, sizeof(int), file );
    fread( &systemd->indexplanet, 1, sizeof(int), file );
    fread( &systemd->numplanets, 1, sizeof(int), file );
    fread( &systemd->empire, 1, sizeof(int), file );
    fread( &systemd->unexplored, 1, sizeof(int), file );
    return 1;
  }
  if( (unsigned int)sysid >= dbMapBInfoStatic[2] )
    return -3;
  memcpy( systemd, &dbMapSystems[sysid], sizeof(dbMainSystemDef) );
  return 1;
}

int dbMapSetPlanet( int plnid, dbMainPlanetPtr planetd )
{
  FILE *file;
  if( !( file = dbFileGenOpen( DB_FILE_MAP ) ) )
    return -3;
  if( (unsigned int)plnid >= dbMapBInfoStatic[3] )
    return -3;
  fseek( file, 28+32+(dbMapBInfoStatic[2]*20)+(plnid*184), SEEK_SET );
  fwrite( &planetd->system, 1, 1*sizeof(int), file );
  fwrite( &planetd->position, 1, 1*sizeof(int), file );
  fwrite( &planetd->owner, 1, 1*sizeof(int), file );
  fwrite( &planetd->size, 1, 1*sizeof(int), file );
  fwrite( &planetd->flags, 1, 1*sizeof(int), file );
  fwrite( &planetd->population, 1, 1*sizeof(int), file );
  fwrite( &planetd->maxpopulation, 1, 1*sizeof(int), file );
  fwrite( &planetd->special, 1, 3*sizeof(int), file );
  fwrite( &planetd->building, 1, 16*sizeof(int), file );
  fwrite( &planetd->unit, 1, 16*sizeof(int), file );
  fwrite( &planetd->construction, 1, 1*sizeof(int), file );
  fwrite( &planetd->protection, 1, 1*sizeof(int), file );
  fwrite( &planetd->surrender, 1, 1*sizeof(int), file );
  return 1;
}

int dbMapRetrievePlanet( int plnid, dbMainPlanetPtr planetd )
{
  FILE *file;
  if( plnid == -1)
  	return -3;
  if( !( file = dbFileGenOpen( DB_FILE_MAP ) ) )
    return -3;
  if( (unsigned int)plnid >= dbMapBInfoStatic[3] )
    return -3;
  fseek( file, 28+32+(dbMapBInfoStatic[2]*20)+(plnid*184), SEEK_SET );
  fread( &planetd->system, 1, 1*sizeof(int), file );
  fread( &planetd->position, 1, 1*sizeof(int), file );
  fread( &planetd->owner, 1, 1*sizeof(int), file );
  fread( &planetd->size, 1, 1*sizeof(int), file );
  fread( &planetd->flags, 1, 1*sizeof(int), file );
  fread( &planetd->population, 1, 1*sizeof(int), file );
  fread( &planetd->maxpopulation, 1, 1*sizeof(int), file );
  fread( &planetd->special, 1, 3*sizeof(int), file );
  fread( &planetd->building, 1, 16*sizeof(int), file );
  fread( &planetd->unit, 1, 16*sizeof(int), file );
  fread( &planetd->construction, 1, 1*sizeof(int), file );
  fread( &planetd->protection, 1, 1*sizeof(int), file );
  fread( &planetd->surrender, 1, 1*sizeof(int), file );
  return 1;
}


int dbMapSetEmpire( int famid, dbMainEmpirePtr empired )
{
 FILE *file;
 dbUserPtr user;

 if(!( file = dbFileGenOpen( DB_FILE_MAP ) ) )
   return -3;
 if((unsigned int)famid >= dbMapBInfoStatic[4])
   return -3;
//---------------------
  if ( empired->numplayers == 1)
     {
       empired->leader = empired->player[0];
       if(( user = dbUserLinkID( empired->leader ) ))
         {
           user->flags &= 0xFFFF;
           user->flags |= CMD_USER_FLAGS_LEADER | CMD_USER_FLAGS_ACTIVATED;
           dbUserSave( empired->leader, user);
         }
      }
//-----------------------
  fseek( file, 28+32+(dbMapBInfoStatic[2]*20)+(dbMapBInfoStatic[3]*184)+(famid*336), SEEK_SET );
  fwrite( &empired->numplayers, 1, 1*sizeof(int), file );
  fwrite( empired->player, 1, 32*sizeof(int), file );
  fwrite( &empired->homeid, 1, 1*sizeof(int), file );
  fwrite( &empired->homepos, 1, 1*sizeof(int), file );
  fwrite( empired->name, 1, 64, file );
  fwrite( &empired->leader, 1, 1*sizeof(int), file );
  fwrite( empired->vote, 1, 32, file );
  fwrite( &empired->picmime, 1, sizeof(int), file );
  fwrite( &empired->pictime, 1, sizeof(int), file );
  fwrite( &empired->planets, 1, sizeof(int), file );
  fwrite( &empired->networth, 1, sizeof(int), file );
  fwrite( &empired->artefacts, 1, sizeof(int), file );
  fwrite( &empired->rank, 1, sizeof(int), file );
  return 1;
}

int dbMapRetrieveEmpire( int famid, dbMainEmpirePtr empired )
{
  FILE *file;
  if( !( file = dbFileGenOpen( DB_FILE_MAP ) ) )
    return -3;
  if( (unsigned int)famid >= dbMapBInfoStatic[4] )
    return -3;	//dbMapBInfoStatic is the 7 first int of map file
  fseek( file, 28+32+(dbMapBInfoStatic[2]*20)+(dbMapBInfoStatic[3]*184)+(famid*336), SEEK_SET );
  fread( &empired->numplayers, 1, 1*sizeof(int), file );
  fread( empired->player, 1, 32*sizeof(int), file );
  fread( &empired->homeid, 1, 1*sizeof(int), file );
  fread( &empired->homepos, 1, 1*sizeof(int), file );
  fread( empired->name, 1, 64, file );
  fread( &empired->leader, 1, 1*sizeof(int), file );
  fread( empired->vote, 1, 32, file );
  fread( &empired->picmime, 1, sizeof(int), file );
  fread( &empired->pictime, 1, sizeof(int), file );
  fread( &empired->planets, 1, sizeof(int), file );
  fread( &empired->networth, 1, sizeof(int), file );
  fread( &empired->artefacts, 1, sizeof(int), file );
  fread( &empired->rank, 1, sizeof(int), file );
  return 1;
}




/*
relation
  4:tick
  4:type
  4:empire
  4:flags
*/

int dbEmpireRelsAdd( int id, int *rel )
{
  int pos;
  FILE *file;
  if( (unsigned int)id >= dbMapBInfoStatic[4] )
    return -3;
  if( !( file = dbFileFamOpen( id, 0 ) ) )
    return -3;
  fread( &pos, 1, sizeof(int), file );
  fseek( file, 4+(pos*16), SEEK_SET );
  fwrite( rel, 1, 4*sizeof(int), file );
  fseek( file, 0, SEEK_SET );
  pos++;
  fwrite( &pos, 1, sizeof(int), file );
  fclose( file );
  return (pos-1);
}

int dbEmpireRelsRemove( int id, int relid )
{
  int a, num, data[4];
  FILE *file;
  if( (unsigned int)id >= dbMapBInfoStatic[4] )
    return -3;
  if( !( file = dbFileFamOpen( id, 0 ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( (unsigned int)relid >= num )
  {
    fclose( file );
    return -2;
  }
  if( relid+1 < num )
  {
    fseek( file, 4+(num*16)-16, SEEK_SET );
    fread( data, 1, 16, file );
    fseek( file, 4+(relid*16), SEEK_SET );
    fwrite( data, 1, 16, file );
  }
  fseek( file, 0, SEEK_SET );
  a = num-1;
  fwrite( &a, 1, sizeof(int), file );
  fclose( file );
  return 1;
}

int dbEmpireRelsList( int id, int **rel )
{
  int num;
  FILE *file;
  int *relp;
  if( (unsigned int)id >= dbMapBInfoStatic[4] )
    return -3;
  if( !( file = dbFileFamOpen( id, 0 ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( !( relp = malloc( num*4*sizeof(int) ) ) )
  {
    fclose( file );
    return -1;
  }
  fread( relp, 1, num*4*sizeof(int), file );
  fclose( file );
  *rel = relp;
  return num;
}

int dbEmpireRelsGet( int id, int relid, int *rel )
{
  int num;
  FILE *file;
  if( (unsigned int)id >= dbMapBInfoStatic[4] )
    return -3;
  if( !( file = dbFileFamOpen( id, 0 ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( (unsigned int)relid >= num )
  {
    fclose( file );
    return -1;
  }
  fseek( file, 4+relid*16, SEEK_SET );
  fread( rel, 1, 4*sizeof(int), file );
  fclose( file );
  return num;
}



int dbEmpireMessageSet( int id, int num, unsigned char *text )
{
  FILE *file;
  if( (unsigned int)id >= dbMapBInfoStatic[4] )
    return -3;
  if( !( file = dbFileFamOpen( id, 1 ) ) )
    return -3;
  fseek( file, num*4096, SEEK_SET );
  fwrite( text, 1, strlen( text ) + 1, file );
  fclose( file );
  return 1;
}

int dbEmpireMessageRetrieve( int id, int num, unsigned char *text )
{
  FILE *file;
  if( (unsigned int)id >= dbMapBInfoStatic[4] )
    return -3;
  if( !( file = dbFileFamOpen( id, 1 ) ) )
    return -3;
  fseek( file, num*4096, SEEK_SET );
  fread( text, 1, 4096, file );
  fclose( file );
  return 1;
}







/*
market
   4:num max bids
   4:list free

3 * 2 * 80 * 12 ; res*action*prices*struct
struct( 12 )
   4:total of bids
   4:first list ID
   4:last list ID
struct( 16 )
   4:previous id
   4:next id
   4:quantity
   4:owner ID


bid :
4:action (0buy,1sell)
4:ressource
4:price
4:quantity
4:user ID
*/

#define DB_MARKET_BIDSOFF ( 8 + 3*2*DB_MARKET_RANGE*12 )

int dbMarketReset()
{
  int a, array[3];
  FILE *file;
  if( !( file = fopen( dbFileList[DB_FILE_MARKET], "wb+" ) ) )
    return 0;
  fseek( file, 0, SEEK_SET );
  array[0] = 0;
  array[1] = -1;
  fwrite( array, 1, 2*sizeof(int), file );
  array[0] = 0;
  array[1] = -1;
  array[2] = -1;
  for( a = 0 ; a < 6*DB_MARKET_RANGE ; a++ )
    fwrite( array, 1, 3*sizeof(int), file );
  fclose( file );
  return 1;
}

int dbMarketFull( int *list )
{
  int a;
  FILE *file;
  if( !( file = dbFileGenOpen( DB_FILE_MARKET ) ) )
    return -3;
  fseek( file, 8, SEEK_SET );
  for( a = 0 ; a < 3*2*DB_MARKET_RANGE ; a++ )
  {
    fseek( file, 8+a*12, SEEK_SET );
    fread( &list[a], 1, sizeof(int), file );
  }
  return 1;
}


int dbMarketReplaceFull( int *list )
{
  int a;
  FILE *file;
  if( !( file = dbFileGenOpen( DB_FILE_MARKET ) ) )
    return -3;
  fseek( file, 8, SEEK_SET );
  for( a = 0 ; a < 3*2*DB_MARKET_RANGE ; a++ )
  {
    fseek( file, 8+a*12, SEEK_SET );
    fwrite( &list[a], 1, sizeof(int), file );
  }
  return 1;
}



int dbMarketAdd( int *bid )
{
  int a, offs, num, lcur, lfree, databid[4];
  int mhead[3];
  FILE *file;
  if( !( file = dbFileGenOpen( DB_FILE_MARKET ) ) )
    return -3;

  offs = 8;
  offs += bid[DB_MARKETBID_RESSOURCE] * 2 * DB_MARKET_RANGE * 12;
  offs += bid[DB_MARKETBID_ACTION] * DB_MARKET_RANGE * 12;
  offs += bid[DB_MARKETBID_PRICE] * 12;

  fseek( file, offs, SEEK_SET );
  fread( mhead, 1, 12, file );

  fseek( file, 0, SEEK_SET );
  fread( &num, 1, sizeof(int), file );
  fread( &lfree, 1, sizeof(int), file );

  if( lfree != -1 )
  {
    fseek( file, DB_MARKET_BIDSOFF+lfree*16 + 4, SEEK_SET );
    fread( &a, 1, sizeof(int), file );
    fseek( file, 4, SEEK_SET );
    fwrite( &a, 1, sizeof(int), file );
    lcur = lfree;
  }
  else
  {
    lcur = num;
    num++;
    fseek( file, 0, SEEK_SET );
    fwrite( &num, 1, sizeof(int), file );
  }

  databid[0] = mhead[2];
  databid[1] = -1;
  databid[2] = bid[DB_MARKETBID_QUANTITY];
  databid[3] = bid[DB_MARKETBID_USERID];

  mhead[0] += bid[DB_MARKETBID_QUANTITY];
  if( mhead[1] == -1 )
    mhead[1] = lcur;
  else
  {
    fseek( file, DB_MARKET_BIDSOFF+mhead[2]*16 + 4, SEEK_SET );
    fwrite( &lcur, 1, sizeof(int), file );
  }
  mhead[2] = lcur;

  fseek( file, DB_MARKET_BIDSOFF+lcur*16, SEEK_SET );
  fwrite( databid, 1, 16, file );

  fseek( file, offs, SEEK_SET );
  fwrite( mhead, 1, 12, file );

  return lcur;
}

int dbMarketRemove( int *bid, int lcur )
{
  int offs, lfree, databid[4];
  int mhead[3];
  FILE *file;
  if( !( file = dbFileGenOpen( DB_FILE_MARKET ) ) )
    return -3;

  offs = 8;
  offs += bid[DB_MARKETBID_RESSOURCE] * 2 * DB_MARKET_RANGE * 12;
  offs += bid[DB_MARKETBID_ACTION] * DB_MARKET_RANGE * 12;
  offs += bid[DB_MARKETBID_PRICE] * 12;

  fseek( file, offs, SEEK_SET );
  fread( mhead, 1, 12, file );
  fseek( file, DB_MARKET_BIDSOFF+lcur*16, SEEK_SET );
  fread( databid, 1, 16, file );

  if( databid[0] != -1 )
  {
    fseek( file, DB_MARKET_BIDSOFF+databid[0]*16+4, SEEK_SET );
    fwrite( &databid[1], 1, sizeof(int), file );
  }
  if( databid[1] != -1 )
  {
    fseek( file, DB_MARKET_BIDSOFF+databid[1]*16, SEEK_SET );
    fwrite( &databid[0], 1, sizeof(int), file );
  }

  mhead[0] -= databid[2];
  if( mhead[1] == lcur )
    mhead[1] = databid[1];
  if( mhead[2] == lcur )
    mhead[2] = databid[0];
  fseek( file, offs, SEEK_SET );
  fwrite( mhead, 1, 12, file );

  fseek( file, 4, SEEK_SET );
  fread( &lfree, 1, sizeof(int), file );

  databid[0] = -1;
  databid[1] = lfree;
  databid[2] = -1;
  databid[3] = -1;
  fseek( file, DB_MARKET_BIDSOFF+lcur*16, SEEK_SET );
  fwrite( databid, 1, 16, file );

  fseek( file, 4, SEEK_SET );
  fwrite( &lcur, 1, sizeof(int), file );

  return lcur;
}

int dbMarketListStart( int *bid )
{
  int a, offs;
  FILE *file;
  if( !( file = dbFileGenOpen( DB_FILE_MARKET ) ) )
    return -3;

  offs = 8;
  offs += bid[DB_MARKETBID_RESSOURCE] * 2 * DB_MARKET_RANGE * 12;
  offs += bid[DB_MARKETBID_ACTION] * DB_MARKET_RANGE * 12;
  offs += bid[DB_MARKETBID_PRICE] * 12;

  fseek( file, offs + 4, SEEK_SET );
  fread( &a, 1, 4, file );

  return a;
}

int dbMarketListNext( int lcur, int *result )
{
  int databid[4];
  FILE *file;
  if( !( file = dbFileGenOpen( DB_FILE_MARKET ) ) )
    return -3;
  fseek( file, DB_MARKET_BIDSOFF+lcur*16, SEEK_SET );
  fread( databid, 1, 16, file );
  result[0] = databid[2];
  result[1] = databid[3];
  return databid[1];
}

int dbMarketSetQuantity( int *bid, int lcur, int quantity, int loss )
{
  int offs;
  int mhead[3];
  FILE *file;
  if( !( file = dbFileGenOpen( DB_FILE_MARKET ) ) )
    return -3;

  offs = 8;
  offs += bid[DB_MARKETBID_RESSOURCE] * 2 * DB_MARKET_RANGE * 12;
  offs += bid[DB_MARKETBID_ACTION] * DB_MARKET_RANGE * 12;
  offs += bid[DB_MARKETBID_PRICE] * 12;

  fseek( file, offs, SEEK_SET );
  fread( mhead, 1, 12, file );
  mhead[0] -= loss;
  fseek( file, offs, SEEK_SET );
  fwrite( mhead, 1, 12, file );

  fseek( file, DB_MARKET_BIDSOFF+lcur*16 + 8, SEEK_SET );
  fwrite( &quantity, 1, 4, file );

  return 0;
}




/*
user%d/market
  4:number of bids
  4:flags
struct ( 20 )
  4:action
  4:resource
  4:price
  4:quantity
  4:bid ID

*/

int dbUserMarketReset( int id )
{
  int a;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_MARKET ) ) )
    return -3;
  a = 0;
  fwrite( &a, 1, sizeof(int), file );
  fwrite( &a, 1, sizeof(int), file );
  fclose( file );
  return 1;
}

int dbUserMarketAdd( int id, int bidid, int action, int resource, int price, int quantity )
{
  int pos;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_MARKET ) ) )
    return -3;
  fread( &pos, 1, sizeof(int), file );
  fseek( file, 8+(pos*20), SEEK_SET );
  fwrite( &action, 1, sizeof(int), file );
  fwrite( &resource, 1, sizeof(int), file );
  fwrite( &price, 1, sizeof(int), file );
  fwrite( &quantity, 1, sizeof(int), file );
  fwrite( &bidid, 1, sizeof(int), file );
  fseek( file, 0, SEEK_SET );
  pos++;
  fwrite( &pos, 1, sizeof(int), file );
  fclose( file );
  return 1;
}

int dbUserMarketList( int id, int **list )
{
  int num;
  int *listp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_MARKET ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( !( listp = malloc( num*5*sizeof(int) ) ) )
    return -1;
  fseek( file, 8, SEEK_SET );
  fread( listp, 1, num*5*sizeof(int), file );
  fclose( file );
  *list = listp;
  return num;
}

int dbUserMarketQuantity( int id, int bidid, int quantity )
{
  int a, b, num;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_MARKET ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  for( a = 0 ; a < num ; a++ )
  {
    fseek( file, 8+(a*20)+16, SEEK_SET );
    fread( &b, 1, sizeof(int), file );
    if( b != bidid )
      continue;
    fseek( file, 8+(a*20)+12, SEEK_SET );
    fwrite( &quantity, 1, sizeof(int), file );
    fclose( file );
    return 1;
  }
  fclose( file );
  return -3;
}

int dbUserMarketRemove( int id, int bidid )
{
  int a, b, num, data[5];
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_MARKET ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( num >= 2 )
  {
    fseek( file, 8+(num*20)-20, SEEK_SET );
    fread( data, 1, 20, file );
  }
  fseek( file, 8, SEEK_SET );
  for( a = 0 ; a < num ; a++ )
  {
    fseek( file, 8+(a*20)+16, SEEK_SET );
    fread( &b, 1, sizeof(int), file );
    if( b != bidid )
      continue;
    if( ( num >= 2 ) && ( a+1 < num ) )
    {
      fseek( file, 8+(a*20), SEEK_SET );
      fwrite( data, 1, 20, file );
    }
    fseek( file, 0, SEEK_SET );
    num--;
    fwrite( &num, 1, sizeof(int), file );
    fclose( file );
    return 1;
  }
  fclose( file );
  return -3;
}







/*
forums
  4:number
struct ( X )
N:X:dbForumForumDef

forum%d/threads
  4:num
  4:list used
  4:list free
  4:new num
struct ( X )
  X;dbForumForumDef
struct ( X )
  4:previous
  4:next
  X:dbForumThreadDef

forum%d/thread%d
  4:number
  4:offset
struct ( X )
  X:dbForumThreadDef
struct ( X+Y )
  4:offset
  X:dbForumPostInDef
  Y:text
*/

int dbForumListForums( int perms, dbForumForumPtr *forums )
{
  int num;
  FILE *file;
  dbForumForumPtr forumsp;
  char szSource[500];
  
  sprintf(szSource, "%s/forums", PUBLIC_FORUM_DIRECTORY);
  if( !( file = fopen( szSource, "rb+" ) ))
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( !( forumsp = malloc( num * sizeof(dbForumForumDef) ) ) )
    return -3;
  fread( forumsp, 1, num*sizeof(dbForumForumDef), file );
  *forums = forumsp;
  fclose( file );
  return num;
}


int dbForumListThreads( int forum, int base, int end, dbForumForumPtr forumd, dbForumThreadPtr *threads )
{
  int a, b, c, d, num, lused, lfree, numnext;
  FILE *file;
  unsigned char fname[556];
  dbForumThreadPtr threadsp;

  *threads = 0;
  if( base < 0 )
    return -3;
  if(forum > 100)
  	sprintf( fname, "forum%d/threads", forum );
  else
  	sprintf( fname, "%s/forum%d/threads", PUBLIC_FORUM_DIRECTORY, forum );
  
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
  fread( &num, 1, sizeof(int), file );
  fread( &lused, 1, sizeof(int), file );
  fread( &lfree, 1, sizeof(int), file );
  fread( &numnext, 1, sizeof(int), file );
  fread( forumd, 1, sizeof(dbForumForumDef), file );

  if( end < num )
    num = end;
  if( base > num )
  {
    fclose( file );
    return -3;
  }

  if( !( threadsp = malloc( num * sizeof(dbForumThreadDef) ) ) )
    return -3;
  if( base == num )
  {
    fclose( file );
    *threads = threadsp;
    return 0;
  }
  for( a = lused, b = 0 ; b < base ; b++ )
  {
    fseek( file, 16+sizeof(dbForumForumDef) + a * ( sizeof(dbForumThreadDef) + 8 ) + 4, SEEK_SET );
    fread( &a, 1, sizeof(int), file );
  }
  for( d = 0 ; b < num ; b++, d++ )
  {
    fseek( file, 16+sizeof(dbForumForumDef) + a * ( sizeof(dbForumThreadDef) + 8 ) + 4, SEEK_SET );
    c = a;
    fread( &a, 1, sizeof(int), file );
    fread( &threadsp[d], 1, sizeof(dbForumThreadDef), file );
    threadsp[d].id = c;
  }

  *threads = threadsp;
  fclose( file );

  return d;
}


int dbForumListPosts( int forum, int thread, int base, int end, dbForumThreadPtr threadd, dbForumPostPtr *posts )
{
  int a, b, offset, num;
  unsigned char fname[556];
  FILE *file;
  dbForumPostPtr postsp;

  *posts = 0;
  if( base < 0 )
    return -3;
  if(forum > 100)
  	sprintf( fname, "forum%d/thread%d", forum, thread );
  else
  	sprintf( fname, "%s/forum%d/thread%d", PUBLIC_FORUM_DIRECTORY, forum, thread );
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
  fread( &num, 1, sizeof(int), file );

  fseek( file, 8, SEEK_SET );
  fread( threadd, 1, sizeof(dbForumThreadDef), file );

  if( end < num )
    num = end;
  if( base >= num )
  {
    fclose( file );
    return -3;
  }

  if( !( postsp = malloc( num * sizeof(dbForumPostDef) ) ) )
    return -3;
  offset = 8+sizeof(dbForumThreadDef);
  for( a = 0 ; a < base ; a++ )
  {
    fseek( file, offset, SEEK_SET );
    fread( &offset, 1, sizeof(int), file );
  }

  for( b = 0 ; a < num ; a++, b++ )
  {
    fseek( file, offset, SEEK_SET );
    fread( &offset, 1, sizeof(int), file );
    fread( &postsp[b].post, 1, sizeof(dbForumPostInDef), file );
    postsp[b].text = 0;
    if( (unsigned int)((postsp[b].post).length) >= 65536 )
    {
      (postsp[b].post).flags |= DB_FORUM_FLAGS_POSTERROR;
      continue;
    }
    if( !( postsp[b].text = malloc( (postsp[b].post).length + 1 ) ) )
    {
      (postsp[b].post).flags |= DB_FORUM_FLAGS_POSTERROR;
      continue;
    }
    fread( postsp[b].text, 1, (postsp[b].post).length, file );
    postsp[b].text[ (postsp[b].post).length ] = 0;
  }

  *posts = postsp;
  fclose( file );
  return b;
}




int dbForumRetrieveForum( int forum, dbForumForumPtr forumd )
{
  FILE *file;
  unsigned char fname[556];
  
  if(forum > 100)
  	sprintf( fname, "forum%d/threads", forum );
  else
  	sprintf( fname, "%s/forum%d/threads", PUBLIC_FORUM_DIRECTORY, forum );
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
  fseek( file, 16, SEEK_SET );
  fread( forumd, 1, sizeof(dbForumForumDef), file );
  fclose( file );
  return 1;
}



int dbForumAddForum( dbForumForumPtr forumd, int type, int nid )
{
  int a, num;
  FILE *file;
  unsigned char fname[532];
  num = nid;
  if( !( type ) )
  {
  	sprintf(fname, "%s/forums", PUBLIC_FORUM_DIRECTORY);
    if( !( file = fopen( fname, "rb+" ) ))
      return -3;
    fread( &num, 1, sizeof(int), file );
    fseek( file, 4+num*sizeof(dbForumForumDef), SEEK_SET );
    fwrite( forumd, 1, sizeof(dbForumForumDef), file );
    num++;
    fseek( file, 0, SEEK_SET );
    fwrite( &num, 1, sizeof(int), file );
    num--;
    fclose( file );
  }
	
	if(num > 100)
  	sprintf( fname, "forum%d", num );
  else
  	sprintf( fname, "%s/forum%d", PUBLIC_FORUM_DIRECTORY, num );
  if( mkdir( fname, S_IRWXU ) == -1 )
  {
    printf( "Error %02d, mkdir\n", errno );
    return -3;
  }
	
	if(num > 100)
  	sprintf( fname, "forum%d/threads", num );
  else
  	sprintf( fname, "%s/forum%d/threads", PUBLIC_FORUM_DIRECTORY, num );
  if( !( file = fopen( fname, "wb" ) ))
    return -3;
  a = 0;
  fwrite( &a, 1, sizeof(int), file );
  a = -1;
  fwrite( &a, 1, sizeof(int), file );
  fwrite( &a, 1, sizeof(int), file );
  a = 0;
  fwrite( &a, 1, sizeof(int), file );
  fwrite( forumd, 1, sizeof(dbForumForumDef), file );
  fclose( file );

  return num;
}

int dbForumRemoveForum( int forum )
{
  int a, b, num;
  FILE *file;
  DIR *dirdata;
  struct dirent *direntry;
  unsigned char fname[256];
  unsigned char *frcopy;
  dbForumForumDef forumd;

	if(forum > 100)
  	a = sprintf( fname, DB_DIRECTORY "/forum%d", forum );
  else
  	a = sprintf( fname,  "%s/forum%d", PUBLIC_FORUM_DIRECTORY, forum );
  if( !( dirdata = opendir( fname ) ) )
  {
    printf( "Error %03d, opendir\n", errno );
    return -3;
  }
  fname[a] = '/';
  while( ( direntry = readdir( dirdata ) ) )
  {
    sprintf( &fname[a+1], direntry->d_name );
    unlink( fname );
  }
  closedir( dirdata );
  
  if(forum > 100)
  	sprintf( fname, "forum%d", forum );
  else
  	sprintf( fname, "%s/forum%d", PUBLIC_FORUM_DIRECTORY, forum );
  rmdir( fname );

	if(forum > 100)
		sprintf( fname, "forums" );
	else
		sprintf( fname, "%s/forums", PUBLIC_FORUM_DIRECTORY );	
	
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( forum >= num )
  {
    fclose( file );
    return 0;
  }
  fseek( file, 4+forum*sizeof(dbForumForumDef), SEEK_SET );
  fread( &forumd, 1, sizeof(dbForumForumDef), file );
  forumd.flags |= DB_FORUM_FLAGS_FORUMUNUSED;
  fseek( file, 4+forum*sizeof(dbForumForumDef), SEEK_SET );
  fwrite( &forumd, 1, sizeof(dbForumForumDef), file );
  a = num - ( forum + 1 );
  if( a )
  {
    b = a * sizeof(dbForumForumDef);
    frcopy = malloc( b );
    fseek( file, 4+(forum+1)*sizeof(dbForumForumDef), SEEK_SET );
    fread( frcopy, 1, b, file );
    fseek( file, 4+(forum+0)*sizeof(dbForumForumDef), SEEK_SET );
    fwrite( frcopy, 1, b, file );
    free( frcopy );
  }
  num--;
  fseek( file, 0, SEEK_SET );
  fwrite( &num, 1, sizeof(int), file );
  fclose( file );

  return num;
}



int dbForumAddThread( int forum, dbForumThreadPtr threadd )
{
  int a, num, lused, lfree, numnext, lcur, lnext;
  FILE *file;
  unsigned char fname[256];
  dbForumForumDef forumd;

	if(forum > 100)
  	sprintf( fname, "forum%d/threads", forum );
  else
  	sprintf( fname, "%s/forum%d/threads", PUBLIC_FORUM_DIRECTORY, forum );
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
  fread( &num, 1, sizeof(int), file );
  fread( &lused, 1, sizeof(int), file );
  fread( &lfree, 1, sizeof(int), file );
  fread( &numnext, 1, sizeof(int), file );
  fread( &forumd, 1, sizeof(dbForumForumDef), file );

  if( lfree != -1 )
  {
    fseek( file, 16+sizeof(dbForumForumDef) + lfree * ( sizeof(dbForumThreadDef) + 8 ) + 4, SEEK_SET );
    fread( &lnext, 1, sizeof(int), file );
    fseek( file, 8, SEEK_SET );
    fwrite( &lnext, 1, sizeof(int), file );
    lcur = lfree;
  }
  else
  {
    lcur = numnext;
    numnext++;
    fseek( file, 12, SEEK_SET );
    fwrite( &numnext, 1, sizeof(int), file );
  }
  forumd.time = threadd->time;
  forumd.tick = threadd->tick;
  forumd.threads++;

  fseek( file, 16+sizeof(dbForumForumDef) + lcur * ( sizeof(dbForumThreadDef) + 8 ), SEEK_SET );
  a = -1;
  fwrite( &a, 1, sizeof(int), file );
  fwrite( &lused, 1, sizeof(int), file );
  threadd->flags &= 0xFFFFFFFF - DB_FORUM_FLAGS_THREADFREE;
  fwrite( threadd, 1, sizeof(dbForumThreadDef), file );

  if( lused != -1 )
  {
    fseek( file, 16+sizeof(dbForumForumDef) + lused * ( sizeof(dbForumThreadDef) + 8 ), SEEK_SET );
    fwrite( &lcur, 1, sizeof(int), file );
  }

  fseek( file, 0, SEEK_SET );
  num++;
  fwrite( &num, 1, sizeof(int), file );
  fwrite( &lcur, 1, sizeof(int), file );
  fseek( file, 16, SEEK_SET );
  fwrite( &forumd, 1, sizeof(dbForumForumDef), file );
  fclose( file );

	if(forum > 100)
		sprintf( fname, "forums" );
	else
		sprintf( fname, "%s/forums", PUBLIC_FORUM_DIRECTORY );	
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( forum < num )
  {
    fseek( file, 4+forum*sizeof(dbForumForumDef), SEEK_SET );
    fwrite( &forumd, 1, sizeof(dbForumForumDef), file );
  }
  fclose( file );
	
	if(forum > 100)
		sprintf( fname, "forum%d/thread%d", forum, lcur );
	else
		sprintf( fname, "%s/forum%d/thread%d", PUBLIC_FORUM_DIRECTORY, forum, lcur );

  if( !( file = fopen( fname, "wb+" ) ))
    return -3;
  a = 0;
  fwrite( &a, 1, sizeof(int), file );
  a = 8 + sizeof(dbForumThreadDef);
  fwrite( &a, 1, sizeof(int), file );
  fwrite( threadd, 1, sizeof(dbForumThreadDef), file );
  fclose( file );

  return lcur;
}

int dbForumRemoveThread( int forum, int thread )
{
  int a, num, lused, lfree, numnext, lprev, lnext;
  FILE *file;
  unsigned char fname[256];
  dbForumForumDef forumd;
  dbForumThreadDef threadd;

	if(forum > 100)
  	sprintf( fname, "forum%d/threads", forum );
  else
  	sprintf( fname, "%s/forum%d/threads", PUBLIC_FORUM_DIRECTORY, forum );
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
  fread( &num, 1, sizeof(int), file );
  fread( &lused, 1, sizeof(int), file );
  fread( &lfree, 1, sizeof(int), file );
  fread( &numnext, 1, sizeof(int), file );
  fread( &forumd, 1, sizeof(dbForumForumDef), file );
  if( (unsigned int)thread >= numnext )
  {
    fclose( file );
    return -3;
  }

  fseek( file, 16+sizeof(dbForumForumDef) + thread * ( sizeof(dbForumThreadDef) + 8 ), SEEK_SET );
  fread( &lprev, 1, sizeof(int), file );
  fread( &lnext, 1, sizeof(int), file );
  fread( &threadd, 1, sizeof(dbForumThreadDef), file );
  if( ( threadd.flags & DB_FORUM_FLAGS_THREADFREE ) )
  {
    fclose( file );
    return -3;
  }

  fseek( file, 16+sizeof(dbForumForumDef) + thread * ( sizeof(dbForumThreadDef) + 8 ), SEEK_SET );
  threadd.flags |= DB_FORUM_FLAGS_THREADFREE;
  a = -1;
  fwrite( &a, 1, sizeof(int), file );
  fwrite( &lfree, 1, sizeof(int), file );
  fwrite( &threadd, 1, sizeof(dbForumThreadDef), file );

  if( lprev != -1 )
    fseek( file, 16+sizeof(dbForumForumDef) + lprev * ( sizeof(dbForumThreadDef) + 8 ) + 4, SEEK_SET );
  else
    fseek( file, 4, SEEK_SET );
  fwrite( &lnext, 1, sizeof(int), file );
  if( lnext != -1 )
  {
    fseek( file, 16+sizeof(dbForumForumDef) + lnext * ( sizeof(dbForumThreadDef) + 8 ), SEEK_SET );
    fwrite( &lprev, 1, sizeof(int), file );
  }
  fseek( file, 8, SEEK_SET );
  fwrite( &thread, 1, sizeof(int), file );

  fseek( file, 0, SEEK_SET );
  num--;
  fwrite( &num, 1, sizeof(int), file );

  fseek( file, 16, SEEK_SET );
  forumd.threads--;
  fwrite( &forumd, 1, sizeof(dbForumForumDef), file );
  fclose( file );

	if(forum > 100)
  	sprintf( fname, "forum%d/thread%d", forum, thread );
  else
  	sprintf( fname, "%s/forum%d/thread%d", PUBLIC_FORUM_DIRECTORY, forum, thread );
  unlink( fname );

	if(forum > 100)
		sprintf( fname, "forums" );
	else
		sprintf( fname, "%s/forums", PUBLIC_FORUM_DIRECTORY );	
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( forum < num )
  {
    fseek( file, 4+forum*sizeof(dbForumForumDef), SEEK_SET );
    fread( &forumd, 1, sizeof(dbForumForumDef), file );
    forumd.threads--;
    fseek( file, 4+forum*sizeof(dbForumForumDef), SEEK_SET );
    fwrite( &forumd, 1, sizeof(dbForumForumDef), file );
  }
  fclose( file );

  return num;
}


int dbForumAddPost( int forum, int thread, dbForumPostPtr postd )
{
  int a, num, offset, lused, lprev, lnext;
  FILE *file;
  unsigned char fname[556];
  dbForumThreadDef threadd;
  dbForumForumDef forumd;
	
	if(forum > 100)
  	sprintf( fname, "forum%d/thread%d", forum, thread );
  else
  	sprintf( fname, "%s/forum%d/thread%d", PUBLIC_FORUM_DIRECTORY, forum, thread );
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
  fread( &num, 1, sizeof(int), file );
  num++;
  fseek( file, 0, SEEK_SET );
  fwrite( &num, 1, sizeof(int), file );
  fread( &offset, 1, sizeof(int), file );

  fread( &threadd, 1, sizeof(dbForumThreadDef), file );
  fseek( file, 8, SEEK_SET );
  threadd.time = postd->post.time;
  threadd.tick = postd->post.tick;
  threadd.posts++;
  fwrite( &threadd, 1, sizeof(dbForumThreadDef), file );

  fseek( file, offset, SEEK_SET );
  a = offset + ( 4 + sizeof(dbForumPostInDef) + (postd->post).length );
  fwrite( &a, 1, sizeof(int), file );
  fwrite( &(postd->post), 1, sizeof(dbForumPostInDef), file );
  fwrite( postd->text, 1, (postd->post).length, file );

  fseek( file, 4, SEEK_SET );
  fwrite( &a, 1, sizeof(int), file );

  fclose( file );

	if(forum > 100)
  	sprintf( fname, "forum%d/threads", forum );
  else
  	sprintf( fname, "%s/forum%d/threads", PUBLIC_FORUM_DIRECTORY, forum );
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
  fseek( file, 4, SEEK_SET );
  fread( &lused, 1, sizeof(int), file );

  fseek( file, 16+sizeof(dbForumForumDef) + thread * ( sizeof(dbForumThreadDef) + 8 ), SEEK_SET );
  fread( &lprev, 1, sizeof(int), file );
  fread( &lnext, 1, sizeof(int), file );
  fwrite( &threadd, 1, sizeof(dbForumThreadDef), file );

  if( lused != thread )
  {
    if( lprev != -1 )
    {
      fseek( file, 16+sizeof(dbForumForumDef) + lprev * ( sizeof(dbForumThreadDef) + 8 ) + 4, SEEK_SET );
      fwrite( &lnext, 1, sizeof(int), file );
    }
    if( lnext != -1 )
    {
      fseek( file, 16+sizeof(dbForumForumDef) + lnext * ( sizeof(dbForumThreadDef) + 8 ), SEEK_SET );
      fwrite( &lprev, 1, sizeof(int), file );
    }
    fseek( file, 16+sizeof(dbForumForumDef) + thread * ( sizeof(dbForumThreadDef) + 8 ), SEEK_SET );
    a = -1;
    fwrite( &a, 1, sizeof(int), file );
    fwrite( &lused, 1, sizeof(int), file );
    if( lused != -1 )
    {
      fseek( file, 16+sizeof(dbForumForumDef) + lused * ( sizeof(dbForumThreadDef) + 8 ), SEEK_SET );
      fwrite( &thread, 1, sizeof(int), file );
    }
    fseek( file, 4, SEEK_SET );
    fwrite( &thread, 1, sizeof(int), file );
  }

  fclose( file );

	if(forum > 100)
		sprintf( fname, "forums" );
	else
		sprintf( fname, "%s/forums", PUBLIC_FORUM_DIRECTORY );
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( forum < num )
  {
    fseek( file, 4+forum*sizeof(dbForumForumDef), SEEK_SET );
    fread( &forumd, 1, sizeof(dbForumForumDef), file );
    forumd.time = threadd.time;
    forumd.tick = threadd.tick;
    fseek( file, 4+forum*sizeof(dbForumForumDef), SEEK_SET );
    fwrite( &forumd, 1, sizeof(dbForumForumDef), file );
  }
  fclose( file );

  return 1;
}

int dbForumRemovePost( int forum, int thread, int post )
{
  int a, num, offset, offset2;
  FILE *file;
  unsigned char fname[556];
  dbForumThreadDef threadd;
  dbForumPostDef postd;

	if(forum > 100)
  	sprintf( fname, "forum%d/thread%d", forum, thread );
  else
  	sprintf( fname, "%s/forum%d/thread%d", PUBLIC_FORUM_DIRECTORY, forum, thread );
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
  fread( &num, 1, sizeof(int), file );
  fread( &offset, 1, sizeof(int), file );
  if( (unsigned int)post >= num )
  {
    fclose( file );
    return -3;
  }

  fread( &threadd, 1, sizeof(dbForumThreadDef), file );
  fseek( file, 8, SEEK_SET );
  threadd.posts--;
  fwrite( &threadd, 1, sizeof(dbForumThreadDef), file );

  fseek( file, 8+sizeof(dbForumThreadDef), SEEK_SET );
  offset = 8+sizeof(dbForumThreadDef);
  for( a = 0 ; a < num ; a++ )
  {
    if( a != post )
    {
      fread( &offset, 1, sizeof(int), file );
      fseek( file, offset, SEEK_SET );
      continue;
    }

    fread( &offset2, 1, sizeof(int), file );
    for( a++ ; a < num ; a++ )
    {
      fseek( file, offset2, SEEK_SET );
      fread( &offset2, 1, sizeof(int), file );
      fread( &postd.post, 1, sizeof(dbForumPostInDef), file );
      if( !( postd.text = malloc( (postd.post).length ) ) )
        (postd.post).length = 0;
      fread( postd.text, 1, (postd.post).length, file );

      fseek( file, offset, SEEK_SET );
      offset += 4 + sizeof(dbForumPostInDef) + (postd.post).length;
      fwrite( &offset, 1, sizeof(int), file );
      fwrite( &postd.post, 1, sizeof(dbForumPostInDef), file );
      fwrite( postd.text, 1, (postd.post).length, file );
      if( postd.text )
        free( postd.text );
    }

    fseek( file, 0, SEEK_SET );
    num--;
    fwrite( &num, 1, sizeof(int), file );
    fwrite( &offset, 1, sizeof(int), file );

    fclose( file );

		if(forum > 100)
    	sprintf( fname, "forum%d/threads", forum );
    else
    	sprintf( fname, "%s/forum%d/threads", PUBLIC_FORUM_DIRECTORY, forum );
    if( !( file = fopen( fname, "rb+" ) ))
      return -3;
    fseek( file, 16+sizeof(dbForumForumDef) + thread * ( sizeof(dbForumThreadDef) + 8 ) + 8, SEEK_SET );
    fwrite( &threadd, 1, sizeof(dbForumThreadDef), file );
    fclose( file );

    return num;
  }

  return -3;
}

int dbForumEditPost( int forum, int thread, int post, dbForumPostPtr postd )
{
  int a, b, num, offset;
  FILE *file;
  unsigned char fname[556];
  dbForumThreadDef threadd;
  dbForumPostPtr posts;
/*
  if( dbForumListPosts( forum, thread, post+1, 65536, &threadd, &posts ) < 0 )
    return -3;
*/
  posts = 0;
  dbForumListPosts( forum, thread, post+1, 65536, &threadd, &posts );

	if(forum > 100)
  	sprintf( fname, "forum%d/thread%d", forum, thread );
  else
  	sprintf( fname, "%s/forum%d/thread%d", PUBLIC_FORUM_DIRECTORY, forum, thread );
  if( !( file = fopen( fname, "rb+" ) ))
    return -3;
  fread( &num, 1, sizeof(int), file );
  fread( &offset, 1, sizeof(int), file );
  if( (unsigned int)post >= num )
  {
    if( posts )
      free( posts );
    fclose( file );
    return -3;
  }

  fseek( file, 8+sizeof(dbForumThreadDef), SEEK_SET );
  offset = 8+sizeof(dbForumThreadDef);
  for( a = 0 ; a < num ; a++ )
  {
    if( a != post )
    {
      fread( &offset, 1, sizeof(int), file );
      fseek( file, offset, SEEK_SET );
      continue;
    }

    fseek( file, offset, SEEK_SET );
    offset += 4 + sizeof(dbForumPostInDef) + (postd->post).length;
    fwrite( &offset, 1, sizeof(int), file );
    fwrite( &postd->post, 1, sizeof(dbForumPostInDef), file );
    fwrite( postd->text, 1, (postd->post).length, file );

    for( b = 0, a++ ; a < num ; a++, b++ )
    {
      fseek( file, offset, SEEK_SET );
      offset += 4 + sizeof(dbForumPostInDef) + (posts[b].post).length;
      fwrite( &offset, 1, sizeof(int), file );
      fwrite( &posts[b].post, 1, sizeof(dbForumPostInDef), file );
      fwrite( posts[b].text, 1, (posts[b].post).length, file );
      if( posts[b].text )
        free( posts[b].text );
    }
    if( posts )
      free( posts );

    fseek( file, 4, SEEK_SET );
    fwrite( &offset, 1, sizeof(int), file );

    fclose( file );
    return num;
  }

  if( posts )
    free( posts );
  return -3;
}




int dbMailList( int id, int type, int base, int end, dbMailPtr *mails, int *rtnum )
{
  int a, b, offset, num;
  FILE *file;
  dbMailPtr mailsp;
  if( ( type & 0xFFFFFFE ) )
    return -3;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_MAILIN+type ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
	if( rtnum )
    *rtnum = num;
  if( end < num )
    num = end;
  if( base > num )
  {
    fclose( file );
    return -3;
  }

  if( !( mailsp = malloc( num * sizeof(dbMailDef) ) ) )
    return -3;
  if( base == num )
  {
    fclose( file );
    *mails = mailsp;
    return 0;
  }
  offset = 8;
  for( a = 0 ; a < base ; a++ )
  {
    fseek( file, offset, SEEK_SET );
    fread( &offset, 1, sizeof(int), file );
  }

  for( b = 0 ; a < num ; a++, b++ )
  {
    fseek( file, offset, SEEK_SET );
    fread( &offset, 1, sizeof(int), file );
    fread( &mailsp[b].mail, 1, sizeof(dbMailInDef), file );
    mailsp[b].text = 0;
    if( (unsigned int)((mailsp[b].mail).length) >= 65536 )
      continue;
    if( !( mailsp[b].text = malloc( (mailsp[b].mail).length + 1 ) ) )
      continue;
    fread( mailsp[b].text, 1, (mailsp[b].mail).length, file );
    mailsp[b].text[ (mailsp[b].mail).length ] = 0;
  }

  *mails = mailsp;
  fclose( file );
  return b;
}

int dbMailAdd( int id, int type, dbMailPtr maild )
{
  int a, num, offset;
  FILE *file;

  if( ( type & 0xFFFFFFE ) )
    return -3;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_MAILIN+type ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  num++;
  fseek( file, 0, SEEK_SET );
  fwrite( &num, 1, sizeof(int), file );
  fread( &offset, 1, sizeof(int), file );

  fseek( file, offset, SEEK_SET );
  a = offset + ( 4 + sizeof(dbMailInDef) + (maild->mail).length );
  fwrite( &a, 1, sizeof(int), file );
  fwrite( &(maild->mail), 1, sizeof(dbMailInDef), file );
  fwrite( maild->text, 1, (maild->mail).length, file );

  fseek( file, 4, SEEK_SET );
  fwrite( &a, 1, sizeof(int), file );
  fclose( file );
  return 1;
}

int dbMailRemove( int id, int type, int message )
{
  int a, num, offset, offset2;
  FILE *file;
  dbMailDef maild;

  if( ( type & 0xFFFFFFE ) )
    return -3;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_MAILIN+type ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  fread( &offset, 1, sizeof(int), file );
  if( (unsigned int)message >= num )
  {
    fclose( file );
    return -3;
  }

  fseek( file, 8, SEEK_SET );
  offset = 8;
  for( a = 0 ; a < num ; a++ )
  {
    if( a != message )
    {
      fread( &offset, 1, sizeof(int), file );
      fseek( file, offset, SEEK_SET );
      continue;
    }

    fread( &offset2, 1, sizeof(int), file );
    for( a++ ; a < num ; a++ )
    {
      fseek( file, offset2, SEEK_SET );
      fread( &offset2, 1, sizeof(int), file );
      fread( &maild.mail, 1, sizeof(dbMailInDef), file );
      if( !( maild.text = malloc( (maild.mail).length ) ) )
        (maild.mail).length = 0;
      fread( maild.text, 1, (maild.mail).length, file );

      fseek( file, offset, SEEK_SET );
      offset += 4 + sizeof(dbMailInDef) + (maild.mail).length;
      fwrite( &offset, 1, sizeof(int), file );
      fwrite( &maild.mail, 1, sizeof(dbMailInDef), file );
      fwrite( maild.text, 1, (maild.mail).length, file );
      if( maild.text )
        free( maild.text );
    }

    fseek( file, 0, SEEK_SET );
    num--;
    fwrite( &num, 1, sizeof(int), file );
    fwrite( &offset, 1, sizeof(int), file );

    fclose( file );
    return num;
  }

  return -3;
}

int dbMailEmpty( int id, int type )
{
  FILE *file;
  if( ( type & 0xFFFFFFE ) )
    return -3;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_MAILIN+type ) ) )
    return -3;
  fwrite( dbFileUserListData[DB_FILE_USER_MAILIN+type], 1, dbFileUserListBase[DB_FILE_USER_MAILIN+type], file );
  fclose( file );
  return 1;
}








int dbUserSpecOpAdd( int id, dbUserSpecOpPtr specopd )
{
  int pos;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_SPECOPS ) ) )
    return -3;
  fread( &pos, 1, sizeof(int), file );
  fseek( file, 4+(pos*sizeof(dbUserSpecOpDef)), SEEK_SET );
  fwrite( specopd, 1, sizeof(dbUserSpecOpDef), file );
  fseek( file, 0, SEEK_SET );
  pos++;
  fwrite( &pos, 1, sizeof(int), file );
  fclose( file );
  return (pos-1);
}

int dbUserSpecOpRemove( int id, int specopid )
{
  int a, num;
  dbUserSpecOpDef data;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_SPECOPS ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( (unsigned int)specopid >= num )
  {
    fclose( file );
    return -2;
  }
  if( specopid+1 < num )
  {
    fseek( file, 4+(num*sizeof(dbUserSpecOpDef))-sizeof(dbUserSpecOpDef), SEEK_SET );
    fread( &data, 1, sizeof(dbUserSpecOpDef), file );
    fseek( file, 4+(specopid*sizeof(dbUserSpecOpDef)), SEEK_SET );
    fwrite( &data, 1, sizeof(dbUserSpecOpDef), file );
  }
  fseek( file, 0, SEEK_SET );
  a = num-1;
  fwrite( &a, 1, sizeof(int), file );
  fclose( file );
  return 1;
}

int dbUserSpecOpList( int id, dbUserSpecOpPtr *specopd )
{
  int num;
  dbUserSpecOpPtr specopp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_SPECOPS ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( !( specopp = malloc( num*sizeof(dbUserSpecOpDef) ) ) )
  {
    fclose( file );
    return -1;
  }
  fread( specopp, 1, num*sizeof(dbUserSpecOpDef), file );
  fclose( file );
  *specopd = specopp;
  return num;
}

int dbUserSpecOpSet( int id, int specopid, dbUserSpecOpPtr specopd )
{
  int num;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_SPECOPS ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( (unsigned int)specopid >= num )
  {
    fclose( file );
    return -2;
  }
  fseek( file, 4+(specopid*sizeof(dbUserSpecOpDef)), SEEK_SET );
  fwrite( specopd, 1, sizeof(dbUserSpecOpDef), file );
  fclose( file );
  return num;
}

int dbUserSpecOpRetrieve( int id, int specopid, dbUserSpecOpPtr specopd )
{
  int num;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_SPECOPS ) ) )
    return -3;
  fread( &num, 1, sizeof(int), file );
  if( (unsigned int)specopid >= num )
  {
    fclose( file );
    return -2;
  }
  fseek( file, 4+(specopid*sizeof(dbUserSpecOpDef)), SEEK_SET );
  fread( specopd, 1, sizeof(dbUserSpecOpDef), file );
  fclose( file );
  return num;
}

int dbUserSpecOpEmpty( int id )
{
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_SPECOPS ) ) )
    return -3;
  fwrite( dbFileUserListData[DB_FILE_USER_SPECOPS], 1, dbFileUserListBase[DB_FILE_USER_SPECOPS], file );
  fclose( file );
  return 1;
}












int dbUserDescSet( int id, dbUserDescPtr descd )
{
//  int num;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_RECORD ) ) )
    return -3;
  fseek( file, 0, SEEK_SET );
  fwrite( descd, 1, sizeof(dbUserDescDef), file );
  fclose( file );
  return 1;
}

int dbUserDescRetrieve( int id, dbUserDescPtr descd )
{
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_RECORD ) ) )
    return -3;
  fseek( file, 0, SEEK_SET );
  fread( descd, 1, sizeof(dbUserDescDef), file );
  fclose( file );
  return 1;
}



int dbUserRecordAdd( int id, dbUserRecordPtr recordd )
{
  int num = 0;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_RECORD ) ) )
    return -3;
  fseek( file, sizeof(dbUserDescDef), SEEK_SET );
  fread( &num, 1, 4, file );
  num++;
  fseek( file, sizeof(dbUserDescDef), SEEK_SET );
  fwrite( &num, 1, 4, file );
  fseek( file, sizeof(dbUserDescDef)+4 + ( num - 1 ) * sizeof(dbUserRecordDef), SEEK_SET );
  fwrite( recordd, 1, sizeof(dbUserRecordDef), file );
  fclose( file );
  return num;
}

int dbUserRecordList( int id, dbUserRecordPtr *records )
{
  int num = 0;
  dbUserRecordPtr recordp;
  FILE *file;
  if( !( file = dbFileUserOpen( id, DB_FILE_USER_RECORD ) ) )
    return -3;
  fseek( file, sizeof(dbUserDescDef), SEEK_SET );
  fread( &num, 1, 4, file );
  if( !( recordp = malloc( num * sizeof(dbUserRecordDef) ) ) )
    return -3;
  fread( recordp, 1, num * sizeof(dbUserRecordDef), file );
  fclose( file );
  *records = recordp;
  return num;
}







