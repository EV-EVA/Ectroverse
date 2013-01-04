/*
Losing an illusion makes you wiser than finding a truth.
- Ludwig Borne

We would rather be ruined than changed.
We would rather die in our dread,
Than climb the cross of the moment,
And let our illusions die.

Without leaving the house I know the whole universe.
- Lao-Tzu

1.	Get enough food to eat, and eat it.
2.	Find a place to sleep where it is quiet, and sleep there.
3.	Reduce intellectual and emotional noise until you arrive at the silence of yourself, and listen to it.
4.
- Richard Brautigan

If you can spend a perfectly useless afternoon in a perfectly useless manner, you have learned how to live. 
- Lin Yu-T'ang

Enlightenment is not something you achieve.  It is the absence of something.  All your life you
have been going forward after something, pursuing some goal.  Enlightenment is dropping all that.
- Charlotte Joko Beck
*/


typedef struct
{
  int id;
  int level;
  int flags;
  int reserved;
  unsigned char name[32];

  // fast access informations
  unsigned char faction[32];
  unsigned char forumtag[32];
  int session[4];
  int lasttime;

  void *next;
  void **prev;
} dbUserDef, *dbUserPtr;

extern dbUserPtr dbUserList;


int dbInit();
void dbEnd();
void dbFlush();

int dbMapFindSystem( int x, int y );
int dbMapFindValid( int x, int y );


int dbUserSearch( unsigned char *name );
int dbUserSearchFaction( unsigned char *name );
int dbUserAdd( unsigned char *name, unsigned char *faction, unsigned char *forumtag );
int dbUserRemove( int id );
dbUserPtr dbUserLinkID( int id );

int dbUserSave( int id, dbUserPtr user );

int dbUserSetPassword( int id, unsigned char *pass );
int dbUserRetrievePassword( int id, unsigned char *pass );
int dbUserLinkDatabase( void *, int id );

int dbSessionSet( dbUserPtr user, char *hash, int *session );
int dbSessionRetrieve( dbUserPtr user, int *session );


typedef struct
{
  unsigned char faction[32];
  unsigned char forumtag[32];
  long long int ressource[8];
  int empire;
  long long int infos[16];
  long long int research[8];
  int readiness[3];
  int home; // ( y << 20 ) + ( x << 8 ) + planet
  long long int totalbuilding[16];
  long long int totalunit[16];
  long long int totalresearch[8];
  long long int networth;
  int allocresearch[8];
  long long int fundresearch;
  int planets;
  int config_fleet;
  int config_flee[4];
  int createtime;
  int lasttime;
  int config_mapsize;
  int config_map[8];
  int tagpoints;
  int raceid;
  int artefacts;
  int rank;
  int aidaccess;
  long long int articount;		//Can be use to count thing for artefact (ex: delay)
} dbUserMainDef, *dbUserMainPtr;

int dbUserMainSet( int id, dbUserMainPtr main );
int dbUserMainRetrieve( int id, dbUserMainPtr main );




typedef struct
{
  int type;
  int quantity;
  int time;
  int plnid;
  int plnpos;
  long long int cost[4];
} dbUserBuildDef, *dbUserBuildPtr;

int dbUserBuildAdd( int id, int type, long long int *cost, int quantity, int time, int plnid, int plnloc );
int dbUserBuildRemove( int id, int bldid );
int dbUserBuildList( int id, dbUserBuildPtr *build );
int dbUserBuildListReduceTime( int id, dbUserBuildPtr *build );
int dbUserBuildEmpty( int id );



int dbUserPlanetNumber( int id );
int dbUserPlanetAdd( int id, int plnid, int sysid, int plnloc, int flags );
int dbUserPlanetRemove( int id, int plnid );

int dbUserPlanetSetFlags( int id, int plnid, int flags );
int dbUserPlanetListIndicesSorted( int id, int **list, int sort );
int dbUserPlanetListIndices( int id, int **list );
int dbUserPlanetListCoords( int id, int **list );
int dbUserPlanetListSystems( int id, int **list );
int dbUserPlanetListFull( int id, int **list );

int dbUserPortalsList( int id, int **list );
int dbUserPortalsListIndices( int id, int **list );
int dbUserPortalsListCoords( int id, int **list );


typedef struct
{
  int unit[16];
  int order;
  int destination;
  int destid;
  int sysid;
  int source;
  int flags;
  int time;
  int basetime;
  int reserved;
} dbUserFleetDef, *dbUserFleetPtr;

