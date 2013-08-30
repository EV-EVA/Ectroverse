
int cmdUserNewsAdd( int id, long long int *data, long long int flags );

int cmdFindDistPortal( int id, int tx, int ty, int *distptr, int *portalpos );

void cmdExecuteFlush();



void cmdTickInit();

int cmdTick();

void cmdTickEnd();



int cmdExecNewUser( unsigned char *name, unsigned char *pass, unsigned char *faction );
int cmdExecNewUserEmpire( int id, int famnum, unsigned char *fampass, int raceid, int level );
int cmdExecUserDeactivate( int id, int flags );
int cmdUserDelete( int id );
int cmdExecChangeName( int id, unsigned char *faction );
int cmdExecChangePassword( int id, unsigned char *pass );

int cmdExecAddBuild( int id, int type, int quantity, int plnid, int maxbuild );
int cmdExecAddBid( int id, int action, int resource, int price, int quantity );
int cmdExecRemoveBid( int id, int bidid );
int cmdExecGetMarket( int *market );
int cmdExecSendAid( int id, int destid, int fam, int *res , int nAlly);
int cmdExecGetAid( int id, int destid, int fam, int *res );
int cmdExecAidAccess( int id, int access );
int cmdExecChangeVote( int id, int vote );
int cmdExecChangFamName( int fam, unsigned char *name );
int cmdExecFamMemberFlags( int id, int fam, int flags );
int cmdExecSetFamPass( int fam, unsigned char *pass );
int cmdExecGetFamPass( int fam, unsigned char *pass );

int cmdExecFindRelation( int fam, int famtarget, int *numallies, int flags );
int cmdExecAddRelation( int fam, int type, int famtarget );
int cmdExecDelRelation( int fam, int relid );

int cmdExecSetSectsize( int id, int size );

int cmdExecSendFleetInfos( int id, int plnid, int *fr );
int cmdExecSendFleet( int id, int x, int y, int z, int order, int *sendunit );
int cmdExecSendAgents( int id, int x, int y, int z, int order, int agents );
int cmdExecSendGhosts( int id, int x, int y, int z, int order, int ghosts );
int cmdExecExploreInfo( int id, int plnid, int *ibuffer );
int cmdExecExplore( int id, int plnid, int *ibuffer );
int cmdExecChangeFleet( int id, int x, int y, int z, int order, int fltid );


int cmdExecOfferPlanet( int id, int destid, int plnid );
int cmdExecTakePlanet( int id, int plnid );



int cmdExecute( svConnectionPtr cnt, int *cmd, void *buffer, int size );

void cmdGetBuildCosts( dbUserMainPtr maind, int type, long long int *buffer );
float cmdGetBuildOvercost( int size, int total );
void cmdGetBuildOverbuild( int size, int total, long long int *buffer );

int cmdGetOpPenalty( int research, int requirement );

unsigned char *cmdTagFind( int points );


int specopAgentsAllowed( int specop, int raceid );
int specopPsychicsAllowed( int specop, int raceid );
int specopGhostsAllowed( int specop, int raceid );


int specopAgentsReadiness( int specop, dbUserMainPtr maind, dbUserMainPtr main2d );
void specopAgentsPerformOp( int id, int fltid, dbUserFleetPtr fleetd, long long int *newd );
int specopPsychicsReadiness( int specop, dbUserMainPtr maind, dbUserMainPtr main2d );
void specopPsychicsPerformOp( int id, int targetid, int specop, int psychics, long long int *newd );
int specopGhostsReadiness( int specop, dbUserMainPtr maind, dbUserMainPtr main2d );
void specopGhostsPerformOp( int id, int fltid, dbUserFleetPtr fleetd, long long int *newd );


float specopDarkWebCalc( int id );
float specopSolarCalc( int id );
float specopWarIllusionsCalc( int id );
int specopShieldingCalc( int id, int plnid );
int specopForcefieldCalc( int id, int plnid );
int specopVortexListCalc( int id, int num, int **buffer );



