int iohttpIdentifyHex( unsigned char *num )
{
 int a, b, c;
 for( a = b = 0 ; ; b <<= 4 )
 {
  if( ( num[a] >= '0' ) && ( num[a] <= '9' ) )
   b += c = num[a] - '0';
  else if( ( num[a] >= 'a' ) && ( num[a] <= 'z' ) )
   b += c = num[a] - ('a'-10);
  else if( ( num[a] >= 'A' ) && ( num[a] <= 'Z' ) )
   b += c = num[a] - ('A'-10);
  if( ++a == 4 )
   break;
 }
 return b;
}

void iohttpBase( svConnectionPtr cnt, int flags );

int iohttpIdentify( svConnectionPtr cnt, int action )
{
 int a, b, id, session[4];
 unsigned char *src;
 FILE *file;
 struct stat stdata;
 unsigned char *data;
 iohttpDataPtr iohttp = cnt->iodata;

 if( !( src = iohttp->cookie ) )
  goto iohttpIdentifyL0;
 if( !( src = ioCompareFindWords( src, "USRID=" ) ) )
  goto iohttpIdentifyL0;

 id = iohttpIdentifyHex( &src[0] );
 if( dbUserLinkDatabase( cnt, id ) < 0 )
  goto iohttpIdentifyL0;
 if( dbSessionRetrieve( cnt->dbuser, session ) < 0 )
  goto iohttpIdentifyL0;
 for( a = 0 ; a < 4 ; a++ )
 {
  b = iohttpIdentifyHex( &src[4+(a<<2)] );
  if( session[a] != b )
   goto iohttpIdentifyL0;
 }

 if(( action & 2 )&&(cnt->dbuser))
 {
  if( !( (cnt->dbuser)->flags & CMD_USER_FLAGS_ACTIVATED ) && ( (cnt->dbuser)->level < LEVEL_MODERATOR ) )
  {
   if( action & 1 )
   {
    if( action & 8 )
     iohttpBase( cnt, 1|2 );
    svSendString( cnt, "This account has not been activated yet.</body></html>" );
   }
   return -1;
  }
 }
 if( action & 4 )
 {
  if( (cnt->dbuser)->flags & CMD_USER_FLAGS_ACTIVATED )
  {
   if( action & 1 )
   {
    if( action & 8 )
     iohttpBase( cnt, 1|2 );
    svSendString( cnt, "This account has been activated.</body></html>" );
   }
   return -1;
  }
 }

 return id;

 iohttpIdentifyL0:
 if( action & 1 )
 {
  if( action & 8 )
   iohttpBase( cnt, 1|2 );
  if( stat( IOHTTP_READ_DIRECTORY "/login.html", &stdata ) != -1 )
  {
   if( ( data = malloc( stdata.st_size + 1 ) ) )
   {
    data[stdata.st_size] = 0;
    if( ( file = fopen( IOHTTP_READ_DIRECTORY "/login.html", "rb" ) ) )
    {
     fread( data, 1, stdata.st_size, file );
     svSendString( cnt, data );
     fclose( file );
    }
    free( data );
   }
  }
  else
   svSendString( cnt, "<br><br>You are not logged in!<br><a href=\"login\" target=\"_top\">Log in</a><br><br><br>If you were playing just a few seconds ago, the server program was probably updated and restarted.<br><br>Information about any important update can be found on the <a href=\"/\" target=\"_top\">front page</a> or the <a href=\"/forum?forum=0\">Announcements forum</a></body></html>" );
 }

 return -1;
}

void iohttpBase( svConnectionPtr cnt, int flags )
{
	
	svSendString( cnt, "Content-Type: text/html\n\n" );
 svSendString( cnt, "<html><head>");
 if( flags & 4 )
  svSendString( cnt, "<base target=\"_blank\">" );
 
 svSendString( cnt, "<style type=\"text/css\">body,td{font-size:smaller;font-family:verdana,geneva,arial,helvetica,sans-serif;}a:hover{color:#00aaaa}</style></head><body bgcolor=\"#000000\" text=\"#C0D0D8\" link=\"#FFFFFF\" alink=\"#FFFFFF\" vlink=\"#FFFFFF\"" );
 if( flags & 1 )
 {
  svSendString( cnt, " background=\"mbg.gif\"" );
  if( !( flags & 2 ) )
   svSendString( cnt, " bgproperties=\"fixed\"" );
 }
 svSendString( cnt, " marginheight=\"0\" topmargin=\"0\"><center>" );
 return;
}

int iohttpHeader( svConnectionPtr cnt, int id, dbUserMainPtr mainp )
{
 int a;
 if( dbUserMainRetrieve( id, mainp ) < 0 )
 {
  svSendString( cnt, "Error while retriving user's main data</center></body></html>" );
  return 0;
 }

 svSendString( cnt, "<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" border=\"0\" align=\"center\">" );

 svSendString( cnt, "<tr>" );
 svSendString( cnt, "<td width=\"35\" rowspan=\"2\"><img height=\"63\" src=\"i02.jpg\" width=\"35\"></td>" );
 svSendString( cnt, "<td width=\"100%\" height=\"21\" align=\"center\">" );

 svSendString( cnt, "<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" background=\"i05.jpg\" border=\"0\" align=\"center\">" );
 svSendString( cnt, "<tr>" );
 svSendString( cnt, "<td width=\"41\"><img height=\"21\" src=\"i03.jpg\" width=\"41\"></td>" );
 svSendString( cnt, "<td background=\"i05.jpg\">" );

 if( svTickStatus )
 svSendPrintf( cnt, "<table width=\"100%%\" cellspacing=\"0\" cellpadding=\"0\"><tr><td width=\"30%%\" align=\"center\"><font size=\"1\"><b>Networth : %lld</b></font></td><td width=\"40%%\" align=\"center\"><font size=\"1\"><b>Tick time : %d seconds left</b></font></td><td width=\"30%%\" align=\"center\"><font size=\"1\"><b>Population : %lld0</b></font></td></tr></table>", mainp->networth, (int)( svTickTime - time(0) ), mainp->ressource[CMD_RESSOURCE_POPULATION] );
 else
 svSendPrintf( cnt, "<table width=\"100%%\" cellspacing=\"0\" cellpadding=\"0\"><tr><td width=\"30%%\" align=\"center\"><font size=\"1\"><b>Networth : %lld</b></font></td><td width=\"40%%\" align=\"center\"><font size=\"1\"><b>Tick time : time frozen</b></font></td><td width=\"30%%\" align=\"center\"><font size=\"1\"><b>Population : %lld0</b></font></td></tr></table>", mainp->networth, mainp->ressource[CMD_RESSOURCE_POPULATION] );

 svSendString( cnt, "</td><td width=\"78\"><img height=\"21\" src=\"i07.jpg\" width=\"78\"></td></tr></table>" );
 svSendString( cnt, "</td></tr>" );

 svSendString( cnt, "<tr><td width=\"100%\" align=\"center\">" );
 svSendString( cnt, "<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" background=\"i15.jpg\" border=\"0\" align=\"center\"><tr>" );

 a = dbUserNewsGetFlags( id );
 svSendString( cnt, "<td width=\"41\"><a href=\"hq\"><img height=\"42\" title=\"mail\" src=\"i09" );
 if( a & CMD_NEWS_FLAGS_MAIL )
  svSendString( cnt, "a" );
 svSendString( cnt, ".jpg\" width=\"41\" border=\"0\"></a></td>" );

 svSendString( cnt, "<td width=\"40\"><a href=\"hq\"><img height=\"42\" title=\"reports\" src=\"i10" );
 if( a & CMD_NEWS_FLAGS_BUILD )
  svSendString( cnt, "a" );
 svSendString( cnt, ".jpg\" width=\"40\" border=\"0\"></a></td>" );

 svSendString( cnt, "<td width=\"39\"><a href=\"hq\"><img height=\"42\" title=\"economy\" src=\"i11" );
 if( a & CMD_NEWS_FLAGS_AID )
  svSendString( cnt, "a" );
 svSendString( cnt, ".jpg\" width=\"39\" border=\"0\"></a></td>" );

 svSendString( cnt, "<td width=\"39\"><a href=\"hq\"><img height=\"42\" title=\"fleets\" src=\"i12" );
 if( a & CMD_NEWS_FLAGS_ATTACK )
  svSendString( cnt, "a" );
 else if( a & CMD_NEWS_FLAGS_FLEET )
  svSendString( cnt, "b" );
 svSendString( cnt, ".jpg\" width=\"39\" border=\"0\"></a></td>" );

 svSendString( cnt, "<td width=\"18\"><img height=\"42\" src=\"i13.jpg\" width=\"18\"></td>" );
 svSendString( cnt, "<td width=\"100%\" background=\"i15.jpg\" align=\"left\">" );

 svSendString( cnt, "<table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\" border=\"0\"><tr>" );
 svSendString( cnt, "<td width=\"50%\" align=\"center\" nowrap><font size=\"1\"><b>" );
 svSendPrintf( cnt, "Energy: %lld<br>Mineral: %lld</b></font></td><td width=\"50%%\" align=\"center\" nowrap><font size=\"1\"><b>Crystal: %lld<br>Ectrolium: %lld</b></font>", mainp->ressource[0], mainp->ressource[1], mainp->ressource[2], mainp->ressource[3] );
 svSendString( cnt, "</td></tr></table>" );

 svSendString( cnt, "</td><td width=\"49\"><img height=\"42\" src=\"i17.jpg\" width=\"49\"></td></tr></table>" );

 svSendString( cnt, "</td></tr></table><br>" );

 return 1;
}


void iohttpBodyInit( svConnectionPtr cnt, unsigned char *title, ... )
{
 char text[4096];
 va_list ap;
 va_start( ap, title );
 vsnprintf( text, 4096, title, ap );
 svSendString( cnt, "<table cellspacing=\"0\" cellpadding=\"0\" width=\"90%\" border=\"0\" align=\"center\" background=\"i27.jpg\"><tr><td width=\"10%\"><img height=\"24\" src=\"i25.jpg\" width=\"22\"></td><td width=\"80%\" align=\"center\" nowrap><b><font face=\"verdana\" size=\"2\">" );
 svSendString( cnt, text );
 svSendString( cnt, "</font></b></td><td width=\"10%\" align=\"right\"><img height=\"24\" src=\"i30.jpg\" width=\"62\"></td></tr></table>" );
 svSendString( cnt, "<table cellspacing=\"0\" cellpadding=\"0\" width=\"90%\" border=\"0\" align=\"center\"><tr><td width=\"7\" background=\"i38.jpg\">&nbsp;</td><td bgcolor=\"#0b1119\" align=\"center\"><br>" );
 return;
}

void iohttpBodyEnd( svConnectionPtr cnt )
{
 svSendString( cnt, "<br></td><td width=\"7\" background=\"i43.jpg\">&nbsp;</td></tr></table><table cellspacing=\"0\" cellpadding=\"0\" width=\"90%\" border=\"0\" align=\"center\"><tr><td width=\"62\"><img height=\"12\" src=\"i45.jpg\" width=\"62\"></td><td width=\"100%\" background=\"i47.jpg\"><img height=\"12\" src=\"i47.jpg\" width=\"1\"></td><td width=\"62\"><img height=\"12\" src=\"i49.jpg\" width=\"62\"></td></tr></table></center><br><br><br></body></html>" );
 return;
}



void iohttpFunc_races( svConnectionPtr cnt )
{
 iohttpBase( cnt, 0 );
 svSendString( cnt, "<br><b>Ectroverse races</b><br><br>" );

 svSendString( cnt, "<h3>Harks</h3><table width=\"620\" border=\"0\"><tr>" );
 svSendString( cnt, "<td valign=\"top\" width=\"340\"><i>Main bonuses</i><br>-10% Energy production<br>+20% Research production <br>-20% Population upkeep reduction<br>+40% Attack strength<br>-10% Defence strength<br>+40% Travel speed<br><br>" );
 svSendString( cnt, "<i>Special</i><br>Fighters units 20% stronger<br>Higher crystal production by 25%<br>Military Research production limit of 250%<br>Halved culture Research production </td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Operations</i><br>Network virus<br>Infiltration<br>Bio infection<br>Military sabotage<br>Nuke Planet<br><br><i>Spells</i><br>Irradiate Ectrolium<br>Incandescence<br>Black Mist<br>War Illusions</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Incantations</i><br>Portal Force Field<br>Vortex Portal<br>Energy Surge</td>" );
 svSendString( cnt, "</tr></table><br><br>" );

 svSendString( cnt, "<h3>Manticarias</h3><table width=\"620\" border=\"0\"><tr>" );
 svSendString( cnt, "<td valign=\"top\" width=\"340\"><i>Main bonuses</i><br>+40% Energy production<br>-10% Research production<br>-10% Population upkeep reduction<br>+40% Psychics strength<br>+20% Ghost Ships strength<br>-30% Attack strength<br>+10% Defence strength<br><br>" );
 svSendString( cnt, "<i>Special</i><br>Higher solars collectors efficiency by 15%<br>Doubled culture Research production </td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Operations</i><br>Spy Target<br>Observe Planet<br>Energy Transfer<br><br><i>Spells</i><br>Dark Web<br>Black Mist<br>War Illusions<br>Psychic Assault<br>Phantoms</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Incantations</i><br>Planetary Shielding<br>Mind Control</td>" );
 svSendString( cnt, "</tr></table><br><br>" );

 svSendString( cnt, "<h3>Foohons</h3><table width=\"620\" border=\"0\"><tr>" );
 svSendString( cnt, "<td valign=\"top\" width=\"340\"><i>Main bonuses</i><br>-20% Energy production<br>+50% Research production<br>+10% Population upkeep reduction<br>+10% Ghost Ships strength<br>+20% Attack strength<br>+10% Defence strength<br><br>" );
 svSendString( cnt, "<i>Special</i><br>Each 4000 population produces 1 research point each week<br>Ectrolium production increased by 20%</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Operations</i><br>Spy Target<br>Observe Planet<br>Infiltration<br>Military Sabotage<br>High Infiltration<br>Planetary Beacon<br><br><i>Spells</i><br>Irradiate Ectrolium<br>Dark Web<br>Incandescence<br>Psychic Assault</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Incantations</i><br>Sense Artefact<br>Survey System<br>Vortex Portal<br>Mind Control</td>" );
 svSendString( cnt, "</tr></table><br><br>" );

 svSendString( cnt, "<h3>Spacebornes</h3><table width=\"620\" border=\"0\"><tr>" );
 svSendString( cnt, "<td valign=\"top\" width=\"340\"><i>Main bonuses</i><br>+30% Energy production<br>+10% Research production<br>+20% Population upkeep reduction<br>-30% Psychics strength<br>+30% Agents strengths<br>+20% Defence strength<br>+80% Travel speed<br><br>" );
 svSendString( cnt, "<i>Special</i><br>Soldiers and droids units 10% stronger<br>Energy production Research production limit of 250%<br>Halved technology Research production </td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Operations</i><br>Spy Target<br>Observe Planet<br>Network Virus<br>Bio Infection<br>Energy Transfer<br>Nuke Planet<br>Planetary Beacon<br><br><i>Spells</i><br>Irradiate Ectrolium<br>Incandescence<br>Black Mist</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Incantations</i><br>Survey System<br>Planetary Shielding<br></td>" );
 svSendString( cnt, "</tr></table><br><br>" );

 svSendString( cnt, "<h3>Dreamweavers</h3><table width=\"620\" border=\"0\"><tr>" );
 svSendString( cnt, "<td valign=\"top\" width=\"340\"><i>Main bonuses</i><br>-20% Energy production<br>+40% Research production<br>-10% Population upkeep reduction<br>+50% Psychics strength<br>+30% Ghost Ships strength<br>-30% Defence strength<br><br>" );
 svSendString( cnt, "<i>Special</i><br>Doubled technology Research production <br>Higher crystal production by 25%<br>Construction Research production limit of 250%<br>Halved military Research production </td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Operations</i><br>Network Virus<br>Bio Infection<br>Energy Transfer<br>Military Sabotage<br><br><i>Spells</i><br>Irradiate Ectrolium<br>Dark Web<br>Incandescence<br>Black Mist<br>War Illusions<br>Psychic Assault<br>Phantoms</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Incantations</i><br>Sense Artefact<br>Portal Force Field<br>Mind Control<br>Energy Surge</td>" );
 svSendString( cnt, "</tr></table><br><br>" );
/*	
	svSendString( cnt, "<h3>Furtifons</h3><table width=\"620\" border=\"0\"><tr>" );
 svSendString( cnt, "<td valign=\"top\" width=\"340\"><i>Main bonuses</i><br>-10% Attack strength<br>-10% Research production<br>-10% Population upkeep reduction<br>+20% Agents strengths<br>+20% Ghost Ships strength<br>+60% Travel speed<br><br>" );
 svSendString( cnt, "<i>Special</i><br>Culture Research production provides a psychic shield for planets<br>Double Operations research<br>Construction Research production limit of 250%</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Operations</i><br>Spy Target<br>Observe Planet<br>Energy Transfer<br>Infiltration<br>High Infiltration<br>Planetary Beacon<br><br><i>Spells</i><br>Incandescence<br>War Illusions</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Incantations</i><br>Sense Artefact<br>Survey System<br>Planetary Shielding<br>Portal Force Field<br>Vortex Portal</td>" );
	svSendString( cnt, "</tr></table><br><br>" );
	
	svSendString( cnt, "<h3>Samsonites</h3><table width=\"620\" border=\"0\"><tr>" );
 svSendString( cnt, "<td valign=\"top\" width=\"340\"><i>Main bonuses</i><br>+30% Attack strength<br>+20% Defence strength<br>+10% Energy production<br>+10% Research production<br>-10% Agents strength<br>-10% Psychics strength<br>-20% Travel speed<br><br>" );
 svSendString( cnt, "<i>Special</i><br>Ignore Dark Webs effects<br>185% Research production max</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Operations</i><br>Bio Infection<br>Military Sabotage<br>Nuke Planet<br><br><i>Spells</i><br>Black Mist<br>Phantoms</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Incantations</i><br>Survey System<br>Mind Control<br>Energy Surge</td>" );
	svSendString( cnt, "</tr></table><br><br>" );

	svSendString( cnt, "<h3>Ultimums</h3><table width=\"620\" border=\"0\"><tr>" );
 svSendString( cnt, "<td valign=\"top\" width=\"340\"><i>Main bonuses</i><br>+70% stronger for all units<br><br>" );
 svSendString( cnt, "<i>Special</i><br>Double Research production points production <br>All Research production maximum are 250%<br>Each 4000 population produces a Research production point<br>Higher solars collectors efficiency by 15%</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Operations</i><br>Spy Target<br>Observe Planet<br>Network Virus<br>Infiltration<br>Bio Infection<br>Energy Transfer<br>Military Sabotage<br><br>Nuke Planet<br>High Infiltration<br>Planetary Beacon<br><i>Spells</i><br>Irradiate Ectrolium<br>Dark Web<br>Incandescence<br>Black Mist<br>War Illusions<br>Psychic Assault<br>Phantoms</td>" );
 svSendString( cnt, "<td valign=\"top\" width=\"140\"><i>Incantations</i><br>Sense Artefact<br>Survey System<br>Planetary Shielding<br>Portal Force Field<br>Vortex Portal<br>Mind Control<br>Energy Surge</td>" );
 svSendString( cnt, "</tr></table><br><br>" );
*/
 svSendString( cnt, "<br><br><br>" );

/*
Psychic Assault - attemps to kill the psychics of another faction, causing psychics casualities on both sides.

Sense Artefact - Attempts to locate the nearest artefact, from a particular system
Survey System - Attempt to determine the size, resource bonus and the precense of portals for each planet of a solar system
Planetary Shielding - Create temporary shielding protecting defensive units for a specific planet
Portal Force Field - Create a force field around a specific planet, making portal travel very difficult, reducing the owner capabilities to protect it
Vortex Portal - Create a temporary portal in the targeted system from which fleets can attack and return to the main fleet
Mind Control - Take control of an enemy planet with all its infrastructure, most of the population is killed
Energy Surge - Spreads a destructive wave in an faction network, feeding on the faction's energy reserves ; destroying resource reserves, research and buildings
*/

 return;
}

void iohttpFunc_register( svConnectionPtr cnt )
{
 iohttpBase ( cnt, 0 );
  svSendString ( cnt, "<br><br><h3>Register</h3><br>" );
  svSendString( cnt, "Check <a href=\"gettingstarted\">this</a> page to get you started.<br><br>" );
  svSendString( cnt, "<form action=\"register2\" method=\"POST\">User name<br><input type=\"text\" name=\"name\"><br><br>Password<br><input type=\"password\" name=\"pass\"><br><br>Faction name<br><input type=\"text\" name=\"faction\"><br><br><input type=\"submit\" value=\"OK\"></form>" );
  svSendString( cnt, "</center></body></html>" );
  return;
}

void iohttpFunc_register2( svConnectionPtr cnt )
{
 int a, i, id;
 int session[4];
 unsigned char *name, *pass, *faction;
 FILE *file;
 iohttpDataPtr iohttp;
 unsigned char timebuf[256];
 long long int newd[DB_USER_NEWS_BASE];
 dbMailDef maild;
 char Message[] = "Congratulations! You have successfully registered your account!<br>Good luck and have fun,<br><br>- EVA";

 iohttpVarsInit( cnt );
 name = iohttpVarsFind( "name" );
 pass = iohttpVarsFind( "pass" );
 faction = iohttpVarsFind( "faction" );
 iohttpVarsCut();
 if( ( name ) && ( pass ) && ( faction ) )
 {
  if( ( id = cmdExecNewUser( name, pass, faction ) ) < 0 )
  {
   iohttpBase( cnt, 0 );
   if( cmdErrorString )
    svSendString( cnt, cmdErrorString );
   else
    svSendString( cnt, "Error encountered while registering user" );
   goto iohttpFunc_register2L0;
  }
  	newd[0] = svTickNum;
	 	newd[1] = CMD_NEWS_FLAGS_NEW;
  newd[2] = CMD_NEWS_MAIL;
  newd[3] = 0;
  newd[4] = 0; //From the admin
  newd[5] = 0; //From the admin team
  memcpy( &newd[6], "Admin", 6 );
  cmdUserNewsAdd( id, newd, CMD_NEWS_FLAGS_MAIL );
  	
  	
		(maild.mail).length = strlen(Message);
		maild.text = Message;
		(maild.mail).authorid = 0;
  sprintf( (maild.mail).authorname, "Admin" );
  (maild.mail).authorempire = 0;
  (maild.mail).time = time( 0 )-(3600*SERVER_TIME_ZONE);
  (maild.mail).tick = svTickNum;
  (maild.mail).flags = 0;
  if( dbMailAdd( id, 0, &maild ) < 0 )
  {
   printf("Error sending registration email\n");
  }
  
  if( ( dbUserLinkDatabase( cnt, id ) < 0 ) || ( dbSessionSet( cnt->dbuser, 0, session ) < 0 ) )
  {
   iohttpBase( cnt, 0 );
   svSendString( cnt, "Error encountered while registering session" );
   goto iohttpFunc_register2L0;
  }
  svSendPrintf( cnt, "Set-Cookie: USRID=%04x%04x%04x%04x%04x; path=/\n", id, session[0], session[1], session[2], session[3] );
  iohttpBase( cnt, 0 );

  svSendPrintf( cnt, "New user created<br>User name : %s<br>Password : %s<br>Faction name : %s<br>Account ID : %d<br>", name, pass, faction, id );

  if( ( file = fopen( LOGS_DIRECTORY "/register", "ab" ) ) )
  {
   //fprintf( file, "Register ID %d ( %x )\n", id, id );
   a = time( 0 )-(3600*SERVER_TIME_ZONE);
   strftime( timebuf, 256, "%T, %b %d %Y;", localtime( (time_t *)&a ) );
   fprintf( file, "Time %s", timebuf );
   fprintf( file, "Name %s;", name );
   fprintf( file, "Password %s;", pass );
   fprintf( file, "Faction %s;", faction );
   fprintf( file, "IP %s;", inet_ntoa( cnt->sockaddr.sin_addr ) );
   iohttp = cnt->iodata;
   strcpy(timebuf, iohttp->user_agent);
	  for(i=0;i<strlen(timebuf);i++)
	  {
	  	if(timebuf[i] == ';')
	  		timebuf[i] = ',';
	  }
	  fprintf( file, "%s;", timebuf );
   fprintf( file, "Cookie %s;;", iohttp->cookie );
   fprintf(file, "ID : %d ( %X );\n", id, id);
   fclose( file );
  }
 }
 else
 {
  iohttpBase( cnt, 0 );
  if( ( id = iohttpIdentify( cnt, 4|1 ) ) < 0 )
   return;
  svSendString( cnt, "This account was not activated yet." );
 }

 svSendString( cnt, "<form action=\"register3\" method=\"POST\"><br><br>Empire number<br><i>Leave blank to join a random empire</i><br><input type=\"text\" name=\"empire\"><br><br>" );
 svSendString( cnt, "Empire password<br><i>Only required if defined by the leader of the empire to join.</i><br><input type=\"text\" name=\"fampass\"><br><br>" );
 svSendString( cnt, "Faction race<br><i>The race of your people define many characteristics affecting different aspects of your faction.</i> - <a href=\"races\" target=\"_blank\">See races</a><br><select name=\"race\">" );
 for( a = 0 ; a < CMD_RACE_NUMUSED-1 ; a++ )
  svSendPrintf( cnt, "<option value=\"%d\">%s", a, cmdRaceName[a] );
 svSendString( cnt, "</select><br><br>" );

 svSendString( cnt, "<input type=\"submit\" value=\"OK\"></form>" );

 svSendString( cnt, "<br><br><a href=\"famranks\" target=\"_blank\">See empire rankings</a>" );
 svSendString( cnt, "<br><a href=\"rankings\" target=\"_blank\">See faction rankings</a>" );

 iohttpFunc_register2L0:
 svSendString( cnt, "</center></body></html>" );

 return;
}

void iohttpFunc_register3( svConnectionPtr cnt )
{
 int a, id, raceid;
 unsigned char *empire;
 unsigned char *fampass;
 unsigned char *race;

 iohttpBase( cnt, 0 );
 if( ( id = iohttpIdentify( cnt, 1|4 ) ) < 0 )
  return;

 iohttpVarsInit( cnt );
 empire = iohttpVarsFind( "empire" );
 fampass = iohttpVarsFind( "fampass" );
 race = iohttpVarsFind( "race" );
 iohttpVarsCut();
 if( ( empire ) && ( race ) )
 {
  for( a = 0 ; a < 31 ; a++ )
  {
   if( ( fampass[a] == 10 ) || ( fampass[a] == 13 ) )
    break;
  }
		
		if( empire[0] == 0 )
  	a = -1;
  else if( empire[0] == '#' ) 
   sscanf( &empire[1], "%d", &a );
  else
   sscanf( empire, "%d", &a );
  sscanf( race, "%d", &raceid );
  
  if( cmdExecNewUserEmpire( id, a, fampass, raceid, (cnt->dbuser)->level ) < 0 )
  {
   if( cmdErrorString )
    svSendString( cnt, cmdErrorString );
   else
    svSendString( cnt, "Error encountered while registering user" );
   svSendString( cnt, "<br><br><a href=\"/register2\">Try again</a>" );
   goto iohttpFunc_register3L0;
  }
  svSendPrintf( cnt, "<b>Account activated!</b><br>" );
  svSendString( cnt, "<br><br><a href=\"/\">Main page</a><br><br><a href=\"/main\">Log in</a>" );
  svSendString( cnt, "</center></body></html>" );
  return;
 }
 else
  svSendString( cnt, "Incorrect query!" );

 iohttpFunc_register3L0:
 svSendString( cnt, "<br><br><a href=\"/\">Main page</a><br><br><a href=\"/login\">Log in</a>" );
 svSendString( cnt, "</center></body></html>" );
 return;
}


void iohttpFunc_login( svConnectionPtr cnt )
{
 iohttpBase( cnt, 0 );

 svSendString( cnt, "<br><br><h3>Login</h3><br>" );
 svSendString( cnt, "<form action=\"main\" method=\"POST\">Name<br><input type=\"text\" name=\"name\"><br><br>Password<br><input type=\"password\" name=\"pass\"><br><br><input type=\"submit\" value=\"OK\"></form>" );

 svSendString( cnt, "</center></body></html>" );

 return;
}

void iohttpNewsString( svConnectionPtr cnt, long long int *newsd );

void iohttpFunc_main( svConnectionPtr cnt )
{
 int a, i, id, num;
 unsigned char *name, *pass;
 unsigned char rtpass[32];
 int session[4];
 FILE *file;
 iohttpDataPtr iohttp;
 unsigned char timebuf[256];
 long long int *newsp, *newsd;
 dbUserMainDef maind;

 iohttpVarsInit( cnt );
 name = iohttpVarsFind( "name" );
 pass = iohttpVarsFind( "pass" );
 iohttpVarsCut();

 if( ( file = fopen( LOGS_DIRECTORY "/login", "ab" ) ) )
 {
  a = time( 0 )-(3600*SERVER_TIME_ZONE);
  strftime( timebuf, 256, "%T, %b %d %Y", localtime( (time_t *)&a ) );
  fprintf( file, "%s;", timebuf );
  fprintf( file, "%s;", name );
  fprintf( file, "%s;", pass );
  fprintf( file, "%s;", inet_ntoa( cnt->sockaddr.sin_addr ) );
  iohttp = cnt->iodata;
  strcpy(timebuf, iohttp->user_agent);
  for(i=0;i<strlen(timebuf);i++)
  {
  	if(timebuf[i] == ';')
  		timebuf[i] = ',';
  }
  fprintf( file, "%s;", timebuf );
  fprintf( file, "%s;", iohttp->cookie );
 }

 if( ( name ) && ( pass ) )
 {
  for( a = 0 ; name[a] ; a++ )
  {
   if( name[a] == '+' )
    name[a] = ' ';
   else if( ( name[a] == 10 ) || ( name[a] == 13 ) )
    name[a] = 0;
  }
  for( a = 0 ; pass[a] ; a++ )
  {
   if( pass[a] == '+' )
    pass[a] = ' ';
   else if( ( pass[a] == 10 ) || ( pass[a] == 13 ) )
    pass[a] = 0;
  }
  if( ( id = dbUserSearch( name ) ) < 0 )
   goto iohttpFunc_mainL0;
  if( dbUserRetrievePassword( id, rtpass ) < 0 )
   goto iohttpFunc_mainL0;
  if( !( ioCompareExact( pass, rtpass ) ) )
   goto iohttpFunc_mainL0;
  if( dbUserLinkDatabase( cnt, id ) < 0 )
   goto iohttpFunc_mainL0;


  if( dbSessionSet( cnt->dbuser, rtpass, session ) < 0 )
   goto iohttpFunc_mainL0;


  svSendPrintf( cnt, "Set-Cookie: USRID=%04x%04x%04x%04x%04x; path=/\n", id, session[0], session[1], session[2], session[3] );

  dbUserMainRetrieve( id, &maind );
  maind.lasttime = time( 0 );
  dbUserMainSet( id, &maind );

  if( ( file ) && ( (cnt->dbuser)->flags & ( CMD_USER_FLAGS_KILLED | CMD_USER_FLAGS_DELETED | CMD_USER_FLAGS_NEWROUND ) ) )
  {
   fprintf( file, "ID : %d ( %x ) deactivated\n", id, id );
   fclose( file );
   file = 0;
  }

  if( (cnt->dbuser)->flags & CMD_USER_FLAGS_KILLED )
  {
   iohttpBase( cnt, 0 );
   svSendString( cnt, "Your Home Planet has been conquered and whiped out, your faction has been destroyed!<br><br><a href=\"register2\">Rejoin the Galaxy</a><br><br>" );
   num = dbUserNewsList( id, &newsp );
   newsd = newsp;
   if( !( num ) )
    svSendString( cnt, "<b>No reports</b>" );
   for( a = 0 ; a < num ; a++, newsd += DB_USER_NEWS_BASE )
   { 
    iohttpNewsString( cnt, newsd );
   }
   if( newsp )
    free( newsp );
   goto iohttpFunc_mainL1;
  }
  if( (cnt->dbuser)->flags & CMD_USER_FLAGS_DELETED )
  {
   iohttpBase( cnt, 0 );
   svSendString( cnt, "<br>Your account have been deleted by an administrator, most likely for not respecting a rule of the game.<br><br><a href=\"register2\">Register this account again</a><br><br>" );
   goto iohttpFunc_mainL1;
  }
  if( (cnt->dbuser)->flags & CMD_USER_FLAGS_NEWROUND )
  {
   iohttpBase( cnt, 0 );
   svSendString( cnt, "<br>The account has been deactivated for the new round, starting soon!<br>You'll be asked to join an empire of your choice again.<br><br><a href=\"register2\">Complete account registration</a><br><br>" );
   goto iohttpFunc_mainL1;
  }

  if( !( (cnt->dbuser)->flags & CMD_USER_FLAGS_ACTIVATED ) )
  {
   iohttpBase( cnt, 0 );
   svSendString( cnt, "<br>The activation of this account was not completed.<br><br><a href=\"register2\">Continue registration</a><br><br>" );
   iohttpFunc_mainL1:
   svSendString( cnt, "<a href=\"forum\">Public Forums</a>" );
   if(cnt->dbuser)
   {
	   if( (cnt->dbuser)->level >= LEVEL_MODERATOR )
	    svSendString( cnt, "<br><br><a href=\"moderator\">Moderator panel</a>" );
	   if( (cnt->dbuser)->level >= LEVEL_ADMINISTRATOR )
	    svSendString( cnt, "<br><a href=\"admin\">Admin panel</a>" );
	  }
   svSendString( cnt, "</center></body></html>" );
   return;
  }

 }
 else
 {
  if( ( id = iohttpIdentify( cnt, 0 ) ) < 0 )
   goto iohttpFunc_mainL0;
 }

 if( file )
 {
  fprintf( file, "ID : %d ( %x )\n", id, id );
  fclose( file );
  file = 0;
 }

 svSendString( cnt, "Content-Type: text/html\n\n" );
 svSendString( cnt, "<html><head><title>Ectroverse</title></head><frameset cols=\"155,*\" framespacing=\"0\" border=\"0\" marginwidth=\"0\" marginheight=\"0\" frameborder=\"no\">" );
 svSendString( cnt, "<frame src=\"menu\" name=\"menu\" marginwidth=\"0\" marginheight=\"0\" scrolling=\"no\" noresize>" );
 svSendString( cnt, "<frame src=\"hq\" name=\"main\" marginwidth=\"0\" marginheight=\"0\" noresize>" );
 svSendString( cnt, "<noframes>Your browser does not support frames! That's uncommon :).<br><br><a href=\"menu\">Menu</a></noframes>" );
 svSendString( cnt, "</frameset></html>" );
 return;

 iohttpFunc_mainL0:

 if( file )
 {
  fprintf( file, "Failed\n" );
  fclose( file );
  file = 0;
 }

 iohttpBase( cnt, 0 );
 svSendString( cnt, "Name or password incorrect" );
 svSendString( cnt, "</center></body></html>" );
 return;
}


void iohttpFunc_menu( svConnectionPtr cnt )
{
 int id, i, j;
 char szFaction[32];
 dbUserMainDef maind;
 svSendString( cnt, "Content-Type: text/html\n\n" );
 svSendString( cnt, "<html><head><style type=\"text/css\">a {\ntext-decoration: none\n}\na:hover {\ncolor: #00aaaa\n}\n</style></head><body bgcolor=\"#000000\" text=\"#FFFFFF\" link=\"#FFFFFF\" alink=\"#FFFFFF\" vlink=\"#FFFFFF\" leftmargin=\"0\" background=\"mbg.gif\">" );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( dbUserMainRetrieve( id, &maind ) < 0 )
  maind.empire = -1;

 svSendString( cnt, "<br><table cellspacing=\"0\" cellpadding=\"0\" width=\"150\" background=\"i36.jpg\" border=\"0\" align=\"center\"><tr><td><img height=\"40\" src=\"i18.jpg\" width=\"150\"></td></tr><tr><td background=\"i23.jpg\" height=\"20\"><b><font face=\"Tahoma\" size=\"2\">" );

 svSendString( cnt, "<a href=\"hq\" target=\"main\">Headquarters</a></font></b></td></tr><tr><td background=\"i36.jpg\"><table width=\"125\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\" align=\"left\"><tr><td><b><font face=\"Tahoma\" size=\"2\">" );
 svSendString( cnt, "<a href=\"council\" target=\"main\">Council</a><br><a href=\"units\" target=\"main\">Units</a><br><a href=\"market\" target=\"main\">Market</a><br><a href=\"planets\" target=\"main\">Planets</a><br>" );
 svSendPrintf( cnt, "<a href=\"empire\" target=\"main\">Empire</a><br>&nbsp;&nbsp;- <a href=\"forum?forum=%d\" target=\"main\">Forum</a><br>&nbsp;&nbsp;- <a href=\"famaid\" target=\"main\">Send aid</a><br>&nbsp;&nbsp;- <a href=\"famgetaid\" target=\"main\">Receive aid</a><br>&nbsp;&nbsp;- <a href=\"famnews\" target=\"main\">News</a><br>&nbsp;&nbsp;- <a href=\"famrels\" target=\"main\">Relations</a><br>", maind.empire + 100 );
 svSendString( cnt, "<a href=\"fleets\" target=\"main\">Fleets</a><br>" );
 svSendString( cnt, "<a href=\"mappick\" target=\"main\">Galaxy map</a><br>&nbsp;&nbsp;- <a href=\"map\" target=\"main\">Full map</a><br>&nbsp;&nbsp;- <a href=\"mapadv\" target=\"main\">Map generation</a><br>" );
 svSendString( cnt, "<a href=\"research\" target=\"main\">Research</a><br>" );
 svSendString( cnt, "<a href=\"spec\" target=\"main\">Operations</a><br>" );

 svSendString( cnt, "</font></b></td></tr></table></td></tr><tr><td background=\"i36.jpg\"><img height=\"15\" src=\"i53.jpg\" width=\"150\"></td></tr><tr><td background=\"i36.jpg\"><table width=\"125\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\" align=\"left\"><tr><td><b><font face=\"Tahoma\" size=\"2\">" );
 svSendString( cnt, "<a href=\"mail?type=0\" target=\"main\">Messages</a><br><a href=\"rankings\" target=\"main\">Faction rankings</a><br><a href=\"famranks\" target=\"main\">Empire rankings</a><br>" );
 svSendString( cnt, "<a href=\"forum\" target=\"main\">Forums</a><br>" );
 svSendString( cnt, "<a href=\"account\" target=\"main\">Account</a><br>" );
 svSendString( cnt, "<a href=\"logout\" target=\"_top\">Logout</a><br><br>" );
 
 svSendString( cnt, "<form action=\"search\" method=\"POST\" target=\"main\"><input type=\"text\" name=\"search\" size=\"10\" value=\"\"><input type=\"submit\" size=\"2\" value=\"OK\"></form><br>" );
 
 strcpy(szFaction, maind.faction);
 for(i=0;i<strlen(szFaction);i++)
	{
		if (szFaction[i] == ' ')
		{
			for(j=i;j<(strlen(szFaction)-1);j++)
				szFaction[j] = szFaction[j+1];
			szFaction[j] = '\0';
		}
		if(i == 15)
		{
			szFaction[i-1] = '\0';
			break;
		}
	}
 svSendString( cnt, "<a href=\"http://evtools.awardspace.com/starfury\" target=\"blank\">Guide</a><br>" );
 svSendString( cnt, "<a href=\"http://www.ectroverse.org:9122/chat\" target=\"blank\">Chat</a><br>" );

 if( cnt->dbuser )
 {
  if( (cnt->dbuser)->level >= LEVEL_MODERATOR )
   svSendString( cnt, "<br><a href=\"moderator\" target=\"main\">Moderator panel</a>" );
  if( (cnt->dbuser)->level >= LEVEL_ADMINISTRATOR )
   svSendString( cnt, "<br><a href=\"admin\" target=\"main\">Admin panel</a>" );
 }

 svSendString( cnt, "</font></b></td></tr></table></td></tr><tr><td><img height=\"20\" src=\"i55.jpg\" width=\"150\"></td></tr><tr><td><img height=\"75\" src=\"i56.jpg\" width=\"150\"></td></tr></table></body></html>" );

 return;
}


void iohttpNewsString( svConnectionPtr cnt, long long int *newsd )
{
 int a, b;
 dbUserMainDef maind;
 dbUserPtr user;
	
 svSendPrintf( cnt, "<br><br><i>Week %lld, year %lld</i><br>", newsd[0] % 52, newsd[0] / 52 );
 if( newsd[2] == CMD_NEWS_BUILDING )
  svSendPrintf( cnt, "You built %lld %s on the <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a>", newsd[4], cmdBuildingName[ newsd[3] & 0xFFF ], newsd[5], ( newsd[6] >> 8 ) & 0xFFF, newsd[6] >> 20, newsd[6] & 0xFF );
 else if( newsd[2] == CMD_NEWS_UNIT )
  svSendPrintf( cnt, "You built %lld %s", newsd[4], cmdUnitName[ newsd[3] & 0xFFF ] );
 else if( newsd[2] == CMD_NEWS_EXPLORE )
 {
  svSendPrintf( cnt, "Your exploration ship reached the <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a> and established a colony.", newsd[3], ( newsd[4] >> 8 ) & 0xFFF, newsd[4] >> 20, newsd[4] & 0xFF );
  if( ( newsd[5] >= 0 ) && ( newsd[5] < ARTEFACT_NUMUSED ) )
   svSendPrintf( cnt, "<br>We discovered an ancient artefact on this planet! <b>%s</b>", artefactName[ newsd[5] ] );
 }
 else if( newsd[2] == CMD_NEWS_EXPLORE_FAILED )
  svSendPrintf( cnt, "Your exploration ship reached the <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a>, but the planet was already habited. The ship is now awaiting your orders in this solar system.", newsd[3], ( newsd[4] >> 8 ) & 0xFFF, newsd[4] >> 20, newsd[4] & 0xFF );
 else if(newsd[2] == CMD_NEWS_FLEETS_MERGE)
 	svSendPrintf(cnt, "Two of your fleets have merge at position %lld,%lld after travelling %lld weeks", (newsd[3] >> 8 ) & 0xFFF, newsd[3] >> 20, newsd[6]);
 else if( newsd[2] == CMD_NEWS_STATION )
  svSendPrintf( cnt, "Your fleet reached the planet <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a> and stationned.", newsd[3], ( newsd[4] >> 8 ) & 0xFFF, newsd[4] >> 20, newsd[4] & 0xFF );
 else if( newsd[2] == CMD_NEWS_STATION_FAILED )
  svSendPrintf( cnt, "Your fleet heading for the <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a> could not station as you do not own this planet. Your forces is now awaiting your orders in this solar system.", newsd[3], ( newsd[4] >> 8 ) & 0xFFF, newsd[4] >> 20, newsd[4] & 0xFF );
 else if( newsd[2] == CMD_NEWS_RECALL )
  svSendPrintf( cnt, "A fleet travelling for %lld weeks rejoined our main forces", newsd[3] );
 else if( newsd[2] == CMD_NEWS_ATTACK )
 {
  if( dbUserMainRetrieve( newsd[3], &maind ) < 0 )
   return;
  svSendPrintf( cnt, "You lost the <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a> to <a href=\"player?id=%lld\">%s</a> of <a href=\"empire?id=%lld\">empire #%lld</a>", newsd[5], ( newsd[6] >> 8 ) & 0xFF, newsd[6] >> 20, newsd[6] & 0xFF, newsd[3], maind.faction, newsd[4], newsd[4] );
  goto iohttpNewsStringL0;
 }
 else if( newsd[2] == CMD_NEWS_ATTACK_FAILED )
 {
  if( dbUserMainRetrieve( newsd[3], &maind ) < 0 )
   return;
  svSendPrintf( cnt, "Your <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a> was unsuccessfully attacked by <a href=\"player?id=%lld\">%s</a> of <a href=\"empire?id=%lld\">empire #%lld</a>", newsd[5], ( newsd[6] >> 8 ) & 0xFF, newsd[6] >> 20, newsd[6] & 0xFF, newsd[3], maind.faction, newsd[4], newsd[4] );
  iohttpNewsStringL0:
  svSendString( cnt, "<br>You lost : " );
  for( a = b = 0 ; a < CMD_UNIT_FLEET ; a++ )
  {
   if( !( newsd[8+a] ) )
    continue;
   if( b )
    svSendString( cnt, ", " );
   svSendPrintf( cnt, "%lld %s", newsd[8+a], cmdUnitName[a] );
   b = 1;
  }
  if( newsd[8+2*CMD_UNIT_FLEET] )
  {
   if( b )
    svSendString( cnt, ", " );
   svSendPrintf( cnt, "%lld %s", newsd[8+2*CMD_UNIT_FLEET], cmdBuildingName[CMD_BUILDING_SATS] );
   b = 1;
  }
  if( !( b ) )
   svSendString( cnt, "Nothing" );
  svSendString( cnt, "<br>You destroyed : " );
  for( a = b = 0 ; a < CMD_UNIT_FLEET ; a++ )
  {
   if( !( newsd[8+CMD_UNIT_FLEET+a] ) )
    continue;
   if( b )
    svSendString( cnt, ", " );
   svSendPrintf( cnt, "%lld %s", newsd[8+CMD_UNIT_FLEET+a], cmdUnitName[a] );
   b = 1;
  }
  if( !( b ) )
   svSendString( cnt, "Nothing" );
  if( ( newsd[7] & 0xF00 ) )
  {
   b = newsd[7] & 0xF00;
   svSendString( cnt, "<br>Your forces preferred to avoid directly engaging enemy units in " );
   if( b == 0x100 )
    svSendString( cnt, "the first phase" );
   else if( b == 0x200 )
    svSendString( cnt, "the second phase" );
   else if( b == 0x300 )
    svSendString( cnt, "the first and second phases" );
   else if( b == 0x400 )
    svSendString( cnt, "the third phase" );
   else if( b == 0x500 )
    svSendString( cnt, "the first and third phases" );
   else if( b == 0x600 )
    svSendString( cnt, "the second and third phases" );
   else if( b == 0x700 )
    svSendString( cnt, "the first, second and third phases" );
   else if( b == 0x800 )
    svSendString( cnt, "the fourth phase" );
   else if( b == 0x900 )
    svSendString( cnt, "the first and fourth phases" );
   else if( b == 0xA00 )
    svSendString( cnt, "the second and fourth phases" );
   else if( b == 0xB00 )
    svSendString( cnt, "the first, second and fourth phases" );
   else if( b == 0xC00 )
    svSendString( cnt, "the third and fourth phases" );
   else if( b == 0xD00 )
    svSendString( cnt, "the first, third and fourth phases" );
   else if( b == 0xE00 )
    svSendString( cnt, "the second, third and fourth phases" );
   else if( b == 0xF00 )
    svSendString( cnt, "all phases" );
   svSendString( cnt, " of the battle" );
  }
  if( ( newsd[7] & 0xFF ) )
  {
   svSendString( cnt, "<br>Overwhelmed by defending enemy forces, the attacking fleet hastidly retreated to minimize losses in the " );
   if( ( newsd[7] & 0x10 ) )
    svSendString( cnt, "first phase of the battle" );
   if( ( newsd[7] & 0x20 ) )
    svSendString( cnt, "second phase of the battle" );
   if( ( newsd[7] & 0x40 ) )
    svSendString( cnt, "third phase of the battle" );
   if( ( newsd[7] & 0x80 ) )
    svSendString( cnt, "fourth phase of the battle" );
  }
 }
 else if( newsd[2] == CMD_NEWS_MARKET_BOUGHT )
  svSendPrintf( cnt, "You bought %lld %s on the market.", newsd[4], cmdRessourceName[newsd[3]+1] );
 else if( newsd[2] == CMD_NEWS_MARKET_SOLD )
  svSendPrintf( cnt, "You sold %lld %s on the market.", newsd[4], cmdRessourceName[newsd[3]+1] );
 else if( newsd[2] == CMD_NEWS_AID )
 {
  if( dbUserMainRetrieve( newsd[3], &maind ) < 0 )
   return;
  svSendPrintf( cnt, "You received an aid shipment from %s!<br>", maind.faction );
  for( a = b = 0 ; a < 4 ; a++ )
  {
   if( !( newsd[4+a] ) )
    continue;
   if( b )
    svSendString( cnt, ", " );
   svSendPrintf( cnt, "%lld %s", newsd[4+a], cmdRessourceName[a] );
   b = 1;
  }
  svSendString( cnt, " has been added to the reserves." );
 }
 else if( newsd[2] == CMD_NEWS_GETAID )
 {
  if( dbUserMainRetrieve( newsd[3], &maind ) < 0 )
   return;
  svSendPrintf( cnt, "%s requested an aid shipment!<br>", maind.faction );
  for( a = b = 0 ; a < 4 ; a++ )
  {
   if( !( newsd[4+a] ) )
    continue;
   if( b )
    svSendString( cnt, ", " );
   svSendPrintf( cnt, "%lld %s", newsd[4+a], cmdRessourceName[a] );
   b = 1;
  }
  svSendString( cnt, " has been taken from the faction reserves." );
 }
 else if( newsd[2] == CMD_NEWS_MAIL )
  svSendPrintf( cnt, "You received a <a href=\"mail?type=0\">message</a> from <a href=\"player?id=%lld\">%s</a> of <a href=\"empire?id=%lld\">empire #%lld</a>.", newsd[4], (char *)&newsd[6], newsd[5], newsd[5] );

 else if( ( newsd[2] >= CMD_NEWS_NUMOPBEGIN ) && ( newsd[2] <= CMD_NEWS_NUMOPEND ) )
 {
  svSendPrintf( cnt, "Your agents reached their destination, the <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a>", newsd[3], ( newsd[4] >> 8 ) & 0xFFF, newsd[4] >> 20, newsd[4] & 0xFF );
  if( ( user = dbUserLinkID( newsd[5] ) ) )
   svSendPrintf( cnt, " owned by <a href=\"player?id=%lld\">%s</a> of <a href=\"empire?id=%lld\">empire #%lld</a>", newsd[5], user->faction, newsd[6], newsd[6] );
  svSendPrintf( cnt, " to perform <b>%s</b>.<br>", cmdAgentopName[newsd[9]] );
  if( user )
  {
   if( newsd[7] == -1 )
    svSendPrintf( cnt, "<i>Your agents successfully stayed undiscovered during the operation.</i><br>" );
   else
    svSendPrintf( cnt, "<i>%lld of your agents were caught, but your forces still managed to kill %lld defending agents.</i><br>", newsd[7], newsd[8] );
  }

  if( newsd[2] == CMD_NEWS_OPSPYTARGET )
  {
   svSendPrintf( cnt, "Your agents gathered the following information.<br>" );
   svSendString( cnt, "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\"><tr><td>" );
   if( newsd[10] == -1 )
    svSendString( cnt, "Fleet readiness : unknown<br>" );
   else
    svSendPrintf( cnt, "Fleet readiness : %lld%%<br>", newsd[10] >> 16 );
   if( newsd[11] == -1 )
    svSendString( cnt, "Psychics readiness : unknown<br>" );
   else
    svSendPrintf( cnt, "Psychics readiness : %lld%%<br>", newsd[11] >> 16 );
   if( newsd[12] == -1 )
    svSendString( cnt, "Agents readiness : unknown<br>" );
   else
    svSendPrintf( cnt, "Agents readiness : %lld%%<br>", newsd[12] >> 16 );
   for( a = 13 ; a < 13+CMD_RESSOURCE_NUMUSED ; a++ )
   {
    svSendString( cnt, cmdRessourceName[a-13] );
    if( newsd[a] == -1 )
     svSendString( cnt, " : unknown<br>" );
    else
     svSendPrintf( cnt, " : %lld<br>", newsd[a] );
   }
   svSendString( cnt, "</td></tr></table>" );
  }
  else if( newsd[2] == CMD_NEWS_OPOBSERVEPLANET )
  {
   svSendPrintf( cnt, "Your agents gathered the following information.<br>" );
   svSendString( cnt, "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\"><tr><td>" );
   if( newsd[10] == -1 )
    svSendString( cnt, "Planet size : unknown<br>" );
   else
    svSendPrintf( cnt, "Planet size : %lld<br>", newsd[10] );
   if( newsd[11] )
   {
    if( newsd[11] == -1 )
     svSendString( cnt, "Population : unknown<br>" );
    else
     svSendPrintf( cnt, "Population : %lld0<br>", newsd[11] );
   }
   if( newsd[12] )
   {
    if( newsd[12] == -1 )
     svSendString( cnt, "Maximum population : unknown<br>" );
    else
     svSendPrintf( cnt, "Maximum population : %lld0<br>", newsd[12] );
   }
   if( newsd[13] == -1 )
    svSendString( cnt, "Portals coverage : unknown<br>" );
   else
    svSendPrintf( cnt, "Portals coverage : %lld%%<br>", newsd[13] );
   if( newsd[14] == -1 )
    svSendString( cnt, "Portal : unknown<br>" );
   else
   {
    if( newsd[14] == CMD_PLANET_FLAGS_PORTAL )
     svSendPrintf( cnt, "Portal : Present<br>" );
    else if( newsd[14] == CMD_PLANET_FLAGS_PORTAL_BUILD )
     svSendPrintf( cnt, "Portal : Under construction<br>" );
    else if( newsd[14] == CMD_PLANET_FLAGS_PORTAL_BUILD )
     svSendPrintf( cnt, "Portal : None<br>" );
   }
   for( a = 15 ; a < 15+CMD_BLDG_NUMUSED ; a++ )
   {
    svSendString( cnt, cmdBuildingName[a-15] );
    if( newsd[a] == -1 )
     svSendString( cnt, " : unknown<br>" );
    else
     svSendPrintf( cnt, " : %lld<br>", newsd[a] );
   }
   if( newsd[15+CMD_BLDG_NUMUSED] >= 0 )
    svSendPrintf( cnt, "We discovered an ancient artefact on this planet! <b>%s</b><br>", artefactName[ newsd[15+CMD_BLDG_NUMUSED] ] );
   if( newsd[15+2+CMD_BLDG_NUMUSED] > 0 )
    svSendPrintf( cnt, "%s production : +%lld%%<br>", cmdRessourceName[ newsd[15+1+CMD_BLDG_NUMUSED] ], newsd[15+2+CMD_BLDG_NUMUSED] );
   svSendString( cnt, "</td></tr></table>" );
  }
  else if( newsd[2] == CMD_NEWS_OPNETWORKVIRUS )
  {
   svSendPrintf( cnt, "These are the results of the operation.<br>" );
   svSendString( cnt, "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\"><tr><td>" );
   if( newsd[10] == -1 )
    svSendString( cnt, "Destroying research : failed<br>" );
   else
    svSendPrintf( cnt, "Destroying research : %lld %% lost<br>", newsd[10] );
   if( newsd[11] == -1 )
    svSendString( cnt, "Increasing upkeep : failed<br>" );
   else
    svSendPrintf( cnt, "Increasing upkeep : %lld weeks<br>", newsd[11] );
   svSendString( cnt, "</td></tr></table>" );
  }
  else if( newsd[2] == CMD_NEWS_OPINFILTRATION )
  {
   svSendPrintf( cnt, "Your agents gathered the following information.<br>" );
   svSendString( cnt, "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\"><tr><td>" );
   for( a = 10 ; a < 10+CMD_RESSOURCE_NUMUSED ; a++ )
   {
    svSendString( cnt, cmdRessourceName[a-10] );
    if( newsd[a] == -1 )
     svSendString( cnt, " : unknown<br>" );
    else
     svSendPrintf( cnt, " : %lld<br>", newsd[a] );
   }
   for( a = 14 ; a < 14+CMD_RESEARCH_NUMUSED ; a++ )
   {
    svSendPrintf( cnt, "%s research", cmdResearchName[a-14] );
    if( newsd[a] == -1 )
     svSendString( cnt, " : unknown<br>" );
    else
     svSendPrintf( cnt, " : %lld %%<br>", newsd[a] );
   }
   for( a = 14+CMD_RESEARCH_NUMUSED ; a < 14+CMD_RESEARCH_NUMUSED+CMD_BLDG_NUMUSED ; a++ )
   {
    svSendString( cnt, cmdBuildingName[a-14-CMD_RESEARCH_NUMUSED] );
    if( newsd[a] == -1 )
     svSendString( cnt, " : unknown<br>" );
    else
     svSendPrintf( cnt, " : %lld<br>", newsd[a] );
   }
   svSendString( cnt, "</td></tr></table>" );
  }
  else if( newsd[2] == CMD_NEWS_OPBIOINFECTION )
  {
   svSendPrintf( cnt, "These are the results of the operation.<br>" );
   svSendString( cnt, "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\"><tr><td>" );
   if( newsd[10] == -1 )
    svSendString( cnt, "Kill population : failed<br>" );
   else
    svSendPrintf( cnt, "Kill population : %lld0<br>", newsd[10] );
   svSendString( cnt, "</td></tr></table>" );
  }
  else if( newsd[2] == CMD_NEWS_OPENERGYTRANSFER )
  {
   svSendPrintf( cnt, "These are the results of the operation.<br>" );
   svSendString( cnt, "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\"><tr><td>" );
   if( newsd[10] == -1 )
    svSendString( cnt, "Target's loss of energy : failed<br>" );
   else
    svSendPrintf( cnt, "Target's loss of energy : %lld<br>", newsd[10] );
   if( newsd[11] == -1 )
    svSendString( cnt, "Energy successfully acquired : failed<br>" );
   else
    svSendPrintf( cnt, "Energy successfully acquired : %lld<br>", newsd[11] );
   svSendString( cnt, "</td></tr></table>" );
  }
  else if( newsd[2] == CMD_NEWS_OPMILITARYSAB )
  {
   svSendPrintf( cnt, "These are the results of the operation.<br>" );
   svSendString( cnt, "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\"><tr><td>" );
   if( newsd[10] == -1 )
    svSendString( cnt, "Destroy units : failed<br>" );
   else if( newsd[10] == -2 )
    svSendString( cnt, "Destroy units : failed<br>It appears there is no portal on the planet to reach the main fleet from!<br>" );
   else
   {
    for( a = 10 ; a < 10+CMD_UNIT_FLEET ; a++ )
    {
     if( newsd[a] > 0 )
      svSendPrintf( cnt, "%s : %lld destroyed<br>", cmdUnitName[a-10], newsd[a] );
    }
   }
   svSendString( cnt, "</td></tr></table>" );
  }
  else if( newsd[2] == CMD_NEWS_OPNUKEPLANET )
  {
   svSendPrintf( cnt, "These are the results of the operation.<br>" );
   svSendString( cnt, "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\"><tr><td>" );
   if( newsd[10] == -1 )
    svSendString( cnt, "Place nuclear devices : failed<br>" );
   else
    svSendString( cnt, "Place nuclear devices : succeeded, planet now uninhabited<br>" );
   svSendString( cnt, "</td></tr></table>" );
  }
  else if( newsd[2] == CMD_NEWS_OPHIGHINFIL )
  {
   svSendPrintf( cnt, "These are the results of the operation.<br>" );
   svSendString( cnt, "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\"><tr><td>" );
   if( newsd[10] == -1 )
    svSendString( cnt, "Infiltration : failed<br>" );
   else
   {
    svSendString( cnt, "Faction infiltration : success<br>" );
    if( newsd[10] & 1 )
     svSendString( cnt, "Planets network infiltration : success<br>" );
    else
     svSendString( cnt, "Planets network infiltration : failed<br>" );
    if( newsd[10] & 2 )
     svSendString( cnt, "Fleets infiltration : success<br>" );
    else
     svSendString( cnt, "Fleets infiltration : failed<br>" );
    svSendString( cnt, "You can access the faction information in the <a href=\"spec\">list of current operations</a> for the next two years.<br>" );
   }
  }
	 else if( newsd[2] == CMD_NEWS_OPPLANETBEACON )
  {
   svSendPrintf( cnt, "These are the results of the operation.<br>" );
   svSendString( cnt, "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\"><tr><td>" );
   if( newsd[10] >= 1 )
   {
    svSendString( cnt, "Planeteray beacon : success<br>" );
    svSendString( cnt, "The planet have 0 dark web effect but 110% of protection by the owner fleet, all this for 24 ticks<br>" );
   }
   else
   {
    svSendString( cnt, "Planeteray beacon : fail<br>" );
   }
   svSendString( cnt, "</td></tr></table>" );
  }
 }
 else if( ( newsd[2] >= CMD_NEWS_NUMOPTARGETBEGIN ) && ( newsd[2] <= CMD_NEWS_NUMOPTARGETEND ) )
 {
  if( newsd[7] != -1 )
  {
   svSendString( cnt, "Your forces intercepted some agents from " );
   if( ( user = dbUserLinkID( newsd[5] ) ) )
    svSendPrintf( cnt, "<a href=\"player?id=%lld\">%s</a> of <a href=\"empire?id=%lld\">empire #%lld</a>", newsd[5], user->faction, newsd[6], newsd[6] );
   else
    svSendString( cnt, "an unknown faction" );
  }
  else
   svSendString( cnt, "Your forces found traces of agents from an unknown faction" );

  svSendPrintf( cnt, " performing a <b>%s</b> operation on <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a>.<br>", cmdAgentopName[newsd[9]], newsd[3], ( newsd[4] >> 8 ) & 0xFFF, newsd[4] >> 20, newsd[4] & 0xFF );
  if( newsd[7] != -1 )
   svSendPrintf( cnt, "<i>%lld enemy agents have been arrested, %lld of your agents have been killed.</i><br>", newsd[7], newsd[8] );

  if( newsd[2] == CMD_NEWS_OPNETWORKVIRUS_TARGET )
  {
   svSendString( cnt, "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\"><tr><td>" );
   if( newsd[10] != -1 )
    svSendPrintf( cnt, "%lld %% of your research has been lost!<br>", newsd[10] );
   if( newsd[11] != -1 )
    svSendPrintf( cnt, "Your net building upkeep will be 15%% for higher for %lld weeks.<br>", newsd[11] );
   svSendString( cnt, "</td></tr></table>" );
  }
  else if( newsd[2] == CMD_NEWS_OPBIOINFECTION_TARGET )
  {
   if( newsd[10] != -1 )
    svSendPrintf( cnt, "%lld0 citizens of your faction died as the disease rapidly spreads.<br>", newsd[10] );
  }
  else if( newsd[2] == CMD_NEWS_OPENERGYTRANSFER_TARGET )
  {
   if( newsd[10] != -1 )
    svSendPrintf( cnt, "%lld energy has been reported missing from your reserves.<br>", newsd[10] );
  }
  else if( newsd[2] == CMD_NEWS_OPMILITARYSAB_TARGET )
  {
   svSendString( cnt, "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\"><tr><td>" );
   if( newsd[10] >= 0 )
   {
    for( a = 10 ; a < 10+CMD_UNIT_FLEET ; a++ )
    {
     if( newsd[a] > 0 )
      svSendPrintf( cnt, "%s : %lld destroyed<br>", cmdUnitName[a-10], newsd[a] );
    }
   }
   svSendString( cnt, "</td></tr></table>" );
  }
  else if( newsd[2] == CMD_NEWS_OPNUKEPLANET_TARGET )
  {
   if( newsd[10] != -1 )
    svSendString( cnt, "Nuclear devices exploded everywhere on the surface of the planet! All buildings have been destroyed and the planet is now unhabited.<br>" );
  }
 }

 else if( ( newsd[2] >= CMD_NEWS_NUMSPBEGIN ) && ( newsd[2] <= CMD_NEWS_NUMSPEND ) )
 {
  svSendPrintf( cnt, "Your psychics are casting <b>%s</b> on ", cmdPsychicopName[newsd[7]] );
  if( newsd[3] == (cnt->dbuser)->id )
   svSendPrintf( cnt, "your faction.<br>" );
  else
  {
   if( ( user = dbUserLinkID( newsd[3] ) ) )
    svSendPrintf( cnt, "<a href=\"player?id=%lld\">%s</a> of <a href=\"empire?id=%lld\">empire #%lld</a>.<br>", newsd[3], user->faction, newsd[4], newsd[4] );
   else
    svSendPrintf( cnt, "an unknown faction.<br>" );
   if( ( newsd[3] != (cnt->dbuser)->id ) && ( user ) )
   {
    if( newsd[5] == -1 )
     svSendPrintf( cnt, "<i>Your psychics successfully stayed undiscovered while performing the spell.</i><br>" );
    else
     svSendPrintf( cnt, "<i>%lld of your psychics went mad, it appears %lld defending psychics also suffered critical brain damages.</i><br>", newsd[5], newsd[6] );
   }
  }

  if( newsd[2] == CMD_NEWS_SPIRRAECTRO )
  {
   if( newsd[8] != -1 )
    svSendPrintf( cnt, "%lld ectrolium has been irradiated and is now unusable!<br>", newsd[8] );
  }
  else if( newsd[2] == CMD_NEWS_SPDARKWEB )
  {
   if( newsd[8] != -1 )
    svSendPrintf( cnt, "Your faction is now more difficult to attack by %lld %%.<br>", newsd[8] );
  }
  else if( newsd[2] == CMD_NEWS_SPINCANDESCENCE )
  {
   if( newsd[8] != -1 )
    svSendPrintf( cnt, "%lld crystal has been converted in %lld energy.<br>", newsd[8], newsd[9] );
  }
  else if( newsd[2] == CMD_NEWS_SPBLACKMIST )
  {
   if( newsd[8] != -1 )
    svSendPrintf( cnt, "A dense mist is spreading around the target's planets, your psychics estimate solar collectors efficiency has been reduced by %lld %%.<br>", newsd[8] );
  }
  else if( newsd[2] == CMD_NEWS_SPWARILLUSIONS )
  {
   if( newsd[8] != -1 )
    svSendPrintf( cnt, "Various illusions will accompany your fleets for %lld weeks, keeping around %lld %% of enemy fire away from friendly units.<br>", newsd[9], newsd[8] );
  }
  else if( newsd[2] == CMD_NEWS_SPPHANTOMS )
  {
   if( newsd[8] != -1 )
    svSendPrintf( cnt, "%lld Phantoms have been conjured to fight for your faction!<br>", newsd[8] );
  }

 }

 else if( ( newsd[2] >= CMD_NEWS_NUMSPTARGETBEGIN ) && ( newsd[2] <= CMD_NEWS_NUMSPTARGETEND ) )
 {
  if( newsd[5] != -1 )
  {
   svSendString( cnt, "Your forces felt the influence of psychics from " );
   if( ( user = dbUserLinkID( newsd[3] ) ) )
    svSendPrintf( cnt, "<a href=\"player?id=%lld\">%s</a> of <a href=\"empire?id=%lld\">empire #%lld</a>", newsd[3], user->faction, newsd[4], newsd[4] );
   else
    svSendString( cnt, "an unknown faction" );
  }
  else
   svSendString( cnt, "Your forces felt the influence of psychics from an unknown faction" );
  svSendPrintf( cnt, " performing a <b>%s</b> spell.<br>", cmdPsychicopName[newsd[7]] );
  if( newsd[5] != -1 )
   svSendPrintf( cnt, "<i>%lld enemy psychics have went mad, %lld of your psychics also suffered critical brain damages.</i><br>", newsd[5], newsd[6] );

  if( newsd[2] == CMD_NEWS_SPIRRAECTRO_TARGET )
  {
   if( newsd[8] != -1 )
    svSendPrintf( cnt, "%lld ectrolium has been irradiated and is now unusable!<br>", newsd[8] );
  }
  else if( newsd[2] == CMD_NEWS_SPBLACKMIST_TARGET )
  {
   if( newsd[8] != -1 )
    svSendPrintf( cnt, "A dense mist is spreading around your planets, your psychics estimate solar collectors efficiency has been reduced by %lld %%.<br>", newsd[8] );
  }
 }




 else if( ( newsd[2] >= CMD_NEWS_NUMINBEGIN ) && ( newsd[2] <= CMD_NEWS_NUMINEND ) )
 {
  svSendPrintf( cnt, "Your ghost ships casted <b>%s</b> ", cmdGhostopName[newsd[10]] );
  if( cmdGhostopFlags[ newsd[10] ] & 4 )
   svSendPrintf( cnt, "in the system %lld,%lld<br>", ( newsd[4] >> 8 ) & 0xFFF, newsd[4] >> 20 );
  else
  {
   svSendPrintf( cnt, "on the <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a>", newsd[3], ( newsd[4] >> 8 ) & 0xFFF, newsd[4] >> 20, newsd[4] & 0xFF );
   if( ( user = dbUserLinkID( newsd[5] ) ) )
   {
    svSendPrintf( cnt, " owned by <a href=\"player?id=%lld\">%s</a> of <a href=\"empire?id=%lld\">empire #%lld</a><br>", newsd[5], user->faction, newsd[6], newsd[6] );
    if( newsd[7] <= 0 )
     svSendPrintf( cnt, "<i>Your ghost ships successfully stayed undiscovered during the incantation.</i><br>" );
    else
     svSendPrintf( cnt, "<i>%lld of your ghost ships were oblitered by defending forces.</i><br>", newsd[7] );
    if( newsd[8] > 0 )
     svSendPrintf( cnt, "<i>%lld defending psychics were killed.</i><br>", newsd[8] );
    if( newsd[9] > 0 )
     svSendPrintf( cnt, "<i>%lld defending ghost ships were destroyed.</i><br>", newsd[9] );
   }
  }

  if( newsd[2] == CMD_NEWS_INSENSE )
  {
   if( newsd[11] & 0x10000 )
   {
    a = newsd[11] & 0xFFFF;
    if( a >= 4 )
     svSendPrintf( cnt, "<i>An artefact has been discovered, at the coordinates %lld,%lld:%lld!</i><br>", ( newsd[12] >> 8 ) & 0xFF, (newsd[12] >> 20) & 0xFF, newsd[12] & 0xFF );
    else if( a == 3 )
     svSendPrintf( cnt, "<i>An artefact has been found in a nearby system, at the coordinates %lld,%lld!</i><br>", ( newsd[12] >> 8 ) & 0xFF, (newsd[12] >> 20) & 0xFF );
    else if( a == 2 )
     svSendPrintf( cnt, "<i>An artefact was detected in the area, but its precise location remains unknown.</i><br>" );
    else
     svSendPrintf( cnt, "<i>An artefact was felt somewhere in the area!</i><br>" );
   }
   else
    svSendPrintf( cnt, "<i>No artefact was felt in the area.</i><br>" );
   
   
   if( dbUserMainRetrieve( (cnt->dbuser)->id, &maind ) < 0 )
   	return;			   
  }
  else if( newsd[2] == CMD_NEWS_INSURVEY )
  {
   if( newsd[11] )
   {
    svSendPrintf( cnt, "Planets surveyed : %lld<br>", newsd[11] );
    svSendPrintf( cnt, "Incantation duration : %lld<br>", newsd[12] );
    svSendPrintf( cnt, "You can access the system information in the <a href=\"spec\">list of current operations</a> for the duration of the incantation.<br>" );
   }
   else
    svSendString( cnt, "Planets surveyed : none<br>" );
  }
  else if( newsd[2] == CMD_NEWS_INSHIELDING )
  {
   svSendPrintf( cnt, "Planetary shield strength : %lld<br>", newsd[11] );
   svSendPrintf( cnt, "Incantation duration : %lld<br>", newsd[12] );
  }
  else if( newsd[2] == CMD_NEWS_INFORCEFIELD )
  {
   svSendPrintf( cnt, "Force field strength : %lld%%<br>", newsd[11] );
   svSendPrintf( cnt, "Incantation duration : %lld<br>", newsd[12] );
  }
  else if( newsd[2] == CMD_NEWS_INVORTEX )
  {
   svSendString( cnt, "Vortex portal duration : " );
   if( newsd[11] != -1 )
    svSendPrintf( cnt, "%lld<br>", newsd[11] );
   else
    svSendString( cnt, "failed<br>" );
  }
  else if( newsd[2] == CMD_NEWS_INMINDCONTROL )
  {
   svSendString( cnt, "Mind control incantation : " );
   if( newsd[11] == -1 )
    svSendString( cnt, "failed<br>" );
   else
    svSendString( cnt, "success<br>" );
  }
  else if( newsd[2] == CMD_NEWS_INENERGYSURGE )
  {
   if( newsd[17] == -1 )
    svSendString( cnt, "Energy surge : failed<br>" );
   else
   {
    if( newsd[17] > 0 )
     svSendPrintf( cnt, "Energy wasted : %lld<br>", newsd[17] );
    if( newsd[11] > 0 )
     svSendPrintf( cnt, "Mineral lost : %lld<br>", newsd[11] );
    if( newsd[12] > 0 )
     svSendPrintf( cnt, "Crystal lost : %lld<br>", newsd[12] );
    if( newsd[13] > 0 )
     svSendPrintf( cnt, "Ectrolium lost : %lld<br>", newsd[13] );
    if( newsd[14] > 0 )
     svSendPrintf( cnt, "Solar collectors destroyed : %lld<br>", newsd[14] );
    if( newsd[15] > 0 )
     svSendPrintf( cnt, "Fission reactors destroyed : %lld<br>", newsd[15] );
    if( newsd[16] > 0 )
     svSendPrintf( cnt, "Research points eliminated : %lld<br>", newsd[16] );
   }
  }



 }
 else if( ( newsd[2] >= CMD_NEWS_NUMINTARGETBEGIN ) && ( newsd[2] <= CMD_NEWS_NUMINTARGETEND ) )
 {
  if( newsd[7] != -1 )
  {
   svSendString( cnt, "Your forces were the target of ghost ships from" );
   if( ( user = dbUserLinkID( newsd[5] ) ) )
    svSendPrintf( cnt, " <a href=\"player?id=%lld\">%s</a> of <a href=\"empire?id=%lld\">empire #%lld</a>", newsd[5], user->faction, newsd[6], newsd[6] );
   else
    svSendString( cnt, " an unknown faction" );
  }
  else
   svSendString( cnt, "Your forces were the target of ghost ships from an unknown faction" );
  svSendPrintf( cnt, " performing a <b>%s</b> incantation", cmdGhostopName[newsd[10]] );
  if( newsd[3] != -1 )
   svSendPrintf( cnt, " on <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a>", newsd[3], ( newsd[4] >> 8 ) & 0xFFF, newsd[4] >> 20, newsd[4] & 0xFF );
  svSendString( cnt, ".<br>" );
  if( newsd[7] > 0 )
   svSendPrintf( cnt, "<i>%lld enemy ghost ships were destroyed.</i><br>", newsd[7] );
  if( newsd[8] > 0 )
   svSendPrintf( cnt, "<i>%lld of your psychics were killed.</i><br>", newsd[8] );
  if( newsd[9] > 0 )
   svSendPrintf( cnt, "<i>%lld of your ghost ships were destroyed.</i><br>", newsd[9] );

  if( newsd[2] == CMD_NEWS_INSHIELDING_TARGET )
  {
   svSendPrintf( cnt, "Planetary shield strength : %lld<br>", newsd[11] );
   svSendPrintf( cnt, "Incantation duration : %lld<br>", newsd[12] );
  }
  else if( newsd[2] == CMD_NEWS_INFORCEFIELD_TARGET )
  {
   svSendPrintf( cnt, "Force field strength : %lld%%<br>", newsd[11] );
   svSendPrintf( cnt, "Incantation duration : %lld<br>", newsd[12] );
  }
  else if( newsd[2] == CMD_NEWS_INMINDCONTROL_TARGET )
  {
   svSendString( cnt, "Mind control incantation : " );
   if( newsd[11] == -1 )
    svSendString( cnt, "failed<br>" );
   else
    svSendString( cnt, "success<br>" );
  }

  else if( newsd[2] == CMD_NEWS_INENERGYSURGE_TARGET )
  {
   if( newsd[17] == -1 )
    svSendString( cnt, "Energy surge : failed<br>" );
   else
   {
    if( newsd[17] > 0 )
     svSendPrintf( cnt, "Energy wasted : %lld<br>", newsd[17] );
    if( newsd[11] > 0 )
     svSendPrintf( cnt, "Mineral lost : %lld<br>", newsd[11] );
    if( newsd[12] > 0 )
     svSendPrintf( cnt, "Crystal lost : %lld<br>", newsd[12] );
    if( newsd[13] > 0 )
     svSendPrintf( cnt, "Ectrolium lost : %lld<br>", newsd[13] );
    if( newsd[14] > 0 )
     svSendPrintf( cnt, "Solar collectors destroyed : %lld<br>", newsd[14] );
    if( newsd[15] > 0 )
     svSendPrintf( cnt, "Fission reactors destroyed : %lld<br>", newsd[15] );
    if( newsd[16] > 0 )
     svSendPrintf( cnt, "Research points eliminated : %lld<br>", newsd[16] );
   }
  }


 }



 else if( newsd[2] == CMD_NEWS_PLANET_OFFER )
 {
  if( ( user = dbUserLinkID( newsd[3] ) ) )
   svSendPrintf( cnt, "<a href=\"player?id=%lld\">%s</a> offered a <a href=\"planet?id=%lld\">planet</a> to your faction. <a href=\"pltake?id=%lld\">Take it</a><br>", newsd[3], user->faction, newsd[4], newsd[4] );
 }
 else if( newsd[2] == CMD_NEWS_PLANET_GIVEN )
 {
  if( ( user = dbUserLinkID( newsd[3] ) ) )
   svSendPrintf( cnt, "<a href=\"player?id=%lld\">%s</a> took control of a <a href=\"planet?id=%lld\">planet</a> previously offered.<br>", newsd[3], user->faction, newsd[4] );
 }
 else if( newsd[2] == CMD_NEWS_PLANET_TAKEN )
 {
  if( ( user = dbUserLinkID( newsd[3] ) ) )
   svSendPrintf( cnt, "You took control of a <a href=\"planet?id=%lld\">planet</a> offered by <a href=\"player?id=%lld\">%s</a>.<br>", newsd[4], newsd[3], user->faction );
 }



 else if( newsd[2] == CMD_NEWS_OPCANCEL )
  svSendPrintf( cnt, "An agents operation has been cancelled." );
 else if( newsd[2] == CMD_NEWS_SPCANCEL )
  svSendPrintf( cnt, "A psychics spell has been cancelled." );
 else if( newsd[2] == CMD_NEWS_INCANCEL )
  svSendPrintf( cnt, "A ghost ships incantation has been cancelled." );

 else if( newsd[2] != -1 )
  svSendPrintf( cnt, "Unknown report code : %lld ( tell the admin, thanks ;) )", newsd[2] );

 return;
}


int iohttpFamNewsEntryCount;

void iohttpFamNewsEntry( svConnectionPtr cnt, int picture, long long int *newsd )
{
 svSendPrintf( cnt, "<tr><td" );
 if( !( iohttpFamNewsEntryCount ) )
  svSendPrintf( cnt, " width=\"5%%\"" );
 if( picture >= 0 )
  svSendPrintf( cnt, "><img src=\"fn%d.gif\"></td><td", picture );
 else
  svSendPrintf( cnt, "><br></td><td" );
 if( !( iohttpFamNewsEntryCount ) )
 {
  svSendPrintf( cnt, " width=\"95%%\"" );
  iohttpFamNewsEntryCount = 1;
 }
 svSendPrintf( cnt, "><i>Week %lld, year %lld</i><br>", newsd[0] % 52, newsd[0] / 52 );
 return;
}

void iohttpFamNews( svConnectionPtr cnt, int num, long long int *newsd, dbMainEmpirePtr empired )
{
 int a, b, c;
 dbUserMainDef maind;
 dbUserMainDef mfamd[32];
 dbUserPtr user;

 memset( mfamd, 0, 32*sizeof(dbUserMainDef) );
 for( a = 0 ; a < empired->numplayers ; a++ )
 {
  if( dbUserMainRetrieve( empired->player[a], &mfamd[a] ) < 0 )
   continue;
 }
 svSendPrintf( cnt, "Current date : Week %d, year %d<br>", svTickNum % 52, svTickNum / 52 );
 if( svTickStatus )
  svSendPrintf( cnt, "%d seconds before tick<br>", (int)( svTickTime - time(0) ) );
 else
  svSendPrintf( cnt, "Time frozen<br>" );

 if( !( num ) )
 {
  svSendString( cnt, "<b>No reports</b>" );
  return;
 }

 iohttpFamNewsEntryCount = 0;
 svSendString( cnt, "<table cellspacing=\"4\" cellpadding=\"4\">" );
 for( c = 0 ; c < num ; c++, newsd += DB_USER_NEWS_BASE )
 {
  for( b = 0 ; ; b++ )
  {
   if( b == empired->numplayers )
    goto iohttpFamNewsL0;
   if( newsd[1] == empired->player[b] )
    break;
  }
  if( newsd[2] == CMD_NEWS_EXPLORE )
  {
   iohttpFamNewsEntry( cnt, 0, newsd );
   svSendPrintf( cnt, "An exploration ship of %s reached the <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a> and established a colony.", mfamd[b].faction, newsd[3], ( newsd[4] >> 8 ) & 0xFFF, newsd[4] >> 20, newsd[4] & 0xFF );
   if( ( newsd[5] >= 0 ) && ( newsd[5] < ARTEFACT_NUMUSED ) )
    svSendPrintf( cnt, "<br>An ancient artefact has been discovered on this planet! <b>%s</b>", artefactName[ newsd[5] ] );
  }
  else if( newsd[2] == CMD_NEWS_EXPLORE_FAILED )
  {
   iohttpFamNewsEntry( cnt, 1, newsd );
   svSendPrintf( cnt, "An exploration ship of %s reached the <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a>, but the planet was already habited.", mfamd[b].faction, newsd[3], ( newsd[4] >> 8 ) & 0xFFF, newsd[4] >> 20, newsd[4] & 0xFF );
  }
  else if( newsd[2] == CMD_NEWS_FAMATTACK )
  {
   iohttpFamNewsEntry( cnt, 2, newsd );
   if( dbUserMainRetrieve( newsd[3], &maind ) < 0 )
    goto iohttpFamNewsL0;
   svSendPrintf( cnt, "%s attacked and took control of the <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a> previously owned by <a href=\"player?id=%lld\">%s</a> of <a href=\"empire?id=%lld\">empire #%lld</a>", mfamd[b].faction, newsd[5], ( newsd[6] >> 8 ) & 0xFF, newsd[6] >> 20, newsd[6] & 0xFF, newsd[3], maind.faction, newsd[4], newsd[4] );
   goto iohttpFamNewsL1;
  }
  else if( newsd[2] == CMD_NEWS_FAMATTACK_FAILED )
  {
   iohttpFamNewsEntry( cnt, 3, newsd );
   if( dbUserMainRetrieve( newsd[3], &maind ) < 0 )
    goto iohttpFamNewsL0;
   svSendPrintf( cnt, "%s attacked and failed to capture the <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a> owned by <a href=\"player?id=%lld\">%s</a> of <a href=\"empire?id=%lld\">empire #%lld</a>", mfamd[b].faction, newsd[5], ( newsd[6] >> 8 ) & 0xFF, newsd[6] >> 20, newsd[6] & 0xFF, newsd[3], maind.faction, newsd[4], newsd[4] );
   goto iohttpFamNewsL1;
  }
  else if( newsd[2] == CMD_NEWS_ATTACK )
  {
   iohttpFamNewsEntry( cnt, 5, newsd );
   if( dbUserMainRetrieve( newsd[3], &maind ) < 0 )
    goto iohttpFamNewsL0;
   svSendPrintf( cnt, "%s lost the <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a> to <a href=\"player?id=%lld\">%s</a> of <a href=\"empire?id=%lld\">empire #%lld</a>", mfamd[b].faction, newsd[5], ( newsd[6] >> 8 ) & 0xFF, newsd[6] >> 20, newsd[6] & 0xFF, newsd[3], maind.faction, newsd[4], newsd[4] );
   goto iohttpFamNewsL1;
  }
  else if( newsd[2] == CMD_NEWS_ATTACK_FAILED )
  {
   iohttpFamNewsEntry( cnt, 4, newsd );
   if( dbUserMainRetrieve( newsd[3], &maind ) < 0 )
    goto iohttpFamNewsL0;
   svSendPrintf( cnt, "The <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a>, owned by %s, was unsuccessfully attacked by <a href=\"player?id=%lld\">%s</a> of <a href=\"empire?id=%lld\">empire #%lld</a>", newsd[5], ( newsd[6] >> 8 ) & 0xFF, newsd[6] >> 20, newsd[6] & 0xFF, mfamd[b].faction, newsd[3], maind.faction, newsd[4], newsd[4] );
   iohttpFamNewsL1:
   svSendString( cnt, "<br>Defender losses : " );
   for( a = b = 0 ; a < CMD_UNIT_FLEET ; a++ )
   {
    if( !( newsd[8+a] ) )
     continue;
    if( b )
     svSendString( cnt, ", " );
    svSendPrintf( cnt, "%lld %s", newsd[8+a], cmdUnitName[a] );
    b = 1;
   }
   if( newsd[8+2*CMD_UNIT_FLEET] )
   {
    if( b )
     svSendString( cnt, ", " );
    svSendPrintf( cnt, "%lld %s", newsd[8+2*CMD_UNIT_FLEET], cmdBuildingName[CMD_BUILDING_SATS] );
    b = 1;
   }
   if( !( b ) )
    svSendString( cnt, "Nothing" );
   svSendString( cnt, "<br>Attacker losses : " );
   for( a = b = 0 ; a < CMD_UNIT_FLEET ; a++ )
   {
    if( !( newsd[8+CMD_UNIT_FLEET+a] ) )
     continue;
    if( b )
     svSendString( cnt, ", " );
    svSendPrintf( cnt, "%lld %s", newsd[8+CMD_UNIT_FLEET+a], cmdUnitName[a] );
    b = 1;
   }
   if( !( b ) )
    svSendString( cnt, "Nothing" );
   if( ( newsd[7] & 0xF00 ) )
   {
    b = newsd[7] & 0xF00;
    svSendString( cnt, "<br>Defending forces preferred to avoid directly engaging enemy units in " );
    if( b == 0x100 )
     svSendString( cnt, "the first phase" );
    else if( b == 0x200 )
     svSendString( cnt, "the second phase" );
    else if( b == 0x300 )
     svSendString( cnt, "the first and second phases" );
    else if( b == 0x400 )
     svSendString( cnt, "the third phase" );
    else if( b == 0x500 )
     svSendString( cnt, "the first and third phases" );
    else if( b == 0x600 )
     svSendString( cnt, "the second and third phases" );
    else if( b == 0x700 )
     svSendString( cnt, "the first, second and third phases" );
    else if( b == 0x800 )
     svSendString( cnt, "the fourth phase" );
    else if( b == 0x900 )
     svSendString( cnt, "the first and fourth phases" );
    else if( b == 0xA00 )
     svSendString( cnt, "the second and fourth phases" );
    else if( b == 0xB00 )
     svSendString( cnt, "the first, second and fourth phases" );
    else if( b == 0xC00 )
     svSendString( cnt, "the third and fourth phases" );
    else if( b == 0xD00 )
     svSendString( cnt, "the first, third and fourth phases" );
    else if( b == 0xE00 )
     svSendString( cnt, "the second, third and fourth phases" );
    else if( b == 0xF00 )
     svSendString( cnt, "all phases" );
    svSendString( cnt, " of the battle" );
   }
   if( ( newsd[7] & 0xFF ) )
   {
    svSendString( cnt, "<br>Overwhelmed by defending enemy forces, the attacking fleet hastidly retreated to minimize losses in the " );
    if( ( newsd[7] & 0x10 ) )
     svSendString( cnt, "first phase of the battle" );
    if( ( newsd[7] & 0x20 ) )
     svSendString( cnt, "second phase of the battle" );
    if( ( newsd[7] & 0x40 ) )
     svSendString( cnt, "third phase of the battle" );
    if( ( newsd[7] & 0x80 ) )
     svSendString( cnt, "fourth phase of the battle" );
   }
  }
  else if( newsd[2] == CMD_NEWS_AID )
  {
   iohttpFamNewsEntry( cnt, 6, newsd );
   if( dbUserMainRetrieve( newsd[3], &maind ) < 0 )
    goto iohttpFamNewsL0;
   svSendPrintf( cnt, "%s received an aid shipment from %s!<br>", mfamd[b].faction, maind.faction );
   for( a = b = 0 ; a < 4 ; a++ )
   {
    if( !( newsd[4+a] ) )
     continue;
    if( b )
     svSendString( cnt, ", " );
    svSendPrintf( cnt, "%lld %s", newsd[4+a], cmdRessourceName[a] );
    b = 1;
   }
   svSendString( cnt, " has been added to the faction reserves." );
  }
  else if( newsd[2] == CMD_NEWS_GETAID )
  {
   iohttpFamNewsEntry( cnt, 7, newsd );
   if( dbUserMainRetrieve( newsd[3], &maind ) < 0 )
    goto iohttpFamNewsL0;
   svSendPrintf( cnt, "%s requested an aid shipment from %s!<br>", maind.faction, mfamd[b].faction );
   for( a = b = 0 ; a < 4 ; a++ )
   {
    if( !( newsd[4+a] ) )
     continue;
    if( b )
     svSendString( cnt, ", " );
    svSendPrintf( cnt, "%lld %s", newsd[4+a], cmdRessourceName[a] );
    b = 1;
   }
   svSendString( cnt, " has been added to the faction reserves." );
  }
  else if( ( newsd[2] >= CMD_NEWS_NUMOPBEGIN ) && ( newsd[2] <= CMD_NEWS_NUMOPEND ) )
  {
   iohttpFamNewsEntry( cnt, -1, newsd );
   svSendPrintf( cnt, "Agents sent by %s reached their destination, the <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a>", mfamd[b].faction, newsd[3], ( newsd[4] >> 8 ) & 0xFFF, newsd[4] >> 20, newsd[4] & 0xFF );
   if( ( user = dbUserLinkID( newsd[5] ) ) )
    svSendPrintf( cnt, " owned by <a href=\"player?id=%lld\">%s</a> of <a href=\"empire?id=%lld\">empire #%lld</a>", newsd[5], user->faction, newsd[6], newsd[6] );
   svSendPrintf( cnt, " to perform <b>%s</b>.", cmdAgentopName[newsd[9]] );
   if( user )
   {
    if( newsd[7] == -1 )
     svSendPrintf( cnt, "<br><i>The agents successfully stayed undiscovered during the operation.</i>" );
    else
     svSendPrintf( cnt, "<br><i>%lld of the agents were caught, but still managed to kill %lld defending agents.</i>", newsd[7], newsd[8] );
   }
  }
  else if( ( newsd[2] >= CMD_NEWS_NUMOPTARGETBEGIN ) && ( newsd[2] <= CMD_NEWS_NUMOPTARGETEND ) )
  {
   iohttpFamNewsEntry( cnt, -1, newsd );
   if( newsd[7] != -1 )
   {
    svSendPrintf( cnt, "The forces of %s intercepted some agents from ", mfamd[b].faction );
    if( ( user = dbUserLinkID( newsd[5] ) ) )
     svSendPrintf( cnt, "<a href=\"player?id=%lld\">%s</a> of <a href=\"empire?id=%lld\">empire #%lld</a>", newsd[5], user->faction, newsd[6], newsd[6] );
    else
     svSendString( cnt, "an unknown faction" );
   }
   else
    svSendPrintf( cnt, "The forces of %s found traces of agents from an unknown faction", mfamd[b].faction );
   svSendPrintf( cnt, " performing a <b>%s</b> operation on <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a>.", cmdAgentopName[newsd[9]], newsd[3], ( newsd[4] >> 8 ) & 0xFFF, newsd[4] >> 20, newsd[4] & 0xFF );
   if( newsd[7] != -1 )
    svSendPrintf( cnt, "<br><i>%lld enemy agents have been arrested, %lld defending agents have been killed.</i>", newsd[7], newsd[8] );
  }
  else if( ( newsd[2] >= CMD_NEWS_NUMSPBEGIN ) && ( newsd[2] <= CMD_NEWS_NUMSPEND ) )
  {
   iohttpFamNewsEntry( cnt, -1, newsd );
   svSendPrintf( cnt, "Psychics of %s casted <b>%s</b> on ", mfamd[b].faction, cmdPsychicopName[newsd[7]] );
   if( newsd[3] == newsd[1] )
    svSendPrintf( cnt, "their faction." );
   else
   {
    if( ( user = dbUserLinkID( newsd[3] ) ) )
     svSendPrintf( cnt, "<a href=\"player?id=%lld\">%s</a> of <a href=\"empire?id=%lld\">empire #%lld</a>.", newsd[3], user->faction, newsd[4], newsd[4] );
    else
     svSendPrintf( cnt, "an unknown faction." );
    if( user )
    {
     if( newsd[5] == -1 )
      svSendPrintf( cnt, "<br><i>The psychics successfully stayed undiscovered while performing the spell.</i>" );
     else
      svSendPrintf( cnt, "<br><i>%lld of the psychics went mad, it appears %lld defending psychics also suffered critical brain damages.</i>", newsd[5], newsd[6] );
    }
   }
  }
  else if( ( newsd[2] >= CMD_NEWS_NUMSPTARGETBEGIN ) && ( newsd[2] <= CMD_NEWS_NUMSPTARGETEND ) )
  {
   iohttpFamNewsEntry( cnt, -1, newsd );
   if( newsd[5] != -1 )
   {
    svSendPrintf( cnt, "The forces of %s felt the influence of psychics from ", mfamd[b].faction );
    if( ( user = dbUserLinkID( newsd[3] ) ) )
     svSendPrintf( cnt, "<a href=\"player?id=%lld\">%s</a> of <a href=\"empire?id=%lld\">empire #%lld</a>", newsd[3], user->faction, newsd[4], newsd[4] );
    else
     svSendString( cnt, "an unknown faction" );
   }
   else
    svSendPrintf( cnt, "The forces of %s felt the influence of psychics from an unknown faction", mfamd[b].faction );
   svSendPrintf( cnt, " casting <b>%s</b>.", cmdPsychicopName[newsd[7]] );
   if( newsd[5] != -1 )
    svSendPrintf( cnt, "<br><i>%lld of the psychics went mad, it appears %lld defending psychics also suffered critical brain damages.</i>", newsd[5], newsd[6] );
  }
  else if( ( newsd[2] >= CMD_NEWS_NUMINBEGIN ) && ( newsd[2] <= CMD_NEWS_NUMINEND ) )
  {
   iohttpFamNewsEntry( cnt, -1, newsd );
   svSendPrintf( cnt, "Ghost ships of %s casted <b>%s</b> ", mfamd[b].faction, cmdGhostopName[newsd[10]] );
   if( cmdGhostopFlags[ newsd[10] ] & 4 )
    svSendPrintf( cnt, "in the system %lld,%lld<br>", ( newsd[4] >> 8 ) & 0xFFF, newsd[4] >> 20 );
   else
   {
    svSendPrintf( cnt, "on the <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a>", newsd[3], ( newsd[4] >> 8 ) & 0xFFF, newsd[4] >> 20, newsd[4] & 0xFF );
    if( ( user = dbUserLinkID( newsd[5] ) ) )
    {
     svSendPrintf( cnt, " owned by <a href=\"player?id=%lld\">%s</a> of <a href=\"empire?id=%lld\">empire #%lld</a><br>", newsd[5], user->faction, newsd[6], newsd[6] );
     if( newsd[7] <= 0 )
      svSendPrintf( cnt, "<i>The ghost ships successfully stayed undiscovered during the incantation.</i><br>" );
     else
      svSendPrintf( cnt, "<i>%lld ghost ships were oblitered by defending forces.</i><br>", newsd[7] );
     if( newsd[8] > 0 )
      svSendPrintf( cnt, "<i>%lld defending psychics were killed.</i><br>", newsd[8] );
     if( newsd[9] > 0 )
      svSendPrintf( cnt, "<i>%lld defending ghost ships were destroyed.</i><br>", newsd[9] );
    }
   }
  }
  else if( ( newsd[2] >= CMD_NEWS_NUMINTARGETBEGIN ) && ( newsd[2] <= CMD_NEWS_NUMINTARGETEND ) )
  {
   iohttpFamNewsEntry( cnt, -1, newsd );
   if( newsd[7] != -1 )
   {
    svSendPrintf( cnt, "The forces of %s were the target of ghost ships from", mfamd[b].faction );
    if( ( user = dbUserLinkID( newsd[5] ) ) )
     svSendPrintf( cnt, " <a href=\"player?id=%lld\">%s</a> of <a href=\"empire?id=%lld\">empire #%lld</a>", newsd[5], user->faction, newsd[6], newsd[6] );
    else
     svSendString( cnt, " an unknown faction" );
   }
   else
    svSendPrintf( cnt, "The forces of %s were the target of ghost ships from an unknown faction", mfamd[b].faction );
   svSendPrintf( cnt, " performing a <b>%s</b> incantation", cmdGhostopName[newsd[10]] );
   if( newsd[3] != -1 )
    svSendPrintf( cnt, " on <a href=\"planet?id=%lld\">planet %lld,%lld:%lld</a>", newsd[3], ( newsd[4] >> 8 ) & 0xFFF, newsd[4] >> 20, newsd[4] & 0xFF );
   svSendString( cnt, ".<br>" );
   if( newsd[7] > 0 )
    svSendPrintf( cnt, "<i>%lld enemy ghost ships were destroyed.</i><br>", newsd[7] );
   if( newsd[8] > 0 )
    svSendPrintf( cnt, "<i>%lld defending psychics were killed.</i><br>", newsd[8] );
   if( newsd[9] > 0 )
    svSendPrintf( cnt, "<i>%lld defending ghost ships were destroyed.</i><br>", newsd[9] );
  }



  else if( newsd[2] == CMD_NEWS_PLANET_TAKEN )
  {
   iohttpFamNewsEntry( cnt, -1, newsd );
   if( ( user = dbUserLinkID( newsd[3] ) ) )
    svSendPrintf( cnt, "The forces of %s took control of a <a href=\"planet?id=%lld\">planet</a> offered by %s.<br>", mfamd[b].faction, newsd[4], user->faction );
  }




  else if( newsd[2] != -1 )
  {
   iohttpFamNewsEntry( cnt, -1, newsd );
   svSendPrintf( cnt, "Unknown report code : %lld ( tell the admin, thanks ;) )", newsd[2] );
  }
  iohttpFamNewsL0:
  svSendString( cnt, "</td></tr>" );
 }
 svSendString( cnt, "</table>" );

 return;
}


void iohttpFunc_hq( svConnectionPtr cnt )
{
 int id, a, num;
 dbUserMainDef maind;
 long long int *newsp, *newsd;
 FILE *file;
 struct stat stdata;
 unsigned char *data;
 unsigned char message[4096];
 unsigned char sMd5[33];
 //FILE *fFile;

	iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 num = dbUserNewsListUpdate( id, &newsp, svTickNum );
 if( !( iohttpHeader( cnt, id, &maind ) ) )
 {
  free( newsp );
  return;
 }
 iohttpBodyInit( cnt, "Headquarters" );
/*
	For Round END script need reuz
	sprintf(message, "echo -n %s%d | md5sum > out", maind.faction, id+9);
	system(message);
	fFile = fopen("out", "rb");
	sMd5[32] = 0;
	fread(sMd5, 16, 1, fFile);
	fread(&(sMd5[16]), 16, 1, fFile);
	system("rm out");
*/
	sMd5[0] = 0;
 svSendPrintf( cnt, "Current date : Week %d, year %d<br>", svTickNum % 52, svTickNum / 52 );
 if( svTickStatus )
  svSendPrintf( cnt, "%d seconds before tick<br>", (int)( svTickTime - time(0) ) );
 else
  svSendPrintf( cnt, "Time frozen<br>" );

 svSendPrintf( cnt, "User <b>%s</b><br>Faction <b>%s</b><br><br>", cnt->dbuser->name, maind.faction );
	
//	if(svRoundEnd)
//		svSendPrintf( cnt, "<a href=\"?page=Vote&id=%d&key=%s\">Click here to vote for the End of Round award</a>", id, sMd5);

 svSendPrintf( cnt, "<table width=\"400\" border=\"0\"><tr><td align=\"center\">Empire : #%d<br>Planets : %d<br>Population : %lld0<br>Networth : %lld</td>", maind.empire, maind.planets, maind.ressource[CMD_RESSOURCE_POPULATION], maind.networth );
 svSendPrintf( cnt, "<td align=\"center\">Fleet readiness : %d%%<br>Psychics readiness : %d%%<br>Agents readiness : %d%%<br>Home planet : %d,%d:%d</td></tr></table><br>", maind.readiness[0] >> 16, maind.readiness[1] >> 16, maind.readiness[2] >> 16, ( maind.home >> 8 ) & 0xFFF, maind.home >> 20, maind.home & 0xFF );

 if( stat( IOHTTP_READ_DIRECTORY "/hq.html", &stdata ) != -1 )
 {
  if( ( data = malloc( stdata.st_size + 1 ) ) )
  {
   data[stdata.st_size] = 0;
   if( ( file = fopen( IOHTTP_READ_DIRECTORY "/hq.html", "rb" ) ) )
   {
    fread( data, 1, stdata.st_size, file );
    svSendString( cnt, data );
    fclose( file );
   }
   free( data );
  }
 }

 if( ( dbEmpireMessageRetrieve( maind.empire, 0, message ) >= 0 ) && ( message[0] ) )
 {
  svSendString( cnt, "<b>Message from your leader</b><br>" );
  svSendString( cnt, message );
  svSendString( cnt, "<br><br>" );
 }

 newsd = newsp;
 if( num )
 {
  svSendString( cnt, "<b>New reports</b>" );
  svSendString( cnt, "<br><a href=\"news\">See older reports</a><br>" );
  svSendString( cnt, "<table><tr><td>" );
  for( a = 0 ; a < num ; a++, newsd += DB_USER_NEWS_BASE )
  {
   iohttpNewsString( cnt, newsd );
  }
  svSendString( cnt, "</td></tr></table>" );
 }
 else
 {
  svSendString( cnt, "<b>No new reports</b>" );
  svSendString( cnt, "<br><a href=\"news\">See older reports</a>" );
 }
 if( newsp )
  free( newsp );

 iohttpBodyEnd( cnt );

 return;
}


void iohttpFunc_news( svConnectionPtr cnt )
{
 volatile int id, a, num;
 dbUserMainDef maind;
 long long int *newsp, *newsd;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Older reports" );

 svSendPrintf( cnt, "Current date : Week %d, year %d<br>", svTickNum % 52, svTickNum / 52 );
 if( svTickStatus )
  svSendPrintf( cnt, "%d seconds before tick<br>", (int)( svTickTime - time(0) ) );
 else
  svSendPrintf( cnt, "Time frozen<br>" );

 num = dbUserNewsList( id, &newsp );
 newsd = newsp;
 if( !( num ) )
  svSendString( cnt, "<b>No reports</b>" );
 else
 {
  svSendString( cnt, "<table><tr><td>" );
  for( a = 0 ; a < num ; a++, newsd += DB_USER_NEWS_BASE )
   iohttpNewsString( cnt, newsd );
  svSendString( cnt, "</td></tr></table>" );
 }
 if( newsp )
  free( newsp );

 iohttpBodyEnd( cnt );

 return;
}


void iohttpFunc_council( svConnectionPtr cnt )
{
 int a, b, c, id, numbuild;
 dbUserBuildPtr build;
 dbUserMainDef maind;
 int sums[16];

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Council" );

 if( ( numbuild = dbUserBuildList( id, &build ) ) < 0 )
 {
  svSendString( cnt, "Error while retriving user build list</body></html>" );
  return;
 }
 svSendString( cnt, "<table width=\"95%\"><tr><td width=\"48%%\" align=\"center\" valign=\"top\"><table>" );

 svSendString( cnt, "<tr><td><b>Energy</b></td><td>&nbsp;</td></tr>" );
 svSendPrintf( cnt, "<tr><td>Production</td><td align=\"right\">+%lld</td></tr>", maind.infos[4] );
 svSendPrintf( cnt, "<tr><td>Decay</td><td align=\"right\">-%lld</td></tr>", maind.infos[5] );
 svSendPrintf( cnt, "<tr><td>Buildings upkeep</td><td align=\"right\">-%lld</td></tr>", maind.infos[6] );
 svSendPrintf( cnt, "<tr><td>Population upkeep reduction</td><td align=\"right\">+%lld</td></tr>", maind.infos[8] );
 svSendPrintf( cnt, "<tr><td>Units upkeep</td><td align=\"right\">-%lld</td></tr>", maind.infos[7] );
 svSendPrintf( cnt, "<tr><td>Portals upkeep</td><td align=\"right\">-%lld</td></tr>", maind.infos[11] );
 svSendPrintf( cnt, "<tr><td>Energy income</td><td align=\"right\">%+lld</td></tr>", maind.infos[CMD_RESSOURCE_ENERGY] );

 svSendString( cnt, "</table><br></td><td width=\"45%%\" align=\"center\" valign=\"top\"><table>" );

 svSendString( cnt, "<tr><td><b>Resources</b></td><td>&nbsp;</td></tr>" );
 svSendPrintf( cnt, "<tr><td>Mineral produced</td><td>+%lld</td></tr>", maind.infos[CMD_RESSOURCE_MINERAL] );
 svSendPrintf( cnt, "<tr><td>Crystal produced</td><td>+%lld</td></tr>", maind.infos[9] );
 svSendPrintf( cnt, "<tr><td>Crystal decayed</td><td>-%lld</td></tr>", maind.infos[10] );
 svSendPrintf( cnt, "<tr><td>Ectrolium produced</td><td>+%lld</td></tr>", maind.infos[CMD_RESSOURCE_ECTROLIUM] );

 svSendString( cnt, "</table><br></td></tr><tr><td align=\"center\" valign=\"top\">" );

 svSendString( cnt, "<b>Buildings</b><br>" );
 for( a = b = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
 {
  svSendPrintf( cnt, "%s : %lld<br>", cmdBuildingName[a], maind.totalbuilding[a] );
  b += (int)maind.totalbuilding[a];
 }
 svSendPrintf( cnt, "Total : %d<br><br>", b );
 svSendString( cnt, "<b>Buildings under construction</b><br><table><form name=\"cancelbuild\" action=\"cancelbuild\">" );
 memset( sums, 0, 16*sizeof(int) );
 svSendString( cnt, "<script language=\"javascript\">function togglemb() { for(i=0;i<document.forms[0].length;i++) if(document.forms[0].elements[i].type == \"checkbox\") document.forms[0].elements[i].click(); }</script>" );
 for( a = c = 0 ; a < numbuild ; a++ )
 {
  if( build[a].type >> 16 )
   continue;
  svSendPrintf( cnt, "<tr><td>%d %s in %d weeks at <a href=\"planet?id=%d\">%d,%d:%d</a></td><td><input type=\"checkbox\" name=\"b%d\"></td></tr>", build[a].quantity, cmdBuildingName[ build[a].type & 0xFFFF ], build[a].time, build[a].plnid, ( build[a].plnpos >> 8 ) & 0xFFF, build[a].plnpos >> 20, build[a].plnpos & 0xFF , a);
  sums[ build[a].type & 0xFFFF ] += build[a].quantity;
  c++;
 }
 if( !( c ) )
  svSendString( cnt, "</form></table>None<br>" );
 else
 {
 	svSendString(cnt, "<tr><td></td><td><a href=\"javascript:togglemb()\">Toggle</font></a></td></tr>");
 	svSendString(cnt, "<tr><td></td><td><input type=\"submit\" value=\"Cancel\"></td></tr></form></table>");
  svSendString( cnt, "<br><i>Summary</i><br>" );
  for( a = b = 0 ; a < CMD_BLDG_NUMUSED+1 ; a++ )
  {
   if( !( sums[a] ) )
    continue;
   svSendPrintf( cnt, "%d %s<br>", sums[a], cmdBuildingName[a] );
   b += sums[a];
  }
  svSendPrintf( cnt, "<i>Total of %d buildings under construction</i><br>", b );
 }

 svSendString( cnt, "</td><td align=\"center\" valign=\"top\">" );

 svSendString( cnt, "<b>Units</b><br>" );
 for( a = b = 0 ; a < CMD_UNIT_NUMUSED ; a++ )
 {
  svSendPrintf( cnt, "%s : %lld <br>", cmdUnitName[a], maind.totalunit[a] );
  b += (int)maind.totalunit[a];
 }
 svSendPrintf( cnt, "Total : %d<br><br>", b );
 svSendString( cnt, "<b>Units under construction</b><br><table><form name=\"cancelunit\" action=\"cancelbuild\">" );
 svSendString( cnt, "<script language=\"javascript\">function togglem() { for(i=0;i<document.forms[1].length;i++) if(document.forms[1].elements[i].type == \"checkbox\") document.forms[1].elements[i].click(); }</script>" );
 
 memset( sums, 0, 16*sizeof(int) );
 for( a = c = 0 ; a < numbuild ; a++ )
 {
  if( !( build[a].type >> 16 ) )
   continue;
  svSendPrintf( cnt, "<tr><td>%d %s in %d weeks</td><td><input type=\"checkbox\" name=\"b%d\"></td></tr>", build[a].quantity, cmdUnitName[ build[a].type & 0xFFFF ], build[a].time, a);
  sums[ build[a].type & 0xFFFF ] += build[a].quantity;
  c++;
 }
 if( !( c ) )
  svSendString( cnt, "</form></table>None<br>" );
 else
 {
  svSendString(cnt, "<tr><td></td><td><a href=\"javascript:togglem()\">Toggle</font></a></td></tr>");
  svSendString( cnt, "<tr><td></td><td><input type=\"submit\" value=\"Cancel\"></td></tr></form></table><br><i>Summary</i><br>" );
  for( a = b = 0 ; a < CMD_UNIT_NUMUSED ; a++ )
  {
   if( !( sums[a] ) )
    continue;
   svSendPrintf( cnt, "%d %s<br>", sums[a], cmdUnitName[a] );
   b += sums[a];
  }
  svSendPrintf( cnt, "<i>Total of %d units under construction</i><br>", b );
 }

 svSendString( cnt, "</td></tr></table>" );

 free( build );
 iohttpBodyEnd( cnt );
 return;
}



void iohttpFunc_units( svConnectionPtr cnt )
{
 int a, b, c, id;
 long long int resbuild[CMD_RESSOURCE_NUMUSED+2];
 dbUserMainDef maind;
 unsigned char *buildunit[CMD_UNIT_NUMUSED];
 unsigned char buildname[16];
 unsigned char buildstring[CMD_UNIT_NUMUSED][128];

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;

 iohttpVarsInit( cnt );
 for( a = 0 ; a < CMD_UNIT_NUMUSED ; a++ )
 {
  sprintf( buildname, "unit%d", a );
  buildunit[a] = iohttpVarsFind( buildname );
 }
 iohttpVarsCut();

 for( a = 0 ; a < CMD_UNIT_NUMUSED ; a++ )
 {
  buildstring[a][0] = 0;
  if( !( buildunit[a] ) )
   continue;
  if( sscanf( buildunit[a], "%d", &b ) <= 0 )
   continue;
  if( b <= 0 )
   continue;
  c = cmdExecAddBuild( id, 0x10000 | a, b, -1, 0 );
  if( c >= 0 )
   sprintf( buildstring[a], "<i>You started building %d %s</i><br><br>", b, cmdUnitName[a] );
  else if( cmdErrorString )
   sprintf( buildstring[a], "<i>%s</i><br><br>", cmdErrorString );
  else
   sprintf( buildstring[a], "<i>Error while adding build item</i><br>" );
 }

 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Units" );

 for( a = 0 ; a < CMD_UNIT_NUMUSED ; a++ )
 {
  if( buildstring[a][0] )
   svSendString( cnt, buildstring[a] );
 }

 svSendString( cnt, "<form action=\"units\" method=\"POST\"><table cellspacing=\"6\" border=\"0\"><tr><td><b>Unit</b></td><td><b>Cost</b></td><td><b>Owned</b></td><td><b>Build</b></td></tr>" );
 for( a = 0 ; a < CMD_UNIT_NUMUSED ; a++ )
 {
  cmdGetBuildCosts( &maind, 0x10000 | a, resbuild );
  if( !( maind.totalunit[a] ) && ( resbuild[0] == -2 ) )
   continue;
  svSendPrintf( cnt, "<tr><td valign=\"top\"><font color=\"#FFFFFF\">%s</font><br>", cmdUnitName[a] );

  // units pictures, we have them all hosted by blueD
  
  svSendString( cnt, "</td><td valign=\"top\" nowrap>" );

  if( resbuild[0] < 0 )
  {
   if( resbuild[0] == -1 )
    svSendPrintf( cnt, "<font color=\"#FF0000\">Unavailable<br>Tech too low</font></td><td valign=\"top\" align=\"center\">%lld</td><td>&nbsp;</td></tr>", maind.totalunit[a] );
   else if( resbuild[0] == -2 )
    svSendPrintf( cnt, "<font color=\"#FF0000\">Unavailable<br>Can not be built</font></td><td valign=\"top\" align=\"center\">%lld</td><td>&nbsp;</td></tr>", maind.totalunit[a] );
   continue;
  }
  for( b = 0 ; b < CMD_RESSOURCE_NUMUSED+1 ; b++ )
  {
   if( resbuild[b] )
    svSendPrintf( cnt, "%lld %s<br>", resbuild[b], cmdRessourceName[b] );
  }
  if( resbuild[b] )
   svSendPrintf( cnt, "<font color=\"#FF2020\">Tech : +%lld%% costs</font><br>", resbuild[b] );
  svSendPrintf( cnt, "</td><td valign=\"top\" align=\"center\">%lld</td><td valign=\"top\"><input type=\"text\" name=\"unit%d\" size=\"10\"></td></tr>", maind.totalunit[a], a );
 }
 svSendString( cnt, "</table><input type=\"submit\" value=\"Build\"></form>" );

 iohttpBodyEnd( cnt );
 return;
}



void iohttpFunc_market( svConnectionPtr cnt )
{
 int a, b, c, d, id, fullmarket[6*DB_MARKET_RANGE], rmarket[12];
 int action, resource, price, quantity;
 int *buffer;
 dbUserMainDef maind;
 unsigned char *pricestring, *quantitystring, *resstring, *actionstring, *viewstring, *fullstring, *rmbidstring;
 unsigned char *marketstring;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;

 iohttpVarsInit( cnt );
 pricestring = iohttpVarsFind( "price" );
 quantitystring = iohttpVarsFind( "quantity" );
 actionstring = iohttpVarsFind( "action" );
 resstring = iohttpVarsFind( "res" );
 viewstring = iohttpVarsFind( "view" );
 fullstring = iohttpVarsFind( "full" );
 rmbidstring = iohttpVarsFind( "rmbid" );
 iohttpVarsCut();
	
	/****
	svSendString(cnt, "Market is close<br>But you can have ally");
	iohttpBodyEnd( cnt );
	return;
	//****/
	
 marketstring = 0;
 if( resstring )
 {
  if( !( actionstring ) )
   marketstring = "You must select an action to place a bid, either buy or sell.";
  else if( ( sscanf( actionstring, "%d", &action ) == 1 ) && ( resstring ) && ( sscanf( resstring, "%d", &resource ) == 1 ) && ( pricestring ) && ( sscanf( pricestring, "%d", &price ) == 1 ) && ( quantitystring ) && ( sscanf( quantitystring, "%d", &quantity ) == 1 ) )
  {
   if( cmdExecAddBid( id, action, resource, price, quantity ) < 0 )
   {
    if( cmdErrorString )
     marketstring = cmdErrorString;
    else
     marketstring = "Error while adding build item";
   }
   else
    marketstring = "Bid placed";
  }
  else
   marketstring = "Incorrect request.";
 }

 if( rmbidstring )
 {
  if( sscanf( rmbidstring, "%d", &a ) == 1 )
  {
   if( cmdExecRemoveBid( id, a ) < 0 )
    marketstring = "Error while removing bid";
   else
    marketstring = "Bid withdrawn";
   viewstring = (void *)1;
  }
 }

 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Market" );
 if( marketstring )
  svSendPrintf( cnt, "<i>%s</i><br><br>", marketstring );

 if( viewstring )
 {
  b = dbUserMarketList( id, &buffer );
  svSendString( cnt, "<b>Your bids</b><br>" );
  if( b <= 0 )
   svSendString( cnt, "None<br>" );
  for( a = c = 0 ; a < b ; a++, c += 5 )
  {
   if( !( buffer[c+DB_MARKETBID_ACTION] ) )
    svSendString( cnt, "Buying" );
   else
    svSendString( cnt, "Selling" );
   svSendPrintf( cnt, " %d %s at %d - <a href=\"market?rmbid=%d\">Withdraw bid</a><br>", buffer[c+DB_MARKETBID_QUANTITY], cmdRessourceName[buffer[c+DB_MARKETBID_RESSOURCE]+1], buffer[c+DB_MARKETBID_PRICE], buffer[c+DB_MARKETBID_BIDID] );
  }
  svSendString( cnt, "<br>" );
  if( buffer )
   free( buffer );
 }

 svSendString( cnt, "<a href=\"market?view=1\">View your bids</a><br>" );
 svSendString( cnt, "<a href=\"market?full=1\">View market statistics</a><br><br>" );

 if( !( fullstring ) )
 {
  if( cmdExecGetMarket( rmarket ) < 0 )
  {
   svSendString( cnt, "Error while retriving market" );
   iohttpBodyEnd( cnt );
   return;
  }
  svSendString( cnt, "<table width=\"400\"><tr><td width=\"150\">&nbsp;</td><td width=\"60\" align=\"center\">Buy</td><td width=\"60\" align=\"center\">Needed</td><td width=\"10\">&nbsp;</td><td width=\"60\" align=\"center\">Sell</td><td width=\"60\" align=\"center\">Stock</td></tr>" );
  for( a = 0, b = 1 ; a < 3*4 ; a += 4, b++ )
  {
   svSendPrintf( cnt, "<tr><td>%s</td><td align=\"center\">", cmdRessourceName[b] );
   if( rmarket[a+0] >= 0 )
    svSendPrintf( cnt, "%d</td><td align=\"center\">%d", rmarket[a+1], rmarket[a+0] );
   else
    svSendString( cnt, "-</td><td align=\"center\">-" );
   svSendString( cnt, "</td><td>...</td><td align=\"center\">" );
   if( rmarket[a+2] >= 0 )
    svSendPrintf( cnt, "%d</td><td align=\"center\">%d</td>", rmarket[a+3], rmarket[a+2] );
   else
    svSendString( cnt, "-</td><td align=\"center\">-</td>" );
   svSendString( cnt, "</tr>" );
  }
  svSendPrintf( cnt, "</table>" );
 }
 else
 {
  if( dbMarketFull( fullmarket ) < 0 )
  {
   svSendString( cnt, "Error while retriving market" );
   iohttpBodyEnd( cnt );
   return;
  }
  for( b = c = 0 ; b < 3*2 ; b += 2 )
  {
   rmarket[b+0] = -1;
   rmarket[b+1] = -1;
   for( a = DB_MARKET_RANGE-1 ; a >= 1 ; a-- )
   {
    if( fullmarket[c+a] )
    {
     rmarket[b+0] = a;
     break;
    }
   }
   c += DB_MARKET_RANGE;
   for( a = 1 ; a < DB_MARKET_RANGE ; a++ )
   {
    if( fullmarket[c+a] )
    {
     rmarket[b+1] = a;
     break;
    }
   }
   c += DB_MARKET_RANGE;
  }
  for( c = d = 0 ; c < 3 ; c++, d += 2 )
  {
   svSendPrintf( cnt, "<b>Buying %s</b><br>", cmdRessourceName[c+1] );
   if( rmarket[d+0] == -1 )
    svSendString( cnt, "No bids<br><br>" );
   else
   {
    svSendString( cnt, "<table cellspacing=\"6\" cellpading=\"6\"><tr align=\"center\"><td align=\"right\">Price</td>" );
    b = rmarket[d+0] - 10;
    if( b <= 0 )
     b = 1;
    for( a = b ; a <= rmarket[d+0] ; a++ )
     svSendPrintf( cnt, "<td>%d</td>", a );
    svSendString( cnt, "</tr><tr align=\"center\"><td align=\"right\">Needed</td>" );
    for( a = b ; a <= rmarket[d+0] ; a++ )
     svSendPrintf( cnt, "<td>%d</td>", fullmarket[(d+0)*DB_MARKET_RANGE+a] );
    svSendString( cnt, "</tr></table><br>" );
   }
   svSendPrintf( cnt, "<b>Selling %s</b><br>", cmdRessourceName[c+1] );
   if( rmarket[d+1] == -1 )
    svSendString( cnt, "No bids<br><br>" );
   else
   {
    svSendString( cnt, "<table cellspacing=\"6\" cellpading=\"6\"><tr align=\"center\"><td align=\"right\">Price</td>" );
    b = rmarket[d+1] + 10;
    if( b >= DB_MARKET_RANGE )
     b = DB_MARKET_RANGE-1;
    for( a = rmarket[d+1] ; a <= b ; a++ )
     svSendPrintf( cnt, "<td>%d</td>", a );
    svSendString( cnt, "</tr><tr align=\"center\"><td align=\"right\">Stock</td>" );
    for( a = rmarket[d+1] ; a <= b ; a++ )
     svSendPrintf( cnt, "<td>%d</td>", fullmarket[(d+1)*DB_MARKET_RANGE+a] );
    svSendString( cnt, "</tr></table><br>" );
   }
  }
 }

 svSendString( cnt, "<form action=\"market\" method=\"POST\"><table cellspacing=\"4\" border=\"0\"><tr><td><input type=\"radio\" value=\"0\" name=\"action\">Buy<br><input type=\"radio\" value=\"1\" name=\"action\">Sell</td><td><select name=\"res\">" );
 for( a = 0 ; a < 3 ; a++ )
  svSendPrintf( cnt, "<option value=\"%d\">%s", a, cmdRessourceName[a+1] );
 svSendString( cnt, "</select></td></tr><tr><td align=\"right\">Price :</td><td><input type=\"text\" name=\"price\" size=\"10\"></td></tr><tr><td>Quantity :</td><td><input type=\"text\" name=\"quantity\" size=\"10\"></td></tr></table><input type=\"submit\" value=\"Execute\"></form>" );
 svSendString( cnt, "<br><i>Note that resources placed on the market still decay, including energy.</i>" );

 iohttpBodyEnd( cnt );
 return;
}



void iohttpFunc_planets( svConnectionPtr cnt )
{
 int a, b, c, d, id, sort;
 char szColor[10];
 float fa;
 int *buffer;
 dbUserMainDef maind;
 dbMainPlanetDef planetd;
 int totals[7];
 float totalob;
 unsigned char *sortstring;


 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Planets" );


 iohttpVarsInit( cnt );
 sortstring = iohttpVarsFind( "sort" );
 iohttpVarsCut();

 if( !( sortstring ) || ( sscanf( sortstring, "%d", &sort ) <= 0 ) )
  sort = 0;
 // printf("hier gaan we naar de sort\n");
 if( ( b = dbUserPlanetListIndicesSorted( id, &buffer, sort ) ) <= 0 )
  {
   svSendString( cnt, "Error while retriving planets list" );
   return;
  }
/*
 CODE VOOR DE SORTS ETC ETC WERKEND FOR SURE

   svSendString( cnt, "<script language=\"javascript\">function togglemb() { for(i=0;i<document.forms[0].length;i++) if(document.forms[0].elements[i].type == \"checkbox\") document.forms[0].elements[i].click(); }</script>" );
   svSendString( cnt, "<form action=\"massbuild\" method=\"POST\">");
 
 if (sort == 0)
  svSendString( cnt, "<table width=\"100%\"><tr><td><a href=\"/planets?sort=10\">Planet</a></td>");
 else
  svSendString( cnt, "<table width=\"100%\"><tr><td><a href=\"/planets?sort=0\">Planet</a></td>");
 if (sort == 1)
  svSendString( cnt, "<td><a href=\"/planets?sort=11\">Size</a> ");
 else
  svSendString( cnt, "<td><a href=\"/planets?sort=1\">Size</a> ");
 if (sort == 6)
  svSendString( cnt, "<a href=\"/planets?sort=16\">Bonus</a></td>");
 else
  svSendString( cnt, "<a href=\"/planets?sort=6\">Bonus</a></td>");
 if (sort == 2)
  svSendString( cnt, "<td><a href=\"/planets?sort=12\">Buildings</a>");
 else
  svSendString( cnt, "<td><a href=\"/planets?sort=2\">Buildings</a>");
 if (sort == 3)
  svSendString( cnt, " - <a href=\"/planets?sort=13\">Overbuilding</a></td>");
 else
  svSendString( cnt, " - <a href=\"/planets?sort=3\">Overbuilding</a></td>");

 if (sort == 4)
  svSendString( cnt, "<td><a href=\"/planets?sort=14\">Population</a></td>");
 else
  svSendString( cnt, "<td><a href=\"/planets?sort=4\">Population</a></td>");
    
 if (sort == 5)
  svSendString( cnt, "<td><a href=\"/planets?sort=15\">Protection</a></td>");	
 else
  svSendString( cnt, "<td><a href=\"/planets?sort=5\">Protection</a></td>");
 
 svSendString( cnt, "<td>Build</td>");
 svSendString( cnt, "<td width=\"2%\"><a href=\"javascript:togglemb()\"><font size=\"1\">All</font></a></td></tr>" );
*/
 svSendString( cnt, "<script language=\"javascript\">function togglemb() { for(i=0;i<document.forms[0].length;i++) if(document.forms[0].elements[i].type == \"checkbox\") document.forms[0].elements[i].click(); }</script>" );
 svSendString( cnt, "<form action=\"massbuild\" method=\"POST\">");

if (sort == 0)
 svSendString( cnt, "<table width=\"100%%\"><tr><td width=\"15%%\"><a href=\"/planets?sort=10\">Planet</a></td>");
else
 svSendString( cnt, "<table width=\"100%%\"><tr><td width=\"15%%\"><a href=\"/planets?sort=0\">Planet</a></td>");

if ( ( sort == 1 ) || ( sort == 6 ) )
 svSendString( cnt, "<td width=\"10%%\"><a href=\"/planets?sort=11\">Size</a> - <a href=\"/planets?sort=16\">Bonus</a></td>");
else
 svSendString( cnt, "<td width=\"10%%\"><a href=\"/planets?sort=1\">Size</a> - <a href=\"/planets?sort=6\">Bonus</a></td>");

if ( ( sort == 2 || sort == 3 ) )
 svSendString( cnt, "<td width=\"20%%\"><a href=\"/planets?sort=12\">Buildings</a> - <a href=\"/planets?sort=13\">Overbuilding</a></td>");
else
 svSendString( cnt, "<td width=\"20%%\"><a href=\"/planets?sort=2\">Buildings</a> - <a href=\"/planets?sort=3\">Overbuilding</a></td>");

if (sort == 4)
 svSendString( cnt, "<td width=\"25%%\"><a href=\"/planets?sort=14\">Population</a></td>");
else
 svSendString( cnt, "<td width=\"25%%\"><a href=\"/planets?sort=4\">Population</a></td>");

if (sort == 5)
 svSendString( cnt, "<td width=\"28%\">Build - <a href=\"/planets?sort=15\">Protection</a></td>");
else
 svSendString( cnt, "<td width=\"28%\">Build - <a href=\"/planets?sort=5\">Protection</a></td>");

 svSendString( cnt, "<td width=\"2%\"><a href=\"javascript:togglemb()\"><font size=\"1\">Toggle</font></a></td></tr>" );



 memset( totals, 0, 7*sizeof(int) );
 totalob = 0.0;

 for( a = 0 ; a < b ; a++ )
 {
 	dbMapRetrievePlanet( buffer[a], &planetd );
  for( c = d = 0 ; c < CMD_BLDG_NUMUSED ; c++ )
   d += planetd.building[c];
  if(planetd.flags & CMD_PLANET_FLAGS_BEACON)
  	svSendPrintf( cnt, "<tr><td><a href=\"planet?id=%d\">%d,%d:%d</a><img src=\"beacon.jpg\"></td><td>%d</td><td>%d", buffer[a], ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, planetd.position & 0xFF, planetd.size, d );
  else
  	svSendPrintf( cnt, "<tr><td><a href=\"planet?id=%d\">%d,%d:%d</a></td><td>%d</td><td>%d", buffer[a], ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, planetd.position & 0xFF, planetd.size, d );
  if( planetd.construction )
   svSendPrintf( cnt, " ( %d )", planetd.construction );
  for( c = CMD_BLDG_OVERBUILD ; c < CMD_BLDG_NUMUSED ; c++ )
   d -= planetd.building[c];
  fa = cmdGetBuildOvercost( planetd.size, d+planetd.construction );
  if( fa > 1.0 )
   svSendPrintf( cnt, " <font color=\"#FF2020\">+%.2f%%</font>", 100.0 * (fa-1.0) );
  totalob += fa;
  totals[0] += planetd.size;
  totals[1] += d;
  totals[2] += planetd.construction;
  sprintf(szColor, "C0D0D8");
  if((planetd.population*10/planetd.maxpopulation)>=8)
  {
  	sprintf(szColor, "6699FF");
  }
  if((planetd.population/planetd.maxpopulation)==1)
  {
  	sprintf(szColor, "FFFF66");
  }
  svSendPrintf( cnt, "</td><td><font color=\"#%s\">%d0 / %d0</font></td><td><a href=\"build?id=%d\">Build</a>", szColor, planetd.population, planetd.maxpopulation, buffer[a] );
  if( planetd.flags & CMD_PLANET_FLAGS_PORTAL )
  {
   svSendString( cnt, " Portal" );
   totals[5]++;
  }
  if( planetd.protection >= 70 )
   svSendString( cnt, " <font color=\"#00FF00" );
  else if( planetd.protection >= 40 )
   svSendString( cnt, " <font color=\"#FFFF00" );
  else
   svSendString( cnt, " <font color=\"#FF0000" );
  svSendPrintf( cnt, "\">- %d%%</font>", planetd.protection );
  for( c = d = 0 ; c < CMD_UNIT_NUMUSED ; c++ )
   d += planetd.unit[c];
  if( d )
  {
   svSendPrintf( cnt, " <i>Units (%d)</i>", d );
   totals[6] += d;
  }

  d = (int)artefactPrecense( &planetd );
  if( d >= 0 )
   svSendPrintf( cnt, " <img src=\"%s\">", artefactImage[d] );
  else if(planetd.special[1])
  	svSendPrintf( cnt, " <img src=\"pr%d.gif\">+%d%%", planetd.special[0], planetd.special[1] );

  svSendPrintf( cnt, "</td><td align=\"center\"><input type=\"checkbox\" name=\"m%d\"></td></tr>", buffer[a] );
  totals[3] += planetd.population;
  totals[4] += planetd.maxpopulation;
 }
// printf("nog steeds in iohttp\n");
 svSendPrintf( cnt, "<tr><td>%d Planets</td><td>%d</td><td>%d", b, totals[0], totals[1] );
 if( totals[2] )
  svSendPrintf( cnt, " ( %d )", totals[2] );
 totalob /= (float)b;
 if( totalob > 1.001 )
  svSendPrintf( cnt, " <font color=\"#FF2020\">+%.2f%%</font>", 100.0 * (totalob-1.0) );
// printf("nog wa verder in iohttp\n");
//Total
	sprintf(szColor, "FFFFFF");
 if((totals[3]*10/totals[4])>=8)
 {
 	sprintf(szColor, "FFFF00");
 }
 if((totals[3]/totals[4])==1)
 {
 	sprintf(szColor, "FF2020");
 }
 svSendPrintf( cnt, "</td><td><font color=\"#%s\">%d0 / %d0</font></td><td> %d Portals", szColor, totals[3], totals[4], totals[5] );
 if( totals[6] )
  svSendPrintf( cnt, " <i>Units (%d)</i>", totals[6] );
 svSendString( cnt, "</td><td></td></tr>" );
 svSendString( cnt, "</table><br><div align=\"right\"><input type=\"submit\" value=\"Build on selected planets\"></div></form>" );
// printf("nu nog het einde van iohttp\n");
 free( buffer );
 iohttpBodyEnd( cnt );
// printf("hier komt het einde van iohttp\n");
 return;
}



void iohttpFunc_empire( svConnectionPtr cnt )
{
 int a, b, c, d, nAlly, first, id, curtime, curfam;
 dbUserMainDef maind;
 dbMainEmpireDef empired;
 unsigned char *empirestring;
 unsigned char fname[64];
 dbUserPtr user;
 dbUserMainDef mainp[32];
 int stats[64];

 iohttpBase( cnt, 1 );
 maind.empire = -1;
 if( ( id = iohttpIdentify( cnt, 2 ) ) >= 0 )
 {
  if( !( iohttpHeader( cnt, id, &maind ) ) )
   return;
 }

 iohttpVarsInit( cnt );
 empirestring = iohttpVarsFind( "id" );
 iohttpVarsCut();

 if( !( empirestring ) || ( sscanf( empirestring, "%d", &curfam ) <= 0 ) )
  curfam = maind.empire;

 if( dbMapRetrieveEmpire( curfam, &empired ) < 0 )
 {
  svSendString( cnt, "This empire does not seem to exist!</body></html>" );
  return;
 }

 if( empired.name[0] )
  iohttpBodyInit( cnt, "%s #%d ( <a href=\"system?id=%d\">%d,%d</a> )", empired.name, curfam, empired.homeid, empired.homepos & 0xFFFF, empired.homepos >> 16 );
 else
  iohttpBodyInit( cnt, "Empire #%d ( <a href=\"system?id=%d\">%d,%d</a> )", curfam, empired.homeid, empired.homepos & 0xFFFF, empired.homepos >> 16 );

 if( curfam == maind.empire )
  svSendPrintf( cnt, "<a href=\"/forum?forum=%d\">Empire Forum</a>", curfam + 100 );
 if( curfam == maind.empire )
  svSendPrintf( cnt, " <a href=\"famvote\">Change vote</a> <a href=\"famaid\">Send aid</a> <a href=\"famgetaid\">Receive aid</a> <a href=\"famnews\">Empire news</a> <a href=\"famrels\">Empire relations</a>" );
 if( ( id != -1 ) && ( id == empired.leader ) )
  svSendPrintf( cnt, " <a href=\"famleader\">Leader options</a>" );
 if( curfam == maind.empire )
  svSendPrintf( cnt, "<br>" );

 sprintf( fname, "/fampic%02d%d", curfam, empired.pictime );
 if( iohttpFileFind( fname ) )
  svSendPrintf( cnt, "<br><img src=\"%s\"><br>", &fname[1] );

 if( !( empired.numplayers ) )
 {
  iohttpBodyEnd( cnt );
  return;
 }

 for( a = 0 ; a < empired.numplayers ; a++ )
 {
  if( dbUserMainRetrieve( empired.player[a], &mainp[a] ) < 0 )
  {
   svSendString( cnt, "Error while retriving user's main data" );
   continue;
  }
 }
 first = 0;
 stats[0] = -1;
 stats[1] = -1;
 for( b = 1, c = 2 ; b < empired.numplayers ; b++, c += 2 )
 {
  a = first >> 1;
  if( ( mainp[b].planets > mainp[a].planets ) || ( ( mainp[b].planets == mainp[a].planets ) && ( mainp[b].networth > mainp[a].networth ) ) )
  {
   stats[first+0] = c;
   stats[c+1] = first;
   stats[c+0] = -1;
   first = c;
   continue;
  }
  d = first;
  a = stats[d+1];
  for( ; ; )
  {
   if( a == -1 )
   {
    stats[d+1] = c;
    stats[c+0] = d;
    stats[c+1] = -1;
    break;
   }
   if( ( mainp[b].planets > mainp[a>>1].planets ) || ( ( mainp[b].planets == mainp[a>>1].planets ) && ( mainp[b].networth > mainp[a>>1].networth ) ) )
   {
    stats[c+0] = d;
    stats[c+1] = a;
    stats[d+1] = c;
    stats[a+0] = c;
    break;
   }
   d = a;
   a = stats[a+1];
  }
 }

 svSendString( cnt, "<table cellspacing=\"8\"><tr><td>&nbsp;</td><td><b>Empire factions</b></td><td>Race</td><td>Planets</td><td>Networth</td><td></td></tr>" );
 curtime = time( 0 );
 for( a = first ; a != -1 ; )
 {
  b = a >> 1;
  c = empired.player[b];
  user = dbUserLinkID( c );
  user->flags |= CMD_USER_FLAGS_ACTIVATED;
  dbUserSave( c, user);
  svSendString( cnt, "<tr>" );
  if( !( user ) || !( user->flags & ( CMD_USER_FLAGS_LEADER | CMD_USER_FLAGS_VICELEADER | CMD_USER_FLAGS_INDEPENDENT ) ) )
   svSendString( cnt, "<td>&nbsp;</td>" );
  else if( user->flags & CMD_USER_FLAGS_LEADER )
   svSendString( cnt, "<td><i>Leader</i></td>" );
  else if( user->flags & CMD_USER_FLAGS_VICELEADER )
   svSendString( cnt, "<td><i>Vice-leader</i></td>" );
  else
   svSendString( cnt, "<td><i>Independent</i></td>" );
  svSendPrintf( cnt, "<td><a href=\"player?id=%d\">", c );
  if( empired.leader == c )
   svSendPrintf( cnt, "<font color=\"#FFC040\"><b>%s</b></font>", mainp[b].faction );
  else
   svSendString( cnt, mainp[b].faction );
  svSendPrintf( cnt, "</a></td><td><a href=\"races\">%s</a></td><td>%d</td><td>%lld</td><td>", cmdRaceName[mainp[b].raceid], mainp[b].planets, mainp[b].networth );
  nAlly = cmdExecFindRelation(maind.empire, curfam, 0, 0);
  
  if( ( id >= 0 ) && ( user ) && ( ( curfam == maind.empire ) || (nAlly == CMD_RELATION_ALLY) || ( ( cnt->dbuser ) && ( cnt->dbuser->level >= LEVEL_MODERATOR ) ) ) )
  {
  	
   b = curtime - user->lasttime;
   if( b < 5*60 )
    svSendString( cnt, "[online]" );
   else if( ( cnt->dbuser->flags & ( CMD_USER_FLAGS_LEADER | CMD_USER_FLAGS_VICELEADER ) ) || ( cnt->dbuser->level >= LEVEL_MODERATOR ) )
   {
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
     svSendPrintf( cnt, "%dm ", b/60 );
   }
  }
  svSendString( cnt, "</td></tr>" );
  a = stats[a+1];
 }
 svSendString( cnt, "</table><br>" );


/*
 if( empired.artefacts )
 {
  svSendString( cnt, "<table border=\"0\"><tr><td>" );
  for( a = 0, b = 1 ; a < ARTEFACT_NUMUSED ; a++, b <<= 1 )
  {
   if( empired.artefacts & b )
    svSendPrintf( cnt, "<img src=\"%s\"> %s<br>", artefactImage[a], artefactDescription[a] );
  }
  svSendString( cnt, "</td></tr></table>" );
 }
*/
 if( empired.artefacts )
 {
 	if( ( id >= 0 ) && ( ( curfam == maind.empire ) || ( ( cnt->dbuser ) && ( cnt->dbuser->level >= LEVEL_MODERATOR ) ) ) )
   c = ARTEFACT_NUMUSED;
  else
  {
   c = 0;
   for( a = 0, b = 1 ; a < ARTEFACT_NUMUSED ; a++, b <<= 1 )
   {
    if( empired.artefacts & b )
     c++;
   }
  }
  if( ( 3*c >= ARTEFACT_NUMUSED ) || ( (3*dbArtefactMax)/2 >= ARTEFACT_NUMUSED ) )
  {
   svSendString( cnt, "<table border=\"0\"><tr><td>" );
   for( a = 0, b = 1 ; a < ARTEFACT_NUMUSED ; a++, b <<= 1 )
   {
   	if( empired.artefacts & b )
   	{
   		svSendPrintf( cnt, "<img src=\"%s\"> %s<br>", artefactImage[a], artefactDescription[a] );
   	}
   }
   svSendString( cnt, "</td></tr></table>" );
  }
 }
 if( curfam == maind.empire )
  svSendString( cnt, "<br>Empire members are marked online if a page was requested in the last 5 minutes." );	

 iohttpBodyEnd( cnt );
 return;
}

void iohttpFunc_famaid( svConnectionPtr cnt )
{
 int a, b, id, res[4], j, i, k, nAlly;
 int *rel;
 dbUserMainDef maind;
 dbMainEmpireDef empired;
 dbMainEmpireDef empire2d;
 unsigned char *playerstring, *resstring[4];
 unsigned char *reportstring;
 dbUserPtr user;

  iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;

 iohttpVarsInit( cnt );
 playerstring = iohttpVarsFind( "id" );
 resstring[0] = iohttpVarsFind( "r0" );
 resstring[1] = iohttpVarsFind( "r1" );
 resstring[2] = iohttpVarsFind( "r2" );
 resstring[3] = iohttpVarsFind( "r3" );
 iohttpVarsCut();

	if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Send Aid" );
	
	//Find ally (Both way alliance)
// nAlly = -1;
// if( ( j = dbEmpireRelsList( maind.empire, &rel ) ) >= 0 )
// {
 	//printf("%d\n",j);
// 	for( i = 0 ; i < j*4 ; i += 4 )
//  {
  	//printf("%d %d %d %d\n", rel[i], rel[i+1], rel[i+2], rel[i+3]);
//   if( ( rel[i+3] & 1 ) || ( rel[i+1] != CMD_RELATION_ALLY ) )
//    continue;
//   for( k = 0 ; k < j*4 ; k += 4 )
//   {
   	//printf("checking %d and %d\n", rel[i+2] ,rel[k+2]);
//    if( !( rel[k+3] & 1 ) || ( rel[k+1] != CMD_RELATION_ALLY ) || ( rel[i+2] != rel[k+2] ) )
//     continue;
    //Found the ally if we reach this line 
//    nAlly = rel[i+2];
//    dbMapRetrieveEmpire( nAlly, &empire2d );
    //printf("found one %d\n", nAlly);
//    j = dbEmpireRelsList( maind.empire, &rel );
    //empire2d is the ally so list them in the trade box and allow trade to them
//  		}
//  }
//  free( rel );
// }
 //Find ally end
	
 reportstring = 0;
 if( playerstring )
 {
  if( sscanf( playerstring, "%d", &b ) != 1 )
   goto iohttpFunc_famaidL0;
    
  memset( res, 0, 4*sizeof(int) );
  for( a = 0 ; a < 4 ; a++ )
  {
   if( resstring[a] )
    sscanf( resstring[a], "%d", &res[a] );
  }
  
  if( cmdExecSendAid( id, b, maind.empire, res, nAlly ) < 0 )
  {
   if( cmdErrorString )
    reportstring = cmdErrorString;
   else
    reportstring = "Error while sending aid";
  }
  else
   reportstring = "Aid sent";
 }
 iohttpFunc_famaidL0:

 

 if( reportstring )
  svSendPrintf( cnt, "<i>%s</i><br><br>", reportstring );

 if( dbMapRetrieveEmpire( maind.empire, &empired ) < 0 )
 {
  svSendString( cnt, "Error while retrieving empire data" );
  iohttpBodyEnd( cnt );
  return;
 }
 if(( empired.numplayers == 1 )&&(nAlly == -1))
 {
  svSendString( cnt, "There is no one to send aid to in your empire yet!" );
  iohttpBodyEnd( cnt );
  return;
 }

 svSendString( cnt, "<form action=\"famaid\" method=\"POST\">Send to : <select name=\"id\">" );
 for( a = 0 ; a < empired.numplayers ; a++ )
 {
  if( empired.player[a] == id )
   continue;
  if( !( user = dbUserLinkID( empired.player[a] ) ) )
   continue;
  svSendPrintf( cnt, "<option value=\"%d\">%s", empired.player[a], user->faction );
 }
 
 for( a = 0 ; a < empire2d.numplayers ; a++ )
 {
  if( !( user = dbUserLinkID( empire2d.player[a] ) ) )
   continue;
  svSendPrintf( cnt, "<option value=\"%d\">%s", empire2d.player[a], user->faction );
 }
 svSendString( cnt, "</select><br><br><table width=\"100%\" cellspacing=\"4\">" );
 for( a = 0 ; a < CMD_RESSOURCE_NUMUSED ; a++ )
  svSendPrintf( cnt, "<tr><td width=\"50%%\" align=\"right\">%lld %s</td><td width=\"50%%\"><input type=\"text\" name=\"r%d\" size=\"10\"></td></tr>", maind.ressource[a], cmdRessourceName[a], a );
 svSendString( cnt, "</table><br>" );
 svSendString( cnt, "<input type=\"submit\" value=\"Send\"></form>" );

 iohttpBodyEnd( cnt );
 return;
}


void iohttpFunc_famgetaid( svConnectionPtr cnt )
{
 int a, b, id, res[4];
 dbUserMainDef maind, main2d;
 dbMainEmpireDef empired;
 unsigned char *accessstring, *playerstring, *resstring[4];
 unsigned char *reportstring;
 dbUserPtr user;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;

 iohttpVarsInit( cnt );
 accessstring = iohttpVarsFind( "access" );
 playerstring = iohttpVarsFind( "id" );
 resstring[0] = iohttpVarsFind( "r0" );
 resstring[1] = iohttpVarsFind( "r1" );
 resstring[2] = iohttpVarsFind( "r2" );
 resstring[3] = iohttpVarsFind( "r3" );
 iohttpVarsCut();

 reportstring = 0;
 if( accessstring )
 {
  if( sscanf( accessstring, "%d", &b ) != 1 )
   goto iohttpFunc_famaidL0;
  cmdExecAidAccess( id, b );
  reportstring = "Faction resources access rights have been changed";
 }
 if( playerstring )
 {
  if( sscanf( playerstring, "%d", &b ) != 1 )
   goto iohttpFunc_famaidL0;
  if( dbUserMainRetrieve( id, &maind ) < 0 )
  {
   reportstring = "Error retrieving user's infos";
   goto iohttpFunc_famaidL0;
  }
  memset( res, 0, 4*sizeof(int) );
  for( a = 0 ; a < 4 ; a++ )
  {
   if( resstring[a] )
    sscanf( resstring[a], "%d", &res[a] );
  }
  if( cmdExecGetAid( b, id, maind.empire, res ) < 0 )
  {
   if( cmdErrorString )
    reportstring = cmdErrorString;
   else
    reportstring = "Error while receiving aid";
  }
  else
   reportstring = "Aid received";
 }
 iohttpFunc_famaidL0:

 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Receive Aid" );

 if( reportstring )
  svSendPrintf( cnt, "<i>%s</i><br><br>", reportstring );

 if( dbMapRetrieveEmpire( maind.empire, &empired ) < 0 )
 {
  svSendString( cnt, "Error while retrieving empire data" );
  iohttpBodyEnd( cnt );
  return;
 }

 for( a = 0 ; a < empired.numplayers ; a++ )
 {
  if( empired.player[a] == id )
   continue;
  if( !( user = dbUserLinkID( empired.player[a] ) ) )
   continue;
  if( dbUserMainRetrieve( empired.player[a], &main2d ) < 0 )
   continue;
  svSendPrintf( cnt, "<b>%s</b> - ", user->faction );
  svSendPrintf( cnt, "Energy: %lld&nbsp;&nbsp;Mineral: %lld&nbsp;&nbsp;Crystal: %lld&nbsp;&nbsp;Ectrolium: %lld", main2d.ressource[0], main2d.ressource[1], main2d.ressource[2], main2d.ressource[3] );
  svSendPrintf( cnt, "<br><br>" );
 }

 if( empired.numplayers == 1 )
 {
  svSendString( cnt, "There is no one to get aid from in your empire yet!" );
  iohttpBodyEnd( cnt );
  return;
 }

 svSendString( cnt, "<table border=\"0\"><tr><td width=\"50%\" align=\"center\">" );

 svSendString( cnt, "<form action=\"famgetaid\" method=\"POST\">Get aid from : <select name=\"id\">" );
 for( a = 0 ; a < empired.numplayers ; a++ )
 {
  if( empired.player[a] == id )
   continue;
  if( !( user = dbUserLinkID( empired.player[a] ) ) )
   continue;
  svSendPrintf( cnt, "<option value=\"%d\">%s", empired.player[a], user->faction );
 }
 svSendString( cnt, "</select><br><br><table width=\"100%\" cellspacing=\"4\">" );
 for( a = 0 ; a < CMD_RESSOURCE_NUMUSED ; a++ )
  svSendPrintf( cnt, "<tr><td width=\"50%%\" align=\"right\">%s</td><td width=\"50%%\"><input type=\"text\" name=\"r%d\" size=\"10\"></td></tr>", cmdRessourceName[a], a );
 svSendString( cnt, "</table><br>" );
 svSendString( cnt, "<input type=\"submit\" value=\"Send\"></form>" );

 svSendString( cnt, "</td><td width=\"50%\" align=\"center\">" );

 svSendString( cnt, "<form action=\"famgetaid\" method=\"POST\"><br>Allow access to your faction reserves to :<br><select name=\"access\">" );
 svSendPrintf( cnt, "<option value=\"0\"" );
 if( maind.aidaccess == 0 )
  svSendPrintf( cnt, " selected" );
 svSendPrintf( cnt, ">No one" );
 svSendPrintf( cnt, "<option value=\"1\"" );
 if( maind.aidaccess == 1 )
  svSendPrintf( cnt, " selected" );
 svSendPrintf( cnt, ">Only the leader" );
 svSendPrintf( cnt, "<option value=\"2\"" );
 if( maind.aidaccess == 2 )
  svSendPrintf( cnt, " selected" );
 svSendPrintf( cnt, ">The leader and vice-leaders" );
 svSendPrintf( cnt, "<option value=\"3\"" );
 if( maind.aidaccess == 3 )
  svSendPrintf( cnt, " selected" );
 svSendPrintf( cnt, ">All factions" );
 svSendString( cnt, "</select><br><br>" );
 svSendString( cnt, "<input type=\"submit\" value=\"Change access\"></form>" );

 svSendString( cnt, "</td></tr></table>" );


/*
 svSendString( cnt, "<form action=\"famgetaid\" method=\"POST\">Transfer a planet to : <select name=\"id\">" );
 for( a = 0 ; a < empired.numplayers ; a++ )
 {
  if( empired.player[a] == id )
   continue;
  if( !( user = dbUserLinkID( empired.player[a] ) ) )
   continue;
  svSendPrintf( cnt, "<option value=\"%d\">%s", empired.player[a], user->faction );
 }
 svSendString( cnt, "</select><br><br><table width=\"100%\" cellspacing=\"4\">" );
 for( a = 0 ; a < CMD_RESSOURCE_NUMUSED ; a++ )
  svSendPrintf( cnt, "<tr><td width=\"50%%\" align=\"right\">%s</td><td width=\"50%%\"><input type=\"text\" name=\"r%d\" size=\"10\"></td></tr>", cmdRessourceName[a], a );
 svSendString( cnt, "</table><br>" );
 svSendString( cnt, "<input type=\"submit\" value=\"Send\"></form>" );
*/


/*
int battleReadinessLoss( dbUserMainPtr maind, dbUserMainPtr main2d )
*/


 iohttpBodyEnd( cnt );
 return;
}



void iohttpFunc_famvote( svConnectionPtr cnt )
{
 int a, b, id, fampos, vote;
 dbUserMainDef maind, main2d;
 dbMainEmpireDef empired;
 unsigned char *votestring;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Empire votes" );

 iohttpVarsInit( cnt );
 votestring = iohttpVarsFind( "id" );
 iohttpVarsCut();

 if( dbMapRetrieveEmpire( maind.empire, &empired ) < 0 )
 {
  svSendString( cnt, "Error while retrieving empire data" );
  iohttpBodyEnd( cnt );
  return;
 }
 for( a = 0 ; ; a++ )
 {
  if( a == empired.numplayers )
  {
   svSendString( cnt, "This is a strange error" );
   iohttpBodyEnd( cnt );
   return;
  }
  if( id == empired.player[a] )
  {
   fampos = a;
   break;
  }
 }

 if( ( votestring ) && ( sscanf( votestring, "%d", &vote ) == 1 ) )
 {
  if( cmdExecChangeVote( id, vote ) < 0 )
   svSendString( cnt, "<i>Failed to change vote...?!</i><br><br>" );
  else
  {
   svSendString( cnt, "<i>Vote changed</i><br><br>" );
   dbMapRetrieveEmpire( maind.empire, &empired );
  }
 }

 if( empired.leader == -1 )
  svSendString( cnt, "<b>Your empire didn't elect a leader yet</b>" );
 else if( empired.leader == id )
  svSendString( cnt, "<b>You are the leader</b>" );
 else
 {
  if( dbUserMainRetrieve( empired.leader, &main2d ) < 0 )
   svSendString( cnt, "Error while retriving leader's main data" );
  else
   svSendPrintf( cnt, "<b>The empire leader is %s</b>", main2d.faction );
 }

 svSendString( cnt, "<br><form action=\"famvote\" method=\"POST\"><table cellspacing=\"8\"><tr><td><b>Empire members</b></td><td>Networth</td><td>Planets</td><td>Your vote</td></tr>" );
 for( a = b = 0 ; a < empired.numplayers ; a++ )
 {
  if( dbUserMainRetrieve( empired.player[a], &main2d ) < 0 )
  {
   svSendString( cnt, "Error while retriving user's main data" );
   return;
  }
  svSendPrintf( cnt, "<tr><td><a href=\"player?id=%d\">", empired.player[a] );
  if( empired.leader == empired.player[a] )
   svSendPrintf( cnt, "<font color=\"#FFC040\"><b>%s</b></font>", main2d.faction );
  else
   svSendString( cnt, main2d.faction );
  svSendPrintf( cnt, "</a></td><td>%lld</td><td>%d</td><td align=\"center\"><input type=\"radio\" value=\"%d\" name=\"id\"", main2d.networth, main2d.planets, empired.player[a] );
  if( empired.vote[fampos] == a )
  {
   svSendString( cnt, " checked" );
   b = 1;
  }
  svSendString( cnt, "></td></tr>" );
 }
 svSendString( cnt, "<tr><td>Vote blank</td><td>&nbsp;</td><td>&nbsp;</td><td align=\"center\"><input type=\"radio\" value=\"-1\" name=\"id\"" );
 if( !( b ) )
  svSendString( cnt, " checked" );
 svSendString( cnt, "></td></tr>" );
 svSendString( cnt, "</table><br><input type=\"submit\" value=\"Change vote\"></form>" );

 iohttpBodyEnd( cnt );
 return;
}

void iohttpFunc_famnews( svConnectionPtr cnt )
{
 int a, id, num, curfam;
 dbUserMainDef maind;
 dbMainEmpireDef empired;
 unsigned char *empirestring;
 long long int *newsp, *newsd;
	FILE *fFile;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;

 iohttpVarsInit( cnt );
 empirestring = iohttpVarsFind( "id" );
 iohttpVarsCut();

 curfam = maind.empire;
 if( empirestring )
 {
  sscanf( empirestring, "%d", &curfam );
  a = cmdExecFindRelation( maind.empire, curfam, 0, 0 );
  if( ( a != CMD_RELATION_ALLY ) && ( cnt->dbuser->level < LEVEL_MODERATOR ) )
   curfam = maind.empire;
  if(cnt->dbuser->level >= LEVEL_MODERATOR)
 	{
 		fFile = fopen( LOGS_DIRECTORY "/modlog.txt", "a+t" );
 		if( fFile )
 		{
 			fprintf( fFile, "%s >view news of empire %d\n",maind.faction, curfam);
 			fclose(fFile);
 		}
 	}
 }

 if( ( dbMapRetrieveEmpire( curfam, &empired ) ) < 0 )
 {
  svSendString( cnt, "This empire does not seem to exist!</body></html>" );
  return;
 }

 iohttpBodyInit( cnt, "Empire #%d news", curfam );

	num = dbFamNewsList( curfam, &newsp, svTickNum );
 newsd = newsp;
 iohttpFamNews( cnt, num, newsd, &empired );
 if( newsp )
  free( newsp );

 iohttpBodyEnd( cnt );
 return;
}

void iohttpFunc_famrels( svConnectionPtr cnt )
{
 int a, b, c, id, curfam, nEmp;
 dbUserMainDef maind;
 dbMainEmpireDef empired;
 unsigned char *empirestring;
 int *rel;
 unsigned char message[4096];

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;

 iohttpVarsInit( cnt );
 empirestring = iohttpVarsFind( "id" );
 iohttpVarsCut();

 curfam = maind.empire;
 if(empirestring)
 	nEmp = atoi(empirestring);
 else
 	nEmp = -1;
 if(( ( cnt->dbuser->level >= LEVEL_MODERATOR ) && ( empirestring ) )|| ( cmdExecFindRelation(maind.empire, nEmp, &c, 0) == CMD_RELATION_ALLY))
  sscanf( empirestring, "%d", &curfam );

	if( ( dbMapRetrieveEmpire( curfam, &empired ) ) < 0 )
 {
  svSendString( cnt, "This empire does not seem to exist!</body></html>" );
  return;
 }

 iohttpBodyInit( cnt, "Empire #%d relations", curfam );

 if( ( dbEmpireMessageRetrieve( curfam, 1, message ) >= 0 ) && ( message[0] ) )
 {
  svSendString( cnt, "<b>Message from your leader</b><br>" );
  svSendString( cnt, message );
  svSendString( cnt, "<br><br>" );
 }

 if( ( b = dbEmpireRelsList( curfam, &rel ) ) >= 0 )
 {
  svSendString( cnt, "<b>Empire relations</b><br><br>" );
  if( b == 0 )
   svSendString( cnt, "No relations" );
  b <<= 2;
  for( a = 0 ; a < b ; a += 4 )
  {
   if( rel[a+3] & 1 )
    continue;
   if( rel[a+1] == CMD_RELATION_ALLY )
    svSendPrintf( cnt, "You are offering an <font color=\"#80FF80\">alliance</font> to <a href=\"empire?id=%d\">empire #%d</a><br><br>", rel[a+2], rel[a+2] );
   else if( rel[a+1] == CMD_RELATION_WAR )
    svSendPrintf( cnt, "You declared <font color=\"#FF0000\">war</font> to <a href=\"empire?id=%d\">empire #%d</a><br><br>", rel[a+2], rel[a+2] );
  }
  for( a = 0 ; a < b ; a += 4 )
  {
   if( !( rel[a+3] & 1 ) )
    continue;
   if( rel[a+1] == CMD_RELATION_ALLY )
    svSendPrintf( cnt, "<a href=\"empire?id=%d\">Empire #%d</a> offered a <font color=\"#80FF80\">alliance</font><br><br>", rel[a+2], rel[a+2] );
   else if( rel[a+1] == CMD_RELATION_WAR )
    svSendPrintf( cnt, "<a href=\"empire?id=%d\">Empire #%d</a> declared <font color=\"#FF0000\">war</font>!<br><br>", rel[a+2], rel[a+2] );
  }
  svSendString( cnt, "<br>" );
  for( a = 0 ; a < b ; a += 4 )
  {
   if( ( rel[a+3] & 1 ) || ( rel[a+1] != CMD_RELATION_ALLY ) )
    continue;
   for( c = 0 ; c < b ; c += 4 )
   {
    if( !( rel[c+3] & 1 ) || ( rel[c+1] != CMD_RELATION_ALLY ) || ( rel[a+2] != rel[c+2] ) )
     continue;
    svSendPrintf( cnt, "Your empire is <font color=\"#00FF00\">allied</font> to <a href=\"empire?id=%d\">empire #%d</a> - <a href=\"/forum?forum=%d\">Empire Forum</a> - <a href=\"/famnews?id=%d\">Empire news</a> - <a href=\"/famrels?id=%d\">Empire relations</a><br>", rel[a+2], rel[a+2], 100+rel[a+2], rel[a+2], rel[a+2]);
   }
  }
  free( rel );
 }

 iohttpBodyEnd( cnt );
 return;
}

int iohttpForumFilter( unsigned char *dest, unsigned char *string, int size, int html );
int iohttpForumFilter2( unsigned char *dest, unsigned char *string, int size );
int iohttpForumFilter3( unsigned char *dest, unsigned char *string, int size );

void iohttpFunc_famleader( svConnectionPtr cnt )
{
 int a, b, c, id, filesize, curfam, sid, status, relfam, reltype;
 dbUserMainDef maind;
 dbMainEmpireDef empired;
 unsigned char *empirestring, *fnamestring, *sidstring, *statusstring, *fampassstring, *relfamstring, *reltypestring, *hqmesstring, *relsmesstring, *fampic, *filename;
 unsigned char fname[256];
 FILE *file;
 iohttpFilePtr cfile;
 dbUserPtr user;
 int *rel;
 unsigned char message[4096], message2[4096];

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;

 empirestring = fnamestring = sidstring = statusstring = fampassstring = reltypestring = relfamstring = hqmesstring = relsmesstring = filename = 0;
 if( !( fampic = iohttpVarsUpload( cnt, &filename, &filesize ) ) )
 {
  iohttpVarsInit( cnt );
  empirestring = iohttpVarsFind( "id" );
  fnamestring = iohttpVarsFind( "fname" );
  sidstring = iohttpVarsFind( "sid" );
  statusstring = iohttpVarsFind( "status" );
  fampassstring = iohttpVarsFind( "fampass" );
  relfamstring = iohttpVarsFind( "relfam" );
  reltypestring = iohttpVarsFind( "reltype" );
  hqmesstring = iohttpVarsFind( "hqmes" );
  relsmesstring = iohttpVarsFind( "relsmes" );
  iohttpVarsCut();
 }

 curfam = maind.empire;
 if( ( cnt->dbuser->level >= LEVEL_MODERATOR ) && ( empirestring ) )
  sscanf( empirestring, "%d", &curfam );

 if( dbMapRetrieveEmpire( curfam, &empired ) < 0 )
 {
  svSendString( cnt, "This empire does not seem to exist!</body></html>" );
  return;
 }
 if( ( empired.leader != id ) && ( cnt->dbuser->level < LEVEL_MODERATOR ) )
 {
  svSendString( cnt, "You are not the leader of this empire!</body></html>" );
  return;
 }

 iohttpBodyInit( cnt, "Leader options" );
 if( fnamestring )
 {
  a = iohttpForumFilter( fname, fnamestring, 63, 0 );
  if( cmdExecChangFamName( curfam, fname ) < 0 )
  {
   if( cmdErrorString )
    svSendPrintf( cnt, "<i>%s</i><br><br>", cmdErrorString );
   else
    svSendString( cnt, "<i>Error while changing empire name!</i><br><br>" );
  }
  else
  {
   svSendString( cnt, "<i>Empire name changed</i><br><br>" );
   dbMapRetrieveEmpire( curfam, &empired );
  }
 }

 if( fampassstring )
 {
  iohttpForumFilter( fname, fampassstring, 32, 0 );
  
  //ONLY ROUND #23 - 24 changes comment the next line
  
  if(cmdExecSetFamPass( curfam, fname) != 1)
			svSendString( cnt, "<i>Empire password did not changed</i><br><br>" );
		else
			svSendString( cnt, "<i>Empire password changed</i><br><br>" );
 }

 if( fampic )
 {
  if( filesize <= 0 )
  {
   svSendString( cnt, "<i>Error while retrieving empire picture?</i><br><br>" );
   goto iohttpFunc_famleaderL0;
  }
  b = 4;
  if( filename )
  {
   for( a = 0 ; filename[a] ; a++ )
   {
    if( filename[a] == '"' )
     filename[a] = 0;
   }
   b = iohttpMimeFind( filename );
  }
  sprintf( fname, IOHTTP_FILES_DIRECTORY "/fampic%02d", curfam );
  if( ( file = fopen( fname, "wb" ) ) )
  {
   fwrite( fampic, 1, filesize, file );
   fclose( file );
   sprintf( fname, "/fampic%02d%d", curfam, empired.pictime );
   iohttpFileDel( fname );
   if( ( cfile = iohttpFileAdd( filesize ) ) )
   {
    cfile->type = 0;
    cfile->mime = b;
    empired.picmime = b;
    empired.pictime = time( 0 );
    a = time( 0 );
    memcpy( &cfile->scurtime, &a, sizeof(time_t) );
    sprintf( cfile->path, "/fampic%02d%d", curfam, empired.pictime );
    memcpy( cfile->data, fampic, filesize );
    svSendString( cnt, "<i>Empire picture uploaded on server</i><br><br>" );
   }
  }
  if( dbMapSetEmpire( curfam, &empired ) < 0 )
  {
   svSendString( cnt, "Error while saving empire data</body></html>" );
   return;
  }
 }

 if( ( sidstring ) && ( statusstring ) && ( sscanf( sidstring, "%d", &sid ) == 1 ) && ( sscanf( statusstring, "%d", &status ) == 1 ) )
 {
  if( cmdExecFamMemberFlags( sid, curfam, status ) < 0 )
  {
   svSendString( cnt, "Error while changing player status</body></html>" );
   return;
  }
  svSendString( cnt, "<i>Empire member status updated</i><br><br>" );
 }

 if( reltypestring )
 {
 	if( sscanf( reltypestring, "%d", &reltype ) != 1 )
   goto iohttpFunc_famleaderL0;
  if( !( relfamstring ) )
  {
  	//Cancel all rel cancelation by commenting this if
   if( cmdExecDelRelation( curfam, reltype ) < 0 )
   {
    if( cmdErrorString )
     svSendPrintf( cnt, "<i>%s</i><br><br>", cmdErrorString );
    else
     svSendString( cnt, "<i>Error while cancelling relation</i><br><br>" );
   }
   else
    svSendString( cnt, "<i>Relation cancelled</i><br><br>" );
   goto iohttpFunc_famleaderL0;
  }
  if( sscanf( relfamstring, "%d", &relfam ) != 1 )
   goto iohttpFunc_famleaderL0;
   
   //ONLY #23 - 24 Can't give ally rel only war :D
//   reltype = CMD_RELATION_WAR;
   
  if( cmdExecAddRelation( curfam, reltype, relfam ) < 0 )
  {
   if( cmdErrorString )
    svSendPrintf( cnt, "<i>%s</i><br><br>", cmdErrorString );
   else
    svSendString( cnt, "<i>Error while adding relation</i><br><br>" );
  }
  else
   svSendPrintf( cnt, "<i>Changed relation with empire #%d</i><br><br>", relfam );
 }

 if( hqmesstring )
 {
  iohttpForumFilter( message, hqmesstring, 4096, 0 );
  iohttpForumFilter2( message2, message, 4096 );
  dbEmpireMessageSet( curfam, 0, message2 );
  svSendString( cnt, "<i>Leader message changed</i><br><br>" );
 }

 if( relsmesstring )
 {
  iohttpForumFilter( message, relsmesstring, 4096, 0 );
  iohttpForumFilter2( message2, message, 4096 );
  dbEmpireMessageSet( curfam, 1, message2 );
  svSendString( cnt, "<i>Relations message changed</i><br><br>" );
 }

 iohttpFunc_famleaderL0:

 svSendString( cnt, "<table>" );

 svSendString( cnt, "<tr><td><form action=\"famleader\" method=\"POST\">Empire name</td></tr>" );
 svSendPrintf( cnt, "<tr><td><input type=\"hidden\" name=\"id\" value=\"%d\"><input type=\"text\" name=\"fname\" size=\"64\" value=\"%s\"></td></tr>", curfam, empired.name );
 svSendString( cnt, "<tr><td><input type=\"submit\" value=\"Change\"></form><br><br><br></td></tr>" );

 svSendString( cnt, "<tr><td><form enctype=\"multipart/form-data\" action=\"famleader\" method=\"POST\">Empire picture</td></tr>" );
 svSendString( cnt, "<tr><td><i>Note : Family pictures can't exceed 64k</i></td></tr>" );
 svSendString( cnt, "<tr><td><input type=\"file\" name=\"fname\" size=\"64\"></td></tr>" );
 svSendString( cnt, "<tr><td><input type=\"submit\" value=\"Upload\"></form><br><br><br></td></tr>" );

 svSendString( cnt, "<tr><td>Set an empire member status</td></tr>" );
 svSendString( cnt, "<tr><td><i>Vice-leaders can edit and delete posts in the forum.<br>Factions marked independent aren't allowed to read the empire forum, and can lose their home planet.</i></td></tr>" );
 svSendString( cnt, "<tr><td><form action=\"famleader\" method=\"POST\"><select name=\"sid\">" );
 for( a = 0 ; a < empired.numplayers ; a++ )
 {
  if( empired.player[a] == id )
   continue;
  if( !( user = dbUserLinkID( empired.player[a] ) ) )
   continue;
  svSendPrintf( cnt, "<option value=\"%d\">%s", empired.player[a], user->faction );
 }
 svSendString( cnt, "</select> <select name=\"status\"><option value=\"0\">No tag<option value=\"1\">Vice-leader<option value=\"2\">Independent</select></td></tr>" );
 svSendString( cnt, "<tr><td><input type=\"submit\" value=\"Change\"></form><br><br><br></td></tr>" );

 cmdExecGetFamPass( curfam, fname );
 svSendString( cnt, "<tr><td><form action=\"famleader\" method=\"POST\">Empire password</td></tr>" );
 svSendString( cnt, "<tr><td><i>The empire password is required for players to join the empire. Leave the field blank to let anyone join.</i></td></tr>" );
 svSendPrintf( cnt, "<tr><td><input type=\"hidden\" name=\"id\" value=\"%d\"><input type=\"text\" name=\"fampass\" size=\"64\" value=\"%s\"></td></tr>", curfam, fname );
 svSendString( cnt, "<tr><td><input type=\"submit\" value=\"Change\"></form><br><br><br></td></tr>" );

 svSendString( cnt, "<tr><td><table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\">" );
// svSendString( cnt, "<tr><td width=\"40%\"><form action=\"famleader\" method=\"POST\">Offer an alliance to an empire</td><td width=\"60%\" rowspan=\"4\" valign=\"top\">" );
 if( ( b = dbEmpireRelsList( curfam, &rel ) ) >= 0 )
 {
  svSendString( cnt, "<b>Empire relations</b><br><br>" );
  if( b == 0 )
   svSendString( cnt, "No relations" );
  b <<= 2;
  for( a = 0 ; a < b ; a += 4 )
  {
   if( rel[a+3] & 1 )
    continue;
   if( rel[a+1] == CMD_RELATION_ALLY )
    svSendPrintf( cnt, "You are offering an alliance to <a href=\"empire?id=%d\">empire #%d</a> - <a href=\"famleader?id=%d&reltype=%d\">Cancel</a><br>", rel[a+2], rel[a+2], curfam, a >> 2 );
   else if( rel[a+1] == CMD_RELATION_WAR )
    svSendPrintf( cnt, "You declared war on <a href=\"empire?id=%d\">empire #%d</a> - <a href=\"famleader?id=%d&reltype=%d\">Cancel</a><br>", rel[a+2], rel[a+2], curfam, a >> 2 );
  }
  for( a = 0 ; a < b ; a += 4 )
  {
   if( !( rel[a+3] & 1 ) )
    continue;
   if( rel[a+1] == CMD_RELATION_ALLY )
    svSendPrintf( cnt, "<a href=\"empire?id=%d\">Empire #%d</a> offered a alliance<br>", rel[a+2], rel[a+2] );
   else if( rel[a+1] == CMD_RELATION_WAR )
    svSendPrintf( cnt, "<a href=\"empire?id=%d\">Empire #%d</a> declared war!<br>", rel[a+2], rel[a+2] );
  }
  svSendString( cnt, "<br>" );
  for( a = 0 ; a < b ; a += 4 )
  {
   if( ( rel[a+3] & 1 ) || ( rel[a+1] != CMD_RELATION_ALLY ) )
    continue;
   for( c = 0 ; c < b ; c += 4 )
   {
    if( !( rel[c+3] & 1 ) || ( rel[c+1] != CMD_RELATION_ALLY ) || ( rel[a+2] != rel[c+2] ) )
     continue;
    svSendPrintf( cnt, "<i>Allied to empire #%d</i><br>", rel[a+2] );
   }
  }
  free( rel );
 }
 svSendString( cnt, "</td></tr>" );
 svSendString( cnt, "<tr><td><input type=\"text\" name=\"relfam\" size=\"8\"> <input type=\"hidden\" name=\"reltype\" value=\"0\"> <input type=\"submit\" value=\"Send\"></form><br><br><br></td></tr>" );
 svSendString( cnt, "<tr><td><form action=\"famleader\" method=\"POST\">Declare war to an empire</td></tr>" );
 svSendString( cnt, "<tr><td><input type=\"text\" name=\"relfam\" size=\"8\"> <input type=\"hidden\" name=\"reltype\" value=\"1\"> <input type=\"submit\" value=\"Send\"></form><br><br><br></td></tr>" );
 svSendString( cnt, "</table></td></tr>" );

 dbEmpireMessageRetrieve( curfam, 0, message );
 iohttpForumFilter3( message2, message, 4096 );
 svSendString( cnt, "<tr><td><form action=\"famleader\" method=\"POST\">Leader message</td></tr>" );
 svSendPrintf( cnt, "<tr><td><input type=\"hidden\" name=\"id\" value=\"%d\"><textarea name=\"hqmes\" wrap=\"soft\" rows=\"4\" cols=\"64\">%s</textarea></td></tr>", curfam, message2 );
 svSendString( cnt, "<tr><td><input type=\"submit\" value=\"Change\"></form><br><br><br></td></tr>" );

 dbEmpireMessageRetrieve( curfam, 1, message );
 iohttpForumFilter3( message2, message, 4096 );
 svSendString( cnt, "<tr><td><form action=\"famleader\" method=\"POST\">Relations message</td></tr>" );
 svSendPrintf( cnt, "<tr><td><input type=\"hidden\" name=\"id\" value=\"%d\"><textarea name=\"relsmes\" wrap=\"soft\" rows=\"4\" cols=\"64\">%s</textarea></td></tr>", curfam, message2 );
 svSendString( cnt, "<tr><td><input type=\"submit\" value=\"Change\"></form><br><br><br></td></tr>" );

 svSendString( cnt, "</table>" );


/* Images upload?
 svSendString( cnt, "<form enctype=\"multipart/form-data\" action=\"famleader\" method=\"POST\">" );
 svSendPrintf( cnt, "Empire name<br><br><input type=\"file\" name=\"fname\">" );
 svSendPrintf( cnt, "<input type=\"hidden\" name=\"id\" value=\"sdaffsafsd\">" );
 svSendString( cnt, "<br><br><input type=\"submit\" value=\"Change\"></form>" );

Content-length: 196
Content-Type: multipart/form-data; boundary=----------r7GsgKZ4BfAeDg8UgRpujN

------------r7GsgKZ4BfAeDg8UgRpujN
Content-Disposition: form-data; name="fname"; filename="hello.txt"
Content-Type: text/plain; name="hello.txt"

ABCDE

------------r7GsgKZ4BfAeDg8UgRpujN--
*/

 iohttpBodyEnd( cnt );

 return;
}


#define IOHTTP_MAPPICK_DIVIDE (3)

#define IOHTTP_MAPADV_ENTRIES (8)

void iohttpMapSystemsBuffer( int *mapp, int *buffer, int andl, int numpl, int type )
{
 int a, b, x, y;
 if( type )
 {
  for( a = 0 ; a < numpl ; a++ )
  {
   x = ( buffer[a] >> 8 ) & 0xFFF;
   y = buffer[a] >> 20;
   b = y * dbMapBInfoStatic[0] + x;
   mapp[b] &= 0xFF00FFFF;
   mapp[b] |= andl;
  }
 }
 else
 {
  for( a = 0 ; a < numpl ; a++ )
  {
   x = ( buffer[a] >> 8 ) & 0xFFF;
   y = buffer[a] >> 20;
   mapp[ y * dbMapBInfoStatic[0] + x ] |= andl;
  }
 }
 free( buffer );
 return;
}


void iohttpFunc_map( svConnectionPtr cnt )
{
 int a, b, c, d, numpl, id, x, y, i, explcol, newwins, cid;
 int *buffer;
 dbUserMainDef maind;
 dbMainSystemDef systemd;
 dbMainEmpireDef empired;
 int *mapp;
 int zoompos[2];
 unsigned char advname[16];
 unsigned char *varstring;
 int advopt[IOHTTP_MAPADV_ENTRIES];
 int advcol[IOHTTP_MAPADV_ENTRIES];
 unsigned char *advdet[IOHTTP_MAPADV_ENTRIES];
 unsigned char advdetnum[IOHTTP_MAPADV_ENTRIES][16];
 unsigned char *configstring;
 int basex, basey, endx, endy, zoomsize;
 int config[8];

 if( ( id = iohttpIdentify( cnt, 1|2|8 ) ) < 0 )
  return;
 if( dbUserMainRetrieve( id, &maind ) < 0 )
  maind.empire = -1;

 if( !( mapp = malloc( dbMapBInfoStatic[0]*dbMapBInfoStatic[1]*sizeof(int) ) ) )
 {
  iohttpBase( cnt, 1|2 );
  svSendString( cnt, "Error, could not allocate memory</body></html>" );
  return;
 }
 memset( mapp, 0, dbMapBInfoStatic[0]*dbMapBInfoStatic[1]*sizeof(int) );

 iohttpVarsInit( cnt );
 if( !( iohttpVarsMapcoords( cnt, zoompos ) ) )
 {
  zoompos[0] = 0;
  zoompos[1] = 0;
  zoomsize = -1;
 }
 else
 {
  zoompos[0] /= IOHTTP_MAPPICK_DIVIDE;
  zoompos[1] /= IOHTTP_MAPPICK_DIVIDE;
  zoomsize = maind.config_mapsize & 0xFFFF;
  if( (unsigned int)zoomsize > 60 )
   zoomsize = 15;
 }

 configstring = iohttpVarsFind( "setdefault" );
 for( a = 0 ; a < IOHTTP_MAPADV_ENTRIES ; a++ )
 {
  config[a] = 0;
  sprintf( advname, "e%d", a );
  varstring = iohttpVarsFind( advname );
  if( !( varstring ) || ( sscanf( varstring, "%d", &advopt[a] ) != 1 ) )
   advopt[a] = -1;
  advname[0] = 'c';
  varstring = iohttpVarsFind( advname );
  if( !( varstring ) || ( sscanf( varstring, "%d", &advcol[a] ) != 1 ) || ( (unsigned int)advcol[a] >= 6 ) )
   advopt[a] = -1;
  else
   config[a] = ( advopt[a] ) + ( advcol[a] << 4 ) + 0x10000000;
  advname[0] = 'u';
  advdet[a] = iohttpVarsFind( advname );
 }
 newwins = 0;
 if( ( advopt[0] == -1 ) && ( maind.config_mapsize & 0x10000 ) )
  newwins = 1;
 else if( iohttpVarsFind( "newwin" ) )
  newwins = 1;

 if( newwins )
  iohttpBase( cnt, 1|2|4 );
 else
  iohttpBase( cnt, 1|2 );

 iohttpVarsCut();
 if( advopt[0] == -1 )
 {
  for( a = 0 ; a < IOHTTP_MAPADV_ENTRIES ; a++ )
  {
   advopt[a] = maind.config_map[a] & 0xF;
   advcol[a] = ( maind.config_map[a] >> 4 ) & 0xF;
   b = maind.config_map[a] >> 8;
   if( b >= 0x10000 )
    continue;
   sprintf( advdetnum[a], "%d", b );
   advdet[a] = advdetnum[a];
  }
 }

 explcol = 0;
 for( a = 0 ; a < IOHTTP_MAPADV_ENTRIES ; a++ )
 {
  if( (unsigned int)advcol[a] < 3 )
   advcol[a] = ( ( 1 << advcol[a] ) << 16 ) | ( 1 << 20 );
  else
   advcol[a] = ( 1 << (advcol[a]-3) ) << 16;
  if( advopt[a] == 6 )
   explcol = advcol[a] & 0xF0000;
 }
 for( a = 0 ; a < dbMapBInfoStatic[2] ; a++ )
 {
  dbMapRetrieveSystem( a, &systemd );
  x = systemd.position & 0xFFFF;
  y = systemd.position >> 16;
  i = y * dbMapBInfoStatic[0] + x;
  mapp[i] = a+1;
  if( systemd.empire != -1 )
   mapp[i] |= 0x1000000;
  else if( systemd.unexplored )
   mapp[i] |= explcol;
 }

 for( a = 0, d = explcol ; a < IOHTTP_MAPADV_ENTRIES ; a++ )
 {
  if( (unsigned int)advopt[a] >= 6 )
   continue;
  d |= advopt[a];

  if( advopt[a] == 1 )
  {
   if( ( numpl = dbUserPlanetListCoords( id, &buffer ) ) < 0 )
    continue;
   iohttpMapSystemsBuffer( mapp, buffer, advcol[a] & 0xF0000, numpl, advcol[a] >> 20 );
  }
  else if( advopt[a] == 2 )
  {
   if( ( numpl = dbUserPortalsListCoords( id, &buffer ) ) < 0 )
    continue;
   iohttpMapSystemsBuffer( mapp, buffer, advcol[a] & 0xF0000, numpl, advcol[a] >> 20 );
  }
  else if( advopt[a] == 3 )
  {
   if( dbUserMainRetrieve( id, &maind ) < 0 )
    continue;
   cid = maind.empire;
   goto iohttpFunc_mapL0;
  }
  else if( advopt[a] == 4 )
  {
   if( !( advdet[a] ) || !( advdet[a][0] ) )
   {
    svSendPrintf( cnt, "#%d You must specify the faction name or user ID.<br><br>", a+1 );
    continue;
   }
   if( ( advdet[a][0] >= '0' ) && ( advdet[a][0] <= '9' ) )
    sscanf( advdet[a], "%d", &cid );
   else
   {
    for( c = 0 ; advdet[a][c] ; c++ )
    {
     if( advdet[a][c] == '+' )
      advdet[a][c] = ' ';
     else if( ( advdet[a][c] == 10 ) || ( advdet[a][c] == 13 ) )
      advdet[a][c] = 0;
    }
    cid = dbUserSearchFaction( advdet[a] );
    if( cid < 0 )
    {
     svSendPrintf( cnt, "#%d The faction %s does not seem to exist, the syntax must be exact or use the user ID.<br><br>", a+1, advdet[a] );
     continue;
    }
   }
   if( ( numpl = dbUserPlanetListCoords( cid, &buffer ) ) < 0 )
   {
    svSendPrintf( cnt, "#%d There is no user with an ID of %d.<br><br>", a+1, cid );
    continue;
   }
   iohttpMapSystemsBuffer( mapp, buffer, advcol[a] & 0xF0000, numpl, advcol[a] >> 20 );
   config[a] &= 0xFF;
   config[a] |= cid << 8;
  }
  else if( advopt[a] == 5 )
  {
   if( !( advdet[a] ) || !( advdet[a][0] ) || ( sscanf( advdet[a], "%d", &cid ) <= 0 ) )
   {
    svSendPrintf( cnt, "#%d You must specify the empire number.<br><br>", a+1 );
    continue;
   }
   iohttpFunc_mapL0:
   if( dbMapRetrieveEmpire( cid, &empired ) < 0 )
   {
    svSendPrintf( cnt, "#%d This empire doesn't seem to exist.<br><br>", a+1 );
    continue;
   }
   config[a] &= 0xFF;
   config[a] |= cid << 8;
   for( c = 0 ; c < empired.numplayers ; c++ )
   {
    if( ( numpl = dbUserPlanetListCoords( empired.player[c], &buffer ) ) < 0 )
    {
     svSendPrintf( cnt, "#%d There is no user with an ID of %d.<br><br>", a+1, cid );
     continue;
    }
    iohttpMapSystemsBuffer( mapp, buffer, advcol[a] & 0xF0000, numpl, advcol[a] >> 20 );
   }
  }
 }
 if( !( d ) )
 {
  if( ( numpl = dbUserPlanetListCoords( id, &buffer ) ) >= 0 )
   iohttpMapSystemsBuffer( mapp, buffer, 1 << 16, numpl, 0 );
 }

 if( ( configstring ) && ( dbUserMainRetrieve( id, &maind ) >= 0 ) )
 {
  maind.config_mapsize &= 0xFFFF;
  if( newwins )
   maind.config_mapsize |= 0x10000;
  memcpy( maind.config_map, config, 8*sizeof(int) );
  dbUserMainSet( id, &maind );
 }

 if( zoomsize <= 0 )
 {
  basex = basey = 0;
  endx = dbMapBInfoStatic[0];
  endy = dbMapBInfoStatic[1];
 }
 else
 {
  if( zoompos[0] < zoomsize )
  {
   basex = 0;
   endx = ( zoomsize << 1 );
  }
  else if( zoompos[0] > dbMapBInfoStatic[0]-zoomsize )
  {
   basex = dbMapBInfoStatic[0] - ( zoomsize << 1 );
   endx = dbMapBInfoStatic[0];
  }
  else
  {
   basex = zoompos[0] - zoomsize;
   endx = zoompos[0] + zoomsize;
  }
  if( zoompos[1] < zoomsize )
  {
   basey = 0;
   endy = ( zoomsize << 1 );
  }
  else if( zoompos[1] > dbMapBInfoStatic[1]-zoomsize )
  {
   basey = dbMapBInfoStatic[1] - ( zoomsize << 1 );
   endy = dbMapBInfoStatic[1];
  }
  else
  {
   basey = zoompos[1] - zoomsize;
   endy = zoompos[1] + zoomsize;
  }
 }
 svSendPrintf( cnt, "<table cellspacing=\"0\" cellpading=\"0\" width=\"%d\"><colgroup><col span=\"%d\" width=\"24\">", 24*zoomsize, zoomsize );
 svSendString( cnt, "<tr><td>&nbsp;</td>" );
 for( x = basex ; x < endx ; x++ )
  svSendPrintf( cnt, "<td align=\"center\">%d</td>", x );
 svSendString( cnt, "<td>&nbsp;</td></tr>" );
 i = basex + ( basey * dbMapBInfoStatic[0] );
 srand( 0 );
 for( y = basey ; y < endy ; y++ )
 {
  svSendPrintf( cnt, "<tr><td>%d</td>", y );
  for( x = basex ; x < endx ; x++, i++ )
  {
   if( !( mapp[i] ) )
    svSendString( cnt, "<td> " );
   else
   {
    a = '0' + ( ( mapp[i] >> 16 ) & 0xF );
    b = '5';
    if( !( mapp[i] >> 24 ) )
     b = '0' + ( rand() % 5 );
    svSendPrintf( cnt, "<td><a href=\"system?id=%d\"", ( mapp[i] & 0xFFFF ) - 1 );
    svSendPrintf( cnt, "><img border=\"0\" src=\"m%c%c.gif\" title=\"%d,%d\"></a>", a, b, x, y );
   }
  }
  if( zoomsize >= 0 )
   i += dbMapBInfoStatic[0] - ( zoomsize << 1 );
  svSendPrintf( cnt, "<td>%d</tr>", y );
 }
 svSendString( cnt, "<tr><td>&nbsp;</td>" );
 for( x = basex ; x < endx ; x++ )
  svSendPrintf( cnt, "<td align=\"center\">%d</td>", x );
 svSendString( cnt, "<td>&nbsp;</td></tr>" );
 svSendString( cnt, "</table>" );

 free( mapp );
 svSendString( cnt, "</center></body></html>" );
 return;
}

void iohttpFunc_mappick( svConnectionPtr cnt )
{
 int a, b, id;
 dbUserMainDef maind;
 unsigned char *sizestring;
 static int sizes[6] = { 15, 20, 25, 30, 45, 60 };

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Galaxy map" );

 iohttpVarsInit( cnt );
 sizestring = iohttpVarsFind( "size" );
 iohttpVarsCut();

 if( ( sizestring ) && ( sscanf( sizestring, "%d", &a ) == 1 ) )
 {
  if( cmdExecSetSectsize( id, a ) >= 0 )
  {
   svSendPrintf( cnt, "<i>Default map sector size changed for %d</i><br><br>", a );
   maind.config_mapsize = a | ( maind.config_mapsize & 0xFFFF0000 );
  }
  else
   svSendString( cnt, "<i>Error while changing default sector size</i><br><br>" );
 }

 a = ( maind.home >> 8 ) & 0xFFF;
 b = maind.home >> 20;
 svSendPrintf( cnt, "Click on the part of the map you want to render with your default <a href=\"mapadv\">map generation</a> settings.<br>Your home system is located at the coordinates <a href=\"map?%d,%d\">%d,%d</a> ( click to zoom there ).<br><br>", a * IOHTTP_MAPPICK_DIVIDE, b * IOHTTP_MAPPICK_DIVIDE, a, b );

 a = dbMapBInfoStatic[0] * IOHTTP_MAPPICK_DIVIDE;
 svSendPrintf( cnt, "<table border=\"0\" width=\"%d\" cellpadding=\"0\" cellspacing=\"0\">", a + 80 );
 a /= 3;
 svSendPrintf( cnt, "<tr><td width=\"40\">&nbsp;</td><td width=\"%d\" align=\"left\"><b>0</b></td><td width=\"%d\" align=\"center\"><b>%d</b></td><td width=\"%d\" align=\"right\"><b>%d</b></td><td width=\"40\">&nbsp;</td></tr>", a, a, dbMapBInfoStatic[0] >> 1, a, dbMapBInfoStatic[0] );

 svSendPrintf( cnt, "<tr><td height=\"%d\" align=\"right\" valign=\"top\"><b>0</b></td>", a );
 svSendPrintf( cnt, "<td colspan=\"3\" rowspan=\"3\"><a href=\"map\"><img src=\"galaxyr%d.gif\" ismap></a></td>", ROUND_ID );
 svSendPrintf( cnt, "<td height=\"%d\" align=\"left\" valign=\"top\"><b>0</b></td></tr>", a );
 svSendPrintf( cnt, "<tr><td height=\"%d\" align=\"right\" valign=\"center\"><b>%d</b></td><td height=\"%d\" align=\"left\" valign=\"center\"><b>%d</b></td></tr>", a, dbMapBInfoStatic[0] >> 1, a, dbMapBInfoStatic[0] >> 1 );
 svSendPrintf( cnt, "<tr><td height=\"%d\" align=\"right\" valign=\"bottom\"><b>%d</b></td><td height=\"%d\" align=\"left\" valign=\"bottom\"><b>%d</b></td></tr>", a, dbMapBInfoStatic[0], a, dbMapBInfoStatic[0] );

 svSendPrintf( cnt, "<tr><td>&nbsp;</td><td align=\"left\"><b>0</b></td><td align=\"center\"><b>%d</b></td><td align=\"right\"><b>%d</b></td><td>&nbsp;</td></tr>", dbMapBInfoStatic[0] >> 1, dbMapBInfoStatic[0] );
 svSendString( cnt, "</table>" );

 svSendString( cnt, "<br><form action=\"mappick\" method=\"GET\"><select name=\"size\">" );
 for( a = 0 ; a < 6 ; a++ )
 {
  svSendPrintf( cnt, "<option value=\"%d\"", sizes[a] );
  if( ( maind.config_mapsize & 0xFFFF ) == sizes[a] )
   svSendString( cnt, " selected" );
  svSendPrintf( cnt, ">Galaxy sectors of %d by %d", sizes[a], sizes[a] );
 }
 svSendString( cnt, "</select>&nbsp;&nbsp;&nbsp;&nbsp;<input type=\"submit\" value=\"Change\"></form>" );

 iohttpBodyEnd( cnt );
 return;
}

void iohttpFunc_mapadv( svConnectionPtr cnt )
{
 int a, b, id;
 dbUserMainDef maind;
 static unsigned char *advopt[7] = { ">Unused", ">Your planets", ">Your portals", ">Your empire", ">Planets of faction :", ">Planets of empire :", ">Unexplored planets" };
 int advoptbase[IOHTTP_MAPADV_ENTRIES] = { 1, 2, 0, 0, 0, 0, 0, 0 };
 static unsigned char *advcol[6] = { ">Green", ">Blue", ">Red", ">Green merged", ">Blue merged", ">Red merged" };
 int advcolbase[IOHTTP_MAPADV_ENTRIES] = { 1, 0, 0, 0, 0, 0, 0, 0 };
 int advnumbase[IOHTTP_MAPADV_ENTRIES] = { -1, -1, -1, -1, -1, -1, -1, -1 };

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Map generation" );

 for( a = 0 ; a < IOHTTP_MAPADV_ENTRIES ; a++ )
 {
  advoptbase[a] = maind.config_map[a] & 0xF;
  advcolbase[a] = ( maind.config_map[a] >> 4 ) & 0xF;
  if( ( advoptbase[a] == 4 ) || ( advoptbase[a] == 5 ) )
   advnumbase[a] = maind.config_map[a] >> 8;
 }

 svSendString( cnt, "<form action=\"map\" method=\"GET\"><table cellspacing=\"6\"><tr><td>#</td><td>Planets list</td><td>Details</td><td>Colour</td></tr>" );
 for( a = 0 ; a < IOHTTP_MAPADV_ENTRIES ; a++ )
 {
  svSendPrintf( cnt, "<tr><td>%d</td><td><select name=\"e%d\">", a+1, a );
  for( b = 0 ; b < 7 ; b++ )
  {
   svSendPrintf( cnt, "<option value=\"%d\"", b );
   if( advoptbase[a] == b )
    svSendString( cnt, " selected" );
   svSendString( cnt, advopt[b] );
  }
  svSendString( cnt, "</select></td>" );
  svSendPrintf( cnt, "<td><input type=\"text\" name=\"u%d\" size=\"16\"", a );
  if( (unsigned int)advnumbase[a] < 0x10000 )
   svSendPrintf( cnt, " value=\"%d\"", advnumbase[a] );
  svSendString( cnt, "></td>" );
  svSendPrintf( cnt, "<td><select name=\"c%d\">", a );
  for( b = 0 ; b < 6 ; b++ )
  {
   svSendPrintf( cnt, "<option value=\"%d\"", b );
   if( advcolbase[a] == b )
    svSendString( cnt, " selected" );
   svSendString( cnt, advcol[b] );
  }
  svSendString( cnt, "</select></td></tr>" );
 }
 svSendString( cnt, "<tr><td colspan=\"4\" align=\"center\"><table cellspacing=\"0\" cellpadding=\"0\"><tr><td><input type=\"checkbox\" name=\"newwin\"" );
 if( maind.config_mapsize & 0x10000 )
  svSendString( cnt, " checked" );
 svSendString( cnt, "> Open system views in new windows<br><input type=\"checkbox\" name=\"setdefault\"> Save these settings as the default</td></tr></table></td></tr>" );
 svSendString( cnt, "<tr><td colspan=\"4\" align=\"center\"><input type=\"submit\" value=\"Generate map\"></td></tr>" );
 svSendString( cnt, "</table></form>" );

 svSendString( cnt, "<i>Note : The details fields are used to specify exact faction names or ID, or empire numbers.</i><br><br><br>" );

 svSendString( cnt, "<b>Reset map defaults to</b><br><table><tr><td>" );
 svSendString( cnt, "<a href=\"map?e0=1&c0=0&newwin=1&setdefault=1\">Your planets in green</a><br>" );
 svSendString( cnt, "<a href=\"map?e0=1&c0=1&e1=2&c1=0&newwin=1&setdefault=1\">Your planets in blue and portals in green</a><br>" );
 svSendString( cnt, "<a href=\"map?e0=3&c0=2&e1=1&c1=1&e2=2&c2=0&newwin=1&setdefault=1\">Your empire in red, your planets in blue and portals in green</a><br>" );
 svSendString( cnt, "</td></tr></table>" );

 iohttpBodyEnd( cnt );
 return;
}


void iohttpFunc_system( svConnectionPtr cnt )
{
 int a, id, sysid, plnid;
 dbUserMainDef maind, main2d;
 dbMainSystemDef systemd;
 dbMainPlanetDef planetd;
 unsigned char *systemstring;

 int b, c, d, ln, lns[4], pics[64];
 float fa;

 iohttpBase( cnt, 1 );

 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;

 iohttpVarsInit( cnt );
 systemstring = iohttpVarsFind( "id" );
 iohttpVarsCut();

 if( !( systemstring ) || ( sscanf( systemstring, "%d", &sysid ) <= 0 ) || ( dbMapRetrieveSystem( sysid, &systemd ) < 0 ) )
 {
  svSendString( cnt, "This system doesn't seem to exist!</body></html>" );
  return;
 }

 srand( sysid );
 for( a = 0 ; a < systemd.numplanets ; a++ )
  pics[a] = rand() & 0xF;

 svSendPrintf( cnt, "<b>System %d,%d</b><br>", systemd.position & 0xFFFF, systemd.position >> 16 );

 if( systemd.empire != -1 )
  svSendPrintf( cnt, "<b>Home of <a href=\"empire?id=%d\">empire #%d</a></b><br><br>", systemd.empire, systemd.empire );


 plnid = systemd.indexplanet;

 svSendString( cnt, "<table width=\"100%\" cellspacing=\"6\" cellpadding=\"6\">" );
 for( a = b = ln = 0 ; a < systemd.numplanets ; a++, plnid++ )
 {
  if( !( b ) )
  {
   svSendString( cnt, "<tr><td><table width=\"100%\"><tr>" );
   ln = ( rand() % 3 ) + 2;
   for( c = d = 0 ; c < ln ; c++ )
   {
    lns[c] = 2 + ( rand() & 15 );
    d += lns[c];
   }
   fa = 100.0 / (float)d;
   for( c = 0 ; c < ln ; c++, d += lns[c] )
    lns[c] = (int)( (float)lns[c] * fa );
  }

  svSendPrintf( cnt, "<td align=\"center\" width=\"%d%%\">", lns[b] );
  dbMapRetrievePlanet( plnid, &planetd );
  svSendPrintf( cnt, "<a href=\"planet?id=%d\"><img src=\"p%02d.gif\" border=\"0\"></a><br>", plnid, pics[a] );

  if( planetd.owner == -1 )
  {
   svSendPrintf( cnt, "<font color=\"#8090A0\">Planet %d unexplored</font>", a );
   goto iohttpFunc_systemL0;
  }

  if( dbUserMainRetrieve( planetd.owner, &main2d ) < 0 )
   goto iohttpFunc_systemL0;

  svSendPrintf( cnt, "Planet %d owned by <a href=\"player?id=%d\">%s</a> of <a href=\"empire?id=%d\">empire #%d</a><br>", a, planetd.owner, main2d.faction, main2d.empire, main2d.empire );

  iohttpFunc_systemL0:
   svSendString( cnt, "</td>" );
  b = ( b + 1 ) % ln;
  if( !( b ) )
   svSendString( cnt, "</tr></td></table></tr>" );
 }

 if( b )
  svSendString( cnt, "</tr></td></table></tr>" );
 svSendString( cnt, "</table><br><br>" );

 svSendString( cnt, "</center></body></html>" );

 return;
}


void iohttpFunc_player( svConnectionPtr cnt )
{
 int a, b, id, playerid;
 dbUserMainDef maind, main2d;
 unsigned char *playerstring;
 dbUserDescDef descd;
 dbUserRecordPtr recordd;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 2 ) ) >= 0 )
 {
  if( !( iohttpHeader( cnt, id, &maind ) ) )
   return;
 }

 iohttpVarsInit( cnt );
 playerstring = iohttpVarsFind( "id" );
 iohttpVarsCut();

 if( !( playerstring ) || ( sscanf( playerstring, "%d", &playerid ) <= 0 ) || ( dbUserMainRetrieve( playerid, &main2d ) < 0 ) )
 {
  svSendString( cnt, "Are you sure this user exists?</body></html>" );
  return;
 }
 iohttpBodyInit( cnt, main2d.faction );
 svSendPrintf( cnt, "<table border=\"0\"><tr><td><a href=\"empire?id=%d\">Empire : #%d</a><br>Networth : %lld<br>Planets : %d<br>Race : %s<br>Forum tag : <b>%s</b><br>Population : %lld0<br>Home planet : %d,%d:%d<br><br>Faction ID : %d<br><a href=\"mail?to=%d\">Send a message</a><br><a href=\"map?e0=4&u0=%d&c0=5\">Display planets on map</a><br><a href=\"map?e0=1&u0=&c0=3&e1=4&u1=%d&c1=5\">Display planets on map with yours</a><br><a href=\"playerlist?id=%d\">See planets list</a><br><br></td></tr></table>", main2d.empire, main2d.empire, main2d.networth, main2d.planets, cmdRaceName[main2d.raceid], main2d.forumtag, main2d.ressource[CMD_RESSOURCE_POPULATION], ( main2d.home >> 8 ) & 0xFFF, main2d.home >> 20, main2d.home & 0xFF, playerid, playerid, playerid, playerid, playerid );

 if( dbUserDescRetrieve( playerid, &descd ) >= 0 )
 {
  svSendString( cnt, "<i>Faction description</i><br><table border=\"0\"><tr><td>" );
  svSendString( cnt, descd.desc );
  svSendString( cnt, "</td></tr></table>" );
 }

 if( ( b = dbUserRecordList( playerid, &recordd ) ) >= 0 )
 {
  svSendString( cnt, "<table border=\"0\"><tr><td><br><i>Player records</i><br>" );
  svSendPrintf( cnt, "Rounds played : %d", b );
  for( a = b-1 ; a >= 0 ; a-- )
  {
   svSendPrintf( cnt, "<br><br>Round : %d", recordd[a].roundid );
   svSendPrintf( cnt, "<br>Faction name : %s", recordd[a].faction );
   svSendPrintf( cnt, "<br>Title : %s", recordd[a].forumtag );
   svSendPrintf( cnt, "<br>Planets : %d", recordd[a].planets );
   svSendPrintf( cnt, "<br>Networth : %d", recordd[a].networth );
   svSendPrintf( cnt, "<br>Faction rank : %d", recordd[a].rank );
   svSendPrintf( cnt, "<br>Empire : #%d", recordd[a].empire );
   svSendPrintf( cnt, "<br>Empire name : %s", recordd[a].famname );
   svSendPrintf( cnt, "<br>Empire rank : %d", recordd[a].famrank );
   svSendPrintf( cnt, "<br>Empire planets : %d", recordd[a].famplanets );
   svSendPrintf( cnt, "<br>Empire networth : %d", recordd[a].famnetworth );
   svSendPrintf( cnt, "<br>Empire artefacts : %d", recordd[a].artefacts );
  }
  svSendString( cnt, "</td></tr></table>" );
  free( recordd );
 }

 iohttpBodyEnd( cnt );
 return;
}

void iohttpFunc_playerlist( svConnectionPtr cnt )
{
 int a, num, id, playerid;
 int *buffer;
 dbUserMainDef maind, main2d;
 unsigned char *playerstring;

 iohttpBase( cnt, 1 );
 
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) >= 0 )
 {
  if( !( iohttpHeader( cnt, id, &maind ) ) )
   return;
 }

 iohttpVarsInit( cnt );
 playerstring = iohttpVarsFind( "id" );
 iohttpVarsCut();

 if( !( playerstring ) || ( sscanf( playerstring, "%d", &playerid ) <= 0 ) || ( dbUserMainRetrieve( playerid, &main2d ) < 0 ) )
 {
  svSendString( cnt, "Are you sure this user exists?</body></html>" );
  return;
 }
 iohttpBodyInit( cnt, main2d.faction );

 svSendString( cnt, "Planets list<br>" );

 if( ( num = dbUserPlanetListCoords( playerid, &buffer ) ) < 0 )
 {
  svSendString( cnt, "Are you sure this user exists?</body></html>" );
  iohttpBodyEnd( cnt );
  return;
 }
 if( num )
 {
  num--;
  for( a = 0 ; ; a++ )
  {
   svSendPrintf( cnt, "%d,%d:%d", ( buffer[a] >> 8 ) & 0xFF, buffer[a] >> 20, buffer[a] & 0xFF );
   if( a >= num )
    break;
   if( ( a & 7 ) == 7 )
    svSendString( cnt, "<br>" );
   else
    svSendString( cnt, " &nbsp; " );
  }
 }
 free( buffer );

 iohttpBodyEnd( cnt );
 return;
}



void iohttpFunc_planet( svConnectionPtr cnt )
{
 int a, b, cmd[3], cmd2[2], id, plnid, plgive;
 char szString[450];
 char szTemp[100];
 dbUserMainDef maind, main2d;
 dbMainEmpireDef empired;
 dbMainPlanetDef planetd;
 dbUserPtr user2;
 unsigned char *planetstring;
 unsigned char *unstationstring;
 unsigned char *plgivestring;
 static unsigned char *bonusname[4] = { "Solar energy", "Mineral", "Crystal", "Ectrolium" };

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;

 iohttpVarsInit( cnt );
 planetstring = iohttpVarsFind( "id" );
 unstationstring = iohttpVarsFind( "unstation" );
 plgivestring = iohttpVarsFind( "plgive" );
 iohttpVarsCut();

 if( !( planetstring ) || ( sscanf( planetstring, "%d", &plnid ) <= 0 ) || ( dbMapRetrievePlanet( plnid, &planetd ) < 0 ) )
 {
  svSendString( cnt, "This planet doesn't seem to exist!</body></html>" );
  return;
 }
 plgive = -2;
 if( plgivestring )
  sscanf( plgivestring, "%d", &plgive );

 b = planetd.position & 0xFF;
 svSendPrintf( cnt, "<b>Planet %d,%d:%d</b><br><br>", ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, b );

 if( plgive != -2 )
 {
  svSendPrintf( cnt, "<i>Planet offer setting changed</i><br><br>" );
  cmdExecOfferPlanet( id, plgive, plnid );
  planetd.surrender = plgive;
 }

 srand( planetd.system );
 for( a = 0 ; a < b ; a++ )
  rand();
 svSendPrintf( cnt, "<img src=\"p%02d.gif\" border=\"0\"><br><br>", rand() & 0xF );

 if( unstationstring )
 {
  cmd[0] = CMD_UNSTATION_PLANET;
  cmd[1] = id;
  cmd[2] = plnid;
  if( cmdExecute( cnt, cmd, 0, 0 ) < 0 )
   svSendString( cnt, "Error encountered while unstationing forces<br><br>" );
  else
   svSendString( cnt, "<i>Fleet unstationed</i><br><br>" );
  memset( planetd.unit, 0, CMD_UNIT_NUMUSED*sizeof(int) );
 }

 if( planetd.owner == -1 )
 {
  if( planetd.flags & CMD_PLANET_FLAGS_HOME )
   svSendPrintf( cnt, "No one owns this planet, but it is part of a home system and unavailable for exploration" );
svSendPrintf( cnt, "No one owns this planet, it is free to explore.<br><br><a href=\"explore?id=%d\">Explore this planet</a><br><br><a href=\"spec?id=%d\">Special operation</a>", plnid, plnid );
 }
 else if( planetd.owner == id )
 {
  svSendPrintf( cnt, "This planet is yours.<br><br>Population : %d0<br>", planetd.population );
  
  if(planetd.special[1])
   svSendPrintf( cnt, "%s production : <font color=\"#20FF20\">+%d%%</font><br>", bonusname[planetd.special[0]], planetd.special[1] );
  
  svSendPrintf( cnt, "<SCRIPT type=\"text/JavaScript\">\n ");
  sprintf(szString, " function Areyousure(plnid)\n{if(confirm(\"Are you sure you want to raze eveything on this planet??\"))open(\"raze?id=\"+plnid+\"");
  for( b = 0 ; b < CMD_BLDG_NUMUSED ; b++ )
	 {
	  sprintf(szTemp, "&bldg%d=%d", b, planetd.building[b]);
	  strcat(szString, szTemp);
	 }
  strcat(szString, "\",\"_self\");\n}");
  svSendPrintf( cnt, szString);
		
  svSendPrintf( cnt,"</SCRIPT>\n"); 

  svSendPrintf( cnt, "<br><a href=\"build?id=%d\">Build on this planet</a><br><br><a href=\"station?id=%d\">Station on this planet</a><br><br><a href=\"raze?id=%d\">Raze buildings</a><br><br><a href=# onClick=\"Areyousure(%d)\">Raze all buildings</a><br>", plnid, plnid, plnid, plnid );

  
  if( ( dbMapRetrieveEmpire( maind.empire, &empired ) >= 0 ) && ( empired.numplayers >= 2 ) && !( planetd.flags & CMD_PLANET_FLAGS_HOME ) )
  {
   svSendString( cnt, "<form action=\"planet\" method=\"POST\">Offer this planet to :" );
   svSendPrintf( cnt, "<input type=\"hidden\" value=\"%d\" name=\"id\">", plnid );
   svSendString( cnt, "<select name=\"plgive\">" );
   svSendPrintf( cnt, "<option value=\"%d\">%s", -1, "No one" );
   for( a = b = 0 ; a < empired.numplayers ; a++ )
   {
    if( !( user2 = dbUserLinkID( empired.player[a] ) ) )
     continue;
    if( user2->id == id )
     continue;
    if( user2->id != planetd.surrender )
    {
     svSendPrintf( cnt, "<option value=\"%d\">%s", user2->id, user2->faction );
     continue;
    }
    svSendPrintf( cnt, "<option value=\"%d\" selected>%s", user2->id, user2->faction );
    b = 1;
   }
   svSendString( cnt, "</select><br>" );
   svSendString( cnt, "<input type=\"submit\" value=\"Confirm\"></form>" );
  }

  svSendString( cnt, "<table width=\"90%\"><tr><td width=\"45%\" align=\"center\" valign=\"top\"><b>Buildings</b><br><br>" );
  for( a = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
  {
   if( planetd.building[a] )
    svSendPrintf( cnt, "%d %s<br>", planetd.building[a], cmdBuildingName[a] );
  }
  if( planetd.flags & CMD_PLANET_FLAGS_PORTAL )
   svSendString( cnt, "Portal<br>" );
  if( planetd.construction )
   svSendPrintf( cnt, "<br>%d additional buildings are under construction", planetd.construction );
  svSendString( cnt, "</td><td width=\"45%\" align=\"center\" valign=\"top\"><b>Units</b><br><br>" );
  for( a = b = 0 ; a < CMD_UNIT_NUMUSED ; a++ )
  {
   if( planetd.unit[a] )
   {
    svSendPrintf( cnt, "%d %s<br>", planetd.unit[a], cmdUnitName[a] );
    b = 1;
   }
  }
  if( b )
   svSendPrintf( cnt, "<a href=\"planet?id=%d&unstation=1\">Unstation fleet</a>", plnid );
  svSendString( cnt, "</td></tr></table>" );
 }
 else
 {
  cmd2[0] = CMD_RETRIEVE_USERMAIN;
  cmd2[1] = planetd.owner;
  if( cmdExecute( cnt, cmd2, &main2d, 0 ) < 0 )
   svSendString( cnt, "Error while retriving user's main data" );
  else
   svSendPrintf( cnt, "This planet is owned by : <a href=\"player?id=%d\">%s</a> of <a href=\"empire?id=%d\">empire #%d</a>, networth %lld.<br><br><a href=\"attack?id=%d\">Attack this planet</a><br><br><a href=\"spec?id=%d\">Special operation</a>", planetd.owner, main2d.faction, main2d.empire, main2d.empire, main2d.networth, plnid, plnid );
 }
 svSendPrintf( cnt, "<br><br><a href=\"system?id=%d\">View system</a>", planetd.system );

 svSendString( cnt, "</center></body></html>" );

 return;
}


void iohttpFunc_pltake( svConnectionPtr cnt )
{
 int id, plnid;
 dbMainPlanetDef planetd;
 dbUserMainDef maind;
 unsigned char *planetstring;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;

 iohttpVarsInit( cnt );
 planetstring = iohttpVarsFind( "id" );
 iohttpVarsCut();

 if( !( planetstring ) || ( sscanf( planetstring, "%d", &plnid ) <= 0 ) || ( dbMapRetrievePlanet( plnid, &planetd ) < 0 ) )
 {
  svSendString( cnt, "This planet doesn't seem to exist!</body></html>" );
  return;
 }

 svSendPrintf( cnt, "<b>Planet %d,%d:%d</b><br><br>", ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, planetd.position & 0xFF );

 if( planetd.owner == id )
  svSendString( cnt, "You already own this planet!<br>" );
 else if( cmdExecTakePlanet( id, plnid ) >= 0 )
  svSendString( cnt, "You took control of the planet!<br>" );
 else
 {
  if( cmdErrorString )
   svSendPrintf( cnt, "<i>%s</i><br><br>", cmdErrorString );
  else
   svSendString( cnt, "<i>You couldn't take control of the planet, the owner must have cancelled the offer.</i><br><br>" );
 }

 svSendString( cnt, "</center></body></html>" );

 return;
}




void iohttpFunc_build( svConnectionPtr cnt )
{
 int a, b, c, id, cmd[6], plnid, totalb;
 long long int resbuild[CMD_RESSOURCE_NUMUSED+2];
 float fa;
 dbUserMainDef maind;
 dbMainPlanetDef planetd;
 unsigned char *buildbldg[CMD_BLDG_NUMUSED];
 unsigned char *buildplnid;
 unsigned char buildname[16];
 unsigned char buildstring[CMD_BLDG_NUMUSED+1][128];
 unsigned char *portalstring;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;

 iohttpVarsInit( cnt );
 for( a = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
 {
  sprintf( buildname, "bldg%d", a );
  buildbldg[a] = iohttpVarsFind( buildname );
 }
 buildplnid = iohttpVarsFind( "id" );
 portalstring = iohttpVarsFind( "portal" );
 iohttpVarsCut();

 cmd[0] = CMD_RETRIEVE_PLANET;
 if( !( buildplnid ) || ( sscanf( buildplnid, "%d", &cmd[1] ) <= 0 ) || ( cmdExecute( cnt, cmd, &planetd, 0 ) < 0 ) )
 {
  svSendString( cnt, "This planet doesn't seem to exist!</body></html>" );
  return;
 }
 if( planetd.owner != id )
 {
  svSendString( cnt, "You can't build on a planet that you do not own!</body></html>" );
  return;
 }
 plnid = cmd[1];

 cmd[0] = CMD_ADD_BUILD;
 cmd[1] = id;
 cmd[4] = plnid;
 cmd[5] = 0xFFFFFF;
 for( a = 0 ; a < CMD_BLDG_NUMUSED+1 ; a++ )
  buildstring[a][0] = 0;
 for( a = c = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
 {
  if( !( buildbldg[a] ) )
   continue;
  if( sscanf( buildbldg[a], "%d", &b ) <= 0 )
   continue;
  if( b <= 0 )
   continue;
  cmd[2] = a;
  cmd[3] = b;
  b = cmdExecAddBuild( cmd[1], cmd[2], cmd[3], cmd[4], cmd[5] );
/*
  b = cmdExecute( cnt, cmd, 0, 0 );
*/
  if( b >= 0 )
  {
   sprintf( buildstring[a], "<i>You started building %d %s</i><br><br>", cmd[3], cmdBuildingName[a] );
   c++;
  }
  else if( cmdErrorString )
   sprintf( buildstring[a], "<i>%s</i><br><br>", cmdErrorString );
  else
   sprintf( buildstring[a], "<i>Error while adding build item</i><br>" );
 }
 if( portalstring )
 {
  cmd[0] = CMD_ADD_BUILD;
  cmd[1] = id;
  cmd[2] = CMD_BLDG_NUMUSED;
  cmd[3] = 1;
  cmd[4] = plnid;
  b = cmdExecute( cnt, cmd, 0, 0 );
  if( b >= 0 )
  {
   sprintf( buildstring[CMD_BLDG_NUMUSED], "<i>You started building a portal</i><br><br>" );
   c++;
  }
  else if( cmdErrorString )
   sprintf( buildstring[CMD_BLDG_NUMUSED], "<i>%s</i><br><br>", cmdErrorString );
  else
   sprintf( buildstring[CMD_BLDG_NUMUSED], "<i>Error while adding portal build item</i><br><br>" );
 }

 if( c )
 {
  cmd[0] = CMD_RETRIEVE_PLANET;
  cmd[1] = plnid;
  cmdExecute( cnt, cmd, &planetd, 0 );
 }

 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Build on planet %d,%d:%d", ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, planetd.position & 0xFF );
 svSendString( cnt, "<a href=\"stats\">Building Information</a><br><br>" );
 for( a = 0 ; a < CMD_BLDG_NUMUSED+1 ; a++ )
 {
  if( buildstring[a][0] )
   svSendString( cnt, buildstring[a] );
 }

 svSendPrintf( cnt, "<form action=\"build\" method=\"POST\"><input type=\"hidden\" value=\"%d\" name=\"id\">", plnid );

 totalb = planetd.construction;
 for( a = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
  totalb += planetd.building[a];
 if( totalb )
 {
  svSendPrintf( cnt, "There are %d buildings on this planet", totalb );
  if( planetd.construction )
   svSendPrintf( cnt, ", %d of them are still under construction", planetd.construction );
  svSendString( cnt, ". " );
 }
 svSendPrintf( cnt, "This planet can support %d buildings, additional ones will be very expensive.<br>", planetd.size );

 for( a = CMD_BLDG_OVERBUILD ; a < CMD_BLDG_NUMUSED ; a++ )
  totalb -= planetd.building[a];
 if( ( fa = cmdGetBuildOvercost( planetd.size, totalb ) ) != 1.0 )
 {
  svSendPrintf( cnt, "The overbuilding penalty for this planet is <font color=\"#FF2020\">+%.2f%%</font><br>", (fa-1.0)*100.0 );
 }

 svSendString( cnt, "<br><table cellspacing=\"6\"><tr><td><b>Building</b></td><td><b>Cost</b></td><td><b>Owned</b></td><td><b>Build</b></td></tr>" );
 for( a = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
 {
  cmdGetBuildCosts( &maind, a, resbuild );
  if( !( cmdBuildingFlags[a] ) )
   cmdGetBuildOverbuild( planetd.size, totalb, resbuild );
		
  svSendPrintf( cnt, "<tr><td valign=\"top\"><font color=\"#FFFFFF\">%s</font><br></td><td valign=\"top\" nowrap>", cmdBuildingName[a] );
  if( resbuild[0] < 0 )
  {
   svSendPrintf( cnt, "<font color=\"#FF0000\">Unavailable<br>Tech too low</font></td><td valign=\"top\" align=\"center\">%lld</td><td>&nbsp;</td></tr>", maind.totalbuilding[a] );
   continue;
  }
  for( b = 0 ; b < CMD_RESSOURCE_NUMUSED+1 ; b++ )
  {
   if( resbuild[b] )
    svSendPrintf( cnt, "%lld %s<br>", resbuild[b], cmdRessourceName[b] );
  }
  if( resbuild[b] )
   svSendPrintf( cnt, "<font color=\"#FF2020\">Tech : +%lld%% costs</font><br>", resbuild[b] );
  svSendPrintf( cnt, "</td><td valign=\"top\" align=\"center\">%lld</td><td valign=\"top\"><input type=\"text\" name=\"bldg%d\" size=\"10\"></td></tr>", maind.totalbuilding[a], a );
 }

 if( planetd.flags & CMD_PLANET_FLAGS_PORTAL )
  svSendString( cnt, "<tr><td valign=\"top\" colspan=\"2\"><font color=\"#FFFFFF\">Portal</font><br>Already present" );
 else if( planetd.flags & CMD_PLANET_FLAGS_PORTAL_BUILD )
  svSendString( cnt, "<tr><td valign=\"top\" colspan=\"2\"><font color=\"#FFFFFF\">Portal</font><br>Under construction" );
 else
 {
  cmdGetBuildCosts( &maind, CMD_BLDG_NUMUSED, resbuild );
  svSendPrintf( cnt, "<tr><td valign=\"top\"><font color=\"#FFFFFF\">Portal</font></td><td valign=\"top\" nowrap>" );
  if( resbuild[0] < 0 )
   svSendPrintf( cnt, "<font color=\"#FF0000\">Unavailable<br>Tech too low</font></td><td valign=\"top\" align=\"center\">%lld</td><td>&nbsp;</td></tr>", maind.totalbuilding[CMD_BLDG_NUMUSED] );
  else
  {
   for( b = 0 ; b < CMD_RESSOURCE_NUMUSED+1 ; b++ )
   {
    if( resbuild[b] )
     svSendPrintf( cnt, "%lld %s<br>", resbuild[b], cmdRessourceName[b] );
   }
   if( resbuild[b] )
    svSendPrintf( cnt, "<font color=\"#FF2020\">Tech : +%lld%% costs</font><br>", resbuild[b] );
  }
 }
 svSendPrintf( cnt, "</td><td valign=\"top\" align=\"center\">%lld</td><td valign=\"top\">", maind.totalbuilding[CMD_BLDG_NUMUSED] );
 if( !( planetd.flags & ( CMD_PLANET_FLAGS_PORTAL | CMD_PLANET_FLAGS_PORTAL_BUILD ) ) )
  svSendString( cnt, "<input type=\"checkbox\" name=\"portal\"> Build a portal" );
 else
  svSendString( cnt, "&nbsp;" );
 svSendString( cnt, "</td></tr>" );

 svSendString( cnt, "</table><input type=\"submit\" value=\"Build\"></form>" );
 svSendPrintf( cnt, "<br><br><a href=\"planet?id=%d\">View planet</a>", plnid );
 svSendPrintf( cnt, "<br><br><a href=\"system?id=%d\">View system</a><br><br>", planetd.system );

 iohttpBodyEnd( cnt );
 return;
}

void iohttpFunc_cancelbuild(svConnectionPtr cnt)
{
	int id, i, j, nNbr=0, nTotalBuild, nTotal[4];
	int *nBuildp=0, *nTemp=0;
	dbUserMainDef maind;
	dbUserBuildPtr buildp;
	dbMainPlanetDef planetd;
	char buildid[10];
	unsigned char *cBuild;
	
	iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
	memset(&nTotal, 0, 4*sizeof(int));
	nTotalBuild = dbUserBuildList(id, &buildp);
	
 iohttpVarsInit( cnt );
 nBuildp = NULL;
 for( i=0; i<nTotalBuild; i++)
 {
  sprintf( buildid, "b%d", i );
  cBuild = iohttpVarsFind( buildid );
  if(cBuild)
  {
  	if(cBuild[1] == 'n')
  	{
	  	if(nNbr)
	  	{
	  		nTemp = malloc(nNbr*sizeof(int));
	  		memcpy(nTemp, nBuildp, nNbr*sizeof(int));
	  		free(nBuildp);
	  	}
	  	nBuildp = malloc((nNbr+1) * sizeof(int));
	  	if(nNbr)
	  	{
	  		memcpy(nBuildp, nTemp, (nNbr)*sizeof(int));
	  	}
	  	nBuildp[nNbr] = i;
	  	nNbr++;
	  }
  }
 }
 iohttpVarsCut();
	
	dbUserMainRetrieve(id, &maind);
	
	for(i=nNbr-1;i>=0;i--)
 {
 	for(j=0;j<4;j++)
 	{
 		nTotal[j] += (buildp[nBuildp[i]]).cost[j] / 2;
 	}
 	dbUserBuildRemove(id, nBuildp[i]);
 	if( !( buildp[nBuildp[i]].type >> 16 ) )
   {
    dbMapRetrievePlanet( buildp[nBuildp[i]].plnid, &planetd );
    if( buildp[nBuildp[i]].type == CMD_BLDG_NUMUSED )
    {
     // portal
     planetd.flags = 0;
     dbUserPlanetSetFlags( id, buildp[nBuildp[i]].plnid, planetd.flags );
     planetd.construction--;
    }
    else
    {
     planetd.construction -= buildp[nBuildp[i]].quantity;
    }
    dbMapSetPlanet( buildp[nBuildp[i]].plnid, &planetd );
   }
 }
 
 for(i=0;i<4;i++)
 	maind.ressource[i] += (nTotal[i]);
 
 dbUserMainSet(id, &maind);
 
		
	 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Cancelling %d buildings or units", nNbr);
 if(nNbr)
 {
	 	svSendPrintf(cnt, "%d building or unit under construction have been remove<br>", nNbr);
	 	svSendPrintf(cnt, "<table><tr><td>You were refunded with:</td><td></td><td></td></tr>");
	 	for(i=0;i<4;i++)
	 	{
	 		if(nTotal[i])
	 			svSendPrintf(cnt, "<tr><td></td><td>%d</td><td>%s</td></tr>", nTotal[i], cmdRessourceName[i]);
	 	}
	 	svSendString(cnt, "</table>");
	}
 	svSendString(cnt, "<a href=\"council\">Back</a>");
 free( buildp );
 if(nBuildp)
	  		free(nBuildp);
 iohttpBodyEnd( cnt );
}

#define IOHTTP_MASSBUILD_STRING 16384

void iohttpFunc_massbuild( svConnectionPtr cnt )
{
 int a, b, c, d, e, id, cmd[7], numpl, totalpl, totalportal, buildoff, oblimit, oblimitlow;
 long long int resbuild[CMD_RESSOURCE_NUMUSED+2];
 float fa, cost;
 int *buffer;
 dbUserMainDef maind;
 dbMainPlanetDef planetd;
 unsigned char *buildbldg[CMD_BLDG_NUMUSED];
 unsigned char buildname[16];
 unsigned char buildstring[IOHTTP_MASSBUILD_STRING];
 unsigned char *portalstring;
 unsigned char *oblimitstring;
 unsigned char *oblimitstringlow;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;

 cmd[0] = CMD_RETRIEVE_USERPLISTINDICES;
 cmd[1] = id;
 if( ( numpl = cmdExecute( cnt, cmd, &buffer, 0 ) ) < 0 )
 {
  svSendString( cnt, "Error while retriving planets list" );
  return;
 }

 buildoff = 0;
 buildstring[0] = 0;
 svSendString( cnt, "<form action=\"massbuild\" method=\"POST\">" );

 iohttpVarsInit( cnt );
 cost = 0.0;
 for( a = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
 {
  sprintf( buildname, "bldg%d", a );
  buildbldg[a] = iohttpVarsFind( buildname );
 }
 portalstring = iohttpVarsFind( "portal" );

 oblimit = 0xFFFFFF;
 oblimitlow = 0;
 if( iohttpVarsFind( "obcheck" ) )
 {
  oblimitstring = iohttpVarsFind( "oblimit" );
  oblimitstringlow = iohttpVarsFind( "oblimitlow" );
  if( oblimitstring )
   sscanf( oblimitstring, "%d", &oblimit );
  if( oblimitstringlow )
   sscanf( oblimitstringlow, "%d", &oblimitlow );
 }
 cmd[5] = oblimit;
 cmd[6] = oblimitlow;

 for( a = e = totalpl = totalportal = 0 ; a < numpl ; a++ )
 {
  sprintf( buildname, "m%d", buffer[a] );
  if( !( iohttpVarsFind( buildname ) ) )
   continue;

  cmd[0] = CMD_RETRIEVE_PLANET;
  cmd[1] = buffer[a];
  if( ( cmdExecute( cnt, cmd, &planetd, 0 ) < 0 ) )
   continue;
  totalpl++;

  svSendPrintf( cnt, "<input type=\"hidden\" value=\"on\" name=\"m%d\">", buffer[a] );
  cmd[0] = CMD_ADD_BUILD;
  cmd[1] = id;
  cmd[4] = buffer[a];
  for( c = d = 0 ; c < CMD_BLDG_NUMUSED ; c++ )
  {
   if( !( buildbldg[c] ) )
    continue;
   if( sscanf( buildbldg[c], "%d", &b ) <= 0 )
    continue;
   if( b <= 0 )
    continue;
   cmd[2] = c;
   cmd[3] = b;
/*
cmdExecAddBuild !!!
*/
   b = cmdExecute( cnt, cmd, 0, 0 );
   e = 1;
   if( buildoff >= IOHTTP_MASSBUILD_STRING - 256 )
    continue;
   if( b == 0 )
    buildoff += sprintf( &buildstring[buildoff], "<i>No additionnal %s were built", cmdBuildingName[c] );
   else if( b > 0 )
   {
    buildoff += sprintf( &buildstring[buildoff], "<i>You started building %d %s", b, cmdBuildingName[c] );
    d += b;
   }
   else if( cmdErrorString )
    buildoff += sprintf( &buildstring[buildoff], "<i>%s", cmdErrorString );
   else
    buildoff += sprintf( &buildstring[buildoff], "<i>Error while adding build item" );
   buildoff += sprintf( &buildstring[buildoff], " on <a href=\"planet?id=%d\">%d,%d:%d</a></i><br>", buffer[a], ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, planetd.position & 0xFF );
  }
  b = planetd.construction + d;
  for( c = 0 ; c < CMD_BLDG_OVERBUILD ; c++ )
   b += planetd.building[c];
  cost += cmdGetBuildOvercost( planetd.size, b );
  if( ( portalstring ) && !( planetd.flags & ( CMD_PLANET_FLAGS_PORTAL | CMD_PLANET_FLAGS_PORTAL_BUILD ) ) )
  {
   cmd[0] = CMD_ADD_BUILD;
   cmd[1] = id;
   cmd[2] = CMD_BLDG_NUMUSED;
   cmd[3] = 1;
   cmd[4] = buffer[a];
   b = cmdExecute( cnt, cmd, 0, 0 );
   e = 1;
   if( buildoff >= IOHTTP_MASSBUILD_STRING - 256 )
    continue;
   if( b >= 0 )
   {
    buildoff += sprintf( &buildstring[buildoff], "<i>You started building a portal" );
    planetd.flags |= CMD_PLANET_FLAGS_PORTAL_BUILD;
    d = -1;
   }
   else if( cmdErrorString )
    buildoff += sprintf( &buildstring[buildoff], "<i>%s", cmdErrorString );
   else
    buildoff += sprintf( &buildstring[buildoff], "<i>Error while adding portal build item" );
   buildoff += sprintf( &buildstring[buildoff], " on <a href=\"planet?id=%d\">%d,%d:%d</a></i><br>", buffer[a], ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, planetd.position & 0xFF );
  }
  if( planetd.flags & CMD_PLANET_FLAGS_PORTAL )
   totalportal++;
  else if( ( planetd.flags & CMD_PLANET_FLAGS_PORTAL_BUILD ) || ( d == -1 ) )
   totalportal += 1 << 16;
 }

 if( !( iohttpHeader( cnt, id, &maind ) ) )
 {
  free( buffer );
  return;
 }

 iohttpBodyInit( cnt, "Build on %d planets", totalpl );
 svSendString( cnt, "<a href=\"stats\">Building Information</a><br><br>" );
 svSendString( cnt, buildstring );

 svSendString( cnt, "<table><tr><td>" );
 fa = cost / (float)totalpl;
 if( fa > 1.001 )
  svSendPrintf( cnt, "The average overbuilding penalty is <font color=\"#FF2020\">+%.2f%%</font><br>", (fa-1.0)*100.0 );
 svSendString( cnt, "<input type=\"checkbox\" name=\"obcheck\"> build between overbuilding penalty from <input type=\"text\" name=\"oblimitlow\" size=\"4\"> to <input type=\"text\" name=\"oblimit\" size=\"4\"> %" );
 svSendString( cnt, "</td></tr></table>" );

 svSendString( cnt, "<br><table cellspacing=\"6\"><tr><td><b>Building</b></td><td><b>Cost</b></td><td><b>Owned</b></td><td><b>Build</b></td></tr>" );
 for( a = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
 {
  cmdGetBuildCosts( &maind, a, resbuild );
  svSendPrintf( cnt, "<tr><td valign=\"top\"><font color=\"#FFFFFF\">%s</font><br></td><td valign=\"top\" nowrap>", cmdBuildingName[a] );
  if( resbuild[0] < 0 )
  {
   svSendPrintf( cnt, "<font color=\"#FF0000\">Unavailable<br>Tech too low</font></td><td valign=\"top\" align=\"center\">%lld</td><td>&nbsp;</td></tr>", maind.totalbuilding[a] );
   continue;
  }
  for( b = 0 ; b < CMD_RESSOURCE_NUMUSED ; b++ )
  {
   if( resbuild[b] )
    svSendPrintf( cnt, "%lld %s<br>", (long long int)( (double)resbuild[b] * cost ), cmdRessourceName[b] );
  }
  svSendPrintf( cnt, "%lld %s<br>", resbuild[b], cmdRessourceName[b] );
  b++;
  if( resbuild[b] )
   svSendPrintf( cnt, "<font color=\"#FF2020\">Tech : +%lld%% costs</font><br>", resbuild[b] );
  svSendPrintf( cnt, "</td><td valign=\"top\" align=\"center\">%lld</td><td valign=\"top\"><input type=\"text\" name=\"bldg%d\" size=\"10\"></td></tr>", maind.totalbuilding[a], a );
 }

 a = ( totalportal & 0xFFFF ) + ( totalportal >> 16 );
 b = 0;
 if( totalpl == totalportal )
  svSendString( cnt, "<tr><td valign=\"top\" colspan=\"2\"><font color=\"#FFFFFF\">Portal</font><br>All present" );
 else if( totalpl == a )
  svSendString( cnt, "<tr><td valign=\"top\" colspan=\"2\"><font color=\"#FFFFFF\">Portal</font><br>Under construction" );
 else
 {
  b = totalpl - a;
  cmdGetBuildCosts( &maind, CMD_BLDG_NUMUSED, resbuild );
  svSendPrintf( cnt, "<tr><td valign=\"top\"><font color=\"#FFFFFF\">Portal ( %d )</font></td><td valign=\"top\" nowrap>", b );
  if( resbuild[0] < 0 )
   svSendPrintf( cnt, "<font color=\"#FF0000\">Unavailable<br>Tech too low</font></td><td valign=\"top\" align=\"center\">%lld</td><td>&nbsp;</td></tr>", maind.totalbuilding[CMD_BLDG_NUMUSED] );
  else
  {
   for( a = 0 ; a < CMD_RESSOURCE_NUMUSED ; a++ )
   {
    if( resbuild[a] )
     svSendPrintf( cnt, "%lld %s<br>", b*resbuild[a], cmdRessourceName[a] );
   }
   svSendPrintf( cnt, "%lld %s<br>", resbuild[a], cmdRessourceName[a] );
   a++;
   if( resbuild[a] )
    svSendPrintf( cnt, "<font color=\"#FF2020\">Tech : +%lld%% costs</font><br>", resbuild[a] );
  }
 }
 svSendPrintf( cnt, "</td><td valign=\"top\" align=\"center\">%lld</td><td valign=\"top\">", maind.totalbuilding[CMD_BLDG_NUMUSED] );
 if( b )
  svSendString( cnt, "<input type=\"checkbox\" name=\"portal\"> Build portals" );
 else
  svSendString( cnt, "&nbsp;" );
 svSendString( cnt, "</td></tr>" );

 svSendString( cnt, "</table><input type=\"submit\" value=\"Build\"></form>" );

 free( buffer );
 iohttpBodyEnd( cnt );
 return;
}

/*
//Use to chat on irc
void iohttpFunc_chat( svConnectionPtr cnt )
{
	dbUserMainDef maind;
	int id;
	char *szFaction;
	char szTemp[32];
	
	iohttpBase( cnt, 1 );
	if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpVarsInit( cnt );
	szFaction = iohttpVarsFind("faction");
	iohttpVarsCut();
	
	if(!szFaction)
	{
		sprintf(szTemp,"User%d", id);
		szFaction = szTemp;
	}
	
	iohttpBodyInit( cnt, "Chat %s", szFaction);
	svSendPrintf( cnt, "<b>Have fun chatting %s</b><br>", szFaction);
	svSendString( cnt, "<applet code=IRCApplet.class archive=\"irc.jar,pixx.jar\" width=800 height=600>" );
	svSendString( cnt,"<param name=\"CABINETS\" value=\"irc.cab,securedirc.cab,pixx.cab\">");
	svSendPrintf( cnt, "<param name=\"nick\" value=\"%s\"><param name=\"alternatenick\" value=\"user%d\"><param name=\"name\" value=\"EctroverseChat\"><param name=\"host\" value=\"%s\"><param name=\"gui\" value=\"pixx\"><param name=\"command1\" value=\"join #ectroverse\"></applet>", szFaction, id, IRC_SERVER);
	iohttpBodyEnd( cnt );

	return;
}
*/
void iohttpFunc_raze( svConnectionPtr cnt )
{
 int a, b, c, id, cmd[5], plnid, totalb;
 float fa;
 dbUserMainDef maind;
 dbMainPlanetDef planetd;
 unsigned char *razebldg[CMD_BLDG_NUMUSED];
 unsigned char *razeplnid;
 unsigned char razename[16];
 unsigned char *portalstring;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;

 iohttpVarsInit( cnt );
 for( a = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
 {
  sprintf( razename, "bldg%d", a );
  razebldg[a] = iohttpVarsFind( razename );
 }
 razeplnid = iohttpVarsFind( "id" );
 portalstring = iohttpVarsFind( "portal" );
 iohttpVarsCut();

 cmd[0] = CMD_RETRIEVE_PLANET;
 if( !( razeplnid ) || ( sscanf( razeplnid, "%d", &cmd[1] ) <= 0 ) || ( cmdExecute( cnt, cmd, &planetd, 0 ) < 0 ) )
 {
  svSendString( cnt, "This planet doesn't seem to exist!</body></html>" );
  return;
 }
 if( planetd.owner != id )
 {
  svSendString( cnt, "You can't raze on a planet that you do not own!</body></html>" );
  return;
 }
 plnid = cmd[1];

 iohttpBodyInit( cnt, "Raze buildings on planet %d,%d:%d", ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, planetd.position & 0xFF );

 cmd[0] = CMD_RAZE_BUILDINGS;
 cmd[1] = id;
 cmd[4] = plnid;
 for( a = c = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
 {
  if( !( razebldg[a] ) )
   continue;
  if( sscanf( razebldg[a], "%d", &b ) <= 0 )
   continue;
  if( b <= 0 )
   continue;
  cmd[2] = a;
  cmd[3] = b;
  b = cmdExecute( cnt, cmd, 0, 0 );
  if( b >= 0 )
  {
   svSendPrintf( cnt, "<i>You razed %d %s</i><br><br>", cmd[3], cmdBuildingName[a] );
   c++;
  }
  else if( cmdErrorString )
   svSendPrintf( cnt, "<i>%s</i><br><br>", cmdErrorString );
  else
   svSendString( cnt, "<i>Error while razing building</i><br>" );
 }
 if( portalstring )
 {
  cmd[0] = CMD_RAZE_PORTAL;
  cmd[1] = id;
  cmd[2] = plnid;
  b = cmdExecute( cnt, cmd, 0, 0 );
  if( b >= 0 )
  {
   svSendPrintf( cnt, "<i>You razed the %s</i><br><br>", cmdBuildingName[CMD_BLDG_NUMUSED] );
   c++;
  }
  else if( cmdErrorString )
   svSendPrintf( cnt, "<i>%s</i><br><br>", cmdErrorString );
  else
   svSendString( cnt, "<i>Error while razing portal</i><br><br>" );
 }
 if( c )
 {
  cmd[0] = CMD_RETRIEVE_PLANET;
  cmd[1] = plnid;
  cmdExecute( cnt, cmd, &planetd, 0 );
  cmd[0] = CMD_RETRIEVE_USERMAIN;
  cmd[1] = id;
  cmdExecute( cnt, cmd, &maind, 0 );
 }
 svSendPrintf( cnt, "<form action=\"raze\" method=\"POST\"><input type=\"hidden\" value=\"%d\" name=\"id\">", plnid );
 totalb = planetd.construction;
 for( a = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
  totalb += planetd.building[a];
 if( totalb )
 {
  svSendPrintf( cnt, "There are %d buildings on this planet", totalb );
  if( planetd.construction )
   svSendPrintf( cnt, ", %d of them are still under construction", planetd.construction );
  svSendString( cnt, ". " );
 }
 svSendPrintf( cnt, "This planet can support %d buildings, additional ones will be very expensive.<br>", planetd.size );

 for( a = CMD_BLDG_OVERBUILD ; a < CMD_BLDG_NUMUSED ; a++ )
  totalb -= planetd.building[a];
 if( ( fa = cmdGetBuildOvercost( planetd.size, totalb ) ) != 1.0 )
  svSendPrintf( cnt, "The overbuilding penalty for this planet is %.2f%%<br>", (fa-1.0)*100.0 );
 svSendString( cnt, "<br><br><table width=\"320\" border=\"2\">" );
 for( a = b = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
 {
  if( !( planetd.building[a] ) )
   continue;
  svSendPrintf( cnt, "<tr><td align=\"center\"><b>%s</b><br><br>", cmdBuildingName[a] );
  svSendPrintf( cnt, "<input type=\"text\" name=\"bldg%d\" size=\"10\"><br><br>There are %d on this planet<br>( %lld total )</td></tr>", a, planetd.building[a], maind.totalbuilding[a] );
  b = 1;
 }
 if( planetd.flags & CMD_PLANET_FLAGS_PORTAL )
 {
  svSendPrintf( cnt, "<tr><td align=\"center\"><b>%s</b><br><br>", cmdBuildingName[CMD_BLDG_NUMUSED] );
  svSendPrintf( cnt, "<input type=\"checkbox\" name=\"portal\"> Raze the portal<br><br>" );
  b = 1;
 }
 if( b )
  svSendString( cnt, "</table><br><input type=\"submit\" value=\"Raze\"></form>" );
 else
  svSendString( cnt, "</table><br>There are no buildings on this planet</form>" );
 svSendPrintf( cnt, "<br><br><a href=\"planet?id=%d\">View planet</a>", plnid );
 svSendPrintf( cnt, "<br><br><a href=\"system?id=%d\">View system</a><br><br>", planetd.system );

 iohttpBodyEnd( cnt );
 return;
}


void iohttpFunc_fleets( svConnectionPtr cnt )
{
 int a, b, id, num, x, y;
 dbUserFleetPtr fleetd;
 dbMainPlanetDef planetd;
 dbUserMainDef maind, main2d;
 unsigned char varname[8];
 unsigned char *varstring;
 int szTotal[CMD_UNIT_NUMUSED];

	memset(szTotal, 0, CMD_UNIT_NUMUSED*sizeof(int));
	iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;

 iohttpVarsInit( cnt );
 varstring = iohttpVarsFind( "attack" );
 if( varstring )
 {
  if( ( sscanf( varstring, "%d", &a ) == 1 ) && ( (unsigned int)a < 3 ) )
   maind.config_fleet = a;
  varname[0] = 'f';
  varname[2] = 0;
  for( b = 0 ; b < 4 ; b++ )
  {
   varname[1] = '0' + b;
   varstring = iohttpVarsFind( varname );
   if( ( varstring ) && ( sscanf( varstring, "%d", &a ) == 1 ) && ( (unsigned int)a < 10000 ) )
    maind.config_flee[b] = a;
  }
  dbUserMainSet( id, &maind );
 }
 iohttpVarsCut();

 iohttpBodyInit( cnt, "Fleets" );

 if( varstring )
  svSendString( cnt, "<i>Fleets orders changed</i><br><br>" );

 if( ( num = dbUserFleetList( id, &fleetd ) ) <= 0 )
 {
  svSendString( cnt, "Error while retriving user fleets list</body></html>" );
  iohttpBodyEnd( cnt );
  return;
 }

 svSendString( cnt, "<table width=\"100%\"><tr><td width=\"50%\" align=\"center\" valign=\"top\">" );
 svSendString( cnt, "<b>Main fleet</b><br><br>" );
 for( a = b = 0 ; a < CMD_UNIT_NUMUSED ; a++ )
 {
  if( fleetd[0].unit[a] )
  {
   svSendPrintf( cnt, "%d %s<br>", fleetd[0].unit[a], cmdUnitName[a] );
   b = 1;
  }
 }
 svSendString( cnt, "<a href=\"fleetdisband?id=0\">Disband units</a>" );

 svSendString( cnt, "</td><td width=\"50%\" align=\"center\" valign=\"top\">" );
 svSendString( cnt, "<b>Fleets orders</b><br>" );

 svSendString( cnt, "<form action=\"fleets\" method=\"POST\"><table cellspacing=\"8\">" );
 svSendString( cnt, "<tr><td align=\"center\">Order after an attack</td></tr><tr><td align=\"center\"><select name=\"attack\"><option value=\"0\"" );
 if( maind.config_fleet == 0 )
  svSendString( cnt, " selected" );
 svSendString( cnt, ">Station fleet on planet<option value=\"1\"" );
 if( maind.config_fleet == 1 )
  svSendString( cnt, " selected" );
 svSendString( cnt, ">Wait in system<option value=\"2\"" );
 if( maind.config_fleet == 2 )
  svSendString( cnt, " selected" );
 svSendString( cnt, ">Recall to main fleet</select></td></tr>" );

 svSendString( cnt, "<tr><td align=\"center\">Approximations of enemy forces to flee battles<tr><td align=\"center\">" );

 svSendString( cnt, "<table>" );
 svSendPrintf( cnt, "<tr><td align=\"right\" nowrap>Long-range attack</td><td><input type=\"text\" name=\"f0\" size=\"4\" value=\"%d\"> %%</td></tr>", maind.config_flee[0] );
 svSendPrintf( cnt, "<tr><td align=\"right\" nowrap>Air vs air</td><td><input type=\"text\" name=\"f1\" size=\"4\" value=\"%d\"> %%</td></tr>", maind.config_flee[1] );
 svSendPrintf( cnt, "<tr><td align=\"right\" nowrap>Ground vs air</td><td><input type=\"text\" name=\"f2\" size=\"4\" value=\"%d\"> %%</td></tr>", maind.config_flee[2] );
 svSendPrintf( cnt, "<tr><td align=\"right\" nowrap>Ground vs ground</td><td><input type=\"text\" name=\"f3\" size=\"4\" value=\"%d\"> %%</td></tr>", maind.config_flee[3] );
 svSendString( cnt, "</table>" );

 svSendString( cnt, "<tr><td align=\"center\"><input type=\"submit\" value=\"Change\"></td></tr></table></form>" );

 svSendString( cnt, "</td></tr></table>" );

 svSendString( cnt, "<br><b>Travelling fleets</b><br><br>" );
 if( num == 1 )
  svSendString( cnt, "None<br>" );
 else
 {
  svSendString( cnt, "<table width=\"90%%\" cellspacing=\"5\"><tr><td width=\"18%\">Units</td><td width=\"34%\">Order</td><td width=\"18%\">Status</td><td width=\"10%\">Position</td><td width=\"10%\">Changes</td></tr>" );
  for( a = 1 ; a < num ; a++ )
  {
   svSendString( cnt, "<tr><td>" );
   for( b = 0 ; b < CMD_UNIT_NUMUSED ; b++ )
   {
    if( fleetd[a].unit[b] )
    {
     svSendPrintf( cnt, "%d %s<br>", fleetd[a].unit[b], cmdUnitName[b] );
     szTotal[b] += fleetd[a].unit[b];     
    }
   }
   svSendString( cnt, "</td><td>" );
   if( fleetd[a].order == CMD_FLEET_ORDER_MOVE )
   {
    if( fleetd[a].sysid != -1 )
     svSendPrintf( cnt, "%s <a href=\"system?id=%d\">%d,%d</a></td>", cmdFleetOrderName[fleetd[a].order], fleetd[a].sysid, ( fleetd[a].destination >> 8 ) & 0xFFF, fleetd[a].destination >> 20 );
    else
     svSendPrintf( cnt, "%s %d,%d</td>", cmdFleetOrderName[fleetd[a].order], ( fleetd[a].destination >> 8 ) & 0xFFF, fleetd[a].destination >> 20 );
   }
   else if(fleetd[a].order == CMD_FLEET_ORDER_MERGE )
   {
   	svSendPrintf(cnt, "%s at %d,%d</td>", cmdFleetOrderName[fleetd[a].order], ( fleetd[a].destination >> 8 ) & 0xFFF, fleetd[a].destination >> 20);
   }
   else if( fleetd[a].order == CMD_FLEET_ORDER_CANCELED )
    svSendPrintf( cnt, "%s</td>", cmdFleetOrderName[fleetd[a].order] );
   else if( fleetd[a].order == CMD_FLEET_ORDER_RECALL )
    svSendPrintf( cnt, "%s at <a href=\"planet?id=%d\">%d,%d:%d</a></td>", cmdFleetOrderName[fleetd[a].order], fleetd[a].destid, ( fleetd[a].destination >> 8 ) & 0xFFF, fleetd[a].destination >> 20, fleetd[a].destination & 0xFF );
   else
   {
    if( dbMapRetrievePlanet( fleetd[a].destid, &planetd ) < 0 )
     svSendPrintf( cnt, "%s %d,%d</td>", cmdFleetOrderName[fleetd[a].order], ( fleetd[a].destination >> 8 ) & 0xFFF, fleetd[a].destination >> 20 );
    else if( ( planetd.owner != -1 ) && ( dbUserMainRetrieve( planetd.owner, &main2d ) >= 0 ) )
     svSendPrintf( cnt, "%s <a href=\"planet?id=%d\">%d,%d:%d</a> owned by <a href=\"player?id=%d\">%s</a> of <a href=\"empire?id=%d\">empire #%d</a></td>", cmdFleetOrderName[fleetd[a].order], fleetd[a].destid, ( fleetd[a].destination >> 8 ) & 0xFFF, fleetd[a].destination >> 20, fleetd[a].destination & 0xFF, planetd.owner, main2d.faction, main2d.empire, main2d.empire );
    else
     svSendPrintf( cnt, "%s <a href=\"planet?id=%d\">%d,%d:%d</a>, uninhabited</td>", cmdFleetOrderName[fleetd[a].order], fleetd[a].destid, ( fleetd[a].destination >> 8 ) & 0xFFF, fleetd[a].destination >> 20, fleetd[a].destination & 0xFF );
   }
   if( fleetd[a].time != -1 )
    svSendPrintf( cnt, "<td>Moving, %d weeks</td>", fleetd[a].time );
   else if(( fleetd[a].order == CMD_FLEET_ORDER_MOVE ) || (fleetd[a].order == CMD_FLEET_ORDER_MERGE))
    svSendPrintf( cnt, "<td>Waiting</td>" );
   else if( fleetd[a].order == CMD_FLEET_ORDER_CANCELED )
    svSendPrintf( cnt, "<td>Order canceled</td>" );
   else
    svSendPrintf( cnt, "<td><a href=\"fleetattack?id=%d\">Ready to attack</a></td>", a );
   cmdFleetGetPosition( &fleetd[a], &x, &y );
   svSendPrintf( cnt, "<td>%d,%d</td>", x, y );
			if((fleetd[a].unit[2]>0)||(fleetd[a].unit[3]>0)||(fleetd[a].unit[4]>0)||(fleetd[a].unit[8]>0))
   	svSendPrintf( cnt, "<td><a href=\"fleetchange?id=%d\">Details</a></td><td><a href=\"fleetssplit?fleetid=%d\">Split</a></a></td><td><a href=\"fleetsmerge?fltid=%d\">Merge</a></td></tr>", a, a, a);
			else
				svSendPrintf( cnt, "<td><a href=\"fleetchange?id=%d\">Details</a></td></tr>", a);
  } 
  
  svSendString( cnt, "</table>" );
 }
 
 svSendString( cnt, "<br><b>Total traveling fleets</b><br><br>" );
  for(b=0;b < CMD_UNIT_NUMUSED ; b++ )
  {
  	if( szTotal[b] )
     svSendPrintf( cnt, "%d %s<br>", szTotal[b], cmdUnitName[b] );
  }

 svSendString( cnt, "<br><br><b>Dispatch a new fleet</b><br><br><form action=\"fleetsend\" method=\"POST\">" );
 svSendPrintf( cnt, "<select name=\"order\"><option value=\"%d\">%s<option value=\"%d\">%s<option value=\"%d\">%s</select>", CMD_FLEET_ORDER_ATTACK, cmdFleetOrderName[CMD_FLEET_ORDER_ATTACK], CMD_FLEET_ORDER_STATION, cmdFleetOrderName[CMD_FLEET_ORDER_STATION], CMD_FLEET_ORDER_MOVE, cmdFleetOrderName[CMD_FLEET_ORDER_MOVE] );
 svSendString( cnt, "<table cellspacing=\"8\">" );
 for( a = 0 ; a < CMD_UNIT_FLEET ; a++ )
 {
  if( fleetd[0].unit[a] )
   svSendPrintf( cnt, "<tr><td align=\"right\">%d %s</td><td><input type=\"text\" name=\"u%d\" size=\"12\"></td></tr>", fleetd[0].unit[a], cmdUnitName[a], a );
 }
 svSendString( cnt, "</table>X:<input type=\"text\" name=\"X\" size=\"4\"> Y:<input type=\"text\" name=\"Y\" size=\"4\"> Planet:<input type=\"text\" name=\"Z\" size=\"4\"><br><br><input type=\"submit\" value=\"Send\"></form>" );

 free( fleetd );
 iohttpBodyEnd( cnt );
 return;
}

void iohttpFunc_fleetssplit( svConnectionPtr cnt )
{
	int id, i, nId, nError;
	dbUserMainDef maind;
	dbUserFleetDef fleetd, fleet2d;
	long long int disunit[CMD_UNIT_NUMUSED];
	unsigned char disname[10];
	char *varstring;
	
	iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;

	iohttpBodyInit( cnt, "Splitting fleet" );
 iohttpVarsInit( cnt );
 varstring = iohttpVarsFind( "fleetid" );
 if( varstring )
 {
 	//Getting info on the fleet to split
 	nId = atoi(varstring);
 	for(i=0;i<CMD_UNIT_NUMUSED;i++)
 	{
 		sprintf(disname, "u%d", i);
 		varstring = iohttpVarsFind(disname);
 		if(varstring)
 			disunit[i] = strtoll(varstring, NULL, 10);
 		else
 			disunit[i] = 0;
 		if(disunit[i] < 0)
 			disunit[i] = 0;
 	}
 	//We have everything now we check if we can split that
 	if(dbUserFleetRetrieve( id, nId, &fleetd ) > 0)
 	{
 		nError = 0;
 		for(i=0;i<CMD_UNIT_NUMUSED;i++)
 		{
 			if(fleetd.unit[i] < disunit[i])
 				nError = 1;
 		}
 		//Not enough transport remain
 		if ((fleetd.unit[2] - disunit[2] )*100< ((fleetd.unit[5] - disunit[5]) + (fleetd.unit[6] - disunit[6]) + ((fleetd.unit[7] - disunit[7])*4)) )
 		{
 			nError = 2;
 		}
 		else if(disunit[2]*100 < (disunit[5] + disunit[6] + disunit[7]*4))
 		{
 			nError = 3;
 		}
 		else if((fleetd.unit[4]-disunit[4])*100 < (fleetd.unit[0]-disunit[0] + fleetd.unit[1]-disunit[1] + fleetd.unit[2]-disunit[2]))
 		{
 			nError = 4;
 		}
 		else if((disunit[4]*100) < (disunit[0] + disunit[1] + disunit[2]))
 		{
 			nError = 5;
 		}
 		else if((disunit[0] == 0)&&(disunit[1] == 0)&&(disunit[2] == 0)&&(disunit[3] == 0)&&(disunit[4] == 0)&&(disunit[5] == 0)&&(disunit[6] == 0)&&(disunit[7] == 0)&&(disunit[8] == 0))
 			nError = 6;  //In fact its not an error its just split fleet but no fleet is selected
 		else if((fleetd.unit[0]-disunit[0] == 0)&&(fleetd.unit[1]-disunit[1] == 0)&&(fleetd.unit[2]-disunit[2] == 0)&&(fleetd.unit[3]-disunit[3] == 0)&&(fleetd.unit[4]-disunit[4] == 0)&&(fleetd.unit[5]-disunit[5] == 0)&&(fleetd.unit[6]-disunit[6] == 0)&&(fleetd.unit[7]-disunit[7] == 0)&&(fleetd.unit[8]-disunit[8] == 0))
 			nError = 6;  //In fact its not an error its just split fleet but no fleet is selected
 		
 		if(nError == 1)
 			svSendString( cnt, "<i>Error while splitting fleet</i><br>You can split more than what you have!" );
 		else if(nError == 2)
 			svSendString( cnt, "<i>Error while splitting fleet</i><br>You must leave enough transports in your Main Fleet<br>Each transport can hold 100 soldiers or droids, or 25 goliaths." );
 		else if(nError == 3)
 			svSendString( cnt, "<i>Error while splitting fleet</i><br>You don't have enough transports in your split fleet<br>Each transport can hold 100 soldiers or droids, or 25 goliaths." );
 		else if(nError == 4)
 			svSendString( cnt, "<i>Error while splitting fleet</i><br>You don't leave enough carriers in your Main Fleet<br>Each carrier can hold 100 fighters, bombers or transports." ); 		
 		else if(nError == 5)
 			svSendString( cnt, "<i>Error while splitting fleet</i><br>You don't have enough carriers in your split fleet<br>Each carrier can hold 100 fighters, bombers or transports." ); 		
 		else if(nError == 0) //time to split some fleet
 		{
 			//make the same fleet (same order and dest) but not the same unit amount
 			memcpy(&fleet2d, &fleetd, sizeof(dbUserFleetDef));
 			for(i=0;i<CMD_UNIT_NUMUSED;i++)
 			{
 				fleetd.unit[i] -= disunit[i];
 				fleet2d.unit[i] = disunit[i];
 			}
 			dbUserFleetAdd(id, &fleet2d);
 			dbUserFleetSet(id, nId, &fleetd );
 			svSendString(cnt, "<i>Splitting successful</i>");
 		}
 		//Menu go over here
 		//Some JavaScript to do cute things
 		svSendString(cnt, "<SCRIPT language=\"JavaScript\"><!-- // Hiding for no JavaScript\n function calcul()\n{\n");
 		for(i=0;i<CMD_UNIT_NUMUSED;i++)
 		{
 			if(fleetd.unit[i])
 				svSendPrintf(cnt, "if(isNaN(document.forms[\"fleet\"].u%d.value)){document.forms[\"fleet\"].u%d.value = 0}", i, i);
 			if(fleetd.unit[i])
 				svSendPrintf(cnt, "document.forms[\"fleet\"].unit%d.value = document.forms[\"fleet\"].uh%d.value-document.forms[\"fleet\"].u%d.value\n", i, i, i);
 		}
 		svSendString(cnt, "}");
 		
 		svSendString(cnt, "\nfunction check()\n{\nif(document.forms[\"fleet\"].cpct.checked==true){document.forms[\"fleet\"].pct.readOnly=false\n}\nelse{document.forms[\"fleet\"].pct.readOnly=true}\ndocument.forms[\"fleet\"].pct.value=\"\" \n}");
 		
 		svSendString(cnt, "\nfunction pctchange()\n{\nif(document.forms[\"fleet\"].pct.value > 100){document.forms[\"fleet\"].pct.value = 100}\n");
 		for(i=0;i<CMD_UNIT_NUMUSED;i++)
 		{
 			if(fleetd.unit[i])
 				svSendPrintf(cnt, "document.forms[\"fleet\"].u%d.value = Math.round((document.forms[\"fleet\"].uh%d.value*document.forms[\"fleet\"].pct.value)/100)\n", i, i);
 		}
 		svSendString(cnt, "calcul()\n}\n");
 		svSendString(cnt, "\n//--> // done hiding</SCRIPT>");
 		svSendString(cnt, "<form action=fleetssplit method=\"POST\" name=\"fleet\">");
 		svSendPrintf(cnt, "<input type=\"hidden\" name=\"fleetid\" value=\"%d\"><br><table>", nId);
 		svSendString(cnt, "<tr align=\"center\"><b><td></td><td>Main Fleet</td><td></td><td>Split fleet</td></></tr>");
 		for(i=0;i<CMD_UNIT_NUMUSED;i++)
 		{
	 		if(fleetd.unit[i])
	 		{
	 			svSendPrintf(cnt, "<tr><td>%s </td><td><input type=\"hidden\" name=\"uh%d\" value=\"%d\"><input type=\"text\" name=\"unit%d\" value=\"%d\" READONLY</td>", cmdUnitName[i],i, fleetd.unit[i], i, fleetd.unit[i]);
	 			svSendPrintf(cnt, "<td>%s </td><td><input type=\"text\" name=\"u%d\" onKeyUp=\"calcul();\"></td></tr>", cmdUnitName[i], i);
	 		}
	 	}
 		svSendString(cnt, "</table><br><input type=\"submit\" value=\"Split\"> Use % of fleet<input type=\"CHECKBOX\" name=\"cpct\" onClick=\"check()\"> <input type=\"text\" name=\"pct\" READONLY onKeyUp=\"pctchange()\">");
 		svSendString(cnt, "</form>");
 	}
 	else
 		svSendString( cnt, "<i>Error while splitting fleet</i><br>Fleet not found" );
 }
 iohttpBodyEnd( cnt );
}

void iohttpFunc_fleetsmerge( svConnectionPtr cnt)
{
	int i, j, nError, id ,nId, nMax, nX0, nY0, nX1, nY1, fltid;
	int nNbrFleet = 0;
	unsigned char mergename[8];
	unsigned char *var;
	dbUserMainDef maind;
	int nfltid[2];
	dbUserFleetPtr pFleet;
	
	nError = 0;
	iohttpBase(cnt, 1);

	if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
		return;
	if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit(cnt, "Merging fleets");
 nMax = dbUserFleetList(id, &pFleet);
 iohttpVarsInit( cnt );
 var = iohttpVarsFind("fltid");
 if(var)
 	fltid = atoi(var);
 else
 	fltid = -1;
 nError = 0;
 //Read all var here
 for(i=0;i<nMax;i++)
 {
 	sprintf(mergename, "f%d", i);
 	var = iohttpVarsFind( mergename );
 	if(var)
 	{
 		nId = atoi(var);
 		if((nId<1)||(nId > (nMax -1)))
 		{
 			nError = 2;
 			continue;
 		}
 		if((pFleet[nId].unit[2]>0)||(pFleet[nId].unit[3]>0)||(pFleet[nId].unit[4]>0)||(pFleet[nId].unit[8]>0))
 		{
 			nfltid[nNbrFleet] = nId;
 			nNbrFleet++;
 		}
 		else
 			nError = 1;
 	}
 	if(nNbrFleet == 2)
 		break;
 }
 iohttpVarsCut();
 if(nfltid[0] == nfltid[1])
 	nNbrFleet = 1;
 if((nNbrFleet == 1)&&(nError == 0))
 	svSendString(cnt, "<i>Error while merging</i><br>Only one fleet is selected<br>");
 if(nNbrFleet == 2)
 {
 	cmdFleetGetPosition( &pFleet[nfltid[0]], &nX0, &nY0 );
 	cmdFleetGetPosition( &pFleet[nfltid[1]], &nX1, &nY1 );
 	nX0 = (nX1 + nX0)/2;
 	nY0 = (nY1 + nY0)/2;
 	//mean it was merging with another fleet
 	for(i=0;i<nNbrFleet;i++)
 	{
	 	if(pFleet[nfltid[i]].order == CMD_FLEET_ORDER_MERGE)
	 	{
	 		//We find the other one and cancel his order to merge else he will wait way too long :D
	 		for(j=1;j<nMax;j++)
	 		{
	 			if(j == nfltid[i])
	 				continue;
	 			if((pFleet[j].order == CMD_FLEET_ORDER_MERGE)&&(pFleet[j].destid==pFleet[nfltid[i]].destid))
	 			{
	 				pFleet[j].order = CMD_FLEET_ORDER_CANCELED;
	 				pFleet[j].time = -1;
	 				dbUserFleetSet(id, j, &pFleet[j]);
	 			}
	 		}
	 	}
	 }
 	nX1 = cmdExecChangeFleet(id, nX0, nY0, 0, CMD_FLEET_ORDER_MERGE, nfltid[0]);
 	nX0 = cmdExecChangeFleet(id, nX0, nY0, 0, CMD_FLEET_ORDER_MERGE, nfltid[1]);
 	svSendString(cnt, "<i>Merge sucessful</i><br>");
 	if(nX1 > nX0)
 		nX0 = nX1;
 	svSendPrintf(cnt, "They will merge in %d weeks", nX0);
 }
 else if(nError == 2)
 	svSendString(cnt, "<i>Error while merging</i><br>Your fleet must exist before getting merge!!!");
 else if(nError == 1)
 {
 	svSendString(cnt, "<i>Error while merging</i><br>Your fleet must have at least one transport, carrier, cruiser or phantoms to be merge");
 }
 if(fltid > 0)
 {
 	svSendPrintf(cnt, "<br><table><tr>Fleet %d</tr>", fltid);
 	for(i=0;i<CMD_UNIT_NUMUSED;i++)
 	{
 		if(pFleet[fltid].unit[i])
 			svSendPrintf(cnt, "<tr><td>%s</td><td>%d</td></tr>", cmdUnitName[i], pFleet[fltid].unit[i]);
 	}
 	svSendString(cnt, "</table><br><br>");
 	
 	svSendString(cnt, "<form action=\"fleetsmerge\"><table>");
 	svSendPrintf(cnt, "<input name=\"fltid\" type=\"hidden\" value=\"%d\"><input name=\"f0\" type=\"hidden\" value=\"%d\"><tr>", fltid, fltid);
 	
 	for(i=1;i<nMax;i++)
 	{
 		if(i == fltid)		//Skip it if the same
 			continue;
 		if((pFleet[i].unit[2]==0)&&(pFleet[i].unit[3]==0)&&(pFleet[i].unit[4]==0)&&(pFleet[i].unit[8]==0))
 			continue;
 		svSendPrintf(cnt, "<td><input type=\"radio\" name=\"f1\" value=\"%d\" CHECKED >Fleet %d</input><table>", i, i);
 		for(j=0;j<CMD_UNIT_NUMUSED;j++)
 		{
 			if(pFleet[i].unit[j] != 0)
 				svSendPrintf(cnt, "<tr><td>%s</td><td>%d</td></tr>", cmdUnitName[j], pFleet[i].unit[j]);
 		}
 		
 		svSendString(cnt, "</table></td>");
 	}
 	svSendPrintf(cnt, "</tr><tr>");
 	svSendString(cnt, "</table>");
 	svSendPrintf(cnt, "<input type=\"submit\" value=\"Merge\"");
 	svSendString(cnt, "</form>");
 }
 else
 	svSendString(cnt, "<i>Error while merging</i><br>You haven't selected any fleet!");
 
 svSendString(cnt, "<br><a href=\"fleets\">Return to fleets</a>");
 svSendString(cnt, "currently offline hit the back button now!");
 free(pFleet);
 iohttpBodyEnd( cnt );
}

void iohttpFunc_fleetdisband( svConnectionPtr cnt )
{
 int a, b, c, id, cmd[6], fltid;
 dbUserFleetDef fleetd;
 dbUserMainDef maind;
 long long int disunit[CMD_UNIT_NUMUSED];
 unsigned char disname[8];
 unsigned char *sptr, *fleetstring;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Disband units" );

 iohttpVarsInit( cnt );
 memset( disunit, 0, CMD_UNIT_NUMUSED*sizeof(long long int) );
 for( a = 0 ; a < CMD_UNIT_NUMUSED ; a++ )
 {
  sprintf( disname, "dis%d", a );
  sptr = iohttpVarsFind( disname );
  if( !( sptr ) || ( sscanf( sptr, "%lld", &disunit[a] ) != 1 ) )
   disunit[a] = 0;
 }
 fleetstring = iohttpVarsFind( "id" );
 iohttpVarsCut();

 cmd[0] = CMD_RETRIEVE_FLEET;
 cmd[1] = id;
 if( !( fleetstring ) || ( sscanf( fleetstring, "%d", &cmd[2] ) <= 0 ) || ( cmdExecute( cnt, cmd, &fleetd, 0 ) < 0 ) )
 {
  svSendString( cnt, "This fleet doesn't exist!</body></html>" );
  return;
 }
 fltid = cmd[2];

 cmd[0] = CMD_DISBAND_UNITS;
 cmd[1] = id;
 cmd[4] = fltid;
 for( a = c = 0 ; a < CMD_UNIT_NUMUSED ; a++ )
 {
  if( disunit[a] <= 0 )
   continue;
  cmd[2] = a;
  cmd[3] = disunit[a];
  b = cmdExecute( cnt, cmd, 0, 0 );
  if( b >= 0 )
  {
   svSendPrintf( cnt, "<i>You disbanded %d %s</i><br><br>", cmd[3], cmdUnitName[a] );
   c++;
  }
  else if( cmdErrorString )
   svSendPrintf( cnt, "<i>%s</i><br><br>", cmdErrorString );
  else
   svSendString( cnt, "<i>Error while disbanding units</i><br>" );
 }
 if( c )
 {
  cmd[0] = CMD_RETRIEVE_FLEET;
  cmd[1] = id;
  cmd[2] = fltid;
  cmdExecute( cnt, cmd, &fleetd, 0 );
  cmd[0] = CMD_RETRIEVE_USERMAIN;
  cmd[1] = id;
  cmdExecute( cnt, cmd, &maind, 0 );
 }

 svSendPrintf( cnt, "<form action=\"fleetdisband\" method=\"POST\"><input type=\"hidden\" value=\"%d\" name=\"id\">", fltid );
 svSendString( cnt, "<table width=\"320\" border=\"2\">" );
 for( a = 0 ; a < CMD_UNIT_NUMUSED ; a++ )
 {
  if( !( fleetd.unit[a] ) )
   continue;
  svSendPrintf( cnt, "<tr><td align=\"center\"><b>%s</b><br><br>", cmdUnitName[a] );
  svSendPrintf( cnt, "<input type=\"text\" name=\"dis%d\" size=\"10\"><br><br>There are %d in this fleet<br>( %lld total )</td></tr>", a, fleetd.unit[a], maind.totalunit[a] );
 }
 svSendString( cnt, "</table><br><input type=\"submit\" value=\"Disband\"></form>" );

 iohttpBodyEnd( cnt );
 return;
}


void iohttpFunc_fleetsend( svConnectionPtr cnt )
{
 int a, id, order, x, y, z;
 dbUserMainDef maind;
 int sendunit[CMD_UNIT_NUMUSED];
 unsigned char sendname[8];
 unsigned char *orderstring, *xstring, *ystring, *zstring, *sptr;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Sending fleet" );

 iohttpVarsInit( cnt );
 memset( sendunit, 0, CMD_UNIT_NUMUSED*sizeof(int) );
 for( a = 0 ; a < CMD_UNIT_FLEET ; a++ )
 {
  sprintf( sendname, "u%d", a );
  sptr = iohttpVarsFind( sendname );
  if( !( sptr ) || ( sscanf( sptr, "%d", &sendunit[a] ) != 1 ) )
   sendunit[a] = 0;
 }
 orderstring = iohttpVarsFind( "order" );
 xstring = iohttpVarsFind( "X" );
 ystring = iohttpVarsFind( "Y" );
 zstring = iohttpVarsFind( "Z" );
 iohttpVarsCut();

 if( !( orderstring ) || ( sscanf( orderstring, "%d", &order ) != 1 ) )
  goto iohttpFunc_fleetsendL0;
 if( !( xstring ) || ( sscanf( xstring, "%d", &x ) != 1 ) )
  goto iohttpFunc_fleetsendL0;
 if( !( ystring ) || ( sscanf( ystring, "%d", &y ) != 1 ) )
 {
  iohttpFunc_fleetsendL0:
  svSendString( cnt, "<i>Invalid order or coordinates</i><br>" );
  iohttpBodyEnd( cnt );
  return;
 }
 z = 0;
 if( zstring )
  sscanf( zstring, "%d", &z );

 if( ( a = cmdExecSendFleet( id, x, y, z, order, sendunit ) ) < 0 )
 {
  if( cmdErrorString )
   svSendPrintf( cnt, "<i>%s</i><br>", cmdErrorString );
  else
   svSendString( cnt, "<i>Error encountered when sending fleet</i><br>" );
  iohttpBodyEnd( cnt );
  return;
 }

 svSendString( cnt, "<i>Fleet sent!</i><br><br>" );
 if( a )
  svSendPrintf( cnt, "The fleet will reach its destination in %d weeks<br>", a );
 else
  svSendString( cnt, "The fleet reached its destination<br>" );

 iohttpBodyEnd( cnt );
 return;
}


void iohttpFunc_fleetchange( svConnectionPtr cnt )
{
 int a, id, i, fltid, order, x, y, z;
 dbUserFleetDef fleetd;
 dbUserFleetPtr fleetp;
 dbUserMainDef maind;
 unsigned char *fleetstring, *orderstring, *xstring, *ystring, *zstring;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Viewing fleet" );

 iohttpVarsInit( cnt );
 fleetstring = iohttpVarsFind( "id" );
 orderstring = iohttpVarsFind( "order" );
 xstring = iohttpVarsFind( "X" );
 ystring = iohttpVarsFind( "Y" );
 zstring = iohttpVarsFind( "Z" );
 iohttpVarsCut();

 if( !( fleetstring ) || ( sscanf( fleetstring, "%d", &fltid ) <= 0 ) || ( dbUserFleetRetrieve( id, fltid, &fleetd ) < 0 ) )
 {
  svSendString( cnt, "This fleet doesn't exist!</body></html>" );
  iohttpBodyEnd( cnt );
  return;
 }

 order = -1;
 if( ( orderstring ) && ( sscanf( orderstring, "%d", &order ) == 1 ) )
 {
  x = y = z = 0;
  if( xstring )
   sscanf( xstring, "%d", &x );
  if( ystring )
   sscanf( ystring, "%d", &y );
  if( zstring )
   sscanf( zstring, "%d", &z );
  if (dbUserFleetRetrieve(id, fltid, &fleetd) > 0)
  {
  	if(fleetd.order == CMD_FLEET_ORDER_MERGE)
  	{
  		//if you cancel this one you also need to cancel the one that will merge with it
  		a = dbUserFleetList(id, &fleetp);
  		for(i=1;i<a;i++)
  		{
  			if((fleetp[i].order==CMD_FLEET_ORDER_MERGE)&&(fleetp[i].destid == fleetd.destid)&&(i != fltid))
  			{
  				fleetp[i].time = -1;
  				fleetp[i].order = CMD_FLEET_ORDER_CANCELED;
  				dbUserFleetSet(id, i, &fleetp[i]);
  			}
  		}
  		free(fleetp);
  	}
  }
  if( cmdExecChangeFleet( id, x, y, z, order, fltid ) < 0 )
  {
   if( cmdErrorString )
    svSendPrintf( cnt, "<i>%s</i>", cmdErrorString );
   else
    svSendString( cnt, "<i>Error encountered when changing fleet orders</i>" );
   iohttpBodyEnd( cnt );
   return;
  }
  else
  {
   svSendString( cnt, "<i>Order changed</i><br><br>" );
   dbUserFleetRetrieve( id, fltid, &fleetd );
  }
 }

 svSendString( cnt, "<b>Fleet units</b><br><br>" );
 for( a = 0 ; a < CMD_UNIT_NUMUSED ; a++ )
 {
  if( fleetd.unit[a] )
   svSendPrintf( cnt, "%d %s<br>", fleetd.unit[a], cmdUnitName[a] );
 }

 cmdFleetGetPosition( &fleetd, &x, &y );
 svSendPrintf( cnt, "<br><b>Current position</b> : %d,%d", x, y );

 svSendString( cnt, "<br><b>Order</b> : " );
 if( fleetd.order == CMD_FLEET_ORDER_MOVE )
 {
  if( fleetd.sysid != -1 )
   svSendPrintf( cnt, "%s <a href=\"system?id=%d\">%d,%d</a>", cmdFleetOrderName[fleetd.order], fleetd.sysid, ( fleetd.destination >> 8 ) & 0xFFF, fleetd.destination >> 20 );
  else
   svSendPrintf( cnt, "%s %d,%d", cmdFleetOrderName[fleetd.order], ( fleetd.destination >> 8 ) & 0xFFF, fleetd.destination >> 20 );
 }
 else if( fleetd.order == CMD_FLEET_ORDER_CANCELED )
  svSendPrintf( cnt, "%s", cmdFleetOrderName[fleetd.order] );
 else
  svSendPrintf( cnt, "%s <a href=\"planet?id=%d\">%d,%d:%d</a>", cmdFleetOrderName[fleetd.order], fleetd.destid, ( fleetd.destination >> 8 ) & 0xFFF, fleetd.destination >> 20, fleetd.destination & 0xFF );

 svSendString( cnt, "<br><b>Status</b> : " );
 if( fleetd.time != -1 )
  svSendPrintf( cnt, "Moving, %d weeks", fleetd.time );
 else if( fleetd.order == CMD_FLEET_ORDER_MOVE )
  svSendPrintf( cnt, "Waiting" );
 else if( fleetd.order == CMD_FLEET_ORDER_CANCELED )
  svSendPrintf( cnt, "Order canceled" );
 else
  svSendPrintf( cnt, "Ready to attack" );

 svSendPrintf( cnt, "<br><br><b>Change order</b><br><form action=\"fleetchange\" method=\"POST\"><input type=\"hidden\" name=\"id\" value=\"%d\">", fltid );
 if( fleetd.unit[CMD_UNIT_EXPLORATION] )
  svSendPrintf( cnt, "<select name=\"order\"><option value=\"%d\">%s<option value=\"%d\">%s<option value=\"%d\">%s</select>", CMD_FLEET_ORDER_EXPLORE, cmdFleetOrderName[CMD_FLEET_ORDER_EXPLORE], CMD_FLEET_ORDER_MOVE, cmdFleetOrderName[CMD_FLEET_ORDER_MOVE], CMD_FLEET_ORDER_RECALL, cmdFleetOrderName[CMD_FLEET_ORDER_RECALL] );
 else if( fleetd.unit[CMD_UNIT_AGENT] || fleetd.unit[CMD_UNIT_GHOST] )
  svSendPrintf( cnt, "<select name=\"order\"><option value=\"%d\">%s</select>", CMD_FLEET_ORDER_RECALL, cmdFleetOrderName[CMD_FLEET_ORDER_RECALL] );
 else
  svSendPrintf( cnt, "<select name=\"order\"><option value=\"%d\">%s<option value=\"%d\">%s<option value=\"%d\">%s<option value=\"%d\">%s</select>", CMD_FLEET_ORDER_ATTACK, cmdFleetOrderName[CMD_FLEET_ORDER_ATTACK], CMD_FLEET_ORDER_STATION, cmdFleetOrderName[CMD_FLEET_ORDER_STATION], CMD_FLEET_ORDER_MOVE, cmdFleetOrderName[CMD_FLEET_ORDER_MOVE], CMD_FLEET_ORDER_RECALL, cmdFleetOrderName[CMD_FLEET_ORDER_RECALL] );
 svSendPrintf( cnt, "<br><br>X:<input type=\"text\" name=\"X\" size=\"4\" value=\"%d\"> Y:<input type=\"text\" name=\"Y\" size=\"4\" value=\"%d\"> Planet:<input type=\"text\" name=\"Z\" size=\"4\" value=\"%d\"><br><br><input type=\"submit\" value=\"Change order\"></form>", ( fleetd.destination >> 8 ) & 0xFFF, fleetd.destination >> 20, fleetd.destination & 0xFF );
	if((fleetd.unit[2]>0)||(fleetd.unit[3]>0)||(fleetd.unit[4]>0)||(fleetd.unit[8]>0))
		svSendPrintf(cnt, "<br><a href=\"fleetssplit?fleetid=%d\">Split</a><br><a href=\"fleetsmerge?fltid=%d\">Merge</a>", fltid, fltid);
 
 iohttpBodyEnd( cnt );
 return;
}


void iohttpAttackReport( svConnectionPtr cnt, int *results, int sats )
{
 int a;
 for( a = 0 ; a < CMD_UNIT_FLEET ; a++ )
 {
  if( results[4+0*CMD_UNIT_FLEET+a] )
   goto iohttpAttackReportL0;
 }
 for( a = 0 ; a < CMD_UNIT_FLEET ; a++ )
 {
  if( results[4+1*CMD_UNIT_FLEET+a] )
   goto iohttpAttackReportL0;
 }
 svSendString( cnt, "<br>" );
 return;
 iohttpAttackReportL0:
 svSendString( cnt, "<table width=\"60%\"><tr><td width=\"30%\" align=\"center\" valign=\"top\"><i>Attacker losses</i><br>" );
 for( a = 0 ; a < CMD_UNIT_FLEET ; a++ )
 {
  if( results[4+0*CMD_UNIT_FLEET+a] )
   svSendPrintf( cnt, "%d %s<br>", results[4+0*CMD_UNIT_FLEET+a], cmdUnitName[a] );
 }
 svSendString( cnt, "</td><td width=\"30%\" align=\"center\" valign=\"top\"><i>Defender losses</i><br>" );
 for( a = 0 ; a < CMD_UNIT_FLEET ; a++ )
 {
  if( results[4+1*CMD_UNIT_FLEET+a] )
   svSendPrintf( cnt, "%d %s<br>", results[4+1*CMD_UNIT_FLEET+a], cmdUnitName[a] );
 }
 if( sats )
  svSendPrintf( cnt, "%d %s<br>", sats, cmdBuildingName[CMD_BUILDING_SATS] );
 svSendString( cnt, "</tr></table>" );
 return;
}


void iohttpFunc_fleetattack( svConnectionPtr cnt )
{
 int id, cmd[3], fltid;
 int results[4+8*CMD_UNIT_FLEET+2];
 dbUserMainDef maind, main2d;
 unsigned char *fleetstring;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Battle report" );

 iohttpVarsInit( cnt );
 fleetstring = iohttpVarsFind( "id" );
 iohttpVarsCut();

 cmd[0] = CMD_ATTACK_PLANET;
 cmd[1] = id;
 if( !( fleetstring ) || ( sscanf( fleetstring, "%d", &cmd[2] ) <= 0 ) || ( cmdExecute( cnt, cmd, results, 0 ) < 0 ) )
 {
  if( cmdErrorString )
   svSendPrintf( cnt, "<i>%s</i>", cmdErrorString );
  else
   svSendString( cnt, "<i>Error encountered when attacking</i>" );
  iohttpBodyEnd( cnt );
  return;
 }
 fltid = cmd[2];

 if( cmdErrorString )
  svSendPrintf( cnt, "<i>%s</i><br><br>", cmdErrorString );

 results[2] += ( rand() % 17 ) - 8;
 if( results[2] < 0 )
  results[2] = 0;
 if( results[2] > 100 )
  results[2] = 100;
 dbUserMainRetrieve( results[0], &main2d );
 svSendPrintf( cnt, "Attacking <a href=\"player?id=%d\">%s</a> of <a href=\"empire?id=%d\">empire #%d</a><br>Estimated defender portal coverage : %d%%<br>", results[0], main2d.faction, results[1], results[1], results[2] );

 svSendPrintf( cnt, "<br><b>Phase 1 - Defending cruisers engage approaching cruisers and carriers, carrying all other vessels, at long range.</b><br>" );
 if( ( results[3] & 0x10 ) )
  svSendString( cnt, "Overwhelmed by defending enemy forces, your fleet hastidly retreated to minimize losses<br>" );
 else if( results[3] & 0x100 )
  svSendString( cnt, "Your fleet didn't encounter much resistance, it seems enemy forces preferred not to directly engage us<br>" );
 iohttpAttackReport( cnt, &results[0*CMD_UNIT_FLEET], results[4+8*CMD_UNIT_FLEET+0] );
 if( ( results[3] & 0x10 ) )
  goto iohttpFunc_fleetattackL0;

 svSendPrintf( cnt, "<br><b>Phase 2 - Defending fighters and cruisers intercepts enemy vessels, attempting to destroy transports before they land.</b><br>" );
 if( ( results[3] & 0x20 ) )
  svSendString( cnt, "Overwhelmed by defending enemy forces, your fleet hastidly retreated to minimize losses<br>" );
 else if( results[3] & 0x200 )
  svSendString( cnt, "Your fleet didn't encounter much resistance, it seems enemy forces preferred not to directly engage us<br>" );
 iohttpAttackReport( cnt, &results[2*CMD_UNIT_FLEET], results[4+8*CMD_UNIT_FLEET+1] );
 if( ( results[3] & 0x20 ) )
  goto iohttpFunc_fleetattackL0;

 svSendPrintf( cnt, "<br><b>Phase 3 - Ground units guarding the planet with anti-air capabilities fire at the landing transports, protected by bombers and cruisers.</b><br>" );
 if( ( results[3] & 0x40 ) )
  svSendString( cnt, "Overwhelmed by defending enemy forces, your fleet hastidly retreated to minimize losses<br>" );
 else if( results[3] & 0x400 )
  svSendString( cnt, "Your fleet didn't encounter much resistance, it seems enemy forces preferred not to directly engage us<br>" );
 iohttpAttackReport( cnt, &results[4*CMD_UNIT_FLEET], 0 );
 if( ( results[3] & 0x40 ) )
  goto iohttpFunc_fleetattackL0;

 svSendPrintf( cnt, "<br><b>Phase 4 - Attacking ground forces, with support from bombers and cruisers, attempting to take control of the planet.</b><br>" );
 if( ( results[3] & 0x80 ) )
  svSendString( cnt, "Overwhelmed by defending enemy forces, your fleet hastidly retreated to minimize losses<br>" );
 else if( results[3] & 0x800 )
  svSendString( cnt, "Your fleet didn't encounter much resistance, it seems enemy forces preferred not to directly engage us<br>" );
 iohttpAttackReport( cnt, &results[6*CMD_UNIT_FLEET], 0 );
 if( ( results[3] & 0x80 ) )
  goto iohttpFunc_fleetattackL0;


 iohttpFunc_fleetattackL0:

 if( ( results[3] & 1 ) )
  svSendPrintf( cnt, "<br><b>You won, the planet is yours!</b><br>" );
 else
  svSendPrintf( cnt, "<br><b>Defender wins!</b><br>" );

 iohttpBodyEnd( cnt );
 return;
}





void iohttpFunc_explore( svConnectionPtr cnt )
{
 int id, plnid, explore;
 dbMainPlanetDef planetd;
 dbUserMainDef maind;
 unsigned char *planetstring;
 unsigned char *explorestring;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Exploration" );

 iohttpVarsInit( cnt );
 planetstring = iohttpVarsFind( "id" );
 explorestring = iohttpVarsFind( "explore" );
 iohttpVarsCut();

 if( !( planetstring ) || ( sscanf( planetstring, "%d", &plnid ) <= 0 ) || ( dbMapRetrievePlanet( plnid, &planetd ) < 0 ) )
 {
  svSendString( cnt, "This planet doesn't seem to exist!</body></html>" );
  iohttpBodyEnd( cnt );
  return;
 }
 if( planetd.flags & CMD_PLANET_FLAGS_HOME )
 {
  svSendString( cnt, "You can't explore a home planet!</body></html>" );
  iohttpBodyEnd( cnt );
  return;
 }

 if( explorestring  )
 {
  if( ( cmdExecExplore( id, plnid, &explore ) ) < 0 )
  {
   if( cmdErrorString )
    svSendPrintf( cnt, "%s", cmdErrorString );
   else
    svSendString( cnt, "Error encountered while retrieving exploration information</body></html>" );
   goto iohttpFunc_exploreL0;
  }
  svSendPrintf( cnt, "We have sent our exploration ship! If everything goes well, this planet will be ours in %d weeks<br><br>", explore );
 }
 else
 {
  if( ( cmdExecExploreInfo( id, plnid, &explore ) ) < 0 )
  {
   if( cmdErrorString )
    svSendPrintf( cnt, "%s", cmdErrorString );
   else
    svSendString( cnt, "Error encountered while retrieving exploration information</body></html>" );
   goto iohttpFunc_exploreL0;
  }
		
  svSendPrintf( cnt, "It would take %d weeks for an exploration ship to reach this planet.<br><br>", explore );
  svSendPrintf( cnt, "<b><a href=\"explore?id=%d&explore=1\">Explore this planet</a></b><br>", plnid );
 }
 iohttpFunc_exploreL0:
 svSendPrintf( cnt, "<br><br><a href=\"planet?id=%d\">View planet</a>", plnid );
 svSendPrintf( cnt, "<br><br><a href=\"system?id=%d\">View system</a>", planetd.system );

 iohttpBodyEnd( cnt );
 return;
}


void iohttpFunc_attack( svConnectionPtr cnt )
{
 int a, id, plnid, fr;
 float fa;
 dbMainPlanetDef planetd;
 dbUserMainDef maind;
 dbUserFleetDef fleetd;
 unsigned char *planetstring;
 dbUserPtr user;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;

 iohttpVarsInit( cnt );
 planetstring = iohttpVarsFind( "id" );
 iohttpVarsCut();

 if( dbUserFleetRetrieve( id, 0, &fleetd ) < 0 )
 {
  svSendString( cnt, "Error encountered while getting main fleet stats</body></html>" );
  iohttpBodyEnd( cnt );
  return;
 }
 if( !( planetstring ) || ( sscanf( planetstring, "%d", &plnid ) != 1 ) || ( dbMapRetrievePlanet( plnid, &planetd ) < 0 ) )
 {
  svSendString( cnt, "This planet doesn't seem to exist!</body></html>" );
  iohttpBodyEnd( cnt );
  return;
 }
 iohttpBodyInit( cnt, "Attack the planet %d,%d:%d", ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, planetd.position & 0xFF );

 if( !( user = dbUserLinkID( planetd.owner ) ) )
 {
  svSendString( cnt, "You can't attack an uninhabited planet!</body></html>" );
  iohttpBodyEnd( cnt );
  return;
 }
 else if( ( planetd.flags & CMD_PLANET_FLAGS_HOME ) && !( user->flags & CMD_USER_FLAGS_INDEPENDENT ) )
 {
  svSendString( cnt, "You can't attack a home planet!</body></html>" );
  iohttpBodyEnd( cnt );
  return;
 }

 if( ( a = cmdExecSendFleetInfos( id, plnid, &fr ) ) < 0 )
 {
  if( cmdErrorString )
   svSendPrintf( cnt, "<i>%s</i><br>", cmdErrorString );
  else
   svSendString( cnt, "<i>Error while retrieving send fleet information</i><br>" );
 }
 else
 {
  if( !( a ) )
   svSendString( cnt, "A fleet could reach this planet instantaneously." );
  else
   svSendPrintf( cnt, "A fleet would require %d weeks to reach this planet.", a );
  fa = specopDarkWebCalc( planetd.owner );
  
  if(cmdRace[maind.raceid].special & CMD_RACE_SPECIAL_IDW)
  	fa = 1;
  
  svSendPrintf( cnt, " The loss of fleet readiness for an attack is estimated to %d%%.<br>", ( (int)( fa * (float)fr ) ) >> 16 );
  if( fa > 1.05 )
   svSendPrintf( cnt, "<br><i>The area seems filled with a thick <b>Dark Mist</b>, making it more difficult to attack.<br></i>" );
 }

 svSendString( cnt, "<script>function sendmax() {" );
 for( a = 0 ; a < CMD_UNIT_FLEET ; a++ )
 {
  if( fleetd.unit[a] )
   svSendPrintf( cnt, "document.attack.u%d.value=%d; ", a, fleetd.unit[a] );
 }
 svSendString( cnt, "}</script>" );
 svSendString( cnt, "<form name=\"attack\" action=\"fleetsend\" method=\"POST\">" );
 svSendPrintf( cnt, "<input type=\"hidden\" name=\"order\" value=\"%d\">", CMD_FLEET_ORDER_ATTACK );
 svSendString( cnt, "<table cellspacing=\"8\">" );
 for( a = 0 ; a < CMD_UNIT_FLEET ; a++ )
 {
  if( fleetd.unit[a] )
   svSendPrintf( cnt, "<tr><td align=\"right\">%d %s</td><td><input type=\"text\" name=\"u%d\" size=\"12\"></td></tr>", fleetd.unit[a], cmdUnitName[a], a );
 }
 svSendPrintf( cnt, "</table><input type=\"hidden\" name=\"X\" value=\"%d\"><input type=\"hidden\" name=\"Y\" value=\"%d\"><input type=\"hidden\" name=\"Z\" value=\"%d\"><input type=\"button\" value=\"Send maximum\" onclick=\"sendmax()\"> <input type=\"submit\" value=\"Send\"></form>", ( planetd.position >> 8 ) & 0xFFF, ( planetd.position >> 20 ), planetd.position & 0xFF );

 svSendPrintf( cnt, "<br><br><a href=\"planet?id=%d\">View planet</a>", plnid );
 svSendPrintf( cnt, "<br><br><a href=\"system?id=%d\">View system</a>", planetd.system );

 iohttpBodyEnd( cnt );
 return;
}

void iohttpFunc_station( svConnectionPtr cnt )
{
 int a, id, cmd[3], plnid;
 dbMainPlanetDef planetd;
 dbUserMainDef maind;
 dbUserFleetDef fleetd;
 unsigned char *planetstring;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;

 iohttpVarsInit( cnt );
 planetstring = iohttpVarsFind( "id" );
 iohttpVarsCut();

 cmd[0] = CMD_RETRIEVE_FLEET;
 cmd[1] = id;
 cmd[2] = 0;
 if( cmdExecute( cnt, cmd, &fleetd, 0 ) < 0 )
 {
  svSendString( cnt, "Error encountered while getting main fleet stats</body></html>" );
  return;
 }
 cmd[0] = CMD_RETRIEVE_PLANET;
 if( !( planetstring ) || ( sscanf( planetstring, "%d", &cmd[1] ) <= 0 ) || ( cmdExecute( cnt, cmd, &planetd, 0 ) < 0 ) )
 {
  svSendString( cnt, "This planet doesn't seem to exist!</body></html>" );
  return;
 }
 plnid = cmd[1];

 iohttpBodyInit( cnt, "Station forces on the planet %d,%d:%d", ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, planetd.position & 0xFF );

 svSendString( cnt, "<form action=\"fleetsend\" method=\"POST\">" );
 svSendPrintf( cnt, "<input type=\"hidden\" name=\"order\" value=\"%d\">", CMD_FLEET_ORDER_STATION );
 svSendString( cnt, "<table cellspacing=\"8\">" );
 for( a = 0 ; a < CMD_UNIT_FLEET ; a++ )
 {
  if( fleetd.unit[a] )
   svSendPrintf( cnt, "<tr><td align=\"right\">%d %s</td><td><input type=\"text\" name=\"u%d\" size=\"12\"></td></tr>", fleetd.unit[a], cmdUnitName[a], a );
 }
 svSendPrintf( cnt, "</table><input type=\"hidden\" name=\"X\" value=\"%d\"><input type=\"hidden\" name=\"Y\" value=\"%d\"><input type=\"hidden\" name=\"Z\" value=\"%d\"><input type=\"submit\" value=\"Send\"></form>", ( planetd.position >> 8 ) & 0xFFF, ( planetd.position >> 20 ), planetd.position & 0xFF );
 svSendPrintf( cnt, "<br><br><a href=\"planet?id=%d\">View planet</a>", plnid );
 svSendPrintf( cnt, "<br><br><a href=\"system?id=%d\">View system</a>", planetd.system );

 iohttpBodyEnd( cnt );
 return;
}





void iohttpFunc_spec( svConnectionPtr cnt )
{
 int a, b, c, id, plnid, optype, opid;
 dbMainPlanetDef planetd;
 dbUserMainDef maind;
 dbUserFleetDef fleetd;
 unsigned char *planetstring;
 dbUserPtr user;
 dbUserSpecOpPtr specopd;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;

 iohttpVarsInit( cnt );
 planetstring = iohttpVarsFind( "id" );
 iohttpVarsCut();

 if( dbUserFleetRetrieve( id, 0, &fleetd ) < 0 )
 {
  svSendString( cnt, "Error encountered while getting main fleet agent stats</body></html>" );
  iohttpBodyEnd( cnt );
  return;
 }

 user = 0;
 plnid = -1;
 if ( ( planetstring ) && ( sscanf( planetstring, "%d", &plnid ) == 1 ) )
 {
  if( dbMapRetrievePlanet( plnid, &planetd ) < 0 )
  {
   svSendString( cnt, "This planet doesn't seem to exist!</body></html>" );
   return;
  }
  iohttpBodyInit( cnt, "Special Operations on planet %d,%d:%d", ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, planetd.position & 0xFF );
  user = dbUserLinkID( planetd.owner );
 }
 else
  iohttpBodyInit( cnt, "Special Operations" );


 svSendString( cnt, "<form action=\"operation\" method=\"POST\"><table border=\"0\" width=\"70%\" cellspacing=\"4\" cellpadding=\"4\"><tr><td width=\"35%\" valign=\"top\" nowrap>" );
 svSendPrintf( cnt, "<b>Operations</b><br>" );
 for( a = 0 ; a < CMD_AGENTOP_NUMUSED ; a++ )
 {
 	//ARTI CODE give agent op all races
 	if(!((maind.artefacts & ARTEFACT_1_BIT)&&(a == CMD_OPER_OBSERVEPLANET )))
 	{
	  if( !( specopAgentsAllowed( a, maind.raceid ) ) )
	  	continue;
	}
  b = cmdGetOpPenalty( maind.totalresearch[CMD_RESEARCH_OPERATIONS], cmdAgentopTech[a] );
  if( b == -1 )
   svSendPrintf( cnt, "<input type=\"radio\" name=\"op\" value=\"%d\" disabled> %s<br>&nbsp;&nbsp;&nbsp;<font color=\"#FF0000\">Unavailable</font><br>", a, cmdAgentopName[a] );
  else
  {
   svSendPrintf( cnt, "<input type=\"radio\" name=\"op\" value=\"%d\"> %s<br>", a, cmdAgentopName[a] );
   if( b )
    svSendPrintf( cnt, "&nbsp;&nbsp;&nbsp;<font color=\"#FF0000\">Low research penalty : %d%%</font><br>", b );
  }
 }
 svSendString( cnt, "</td><td width=\"35%\" align=\"center\" valign=\"top\" nowrap>" );
 svSendPrintf( cnt, "%d %s<br>", fleetd.unit[CMD_UNIT_AGENT], cmdUnitName[CMD_UNIT_AGENT] );
 svSendPrintf( cnt, "%d%% Agents readiness<br>", maind.readiness[2] >> 16 );
 svSendPrintf( cnt, "Send: <input type=\"text\" size=\"10\" name=\"sendagents\" value=\"%d\"><br><br>", fleetd.unit[CMD_UNIT_AGENT] );
 if( plnid >= 0 )
  svSendPrintf( cnt, "Target planet<br> X:<input type=\"text\" size=\"4\" name=\"planetx\" value=\"%d\"> Y:<input type=\"text\" size=\"4\" name=\"planety\" value=\"%d\"> Planet:<input type=\"text\" size=\"4\" name=\"planetnum\" value=\"%d\">", ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, planetd.position & 0xFF );
 else
  svSendPrintf( cnt, "Target planet<br> X:<input type=\"text\" size=\"4\" name=\"planetx\"> Y:<input type=\"text\" size=\"4\" name=\"planety\"> Planet:<input type=\"text\" size=\"4\" name=\"planetnum\">" );
 svSendString( cnt, "<br><br><input type=\"submit\" value=\"Prepare operation\">" );
 svSendString( cnt, "</td></tr></table></form>" );


 svSendString( cnt, "<form action=\"spell\" method=\"POST\"><table border=\"0\" width=\"70%\" cellspacing=\"4\" cellpadding=\"4\"><tr><td width=\"35%\" valign=\"top\" nowrap>" );
 svSendPrintf( cnt, "<b>Spells</b><br>" );
 for( a = 0 ; a < CMD_PSYCHICOP_NUMUSED ; a++ )
 {
  if( !( specopPsychicsAllowed( a, maind.raceid ) ) )
   continue;
  b = cmdGetOpPenalty( maind.totalresearch[CMD_RESEARCH_WELFARE], cmdPsychicopTech[a] );
  if( b == -1 )
   svSendPrintf( cnt, "<input type=\"radio\" name=\"op\" value=\"%d\" disabled> %s<br>&nbsp;&nbsp;&nbsp;<font color=\"#FF0000\">Unavailable</font><br>", a, cmdPsychicopName[a] );
  else
  {
   svSendPrintf( cnt, "<input type=\"radio\" name=\"op\" value=\"%d\"> %s<br>", a, cmdPsychicopName[a] );
   if( b )
    svSendPrintf( cnt, "&nbsp;&nbsp;&nbsp;<font color=\"#FF0000\">Low research penalty : %d%%</font><br>", b );
  }
 }
 svSendString( cnt, "</td><td width=\"35%\" align=\"center\" valign=\"top\" nowrap>" );
 svSendPrintf( cnt, "%d %s<br>", fleetd.unit[CMD_UNIT_WIZARD], cmdUnitName[CMD_UNIT_WIZARD] );
 svSendPrintf( cnt, "%d%% Psychics readiness<br>", maind.readiness[1] >> 16 );
 svSendPrintf( cnt, "Send: <input type=\"text\" size=\"10\" name=\"sendpsychics\" value=\"%d\"><br><br>", fleetd.unit[CMD_UNIT_WIZARD] );
 svSendPrintf( cnt, "Target faction<br><input type=\"text\" size=\"20\" name=\"target\" value=\"%s\">", ( user != 0 ) ? ( (char *)user->faction ) : ( "Faction name or ID" ) );
 svSendString( cnt, "<br><br><input type=\"submit\" value=\"Prepare spell\">" );
 svSendString( cnt, "</td></tr></table></form>" );


 svSendString( cnt, "<form action=\"incant\" method=\"POST\"><table border=\"0\" width=\"70%\" cellspacing=\"4\" cellpadding=\"4\"><tr><td width=\"35%\" valign=\"top\" nowrap>" );
 svSendPrintf( cnt, "<b>Incantations</b><br>" );
 for( a = 0 ; a < CMD_GHOSTOP_NUMUSED ; a++ )
 {
  if( !( specopGhostsAllowed( a, maind.raceid ) ) )
   continue;
  b = cmdGetOpPenalty( maind.totalresearch[CMD_RESEARCH_WELFARE], cmdGhostopTech[a] );
  if( b == -1 )
   svSendPrintf( cnt, "<input type=\"radio\" name=\"op\" value=\"%d\" disabled> %s<br>&nbsp;&nbsp;&nbsp;<font color=\"#FF0000\">Unavailable</font><br>", a, cmdGhostopName[a] );
  else
  {
   svSendPrintf( cnt, "<input type=\"radio\" name=\"op\" value=\"%d\"> %s<br>", a, cmdGhostopName[a] );
   if( b )
    svSendPrintf( cnt, "&nbsp;&nbsp;&nbsp;<font color=\"#FF0000\">Low research penalty : %d%%</font><br>", b );
  }
 }

 svSendString( cnt, "</td><td width=\"35%\" align=\"center\" valign=\"top\" nowrap>" );
 svSendPrintf( cnt, "%d %s<br>", fleetd.unit[CMD_UNIT_GHOST], cmdUnitName[CMD_UNIT_GHOST] );
 svSendPrintf( cnt, "%d%% Psychics readiness<br>", maind.readiness[1] >> 16 );
 svSendPrintf( cnt, "Send: <input type=\"text\" size=\"10\" name=\"sendghosts\" value=\"%d\"><br><br>", fleetd.unit[CMD_UNIT_GHOST] );
 if( plnid >= 0 )
  svSendPrintf( cnt, "Target system or planet<br> X:<input type=\"text\" size=\"4\" name=\"sysx\" value=\"%d\"> Y:<input type=\"text\" size=\"4\" name=\"sysy\" value=\"%d\"> Planet:<input type=\"text\" size=\"4\" name=\"planet\" value=\"%d\">", ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, planetd.position & 0xFF );
 else
  svSendPrintf( cnt, "Target system or planet<br> X:<input type=\"text\" size=\"4\" name=\"sysx\"> Y:<input type=\"text\" size=\"4\" name=\"sysy\"> Planet:<input type=\"text\" size=\"4\" name=\"planet\">" );
 svSendString( cnt, "<br><br><input type=\"submit\" value=\"Prepare incantation\">" );
 svSendString( cnt, "</td></tr></table></form>" );


 if( ( b = dbUserSpecOpList( id, &specopd ) ) >= 0 )
 {
  svSendString( cnt, "<table width=\"80%\"><tr><td width=\"40%\" align=\"center\" valign=\"top\"><b>Your operations</b><br><br>" );
  for( a = c = 0 ; a < b ; a++ )
  {
   if( specopd[a].type & 0x10000 )
    continue;
   c++;
   optype = ( specopd[a].type & 0xF000 ) >> 12;
   opid = specopd[a].type & 0xFFF;
   if( optype == 0 )
   {
    svSendPrintf( cnt, "%s for %d weeks at <a href=\"planet?id=%d\">%d,%d:%d</a>", cmdAgentopName[opid], specopd[a].time, specopd[a].plnid, ( specopd[a].plnpos >> 8 ) & 0xFFF, specopd[a].plnpos >> 20, specopd[a].plnpos & 0xFF );
    dbMapRetrievePlanet( specopd[a].plnid, &planetd );
    if( planetd.owner == -1 )
     svSendString( cnt, ", unhabited" );
    else
    {
     if( ( user = dbUserLinkID( specopd[a].factionid ) ) )
      svSendPrintf( cnt, " on <a href=\"player?id=%d\">%s</a>", specopd[a].factionid, user->faction );
    }
    if( opid == CMD_OPER_HIGHINFIL )
     svSendPrintf( cnt, " - <a href=\"specinfos?id=%d\">See information</a>", a );
    svSendString( cnt, "<br>" );
   }
   else if( optype == 1 )
   {
    if( !( user = dbUserLinkID( specopd[a].factionid ) ) )
     continue;
    svSendPrintf( cnt, "%s for %d weeks on ", cmdPsychicopName[opid], specopd[a].time );
    if( id != specopd[a].factionid )
     svSendPrintf( cnt, "<a href=\"player?id=%d\">%s</a>", specopd[a].factionid, user->faction );
    else
     svSendString( cnt, "yourself" );
    if( opid == CMD_SPELL_DARKWEB )
     svSendPrintf( cnt, " - +%d%% FR protection", specopd[a].vars[0] );
    else if( opid == CMD_SPELL_BLACKMIST )
     svSendPrintf( cnt, " - -%d%% solar energy", specopd[a].vars[0] );
    else if( opid == CMD_SPELL_WARILLUSIONS )
     svSendPrintf( cnt, " - +%d%% units protection", specopd[a].vars[0] );

    svSendString( cnt, "<br>" );
   }
   else if( optype == 2 )
   {
    if( !( user = dbUserLinkID( specopd[a].factionid ) ) )
     continue;
    svSendPrintf( cnt, "%s for %d weeks on ", cmdGhostopName[opid], specopd[a].time );
    if( !( cmdGhostopFlags[opid] & 4 ) )
    {
     svSendPrintf( cnt, "<a href=\"planet?id=%d\">%d,%d:%d</a>", specopd[a].plnid, ( specopd[a].plnpos >> 8 ) & 0xFFF, specopd[a].plnpos >> 20, specopd[a].plnpos & 0xFF );
     dbMapRetrievePlanet( specopd[a].plnid, &planetd );
     if( planetd.owner == -1 )
      svSendString( cnt, ", unhabited" );
     else
     {
      if( ( user = dbUserLinkID( specopd[a].factionid ) ) )
       svSendPrintf( cnt, " of <a href=\"player?id=%d\">%s</a>", specopd[a].factionid, user->faction );
     }
    }
    else
    {
     if( specopd[a].plnid != -1 )
      svSendPrintf( cnt, "<a href=\"system?id=%d\">%d,%d</a>", specopd[a].plnid, ( specopd[a].plnpos >> 8 ) & 0xFFF, specopd[a].plnpos >> 20 );
     else
      svSendPrintf( cnt, "%d,%d", ( specopd[a].plnpos >> 8 ) & 0xFFF, specopd[a].plnpos >> 20 );
    }
    if( opid == CMD_INCANT_SURVEY )
     svSendPrintf( cnt, " - <a href=\"specinfos?id=%d\">See information</a>", a );
    else if( opid == CMD_INCANT_SHIELDING )
     svSendPrintf( cnt, " - Shielding : %d", specopd[a].vars[0] );
    svSendString( cnt, "<br>" );
   }
  }
  if( !( c ) )
   svSendString( cnt, "None" );
  svSendString( cnt, "</td><td width=\"40%\" align=\"center\" valign=\"top\"><b>Known operations affecting your faction</b><br><br>" );
  for( a = c = 0 ; a < b ; a++ )
  {
   if( !( specopd[a].type & 0x10000 ) )
    continue;
   c++;
   optype = ( specopd[a].type & 0xF000 ) >> 12;
   opid = specopd[a].type & 0xFFF;
   if( optype == 0 )
    svSendPrintf( cnt, "%s for %d weeks from", cmdAgentopName[opid], specopd[a].time );
   else if( optype == 1 )
    svSendPrintf( cnt, "%s for %d weeks from", cmdPsychicopName[opid], specopd[a].time );
   else
    svSendPrintf( cnt, "%s for %d weeks from", cmdGhostopName[opid], specopd[a].time );
   if( ( specopd[a].factionid == -1 ) || !( user = dbUserLinkID( specopd[a].factionid ) ) )
    svSendString( cnt, " an unknown faction<br>" );
   else
    svSendPrintf( cnt, " <a href=\"player?id=%d\">%s</a>", specopd[a].factionid, user->faction );

   if( optype == 2 )
   {
    if( !( cmdGhostopFlags[opid] & 4 ) && ( dbMapRetrievePlanet( specopd[a].plnid, &planetd ) >= 0 ) )
     svSendPrintf( cnt, " on planet <a href=\"planet?id=%d\">%d,%d:%d</a>", specopd[a].plnid, ( specopd[a].plnpos >> 8 ) & 0xFF, specopd[a].plnpos >> 20, specopd[a].plnpos & 0xFF );
    if( opid == CMD_INCANT_SHIELDING )
     svSendPrintf( cnt, " - Shielding : %d", specopd[a].vars[0] );
   }

   svSendString( cnt, "<br>" );
  }
  if( !( c ) )
   svSendString( cnt, "None" );
  svSendString( cnt, "</td></tr></table>" );
  free( specopd );
 }

 if( plnid != -1 )
 {
  svSendPrintf( cnt, "<br><br><a href=\"planet?id=%d\">View planet</a>", plnid );
  svSendPrintf( cnt, "<br><br><a href=\"system?id=%d\">View system</a>", planetd.system );
 }

 iohttpBodyEnd( cnt );
 return;
}


void iohttpFunc_specinfos( svConnectionPtr cnt )
{
 int a, b, c, d, x, y, id, optype, opid, specopid, num;
 int totals[7];
 char szChaine[20];
 int *buffer;
 float fa, totalob;
 unsigned char *specopstring;
 dbUserPtr user;
 dbUserMainDef maind;
 dbUserSpecOpDef specopd;
 dbMainPlanetDef planetd;
 dbUserMainDef main2d;
 dbUserFleetPtr fleetd;
 dbMainSystemDef systemd;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;

 iohttpVarsInit( cnt );
 specopstring = iohttpVarsFind( "id" );
 iohttpVarsCut();

 iohttpBodyInit( cnt, "Operation information" ); 
 if( !( specopstring ) || ( sscanf( specopstring, "%d", &specopid ) != 1 ) || ( dbUserSpecOpRetrieve( id, specopid, &specopd ) < 0 ) || ( specopd.type & 0x10000 ) )
 {
  svSendString( cnt, "<i>Invalid operation</i><br>" );
  iohttpBodyEnd( cnt );
  return;
 }

 optype = ( specopd.type & 0xF000 ) >> 12;
 opid = specopd.type & 0xFFF;
 if( optype == 0 )
 {
  if( opid != CMD_OPER_HIGHINFIL )
   goto iohttpFunc_specinfosL0;
  svSendPrintf( cnt, "<b>%s</b> for %d weeks at <a href=\"planet?id=%d\">%d,%d:%d</a>", cmdAgentopName[opid], specopd.time, specopd.plnid, ( specopd.plnpos >> 8 ) & 0xFFF, specopd.plnpos >> 20, specopd.plnpos & 0xFF );
  dbMapRetrievePlanet( specopd.plnid, &planetd );
  if( planetd.owner == -1 )
   svSendString( cnt, ", unhabited<br><br>" );
  else
  {
   if( ( user = dbUserLinkID( planetd.owner ) ) )
    svSendPrintf( cnt, " on <a href=\"player?id=%d\">%s</a><br><br>", planetd.owner, user->faction );
  }

  if( opid == CMD_OPER_HIGHINFIL )
  {
   if( dbUserMainRetrieve( specopd.factionid, &main2d ) < 0 )
   {
    svSendString( cnt, "Error while retriving main user data" );
    return;
   }
   svSendPrintf( cnt, "<br><table><tr><td valign=\"top\"><i>Faction</i><br>" );
   svSendPrintf( cnt, "Planets : <b>%d</b><br>", main2d.planets );
   svSendPrintf( cnt, "Networth : <b>%lld</b><br>", main2d.networth );
   svSendPrintf( cnt, "Empire : <b>#%d</b><br>", main2d.empire );
   svSendPrintf( cnt, "Fleet readiness : <b>%d %%</b><br>", main2d.readiness[0] >> 16 );
   svSendPrintf( cnt, "Psychics readiness : <b>%d %%</b><br>", main2d.readiness[1] >> 16 );
   svSendPrintf( cnt, "Agents readiness : <b>%d %%</b><br>", main2d.readiness[2] >> 16 );
   svSendPrintf( cnt, "</td><td valign=\"top\"><i>Resources</i><br>" );
   for( a = 0; a < CMD_RESSOURCE_NUMUSED ; a++ )
    svSendPrintf( cnt, "%s : <b>%lld</b><br>", cmdRessourceName[a], main2d.ressource[a] );
   svSendPrintf( cnt, "</td><td valign=\"top\"><i>Research fields</i><br>" );
   for( a = 0; a < CMD_RESEARCH_NUMUSED ; a++ )
    svSendPrintf( cnt, "%s : <b>%lld %%</b><br>", cmdResearchName[a], main2d.totalresearch[a] );
   svSendPrintf( cnt, "</td></tr></table><br><br>" );
   if( specopd.vars[0] & 1 )
   {
    if( ( b = dbUserPlanetListIndices( specopd.factionid, &buffer ) ) < 0 )
    {
     svSendString( cnt, "Error while retriving planets list" );
     return;
    }
    svSendString( cnt, "<table width=\"100%%\"><tr><td width=\"15%%\"><b>Planet</b></td><td width=\"10%%\">Size</td><td width=\"20%%\">Buildings - Penalty</td><td width=\"25%%\">Population</td><td width=\"28%\">Protection</td></tr>" );
    memset( totals, 0, 7*sizeof(int) );
    totalob = 0.0;
    for( a = 0 ; a < b ; a++ )
    {
     dbMapRetrievePlanet( buffer[a], &planetd );
     for( c = d = 0 ; c < CMD_BLDG_NUMUSED ; c++ )
      d += planetd.building[c];
     svSendPrintf( cnt, "<tr><td><a href=\"planet?id=%d\">%d,%d:%d</a></td><td>%d</td><td>%d", buffer[a], ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, planetd.position & 0xFF, planetd.size, d );
     if( planetd.construction )
      svSendPrintf( cnt, " ( %d )", planetd.construction );
     for( c = CMD_BLDG_OVERBUILD ; c < CMD_BLDG_NUMUSED ; c++ )
      d -= planetd.building[c];
     fa = cmdGetBuildOvercost( planetd.size, d+planetd.construction );
     if( fa > 1.0 )
      svSendPrintf( cnt, " <font color=\"#FF2020\">+%.2f%%</font>", 100.0 * (fa-1.0) );
     totalob += fa;
     totals[0] += planetd.size;
     totals[1] += d;
     totals[2] += planetd.construction;
     svSendPrintf( cnt, "</td><td>%d0 / %d0</td><td>", planetd.population, planetd.maxpopulation );
     if( planetd.flags & CMD_PLANET_FLAGS_PORTAL )
     {
      svSendString( cnt, " Portal -" );
      totals[5]++;
     }
     if( planetd.protection >= 70 )
      svSendString( cnt, " <font color=\"#00FF00" );
     else if( planetd.protection >= 40 )
      svSendString( cnt, " <font color=\"#FFFF00" );
     else
      svSendString( cnt, " <font color=\"#FF0000" );
     svSendPrintf( cnt, "\">%d%%</font>", planetd.protection );
     for( c = d = 0 ; c < CMD_UNIT_NUMUSED ; c++ )
      d += planetd.unit[c];
     if( d )
     {
      svSendPrintf( cnt, " <i>Units (%d)</i>", d );
      totals[6] += d;
     }

     d = (int)artefactPrecense( &planetd );
     if( d >= 0 )
      svSendPrintf( cnt, " <img src=\"%s\">", artefactImage[d] );
     
     if(planetd.special[1])
     	svSendPrintf( cnt, " <img src=\"pr%d.gif\">+%d%%", planetd.special[0], planetd.special[1] );

     svSendPrintf( cnt, "</td></tr>" );
     totals[3] += planetd.population;
     totals[4] += planetd.maxpopulation;
    }
    svSendPrintf( cnt, "<tr><td>%d Planets</td><td>%d</td><td>%d", b, totals[0], totals[1] );
    if( totals[2] )
     svSendPrintf( cnt, " ( %d )", totals[2] );
    totalob /= (float)b;
    if( totalob > 1.001 )
     svSendPrintf( cnt, " <font color=\"#FF2020\">+%.2f%%</font>", 100.0 * (totalob-1.0) );
    svSendPrintf( cnt, "</td><td>%d0 / %d0</td><td> %d Portals", totals[3], totals[4], totals[5] );
    if( totals[6] )
     svSendPrintf( cnt, " <i>Units (%d)</i>", totals[6] );
    svSendString( cnt, "</td><td>&nbsp;</td></tr></table><br><br>" );
    free( buffer );
   }
   else
    svSendString( cnt, "Gathering information on the faction planets network failed!<br><br>" );
   if( specopd.vars[0] & 2 )
   {
    if( ( num = dbUserFleetList( specopd.factionid, &fleetd ) ) <= 0 )
    {
     svSendString( cnt, "Error while retriving user fleets list</body></html>" );
     iohttpBodyEnd( cnt );
     return;
    }
    svSendString( cnt, "<b>Main fleet</b><br><br>" );
    for( a = b = 0 ; a < CMD_UNIT_NUMUSED ; a++ )
    {
     if( fleetd[0].unit[a] )
     {
      svSendPrintf( cnt, "%d %s<br>", fleetd[0].unit[a], cmdUnitName[a] );
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
       if( fleetd[a].unit[b] )
        svSendPrintf( cnt, "%d %s<br>", fleetd[a].unit[b], cmdUnitName[b] );
      }
      cmdFleetGetPosition( &fleetd[a], &x, &y );
      svSendPrintf( cnt, "<td>%d,%d</td></tr>", x, y );
     } 
     svSendString( cnt, "</table>" );
    }
   }
   else
    svSendString( cnt, "Gathering information on the faction fleets failed!<br><br>" );
  }
  if( specopd.plnid != -1 )
   svSendPrintf( cnt, "<br><br><a href=\"planet?id=%d\">View planet</a>", specopd.plnid );
 }
 else if( optype == 1 )
 {
  goto iohttpFunc_specinfosL0;
  if( !( user = dbUserLinkID( specopd.factionid ) ) )
   return;
  svSendPrintf( cnt, "<b>%s</b> for %d weeks on <a href=\"player?id=%d\">%s</a><br>", cmdPsychicopName[opid], specopd.time, specopd.factionid, user->faction );
  if( specopd.plnid != -1 )
   svSendPrintf( cnt, "<br><br><a href=\"planet?id=%d\">View planet</a>", specopd.plnid );
 }
 else if( optype == 2 )
 {
  svSendPrintf( cnt, "<b>%s</b> for %d weeks", cmdGhostopName[opid], specopd.time );
  if( specopd.plnid != -1 )
  {
   if( !( cmdGhostopFlags[opid] & 4 ) )
    svSendPrintf( cnt, " on <a href=\"planet?id=%d\">%d,%d:%d</a>", specopd.plnid, ( specopd.plnpos >> 8 ) & 0xFF, specopd.plnpos >> 20, specopd.plnpos & 0xFF );
   else
    svSendPrintf( cnt, " on <a href=\"system?id=%d\">%d,%d</a>", specopd.plnid, ( specopd.plnpos >> 8 ) & 0xFF, specopd.plnpos >> 20 );
  }
  svSendString( cnt, "<br><br>" );

  if( opid == CMD_INCANT_SURVEY )
  {
   if( dbMapRetrieveSystem( specopd.plnid, &systemd ) < 0 )
    return;
   b = systemd.numplanets;
   if( b > 32 )
    b = 32;
   svSendString( cnt, "<table border=\"0\"><tr><td>" );
   for( a = 0 ; a < b ; a++ )
   {
    if( dbMapRetrievePlanet( systemd.indexplanet + a, &planetd ) < 0 )
     continue;
    svSendPrintf( cnt, "<b>Planet <a href=\"planet?id=%d\">%d,%d:%d</a></b>", systemd.indexplanet + a, ( planetd.position >> 8 ) & 0xFF, planetd.position >> 20, planetd.position & 0xFF );
    if( planetd.owner == -1 )
     svSendString( cnt, ", uninhabited<br>" );
    else if( dbUserMainRetrieve( planetd.owner, &main2d ) >= 0 )
     svSendPrintf( cnt, " owned by <a href=\"player?id=%d\">%s</a> of <a href=\"empire?id=%d\">empire #%d</a><br>", planetd.owner, main2d.faction, main2d.empire, main2d.empire );
    if( !( specopd.vars[0] & ( 1 << a ) ) )
    {
     svSendString( cnt, "No information gathered<br><br>" );
     continue;
    }
    svSendPrintf( cnt, "Size : %d<br>", planetd.size );
    d = (int)artefactPrecense( &planetd );
    if( d >= 0 )
     svSendPrintf( cnt, "Artefact : <img src=\"%s\"><br>", artefactImage[d] );
    if( planetd.special[1] )
    {
     switch(planetd.special[0])
     {
     	case 0:
     		strcpy(szChaine, "Energy");
     		break;
     	case 1:
     		strcpy(szChaine, "Mineral");
     		break;
     	case 2:
     		strcpy(szChaine, "Crystal");
     		break;
     	case 3:
     		strcpy(szChaine, "Ectrolium");
     }
     svSendPrintf( cnt, "%s : <img src=\"pr%d.gif\">+%d%%<br>", szChaine, planetd.special[0], planetd.special[1] );
    }
    svSendPrintf( cnt, "Portal : %s<br><br>", ( planetd.flags & CMD_PLANET_FLAGS_PORTAL ) ? "yes" : "no" );
   }
   svSendString( cnt, "</td></tr></table>" );
  }

 }

 iohttpBodyEnd( cnt );
 return;

 iohttpFunc_specinfosL0:
 svSendString( cnt, "This operation doesn't have any additionnal information.<br>" );
 iohttpBodyEnd( cnt );
 return;
}




void iohttpFunc_operation( svConnectionPtr cnt )
{ 
 int a, id, plnid, x, y, z, sysid;
 int agents, specop, arl;
 unsigned char *agentstring, *orderstring, *xstring, *ystring, *zstring;
 dbMainPlanetDef planetd;
 dbUserMainDef maind, main2d;
 dbMainSystemDef systemd;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;

 iohttpVarsInit( cnt );
 orderstring = iohttpVarsFind( "op" );
 agentstring = iohttpVarsFind( "sendagents" );
 xstring = iohttpVarsFind( "planetx" );
 ystring = iohttpVarsFind( "planety" );
 zstring = iohttpVarsFind( "planetnum" );
 iohttpVarsCut();

 if( !( orderstring ) || ( sscanf( orderstring, "%d", &specop ) != 1 ) )
  goto iohttpFunc_operationL0;
 if( (unsigned int)specop >= CMD_AGENTOP_NUMUSED )
  goto iohttpFunc_operationL0;
 if( !( xstring ) || ( sscanf( xstring, "%d", &x ) != 1 ) )
  goto iohttpFunc_operationL0;
 if( !( ystring ) || ( sscanf( ystring, "%d", &y ) != 1 ) )
  goto iohttpFunc_operationL0;
 if( !( zstring ) || ( sscanf( zstring, "%d", &z ) != 1 ) )
  goto iohttpFunc_operationL0;
 if( !( agentstring ) || ( sscanf( agentstring, "%d", &agents ) != 1 ) )
 {
  iohttpFunc_operationL0:
  iohttpBodyInit( cnt, "Agents operation", x, y, z ); 
  svSendString( cnt, "<i>Invalid order or coordinates</i><br>" );
  iohttpBodyEnd( cnt );
  return;
 }

 iohttpBodyInit( cnt, "Agents operation on planet %d,%d:%d", x, y, z ); 

 if( ( sysid = dbMapFindSystem( x, y ) ) < 0 )
 {
  svSendString( cnt, "<i>It seems there is no system at the coordinates entered.</i><br>" );
  iohttpBodyEnd( cnt );
  return;
 }

 if( dbMapRetrieveSystem( sysid, &systemd ) < 0 )
 {
  if( cmdErrorString )
   svSendPrintf( cnt, "<i>%s</i><br>", cmdErrorString );
  else
   svSendString( cnt, "<i>Error encountered when getting System Data</i><br>" );
  iohttpBodyEnd( cnt );
  return;
 }
 if( (unsigned int)z >= systemd.numplanets )
  goto iohttpFunc_operationL0;
 plnid = systemd.indexplanet + z;

 if( dbMapRetrievePlanet( plnid, &planetd ) < 0 )
 {
  if( cmdErrorString )
   svSendPrintf( cnt, "<i>%s</i>", cmdErrorString );
  else
   svSendString( cnt, "<i>Error encountered when getting planet data</i>" );
  iohttpBodyEnd( cnt );
  return;
 }
	
 // only with observe planets you can cast ops on an uninhabited planet.
 if( ( planetd.owner == -1 ) && ( specop != CMD_OPER_OBSERVEPLANET ) )
 {
  svSendPrintf ( cnt, "<i>You can't perform %s on an uninhabited planet.</i>", cmdAgentopName[specop] );
  iohttpBodyEnd( cnt );
  return;
 }

 if( planetd.owner != -1 )
 {
  if( dbUserMainRetrieve( planetd.owner, &main2d ) < 0 )
   svSendString( cnt, "<i>Error encountered when getting Faction Data</i><br>" );
  svSendPrintf( cnt, "Preparing to perform <b>%s</b> on planet <a href=\"planet?id=%d\">%d,%d:%d</a>, owned by <a href=\"player?id=%d\">%s</a> of <a href=\"empire?id=%d\">empire #%d</a>.<br><br>", cmdAgentopName[specop], plnid, x, y, z, planetd.owner, main2d.faction, main2d.empire, main2d.empire );
 }
 else
  svSendPrintf( cnt, "Preparing to perform <b>%s</b> on the unhabited planet <a href=\"planet?id=%d\">%d,%d:%d</a>.<br><br>", cmdAgentopName[specop], plnid, x, y, z );

// we need a table here.
 svSendString( cnt, "<i>" );
 if( specop == CMD_OPER_SPYTARGET )
  svSendString( cnt, "Spy target will reveal information regarding an faction resources and readiness." );
 else if( specop == CMD_OPER_OBSERVEPLANET )
  svSendString( cnt, "Your agents will observe the planet and provide you with all information regarding it, habited or not." );
 else if( specop == CMD_OPER_NETWORKVIRUS )
  svSendString( cnt, "Your agents will introduce viruses in the target faction computers network, destryoing a part of their research files and temporarily increasing buildings upkeep." );
 else if( specop == CMD_OPER_INFILTRATION )
  svSendString( cnt, "Infiltrating the target network will provide you with information regarding its resources, research and buildings." );
 else if( specop == CMD_OPER_BIOINFECTION )
  svSendString( cnt, "Your agents will attempt to spread a dangerous infection which will kill most of the population and spread in nearby planets." );
 else if( specop == CMD_OPER_ENERGYTRANSFER )
  svSendString( cnt, "Your Agents will infiltrate the enemy energy storage facilities and transfer energy to your own energy storage." );
 else if( specop == CMD_OPER_MILITARYSAB )
  svSendString( cnt, "Through an enemy portal, your agents will attempt to reach the enemy fleet and destroy military units." );
 else if( specop == CMD_OPER_NUKEPLANET )
  svSendString( cnt, "Your agents will place powerful nuclear devices on the surface of the planet, destroying all buildings and units, leaving in uninhabited." );
 else if( specop == CMD_OPER_HIGHINFIL )
  svSendString( cnt, "Performing this operation will provide you with detailled information about an faction for several weeks." );
 else if( specop == CMD_OPER_PLANETBEACON )
  svSendString( cnt, "Planetary definition." );
 svSendString( cnt, "</i><br><br>" );

 if( ( a = cmdExecSendFleetInfos( id, plnid, 0 ) ) < 0 )
 {
  if( cmdErrorString )
   svSendPrintf( cnt, "<i>%s</i><br>", cmdErrorString );
  else
   svSendString( cnt, "<i>Error while retrieving send fleet information</i><br>" );
  iohttpBodyEnd( cnt );
  return;
 }
 
 if( !( a ) )
  svSendPrintf( cnt, "Your %d %s could reach this planet instantaneously", agents, cmdUnitName[CMD_UNIT_AGENT] );
 else
  svSendPrintf( cnt, "Your %d %s would reach this planet in %d weeks", agents, cmdUnitName[CMD_UNIT_AGENT], a );

 arl = specopAgentsReadiness( specop, &maind, ( planetd.owner != -1 ) ? ( &main2d ) : ( 0 ) );
 if( arl == -1 )
 {
  svSendPrintf( cnt, ". Unfortunately, we don't have enough %s to perform %s.", cmdResearchName[CMD_RESEARCH_OPERATIONS], cmdAgentopName[specop] );
  iohttpBodyEnd( cnt );
  return;
 }
 svSendPrintf( cnt, " , performing the operation would use %d%% Agent Readiness.<br><br>", arl >> 16 );

 svSendString( cnt, "<form action=\"operationsend\" method=\"POST\">" );
 svSendPrintf( cnt, "<input type=\"hidden\" name=\"X\" value=\"%d\"><input type=\"hidden\" name=\"Y\" value=\"%d\">", x, y );
 svSendPrintf( cnt, "<input type=\"hidden\" name=\"Z\" value=\"%d\"><input type=\"hidden\" name=\"agents\" value=\"%d\">", z, agents );
 svSendPrintf( cnt, "<input type=\"hidden\" name=\"order\" value=\"%d\">", ( specop + CMD_FLEET_ORDER_FIRSTOP ) );
 svSendString( cnt, "<input type=\"submit\" value=\"Send agents\" size=\"10\"></form>" );

 iohttpBodyEnd( cnt );
 return;
}


void iohttpFunc_spell( svConnectionPtr cnt )
{
 int a, id, targetid;
 int psychics, specop, wrl;
 unsigned char *psychicstring, *orderstring, *targetstring;
 dbUserMainDef maind, main2d;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;

 iohttpVarsInit( cnt );
 orderstring = iohttpVarsFind( "op" );
 psychicstring = iohttpVarsFind( "sendpsychics" );
 targetstring = iohttpVarsFind( "target" );
 iohttpVarsCut();

 if( !( orderstring ) || ( sscanf( orderstring, "%d", &specop ) != 1 ) )
  goto iohttpFunc_spellL0;
 if( (unsigned int)specop >= CMD_PSYCHICOP_NUMUSED )
  goto iohttpFunc_spellL0;
 if( !( psychicstring ) || ( sscanf( psychicstring, "%d", &psychics ) != 1 ) )
 {
  iohttpFunc_spellL0:
  iohttpBodyInit( cnt, "Psychics operation" ); 
  svSendString( cnt, "<i>Invalid order or coordinates</i><br>" );
  iohttpBodyEnd( cnt );
  return;
 }

 if( cmdPsychicopFlags[specop] & 2 )
 {
  targetid = id;
  memcpy( &main2d, &maind, sizeof(dbUserMainDef) );
 }
 else
 {
  targetid = -1;
  if( !( targetstring ) )
   goto iohttpFunc_spellL0;
  if( ( targetstring[0] >= '0' ) && ( targetstring[0] <= '9' ) )
   sscanf( targetstring, "%d", &targetid );
  else
  {
   for( a = 0 ; targetstring[a] ; a++ )
   {
    if( targetstring[a] == '+' )
     targetstring[a] = ' ';
    else if( ( targetstring[a] == 10 ) || ( targetstring[a] == 13 ) )
     targetstring[a] = 0;
   }
   targetid = dbUserSearchFaction( targetstring );
  }
  if( ( targetid < 0 ) || ( dbUserMainRetrieve( targetid, &main2d ) < 0 ) )
  {
   iohttpBodyInit( cnt, "Psychics operation" ); 
   svSendPrintf( cnt, "<i>The faction %s does not seem to exist, the syntax must be exact or use the user ID.</i>", targetstring );
   iohttpBodyEnd( cnt );
   return;
  }
 }

 iohttpBodyInit( cnt, "Psychics operation on %s", main2d.faction ); 

 svSendPrintf( cnt, "Casting %s on <a href=\"player?id=%d\">%s</a> of <a href=\"empire?id=%d\">empire #%d</a>.<br><br>", cmdPsychicopName[specop], targetid, main2d.faction, main2d.empire, main2d.empire );

// we need a table here too.
 svSendString( cnt, "<i>" );
 if( specop == CMD_SPELL_IRRAECTRO )
  svSendString( cnt, "Your psychics will attempt to irradiate the target's ectrolium reserves, making it unusable." );
 else if( specop == CMD_SPELL_DARKWEB )
  svSendString( cnt, "Creating a thick dark web spreading in space around your planets will make them much harder to locate and attack." );
 else if( specop == CMD_SPELL_INCANDESCENCE )
  svSendString( cnt, "Your psychics will convert crystal into vast amounts of energy." );
 else if( specop == CMD_SPELL_BLACKMIST )
  svSendString( cnt, "Creating a dense black mist around the target's planets will greatly reduce the effectiveness of solar collectors." );
 else if( specop == CMD_SPELL_WARILLUSIONS )
  svSendString( cnt, "These illusions will help keeping enemy fire away from friendly units." );
 else if( specop == CMD_SPELL_ASSAULT )
  svSendString( cnt, "Your psychics will engage the targeted faction's psychics, to cause large casualities." );
 else if( specop == CMD_SPELL_PHANTOMS )
  svSendString( cnt, "Your psychics will create etheral creatures to join your fleets in battle." );
 svSendString( cnt, "</i><br><br>" );

 wrl = specopPsychicsReadiness( specop, &maind, ( id != targetid ) ? ( &main2d ) : ( 0 ) );
 if( wrl == -1 )
 {
  svSendPrintf( cnt, "Unfortunately, we don't have enough %s to perform %s.", cmdResearchName[CMD_RESEARCH_WELFARE], cmdPsychicopName[specop] );
  iohttpBodyEnd( cnt );
  return;
 }
 svSendPrintf( cnt, "Performing the operation would use %d%% Psychic Readiness.<br><br>", wrl >> 16 );

 svSendString( cnt, "<form action=\"spellsend\" method=\"POST\">" );
 svSendPrintf( cnt, "<input type=\"hidden\" name=\"target\" value=\"%d\"><input type=\"hidden\" name=\"psychics\" value=\"%d\">", targetid, psychics );
 svSendPrintf( cnt, "<input type=\"hidden\" name=\"order\" value=\"%d\">", specop );
 svSendString( cnt, "<input type=\"submit\" value=\"Cast spell\" size=\"10\"></form>" );

 iohttpBodyEnd( cnt );
 return;
}


void iohttpFunc_incant( svConnectionPtr cnt )
{ 
 int a, id, plnid, x, y, z, sysid;
 int ghosts, specop, grl;
 unsigned char *ghoststring, *orderstring, *xstring, *ystring, *zstring;
 dbMainPlanetDef planetd;
 dbUserMainDef maind, main2d;
 dbMainSystemDef systemd;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;

 iohttpVarsInit( cnt );
 orderstring = iohttpVarsFind( "op" );
 ghoststring = iohttpVarsFind( "sendghosts" );
 xstring = iohttpVarsFind( "sysx" );
 ystring = iohttpVarsFind( "sysy" );
 zstring = iohttpVarsFind( "planet" );
 iohttpVarsCut();

 if( !( orderstring ) || ( sscanf( orderstring, "%d", &specop ) != 1 ) )
  goto iohttpFunc_operationL0;
 if( (unsigned int)specop >= CMD_GHOSTOP_NUMUSED )
  goto iohttpFunc_operationL0;
 if( !( xstring ) || ( sscanf( xstring, "%d", &x ) != 1 ) )
  goto iohttpFunc_operationL0;
 if( !( ystring ) || ( sscanf( ystring, "%d", &y ) != 1 ) )
  goto iohttpFunc_operationL0;
 if( !( zstring ) || ( sscanf( zstring, "%d", &z ) != 1 ) )
  z = -1;
 if( !( ghoststring ) || ( sscanf( ghoststring, "%d", &ghosts ) != 1 ) )
 {
  iohttpFunc_operationL0:
  iohttpBodyInit( cnt, "Ghost ships incantation", x, y, z ); 
  svSendString( cnt, "<i>Invalid order or coordinates</i><br>" );
  iohttpBodyEnd( cnt );
  return;
 }
 if( cmdGhostopFlags[specop] & 4 )
  z = -1;

 if( z != -1 )
  iohttpBodyInit( cnt, "Ghost ships operation on planet %d,%d:%d", x, y, z ); 
 else
  iohttpBodyInit( cnt, "Ghost ships operation on system %d,%d", x, y, z ); 

 planetd.owner = -1;
 if( !( cmdGhostopFlags[specop] & 4 ) )
 {
  if( ( sysid = dbMapFindSystem( x, y ) ) < 0 )
  {
   svSendString( cnt, "<i>It seems there is no system at the coordinates entered.</i><br>" );
   iohttpBodyEnd( cnt );
   return;
  }
  if( dbMapRetrieveSystem( sysid, &systemd ) < 0 )
  {
   if( cmdErrorString )
    svSendPrintf( cnt, "<i>%s</i><br>", cmdErrorString );
   else
    svSendString( cnt, "<i>Error encountered when getting System Data</i><br>" );
   iohttpBodyEnd( cnt );
   return;
  }

  if( z == -1 )
   goto iohttpFunc_operationL0;
  if( (unsigned int)z >= systemd.numplanets )
   goto iohttpFunc_operationL0;
  plnid = systemd.indexplanet + z;

  if( dbMapRetrievePlanet( plnid, &planetd ) < 0 )
  {
   if( cmdErrorString )
    svSendPrintf( cnt, "<i>%s</i>", cmdErrorString );
   else
    svSendString( cnt, "<i>Error encountered when getting planet data</i>" );
   iohttpBodyEnd( cnt );
   return;
  }
  if( planetd.owner == -1 )
  {
   svSendPrintf ( cnt, "<i>You can't perform %s on an uninhabited planet.</i>", cmdGhostopName[specop] );
   iohttpBodyEnd( cnt );
   return;
  }
  if( dbUserMainRetrieve( planetd.owner, &main2d ) < 0 )
   svSendString( cnt, "<i>Error encountered when getting faction Data</i><br>" );
  svSendPrintf( cnt, "Preparing to perform <b>%s</b> on planet <a href=\"planet?id=%d\">%d,%d:%d</a>, owned by <a href=\"player?id=%d\">%s</a> of <a href=\"empire?id=%d\">empire #%d</a>.<br><br>", cmdGhostopName[specop], plnid, x, y, z, planetd.owner, main2d.faction, main2d.empire, main2d.empire );
 }
 else
  svSendPrintf( cnt, "Preparing to perform <b>%s</b> at the coordinates %d,%d</a>.<br><br>", cmdGhostopName[specop], x, y );


// we need a table here.
 svSendString( cnt, "<i>" );
 if( specop == CMD_INCANT_SENSE )
  svSendString( cnt, "Your ghost ships will attempt to locate a nearby artefact from the system." );
 else if( specop == CMD_INCANT_SURVEY )
  svSendString( cnt, "Your forces will attempt to determine the size, resource bonus and the precense of portals for each planet of the system." );
 else if( specop == CMD_INCANT_SHIELDING )
  svSendString( cnt, "Your ghost ships will generate shielding protecting defensive units for the planet specified." );
 else if( specop == CMD_INCANT_FORCEFIELD )
  svSendString( cnt, "A Portal force field makes portal travel very difficult for the targeted planet, reducing the owner capabilities to protect it." );
 else if( specop == CMD_INCANT_VORTEX )
  svSendString( cnt, "A vortex portal allows your forces to travel through it to attack, defend nearby planets or return to the main fleet." );
 else if( specop == CMD_INCANT_MINDCONTROL )
  svSendString( cnt, "The ghost ships will attempt to seize control of the targeted planet, through the conversion of leaders and automatic systems. Most of the population is usually killed in the process." );
 else if( specop == CMD_INCANT_ENERGYSURGE )
  svSendString( cnt, "Spreads a destructive wave in a faction network, feeding on its energy reserves, destroying reserves, research and buildings producing energy." );


 svSendString( cnt, "</i><br><br>" );

 if( cmdFindDistPortal( id, x, y, &a, 0 ) <= -2 )
 {
  svSendString( cnt, "<i>You don't have any portal to send the ghost ships from!</i><br>" );
  iohttpBodyEnd( cnt );
  return;
 }
 a = (int)( (float)a / cmdRace[maind.raceid].speed ) >> 8;
	if( !( a ) )
  svSendPrintf( cnt, "Your %d %s could reach this planet instantaneously", ghosts, cmdUnitName[CMD_UNIT_GHOST] );
 else
  svSendPrintf( cnt, "Your %d %s would reach this planet in %d weeks", ghosts, cmdUnitName[CMD_UNIT_GHOST], a );


 grl = specopGhostsReadiness( specop, &maind, ( planetd.owner != -1 ) ? ( &main2d ) : ( 0 ) );
 if( grl == -1 )
 {
  svSendPrintf( cnt, ". Unfortunately, we don't have enough %s to perform %s.", cmdResearchName[CMD_RESEARCH_WELFARE], cmdGhostopName[specop] );
  iohttpBodyEnd( cnt );
  return;
 }
 svSendPrintf( cnt, " , performing the operation would use %d%% Psychic Readiness.<br><br>", grl >> 16 );


 svSendString( cnt, "<form action=\"incantsend\" method=\"POST\">" );
 svSendPrintf( cnt, "<input type=\"hidden\" name=\"X\" value=\"%d\"><input type=\"hidden\" name=\"Y\" value=\"%d\">", x, y );
 svSendPrintf( cnt, "<input type=\"hidden\" name=\"Z\" value=\"%d\"><input type=\"hidden\" name=\"ghosts\" value=\"%d\">", z, ghosts );
 svSendPrintf( cnt, "<input type=\"hidden\" name=\"order\" value=\"%d\">", ( specop + CMD_FLEET_ORDER_FIRSTINCANT ) );
 svSendString( cnt, "<input type=\"submit\" value=\"Send ghost ships\" size=\"10\"></form>" );

 iohttpBodyEnd( cnt );
 return;
}



void iohttpFunc_operationsend( svConnectionPtr cnt )
{
 int a, id, order, x, y, z, agents;
 dbUserMainDef maind;
 unsigned char *orderstring, *xstring, *ystring, *zstring, *agentstring;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Sending agents" );

 iohttpVarsInit( cnt );
 orderstring = iohttpVarsFind( "order" );
 xstring = iohttpVarsFind( "X" );
 ystring = iohttpVarsFind( "Y" );
 zstring = iohttpVarsFind( "Z" );
 agentstring = iohttpVarsFind( "agents" );
 iohttpVarsCut();

 if( !( orderstring ) || ( sscanf( orderstring, "%d", &order ) != 1 ) )
  goto iohttpFunc_fleetsendL0;
 if( !( xstring ) || ( sscanf( xstring, "%d", &x ) != 1 ) )
  goto iohttpFunc_fleetsendL0;
 if( !( ystring ) || ( sscanf( ystring, "%d", &y ) != 1 ) )
  goto iohttpFunc_fleetsendL0;
 if( !( zstring ) || ( sscanf( zstring, "%d", &z ) != 1 ) )
  goto iohttpFunc_fleetsendL0;
 if( !( agentstring ) || ( sscanf( agentstring, "%d", &agents ) != 1 ) )
 {
  iohttpFunc_fleetsendL0:
  svSendString( cnt, "<i>Invalid order or coordinates</i><br>" );
  iohttpBodyEnd( cnt );
  return;
 }

 if( ( a = cmdExecSendAgents( id, x, y, z, order, agents ) ) < 0 )
 {
  if( cmdErrorString )
   svSendPrintf( cnt, "<i>%s</i><br>", cmdErrorString );
  else
   svSendString( cnt, "<i>Error encountered when sending agents</i><br>" );
  iohttpBodyEnd( cnt );
  return;
 }

 svSendString( cnt, "<i>Fleet sent!</i><br><br>" );
 if( a )
  svSendPrintf( cnt, "The agents will reach their destination in %d weeks<br>", a );
 else
 {
  svSendString( cnt, "The agents reached their destination<br>" );
  if( !( cmdFleetActionNewd[1] & CMD_NEWS_FLAGS_NEW ) )
   iohttpNewsString( cnt, cmdFleetActionNewd );
 }

 iohttpBodyEnd( cnt );
 return;
}


void iohttpFunc_spellsend( svConnectionPtr cnt )
{
 int id, order, psychics, targetid;
 dbUserMainDef maind;
 unsigned char *orderstring, *psychicstring, *targetstring;
 long long int newd[DB_USER_NEWS_BASE];

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Casting spell" );

 iohttpVarsInit( cnt );
 orderstring = iohttpVarsFind( "order" );
 psychicstring = iohttpVarsFind( "psychics" );
 targetstring = iohttpVarsFind( "target" );
 iohttpVarsCut();

 if( !( orderstring ) || ( sscanf( orderstring, "%d", &order ) != 1 ) )
  goto iohttpFunc_spellsendL0;
 if( !( targetstring ) || ( sscanf( targetstring, "%d", &targetid ) != 1 ) )
  goto iohttpFunc_spellsendL0;
 if( !( psychicstring ) || ( sscanf( psychicstring, "%d", &psychics ) != 1 ) )
 {
  iohttpFunc_spellsendL0:
  svSendString( cnt, "<i>Invalid order or coordinates</i><br>" );
  iohttpBodyEnd( cnt );
  return;
 }

 newd[0] = svTickNum;
 newd[1] = 0;
 specopPsychicsPerformOp( id, targetid, order, psychics, newd );

 iohttpNewsString( cnt, newd );

 svSendString( cnt, "<br>" );

 iohttpBodyEnd( cnt );
 return;
}


void iohttpFunc_incantsend( svConnectionPtr cnt )
{
 int a, id, order, x, y, z, ghosts;
 dbUserMainDef maind;
 unsigned char *orderstring, *xstring, *ystring, *zstring, *ghoststring;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Sending ghost ships" );

 iohttpVarsInit( cnt );
 orderstring = iohttpVarsFind( "order" );
 xstring = iohttpVarsFind( "X" );
 ystring = iohttpVarsFind( "Y" );
 zstring = iohttpVarsFind( "Z" );
 ghoststring = iohttpVarsFind( "ghosts" );
 iohttpVarsCut();

 if( !( orderstring ) || ( sscanf( orderstring, "%d", &order ) != 1 ) )
  goto iohttpFunc_fleetsendL0;
 if( !( xstring ) || ( sscanf( xstring, "%d", &x ) != 1 ) )
  goto iohttpFunc_fleetsendL0;
 if( !( ystring ) || ( sscanf( ystring, "%d", &y ) != 1 ) )
  goto iohttpFunc_fleetsendL0;
 if( !( zstring ) || ( sscanf( zstring, "%d", &z ) != 1 ) )
  goto iohttpFunc_fleetsendL0;
 if( !( ghoststring ) || ( sscanf( ghoststring, "%d", &ghosts ) != 1 ) )
 {
  iohttpFunc_fleetsendL0:
  svSendString( cnt, "<i>Invalid order or coordinates</i><br>" );
  iohttpBodyEnd( cnt );
  return;
 }

 if( ( a = cmdExecSendGhosts( id, x, y, z, order, ghosts ) ) < 0 )
 {
  if( cmdErrorString )
   svSendPrintf( cnt, "<i>%s</i><br>", cmdErrorString );
  else
   svSendString( cnt, "<i>Error encountered when sending ghosts</i><br>" );
  iohttpBodyEnd( cnt );
  return;
 }

 svSendString( cnt, "<i>Fleet sent!</i><br><br>" );
 if( a )
  svSendPrintf( cnt, "The ghosts will reach their destination in %d weeks<br>", a );
 else
 {
  svSendString( cnt, "The ghosts reached their destination<br>" );
  if( !( cmdFleetActionNewd[1] & CMD_NEWS_FLAGS_NEW ) )
   iohttpNewsString( cnt, cmdFleetActionNewd );
 }

 iohttpBodyEnd( cnt );
 return;
}







void iohttpFunc_research( svConnectionPtr cnt )
{
 int a, b, id, cmd[3];
 unsigned char fundstring[1024];
 unsigned char *fund;
 unsigned char *rschptr[CMD_RESEARCH_NUMUSED];
 int rschvalue[CMD_RESEARCH_NUMUSED];
 unsigned char rschname[16];
 dbUserMainDef maind;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;

 iohttpVarsInit( cnt );
 for( a = 0 ; a < CMD_RESEARCH_NUMUSED ; a++ )
 {
  sprintf( rschname, "rsch%d", a );
  rschptr[a] = iohttpVarsFind( rschname );
 }
 fund = iohttpVarsFind( "fund" );
 iohttpVarsCut();

 fundstring[0] = 0;
 if( ( fund ) && ( sscanf( fund, "%d", &a ) == 1 ) )
 {
  cmd[0] = CMD_FUND_RESEARCH;
  cmd[1] = id;
  cmd[2] = a;
  if( ( b = cmdExecute( cnt, cmd, 0, 0 ) ) < 0 )
  {
   if( cmdErrorString )
    sprintf( fundstring, "<i>%s</i><br><br>", cmdErrorString );
   else
    sprintf( fundstring, "<i>Error while funding research</i><br><br>" );
  }
  else
   sprintf( fundstring, "<i>Research funding increased by %d</i><br><br>", cmd[2] );
 }
 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Research" );

 for( a = 0 ; a < CMD_RESEARCH_NUMUSED ; a++ )
 {
  if( !( rschptr[a] ) || ( sscanf( rschptr[a], "%d", &rschvalue[a] ) < 1 ) )
   goto iohttpFunc_researchL0;
 }
 cmd[0] = CMD_SET_RESEARCH;
 cmd[1] = id;
 if( ( b = cmdExecute( cnt, cmd, rschvalue, 0 ) ) < 0 )
 {
  if( cmdErrorString )
   svSendPrintf( cnt, "<i>%s</i><br><br>", cmdErrorString );
  else
   svSendString( cnt, "<i>Error while setting research</i><br><br>" );
 }
 else
 {
  svSendString( cnt, "<i>Research priorities changed</i><br><br>" );
  memcpy( maind.allocresearch, rschvalue, CMD_RESEARCH_NUMUSED*sizeof(int) );
 }
 iohttpFunc_researchL0:
 if( fundstring )
  svSendString( cnt, fundstring );

 svSendPrintf( cnt, "Increase fundings<br><form action=\"research\" method=\"POST\"><input type=\"text\" name=\"fund\" size=\"12\">&nbsp;<input type=\"submit\" value=\"Fund\"></form><br>Current research fundings : %lld<br><br>", maind.fundresearch );

 svSendString( cnt, "<form action=\"research\" method=\"POST\"><table width=\"90%%\" cellspacing=\"8\">" );
 for( a = 0 ; a < CMD_RESEARCH_NUMUSED ; a++ )
  svSendPrintf( cnt, "<tr><td nowrap><b>%s</b></td><td nowrap>%lld Points</td><td nowrap>%lld%%</td><td>%s</td><td><input type=\"text\" name=\"rsch%d\" value=\"%d\" size=\"6\">%%</td></tr>", cmdResearchName[a], maind.research[a], maind.totalresearch[a], cmdResearchDesc[a], a, maind.allocresearch[a] );
 svSendString( cnt, "<tr><td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td><td><input type=\"submit\" value=\"Change\"></td></tr></table></form>" );

 iohttpBodyEnd( cnt );
 return;
}



#define IOHTTP_MAIL_BUFFER (65536)

#define IOHTTP_MAIL_MAXMESSAGES (256)

void iohttpFunc_mail( svConnectionPtr cnt )
{
 int a, b, skip, id, cmd[2], type, msent;
 dbUserMainDef maind, main2d;
 dbMailPtr mails;
 dbMailDef maild;
 unsigned char *tostring, *mailstring, *typestring, *deletestring, *deleteallstring, *skipstring;
 unsigned char timebuf[256];
 unsigned char *text;
 long long int newd[DB_USER_NEWS_BASE];

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
  return;

 iohttpVarsInit( cnt );
 tostring = iohttpVarsFind( "to" );
 mailstring = iohttpVarsFind( "mail" );
 typestring = iohttpVarsFind( "type" );
 deletestring = iohttpVarsFind( "delete" );
 deleteallstring = iohttpVarsFind( "deleteall" );
 skipstring = iohttpVarsFind( "skip" );
 iohttpVarsCut();

 if( !( iohttpHeader( cnt, id, &maind ) ) )
  return;
 iohttpBodyInit( cnt, "Mail" );

 svSendString( cnt, "<a href=\"mail?type=0\">Received messages</a> - <a href=\"mail?type=1\">Sent messages</a> - <a href=\"mail\">Write a message</a>" );
 if( typestring )
 {
  type = 0;
  sscanf( typestring, "%d", &type );
  svSendPrintf( cnt, " - <a href=\"mail?type=%d&deleteall=1\">Delete all</a>", type );
 }
 svSendString( cnt, "<br><br>" );

 skip = 0;
 if( ( skipstring ) )
  sscanf( skipstring, "%d", &skip );

 msent = 0;
 maild.text = 0;
 if( ( tostring ) && ( mailstring ) )
 {
  if( ( maild.text = malloc( 2*IOHTTP_MAIL_BUFFER ) ) )
  {
	  a = 0;
	  if(id != -1 )
	  {
	  	if(cnt->dbuser)
  		{
		  	if((cnt->dbuser)->level >= LEVEL_MODERATOR )
		   		a = 1;
		  }
	  }
	  iohttpForumFilter( &maild.text[IOHTTP_MAIL_BUFFER], mailstring, IOHTTP_MAIL_BUFFER, a );
	  (maild.mail).length = iohttpForumFilter2( maild.text, &maild.text[IOHTTP_MAIL_BUFFER], IOHTTP_MAIL_BUFFER );
	
	  a = -1;
	  if( ( tostring[0] >= '0' ) && ( tostring[0] <= '9' ) )
	   sscanf( tostring, "%d", &a );
	  else
	  {
	   for( a = 0 ; tostring[a] ; a++ )
	   {
	    if( tostring[a] == '+' )
	     tostring[a] = ' ';
	    else if( ( tostring[a] == 10 ) || ( tostring[a] == 13 ) )
	     tostring[a] = 0;
	   }
	   cmd[0] = CMD_SEARCH_EMPIRE;
	   cmd[1] = id;
	   a = cmdExecute( cnt, cmd, tostring, 0 );
	  }
	  if( a < 0 )
	  {
	   svSendPrintf( cnt, "<i>The faction %s does not seem to exist, the syntax must be exact or use the user ID.</i><br><br>", tostring );
	  }
	  else
	  {
		  cmd[0] = CMD_RETRIEVE_USERMAIN;
		  cmd[1] = a;
		  if( cmdExecute( cnt, cmd, &main2d, 0 ) < 0 )
		  {
		   svSendString( cnt, "<i>Are you sure this user exist?</i><br><br>" );
		  }
				else
				{
			  (maild.mail).authorid = id;
			  sprintf( (maild.mail).authorname, (cnt->dbuser)->faction );
			  (maild.mail).authorempire = maind.empire;
			  (maild.mail).time = time( 0 )-(3600*SERVER_TIME_ZONE);
			  (maild.mail).tick = svTickNum;
			  (maild.mail).flags = 0;
			  if( dbMailAdd( a, 0, &maild ) < 0 )
			  {
			   svSendString( cnt, "<i>Error while sending message</i><br><br>" );
			  }
			  else
			  svSendPrintf( cnt, "<i>Message sent to <a href=\"player?id=%d\">%s</a> of <a href=\"empire?id=%d\">empire #%d</a></i><br><br>", a, main2d.faction, main2d.empire, main2d.empire );
			
			  newd[0] = svTickNum;
			  newd[1] = CMD_NEWS_FLAGS_NEW;
			  newd[2] = CMD_NEWS_MAIL;
			  newd[3] = 0;
			  newd[4] = id;
			  newd[5] = maind.empire;
			  memcpy( &newd[6], maind.faction, 32 );
			  cmdUserNewsAdd( a, newd, CMD_NEWS_FLAGS_MAIL );
			
			  (maild.mail).authorid = a;
			  sprintf( (maild.mail).authorname, main2d.faction );
			  (maild.mail).authorempire = main2d.empire;
			  dbMailAdd( id, 1, &maild );
			
			  msent = 1;
			 }
			}
		}
 }
 if( maild.text )
  free( maild.text );
 
 if( typestring )
 {
 	if( deleteallstring )
  {
   dbMailEmpty( id, type );
   deletestring = 0;
  }
  if( type == 0 )
  {
   if( ( deletestring ) && ( sscanf( deletestring, "%d", &a ) == 1 ) )
   {
    if( dbMailRemove( id, type, a ) >= 0 )
     svSendString( cnt, "<i>Message deleted</i><br><br>" );
    else
     svSendString( cnt, "<i>Error while deleting message</i><br><br>" );
   }
   
   b = dbMailList( id, 0, 0, IOHTTP_MAIL_MAXMESSAGES, &mails, &a );
   if( a > IOHTTP_MAIL_MAXMESSAGES )
    svSendPrintf( cnt, "<i>Your mailbox seems crowded! You have %d messages waiting that won't be displayed until you delete some old messages.</i><br><br>", a - b );
   if( b < 0 )
   {
    svSendString( cnt, "Error retrieving mailbox" );
    iohttpBodyEnd( cnt );
    return;
   }
   if( b == 0 )
    svSendString( cnt, "<br><b>No messages</b><br>" );
   for( b-- ; b >= 0 ; b-- )
   {
    strftime( timebuf, 256, "%T, %b %d %Y", localtime( (time_t *)&(mails[b].mail).time ) );
    svSendPrintf( cnt, "<table width=\"80%%\"><tr><td><b>From</b> : <a href=\"player?id=%d\">%s</a> of <a href=\"empire?id=%d\">empire #%d</a></td><td align=\"right\"><a href=\"mail?type=0&delete=%d\">Delete</a></td></tr><tr><td><b>Received</b> : Week %d, Year %d - %s</td><td align=\"right\"><a href=\"mail?to=%d\">Reply</a></td></tr><tr><td colspan=\"2\">", (mails[b].mail).authorid, (mails[b].mail).authorname, (mails[b].mail).authorempire, (mails[b].mail).authorempire, b, (mails[b].mail).tick % 52, (mails[b].mail).tick / 52, timebuf, (mails[b].mail).authorid );
    svSendString( cnt, mails[b].text );
    svSendString( cnt, "</td></tr></table><br><br>" );
    if( mails[b].text )
     free( mails[b].text );
   }
   if( mails )
    free( mails );
  }
  else
  {
   if( ( deletestring ) && ( sscanf( deletestring, "%d", &a ) == 1 ) )
   {
    if( dbMailRemove( id, type, a ) >= 0 )
     svSendString( cnt, "<i>Message deleted</i><br><br>" );
    else
     svSendString( cnt, "<i>Error while deleting message</i><br><br>" );
   }
   b = dbMailList( id, 1, 0, IOHTTP_MAIL_MAXMESSAGES, &mails, &a );
   if( a > IOHTTP_MAIL_MAXMESSAGES )
    svSendPrintf( cnt, "<i>Your mailbox seems crowded! There are %d sent messages that won't be displayed until you delete some old messages.</i><br><br>", a - b );
   if( b < 0 )
   {
    svSendString( cnt, "Error retrieving mailbox" );
    iohttpBodyEnd( cnt );
    return;
   }
   if( b == 0 )
    svSendString( cnt, "<br><b>No messages</b><br>" );
   for( b-- ; b >= 0 ; b-- )
   {
    strftime( timebuf, 256, "%T, %b %d %Y", localtime( (time_t *)&(mails[b].mail).time ) );
    svSendPrintf( cnt, "<table width=\"80%%\"><tr><td><b>To</b> : <a href=\"player?id=%d\">%s</a> of <a href=\"empire?id=%d\">empire #%d</a></td><td align=\"right\"><a href=\"mail?type=1&delete=%d\">Delete</a></td></tr><tr><td><b>Sent</b> : Week %d, Year %d - %s</td><td align=\"right\"><a href=\"mail?to=%d\">Reply</a></td></tr><tr><td colspan=\"2\">", (mails[b].mail).authorid, (mails[b].mail).authorname, (mails[b].mail).authorempire, (mails[b].mail).authorempire, b, (mails[b].mail).tick % 52, (mails[b].mail).tick / 52, timebuf, (mails[b].mail).authorid );
    svSendString( cnt, mails[b].text );
    svSendString( cnt, "</td></tr></table><br><br>" );
    if( mails[b].text )
     free( mails[b].text );
   }
   if( mails )
    free( mails );
  }
 }
 else
 {
 	svSendString( cnt, "To specify the player to send the message to, enter either the exact faction name or the user ID.<br><br>" );
  svSendString( cnt, "<form action=\"mail\" method=\"POST\"><table width=\"50%%\" cellspacing=\"3\"><tr><td width=\"20%%\">Recipient</td><td>" );
  if( tostring )
  {
   for( a = 0 ; ( a < 31 ) && ( tostring[a] ) ; a++ )
//   for( a = 0 ; ( a < 31 ) && ( tostring[a] ) ; a++ )
   {
    if( tostring[a] == '+' )
     tostring[a] = ' ';
//         if( tostring[a] == '+' )
  //            tostring[a] = ' ';
    else if( ( tostring[a] == 10 ) || ( tostring[a] == 13 ) )
     tostring[a] = 0;
  //    else if( ( tostring[a] == 10 ) || ( tostring[a] == 13 ) )
   //        tostring[a] = 0;
           
   }
   tostring[a] = 0;
   svSendPrintf( cnt, "<input type=\"text\" name=\"to\" size=\"32\" value=\"%s\">", tostring );
  }
  else
   svSendString( cnt, "<input type=\"text\" name=\"to\" size=\"32\">" );
  svSendString( cnt, "</td></tr><tr><td>Message</td><td><textarea name=\"mail\" wrap=\"soft\" rows=\"10\" cols=\"60\">" );
  if( ( msent == 0 ) && ( mailstring ) )
  {
   if( ( text = malloc( 2*IOHTTP_MAIL_BUFFER ) ) )
   {
    a = 0;
    if(cnt->dbuser)
    {
    	if(id != -1 )
     	a = 1;
    }
    iohttpForumFilter( &maild.text[IOHTTP_MAIL_BUFFER], mailstring, IOHTTP_MAIL_BUFFER, a );
    (maild.mail).length = iohttpForumFilter3( maild.text, &maild.text[IOHTTP_MAIL_BUFFER], IOHTTP_MAIL_BUFFER );
    svSendString( cnt, text );
    free( text );
   }
  }
  svSendString( cnt, "</textarea></td></tr><tr><td></td><td><input type=\"submit\" value=\"Send\"></td></tr></table></form>" );
 }

 iohttpBodyEnd( cnt );
 return;
}





void iohttpFunc_rankings( svConnectionPtr cnt )
{
 int id;
 FILE *file;
 struct stat stdata;
 unsigned char *data;
 dbUserMainDef maind;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 2 ) ) >= 0 )
 {
  if( !( iohttpHeader( cnt, id, &maind ) ) )
   return;
 }
 iohttpBodyInit( cnt, "Faction rankings" );

// svSendString( cnt, "<table cellspacing=\"4\"><tr><td>Rank</td><td>Faction</td><td>Empire</td><td>Planets</td><td>Networth</td></tr>" );
 if( stat( DB_DIRECTORY "/ranks.txt", &stdata ) != -1 )
 {
  if( ( data = malloc( stdata.st_size + 1 ) ) )
  {
   data[stdata.st_size] = 0;
   if( ( file = fopen( DB_DIRECTORY "/ranks.txt", "rb" ) ) )
   {
    fread( data, 1, stdata.st_size, file );
    svSendString( cnt, data );
    fclose( file );
   }
   free( data );
  }
 }
// svSendString( cnt, "</table>" );

 iohttpBodyEnd( cnt );
 return;
}

void iohttpFunc_famranks( svConnectionPtr cnt )
{
 int id;
 FILE *file;
 struct stat stdata;
 unsigned char *data;
 dbUserMainDef maind;

 iohttpBase( cnt, 1 );
 if( ( id = iohttpIdentify( cnt, 2 ) ) >= 0 )
 {
  if( !( iohttpHeader( cnt, id, &maind ) ) )
   return;
 }

 iohttpBodyInit( cnt, "Empire rankings" );

// svSendString( cnt, "<table cellspacing=\"4\"><tr><td align=\"right\">Rank</td><td>Name</td><td>Planets</td><td>Players</td><td>Networth</td></tr>" );
 if( stat( DB_DIRECTORY "/famranks.txt", &stdata ) != -1 )
 {
  if( ( data = malloc( stdata.st_size + 1 ) ) )
  {
   data[stdata.st_size] = 0;
   if( ( file = fopen( DB_DIRECTORY "/famranks.txt", "rb" ) ) )
   {
    fread( data, 1, stdata.st_size, file );
    svSendString( cnt, data );
    fclose( file );
   }
   free( data );
  }
 }
// svSendString( cnt, "</table>" );

 iohttpBodyEnd( cnt );
 return;
}

void iohttpFunc_ptrankings( svConnectionPtr cnt )
{
 FILE *file;
 struct stat stdata;
 unsigned char *data;
 svSendString( cnt, "Content-type: text/plain\n\n" );
 if( stat( DB_DIRECTORY "/ranksplain.txt", &stdata ) != -1 )
 {
  if( ( data = malloc( stdata.st_size + 1 ) ) )
  {
   data[stdata.st_size] = 0;
   if( ( file = fopen( DB_DIRECTORY "/ranksplain.txt", "rb" ) ) )
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

void iohttpFunc_ptfamranks( svConnectionPtr cnt )
{
 FILE *file;
 struct stat stdata;
 unsigned char *data;
 svSendString( cnt, "Content-type: text/plain\n\n" );
 if( stat( DB_DIRECTORY "/famranksplain.txt", &stdata ) != -1 )
 {
  if( ( data = malloc( stdata.st_size + 1 ) ) )
  {
   data[stdata.st_size] = 0;
   if( ( file = fopen( DB_DIRECTORY "/famranksplain.txt", "rb" ) ) )
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
 






void iohttpFunc_search( svConnectionPtr cnt )
{
 int id, a, x, y, z, status, sysid;
 dbUserMainDef maind;
 unsigned char *search, *str0, *str1, *str2, *error;
 iohttpDataPtr iohttp = cnt->iodata;
 dbMainSystemDef systemd;
 unsigned char content[256], buf0[256], buf1[256];
 dbUserPtr user;

 iohttpVarsInit( cnt );
 search = iohttpVarsFind( "search" );
 iohttpVarsCut();

 status = 0;
 str0 = 0;
 error = 0;
 if( !( search ) || !( search[0] ) )
 {
  iohttpFunc_searchL0:
  iohttpBase( cnt, 1 );
  if( ( id = iohttpIdentify( cnt, 1|2 ) ) < 0 )
   return;
  if( !( iohttpHeader( cnt, id, &maind ) ) )
   return;
  iohttpBodyInit( cnt, "Search" );

  if( status & 1 )
  {
   if( error )
    svSendString( cnt, error );
   else
    svSendString( cnt, "Unrecognized query format<br><br>" );
  }
  else if( status & 4 )
  {
   svSendPrintf( cnt, "Player search with <i>%s</i> :<br><table><tr><td>", content );
   iohttpCaseLow( buf0, content );
   a = 0;
   for( user = dbUserList ; user ; user = user->next )
   {
    if( !( user->flags & CMD_USER_FLAGS_ACTIVATED ) )
     continue;
    if( a == 256 )
    {
     svSendString( cnt, "<br><i>Too many results!</i>" );
     break;
    }
    iohttpCaseLow( buf1, user->faction );
    if( !( ioCompareFindWords( buf1, buf0 ) ) )
     continue;
    svSendPrintf( cnt, "<a href=\"player?id=%d\">%s</a><br>", user->id, user->faction );
    a++;
   }
   if( a == 0 )
    svSendString( cnt, "No results<br>" );
   svSendString( cnt, "</td></tr></table><br>" );
  }

  svSendString( cnt, "<form action=\"search\" method=\"POST\" target=\"main\"><input type=\"text\" name=\"search\" size=\"32\" value=\"" );
  if( str0 )
   svSendString( cnt, str0 );
  else
   svSendString( cnt, "Search string" );
  svSendString( cnt, "\"><input type=\"submit\" size=\"2\" value=\"Search\"></form><br><br>" );

  svSendString( cnt, "<table><tr><td>" );
  svSendPrintf( cnt, "<b>To find an empire</b><br>Specify the empire number in the search field, as <b>%d</b> or <b>#%d</b>.<br><br>", rand() % dbMapBInfoStatic[4], rand() % dbMapBInfoStatic[4] );
  svSendPrintf( cnt, "<b>To find a system</b><br>Enter the system coordinates in the format <b>x,y</b>, as <b>%d,%d</b>.<br><br>", rand() % dbMapBInfoStatic[0], rand() % dbMapBInfoStatic[1] );
  svSendPrintf( cnt, "<b>To find a planet</b><br>Enter the planet coordinates in the format <b>x,y:z</b>, as <b>%d,%d:%d</b>.<br><br>", rand() % dbMapBInfoStatic[0], rand() % dbMapBInfoStatic[1], rand() & 15 );
  svSendPrintf( cnt, "<b>To see a map sector</b><br>To zoom on a specific area on the map, enter coordinates in the format <b>!x,y</b>, as <b>!%d,%d</b>.<br><br>", rand() % dbMapBInfoStatic[0], rand() % dbMapBInfoStatic[1] );
  svSendString( cnt, "<b>To search an faction name</b><br>To search an faction from its name, enter the partial or full name, it is not case sensitive.<br><br>" );
  svSendPrintf( cnt, "<b>To find an faction</b><br>Enter the faction ID number in the format <b>@ID</b>, as <b>@%d</b>.<br><br>", rand() & 0xFF );
  svSendPrintf( cnt, "<b>To attack a planet</b><br>Enter the planet coordinates in the format <b>&x,y:z</b>, as <b>&%d,%d:%d</b>.<br><br>", rand() % dbMapBInfoStatic[0], rand() % dbMapBInfoStatic[1], rand() & 15 );
  svSendPrintf( cnt, "<b>To perform a special operation on a planet</b><br>Enter the planet coordinates in the format <b>*x,y:z</b>, as <b>*%d,%d:%d</b>.<br><br>", rand() % dbMapBInfoStatic[0], rand() % dbMapBInfoStatic[1], rand() & 15 );
  svSendString( cnt, "</td></tr></table>" );

  iohttpBodyEnd( cnt );
 }
 else
 {
  status = 1;
  iohttp->method = 3;
  iohttp->content = content;

  iohttpForumFilter( content, search, 256, 0 );
  str0 = content;

  // attack
  if( str0[0] == '&' )
  {
   status |= 8;
   str0++;
  }
  // spec
  if( str0[0] == '*' )
  {
   status |= 16;
   str0++;
  }

  // system or planet
  if( ( str1 = ioCompareFindWords( str0, "," ) ) )
  {
   if( ( str2 = ioCompareFindWords( str0, "!" ) ) )
   {
    str0 = str2;
    status |= 2;
   }
   if( sscanf( str0, "%d", &x ) != 1 )
    goto iohttpFunc_searchL0;
   if( sscanf( str1, "%d", &y ) != 1 )
    goto iohttpFunc_searchL0;
   if( status & 2 )
   {
    sprintf( content, "%d,%d", x*IOHTTP_MAPPICK_DIVIDE, y*IOHTTP_MAPPICK_DIVIDE );
    iohttpFunc_map( cnt );
    goto iohttpFunc_searchL1;
   }
   if( ( sysid = dbMapFindSystem( x, y ) ) < 0 )
   {
    error = "There is no system at the coordinates specified<br><br>";
    goto iohttpFunc_searchL0;
   }
   if( dbMapRetrieveSystem( sysid, &systemd ) < 0 )
    goto iohttpFunc_searchL0;
   if( ( str2 = ioCompareFindWords( str1, ":" ) ) )
   {
    if( sscanf( str2, "%d", &z ) != 1 )
     goto iohttpFunc_searchL0;
    if( (unsigned int)z >= systemd.numplanets )
    {
     error = "There is no planet at the coordinates specified<br><br>";
     goto iohttpFunc_searchL0;
    }
    sprintf( content, "id=%d", systemd.indexplanet + z );
    if( status & 8 )
     iohttpFunc_attack( cnt );
    else if( status & 16 )
     iohttpFunc_spec( cnt );
    else
     iohttpFunc_planet( cnt );
    goto iohttpFunc_searchL1;
   }
   sprintf( content, "id=%d", sysid );
   iohttpFunc_system( cnt );
   goto iohttpFunc_searchL1;
  }

  // empire
  if( ( str0[0] >= '0' ) && ( str0[0] <= '9' ) )
  {
   iohttpFunc_searchL2:
   if( sscanf( str0, "%d", &a ) != 1 )
    goto iohttpFunc_searchL0;
   sprintf( content, "id=%d", a );
   iohttpFunc_empire( cnt );
   goto iohttpFunc_searchL1;
  }
  if( str0[0] == '#' )
  {
   str0++;
   goto iohttpFunc_searchL2;
  }

  // player
  if( str0[0] == '@' )
  {
   if( sscanf( &str0[1], "%d", &a ) != 1 )
    goto iohttpFunc_searchL0;
   sprintf( content, "id=%d", a );
   iohttpFunc_player( cnt );
   goto iohttpFunc_searchL1;
  }

  if( ( ( str0[0] >= 'a' ) && ( str0[0] <= 'z' ) ) || ( ( str0[0] >= 'A' ) && ( str0[0] <= 'Z' ) ) )
   status = 4;

  goto iohttpFunc_searchL0;
  iohttpFunc_searchL1:;
 }

 return;
}























#define IOHTTP_FORUM_BUFFER (65536)

#define IOHTTP_FORUM_THREADSNUM (40)

#define IOHTTP_FORUM_POSTSNUM (40)

unsigned char iohttpInputHex( unsigned char *src );

#define IOHTTP_FORUM_SMILETOTAL (62)
#define IOHTTP_FORUM_SMILEBASE (8)

typedef struct
{
 int id;
 unsigned char string[16];
} iohttpForumSmileysDef;

iohttpForumSmileysDef iohttpForumSmileys[IOHTTP_FORUM_SMILETOTAL] =
{
{ 0, ":)" },
{ 1, ":D" },
{ 2, ";)" },
{ 3, "x(" },
{ 3, "X(" },
{ 4, ":(" },
{ 5, ":p" },
{ 5, ":P" },
{ 0, "::smile" },
{ 1, "::laugh" },
{ 2, "::wink" },
{ 3, "::angry" },
{ 4, "::sad" },
{ 5, "::tongue" },
{ 6, "::good" },
{ 7, "::bad" },
{ 8, "::friends" },
{ 9, "::sigh" },
{ 10, "::funny" },
{ 11, "::toilet" },
{ 12, "::wave" },
{ 13, "::fool" },
{ 14, "::king" },
{ 15, "::hammer" },
{ 16, "::party" },
{ 17, "::mad" },
{ 18, "::sleep" },
{ 19, "::sob" },
{ 20, "::cry" },
{ 21, "::angel" },
{ 22, "::popcorn" },
{ 23, "::hat" },
{ 24, "::spin" },
{ 25, "::nuts" },
{ 26, "::snooze" },
{ 27, "::wall" },
{ 28, "::wtf" },
{ 29, "::10on10" },
{ 30, "::1on10" },
{ 31, "::bacon" },
{ 32, "::beer" },
{ 33, "::bounce" },
{ 34, "::cool" },
{ 35, "::crazy" },
{ 36, "::fu" },
{ 37, "::sadder" },
{ 38, "::blame" },
{ 39, "::haha" },
{ 40, "::scream" },
{ 41, "::sobber" },
{ 42, "::ninja" },
{ 43, "::puke" },
{ 44, "::kill" },
{ 53, "::banana" },
{ 46, "::smoke" },
{ 47, "::eyeroller" },
{ 48, "::wow" },
{ 49, "::omg" },
{ 50, "::dead" },
{ 51, "::discohaan" },
{ 52, "::eendje" },
{ 53, "::stick" }
};


int iohttpForumFilter( unsigned char *dest, unsigned char *string, int size, int html )
{
 int a, b;
 unsigned char c;
 for( a = b = 0 ; string[a] ; a++ )
 {
  if( b >= size-1 )
   break;
  c = string[a];
  if( c == '+' )
   c = ' ';
  else if( c == '%' )
  {
   if( !( string[a+1] ) || !( string[a+2] ) )
    break;
   c = iohttpInputHex( &string[a+1] );
   a += 2;
  }
  if( c == 13 )
   continue;
  else if( ( ( c < 32 ) && ( c != 10 ) ) )
   dest[b++] = ' ';
  else if( !( html ) && ( ( c == '<' ) || ( c == '>' ) ) )
   dest[b++] = ' ';
  else
   dest[b++] = c;
 }
 dest[b] = 0;
 return b;
}

int iohttpForumFilter2( unsigned char *dest, unsigned char *string, int size )
{
 int a, b, c;
 unsigned char *string2;
 for( b = c = 0 ; *string ; )
 {
  if( b >= size-20 )
   break;
  if( ( string2 = ioCompareWords( string, "&&**##" ) ) )
  {
   string = string2;
   c ^= 1;
  }
  if( !( c ) )
  {
   for( a = 0 ; a < IOHTTP_FORUM_SMILEBASE ; a++ )
   {
    if( !( string2 = ioCompareWords( string, iohttpForumSmileys[a].string ) ) )
     continue;
    string = string2;
    b += sprintf( &dest[b], "<img src=\"f%02d.gif\">", iohttpForumSmileys[a].id );
    goto iohttpForumFilter2L0;
   }
   if( string[0] == ':' )
   {
    for( ; a < IOHTTP_FORUM_SMILETOTAL ; a++ )
    {
     if( !( string2 = ioCompareWords( string, iohttpForumSmileys[a].string ) ) )
      continue;
     string = string2;
     b += sprintf( &dest[b], "<img src=\"f%02d.gif\">", iohttpForumSmileys[a].id );
     goto iohttpForumFilter2L0;
    }
   }
  }
  if( *string == 10 )
  {
   memcpy( &dest[b], "<br>", 4 );
   b += 4;
   string++;
   continue;
  }
  dest[b++] = *string;
  string++;
  iohttpForumFilter2L0:;
 }
 dest[b] = 0;
 return b;
}

int iohttpForumFilter3( unsigned char *dest, unsigned char *string, int size )
{
 int b, c;
 unsigned char *string2;
 for( b = c = 0 ; *string ; )
 {
  dest[b] = 0;
  if( b >= size-16 )
   break;
  if( ( string2 = ioCompareWords( string, "<br><br><font size=\"1\"><i>Edited by" ) ) )
   break;
  if( ( string2 = ioCompareWords( string, "<br>" ) ) )
  {
   dest[b++] = 10;
   string = string2;
   continue;
  }
  if( !( string2 = ioCompareWords( string, "<img src=\"f" ) ) )
   goto iohttpForumFilter3L0;
  if( !( string2[0] ) || !( string2[1] ) )
   goto iohttpForumFilter3L0;
  c = 10 * ( string2[0]-'0' ) + string2[1]-'0';
  string2 += 2;
  if( !( string2 = ioCompareWords( string2, ".gif\">" ) ) )
   goto iohttpForumFilter3L0;
  if( (unsigned int)c >= IOHTTP_FORUM_SMILETOTAL )
   goto iohttpForumFilter3L0;
  b += sprintf( &dest[b], iohttpForumSmileys[IOHTTP_FORUM_SMILEBASE+c].string );
  string = string2;
  continue;
  iohttpForumFilter3L0:
  dest[b++] = *string;
  string++;
 }
 dest[b] = 0;
 return b;
}

/*
perms = 0xFFF
Accessible for all

perms = 2
Empire forum

perms = 0
Accessible to user level >= mod

*/


int iohttpForumPerms( int id, int forum, svConnectionPtr cnt, dbUserMainPtr maind, int perms )
{
	if( perms >= 4 )
	{
  return 1;
 }
 else if((forum < 100) && (cnt->dbuser) )
 {
 	if(id == -1)
			return 0;
		if(((cnt->dbuser)->level >= LEVEL_MODERATOR)||((cnt->dbuser)->level >= LEVEL_FORUMMOD))
 		return 1;
 	else
 		return 0;
 }
 else if( perms >= 2 )
 {
 	if(id == -1)
			return 0;
 	if(( id == -1 ) || (maind == 0))
   return 0;
  if( (cnt->dbuser)->level >= LEVEL_MODERATOR )
  {
  	return 1;
  }
  
  if( (cnt->dbuser)->flags & CMD_USER_FLAGS_INDEPENDENT )
   return 0;
  if( maind->empire+100 == forum )
   return 1;
  if( cmdExecFindRelation( maind->empire, forum-100, 0, 0 ) == CMD_RELATION_ALLY )
   return 1;
  return 0;
 }
 else if( perms >= 1 )
 {
 	if( id == -1 )
   return 0;
 }
 else
 {
 	if( id == -1 )
   return 0;
  if( ( forum >= 100 ) && ( maind->empire == forum-100 ) && ( (cnt->dbuser)->flags & ( CMD_USER_FLAGS_LEADER | CMD_USER_FLAGS_VICELEADER ) ) )
   return 1;
  if( (cnt->dbuser)->level < LEVEL_MODERATOR )
   return 0;
 }
 return 1;
}






int iohttpForumCleanAuthor( unsigned char *string )
{
 int a, b, fnum, tnum, count = 0;
 unsigned char word[512];
 dbForumForumPtr forums;
 dbForumForumDef forumd;
 dbForumThreadPtr threads;
 fnum = dbForumListForums( 0, &forums );
 if( fnum < 0 ) return -1;
 iohttpForumFilter( word, string, 512, 0 );
 for( a = 0 ; a < fnum ; a++ )
 {
  tnum = dbForumListThreads( a, 0, 0xFFFF, &forumd, &threads );
  if( tnum < 0 )
   continue;
  for( b = tnum-1 ; b >= 0 ; b-- )
  {
   if( !( threads[b].authorname[0] ) && !( threads[b].topic[0] ) )
   {
    dbForumRemoveThread( a, threads[b].id );
    count++;
   }
   else if( ioCompareFindWords( threads[b].authorname, word ) )
   {
    dbForumRemoveThread( a, threads[b].id );
    count++;
   }
  }
 }
 return count;
}


int iohttpForumCleanIP( unsigned char *ipstring )
{
 int a, b, fnum, tnum, count = 0;
 dbForumForumPtr forums;
 dbForumForumDef forumd;
 dbForumThreadPtr threads;
 fnum = dbForumListForums( 0, &forums );
 if( fnum < 0 )
  return -1;
 for( a = 0 ; a < fnum ; a++ )
 {
  tnum = dbForumListThreads( a, 0, 0xFFFF, &forumd, &threads );
  if( tnum < 0 )
   continue;
  for( b = tnum-1 ; b >= 0 ; b-- )
  {
   if( ioCompareExact( inet_ntoa( threads[b].sin_addr ), ipstring ) )
   {
    dbForumRemoveThread( a, threads[b].id );
    count++;
   }
  }
 }
 return count;
}


/*This function will be use to approve file uploaded by player to our image server*/
void iohttpFunc_Approve (svConnectionPtr cnt)
{
	int nAllow = 0; // == 1 if this user have the right of allowing image
	int id, nAction;  //Action == 1 approve 2 == Disapprove (file deleted, msg send to id)
	int a;						//Because Mal always use a and not i
	unsigned char *szAction;
	unsigned char *szID;
	char szName[50];
	char szPass[50];
	char szFileName[50];
	char szTemp[50];
	FILE *fDesc;
	
	if((( id = iohttpIdentify( cnt, 0 ) ) >= 0 ) && (cnt != NULL))
 {
 	if( (cnt->dbuser)->level >= LEVEL_MODERATOR )
 		nAllow ++;
 }
 
 iohttpVarsInit( cnt );
 //Look for variable here
 
 szAction = iohttpVarsFind( "action" );
 szID = iohttpVarsFind( "id" );
 
 iohttpVarsCut();
 iohttpBase( cnt, 1 ); //Init of the html code for the player
 iohttpBodyInit( cnt, "Approving" );
 
 if(nAllow)
 {
 	//Read all the input given by Post then show link to the file to download it before approving it.
 	if((szAction)&&(szID))
 	{
 		nAction = atoi(szAction);
 		//Open the desc file of this file ID
 		sprintf(szFileName, "pending/%s.txt", szID);
 	 fDesc = fopen(szFileName, "r");
 		if(fDesc)
 		{
 			fscanf(fDesc, "%s", szName);
 			fscanf(fDesc, "%s", szPass);
 			fscanf(fDesc, "%s", szFileName);
 			
 			//In both case we must check if the user exist and what is his id
	 		nAllow = -1;
	 		//this if is for formating the user and pass put ' ' instead of +
	 		if( ( szName ) && ( szPass ) )
			 {
			  for( a = 0 ; szName[a] ; a++ )
			  {
			   if( szName[a] == '+' )
			    szName[a] = ' ';
			   else if( ( szName[a] == 10 ) || ( szName[a] == 13 ) )
			    szName[a] = 0;
			  }
			  for( a = 0 ; szPass[a] ; a++ )
			  {
			   if( szPass[a] == '+' )
			    szPass[a] = ' ';
			   else if( ( szPass[a] == 10 ) || ( szPass[a] == 13 ) )
			    szPass[a] = 0;
			  }
			  
			  if( ( id = dbUserSearch( szName ) ) >= 0 )
			  {
				  if( dbUserRetrievePassword( id, szTemp ) >= 0 )
				  {
				  	if( ioCompareExact( szPass, szTemp) ) 
				   	nAllow = id;
				   }
				 }
				 //nAllow will be equal to the id of the user if the good password was supplied
				 
	 		}
	 		
	 		if(nAction == 1)			//Approve
	 		{
	 			svSendPrintf(cnt, "The file %s is approved<br>", szID);
	 		}
	 		else if(nAction == 2)	//Disapprove
	 		{
	 			svSendPrintf(cnt, "This file %s is deleted and a message was sent to the user<br>", szID);
	 		}
				fclose(fDesc);
			}
 	}
 	
 	//List all the file "pending" here
 	
 }
 else
 {
 	svSendString(cnt, "Sorry you don't have the necessary right to approve file upload by all the players!");
 }
 iohttpBodyEnd( cnt );
 return;
}


void iohttpForum( svConnectionPtr cnt )
{
 int a, b, c, d, id, forum, thread, post, action, skip;
 dbUserMainDef maind;
 unsigned char *forumstring, *threadstring, *topicstring, *poststring, *delthreadstring, *delpoststring, *editpoststring, *namestring, *skipstring;
 dbForumForumPtr forums;
 dbForumThreadPtr threads;
 dbForumPostPtr posts;
 dbForumForumDef forumd;
 dbForumThreadDef threadd;
 dbForumPostDef postd;
 unsigned char timebuf[256];
 char timetemp[200];
 unsigned char *text;
	FILE *fFile;
	
 id = iohttpIdentify( cnt, 0 );
	if( !( id == -1 ) )
	{
		if( dbUserMainRetrieve( id, &maind ) == 0 )
			return;
	}
	
 iohttpVarsInit( cnt );
 forumstring = iohttpVarsFind( "forum" );
 threadstring = iohttpVarsFind( "thread" );
 topicstring = iohttpVarsFind( "topic" );
 poststring = iohttpVarsFind( "post" );
 delthreadstring = iohttpVarsFind( "delthread" );
 delpoststring = iohttpVarsFind( "delpost" );
 editpoststring = iohttpVarsFind( "editpost" );
 namestring = iohttpVarsFind( "name" );
 skipstring = iohttpVarsFind( "skip" );
 iohttpVarsCut();

 action = 0;
 if( ( forumstring ) && ( sscanf( forumstring, "%d", &forum ) == 1 ) )
 {
  action = 1;
  if( ( threadstring ) && ( sscanf( threadstring, "%d", &thread ) == 1 ) )
  {
   if( delthreadstring )
    action = 5;
   else if( ( delpoststring ) && ( sscanf( delpoststring, "%d", &post ) == 1 ) )
    action = 6;
   else if( poststring )
   {
    if( ( editpoststring ) && ( sscanf( editpoststring, "%d", &post ) == 1 ) )
     action = 8;
    else
     action = 4;
   }
   else if( ( editpoststring ) && ( sscanf( editpoststring, "%d", &post ) == 1 ) )
    action = 7;
   else
    action = 2;
  }
  else if( ( topicstring ) && ( poststring ) )
  {
  	a = strlen(topicstring);
   if(a)
   	action = 3;
  }
 }

 skip = 0;
 if( skipstring )
  sscanf( skipstring, "%d", &skip );

 svSendString( cnt, "Content-Type: text/html\n\n" );
//svSendString( cnt, "<html><head><style type=\"text/css\">body,td{font-size:smaller;font-family:verdana,geneva,arial,helvetica,sans-serif;}a:hover{color:#00aaaa}</style></head><body bgcolor=\"#000000\" text=\"#C0D0D8\" link=\"#FFFFFF\" alink=\"#FFFFFF\" vlink=\"#B8B8B8\" background=\"http://users.pandora.be/amedee/ectroverse/mbg.gif\" bgproperties=\"fixed\"><center>" );
//svSendString( cnt, "<html><head><style type=\"text/css\">body,td{font-size:smaller;font-family:verdana,geneva,arial,helvetica,sans-serif;}a:hover{color:#00aaaa}</style></head><body bgcolor=\"#000000\" text=\"#D0E0E0\" link=\"#D0F0FF\" alink=\"#FFFFFF\" vlink=\"#C0D0E0\" background=\"http://users.pandora.be/amedee/ectroverse/mbg.gif\" bgproperties=\"fixed\"><center>" );
 svSendString( cnt, "<html><head><style type=\"text/css\">body,td{font-size:smaller;font-family:verdana,geneva,arial,helvetica,sans-serif;}a:hover{color:#00aaaa}</style></head><body bgcolor=\"#000000\" text=\"#F0F0F0\" link=\"#C8E8FF\" alink=\"#FFFFFF\" vlink=\"#D0D0D0\" background=\"mbg.gif\" bgproperties=\"fixed\"><center>" );

 if( action == 0 )
 {
  b = dbForumListForums( 0, &forums );
  if( b < 0 )
  {
   svSendString( cnt, "Error while retrieving list of forums</center></body></html>" );
   return;
  }
  a = time( 0 )-(3600*SERVER_TIME_ZONE);
  strftime( timebuf, 256, "%T, %b %d", localtime( (time_t *)&a ) );
  svSendPrintf( cnt, "<table cellspacing=\"4\" width=\"80%%\"><tr><td><a href=\"/\" target=\"_top\">Ectroverse</a> - Ectroverse public forums</td><td align=\"right\">%s", timebuf );
  if( ( id != -1 ) && ( forum != maind.empire + 100 ) && ( maind.empire != -1 ) )
   svSendPrintf( cnt, " - <a href=\"forum?forum=%d\">Empire forum</a>", maind.empire + 100 );
  svSendString( cnt, "</td></tr></table>" );

  svSendString( cnt, "<table width=\"80%\" cellpadding=\"3\" cellspacing=\"3\" bgcolor=\"#000000\"><tr bgcolor=\"#333333\"><td width=\"70%\">Forums</td><td width=\"10%\">Threads</td><td width=\"20%\">Last post</td></tr>" );
  for( a = 0 ; a < b ; a++ )
  {
   strftime( timebuf, 256, "%T, %b %d %Y", localtime( (time_t *)&(forums[a].time) ) );
   svSendPrintf( cnt, "<tr bgcolor=\"#111111\"><td><a href=\"forum?forum=%d&last=%d\">%s</a></td><td>%d</td><td nowrap>%s<br>Week %d, %d</td></tr>", a, forums[a].time, forums[a].title, forums[a].threads, timebuf, forums[a].tick % 52, forums[a].tick / 52 );
  }
  svSendString( cnt, "</table>" );

  free( forums );
 }
 else if( action == 1 )
 {
  iohttpForumL2:
	
  b = dbForumListThreads( forum, skip, skip+IOHTTP_FORUM_THREADSNUM, &forumd, &threads );
  if( b < 0 )
  {
   svSendString( cnt, "These threads do not exist!</center></body></html>" );
   return;
  }
  if( !( iohttpForumPerms( id, forum, cnt, &maind, forumd.rperms ) ) )
  {
   if( threads )
    free( threads );
   svSendString( cnt, "You are not authorized to view this forum</center></body></html>" );
   return;
  }
  if(cnt->dbuser)
  {
	  if(cnt->dbuser->level >= LEVEL_MODERATOR)
	  {
	  	if(forum>100)
	  	{
	  		fFile = fopen( LOGS_DIRECTORY "/modlog.txt", "a+t" );
		  	if( fFile )
		 		{
		 			fprintf( fFile, "%s > view forum of empire %d\n", maind.faction, forum-100);
		 			fclose(fFile);
		 		}
		 	}
	  	}
	 }
		svSendPrintf( cnt, "<table cellspacing=\"4\" width=\"80%%\"><tr><td><a href=\"/\" target=\"_top\">Ectroverse</a> - <a href=\"forum\">Ectroverse public forums</a> - %s</td><td align=\"right\">", forumd.title );
  if( forum < 100 )
  {
   a = time( 0 )-(3600*SERVER_TIME_ZONE);
   strftime( timebuf, 256, "%T, %b %d", localtime( (time_t *)&a ) );
   svSendString( cnt, timebuf );
   if( ( id != -1 ) && ( forum != maind.empire + 100 ) && ( maind.empire != -1 ) )
    svSendPrintf( cnt, " - <a href=\"forum?forum=%d\">Empire forum</a>", maind.empire + 100 );
   svSendString( cnt, "</td></tr></table>" );
  }
  else
   svSendPrintf( cnt, "Week %d, Year %d</td></tr></table>", svTickNum % 52, svTickNum / 52 );

  if( forumd.threads > IOHTTP_FORUM_THREADSNUM )
  {
   svSendString( cnt, "Pages : " );
   for( a = 0, c = 1 ; a < forumd.threads ; a += IOHTTP_FORUM_THREADSNUM, c++ )
   {
    if( skip != a )
     svSendPrintf( cnt, "<a href=\"forum?forum=%d&skip=%d\">%d</a> ", forum, a, c );
    else
     svSendPrintf( cnt, "<b>%d</b> ", c );
   }
  }

  svSendString( cnt, "<table width=\"80%\" cellpadding=\"3\" cellspacing=\"3\" bgcolor=\"#000000\"><tr bgcolor=\"#333333\"><td width=\"60%\">Topic</td><td width=\"10%\">Posts</td><td width=\"15%\">Author</td><td width=\"15%\">Last post</td></tr>" );
  for( a = 0 ; a < b ; a++ )
  {
  	strftime( timebuf, 256, "%T, %b %d %Y", localtime( (time_t *)&(threads[a].time) ) );
   sprintf(timetemp, "<br>Week %d, Year %d", threads[a].tick % 52, threads[a].tick / 52 );
   strcat( timebuf, timetemp);
   
   svSendPrintf( cnt, "<tr bgcolor=\"#111111\"><td><a href=\"forum?forum=%d&thread=%d&last=%d\">%s</a>", forum, threads[a].id, threads[a].time, threads[a].topic );
   if( threads[a].posts > IOHTTP_FORUM_POSTSNUM )
   {
    svSendString( cnt, " - Pages : " );
    for( d = 0, c = 1 ; d < threads[a].posts ; d += IOHTTP_FORUM_POSTSNUM, c++ )
     svSendPrintf( cnt, "<a href=\"forum?forum=%d&thread=%d&skip=%d&last=%d\">%d</a> ", forum, threads[a].id, d, threads[a].time, c );
   }
   svSendPrintf( cnt, "</td><td>%d</td><td nowrap>%s</td><td nowrap>%s", threads[a].posts, threads[a].authorname, timebuf );
			if(( iohttpForumPerms( id, forum, cnt, &maind, 0 ) )&&(id != -1))
   {
   	svSendPrintf( cnt, " <a href=\"forum?forum=%d&thread=%d&delthread=1\">Delete</a>", forum, threads[a].id );
  	}
  	if(cnt->dbuser)
  	{
	   if(( id != -1 ) && ((((cnt->dbuser)->level >= LEVEL_FORUMMOD))||((cnt->dbuser)->level >= LEVEL_FORUMMOD)))
	    svSendPrintf( cnt, "<br>IP: %s", inet_ntoa( threads[a].sin_addr ) );
	  }
			svSendString( cnt, "</td></tr>" );
  }
  svSendString( cnt, "</table>" );
  if( forumd.threads > IOHTTP_FORUM_THREADSNUM )
  {
   svSendString( cnt, "Pages : " );
   for( a = 0, c = 1 ; a < forumd.threads ; a += IOHTTP_FORUM_THREADSNUM, c++ )
   {
    if( skip != a )
     svSendPrintf( cnt, "<a href=\"forum?forum=%d&skip=%d\">%d</a> ", forum, a, c );
    else
     svSendPrintf( cnt, "<b>%d</b> ", c );
   }
  }

  if( iohttpForumPerms( id, forum, cnt, &maind, forumd.wperms ) )
  {
   svSendPrintf( cnt, "<form action=\"forum\" method=\"POST\"><input type=\"hidden\" name=\"forum\" value=\"%d\"><table cellspacing=\"3\"><tr><td>Name</td><td>", forum );
   if( id == -1 )
//    svSendPrintf( cnt, "<input type=\"text\" name=\"name\" size=\"32\">" );
   return; 
   else
    svSendPrintf( cnt, "%s<input type=\"hidden\" name=\"name\" value=\"%s\">", (cnt->dbuser)->faction, (cnt->dbuser)->faction );
   svSendString( cnt, "</td></tr><tr><td>Topic</td><td><input type=\"text\" name=\"topic\" size=\"32\"></td></tr>" );
   svSendString( cnt, "<tr><td>Post</td><td><textarea name=\"post\" wrap=\"soft\" rows=\"10\" cols=\"60\"></textarea></td></tr><tr><td>&nbsp;</td><td><input type=\"submit\" value=\"Post\"></td></tr></table></form>" );
  }

  if( threads )
   free( threads );
 }
 else if( action == 2 )
 {
  iohttpForumL1:
  if( dbForumRetrieveForum( forum, &forumd ) < 0 )
  {
   svSendString( cnt, "This forum does not exist!</center></body></html>" );
   return;
  }
  b = dbForumListPosts( forum, thread, skip, skip+IOHTTP_FORUM_POSTSNUM, &threadd, &posts );
  if( b < 0 )
  {
   svSendString( cnt, "Error while retrieving list of posts</center></body></html>" );
   return;
  }

  if( !( iohttpForumPerms( id, forum, cnt, &maind, forumd.rperms ) ) )
  {
   for( a = 0 ; a < b ; a++ )
   {
    if( posts[a].text )
     free( posts[a].text );
   }
   if( posts )
    free( posts );
   svSendString( cnt, "You are not authorized to view this forum</center></body></html>" );
   return;
  }

  svSendPrintf( cnt, "<table cellspacing=\"4\" width=\"80%%\"><tr><td><a href=\"/\" target=\"_top\">Ectroverse</a> - <a href=\"forum\">Ectroverse public forums</a> - <a href=\"forum?forum=%d\">%s</a> - %s</td><td align=\"right\">", forum, forumd.title, threadd.topic );
  if( forum < 100 )
  {
   a = time( 0 )-(3600*SERVER_TIME_ZONE);
   strftime( timebuf, 256, "%T, %b %d", localtime( (time_t *)&a ) );
   svSendString( cnt, timebuf );
   if( ( id != -1 ) && ( forum != maind.empire + 100 ) && ( maind.empire != -1 ) )
    svSendPrintf( cnt, " - <a href=\"forum?forum=%d\">Empire forum</a>", maind.empire + 100 );
   svSendString( cnt, "</td></tr></table>" );
  }
  else
   svSendPrintf( cnt, "Week %d, Year %d</td></tr></table>", svTickNum % 52, svTickNum / 52 );

  if( threadd.posts > IOHTTP_FORUM_POSTSNUM )
  {
   svSendString( cnt, "Pages : " );
   for( a = 0, c = 1 ; a < threadd.posts ; a += IOHTTP_FORUM_POSTSNUM, c++ )
   {
    if( skip != a )
     svSendPrintf( cnt, "<a href=\"forum?forum=%d&thread=%d&skip=%d\">%d</a> ", forum, thread, a, c );
    else
     svSendPrintf( cnt, "<b>%d</b> ", c );
   }
  }

  svSendString( cnt, "<table width=\"80%\" cellpadding=\"3\" cellspacing=\"3\" bgcolor=\"#000000\">" );
  for( a = 0 ; a < b ; a++ )
  {
   if( !( posts[a].text ) )
    continue;
   c = a + skip;
   if( forum < 100 )
    strftime( timebuf, 256, "%T, %b %d %Y", localtime( (time_t *)&(posts[a].post.time) ) );
   else
    sprintf( timebuf, "Week %d, Year %d", (posts[a].post).tick % 52, (posts[a].post).tick / 52 );
   svSendPrintf( cnt, "<tr><td valign=\"top\" width=\"10%%\" nowrap bgcolor=\"#282828\"><b>%s</b><br><i>%s</i><br>%s", posts[a].post.authorname, posts[a].post.authortag, timebuf );

   if( iohttpForumPerms( id, forum, cnt, &maind, 0 ) || ( ( id != -1 ) && ( posts[a].post.authorid == id ) && ioCompareExact( posts[a].post.authorname, (cnt->dbuser)->faction ) ))
    svSendPrintf( cnt, "<br><a href=\"forum?forum=%d&thread=%d&editpost=%d\">Edit</a> - <a href=\"forum?forum=%d&thread=%d&delpost=%d\">Delete</a>", forum, thread, c, forum, thread, c );
   if(cnt->dbuser)
   {
	   if( ( id != -1 ) && ( (cnt->dbuser)->level >= LEVEL_MODERATOR ) )
	   {
	    svSendPrintf( cnt, "<br>Account: <a href=\"player?id=%d\">%d</a>", posts[a].post.authorid, posts[a].post.authorid );
	    svSendPrintf( cnt, "<br>IP: %s", inet_ntoa( posts[a].post.sin_addr ) );
	   }
	  }
   svSendString( cnt, "</td><td valign=\"top\" width=\"90%\" bgcolor=\"#181818\">" );
   svSendString( cnt, posts[a].text );
   svSendString( cnt, "</td></tr>" );
   if( posts[a].text )
    free( posts[a].text );
  }
  svSendString( cnt, "</table>" );

  if( threadd.posts > IOHTTP_FORUM_POSTSNUM )
  {
   svSendString( cnt, "Pages : " );
   for( a = 0, c = 1 ; a < threadd.posts ; a += IOHTTP_FORUM_POSTSNUM, c++ )
   {
    if( skip != a )
     svSendPrintf( cnt, "<a href=\"forum?forum=%d&thread=%d&skip=%d\">%d</a> ", forum, thread, a, c );
    else
     svSendPrintf( cnt, "<b>%d</b> ", c );
   }
  }

  if( iohttpForumPerms( id, forum, cnt, &maind, forumd.wperms ) )
  {
   svSendPrintf( cnt, "<form action=\"forum\" method=\"POST\"><input type=\"hidden\" name=\"forum\" value=\"%d\"><input type=\"hidden\" name=\"thread\" value=\"%d\"><table cellspacing=\"3\"><tr><td>Name</td><td>", forum, thread );
   if( id == -1 )
   return;
  //  svSendPrintf( cnt, "<input type=\"text\" name=\"name\" size=\"32\">" );
   else
    svSendPrintf( cnt, "%s<input type=\"hidden\" name=\"name\" value=\"%s\">", (cnt->dbuser)->faction, (cnt->dbuser)->faction );
   svSendString( cnt, "</td></tr><tr><td>Post</td><td><textarea name=\"post\" wrap=\"soft\" rows=\"10\" cols=\"60\"></textarea></td></tr><tr><td>&nbsp;</td><td><input type=\"submit\" value=\"Post\"></td></tr></table></form>" );
  }

  if( posts )
   free( posts );
 }
 else if( action == 3 )
 {
  if( dbForumRetrieveForum( forum, &forumd ) < 0 )
  {
   svSendString( cnt, "This forum does not exist!</center></body></html>" );
   return;
  }
  if( !( iohttpForumPerms( id, forum, cnt, &maind, forumd.wperms ) ) )
  {
   svSendString( cnt, "You are not authorized to post in this forum</center></body></html>" );
   return;
  }
  iohttpForumFilter( threadd.topic, topicstring, DB_FORUM_NAME_SIZE, 0 );
  threadd.posts = 0;
  threadd.authorid = id;
  if( id != -1 )
   sprintf( threadd.authorname, (cnt->dbuser)->faction );
  else if( namestring )
   iohttpForumFilter( threadd.authorname, namestring, 32, 0 );
  else
   sprintf( threadd.authorname, "Anonymous" );
  threadd.time = time( 0 )-(3600*SERVER_TIME_ZONE);
  threadd.tick = svTickNum;
  threadd.flags = 0;
  memcpy( &(threadd.sin_addr), &(cnt->sockaddr.sin_addr), sizeof(struct in_addr) );
  thread = dbForumAddThread( forum, &threadd );
  goto iohttpForumL0;
 }
 else if( action == 4 )
 {
  if( dbForumRetrieveForum( forum, &forumd ) < 0 )
  {
   svSendString( cnt, "This forum does not exist!</center></body></html>" );
   return;
  }
  if( !( iohttpForumPerms( id, forum, cnt, &maind, forumd.wperms ) ) )
  {
   svSendString( cnt, "You are not authorized to post in this forum</center></body></html>" );
   return;
  }
		
	/*	b = dbForumListThreads( forum, thread, thread+1, &forumd, &threads );
  
  if(b)
  {
  	if(threads[thread].flags == DB_FORUM_FLAGS_THREAD_LOCK)
  	{
  		svSendString( cnt, "Sorry this topic is <b>lock</b> and nothing can happen here" );
   	return;
  	}
  }*/
		
  iohttpForumL0:

  if( !( postd.text = malloc( 3 * IOHTTP_FORUM_BUFFER ) ) )
  {
   svSendString( cnt, "</center></body></html>" );
   return;
  }
  a = 0;
  if(cnt->dbuser)
  {
	  if( ( id != -1 ) && ( (cnt->dbuser)->level >= LEVEL_MODERATOR ) )
	   a = 1;
	 }
  iohttpForumFilter( &postd.text[2*IOHTTP_FORUM_BUFFER], poststring, IOHTTP_FORUM_BUFFER, a );
  postd.post.length = iohttpForumFilter2( postd.text, &postd.text[2*IOHTTP_FORUM_BUFFER], IOHTTP_FORUM_BUFFER );
  postd.post.authorid = id;
  if( id != -1 )
  {
   sprintf( postd.post.authorname, (cnt->dbuser)->faction );
   sprintf( postd.post.authortag, (cnt->dbuser)->forumtag );
  }
  else
  {
   if( namestring )
    iohttpForumFilter( postd.post.authorname, namestring, 32, 0 );
   else
    sprintf( postd.post.authorname, "Anonymous" );
   postd.post.authortag[0] = 0;
  }
  postd.post.time = time( 0 )-(3600*SERVER_TIME_ZONE);  //to be in GMT with the server running anywhere worldwide
  postd.post.tick = svTickNum;
  postd.post.flags = 0;
  memcpy( &(postd.post.sin_addr), &(cnt->sockaddr.sin_addr), sizeof(struct in_addr) );
  a = dbForumAddPost( forum, thread, &postd );
  if( a >= 0 )
   svSendPrintf( cnt, "Post added!<br><br>" );
  else
   svSendPrintf( cnt, "Error while adding post<br><br>" );
  free( postd.text );
  goto iohttpForumL1;
 }
 else if( action == 5 )
 {
  if(( !( iohttpForumPerms( id, forum, cnt, &maind, 0 ) ) ) || (id == -1))
  {
   svSendString( cnt, "You are not authorized to delete threads here</center></body></html>" );
   return;
  }
  iohttpForumL3:

  if( dbForumRemoveThread( forum, thread ) >= 0 )
   svSendPrintf( cnt, "Thread removed<br><br>" );
  else
   svSendPrintf( cnt, "Error while removing thread<br><br>" );
  goto iohttpForumL2;
 }
 else if( action == 6 )
 {
  b = dbForumListPosts( forum, thread, post, post+1, &threadd, &posts );
  if( b < 0 )
  {
   svSendString( cnt, "This post doesn't exist</center></body></html>" );
   return;
  }
  if( !( iohttpForumPerms( id, forum, cnt, &maind, 0 ) ) && ( ( id == -1 ) || ( posts[0].post.authorid != id ) || !( ioCompareExact( posts[0].post.authorname, (cnt->dbuser)->faction ) ) ) )
  {
   svSendString( cnt, "You are not authorized to delete this post</center></body></html>" );
   if( posts[0].text )
    free( posts[0].text );
   free( posts );
   return;
  }
  if( posts[0].text )
   free( posts[0].text );
  free( posts );
  a = dbForumRemovePost( forum, thread, post );
  if( a >= 0 )
  {
   if( !( a ) )
    goto iohttpForumL3;
   svSendPrintf( cnt, "Post removed<br><br>" );
  }
  else
   svSendPrintf( cnt, "Error while removing post<br><br>" );
  goto iohttpForumL1;
 }
 else if( action == 7 )
 {
  b = dbForumListPosts( forum, thread, post, post+1, &threadd, &posts );
  if( b < 0 )
  {
   svSendString( cnt, "This post doesn't exist</center></body></html>" );
   return;
  }
  if( !( iohttpForumPerms( id, forum, cnt, &maind, 0 ) ) && ( ( id == -1 ) || ( posts[0].post.authorid != id ) || !( ioCompareExact( posts[0].post.authorname, (cnt->dbuser)->faction ) ) ) )
  {
   svSendString( cnt, "You are not authorized to edit this post</center></body></html>" );
   if( posts[0].text )
    free( posts[0].text );
   free( posts );
   return;
  }

  svSendPrintf( cnt, "<form action=\"forum\" method=\"POST\"><input type=\"hidden\" name=\"forum\" value=\"%d\"><input type=\"hidden\" name=\"thread\" value=\"%d\"><input type=\"hidden\" name=\"editpost\" value=\"%d\">", forum, thread, post );
  svSendString( cnt, "Edit post<br><br><textarea name=\"post\" wrap=\"soft\" rows=\"10\" cols=\"60\">" );

  if( !( text = malloc( 2 * IOHTTP_FORUM_BUFFER ) ) )
  {
   if( posts[0].text )
    free( posts[0].text );
   free( posts );
   svSendString( cnt, "</center></body></html>" );
   return;
  }
  iohttpForumFilter3( text, posts[0].text, 2*IOHTTP_FORUM_BUFFER - 512 );
  svSendString( cnt, text );

  if( posts[0].text )
   free( posts[0].text );
  free( posts );
  free( text );
  svSendString( cnt, "</textarea><br><br><input type=\"submit\" value=\"Post\"></form>" );
 }
 else if( action == 8 )
 {
  b = dbForumListPosts( forum, thread, post, post+1, &threadd, &posts );
  if( b < 0 )
  {
   svSendString( cnt, "This post doesn't exist</center></body></html>" );
   return;
  }
  if( !( iohttpForumPerms( id, forum, cnt, &maind, 0 ) ) && ( ( id == -1 ) || ( posts[0].post.authorid != id ) || !( ioCompareExact( posts[0].post.authorname, (cnt->dbuser)->faction ) ) ) )
  {
   svSendString( cnt, "You are not authorized to edit this post</center></body></html>" );
   if( posts[0].text )
    free( posts[0].text );
   free( posts );
   return;
  }
  sprintf( postd.post.authorname, posts[0].post.authorname );
  sprintf( postd.post.authortag, posts[0].post.authortag );
  postd.post.time = posts[0].post.time;
  postd.post.tick = posts[0].post.tick;
  postd.post.authorid = posts[0].post.authorid;
  postd.post.flags = posts[0].post.flags;
  if( posts[0].text )
   free( posts[0].text );
  free( posts );

  if( !( postd.text = malloc( 3 * IOHTTP_FORUM_BUFFER ) ) )
  {
   svSendString( cnt, "</center></body></html>" );
   return;
  }
  a = 0;
  if(cnt->dbuser)
  {
	  if( ( id != -1 ) && ( (cnt->dbuser)->level >= LEVEL_MODERATOR ) )
	 	 a = 1;
	 }
  iohttpForumFilter( &postd.text[2*IOHTTP_FORUM_BUFFER], poststring, IOHTTP_FORUM_BUFFER, a );
  postd.post.length = iohttpForumFilter2( postd.text, &postd.text[2*IOHTTP_FORUM_BUFFER], 2*IOHTTP_FORUM_BUFFER - 512 );
  a = time( 0 )-(3600*SERVER_TIME_ZONE);
  strftime( timebuf, 256, "%T, %b %d %Y", localtime( (time_t *)&a ) );
  postd.post.length += sprintf( &postd.text[postd.post.length], "<br><br><font size=\"1\"><i>Edited by %s on Week %d, Year %d - %s</i></font>", maind.faction, svTickNum % 52, svTickNum / 52, timebuf );

  a = dbForumEditPost( forum, thread, post, &postd );
  if( a >= 0 )
   svSendPrintf( cnt, "Post edited!<br><br>" );
  else
   svSendPrintf( cnt, "Error while editing post<br><br>" );
  free( postd.text );
  goto iohttpForumL1;
 }

 svSendString( cnt, "</center></body></html>" );
 return;
}
