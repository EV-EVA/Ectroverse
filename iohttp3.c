
#include <sys/utsname.h>

void iohttpFuncConvertTime( unsigned char *buffer, int eltime )
{
  int up_days, up_hrs, up_mins, up_secs;
  up_days = eltime/86400;
  up_hrs = (eltime-(up_days*86400))/3600;
  up_mins = (eltime-(up_days*86400)-(up_hrs*3600))/60;
  up_secs = (eltime-(up_days*86400)-(up_hrs*3600)-(up_mins*60));
  if( up_days )
    sprintf( buffer, "%d %s %02d %s %02d %s %02d %s", up_days, ( up_days==1?"day":"days" ), up_hrs, ( up_hrs==1?"hour":"hours" ), up_mins, ( up_mins==1?"minute":"minutes" ), up_secs, ( up_secs==1?"second":"seconds" ) );
  else
    sprintf( buffer, "%d %s %02d %s %02d %s", up_hrs, ( up_hrs==1?"hour":"hours" ), up_mins, ( up_mins==1?"minute":"minutes" ), up_secs, ( up_secs==1?"second":"seconds" ) );
  return;
}


int linux_get_proc_uptime( unsigned char *buffer, float *uptime )
{
  FILE *file;
  float uptime_seconds;
  file = fopen( "/proc/uptime", "r" );
  if( file )
  {
    fscanf( file, "%f", &uptime_seconds );
    fclose( file );
    if( uptime )
      *uptime = uptime_seconds;
    iohttpFuncConvertTime( buffer, (int)uptime_seconds );
    return 1;
  }
  return 0;
}

int linux_get_proc_loadavg( unsigned char *buffer )
{
  FILE *file;
  float load_1;
  float load_5;
  float load_15;
  file = fopen( "/proc/loadavg", "r" );
  if( file )
  {
    fscanf( file, "%f" "%f" "%f", &load_1, &load_5, &load_15 );
    fclose( file );
    sprintf( buffer, "%4.2f, %4.2f, %4.2f", load_1, load_5, load_15 );
    return 1;
  }
  return 0;
}

int linux_cpuinfo( unsigned char *buffer )
{
  int a;
  FILE *file;
  unsigned char temp[4096];
  file = fopen( "/proc/cpuinfo", "r" );
  if( file )
  {
    a = fread( temp, 1, 1024, file );
    temp[a] = 0;
    fclose( file );
    for( a = 0 ; temp[a] ; a++ )
    {
      if( temp[a] == '\n' )
      {
        memcpy( buffer, "<br>", 4 );
        buffer += 4;
        continue;
      }
      *buffer++ = temp[a];
    }
    *buffer = 0;
    return 1;
  }
  return 0;
}





#include <asm/param.h>
#ifndef CT_TO_SECS
#define CT_TO_SECS(x) ((x)/HZ)
#endif