float battlePortalCalc( int tx, int ty, int *portals, int num, int research );
float battlePortalCoverage( int id, int tx, int ty, int research );




int artefactPrecense( dbMainPlanetPtr planetd );





void cmdFleetGetPosition( dbUserFleetPtr fleetd, int *x, int *y );

int cmdInit();
void cmdEnd();


extern long long int cmdFleetActionNewd[DB_USER_NEWS_BASE];


enum
{
CMD_LOGIN_DATABASE,
CMD_SEARCH_NAME,
CMD_SEARCH_EMPIRE,
CMD_NEWUSER,
CMD_CHANGE_KILLUSER,
CMD_CHANGE_NAME,
CMD_RETRIEVE_NAME,
CMD_CHANGE_PASSWORD,
CMD_RETRIEVE_PASSWORD,
CMD_SET_SESSION,
CMD_RETRIEVE_SESSION,
CMD_SET_SYSTEM,
CMD_RETRIEVE_SYSTEM,
CMD_SET_PLANET,
CMD_RETRIEVE_PLANET,
CMD_SET_FAMILY,
CMD_RETRIEVE_FAMILY,
CMD_SET_USERMAIN,
CMD_RETRIEVE_USERMAIN,
CMD_RETRIEVE_USERPLIST,
CMD_RETRIEVE_USERPLISTINDICES,
CMD_RETRIEVE_USERPLISTSYSTEMS,
CMD_RETRIEVE_USERPORTALS,
CMD_RETRIEVE_USERPORTALSINDICES,
CMD_RETRIEVE_USERBUILDLIST,
CMD_ADD_BUILD,
CMD_RAZE_BUILDINGS,
CMD_RAZE_PORTAL,
CMD_DISBAND_UNITS,
CMD_ADD_FLEET,
CMD_REMOVE_FLEET,
CMD_LIST_FLEET,
CMD_SET_FLEET,
CMD_RETRIEVE_FLEET,
CMD_FUND_RESEARCH,
CMD_SET_RESEARCH,
CMD_EXPLORE_INFO,
CMD_EXPLORE,
CMD_FIND_SYSTEM,
CMD_FIND_PLANET,

CMD_UNSTATION_PLANET,
CMD_ATTACK_PLANET,
CMD_LIST_MARKET,
CMD_RETRIEVE_MARKET,
CMD_ADD_BID,
CMD_LIST_BIDS,
CMD_REMOVE_BID,
CMD_SEND_AID,
CMD_CHANGE_VOTE,
CMD_SET_FAMNAME,
CMD_RETRIEVE_USERFLAGS,
CMD_SET_FAMMEMBER,
};


extern unsigned char *cmdErrorString;


enum
{
CMD_RESSOURCE_ENERGY,
CMD_RESSOURCE_MINERAL,
CMD_RESSOURCE_CRYSTAL,
CMD_RESSOURCE_ECTROLIUM
};

#define CMD_RESSOURCE_NUMUSED 4

#define CMD_RESSOURCE_TIME 4
#define CMD_RESSOURCE_POPULATION 5

extern unsigned char *cmdRessourceName[];



enum
{
CMD_BUILDING_SOLAR,
CMD_BUILDING_FISSION,
CMD_BUILDING_MINING,
CMD_BUILDING_CRYSTAL,
CMD_BUILDING_REFINEMENT,
CMD_BUILDING_CITIES,
CMD_BUILDING_RESEARCH,
CMD_BUILDING_SATS,
CMD_BUILDING_SHIELD,
};

#define CMD_BLDG_OVERBUILD 7

#define CMD_BLDG_NUMUSED 9

extern unsigned char *cmdBuildingName[];

#define CMD_POPULATION_SIZE_FACTOR 20
#define CMD_POPULATION_BASE_FACTOR 2
#define CMD_POPULATION_CITIES 1000