int dbUserFleetAdd( int id, dbUserFleetPtr fleetd );
int dbUserFleetRemove( int id, int fltid );
int dbUserFleetList( int id, dbUserFleetPtr *fleetd );
int dbUserFleetSet( int id, int fltid, dbUserFleetPtr fleetd );
int dbUserFleetRetrieve( int id, int fltid, dbUserFleetPtr fleetd );



int dbUserNewsAdd( int id, long long int *data, long long int flags );
long long int dbUserNewsGetFlags( int id );
int dbUserNewsList( int id, long long int **data );
long long int dbUserNewsListUpdate( int id, long long int **data, long long int time );
int dbUserNewsEmpty( int id );



int dbFamNewsAdd( int id, long long int *data );
int dbFamNewsList( int id, long long int **data, int time );



int dbEmpireMessageSet( int id, int num, unsigned char *text );
int dbEmpireMessageRetrieve( int id, int num, unsigned char *text );



extern int dbMapBInfoStatic[];

int dbMapRetrieveMain( int *binfo );

typedef struct
{
  int position; // ( y << 16 ) + x
  int indexplanet;
  int numplanets;
  int empire;
  int unexplored;
} dbMainSystemDef, *dbMainSystemPtr;

int dbMapSetSystem( int sysid, dbMainSystemPtr systemd );
int dbMapRetrieveSystem( int sysid, dbMainSystemPtr systemd );



typedef struct
{
  int system;
  int position; // ( y << 20 ) + ( x << 8 ) + planet
  int owner;
  int size;
  int flags;
  int population;
  int maxpopulation;
  int special[3];
  int building[16];
  int unit[16];
  int construction;
  int protection;
  int surrender;
  int reserved;
} dbMainPlanetDef, *dbMainPlanetPtr;

int dbMapSetPlanet( int plnid, dbMainPlanetPtr planetd );
int dbMapRetrievePlanet( int plnid, dbMainPlanetPtr planetd );



typedef struct
{
  int numplayers;
  int player[32];
  int homeid;
  int homepos; // ( y << 16 ) + x
  unsigned char name[64];
  int leader;
  char vote[32];
  int picmime;
  int pictime;
  int planets;
  int networth;
  int artefacts;
  int rank;
} dbMainEmpireDef, *dbMainEmpirePtr;

int dbMapSetEmpire( int famid, dbMainEmpirePtr empired );
int dbMapRetrieveEmpire( int famid, dbMainEmpirePtr empired );



int dbEmpireRelsAdd( int id, int *rel );
int dbEmpireRelsRemove( int id, int relid );
int dbEmpireRelsList( int id, int **rel );
int dbEmpireRelsGet( int id, int relid, int *rel );



#define DB_USER_NEWS_SIZE 256
#define DB_USER_NEWS_BASE 30


extern dbMainSystemPtr dbMapSystems;
extern int dbMapSystemsNum;




////////


enum
{
DB_MARKETBID_ACTION,
DB_MARKETBID_RESSOURCE,
DB_MARKETBID_PRICE,
DB_MARKETBID_QUANTITY,
DB_MARKETBID_USERID,

DB_MARKETBID_NUMUSED
};

#define DB_MARKETBID_BIDID 4

int dbMarketReset();
int dbMarketFull( int *list );
int dbMarketAdd( int *bid );
int dbMarketRemove( int *bid, int lcur );
int dbMarketListStart( int *bid );
int dbMarketListNext( int lcur, int *result );
int dbMarketSetQuantity( int *bid, int lcur, int quantity, int loss );

int dbUserMarketReset( int id );
int dbUserMarketAdd( int id, int bidid, int action, int resource, int price, int quantity );
int dbUserMarketList( int id, int **list );
int dbUserMarketQuantity( int id, int bidid, int quantity );
int dbUserMarketRemove( int id, int bidid );


#define DB_MARKET_RANGE 251


////////


#define DB_FORUM_NAME_SIZE 64

typedef struct
{
  unsigned char title[DB_FORUM_NAME_SIZE];
  int threads;
  int rperms;
  int wperms;
  int time;
  int tick;
  int flags;
  int reserved[2];
} dbForumForumDef, *dbForumForumPtr;