void iohttpFunc_status( svConnectionPtr cnt )
{
  int pid;
  FILE *file;
  unsigned char fname[256];
  int stutime, ststime, stpriority, ststarttime, stvsize, strss;
  unsigned char buffer[4096];
  float boottime, runtime, userload, kernelload;
  unsigned char stringuptime[1024];
  struct utsname stustname;
  struct stat stdata;

  pid = getpid();
  sprintf( fname, "/proc/%d/stat", pid );
  if( ( file = fopen( fname, "r" ) ) )
  {
    fscanf( file, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %d %d %*d %*d %d %*d %*u %*u %u %u %u", &stutime, &ststime, &stpriority, &ststarttime, &stvsize, &strss );
    fclose( file );
  }


  stringuptime[0] = 0;
  linux_get_proc_uptime( stringuptime, &boottime );
  runtime = boottime - CT_TO_SECS( ( (float)ststarttime ) );

  userload = 100.0 * ( CT_TO_SECS( ( (float)stutime ) ) / runtime );
  kernelload = 100.0 * ( CT_TO_SECS( ( (float)ststime ) ) / runtime );


  iohttpBase( cnt, 0 );


  svSendString( cnt, "<table width=\"100%\" border=\"0\"><tr><td width=\"50%\" align=\"left\" valign=\"top\">" );

  svSendString( cnt, "<table border=\"0\"><tr><td>" );
  svSendString( cnt, "<b>Ectroverse status</b><br>" );
  svSendPrintf( cnt, "General status : No problems detected<br>" ); // Should we partially keep running through signals?
  svSendPrintf( cnt, "Current date : Week %d, year %d<br>", svTickNum % 52, svTickNum / 52 );
  if( svTickStatus )
    svSendPrintf( cnt, "Tick time : %d seconds left<br>", (int)( svTickTime - time(0) ) );
  else
    svSendPrintf( cnt, "Tick time : time frozen<br>" );
  svSendPrintf( cnt, "Process priority : %d<br><br>", stpriority );
  svSendString( cnt, "<b>Server Processor(s)</b><br>" );
  linux_cpuinfo( buffer );
  svSendString( cnt, buffer );
  if( stat( "/proc/kcore", &stdata ) != -1 )
  {
    svSendString( cnt, "<b>Random Access Memory</b><br>" );
    svSendPrintf( cnt, "User-space memory : %d bytes ( %d mb )<br>", (int)stdata.st_size, (int)(stdata.st_size >> 20) );
  }
  svSendString( cnt, "</td></tr></table>" );

  svSendString( cnt, "</td><td width=\"50%\" align=\"left\" valign=\"top\">" );

  svSendString( cnt, "<table border=\"0\"><tr><td>" );
  svSendString( cnt, "<b>Server status</b><br>" );
  uname( &stustname );
  svSendPrintf( cnt, "Sysname : %s %s<br>", stustname.sysname, stustname.release );
  svSendPrintf( cnt, "Release : %s<br>", stustname.version );
  svSendPrintf( cnt, "Uptime : %s<br><br>", stringuptime );
  svSendString( cnt, "<b>Server program CPU usage ( average )</b><br>" );
  svSendPrintf( cnt, "Total usage : %.3f %%<br>", userload + kernelload );
  svSendPrintf( cnt, "In user mode : %.3f %%<br>", userload );
  svSendPrintf( cnt, "In kernel mode : %.3f %%<br><br>", kernelload );
  svSendString( cnt, "<b>Server program RAM usage ( current )</b><br>" );
  svSendPrintf( cnt, "Memory used : %d bytes ( %d mb )<br>", stvsize, stvsize >> 20 );
  svSendPrintf( cnt, "Resident set size : %d pages<br><br>", strss );
  svSendString( cnt, "</td></tr></table>" );

  svSendString( cnt, "</td></tr></table>" );


  svSendString( cnt, "</center></body></html>" );

  return;
}


void iohttpFunc_account( svConnectionPtr cnt )
{
  int a, id;
  dbUserMainDef maind;
  dbUserDescDef descd;
  unsigned char *faction, *race, *desc;
  unsigned char description[4096];

  iohttpVarsInit( cnt );
  faction = iohttpVarsFind( "newname" );
  race = iohttpVarsFind( "race" );
  desc = iohttpVarsFind( "desc" );
  iohttpVarsCut();

  iohttpBase( cnt, 1 );
  if( ( id = iohttpIdentify( cnt, 1 ) ) < 0 )
    return;
  if( !( iohttpHeader( cnt, id, &maind ) ) )
    return;

  iohttpBodyInit( cnt, "Account Options" );

  if( !( svTickStatus | svTickNum ) )
  {
    if( faction )
    {
      if( cmdExecChangeName( id, faction ) < 0 )
      {
        if( cmdErrorString )
          svSendPrintf( cnt, "<i>%s</i><br><br>", cmdErrorString );
        else
          svSendString( cnt, "<i>Error encountered while changing faction name/i><br><br>" );
      }
      else
      {
        svSendString( cnt, "<i>Faction name has been changed</i><br><br>" );
        dbUserMainRetrieve( id, &maind );
      }
    }
    if( race )
    {
      if( ( sscanf( race, "%d", &a ) == 1 ) && ( (unsigned int)a < CMD_RACE_NUMUSED ) )
      {
      	if(((cnt->dbuser)->level >= LEVEL_MODERATOR)||(a != CMD_RACE_ULTI))
      		maind.raceid = a;
        svSendString( cnt, "<i>Race changed</i><br><br>" );
        dbUserMainSet( id, &maind );
      }
    }
  }

  if( desc )
  {
    if( dbUserDescRetrieve( id, &descd ) >= 0 )
    {
      iohttpForumFilter( description, desc, 4096, 0 );
      iohttpForumFilter2( descd.desc, description, 4096 );
      dbUserDescSet( id, &descd );
    }
  }

  svSendString( cnt, "<table border=\"0\"><tr><td>" );
  svSendPrintf( cnt, "User name : <b>%s</b><br>", (cnt->dbuser)->name );
  svSendPrintf( cnt, "Faction name : <b>%s</b><br>", maind.faction );
  svSendPrintf( cnt, "Faction race : <b>%s</b><br>", cmdRaceName[maind.raceid] );
  svSendPrintf( cnt, "Forum tag : <b>%s</b><br>", maind.forumtag );
  svSendPrintf( cnt, "Tag points : <b>%d</b><br>", maind.tagpoints );
  svSendPrintf( cnt, "Account ID : <b>%d</b><br>", id );

  if( !( svTickStatus | svTickNum ) )
  {
    svSendString( cnt, "<br>Change Faction name<br><i>Only available before time starts.</i><br>" );
    svSendPrintf( cnt, "<form action=\"account\"><input type=\"text\" name=\"newname\" size=\"32\" value=\"%s\"><input type=\"submit\" value=\"Change\"></form>", maind.faction );
    svSendString( cnt, "<form action=\"account\">Faction race - <a href=\"races\">See races</a><br><i>Only available before time starts.</i><br><select name=\"race\">" );
    for( a = 0 ; a < CMD_RACE_NUMUSED-1 ; a++ )
    {
      svSendPrintf( cnt, "<option value=\"%d\"", a );
      if( a == maind.raceid )
        svSendString( cnt, " selected" );
      svSendPrintf( cnt, ">%s", cmdRaceName[a] );
    }
    svSendString( cnt, "</select><input type=\"submit\" value=\"Change\"></form>" );
  }

  description[0] = 0;
  if( dbUserDescRetrieve( id, &descd ) >= 0 )
    iohttpForumFilter3( description, descd.desc, 4096 );

  svSendString( cnt, "<form action=\"account\" method=\"POST\"><i>Faction description</i><br>" );
  svSendString( cnt, "<textarea name=\"desc\" wrap=\"soft\" rows=\"4\" cols=\"64\">" );
  svSendString( cnt, description );
  svSendString( cnt, "</textarea><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"Change\"></form><br>" );

  svSendString( cnt, "<br><a href=\"changepass\">Change your password</a>" );
  svSendString( cnt, "<br><br><a href=\"delete\">Delete your Faction</a>" );
  svSendString( cnt, "</td></tr></table>" );

  iohttpBodyEnd( cnt );
  return;
}


void iohttpFunc_changepass( svConnectionPtr cnt )
{
  int a, b, id;
  dbUserMainDef maind;
  unsigned char oldpass[32];
  unsigned char *newpass[3];

  iohttpVarsInit( cnt );
  newpass[0] = iohttpVarsFind( "password" );
  newpass[1] = iohttpVarsFind( "newpass1" );
  newpass[2] = iohttpVarsFind( "newpass2" );
  iohttpVarsCut();

  iohttpBase( cnt, 1 );
  if( ( id = iohttpIdentify( cnt, 1 ) ) < 0 )
    return;
  if( !( iohttpHeader( cnt, id, &maind ) ) )
    return;

  iohttpBodyInit( cnt, "Change password" );

  if( ( newpass[0] ) && ( newpass[1] ) && ( newpass[2] ) )
  {
    for( b = 0 ; b < 3 ; b++ )
    {
      for( a = 0 ; newpass[b][a] ; a++ )
      {
        if( newpass[b][a] == '+' )
          newpass[b][a] = ' ';
        else if( ( newpass[b][a] == 10 ) || ( newpass[b][a] == 13 ) )
          newpass[b][a] = 0;
      }
    }
    if( dbUserRetrievePassword( id, oldpass ) < 0 )
      svSendString( cnt, "<i>Error encountered when retrieving password.</i><br><br>" );
    else if( !( ioCompareExact( newpass[0], oldpass ) ) )
      svSendString( cnt, "<i>Wrong old password</i><br><br>" );
    else if( !( ioCompareExact( newpass[1], newpass[2] ) ) )
      svSendString( cnt, "<i>Different new passwords? Check your typing.</i><br><br>" );
    else if( cmdExecChangePassword( id, newpass[1] ) < 0 )
    {
      if( cmdErrorString )
        svSendPrintf( cnt, "<i>%s</i><br><br>", cmdErrorString );
      else
        svSendString( cnt, "<i>Error encountered when changing password.</i><br><br>" );
    }
    else
      svSendString( cnt, "<i>Password has been changed.</i><br><br>" );
  }

  svSendString( cnt, "<form action=\"changepass\" method=\"POST\">Current Password:<br><input type=\"text\" name=\"password\" size=\"16\"><br><br>" );
  svSendString( cnt, "New Password:<br><input type=\"text\" name=\"newpass1\" size=\"16\"><br><br>" );
  svSendString( cnt, "Repeat New Password:<br><input type=\"text\" name=\"newpass2\" size=\"16\"><br><br>" );
  svSendString( cnt, "<input type=\"submit\" name=\"Change\"></center>" );

  iohttpBodyEnd( cnt );
  return;
}


void iohttpFunc_delete( svConnectionPtr cnt )
{
  int a, b, c, id;
  dbUserMainDef maind;
  unsigned char *deletestring, *deathstring;

  iohttpVarsInit( cnt );
  deletestring = iohttpVarsFind( "delete" );
  deathstring = iohttpVarsFind( "death" );
  iohttpVarsCut();

  iohttpBase( cnt, 1 );
  if( ( id = iohttpIdentify( cnt, 1 ) ) < 0 )
    return;
  if( !( iohttpHeader( cnt, id, &maind ) ) )
    return;

  iohttpBodyInit( cnt, "Delete faction" );

  c = 3600*48;
  if( !( svTickStatus | svTickNum ) )
    c = 120;
  a = time( 0 );
  if( maind.createtime+c > a )
  {
    b = maind.createtime+c - a;
    svSendPrintf( cnt, "You must wait 48 hours after the creation of an account to delete it, or 2 minutes if time has not started yet. There are %d hours and %d minutes left.<br><br>", b/3600, (b/60)%60 );
    iohttpBodyEnd( cnt );
    return;
  }
  
        // SK: you can't delete when your empire is at war with someone or if you are declaring war on someone.
//      printf("---starting the check for deleting when in war.\n");
     
     int counterRelations, maxRelations, curfam;
     dbMainEmpireDef empired;
     unsigned char *empirestring;
     int *rel;
     int warCounter = 0;
     
     iohttpVarsInit( cnt );
     empirestring = iohttpVarsFind( "id" );
     iohttpVarsCut();
     
//     printf("printing empirestring\n");
//     printf("%u\n",empirestring);
     
     if( !( empirestring ) || ( sscanf( empirestring, "%d", &curfam ) <= 0 ) )
      curfam = maind.empire;
     
     if( dbMapRetrieveEmpire( curfam, &empired ) < 0 )
     {
      svSendString( cnt, "This empire does not seem to exist!</body></html>" );
      return;
     }
     
     if( ( maxRelations = dbEmpireRelsList( curfam, &rel ) ) >= 0 )
     {
               maxRelations <<= 2;
      for( counterRelations = 0 ; counterRelations < maxRelations ; counterRelations += 4 )
      {
       if( rel[counterRelations+3] & 1 )
        continue;
       else if( rel[counterRelations+1] == CMD_RELATION_WAR )
        warCounter++;
      }
      for( counterRelations = 0 ; counterRelations < maxRelations ; counterRelations += 4 )
      {
       if( !( rel[counterRelations+3] & 1 ) )
        continue;
       else if( rel[counterRelations+1] == CMD_RELATION_WAR )
        warCounter++;
      }
     }
//     printf("printing warcounter\n");
//      printf("%d\n",warCounter);
      if( warCounter > 0 )
      {
             svSendPrintf( cnt, "You cannot delete when your empire is at war.<br><br>");
        iohttpBodyEnd( cnt );
            return;
      }
     
//     printf("---ending the check for deleting when in war.\n");

  
  //cant delete for 24hrs after round start
 // if(((svTickNum < 144)&&(svTickNum > 0))||(svTickNum))
 // {
 // 	svSendPrintf(cnt, "You must wait 24 hours after round start to delete your account. There are %d hours and %d minutes left.<br><br>", 23 - svTickNum/6, 59-(svTickNum%6)*10);
 // 	iohttpBodyEnd( cnt );
 //   return;
 // }

  if( deletestring )
  {
    a = 0;
    sscanf( deletestring, "%d", &a );
    if( ( a == 0 ) && ( cmdUserDelete( id ) >= 0 ) )
      svSendPrintf( cnt, "Faction and account deleted!<br>" );
    else if( ( a == 1 ) && ( cmdExecUserDeactivate( id, 0 ) >= 0 ) )
      svSendPrintf( cnt, "Account deactivated!<br>" );
    else
      svSendPrintf( cnt, "Error encountered while deleting faction<br>" );
    iohttpBodyEnd( cnt );
    return;
  }

  svSendString( cnt, "<form action=\"delete\" method=\"POST\"><table><tr><td>Please specify the kind of deletion<br><select name=\"delete\">" );
  svSendString( cnt, "<option value=\"1\">Keep the account to join a different empire" );
  svSendString( cnt, "<option value=\"0\">Delete the faction and the account" );
  svSendString( cnt, "</select><br><br>How should your emperor career end?<br><select name=\"death\">" );
  svSendString( cnt, "<option value=\"0\">I retired and lived happily ever after." );
  svSendString( cnt, "<option value=\"1\">I slipped on a soap bar." );
  svSendString( cnt, "<option value=\"2\">I mixed the television and the auto-destruction device remotes." );
  svSendString( cnt, "<option value=\"3\">I got into an argument with my pet Lynx." );
  svSendString( cnt, "<option value=\"4\">I caught my tie in the blender." );
  svSendString( cnt, "<option value=\"5\">A chicken bone got stuck in my throat." );
  svSendString( cnt, "</select><br><br><input type=\"submit\" value=\"Delete\"></td></tr></table></form>" );

  iohttpBodyEnd( cnt );
  return;
}

void iohttpFunc_logout( svConnectionPtr cnt )
{
  FILE *file;
  struct stat stdata;
  unsigned char *data;

  svSendPrintf( cnt, "Set-Cookie: USRID=%04x%04x%04x%04x%04x; path=/\n", 0, 0, 0, 0, 0 );
  svSendString( cnt, "Content-Type: text/html\n\n" );

  if( stat( IOHTTP_READ_DIRECTORY "/front.html", &stdata ) != -1 )
  {
    if( ( data = malloc( stdata.st_size + 1 ) ) )
    {
      data[stdata.st_size] = 0;
      if( ( file = fopen( IOHTTP_READ_DIRECTORY "/front.html", "rb" ) ) )
      {
        fread( data, 1, stdata.st_size, file );
        svSendString( cnt, data );
        fclose( file );
      }
      free( data );
    }
  }

  return;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void iohttpFunc_admin2( svConnectionPtr cnt )
{
  int id;
  if( ( id = iohttpIdentify( cnt, 0 ) ) < 0 )
    goto denied;
  if( (cnt->dbuser)->level < LEVEL_ADMINISTRATOR )
    goto denied;
  svSendString( cnt, "Content-Type: text/html\n\n" );
  svSendString( cnt, "<html><head><title>Ectroverse</title></head><frameset cols=\"155,*\" framespacing=\"0\" border=\"0\" marginwidth=\"0\" marginheight=\"0\" frameborder=\"no\">" );
  svSendString( cnt, "<frame src=\"adminmenu\" name=\"menu\" marginwidth=\"0\" marginheight=\"0\" scrolling=\"no\" noresize>" );
  svSendString( cnt, "<frame src=\"adminserver\" name=\"main\" marginwidth=\"0\" marginheight=\"0\" noresize>" );
  svSendString( cnt, "<noframes>Your browser does not support frames! That's uncommon :).<br><br><a href=\"menu\">Menu</a></noframes>" );
  svSendString( cnt, "</frameset></html>" );
  return;
  denied:
  svSendString( cnt, "You do not have administrator privileges." );
  svSendString( cnt, "</body></html>" );
  return;
}

void iohttpFunc_adminmenu( svConnectionPtr cnt )
{
  int id;
  if( ( id = iohttpIdentify( cnt, 0 ) ) < 0 )
    goto denied;
  if( (cnt->dbuser)->level < LEVEL_ADMINISTRATOR )
    goto denied;
  svSendString( cnt, "Content-Type: text/html\n\n" );
  svSendString( cnt, "<html><head><style type=\"text/css\">a {\ntext-decoration: none\n}\na:hover {\ncolor: #00aaaa\n}\n</style></head><body bgcolor=\"#000080\" text=\"#00bb00\" link=\"#00bb00\" alink=\"#00bb00\" vlink=\"#00bb00\" leftmargin=\"0\" background=\"http://users.pandora.be/amedee/ectroverse/mbg.gif\">" );
  svSendString( cnt, "<b><font face=\"Tahoma\" size=\"2\">" );

  svSendString( cnt, "<a href=\"adminforum\" target=\"main\">Forums</a><br>" );
  svSendString( cnt, "<a href=\"admintags\" target=\"main\">Player Tags</a><br>" );


  svSendString( cnt, "<a href=\"adminmarket\" target=\"main\">Market</a><br>" );
  svSendString( cnt, "<a href=\"adminplanet\" target=\"main\">Planets</a><br>" );
  svSendString( cnt, "<a href=\"adminempire\" target=\"main\">Empire</a><br>" );
  svSendString( cnt, "<a href=\"adminviewacc\" target=\"main\">View Account</a><br>" );
  svSendString( cnt, "<a href=\"adminaccount\" target=\"main\">Change Account</a><br>" );
  svSendString( cnt, "<a href=\"adminplayer\" target=\"main\">Player Resources</a><br>" );
  svSendString( cnt, "<a href=\"adminresearch\" target=\"main\">Player Research</a><br>" );
  svSendString( cnt, "<a href=\"adminbuild\" target=\"main\">Player Build List</a><br>" );
  svSendString( cnt, "<a href=\"adminfleet\" target=\"main\">Player Fleets&Ops</a><br>" );
  svSendString( cnt, "<a href=\"adminmap\" target=\"main\">Map</a><br>" );
  svSendString( cnt, "<a href=\"adminip\" target=\"main\">IP-Check</a><br>" );
  svSendString( cnt, "<a href=\"adminmods\" target=\"main\">Mods & Admins</a><br><br>" );
  svSendString( cnt, "<a href=\"main\" target=\"_top\">Back to Game</a><br>" );
  svSendString( cnt, "<a href=\"/\" target=\"_top\">Back to Mainpage</a><br>" );
  svSendString( cnt, "</font></b>" );
  svSendString( cnt, "</body></html>" );
  denied:
  svSendString( cnt, "You do not have administrator privileges." );
  svSendString( cnt, "</body></html>" );
  return;
}


void iohttpAdminForm( svConnectionPtr cnt, unsigned char *target )
{
  svSendPrintf( cnt, "<form action=\"%s\" method=\"POST\">", target );
  return;
}

void iohttpAdminSubmit( svConnectionPtr cnt, unsigned char *name )
{
  svSendPrintf( cnt, "<input type=\"submit\" value=\"%s\"></form><br><br>", name );
  return;
}



void iohttpFunc_adminforum( svConnectionPtr cnt )
{
  int a, id;
  unsigned char *actionstring, *str0;
  dbForumForumDef forumd;

  iohttpBase( cnt, 1 );
  if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
    return;
  if( (cnt->dbuser)->level < LEVEL_FORUMMOD )
  {
    svSendString( cnt, "This account does not have adminitrator privileges" );
    svSendString( cnt, "</center></body></html>" );
    return;
  }

  iohttpVarsInit( cnt );


  if( ( actionstring = iohttpVarsFind( "famforum" ) ) )
  {
    for( a = 0 ; a < dbMapBInfoStatic[4] ; a++ )
    {
      sprintf( forumd.title, "Empire %d forum", a );
      forumd.rperms = 2;
      forumd.wperms = 2;
      forumd.flags = DB_FORUM_FLAGS_FORUMFAMILY;
      dbForumAddForum( &forumd, 1, 100+a );
    }
    svSendString( cnt, "<i>Empire forums created</i><br><br>" );
  }

  if( ( actionstring = iohttpVarsFind( "crforum" ) ) )
  {
    forumd.threads = 0;
    forumd.time = 0;
    forumd.tick = 0;
    forumd.flags = 0;
    for( a = 0 ; ( a < 63 ) && ( actionstring[a] ) ; a++ );
    actionstring[a] = 0;
    memcpy( forumd.title, actionstring, a );
    if( !( str0 = iohttpVarsFind( "crforumread" ) ) )
      goto cancel;
    if( !( sscanf( str0, "%d", &forumd.rperms ) ) )
      goto cancel;
    if( !( str0 = iohttpVarsFind( "crforumwrite" ) ) )
      goto cancel;
    if( !( sscanf( str0, "%d", &forumd.wperms ) ) )
      goto cancel;
    forumd.flags = 0;
    dbForumAddForum( &forumd, 1, 0 );
    svSendString( cnt, "<i>Forum created</i><br><br>" );
  }

  if( ( actionstring = iohttpVarsFind( "delforum" ) ) )
  {
    if( !( sscanf( actionstring, "%d", &a ) ) )
      goto cancel;
    if( dbForumRemoveForum( a ) >= 0 )
      svSendString( cnt, "<i>Forum deleted</i><br><br>" );
  }



  meat:

  iohttpAdminForm( cnt, "adminforum" );
  svSendString( cnt, "<input type=\"hidden\" name=\"famforum\"><br>" );
  iohttpAdminSubmit( cnt, "Create empire forums" );

  iohttpAdminForm( cnt, "adminforum" );
  svSendString( cnt, "<input type=\"text\" name=\"crforum\" value=\"Forum name\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"crforumread\" value=\"Read permission\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"crforumwrite\" value=\"Write permission\"><br>" );
  iohttpAdminSubmit( cnt, "Create forum" );

  iohttpAdminForm( cnt, "adminforum" );
  svSendString( cnt, "<input type=\"text\" name=\"delforum\" value=\"Forum number\"><br>" );
  iohttpAdminSubmit( cnt, "Delete forum" );



  svSendString( cnt, "</center></body></html>" );
  return;


  cancel:
  svSendString( cnt, "<i>Command refused, make sure you didn't make an error in the input</i><br><br>" );
  goto meat;

  return;
}








void iohttpFunc_moderator( svConnectionPtr cnt )
{
  int id, a, b, c, d, x, y, num, actionid, i0, numbuild, curtime, cmd[2];
  long long int i1;
  float fa;
  dbUserBuildPtr build;
  dbUserMainDef maind, main2d;
  dbUserFleetDef fleetd;
  dbUserFleetPtr fleetp;
  dbMainPlanetDef planetd;
  dbUserBuildPtr buildd;
  //dbForumForumDef forumd;
  //dbForumThreadPtr pThread;
  unsigned char *actionstring, *str0, *str1;
  dbUserPtr user;
  long long int *newsp, *newsd;
  int *buffer;
  int *plist;
  dbMainEmpireDef empired;
  FILE *file;


  iohttpBase( cnt, 1 );
  if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
    return;
  if( (cnt->dbuser)->level < LEVEL_MODERATOR )
  {
    svSendString( cnt, "This account does not have moderator privileges" );
    svSendString( cnt, "</center></body></html>" );
    return;
  }
    if( !( file = fopen( LOGS_DIRECTORY "/modlog.txt", "a+t" ) ) )
        return;


  svSendString( cnt, "<b>First draft of moderator panel</b><br><br><table width=\"90%\"><tr><td width=\"30%\" align=\"left\" valign=\"top\">" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"player\" value=\"player ID number\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"View player data\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"playernews\" value=\"player ID number\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"View player news\"></form><br><br>" );

  svSendString( cnt, "<form action=\"famnews\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"id\" value=\"empire number\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"View empire news\"></form><br><br>" );

  svSendString( cnt, "<form action=\"forum\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"forum\" value=\"empire number + 100\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"View forum\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"playerpts\" value=\"player ID number\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"playerptsnum\" value=\"Points to add\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"Add tag points\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"playertag\" value=\"player ID number\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"playertagname\" value=\"New tag\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"Change forum tag\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"playerres\" value=\"player ID number\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"resource\" value=\"res ID\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"resqt\" value=\"quantity\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"Set resources\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"seemarket\" value=\"player ID number\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"View bids\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"clearmarket\" value=\"player ID number\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"Clear bids\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"seebuild\" value=\"player ID number\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"See build\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"clearbuild\" value=\"player ID number\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"Clear build\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"rmbuild\" value=\"player ID number\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"rmbuilditem\" value=\"build ID\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"Remove build item\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"seeresearch\" value=\"player ID number\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"See research\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"setfunding\" value=\"player ID number\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"setfundingqt\" value=\"funding\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"Set research funding\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"setresearch\" value=\"player ID number\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"setresearchid\" value=\"research field ID\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"setresearchqt\" value=\"research points\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"Set research points\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"setreadiness\" value=\"player ID number\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"setreadinessid\" value=\"readiness ID\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"setreadinessqt\" value=\"readiness %\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"Set readiness\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"seefleets\" value=\"player ID number\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"See fleets\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"mainfleet\" value=\"player ID number\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"mainfleetid\" value=\"unit ID\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"mainfleetqt\" value=\"unit QT\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"Set main fleet unit\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"clearops\" value=\"player ID number\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"Clear ops\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"giveplanet\" value=\"player ID number\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"giveplanetid\" value=\"planet ID\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"Give planet\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"setportal\" value=\"planet ID number\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"setportalst\" value=\"1 or 0\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"Set portal\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"setbuilding\" value=\"planet ID number\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"setbuildingid\" value=\"building type\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"setbuildingqt\" value=\"quantity\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"Set building\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"givepop\" value=\"player ID number\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"Give population\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"clearfam\" value=\"Family\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"players\" value=\"Number of players\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"Fix Fam\"></form><br><br>" );

  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"changestatus\" value=\"player ID number\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"statusid\" value=\"StatusID\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"Change player status\"></form><br><br>" );
/*
  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"forumlid\" value=\"Forum ID\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"threadlid\" value=\"Thread ID\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"Lock Thread\"></form><br><br>" );
  
  svSendString( cnt, "<form action=\"moderator\" method=\"POST\">" );
  svSendString( cnt, "<input type=\"text\" name=\"forumuid\" value=\"Forum ID\"><br>" );
  svSendString( cnt, "<input type=\"text\" name=\"threaduid\" value=\"Thread ID\"><br>" );
  svSendString( cnt, "<input type=\"submit\" value=\"Unlock Thread\"></form><br><br>" );
*/

  svSendString( cnt, "</td><td width=\"60%\" align=\"left\" valign=\"top\">" );
	
	id = iohttpIdentify( cnt, 0 );
  if( id != -1 )
  {
    cmd[0] = CMD_RETRIEVE_USERMAIN;
    cmd[1] = id;
    cmdExecute( cnt, cmd, &main2d, 0 );
  }

  iohttpVarsInit( cnt );

  if( ( actionstring = iohttpVarsFind( "player" ) ) )
  {
    iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( !( user = dbUserLinkID( actionid ) ) )
      goto iohttpFunc_moderatorL0;
    if( dbUserMainRetrieve( actionid, &maind ) < 0 )
      goto iohttpFunc_moderatorL0;
    svSendPrintf( cnt, "<b>Player ID %d</b><br><br>", actionid );
    svSendPrintf( cnt, "Faction name : %s<br>", maind.faction );
	
	//routine to show how long it has been since a player was online
	curtime = time( 0 );
	b = curtime - maind.lasttime;
	if( b < 5*60 ){
		svSendString( cnt, "[online]<br>" );
	}
	else{
		svSendString( cnt, "<i>Last : " );
		if( b >= 24*60*60 )
		{
		  svSendPrintf( cnt, "%dd ", b/(24*60*60) );
		  b %= 24*60*60;
		}
		if( b >= 60*60 )
		{
		  svSendPrintf( cnt, "%dh ", b/(60*60) );
		  b %= 60*60;
		}
		if( b >= 60 )
		  svSendPrintf( cnt, "%dm <br>", b/60 );
	}
	
	
	
    svSendPrintf( cnt, "Forum tag : %s<br>", maind.forumtag );
    svSendPrintf( cnt, "Tag points : %d<br>", maind.tagpoints );
    svSendPrintf( cnt, "User level : %d<br><br>", user->level );
    svSendPrintf( cnt, "Planets : %d<br>", maind.planets );
    svSendPrintf( cnt, "Networth : %lld<br>", maind.networth );
    svSendPrintf( cnt, "Empire : #%d<br>", maind.empire );
    svSendPrintf( cnt, "Artefacts : 0x%x<br>", maind.artefacts );
	
    for( a = 0; a < CMD_RESSOURCE_NUMUSED ; a++ )
      svSendPrintf( cnt, "%s : %lld<br>", cmdRessourceName[a], maind.ressource[a] );
    for( a = 0; a < CMD_RESEARCH_NUMUSED ; a++ )
      svSendPrintf( cnt, "%s : %lld %%<br>", cmdResearchName[a], maind.totalresearch[a] );
    svSendPrintf( cnt, "Fleet readiness : %d %%<br>", maind.readiness[0] >> 16 );
    svSendPrintf( cnt, "Psychics readiness : %d %%<br>", maind.readiness[1] >> 16 );
    svSendPrintf( cnt, "Agents readiness : %d %%<br>", maind.readiness[2] >> 16 );
    
    fprintf( file, "%s > view player info of player %s\n", main2d.faction, maind.faction);
  }

  if( ( actionstring = iohttpVarsFind( "playernews" ) ) )
  {
    iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( !( user = dbUserLinkID( actionid ) ) )
      goto iohttpFunc_moderatorL0;
    svSendPrintf( cnt, "Current date : Week %d, year %d<br>", svTickNum % 52, svTickNum / 52 );
    if( svTickStatus )
      svSendPrintf( cnt, "%d seconds before tick<br>", (int)( svTickTime - time(0) ) );
    else
      svSendPrintf( cnt, "Time frozen<br>" );
    num = dbUserNewsList( actionid, &newsp );
    newsd = newsp;
    if( !( num ) )
      svSendString( cnt, "<b>No reports</b>" );
    for( a = 0 ; a < num ; a++, newsd += DB_USER_NEWS_BASE )
      iohttpNewsString( cnt, newsd );
    if( newsp )
      free( newsp );
    cmd[0] = CMD_RETRIEVE_USERMAIN;
    cmd[1] = actionid;
    cmdExecute( cnt, cmd, &maind, 0 );
    fprintf( file, "%s > view news of player %s \n",main2d.faction, maind.faction);
  }

  if( ( actionstring = iohttpVarsFind( "playerpts" ) ) )
  {
    if( !( str0 = iohttpVarsFind( "playerptsnum" ) ) )
      goto iohttpFunc_moderatorL0;
    if( sscanf( str0, "%d", &a ) != 1 )
      goto iohttpFunc_moderatorL0;
    iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( dbUserMainRetrieve( actionid, &maind ) < 0 )
      goto iohttpFunc_moderatorL0;
    maind.tagpoints += a;
    sprintf( maind.forumtag, cmdTagFind( maind.tagpoints ) );
    dbUserMainSet( actionid, &maind );
    svSendPrintf( cnt, "The tag points of <b>%s</b> have been increased by <b>%d</b> for a total of <b>%d</b>, tag set to <b>%s</b><br><br>", maind.faction, a, maind.tagpoints, maind.forumtag );
    fprintf( file, "%s > Tag points of player %s have been increased by %d tot a total of %d \n",main2d.faction, maind.faction,a, maind.tagpoints);
  }

  if( ( actionstring = iohttpVarsFind( "playertag" ) ) )
  {
    if( !( str0 = iohttpVarsFind( "playertagname" ) ) )
      goto iohttpFunc_moderatorL0;
    iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( !( user = dbUserLinkID( actionid ) ) )
      goto iohttpFunc_moderatorL0;
    if( ( user->level >= (cnt->dbuser)->level ) && ( id != actionid ) )
    {
      svSendString( cnt, "Permission denied<br><br>" );
      goto iohttpFunc_moderatorL1;
    }
    if( dbUserMainRetrieve( actionid, &maind ) < 0 )
      goto iohttpFunc_moderatorL0;
    maind.forumtag[31] = 0;
    iohttpForumFilter( maind.forumtag, str0, 31, 1 );
    dbUserMainSet( actionid, &maind );
    svSendPrintf( cnt, "Tag of <b>%s</b> changed to <b>%s</b><br><br>", maind.faction, maind.forumtag );
    fprintf( file, "%s > Tag of player %s changed to %s \n",main2d.faction, maind.faction, maind.forumtag);
  }

  if( ( actionstring = iohttpVarsFind( "playerres" ) ) )
  {
    if( !( str0 = iohttpVarsFind( "resource" ) ) )
      goto iohttpFunc_moderatorL0;
    if( !( str1 = iohttpVarsFind( "resqt" ) ) )
      goto iohttpFunc_moderatorL0;
    iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( sscanf( str0, "%d", &i0 ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( sscanf( str1, "%lld", &i1 ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( dbUserMainRetrieve( actionid, &maind ) < 0 )
      goto iohttpFunc_moderatorL0;
    if( (unsigned int)i0 >= 4 )
      goto iohttpFunc_moderatorL0;
    maind.ressource[i0] = i1;
    dbUserMainSet( actionid, &maind );
    svSendPrintf( cnt, "Resource %d of %d changed to %lld.<br><br>", i0, actionid, i1 );
    fprintf( file, "%s > Resource %d of player %s changed to %lld \n",main2d.faction,i0, maind.faction, i1);
  }

  if( ( actionstring = iohttpVarsFind( "seemarket" ) ) )
  {
    iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    b = dbUserMarketList( actionid, &buffer );
    svSendString( cnt, "<b>Bids</b><br>" );
    if( b <= 0 )
      svSendString( cnt, "None<br>" );
    for( a = c = 0 ; a < b ; a++, c += 5 )
    {
      if( !( buffer[c+DB_MARKETBID_ACTION] ) )
        svSendString( cnt, "Buying" );
      else
        svSendString( cnt, "Selling" );
      svSendPrintf( cnt, " %d %s at %d<br>", buffer[c+DB_MARKETBID_QUANTITY], cmdRessourceName[buffer[c+DB_MARKETBID_RESSOURCE]+1], buffer[c+DB_MARKETBID_PRICE] );
    }
    svSendString( cnt, "<br>" );
    if( buffer )
      free( buffer );
    fprintf( file, "%s > market of player %s viewed \n",main2d.faction, maind.faction);

  }

  if( ( actionstring = iohttpVarsFind( "clearmarket" ) ) )
  {
    iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( ( c = dbUserMarketList( actionid, &buffer ) ) >= 0 )
    {
      for( a = b = 0 ; a < c ; a++, b += 5 )
      {
        d = dbUserMarketRemove( actionid, buffer[b+DB_MARKETBID_BIDID] );
        d = dbMarketRemove( &buffer[b], buffer[b+DB_MARKETBID_BIDID] );
      }
      free( buffer );
    }
    svSendPrintf( cnt, "Bids of %d cleared.<br>", actionid );
    fprintf( file, "%s > market of player %s cleared \n",main2d.faction, maind.faction);

  }

  if( ( actionstring = iohttpVarsFind( "seebuild" ) ) )
  {
    iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( ( numbuild = dbUserBuildList( actionid, &build ) ) < 0 )
      goto iohttpFunc_moderatorL0;
    for( a = c = 0 ; a < numbuild ; a++ )
    {
      if( build[a].type >> 16 )
        continue;
      svSendPrintf( cnt, "<b>%d</b> %d %s in %d weeks at <a href=\"planet?id=%d\">%d,%d:%d</a><br>", a, build[a].quantity, cmdBuildingName[ build[a].type & 0xFFFF ], build[a].time, build[a].plnid, ( build[a].plnpos >> 8 ) & 0xFFF, build[a].plnpos >> 20, build[a].plnpos & 0xFF );
    }
    for( a = c = 0 ; a < numbuild ; a++ )
    {
      if( !( build[a].type >> 16 ) )
        continue;
      svSendPrintf( cnt, "<b>%d</b> %d %s in %d weeks<br>", a, build[a].quantity, cmdUnitName[ build[a].type & 0xFFFF ], build[a].time );
    }
    free( build );
    fprintf( file, "%s > build of player %s viewed \n",main2d.faction, maind.faction);
  }

  if( ( actionstring = iohttpVarsFind( "rmbuild" ) ) )
  {
    if( !( str0 = iohttpVarsFind( "rmbuilditem" ) ) )
      goto iohttpFunc_moderatorL0;
    iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( sscanf( str0, "%d", &i0 ) != 1 )
      goto iohttpFunc_moderatorL0;
    a = dbUserBuildRemove( actionid, i0 );
    svSendPrintf( cnt, "Removing build item %d from player %d, return code : %d.<br>", i0, actionid, a );

    fprintf( file, "%s >buildings in build of player %s CLEARED \n",main2d.faction, maind.faction);

  }

  if( ( actionstring = iohttpVarsFind( "seeresearch" ) ) )
  {
    iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( dbUserMainRetrieve( actionid, &maind ) < 0 )
      goto iohttpFunc_moderatorL0;
    svSendPrintf( cnt, "Funding : %lld<br>", maind.fundresearch );
    svSendString( cnt, "<table width=\"90%%\" cellspacing=\"8\">" );
    for( a = 0 ; a < CMD_RESEARCH_NUMUSED ; a++ )
      svSendPrintf( cnt, "<tr><td nowrap><b>%s</b></td><td nowrap>%lld Points</td><td nowrap>%lld%%</td></tr>", cmdResearchName[a], maind.research[a], maind.totalresearch[a] );
    svSendString( cnt, "</table>" );
    fprintf( file, "%s >research of player %s viewed \n",main2d.faction, maind.faction);
  }

  if( ( actionstring = iohttpVarsFind( "setfunding" ) ) )
  {
    if( !( str0 = iohttpVarsFind( "setfundingqt" ) ) )
      goto iohttpFunc_moderatorL0;
    iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( sscanf( str0, "%d", &i0 ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( dbUserMainRetrieve( actionid, &maind ) < 0 )
      goto iohttpFunc_moderatorL0;
    maind.fundresearch = i0;
    dbUserMainSet( actionid, &maind );
    svSendPrintf( cnt, "Research funding of %d set to %lld.", actionid, maind.fundresearch );
    fprintf( file, "%s >research funding of player %s set to %lld\n",main2d.faction, maind.faction , maind.fundresearch);

  }

  if( ( actionstring = iohttpVarsFind( "setresearch" ) ) )
  {
    if( !( str0 = iohttpVarsFind( "setresearchid" ) ) )
      goto iohttpFunc_moderatorL0;
    if( !( str1 = iohttpVarsFind( "setresearchqt" ) ) )
      goto iohttpFunc_moderatorL0;
    iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( sscanf( str0, "%d", &i0 ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( sscanf( str1, "%lld", &i1 ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( dbUserMainRetrieve( actionid, &maind ) < 0 )
      goto iohttpFunc_moderatorL0;
    if( (unsigned int)i0 >= CMD_RESEARCH_NUMUSED )
      goto iohttpFunc_moderatorL0;
    maind.research[i0] = i1;

    for( a = 0 ; a < CMD_RESEARCH_NUMUSED ; a++ )
    {
      fa = cmdRace[maind.raceid].researchmax[a];
      maind.totalresearch[a] = fa * ( 1.0 - exp( (double)maind.research[a] / ( -10.0 * (double)maind.networth ) ) );
    }

    dbUserMainSet( actionid, &maind );
    svSendPrintf( cnt, "Research points of player %d in research field %d set to %lld.", actionid, i0, i1 );
    fprintf( file, "%s >research points of player %s in research field %d set to %lld \n",main2d.faction, maind.faction , i0, i1);
  }

  if( ( actionstring = iohttpVarsFind( "setreadiness" ) ) )
  {
    if( !( str0 = iohttpVarsFind( "setreadinessid" ) ) )
      goto iohttpFunc_moderatorL0;
    if( !( str1 = iohttpVarsFind( "setreadinessqt" ) ) )
      goto iohttpFunc_moderatorL0;
    iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( sscanf( str0, "%d", &i0 ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( sscanf( str1, "%lld", &i1 ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( dbUserMainRetrieve( actionid, &maind ) < 0 )
      goto iohttpFunc_moderatorL0;
    if( (unsigned int)i0 >= 3 )
      goto iohttpFunc_moderatorL0;
    maind.readiness[i0] = i1 << 16;
    dbUserMainSet( actionid, &maind );
    svSendPrintf( cnt, "Readiness %d of player %d set to %lld.", i0, actionid, i1 );
    fprintf( file, "%s >Readiness %d of player %s set to %lld \n",main2d.faction, i0,maind.faction,i1);
  }

  if( ( actionstring = iohttpVarsFind( "seefleets" ) ) )
  {
    iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( ( num = dbUserFleetList( actionid, &fleetp ) ) < 0 )
      goto iohttpFunc_moderatorL0;
    svSendString( cnt, "<b>Main fleet</b><br><br>" );
    for( a = b = 0 ; a < CMD_UNIT_NUMUSED ; a++ )
    {
      if( fleetp[0].unit[a] )
      {
        svSendPrintf( cnt, "%d %s<br>", fleetp[0].unit[a], cmdUnitName[a] );
        b = 1;
      }
    }
    svSendString( cnt, "<br><b>Travelling fleets</b><br><br>" );
    if( num == 1 )
      svSendString( cnt, "None<br>" );
    else
    {
      svSendString( cnt, "<table width=\"90%%\" cellspacing=\"5\"><tr><td width=\"50%\">Units</td><td width=\"50%\">Position</td></tr>" );
      for( a = 1 ; a < num ; a++ )
      {
        svSendString( cnt, "<tr><td>" );
        for( b = 0 ; b < CMD_UNIT_NUMUSED ; b++ )
        {
          if( fleetp[a].unit[b] )
            svSendPrintf( cnt, "%d %s<br>", fleetp[a].unit[b], cmdUnitName[b] );
        }
        cmdFleetGetPosition( &fleetp[a], &x, &y );
        svSendPrintf( cnt, "<td>%d,%d</td></tr>", x, y );
      } 
      svSendString( cnt, "</table>" );
    }
    fprintf( file, "%s >fleet of player %s viewed \n",main2d.faction, maind.faction);
  }

  if( ( actionstring = iohttpVarsFind( "mainfleet" ) ) )
  {
    if( !( str0 = iohttpVarsFind( "mainfleetid" ) ) )
      goto iohttpFunc_moderatorL0;
    if( !( str1 = iohttpVarsFind( "mainfleetqt" ) ) )
      goto iohttpFunc_moderatorL0;
    iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( sscanf( str0, "%d", &i0 ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( sscanf( str1, "%lld", &i1 ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( dbUserFleetRetrieve( actionid, 0, &fleetd ) < 0 )
      goto iohttpFunc_moderatorL0;
    if( (unsigned int)i0 >= CMD_UNIT_NUMUSED )
      goto iohttpFunc_moderatorL0;
    fleetd.unit[i0] = i1;
    dbUserFleetSet( actionid, 0, &fleetd );
    svSendPrintf( cnt, "Unit %d of player %d set to %lld.", i0, actionid, i1 );
    fprintf( file, "%s >unit %d of player %s set to %lld \n",main2d.faction, i0, maind.faction, i1);
  }

  if( ( actionstring = iohttpVarsFind( "clearops" ) ) )
  {
    iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    a = dbUserSpecOpEmpty( actionid );
    svSendPrintf( cnt, "Cleared spec ops on %d : %d.", actionid, a );
    fprintf( file, "%s >ops on/of player %s cleared \n",main2d.faction, maind.faction);
  }

  if( ( actionstring = iohttpVarsFind( "giveplanet" ) ) )
  {
  	if( !( str0 = iohttpVarsFind( "giveplanetid" ) ) )
      goto iohttpFunc_moderatorL0;
    iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( sscanf( str0, "%d", &i0 ) != 1 )
      goto iohttpFunc_moderatorL0;

    if( dbUserMainRetrieve( actionid, &maind ) < 0 )
      goto iohttpFunc_moderatorL0;
    if( dbMapRetrievePlanet( i0, &planetd ) < 0 )
      goto iohttpFunc_moderatorL0;

    if( planetd.owner != -1 )
    {
      dbUserPlanetRemove( planetd.owner, i0 );
      if( planetd.construction )
      {
        b = dbUserBuildList( planetd.owner, &buildd );
        for( a = b-1 ; a >= 0 ; a-- )
        {
          if( buildd[a].plnid == actionid )
            dbUserBuildRemove( planetd.owner, a );
        }
        if( buildd )
          free( buildd );
      }
    }

    maind.planets++;
    memset( planetd.building, 0, CMD_BLDG_NUMUSED*sizeof(int) );
    memset( planetd.unit, 0, CMD_UNIT_NUMUSED*sizeof(int) );
    planetd.maxpopulation = (float)( planetd.size * CMD_POPULATION_SIZE_FACTOR );
    planetd.construction = 0;
    planetd.population = planetd.size * CMD_POPULATION_BASE_FACTOR;
    planetd.owner = actionid;

    dbUserPlanetAdd( actionid, i0, planetd.system, planetd.position, 0 );
    dbMapSetPlanet( i0, &planetd );
    dbUserMainSet( actionid, &maind );
    svSendPrintf( cnt, "Planet %d transfered to %d", i0, actionid );
    fprintf( file, "%s >planet %d transferred to player %s \n",main2d.faction,i0 ,maind.faction);
  }

  if( ( actionstring = iohttpVarsFind( "setportal" ) ) )
  {
    if( !( str0 = iohttpVarsFind( "setportalst" ) ) )
      goto iohttpFunc_moderatorL0;
    iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( sscanf( str0, "%d", &i0 ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( dbMapRetrievePlanet( actionid, &planetd ) < 0 )
      goto iohttpFunc_moderatorL0;
    if( planetd.owner == -1 )
      goto iohttpFunc_moderatorL0;
    planetd.flags = planetd.flags & ( 0xFFFFFFFF - CMD_PLANET_FLAGS_PORTAL - CMD_PLANET_FLAGS_PORTAL_BUILD );
    if( i0 )
      planetd.flags |= CMD_PLANET_FLAGS_PORTAL;
    dbMapSetPlanet( actionid, &planetd );
    dbUserPlanetSetFlags( planetd.owner, actionid, planetd.flags );
    svSendPrintf( cnt, "Planet %d portal set to %d", actionid, i0 );

    fprintf( file, "%s >planet %d portal set to %d \n",main2d.faction, actionid, i0);
  }

  if( ( actionstring = iohttpVarsFind( "setbuilding" ) ) )
  {
    if( !( str0 = iohttpVarsFind( "setbuildingid" ) ) )
      goto iohttpFunc_moderatorL0;
    if( !( str1 = iohttpVarsFind( "setbuildingqt" ) ) )
      goto iohttpFunc_moderatorL0;
    iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( sscanf( str0, "%d", &i0 ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( sscanf( str1, "%lld", &i1 ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( (unsigned int)i0 >= CMD_BLDG_NUMUSED )
      goto iohttpFunc_moderatorL0;
    if( dbMapRetrievePlanet( actionid, &planetd ) < 0 )
      goto iohttpFunc_moderatorL0;
    planetd.building[i0] = i1;
    dbMapSetPlanet( actionid, &planetd );
    svSendPrintf( cnt, "Number of %s on %d set to %lld", cmdBuildingName[i0], actionid, i1 );
    fprintf( file, "%s >Number of building %s on %d set to %lld \n",main2d.faction, cmdBuildingName[i0], actionid, i1);
  }

  if( ( actionstring = iohttpVarsFind( "givepop" ) ) )
  {
    iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    num = dbUserPlanetListIndices( actionid, &plist );
    for( a = 0 ; a < num ; a++ )
    {
      dbMapRetrievePlanet( plist[a], &planetd );
      planetd.population = planetd.maxpopulation;
      dbMapSetPlanet( plist[a], &planetd );
    }
    if( plist )
      free( plist );
    svSendPrintf( cnt, "Gave population to %d", actionid );
    fprintf( file, "%s >Gave population to %s \n",main2d.faction, maind.faction);
  }


  if( ( actionstring = iohttpVarsFind( "clearfam" ) ) )
  {
    if( !( str0 = iohttpVarsFind( "players" ) ) )
      goto iohttpFunc_moderatorL0;
	iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( sscanf( str0, "%d", &i0 ) != 1 )
      goto iohttpFunc_moderatorL0;
    dbMapRetrieveEmpire( actionid, &empired );
    empired.numplayers = i0;
    dbMapSetEmpire( actionid, &empired );
    svSendPrintf( cnt, "fam %d cleared", actionid );
  }

  if( ( actionstring = iohttpVarsFind( "changestatus" ) ) )
  {
    if( !( str0 = iohttpVarsFind( "statusid" ) ) )
      goto iohttpFunc_moderatorL0;
	iohttpVarsCut();
    if( sscanf( actionstring, "%d", &actionid ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( sscanf( str0, "%d", &i0 ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( ( user = dbUserLinkID( actionid ) ) )
    {
		if( i0 == 1 )
		user->flags = CMD_USER_FLAGS_ACTIVATED;
		if( i0 == 2 )
        user->flags = CMD_USER_FLAGS_KILLED;
		if( i0 == 3 )
        user->flags = CMD_USER_FLAGS_DELETED;
		if( i0 == 4 )
        user->flags = CMD_USER_FLAGS_NEWROUND;
		dbUserSave( actionid, user );
    }
    svSendPrintf( cnt, "changed status of %d", actionid );
  }
  /*
  if( ( actionstring = iohttpVarsFind( "forumlid" ) ) )
  {
  	if( !( str0 = iohttpVarsFind( "threadlid" ) ) )
    {  
     	svSendString(cnt, "No thread specified<br>");
      goto iohttpFunc_moderatorL0;
    }
		iohttpVarsCut();
		
		//b = forum id 
		//c = thread id
		if( sscanf( actionstring, "%d", &b ) != 1 )
      goto iohttpFunc_moderatorL0;
    if( sscanf( str0, "%d", &c ) != 1 )
      goto iohttpFunc_moderatorL0;
		
		a = dbForumListThreads(b, c, c+1, &forumd, &pThread);
		if(a)
		{
			printf("Thread %d locked\n", c);
			pThread[0].flags = DB_FORUM_FLAGS_THREAD_LOCK;
			//Remove the thread to add it locked mode
			dbForumRemoveThread( b, c );
			dbForumAddThread( b, pThread);
		}
		
  }
  
  if( ( actionstring = iohttpVarsFind( "forumuid" ) ) )
  {
  	if( !( str0 = iohttpVarsFind( "threaduid" ) ) )
    {  
     	svSendString(cnt, "No thread specified<br>");
      goto iohttpFunc_moderatorL0;
    }
		iohttpVarsCut();
		
		
		
  }
	*/

  fclose( file );

  iohttpFunc_moderatorL0:
  svSendString( cnt, "Command refused<br>" );
  iohttpFunc_moderatorL1:
  svSendString( cnt, "</td></tr></table>" );
  svSendString( cnt, "</center></body></html>" );
  return;
}








void InitHTTP();
void EndHTTP();

void iohttpFunc_admin( svConnectionPtr cnt )
{
  int a, b, c, cmd[2], id;
  int *buffer;
  unsigned char *action[33];
  unsigned char fname[200];
  unsigned char curdir[1024];
  dbForumForumDef forumd;
  dbUserFleetDef fleetd;
  dbUserMainDef maind;
  dbMainSystemDef systemd;
  dbMainPlanetDef planetd;
  dbUserPtr user;
  dbUserDescDef descd;
  ioInterfacePtr io;
  int curtime;
	FILE *fFile;
	
  iohttpBase( cnt, 0 );

  if( ( id = iohttpIdentify( cnt, 0 ) ) < 0 )
    goto iohttpFunc_admin_mainL0;
  if( (cnt->dbuser)->level < LEVEL_ADMINISTRATOR )
    goto iohttpFunc_admin_mainL0;

  iohttpVarsInit( cnt );
  action[0] = iohttpVarsFind( "reloadfiles" );
  action[1] = iohttpVarsFind( "forums" );
  action[2] = iohttpVarsFind( "shutdown" );
  action[3] = iohttpVarsFind( "forumdel" );
  action[4] = iohttpVarsFind( "forumcreate" );
  action[5] = iohttpVarsFind( "setmod" );
  action[6] = iohttpVarsFind( "delplayer" );
  action[7] = iohttpVarsFind( "deactivate" );
  action[8] = iohttpVarsFind( "toggletime" );
  action[9] = iohttpVarsFind( "giveenergy" );
  action[10] = iohttpVarsFind( "crap" );
  action[11] = iohttpVarsFind( "inactives" );
  action[12] = iohttpVarsFind( "newround" );
  action[13] = iohttpVarsFind( "resettags" );
  action[14] = iohttpVarsFind( "changename" );
  action[15] = iohttpVarsFind( "newname" );
  action[16] = iohttpVarsFind( "unexplored" );
  action[17] = iohttpVarsFind( "clearnews" );
  action[18] = iohttpVarsFind( "findartefacts" );
  action[19] = iohttpVarsFind( "resetpsychics" );
  action[20] = iohttpVarsFind( "fixtags" );
  action[21] = iohttpVarsFind( "findbonuses" );
  action[22] = iohttpVarsFind( "genranks" );
  action[23] = iohttpVarsFind( "descs" );
  action[24] = iohttpVarsFind( "newround2" );
  action[25] = iohttpVarsFind( "forumdelauthor" );
  action[26] = iohttpVarsFind( "forumdelip" );
  action[27] = iohttpVarsFind( "setfmod" );
  action[28] = iohttpVarsFind( "setplay" );
  action[29] = iohttpVarsFind("deletecons");
  action[30] = iohttpVarsFind( "EndOfRound" );
  action[31] = iohttpVarsFind( "systemcmd" );
  iohttpVarsCut();

  if( action[0] )
  {
    if( getcwd( curdir, 1024 ) )
    {
      printf( "Admin : Reloading files\n" );
      EndHTTP();
      InitHTTP();
      chdir( curdir );
    }
    svSendString( cnt, "<i>HTTP files reloaded</i><br><br>" );
  }

  if( action[1] )
  {
    forumd.threads = 0;
    forumd.time = 0;
    forumd.tick = 0;
    forumd.flags = 0;

/*

    sprintf( forumd.title, "News" );
    forumd.rperms = 0xFFF;
    forumd.wperms = 0;
    dbForumAddForum( &forumd, 0, 0 );
    
    sprintf( forumd.title, "Ideas" );
    forumd.rperms = 0xFFF;
    forumd.wperms = 0xFFF;
    dbForumAddForum( &forumd, 0, 0 );

    sprintf( forumd.title, "Bugs" );
    forumd.rperms = 0xFFF;
    forumd.wperms = 0xFFF;
    dbForumAddForum( &forumd, 0, 0 );

    sprintf( forumd.title, "General" );
    forumd.rperms = 0xFFF;
    forumd.wperms = 0xFFF;
    dbForumAddForum( &forumd, 0, 0 );

    sprintf( forumd.title, "Questions" );
    forumd.rperms = 0xFFF;
    forumd.wperms = 0xFFF;
    dbForumAddForum( &forumd, 0, 0 );

*/

    for( a = 0 ; a < dbMapBInfoStatic[4] ; a++ )
    {
      sprintf( forumd.title, "Empire %d forum", a );
      forumd.rperms = 2;
      forumd.wperms = 2;
      forumd.flags = DB_FORUM_FLAGS_FORUMFAMILY;
      dbForumAddForum( &forumd, 1, 100+a );
    }

    svSendString( cnt, "<i>Forums created</i><br><br>" );
  }

  if( action[2] )
  {
    printf( "Admin: Shutting down!\n" );
    cmdExecuteFlush();
    dbFlush();

    cmdEnd();
    dbEnd();
    for( a = 0 ; a < IO_INTERFACE_NUM ; a++ )
    {
      io = &ioInterface[a];
      io->End();
    }
    svEnd();


//    zmalloc_check();
    fflush( stdout );

    exit( 1 );
  }

  if( action[3] )
  {
    if( sscanf( action[3], "%d", &a ) == 1 )
    {
      if( dbForumRemoveForum( a ) >= 0 )
        svSendString( cnt, "<i>Forum deleted</i><br><br>" );
    }
  }

  if( action[4] )
  {
    forumd.threads = 0;
    forumd.time = 0;
    forumd.tick = 0;
    forumd.flags = 0;
    iohttpForumFilter( forumd.title, action[4], 64, 0 );
    forumd.rperms = 0xFFF;
    forumd.wperms = 0xFFF;
    if( dbForumAddForum( &forumd, 0, 0 ) >= 0 )
      svSendPrintf( cnt, "Public forum %s added<br><br>", action[4] );
  }

  if( action[5] )
  {
    if( sscanf( action[5], "%d", &a ) == 1 )
    {
      dbUserMainRetrieve( a, &maind );
      sprintf( maind.forumtag, "Moderator" );
      if( ( user = dbUserLinkID( a ) ) )
      {
        user->level = LEVEL_MODERATOR;
        dbUserSave( a, user );
      }
      dbUserMainSet( a, &maind );
      svSendPrintf( cnt, "Player %d set to moderator<br><br>", a );
    }
  }

  if( action[27] )
  {
    if( sscanf( action[27], "%d", &a ) == 1 )
    {
      dbUserMainRetrieve( a, &maind );
      sprintf( maind.forumtag, "Forum Moderator" );
      if( ( user = dbUserLinkID( a ) ) )
      {
        user->level = LEVEL_FORUMMOD;
        dbUserSave( a, user );
      }
      dbUserMainSet( a, &maind );
      svSendPrintf( cnt, "Player %d set to forum moderator<br><br>", a );
    }
  }

  if( action[28] )
  {
    if( sscanf( action[28], "%d", &a ) == 1 )
    {
      dbUserMainRetrieve( a, &maind );
      sprintf( maind.forumtag, "Player" );
      if( ( user = dbUserLinkID( a ) ) )
      {
        user->level = LEVEL_USER;
        dbUserSave( a, user );
      }
      dbUserMainSet( a, &maind );
      svSendPrintf( cnt, "Player %d set to player<br><br>", a );
    }
  }
  
  //Remove all building in construction on that plnid
	if( action[29] )
  {
  	if(sscanf(action[29], "%d", &a) == 1)
  	{
  		if(dbMapRetrievePlanet( a, &planetd )>0)
  		{
  			planetd.construction = 0;
        dbMapSetPlanet( a, &planetd );
  		}
  	}
  }
  
  if( action[6] )
  {
    if( sscanf( action[6], "%d", &a ) == 1 )
    {
      cmd[0] = CMD_CHANGE_KILLUSER;
      cmd[1] = a;
      if( cmdExecute( cnt, cmd, 0, 0 ) >= 0 )
        svSendPrintf( cnt, "Player %d deleted<br><br>", a );
    }
  }

  if( action[7] )
  {
    if( sscanf( action[7], "%d", &a ) == 1 )
    {
      cmdExecUserDeactivate( a, CMD_USER_FLAGS_DELETED );
      svSendPrintf( cnt, "Player %d deactivated<br><br>", a );
    }
  }

  if( action[8] )
  {
    svTickStatus = !svTickStatus;
    svSendPrintf( cnt, "Time flow status : %d<br><br>", svTickStatus );
  }

  if( action[9] )
  {
    if( sscanf( action[9], "%d", &a ) == 1 )
    {
      dbUserMainRetrieve( a, &maind );
      maind.ressource[CMD_RESSOURCE_ENERGY] += (long long int)(200000);
      dbUserMainSet( a, &maind );
      svSendPrintf( cnt, "User %d now got %lld energy<br><br>", a, maind.ressource[CMD_RESSOURCE_ENERGY] );
    }
  }

  if( action[10] )
  {
    for( user = dbUserList ; user ; user = user->next )
    {
      dbUserMainRetrieve( user->id, &maind );
      b = dbUserMarketList( user->id, &buffer );
      for( a = c = 0 ; a < b ; a++, c += 5 )
      {

      if( !( buffer[c+DB_MARKETBID_ACTION] ) )
        svSendString( cnt, "Buying" );
      else
        svSendString( cnt, "Selling" );
      svSendPrintf( cnt, " %d %s at %d - <a href=\"market?rmbid=%d\">Withdraw bid</a><br>", buffer[c+DB_MARKETBID_QUANTITY], cmdRessourceName[buffer[c+DB_MARKETBID_RESSOURCE]+1], buffer[c+DB_MARKETBID_PRICE], buffer[c+DB_MARKETBID_BIDID] );


        if( !( buffer[c+DB_MARKETBID_ACTION] ) )
          maind.ressource[CMD_RESSOURCE_ENERGY] += (long long int)(buffer[c+DB_MARKETBID_QUANTITY] * buffer[c+DB_MARKETBID_PRICE]);
        else
          maind.ressource[buffer[c+DB_MARKETBID_RESSOURCE]+1] += (long long int)(buffer[c+DB_MARKETBID_QUANTITY]);
      }
      if( buffer )
        free( buffer );
      dbUserMainSet( user->id, &maind );
      dbUserMarketReset( user->id );
    }
    dbMarketReset();
    svSendPrintf( cnt, "market crap<br><br>" );
  }

  if( action[11] )
  {
  	//This function should change
  	//it should be played at least 1 of the 3 last round
  	//if not it should be called inactive
  	//the lasttime value is NOT a valid thing
    curtime = time( 0 );
    for( user = dbUserList ; user ; user = user->next )
    {
      if( dbUserMainRetrieve( user->id, &maind ) < 0 )
        continue;
      //after 3 day + one day per tagpoints? you are inactive
      //a = 3*24*60*60 + 24*60*60*(int)sqrt( (double)(maind.tagpoints) );
      a = 120*24*60*60; //90 days
      printf("curr = %d, last = %d ", curtime, user->lasttime);
      if( curtime - user->lasttime < a )
        continue;
      if( user->level >= LEVEL_MODERATOR )
        continue;
      printf("removing id %d\n", user->id);
      cmdUserDelete( user->id );
    }
    svSendPrintf( cnt, "All inactives removed<br><br>" );
  }

  if( action[12] )
  {
  	curtime = time( 0 );
    for( user = dbUserList ; user ; user = user->next )
      cmdExecUserDeactivate( user->id, CMD_USER_FLAGS_NEWROUND );
    svSendPrintf( cnt, "All accounts deactivated<br><br>" );
    svRoundEnd = 0;
		if( ( fFile = fopen( SV_TICK_FILE, "r+" ) ) )
	  {
	   fscanf( fFile, "%d", &a );
	   /* do
	    {
	    	a = fprintf(fFile, " %d", svRoundEnd);
	    }while(!a);*/
	    fclose( fFile );
	  }
  }

  if( action[13] )
  {
    for( user = dbUserList ; user ; user = user->next )
    {
      if( !( dbUserMainRetrieve( user->id, &maind ) ) )
        continue;
      if( user->level == LEVEL_USER )
        sprintf( maind.forumtag, "Player" );
      else if( user->level == LEVEL_MODERATOR )
        sprintf( maind.forumtag, "Moderator" );
      else
        sprintf( maind.forumtag, "Administrator" );
      dbUserMainSet( user->id, &maind );
    }
    svSendPrintf( cnt, "All forum tags reseted<br><br>" );
  }

  if( action[14] )
  {
    if( sscanf( action[14], "%d", &a ) == 1 )
    {
      if( !( dbUserMainRetrieve( a, &maind ) ) )
        return;
      if( !( action[15] ) )
        return;
      iohttpForumFilter( maind.faction, action[15], 32, 0 );
      sprintf( maind.forumtag, "Player" );
      dbUserMainSet( a, &maind );
      svSendPrintf( cnt, "Player %d name changed for %s<br><br>", a, maind.faction );
    }
  }

  if( action[16] )
  {
    for( a = 0 ; a < dbMapBInfoStatic[2] ; a++ )
    {
      dbMapRetrieveSystem( a, &systemd );
      systemd.unexplored = 0;
      for( b = 0 ; b < systemd.numplanets ; b++ )
      {
        dbMapRetrievePlanet( systemd.indexplanet + b, &planetd );
        if( planetd.owner == -1 )
          systemd.unexplored++;
      }
      dbMapSetSystem( a, &systemd );
    }
  }

  if( action[17] )
  {
    for( user = dbUserList ; user ; user = user->next )
      dbUserNewsEmpty( user->id );
    svSendPrintf( cnt, "Cleared news<br><br>" );
  }

  if( action[18] )
  {
    for( a = 0 ; a < dbMapBInfoStatic[3] ; a++ )
    {
      dbMapRetrievePlanet( a, &planetd );
      if( ( b = (int)artefactPrecense( &planetd ) ) < 0 )
        continue;
      svSendPrintf( cnt, "Artefact %d : %d,%d:%d ( %d )<br>", b, ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, planetd.position & 0xFF, a );
    }
    svSendPrintf( cnt, "<br>" );
  }

  if( action[19] )
  {
    if( sscanf( action[19], "%d", &a ) == 1 )
    {
      if( !( dbUserFleetRetrieve( a, 0, &fleetd ) ) )
        return;
      fleetd.unit[CMD_UNIT_WIZARD] = 1000;
      dbUserFleetSet( a, 0, &fleetd );
      svSendPrintf( cnt, "User %d now reset psychics to 1000.<br><br>", a );
    }
  }

  if( action[20] )
  {
    for( user = dbUserList ; user ; user = user->next )
    {
      if( user->level != LEVEL_USER )
        continue;
      dbUserMainRetrieve( user->id, &maind );
      sprintf( maind.forumtag, cmdTagFind( maind.tagpoints ) );
      dbUserMainSet( user->id, &maind );
    }
    dbMarketReset();
    svSendPrintf( cnt, "Tags set<br><br>" );
  }

  if( action[21] )
  {
    for( a = b = 0 ; a < dbMapBInfoStatic[3] ; a++ )
    {
      dbMapRetrievePlanet( a, &planetd );
      if( !( planetd.special[1] ) )
        continue;
      svSendPrintf( cnt, "Bonus +%d%% R%d : %d,%d:%d ( %d )<br>", planetd.special[1], planetd.special[0], ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, planetd.position & 0xFF, a );
      b++;
    }
    svSendPrintf( cnt, "Total : %d planets<br><br>", b );
  }

  if( action[22] )
  {
//    iohttpGenRanks();
    svSendPrintf( cnt, "Ranks generated<br><br>" );
  }

  if( action[23] )
  {
    for( user = dbUserList ; user ; user = user->next )
    {
      dbUserDescRetrieve( user->id, &descd );
      for( a = 0 ; ( a < 4096 ) && ( descd.desc[a] ) ; a++ )
      {
        if( descd.desc[a] >= 128 )
        {
          descd.desc[0] = 0;
          dbUserDescSet( user->id, &descd );
          break;
        }
      }
    }
    svSendPrintf( cnt, "Descs removed<br><br>" );
  }

  if( action[24] )
  {
    curtime = time( 0 );
    for( user = dbUserList ; user ; user = user->next )
      cmdExecUserDeactivate( user->id, 0 );
    svSendPrintf( cnt, "All accounts deactivated<br><br>" );
    svRoundEnd = 0;
		if( ( fFile = fopen( SV_TICK_FILE, "r+" ) ) )
	  {
	   fscanf( fFile, "%d", &a );
	   /* do
	    {
	    	a = fprintf(fFile, " %d", svRoundEnd);
	    }while(!a);*/
	    fclose( fFile );
	  }
  }

  if( action[25] )
  {
    a = iohttpForumCleanAuthor( action[25] );
    svSendPrintf( cnt, "Deleted author %s ; %d threads<br><br>", action[25], a );
  }

  if( action[26] )
  {
    a = iohttpForumCleanIP( action[26] );
    svSendPrintf( cnt, "Deleted ip %s ; %d threads<br><br>", action[26], a );
  }

	//Round is done we send a signal to the server so he know it (its a signal so we can do a script with that)
	if( action[30] )
  {
  	//Signal to round end
  	system("kill -n 10 $(pidof svt)");
  }
	
	//Send some text to OS via system command
	if( action[31] )
  {
		iohttpForumFilter( fname, action[31], 200, 0 );
		system(fname);
  }


  svSendString( cnt, "Administrator interface under construction<br><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"hidden\" name=\"reloadfiles\" value=\"1\"><input type=\"submit\" value=\"Reload HTTP files\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"hidden\" name=\"forums\" value=\"1\"><input type=\"submit\" value=\"Create empire forums\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"hidden\" name=\"shutdown\" value=\"1\"><input type=\"submit\" value=\"Shutdown\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"hidden\" name=\"EndOfRound\" value=\"1\"><input type=\"submit\" value=\"End Of Round\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"text\" name=\"forumdel\" value=\"user ID\"><input type=\"submit\" value=\"Delete forum\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"text\" name=\"forumcreate\" value=\"Forum name\"><input type=\"submit\" value=\"Create forum\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"text\" name=\"setmod\" value=\"user ID\"><input type=\"submit\" value=\"Set mod\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"text\" name=\"setfmod\" value=\"user ID\"><input type=\"submit\" value=\"Set fmod\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"text\" name=\"setplay\" value=\"user ID\"><input type=\"submit\" value=\"Set player\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"text\" name=\"delplayer\" value=\"user ID\"><input type=\"submit\" value=\"Delete player\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"text\" name=\"deactivate\" value=\"user ID\"><input type=\"submit\" value=\"Deactivate user\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"hidden\" name=\"toggletime\" value=\"1\"><input type=\"submit\" value=\"Toggle time flow\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"text\" name=\"giveenergy\" value=\"user ID\"><input type=\"submit\" value=\"Give energy\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"text\" name=\"deletecons\" value=\"Planet ID\"><input type=\"submit\" value=\"Clear construction\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"hidden\" name=\"crap\" value=\"1\"><input type=\"submit\" value=\"Clear market\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"hidden\" name=\"inactives\" value=\"1\"><input type=\"submit\" value=\"Delete inactives\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"hidden\" name=\"newround\" value=\"1\"><input type=\"submit\" value=\"Deactivate all for new round\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"hidden\" name=\"resettags\" value=\"1\"><input type=\"submit\" value=\"Reset tags\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"text\" name=\"changename\" value=\"ID\"><input type=\"text\" name=\"newname\" value=\"New name\"><input type=\"submit\" value=\"Change name\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"hidden\" name=\"unexplored\" value=\"1\"><input type=\"submit\" value=\"Recount unexplored planets\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"hidden\" name=\"clearnews\" value=\"1\"><input type=\"submit\" value=\"Clear all news\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"hidden\" name=\"findartefacts\" value=\"1\"><input type=\"submit\" value=\"Find artefacts\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"text\" name=\"resetpsychics\" value=\"ID\"><input type=\"submit\" value=\"Reset psychics\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"hidden\" name=\"fixtags\" value=\"1\"><input type=\"submit\" value=\"Fix tags\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"hidden\" name=\"findbonuses\" value=\"1\"><input type=\"submit\" value=\"Find bonuses\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"hidden\" name=\"genranks\" value=\"1\"><input type=\"submit\" value=\"Gen ranks\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"hidden\" name=\"descs\" value=\"1\"><input type=\"submit\" value=\"Fix descs\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"hidden\" name=\"newround2\" value=\"1\"><input type=\"submit\" value=\"Deactivate all for new round, no records\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"text\" name=\"forumdelauthor\" value=\"string\"><input type=\"submit\" value=\"Delete forum author\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"text\" name=\"forumdelip\" value=\"IP\"><input type=\"submit\" value=\"Delete forum ip\"></form><br><br>" );
  svSendString( cnt, "<form action=\"admin\" method=\"POST\"><input type=\"text\" name=\"systemcmd\" value=\"Command\"><input type=\"submit\" value=\"Send a system CMD\"></form><br><br>" );

  svSendString( cnt, "</center></body></html>" );
  return;

  iohttpFunc_admin_mainL0:
  svSendString( cnt, "You do not have administrator privileges." );
  svSendString( cnt, "</center></body></html>" );
  svSendString( cnt, "</center></body></html>" );
  return;
}