enum
{
CMD_UNIT_BOMBER,
CMD_UNIT_FIGHTER,
CMD_UNIT_TRANSPORT,
CMD_UNIT_CRUISER,
CMD_UNIT_CARRIER,
CMD_UNIT_SOLDIER,
CMD_UNIT_DROID,
CMD_UNIT_GOLIATH,
CMD_UNIT_PHANTOM,

CMD_UNIT_WIZARD,
CMD_UNIT_AGENT,
CMD_UNIT_GHOST,
CMD_UNIT_EXPLORATION,
};


#define CMD_UNIT_FLEET 9
#define CMD_UNIT_NUMUSED 13
extern unsigned char *cmdUnitName[CMD_UNIT_NUMUSED];



enum
{
CMD_RESEARCH_MILITARY,
CMD_RESERACH_CONSTRUCTION,
CMD_RESEARCH_TECH,
CMD_RESEARCH_ENERGY,
CMD_RESEARCH_PORTALS,
CMD_RESEARCH_WELFARE,
CMD_RESEARCH_OPERATIONS
};

#define CMD_RESEARCH_NUMUSED 7

extern unsigned char *cmdResearchName[CMD_RESEARCH_NUMUSED];
extern unsigned char *cmdResearchDesc[CMD_RESEARCH_NUMUSED];



enum
{
CMD_UNIT_STATS_AIRATTACK,
CMD_UNIT_STATS_AIRDEFENSE,
CMD_UNIT_STATS_GROUNDATTACK,
CMD_UNIT_STATS_GROUNDDEFENSE,
CMD_UNIT_STATS_SPEED,
CMD_UNIT_STATS_NETWORTH
};

#define CMD_UNIT_STATS_NUMUSED 6
#define CMD_UNIT_STATS_BATTLE 4


extern float cmdBuildingUpkeep[CMD_BLDG_NUMUSED];
extern int cmdBuildingFlags[CMD_BLDG_NUMUSED];

extern float cmdUnitUpkeep[CMD_UNIT_NUMUSED];
extern int cmdUnitStats[CMD_UNIT_NUMUSED][CMD_UNIT_STATS_NUMUSED];



#define CMD_SATS_ATTACK 110
#define CMD_SATS_DEFENCE 450
#define CMD_SHIELD_ABSORB 1300




enum
{
CMD_FLEET_ORDER_EXPLORE,
CMD_FLEET_ORDER_STATION,
CMD_FLEET_ORDER_MOVE,
CMD_FLEET_ORDER_ATTACK,
CMD_FLEET_ORDER_RECALL,
CMD_FLEET_ORDER_CANCELED,
CMD_FLEET_ORDER_MERGE,

CMD_FLEET_ORDER_SPYTARGET,
CMD_FLEET_ORDER_OBSERVEPLANET,
CMD_FLEET_ORDER_NETWORKVIRUS,
CMD_FLEET_ORDER_INFILTRATION,
CMD_FLEET_ORDER_BIOINFECTION,
CMD_FLEET_ORDER_ENERGYTRANSFER,
CMD_FLEET_ORDER_MILITARYSAB,
CMD_FLEET_ORDER_NUKEPLANET,
CMD_FLEET_ORDER_HIGHINFIL,
CMD_FLEET_ORDER_PLANETBEACON,

CMD_FLEET_ORDER_SENSE,
CMD_FLEET_ORDER_SURVEY,
CMD_FLEET_ORDER_SHIELDING,
CMD_FLEET_ORDER_FORCEFIELD,
CMD_FLEET_ORDER_VORTEX,
CMD_FLEET_ORDER_MINDCONTROL,
CMD_FLEET_ORDER_ENERGYSURGE,
};

#define CMD_FLEET_ORDER_FIRSTOP (7)
#define CMD_FLEET_ORDER_LASTOP (7+10)

#define CMD_FLEET_ORDER_FIRSTINCANT (7+10)
#define CMD_FLEET_ORDER_LASTINCANT (7+10+7)

#define CMD_FLEET_ORDER_NUMUSED (7+10+7)


extern unsigned char *cmdFleetOrderName[CMD_FLEET_ORDER_NUMUSED];