typedef struct
{
  unsigned char topic[DB_FORUM_NAME_SIZE];
  int posts;
  int authorid;
  unsigned char authorname[32];
  int time;
  int tick;
  int flags;

  int id;


/*
  int reserved;
*/
  struct in_addr sin_addr;
  unsigned char reserved[ 4 - sizeof(struct in_addr) ];


} dbForumThreadDef, *dbForumThreadPtr;

typedef struct
{
  int length;
  int authorid;
  unsigned char authorname[32];
  unsigned char authortag[32];
  int time;
  int tick;
  int flags;


/*
  int reserved[2];
*/
  struct in_addr sin_addr;
  unsigned char reserved[ 8 - sizeof(struct in_addr) ];


} dbForumPostInDef, *dbForumPostInPtr;

typedef struct
{
  unsigned char *text;
  dbForumPostInDef post;
} dbForumPostDef, *dbForumPostPtr;



int dbForumListForums( int perms, dbForumForumPtr *forums );
int dbForumListThreads( int forum, int base, int end, dbForumForumPtr forumd, dbForumThreadPtr *threads );
int dbForumListPosts( int forum, int thread, int base, int end, dbForumThreadPtr threadd, dbForumPostPtr *posts );

int dbForumRetrieveForum( int forum, dbForumForumPtr forumd );

int dbForumAddForum( dbForumForumPtr forumd, int type, int nid );
int dbForumRemoveForum( int forum );

int dbForumAddThread( int forum, dbForumThreadPtr threadd );
int dbForumRemoveThread( int forum, int thread );

int dbForumAddPost( int forum, int thread, dbForumPostPtr postd );
int dbForumRemovePost( int forum, int thread, int post );
int dbForumEditPost( int forum, int thread, int post, dbForumPostPtr postd );


#define DB_FORUM_FLAGS_POSTERROR 1

#define DB_FORUM_FLAGS_FORUMUNUSED 1

#define DB_FORUM_FLAGS_FORUMFAMILY 2

#define DB_FORUM_FLAGS_THREADFREE 1

#define DB_FORUM_FLAGS_THREAD_LOCK 0xEFEF


//////////



typedef struct
{
  int length;
  int authorid;
  unsigned char authorname[32];
  int authorempire;
  int time;
  int tick;
  int flags;
  int reserved[2];
} dbMailInDef, *dbMailInPtr;

typedef struct
{
  unsigned char *text;
  dbMailInDef mail;
} dbMailDef, *dbMailPtr;

int dbMailList( int id, int type, int base, int end, dbMailPtr *mails, int *rtnum );
int dbMailAdd( int id, int type, dbMailPtr maild );
int dbMailRemove( int id, int type, int message );
int dbMailEmpty( int id, int type );









typedef struct
{
  int type; // 0xF000=ops batch  | 0x10000 if casted by someone on the current faction
  int plnid; // planet ID, if applicable
  int plnpos; // planet position, if applicable  ( y << 20 ) + ( x << 8 ) + planet
  int factionid; // target faction, or where the op comes from
  int time;
  int vars[3];
} dbUserSpecOpDef, *dbUserSpecOpPtr;



int dbUserSpecOpAdd( int id, dbUserSpecOpPtr specopd );
int dbUserSpecOpRemove( int id, int specopid );
int dbUserSpecOpList( int id, dbUserSpecOpPtr *specopd );
int dbUserSpecOpSet( int id, int specopid, dbUserSpecOpPtr specopd );
int dbUserSpecOpRetrieve( int id, int specopid, dbUserSpecOpPtr specopd );
int dbUserSpecOpEmpty( int id );













typedef struct
{
  int flags;
  unsigned char desc[4096];
  int reserved[32];
} dbUserDescDef, *dbUserDescPtr;

typedef struct
{
  int roundid;

  int planets;
  int networth;
  unsigned char faction[32];
  unsigned char forumtag[32];
  int rank;

  int empire;
  int famplanets;
  int famnetworth;
  unsigned char famname[64];
  int famrank;
  int artefacts;
  int famflags;

  int reseved[16];
} dbUserRecordDef, *dbUserRecordPtr;

int dbUserDescSet( int id, dbUserDescPtr descd );
int dbUserDescRetrieve( int id, dbUserDescPtr descd );

int dbUserRecordAdd( int id, dbUserRecordPtr recordd );
int dbUserRecordList( int id, dbUserRecordPtr *records );




/////////////////////////////////////////////////////////////////////////////////



extern int dbArtefactPos[ARTEFACT_NUMUSED];

extern int dbArtefactMax;