#define CMD_PLANET_FLAGS_PORTAL 1
#define CMD_PLANET_FLAGS_PORTAL_BUILD 2
#define CMD_PLANET_FLAGS_HOME 4
#define CMD_PLANET_FLAGS_BEACON 8



#define CMD_USER_FLAGS_ACTIVATED 0x1
#define CMD_USER_FLAGS_KILLED 0x10
#define CMD_USER_FLAGS_DELETED 0x20
#define CMD_USER_FLAGS_NEWROUND 0x40

#define CMD_USER_FLAGS_LEADER 0x10000
#define CMD_USER_FLAGS_VICELEADER 0x20000
#define CMD_USER_FLAGS_INDEPENDENT 0x40000




#define CMD_NEWS_FLAGS_MAIL 0x1
#define CMD_NEWS_FLAGS_BUILD 0x2
#define CMD_NEWS_FLAGS_AID 0x4
#define CMD_NEWS_FLAGS_FLEET 0x8
#define CMD_NEWS_FLAGS_ATTACK 0x10



enum
{
CMD_NEWS_BUILDING,
CMD_NEWS_UNIT,
CMD_NEWS_EXPLORE,
CMD_NEWS_EXPLORE_FAILED,
CMD_NEWS_STATION,
CMD_NEWS_STATION_FAILED,
CMD_NEWS_RECALL,
CMD_NEWS_ATTACK,
CMD_NEWS_ATTACK_FAILED,
CMD_NEWS_MARKET_SOLD,
CMD_NEWS_MARKET_BOUGHT,
CMD_NEWS_AID,
CMD_NEWS_GETAID,
CMD_NEWS_FAMATTACK,
CMD_NEWS_FAMATTACK_FAILED,
CMD_NEWS_MAIL,


CMD_NEWS_OPSPYTARGET,
CMD_NEWS_OPOBSERVEPLANET,
CMD_NEWS_OPNETWORKVIRUS,
CMD_NEWS_OPINFILTRATION,
CMD_NEWS_OPBIOINFECTION,
CMD_NEWS_OPENERGYTRANSFER,
CMD_NEWS_OPMILITARYSAB,
CMD_NEWS_OPNUKEPLANET,
CMD_NEWS_OPHIGHINFIL,
CMD_NEWS_OPPLANETBEACON,
CMD_NEWS_OPRESERVED1,
CMD_NEWS_OPRESERVED2,
CMD_NEWS_OPRESERVED3,

CMD_NEWS_OPSPYTARGET_TARGET,
CMD_NEWS_OPOBSERVEPLANET_TARGET,
CMD_NEWS_OPNETWORKVIRUS_TARGET,
CMD_NEWS_OPINFILTRATION_TARGET,
CMD_NEWS_OPBIOINFECTION_TARGET,
CMD_NEWS_OPENERGYTRANSFER_TARGET,
CMD_NEWS_OPMILITARYSAB_TARGET,
CMD_NEWS_OPNUKEPLANET_TARGET,
CMD_NEWS_OPHIGHINFIL_TARGET,
CMD_NEWS_OPPLANETBEACON_TARGET,
CMD_NEWS_OPRESERVED1T,
CMD_NEWS_OPRESERVED2T,
CMD_NEWS_OPRESERVED3T,

CMD_NEWS_OPCANCEL,


CMD_NEWS_SPIRRAECTRO,
CMD_NEWS_SPDARKWEB,
CMD_NEWS_SPINCANDESCENCE,
CMD_NEWS_SPBLACKMIST,
CMD_NEWS_SPWARILLUSIONS,
CMD_NEWS_SPASSAULT,
CMD_NEWS_SPPHANTOMS,
CMD_NEWS_SPRESERVED0,
CMD_NEWS_SPRESERVED1,
CMD_NEWS_SPRESERVED2,

CMD_NEWS_SPIRRAECTRO_TARGET,
CMD_NEWS_SPDARKWEB_TARGET,
CMD_NEWS_SPINCANDESCENCE_TARGET,
CMD_NEWS_SPBLACKMIST_TARGET,
CMD_NEWS_SPWARILLUSIONS_TARGET,
CMD_NEWS_SPASSAULT_TARGET,
CMD_NEWS_SPPHANTOMS_TARGET,
CMD_NEWS_SPRESERVED0T,
CMD_NEWS_SPRESERVED1T,
CMD_NEWS_SPRESERVED2T,

CMD_NEWS_SPCANCEL,


CMD_NEWS_INSENSE,
CMD_NEWS_INSURVEY,
CMD_NEWS_INSHIELDING,
CMD_NEWS_INFORCEFIELD,
CMD_NEWS_INVORTEX,
CMD_NEWS_INMINDCONTROL,
CMD_NEWS_INENERGYSURGE,
CMD_NEWS_INRESERVED0,
CMD_NEWS_INRESERVED1,
CMD_NEWS_INRESERVED2,

CMD_NEWS_INSENSE_TARGET,
CMD_NEWS_INSURVEY_TARGET,
CMD_NEWS_INSHIELDING_TARGET,
CMD_NEWS_INFORCEFIELD_TARGET,
CMD_NEWS_INVORTEX_TARGET,
CMD_NEWS_INMINDCONTROL_TARGET,
CMD_NEWS_INENERGYSURGE_TARGET,
CMD_NEWS_INRESERVED0_TARGET,
CMD_NEWS_INRESERVED1_TARGET,
CMD_NEWS_INRESERVED2_TARGET,

CMD_NEWS_INCANCEL,


CMD_NEWS_PLANET_OFFER,
CMD_NEWS_PLANET_TAKEN,
CMD_NEWS_PLANET_GIVEN,
CMD_NEWS_FLEETS_MERGE,


CMD_NEWS_TOTAL
};


#define CMD_NEWS_FLAGS_NEW 1

#define CMD_NEWS_EXPIRE_TIME (52*4)

#define CMD_NEWS_NUMUSED (CMD_NEWS_TOTAL)


#define CMD_NEWS_NUMOPBEGIN (CMD_NEWS_OPSPYTARGET)
#define CMD_NEWS_NUMOPEND (CMD_NEWS_OPPLANETBEACON)
#define CMD_NEWS_NUMOPTARGETBEGIN (CMD_NEWS_OPSPYTARGET_TARGET)
#define CMD_NEWS_NUMOPTARGETEND (CMD_NEWS_OPPLANETBEACON_TARGET)

#define CMD_NEWS_NUMSPBEGIN (CMD_NEWS_SPIRRAECTRO)
#define CMD_NEWS_NUMSPEND (CMD_NEWS_SPPHANTOMS)
#define CMD_NEWS_NUMSPTARGETBEGIN (CMD_NEWS_SPIRRAECTRO_TARGET)
#define CMD_NEWS_NUMSPTARGETEND (CMD_NEWS_SPPHANTOMS_TARGET)

#define CMD_NEWS_NUMINBEGIN (CMD_NEWS_INSENSE)
#define CMD_NEWS_NUMINEND (CMD_NEWS_INENERGYSURGE)
#define CMD_NEWS_NUMINTARGETBEGIN (CMD_NEWS_INSENSE_TARGET)
#define CMD_NEWS_NUMINTARGETEND (CMD_NEWS_INENERGYSURGE_TARGET)




#define CMD_FLEETS_TRAVEL_SPEED (2)



#define CMD_FLEET_FLAGS_MOVED 1
#define CMD_FLEET_FLAGS_RMERGE 2



enum
{
LEVEL_USER,
LEVEL_FORUMMOD,
LEVEL_MODERATOR,
LEVEL_ADMINISTRATOR,
LEVEL_GOD
};





#define CMD_ENERGY_DECAY (0.005)
#define CMD_CRYSTAL_DECAY (0.02)





#define CMD_RELATION_ALLY (0)
#define CMD_RELATION_WAR (1)
#define CMD_RELATION_ALLY_OFFER (0x10)





enum
{
CMD_OPER_SPYTARGET,
CMD_OPER_OBSERVEPLANET,
CMD_OPER_NETWORKVIRUS,
CMD_OPER_INFILTRATION,
CMD_OPER_BIOINFECTION,
CMD_OPER_ENERGYTRANSFER,
CMD_OPER_MILITARYSAB,
CMD_OPER_NUKEPLANET,
CMD_OPER_HIGHINFIL,
CMD_OPER_PLANETBEACON,
};

enum
{
CMD_SPELL_IRRAECTRO,
CMD_SPELL_DARKWEB,
CMD_SPELL_INCANDESCENCE,
CMD_SPELL_BLACKMIST,
CMD_SPELL_WARILLUSIONS,
CMD_SPELL_ASSAULT,
CMD_SPELL_PHANTOMS,
};

enum
{
CMD_INCANT_SENSE,
CMD_INCANT_SURVEY,
CMD_INCANT_SHIELDING,
CMD_INCANT_FORCEFIELD,
CMD_INCANT_VORTEX,
CMD_INCANT_MINDCONTROL,
CMD_INCANT_ENERGYSURGE,
};

#define CMD_AGENTOP_NUMUSED 10
#define CMD_PSYCHICOP_NUMUSED 7
#define CMD_GHOSTOP_NUMUSED 7

extern unsigned char *cmdAgentopName[CMD_AGENTOP_NUMUSED];
extern unsigned char *cmdPsychicopName[CMD_PSYCHICOP_NUMUSED];
extern unsigned char *cmdGhostopName[CMD_GHOSTOP_NUMUSED];

extern int cmdAgentopTech[CMD_AGENTOP_NUMUSED];
extern int cmdPsychicopTech[CMD_PSYCHICOP_NUMUSED];
extern int cmdGhostopTech[CMD_GHOSTOP_NUMUSED];

extern float cmdAgentopReadiness[CMD_AGENTOP_NUMUSED];
extern float cmdPsychicopReadiness[CMD_PSYCHICOP_NUMUSED];
extern int cmdGhostopReadiness[CMD_GHOSTOP_NUMUSED];

extern float cmdAgentopDifficulty[CMD_AGENTOP_NUMUSED];
extern float cmdPsychicopDifficulty[CMD_PSYCHICOP_NUMUSED];
extern float cmdGhostopDifficulty[CMD_GHOSTOP_NUMUSED];

extern int cmdAgentopStealth[CMD_AGENTOP_NUMUSED];
extern int cmdPsychicopFlags[CMD_PSYCHICOP_NUMUSED];
extern int cmdGhostopFlags[CMD_GHOSTOP_NUMUSED];




enum
{
CMD_RACE_HARKS,
CMD_RACE_MANTI,
CMD_RACE_FOOHONS,
CMD_RACE_SPACE,
CMD_RACE_DREAM,
CMD_RACE_FURTIFON,
CMD_RACE_SAMSONITES,
CMD_RACE_ULTI
};


#define CMD_RACE_NUMUSED 6 


typedef struct
{
  float growth;
  float attack;
  float defense;
  float speed;
  float researchpoints[CMD_RESEARCH_NUMUSED];
  float researchmax[CMD_RESEARCH_NUMUSED];
  float unit[CMD_UNIT_NUMUSED];
  float resource[CMD_RESSOURCE_NUMUSED];
  int special;

  int operations;
  int spells;
  int incantations;

} cmdRaceDef, *cmdRacePtr;

#define CMD_RACE_SPECIAL_POPRESEARCH 1
#define CMD_RACE_SPECIAL_SOLARP15 2
#define CMD_RACE_SPECIAL_FRP25 4
#define CMD_RACE_SPECIAL_IDW 8
#define CMD_RACE_SPECIAL_CULPROTECT 16



cmdRaceDef cmdRace[CMD_RACE_NUMUSED];

extern unsigned char *cmdRaceName[CMD_RACE_NUMUSED];





