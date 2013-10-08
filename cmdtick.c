int cmdTickProduction[CMD_BLDG_NUMUSED];



void cmdTickGenRanks()
{
  int a, b, c, d, first, num, artmax;
  FILE *file, *filep;
  dbUserMainDef maind;
  dbMainEmpirePtr empirep;
  dbUserMainPtr mainp;
  dbUserPtr user;
  int *stats;
  int artefacts[ARTEFACT_NUMUSED], artsnum;


  artmax = 0;


  memset( artefacts, 0, ARTEFACT_NUMUSED*sizeof(int) );
  if( !( file = fopen( DB_DIRECTORY "/famranks.txt", "wb" ) ) )
    return;
  if( !( filep =fopen( DB_DIRECTORY "/famranksplain.txt", "wb" ) ) )
  {
    fclose( file );
    return;
  }
  if( !( stats = malloc( 6*dbMapBInfoStatic[4]*sizeof(int) + sizeof(dbMainEmpireDef)*dbMapBInfoStatic[4] ) ) )
  {
    fclose( file );
    return;
  }
  empirep = (void *)&stats[6*dbMapBInfoStatic[4]];
  memset( stats, 0, 6*dbMapBInfoStatic[4]*sizeof(int) );
  for( b = c = num = 0 ; b < dbMapBInfoStatic[4] ; b++ )
  {
    if( dbMapRetrieveEmpire( b, &empirep[b] ) < 0 )
      continue;
    if( !( empirep[b].numplayers ) )
      continue;

    stats[c+0] = b;
// calc NW, planets and empire artefacts
    for( a = 0 ; a < empirep[b].numplayers ; a++ )
    {
      if( dbUserMainRetrieve( empirep[b].player[a], &maind ) < 0 )
        continue;
      stats[c+1] += maind.planets;
      stats[c+2]++;
      stats[c+3] += (int)maind.networth;
    }
    empirep[b].planets = stats[c+1];
    empirep[b].networth = stats[c+3];
    dbMapSetEmpire( b, &empirep[b] );
    num++;

// set artefacts for empire members
    for( a = 0 ; a < empirep[b].numplayers ; a++ )
    {
      if( dbUserMainRetrieve( empirep[b].player[a], &maind ) < 0 )
        continue;
      maind.artefacts = empirep[b].artefacts;
      dbUserMainSet( empirep[b].player[a], &maind );
    }

    c += 6;
  }

// empire rankings
  first = 0;
  stats[4] = -1;
  stats[5] = -1;
  for( b = 1, c = 6 ; b < num ; b++, c += 6 )
  {
    if( ( stats[c+1] > stats[first+1] ) || ( ( stats[c+1] == stats[first+1] ) && ( stats[c+3] > stats[first+3] ) ) )
    {
      stats[first+4] = c;
      stats[c+5] = first;
      stats[c+4] = -1;
      first = c;
      continue;
    }
    a = stats[first+5];
    d = first;
    for( ; ; )
    {
      if( a == -1 )
      {
        stats[d+5] = c;
        stats[c+4] = d;
        stats[c+5] = -1;
        break;
      }
      if( ( stats[c+1] > stats[a+1] ) || ( ( stats[c+1] == stats[a+1] ) && ( stats[c+3] > stats[a+3] ) ) )
      {
        stats[c+4] = d;
        stats[c+5] = a;
        stats[d+5] = c;
        stats[a+4] = c;
        break;
      }
      d = a;
      a = stats[a+5];
    }
  }
  if( !( num ) )
    first = -1;
  fprintf( file, "<table cellspacing=\"4\"><tr><td align=\"right\">Rank</td><td>Name</td><td>Planets</td><td>Players</td><td>Networth</td></tr>" );
  for( a = first, b = 1 ; a != -1 ; b++ )
  {
    fprintf( file, "<tr><td align=\"right\" nowrap>" );



/*
    for( c = 0, d = 1 ; c < ARTEFACT_NUMUSED ; c++, d <<= 1 )
    {
      if( empirep[stats[a+0]].artefacts & d )
        fprintf( file, "<img src=\"%s\">", artefactImage[c] );
    }
*/
    artsnum = 0;
    for( c = 0, d = 1 ; c < ARTEFACT_NUMUSED ; c++, d <<= 1 )
    {
      if( empirep[stats[a+0]].artefacts & d )
      {
        artsnum++;
        artefacts[c] = 1;
      }
    }
    if( artsnum > artmax )
      artmax = artsnum;
    if( ( 3*artsnum >= ARTEFACT_NUMUSED ) || ( (3*dbArtefactMax)/2 >= ARTEFACT_NUMUSED ) )
    {
      for( c = 0, d = 1 ; c < ARTEFACT_NUMUSED ; c++, d <<= 1 )
      {
        if( empirep[stats[a+0]].artefacts & d )
          fprintf( file, "<img src=\"%s\">", artefactImage[c] );
      }
    }
			
	fprintf( file, " %d</td><td><a href=\"empire?id=%d\">", b, stats[a+0] );
  	if( empirep[stats[a+0]].name[0] )
      fprintf( file, "%s #%d", empirep[stats[a+0]].name, stats[a+0] );
    else
      fprintf( file, "Empire #%d", stats[a+0] );
    fprintf( file, "</a></td><td align=\"center\">%d</td><td align=\"center\">%d</td><td align=\"center\">%d</td></tr>\n", stats[a+1], stats[a+2], stats[a+3] );
		
    /* Plain Text */
    fprintf( filep, "%d:%d:%d:%d:%d:", b, stats[a+0], stats[a+1], stats[a+2], stats[a+3] );
    if( empirep[stats[a+0]].name[0] )
      fprintf( filep, "%s #%d", empirep[stats[a+0]].name, stats[a+0] );
    else
      fprintf( filep, "Empire #%d", stats[a+0] );
    fprintf( filep, "\n" );

    // moo
    empirep[stats[a+0]].rank = b;
    dbMapSetEmpire( stats[a+0], &empirep[stats[a+0]] );




    a = stats[a+5];
  }
  fprintf( file, "</table>" );

  fprintf( file, "<br>" );
  artsnum = 0;
  for( c = 0 ; c < ARTEFACT_NUMUSED ; c++ )
    artsnum |= artefacts[c];
  if( artsnum )
  {
    fprintf( file, "<br><br><b>Artefacts found</b><br>" );
    fprintf( file, "<table><tr><td>" );
    for( c = 0 ; c < ARTEFACT_NUMUSED ; c++ )
    {
      if( artefacts[c] )
        fprintf( file, "<img src=\"%s\"> %s<br>", artefactImage[c], artefactDescription[c] );
    }
    fprintf( file, "</td></tr></table><br>" );
  }
  dbArtefactMax = artmax;

  free( stats );
  fclose( file );
  fclose( filep );




// player rankings
  if( !( file = fopen( DB_DIRECTORY "/ranks.txt", "wb" ) ) )
    return;
  if( !( filep = fopen( DB_DIRECTORY "/ranksplain.txt", "wb" ) ) )
  {
    fclose( file );
    return;
  }
  for( user = dbUserList, num = 0 ; user ; user = user->next )
    num++;
  if( !( num ) || !( stats = malloc( 6*num*sizeof(int) + sizeof(dbUserMainDef)*num ) ) )
  {
    fclose( file );
    return;
  }
  mainp = (void *)&stats[6*num];
  memset( stats, 0, 6*num*sizeof(int) );

  for( b = c = 0, user = dbUserList ; user ; user = user->next )
  {
    if( !( user->flags & CMD_USER_FLAGS_ACTIVATED ) )
	{
	  //printf("user %d not activated\n", user->id );
      continue;
	}
    if( dbUserMainRetrieve( user->id, &mainp[b] ) < 0 )
	{
	  printf("error retreiving maind id: %d\n", user->id );
      continue;
	}
    stats[c+0] = user->id;
    stats[c+1] = mainp[b].planets;
    stats[c+2] = mainp[b].networth;
    stats[c+3] = b;
    b++;
    c += 6;
  }
  num = b;

  first = 0;
  stats[4] = -1;
  stats[5] = -1;
  for( b = 1, c = 6 ; b < num ; b++, c += 6 )
  {
    if( ( stats[c+1] > stats[first+1] ) || ( ( stats[c+1] == stats[first+1] ) && ( stats[c+2] > stats[first+2] ) ) )
    {
      stats[first+4] = c;
      stats[c+5] = first;
      stats[c+4] = -1;
      first = c;
      continue;
    }
    a = stats[first+5];
    d = first;
    for( ; ; )
    {
      if( a == -1 )
      {
        stats[d+5] = c;
        stats[c+4] = d;
        stats[c+5] = -1;
        break;
      }
      if( ( stats[c+1] > stats[a+1] ) || ( ( stats[c+1] == stats[a+1] ) && ( stats[c+2] > stats[a+2] ) ) )
      {
        stats[c+4] = d;
        stats[c+5] = a;
        stats[d+5] = c;
        stats[a+4] = c;
        break;
      }
      d = a;
      a = stats[a+5];
    }
  }
  if( !( num ) )
    first = -1;
  fprintf( file, "<table cellspacing=\"4\"><tr><td>Rank</td><td>Faction</td><td>Empire</td><td>Planets</td><td>Networth</td></tr>" );
  for( a = first, b = 1 ; a != -1 ; b++ )
  {
    fprintf( file, "<tr><td align=\"right\">%d</td><td><a href=\"player?id=%d\">%s</a></td><td><a href=\"empire?id=%d\">empire #%d</a></td><td align=\"center\">%d</td><td align=\"center\">%lld</td></tr>", b, stats[a+0], mainp[stats[a+3]].faction, mainp[stats[a+3]].empire, mainp[stats[a+3]].empire, mainp[stats[a+3]].planets, mainp[stats[a+3]].networth );

    fprintf( filep, "%d:%d:%d:%d:%lld:%s\n", b, stats[a+0], mainp[stats[a+3]].empire, mainp[stats[a+3]].planets, mainp[stats[a+3]].networth, mainp[stats[a+3]].faction );



    // moo
    mainp[stats[a+3]].rank = b;
    dbUserMainSet( stats[a+0], &mainp[stats[a+3]] );



    a = stats[a+5];
  }
  fprintf( file, "</table>" );

  free( stats );
  fclose( file );
  fclose( filep );



  return;
}





void cmdTickInit()
{
  return;
}


void cmdTickEnd()
{
  cmdTickGenRanks();
  return;
}




int cmdTickPlanets( int usrid, dbUserMainPtr mainp )
{
  int a, b, num, nump, population, nInfection;
  int *buffer;
  int *portals;
  dbMainPlanetDef planetd;
  dbMainEmpireDef empired;
  dbUserFleetPtr fleetd;
  dbUserSpecOpPtr specopd;

svDebugTickPass = 0 + 10000;


  memset( mainp->totalbuilding, 0, 16*sizeof(long long int) );
  memset( mainp->totalunit, 0, 16*sizeof(long long int) );
  memset( cmdTickProduction, 0, CMD_BLDG_NUMUSED*sizeof(int) );
  
  
  nInfection = 0;
  if( ( b = dbUserSpecOpList( usrid, &specopd ) ) >= 0 )
  {
  	for(a = 0; a < b; a++)
  	{
  			if (specopd[a].type == (CMD_OPER_BIOINFECTION|0x10000))
  			{
  				nInfection++;
  			}
  	}
  }
  
  if( ( num = dbUserPlanetListIndices( usrid, &buffer ) ) < 0 )
    return 0;
  portals = 0;
  nump = 0;
  if( ( nump = dbUserPortalsListCoords( usrid, &portals ) ) < 0 )
  {
    free( buffer );
    return 0;
  }

svDebugTickPass = 1 + 10000;

  population = 0;
  for( a = 0 ; a < num ; a++ )
  {
    dbMapRetrievePlanet( buffer[a], &planetd );
    
		planetd.maxpopulation = (float)( ( planetd.size * CMD_POPULATION_SIZE_FACTOR ) + ( planetd.building[CMD_BUILDING_CITIES] * CMD_POPULATION_CITIES ) );
		
		//ARTI CODE Super Stacker
	/*	if(mainp->artefacts & ARTEFACT_*_BIT)
			planetd.maxpopulation = (float)( ( planetd.size * CMD_POPULATION_SIZE_FACTOR ) + ( planetd.building[CMD_BUILDING_CITIES] * (CMD_POPULATION_CITIES+1000) ) );
	*/	
svDebugTickPass = 2 + 10000;
		
		//No more pop grow bonus it will count as upkeep reducer multiplier
		//Planet grow pop is 2% each tick
		planetd.population += ceil(planetd.maxpopulation * 0.02 * pow(0.75, (float)nInfection));
	

    if( planetd.population > planetd.maxpopulation )
      planetd.population = planetd.maxpopulation;


    if( ( planetd.flags & CMD_PLANET_FLAGS_PORTAL ) )
      planetd.protection = 100;
    else
    {
      planetd.protection = (int)( 100.0 * battlePortalCalc( ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, portals, nump, mainp->totalresearch[CMD_RESEARCH_PORTALS] ) );      
      
    }
    
  	if( planetd.construction < 0 )
    {
      printf( "Warning : negative construction count : %d\n", planetd.construction );
      planetd.construction = 0;
    }


svDebugTickPass = 3 + 10000;


    /* CRAP */
    for( b = 0 ; b < CMD_BLDG_NUMUSED ; b++ )
    {
      if( planetd.building[b] < 0 )
        planetd.building[b] = 0;
    }


    dbMapSetPlanet( buffer[a], &planetd );


svDebugTickPass = 4 + 10000;



    population += planetd.population;
    for( b = 0 ; b < CMD_BLDG_NUMUSED ; b++ )
    {
      mainp->totalbuilding[b] += planetd.building[b];
      cmdTickProduction[b] += planetd.building[b];
    }
    for( b = 0 ; b < CMD_UNIT_NUMUSED ; b++ )
      mainp->totalunit[b] += planetd.unit[b];
    if( ( planetd.flags & CMD_PLANET_FLAGS_PORTAL ) )
      mainp->totalbuilding[CMD_BLDG_NUMUSED]++;


svDebugTickPass = 5 + 10000;

		if( planetd.special[1] )
    {
      if( planetd.special[0] == 0 )
        cmdTickProduction[CMD_BUILDING_SOLAR] += ( planetd.special[1] * planetd.building[CMD_BUILDING_SOLAR] ) / 100;
      
      else if( planetd.special[0] == 1 )
        cmdTickProduction[CMD_BUILDING_MINING] += ( planetd.special[1] * planetd.building[CMD_BUILDING_MINING] ) / 100;

      else if( planetd.special[0] == 2 )
        cmdTickProduction[CMD_BUILDING_CRYSTAL] += ( planetd.special[1] * planetd.building[CMD_BUILDING_CRYSTAL] ) / 100;

      else if( planetd.special[0] == 3 )
        cmdTickProduction[CMD_BUILDING_REFINEMENT] += ( planetd.special[1] * planetd.building[CMD_BUILDING_REFINEMENT] ) / 100;

    }


svDebugTickPass = 6 + 10000;


    if( ( b = (int)artefactPrecense( &planetd ) ) < 0 )
      continue;
    if( dbMapRetrieveEmpire( mainp->empire, &empired ) < 0 )
      continue;
    empired.artefacts |= 1 << b;
    dbMapSetEmpire( mainp->empire, &empired );


svDebugTickPass = 7 + 10000;

  }
  mainp->planets = num;
  mainp->ressource[CMD_RESSOURCE_POPULATION] = (long long int)population;


svDebugTickPass = 8 + 10000;


  free( buffer );

svDebugTickPass = 9 + 10000;

  if( portals )
    free( portals );

svDebugTickPass = 10 + 10000;

  if( ( num = dbUserFleetList( usrid, &fleetd ) ) < 0 )
    return 0;

svDebugTickPass = 11 + 10000;

  for( a = 0 ; a < num ; a++ )
  {
    for( b = 0 ; b < 16 ; b++ )
      mainp->totalunit[b] += fleetd[a].unit[b];
  }

svDebugTickPass = 12 + 10000;

  free( fleetd );

svDebugTickPass = 13 + 10000;

  return 1;
}





int cmdTick()
{
  int a, c, d, e, num, specopnum, opvirus /*,cmd[3]*/, i;
  float fb, phdecay;
  double fa;
  long long int newd[DB_USER_NEWS_BASE], nIllusion, b;
  int nChicks = 0, penalty;
  int marketbid[DB_MARKETBID_NUMUSED];
  int bidresult[2];
  int *plist;
  int nArti = 0, nNum;
  dbUserPtr user;
  dbUserMainDef maind;
  dbUserBuildPtr build;
  dbMainPlanetDef planetd;
  dbUserFleetDef fleetd;
  dbUserFleetPtr fleetp;
  dbUserSpecOpPtr specopd;
  dbMainEmpireDef empired;


svDebugTickPass = 0;
svDebugTickId = 0;

	//Maybe useless but can t cause trouble only set the news buffer to 0
	memset(&newd, 0, sizeof(long long int)*DB_USER_NEWS_BASE);

  for( a = 0 ; a < dbMapBInfoStatic[4] ; a++ )
  {
    if( dbMapRetrieveEmpire( a, &empired ) < 0 )
      continue;

svDebugTickId = a;
		nArti |= empired.artefacts;	//Will have all discovered arti in here
		empired.artefacts = 0;
    dbMapSetEmpire( a, &empired );
  }


svDebugTickPass = 1;

	
  if( ( dbMapRetrieveMain( dbMapBInfoStatic ) < 0 ) )
    printf( "Tick error #1\n" );   
    
  for( user = dbUserList ; user ; user = user->next )
  {
  		
				
    if( !( user->flags & CMD_USER_FLAGS_ACTIVATED ) )
      continue;

svDebugTickId = user->id;

    if( dbUserMainRetrieve( user->id, &maind ) < 0 )
    {
      printf( "Tick error #2 : %d\n", user->id );
      continue;
    }
    
			
svDebugTickPass = 2;


    if( ( specopnum = dbUserSpecOpList( user->id, &specopd ) )  < 0 )
    {
      printf( "Tick error #3 : %d\n", user->id );
      continue;
    }
    opvirus = 0;
    
    //WAR  ILLUSION we recalcul each tick 
  	for(i=0;i<specopnum;i++)
  	{
  		if (specopd[i].type == (CMD_SPELL_WARILLUSIONS | 0x1000))
			{
				fa = 0.4 + (1.2/255.0) * (float)( rand() & 255 );
				nChicks = maind.totalunit[CMD_UNIT_WIZARD];
				nIllusion = ( fa * cmdRace[maind.raceid].unit[CMD_UNIT_WIZARD] * (float)nChicks * ( 1.0 + 0.005*maind.totalresearch[CMD_RESEARCH_WELFARE] ) / cmdPsychicopDifficulty[CMD_SPELL_WARILLUSIONS] );
		 		penalty = cmdGetOpPenalty( maind.totalresearch[CMD_RESEARCH_WELFARE], cmdPsychicopTech[CMD_SPELL_WARILLUSIONS] );
		 		if( penalty )
		    	nIllusion = (float)nIllusion / ( 1.0 + 0.01*(float)penalty );
				fa = 100.0 * (float)nIllusion / (float)maind.networth;
    		a = (int)( fa * 4.5 );		
    		a += a * rand()%20;
    		if (a<0)
    			a = 0;
    		specopd[i].vars[0] = a;
			}
  	}
  	
    for( a = specopnum-1 ; a >= 0 ; a-- )
    {
      if( specopd[a].type == ( CMD_OPER_NETWORKVIRUS | 0x10000 ) )
        opvirus++;
    }


svDebugTickPass = 3;


    num = dbUserBuildListReduceTime( user->id, &build );
    newd[0] = svTickNum;
    newd[1] = CMD_NEWS_FLAGS_NEW;
    for( a = num-1 ; a >= 0 ; a-- )
    {
      if( build[a].time > 0 )
        continue;
      if( !( build[a].type >> 16 ) )
      {
        dbMapRetrievePlanet( build[a].plnid, &planetd );
        if( build[a].type == CMD_BLDG_NUMUSED )
        {
          // portal
          planetd.flags &= 0xFFFFFFFF - CMD_PLANET_FLAGS_PORTAL_BUILD;
          planetd.flags |= CMD_PLANET_FLAGS_PORTAL;
          dbUserPlanetSetFlags( user->id, build[a].plnid, planetd.flags );
          planetd.construction--;
        }
        else
        {
          planetd.building[ build[a].type ] += build[a].quantity;
          planetd.construction -= build[a].quantity;
        }
        dbMapSetPlanet( build[a].plnid, &planetd );
        newd[2] = CMD_NEWS_BUILDING;
        newd[5] = build[a].plnid;
        newd[6] = build[a].plnpos;
      }
      else
      {
        if( !( dbUserFleetRetrieve( user->id, 0, &fleetd ) ) )
          continue;
        fleetd.unit[ build[a].type & 0xFFFF ] += build[a].quantity;
        if( !( dbUserFleetSet( user->id, 0, &fleetd ) ) )
          continue;
        newd[2] = CMD_NEWS_UNIT;
      }
      dbUserBuildRemove( user->id, a );
      newd[3] = build[a].type;
      newd[4] = build[a].quantity;
      cmdUserNewsAdd( user->id, newd, CMD_NEWS_FLAGS_BUILD );
    }
    free( build );


svDebugTickPass = 4;
		
// calc total of buildings, units, artefacts
    cmdTickPlanets( user->id, &maind );
		
svDebugTickPass = 5;
	

// add research

    for( a = 0 ; a < CMD_RESEARCH_NUMUSED ; a++ )
    {
    	
    	fa = ( (double)(maind.allocresearch[a]) * (double)( 500*cmdTickProduction[CMD_BUILDING_RESEARCH] + maind.fundresearch ) ) / 10000.0;
    	if( cmdRace[maind.raceid].special & CMD_RACE_SPECIAL_POPRESEARCH )
        	fa += ( (double)(maind.allocresearch[a]) * (double)maind.ressource[CMD_RESSOURCE_POPULATION] ) / ( 400.0 * 100.0 );
		
	/*		//ARTI CODE Foohon Ancestry
			if(maind.artefacts & ARTEFACT_*_BIT)
				fa += ( (double)(maind.allocresearch[a]) * (double)maind.ressource[CMD_RESSOURCE_POPULATION] ) / ( 400.0 * 100.0 );
	*/							
			
			
		maind.research[a] += cmdRace[maind.raceid].researchpoints[a] * fa;
		  
     	if( maind.research[a] < 0 )
        	maind.research[a] = 0x7FFFFFFF;			
    }
    maind.fundresearch = (long long int)( 0.9 * (double)maind.fundresearch );
    
svDebugTickPass = 6;

   // SK: because of the network backbone arti, we need to calculate Tech research first
    int addedFromTech = 0;
     
    // calculate total research for tech
     
          //research maximum
          fa = cmdRace[maind.raceid].researchmax[CMD_RESEARCH_TECH];
     
      /*      //      ARTI CODE Divine Stone
                    if(maind.artefacts & ARTEFACT_*_BIT)
                            fa -= 25;
     */
     
          b = fa * ( 1.0 - exp( (double)maind.research[CMD_RESEARCH_TECH] / ( -10.0 * (double)maind.networth ) ) );
     
     
          if( b > maind.totalresearch[CMD_RESEARCH_TECH] )
            maind.totalresearch[CMD_RESEARCH_TECH]++;
          else if( b < maind.totalresearch[CMD_RESEARCH_TECH] )
            maind.totalresearch[CMD_RESEARCH_TECH]--;
     
              addedFromTech = b/10;
     
     
    // calculate total research
        for( a = 0 ; a < CMD_RESEARCH_NUMUSED ; a++ )
        {
                    if(a == CMD_RESEARCH_TECH)
                            continue;
     
          //research maximum
          fa = cmdRace[maind.raceid].researchmax[a];

	// CODE_ARTI
      if( ( maind.artefacts & ARTEFACT_4_BIT ) && ( a == CMD_RESEARCH_MILITARY ) )
        fa += 25.0;		
     
        // put this arti last, you need the other ones calculated before this one.
         /*   //ARTI CODE network backbone    
        if(maind.artefacts & ARTEFACT_*_BIT)
        {
           // exclude tech research from having this bonus (otherwise there is no cap)
           if( a != CMD_RESEARCH_TECH)
               {
                fa += addedFromTech;
               }
        }*/

          b = fa * ( 1.0 - exp( (double)maind.research[a] / ( -10.0 * (double)maind.networth ) ) );
          if( b > maind.totalresearch[a] )
            maind.totalresearch[a]++;
          else if( b < maind.totalresearch[a] )
            maind.totalresearch[a]--;
                    }

svDebugTickPass = 7;


// calc infos
    fa = ( 12.0 * (double)(cmdTickProduction[CMD_BUILDING_SOLAR]) / specopSolarCalc( user->id ) );
    if( cmdRace[maind.raceid].special & CMD_RACE_SPECIAL_SOLARP15 )
      fa *= 1.15;
      
      //ARTI CODE Ether Palace
	if(maind.artefacts & ARTEFACT_32_BIT)
		fa *= 1.30;
		
    fb = ( 40.0 * (double)(cmdTickProduction[CMD_BUILDING_FISSION]) );
   	
   	 	
	fa += fb;
	
	//ARTI CODE Ether Garden
	
	if(maind.artefacts & ARTEFACT_ETHER_BIT)
		fa *= 1.10;	

	//ARTI CODE Ether Palace
	if(maind.artefacts & ARTEFACT_8_BIT)
		fa *= 1.25;	
			
	fb = cmdRace[maind.raceid].resource[CMD_RESSOURCE_ENERGY] * ( 1.00 + 0.01 * (float)maind.totalresearch[CMD_RESEARCH_ENERGY] );

	
  maind.infos[4] = (long long int)( fa * fb );
    
    /* This block is for the automated funding from energy production if used add the funding into the council with maind.infos
    	dbUserMainSet(user->id, &maind);
			cmd[0] = CMD_FUND_RESEARCH;
    	cmd[1] = user->id;
    	cmd[2] = maind.infos[4]*0.08;
			cmdExecute( (svConnectionPtr)NULL, cmd, 0, 0 );
			dbUserMainRetrieve( cmd[1], &maind );
			//maind.infos[4] -= maind.infos[4]*0.08;  //This line remove the actual funding from the production
  */
    
  fa = CMD_ENERGY_DECAY;
  	
		
  maind.infos[5] = fa * (double)maind.ressource[CMD_RESSOURCE_ENERGY];

  // meh! building upkeep
  maind.infos[6] = 0;
  for( a = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
  {
  	if( ( a == CMD_BUILDING_SOLAR ) || ( a == CMD_BUILDING_FISSION ) )
   	  maind.infos[6] += ((float)cmdTickProduction[a])*cmdBuildingUpkeep[a] * fb;
        else
          maind.infos[6] += ((float)cmdTickProduction[a])*cmdBuildingUpkeep[a];
  
  }
    
    
svDebugTickPass = 8;

    for( a = 0 ; a < CMD_UNIT_NUMUSED ; a++ )
	{
	  if( maind.totalunit[a] < 0 )
		maind.totalunit[a] = 0;
	}


    maind.infos[7] = 0;
    for( a = 0 ; a < CMD_UNIT_NUMUSED ; a++ )
		{
			maind.infos[7] += ((float)maind.totalunit[a])*cmdUnitUpkeep[a];
		}
		//ARTI CODE Romulan Military Outpost
		if(maind.artefacts & ARTEFACT_16_BIT)
		maind.infos[7] *= 1.5;	
		maind.infos[8] = (1.0/35.0) * (float)maind.ressource[CMD_RESSOURCE_POPULATION]* ( 1.00 + 0.01 * (float)maind.totalresearch[CMD_RESEARCH_WELFARE] ) * (cmdRace[maind.raceid].growth);
    
    
    if( maind.infos[8] >= maind.infos[6] )
      maind.infos[8] = maind.infos[6];
    
    //virus network mean more upkeep Based on the upkeep of a building with after pop reduction
    for( a = 0 ; a < opvirus ; a++ )
      maind.infos[6] += (long long int)( (float)(maind.infos[6]-maind.infos[8]) * 0.15 );
      
    maind.infos[9] = cmdRace[maind.raceid].resource[CMD_RESSOURCE_CRYSTAL] * (float)(cmdTickProduction[CMD_BUILDING_CRYSTAL]);

		fa = CMD_CRYSTAL_DECAY;
		
		//ARTI CODE Crystalline Entity | reduces crystal decay by 75%
	//	if(maind.artefacts & ARTEFACT_*_BIT)
	//	fa /= 4;
    	
    maind.infos[10] = fa * (double)maind.ressource[CMD_RESSOURCE_CRYSTAL];
    
    maind.infos[11] = pow( (maind.totalbuilding[CMD_BLDG_NUMUSED]-1), 1.2736 ) * 10000.0;
    
    //ARTI CODE Mana Gate
		//if(maind.artefacts & ARTEFACT_MANA_BIT)
		//	maind.infos[11] /= 2;
        
    if( maind.infos[11] < 0 )
      maind.infos[11] = 0;
      
    maind.infos[CMD_RESSOURCE_ENERGY] = maind.infos[4] - maind.infos[5] - maind.infos[6] - maind.infos[7] + maind.infos[8] - maind.infos[11];

    maind.infos[CMD_RESSOURCE_MINERAL] = cmdRace[maind.raceid].resource[CMD_RESSOURCE_MINERAL] * (float)(cmdTickProduction[CMD_BUILDING_MINING]);

	maind.infos[CMD_RESSOURCE_CRYSTAL] = (maind.infos[9] - maind.infos[10]);
	
	
	maind.infos[CMD_RESSOURCE_ECTROLIUM] = cmdRace[maind.raceid].resource[CMD_RESSOURCE_ECTROLIUM] * (float)(cmdTickProduction[CMD_BUILDING_REFINEMENT]);
		
	//ARTI CODE Mineral enhancement
		if(maind.artefacts & ARTEFACT_64_BIT)
	{
		maind.infos[CMD_RESSOURCE_MINERAL] *= 1.50;
	}
/*	//ARTI CODE Ectrolim enhancement
		if(maind.artefacts & ARTEFACT_xxxx_BIT)
	{
		maind.infos[CMD_RESSOURCE_ECTROLIUM] *= 1.50;
	}	*/
svDebugTickPass = 9;


// fleets decay?
    a = 65536*2;
    b = -1;


    if( ( maind.ressource[CMD_RESSOURCE_ENERGY] + maind.infos[CMD_RESSOURCE_ENERGY] ) < 0 )
    {
      a = -65536*3;
      b = CMD_UNIT_GHOST;
    }

// readiness
    for( c = 0 ; c < 3 ; c++ )
    {
      if( a < 0 )
      {
        if( maind.readiness[c] > ( -65536*200 - a ) )
          maind.readiness[c] += a;
        else if( maind.readiness[c] > -65536*200 )
          maind.readiness[c] = -65536*200;
      }
      else
        maind.readiness[c] += a;

			
/* CODE_ARTEFACT 
    if( ( maind.artefacts & ARTEFACT_*_BIT ) && ( c != 0 ) )
    {
      if( maind.readiness[c] > 65536*75 ) //alter this to increase or decrease the readiness, currently decreased to 75%
        maind.readiness[c] = 65536*75;	//alter this to increase or decrease the readiness, currently decreased to 75%
	}
    else
    {
      if( maind.readiness[c] > 65536*100 )
        maind.readiness[c] = 65536*100;
   }
 CODE_ARTEFACT*/

	if( maind.readiness[c] > 65536*100 )
		maind.readiness[c] = 65536*100;


    if( maind.readiness[c] < -65536*500 )
        maind.readiness[c] = -65536*500;

    }


svDebugTickPass = 10;


// calculate phantoms decay rate
    phdecay = 0.20;
    if( maind.totalunit[CMD_UNIT_WIZARD] )
    {
      fa = (double)maind.totalunit[CMD_UNIT_PHANTOM] / (double)maind.totalunit[CMD_UNIT_WIZARD];
      if( fa < 0.05 )
        phdecay = 0.01;
      else
      {
        fa = (double)pow( (double)( (1.0/0.05) * fa ), 2.4 );
        phdecay = 0.01*fa;
        if( phdecay > 0.20 )
          phdecay = 0.20;
      }
    }
    
// fleets
    dbUserMainSet( user->id, &maind );
    fleetp = 0;
    num = dbUserFleetList( user->id, &fleetp );
    for( a = num-1 ; a >= 0 ; a-- )
    {
      e = 0;
      if( a == 0 )
      {
        if( !( dbUserFleetRetrieve( user->id, 0, &fleetp[0] ) ) )
          return 1;
      }
      for( c = b ; c >= 0 ; c-- )
      {
        d = (int)ceil( (float)fleetp[a].unit[c] * 0.02 );
        fleetp[a].unit[c] -= d;
        maind.totalunit[c] -= d;
        e |= d;
      }

      if( fleetp[a].unit[CMD_UNIT_PHANTOM] )
      {
        d = (int)ceil( (float)fleetp[a].unit[CMD_UNIT_PHANTOM] * phdecay );
        fleetp[a].unit[CMD_UNIT_PHANTOM] -= d;
        maind.totalunit[CMD_UNIT_PHANTOM] -= d;
        e |= d;
      }

      if( ( a ) && !( fleetp[a].unit[CMD_UNIT_EXPLORATION] ) && !( fleetp[a].unit[CMD_UNIT_CRUISER] ) && !( fleetp[a].unit[CMD_UNIT_CARRIER] ) && !( fleetp[a].unit[CMD_UNIT_PHANTOM] ) && !( fleetp[a].unit[CMD_UNIT_AGENT] ) && !( fleetp[a].unit[CMD_UNIT_GHOST] ) )
      {
        dbUserFleetRemove( user->id, a );
        continue;
      }
      fleetd.flags &= 0xFFFFFFFF - CMD_FLEET_FLAGS_MOVED;
      if( fleetp[a].time > 0 )
      {
        fleetd.flags |= CMD_FLEET_FLAGS_MOVED;
        fleetp[a].time--;
        dbUserFleetSet( user->id, a, &fleetp[a] );
      }
      else if( e )
        dbUserFleetSet( user->id, a, &fleetp[a] );
      if( ( a != 0 ) && ( fleetp[a].time == 0 ) )
        cmdFleetAction( &fleetp[a], user->id, a, 1 );
    }
    if( fleetp )
      free( fleetp );
    dbUserMainRetrieve( user->id, &maind );


svDebugTickPass = 11;


// units decay on planets
    plist = 0;
    num = dbUserPlanetListIndices( user->id, &plist );
    for( a = 0 ; a < num ; a++ )
    {
      dbMapRetrievePlanet( plist[a], &planetd );
      e = 0;
      if( planetd.unit[CMD_UNIT_PHANTOM] )
      {
        planetd.unit[CMD_UNIT_PHANTOM] -= (int)ceil( phdecay * (float)(planetd.unit[CMD_UNIT_PHANTOM]) );
        e |= 1;
      }
      for( c = b ; c >= 0 ; c-- )
      {
        d = (int)ceil( (float)planetd.unit[c] * 0.02 );
        planetd.unit[c] -= d;
        maind.totalunit[c] -= d;
        e |= d;
      }
      if( e )
        dbMapSetPlanet( plist[a], &planetd );
    }	
    if( plist )
      free( plist );


svDebugTickPass = 12;


// income
    for( a = 0 ; a < CMD_RESSOURCE_NUMUSED ; a++ )
    {
      maind.ressource[a] += maind.infos[a];
      if( maind.ressource[a] < 0 )
        maind.ressource[a] = 0;
    }    

// networth
    a = dbUserPlanetNumber( user->id );
    maind.networth = ( 800 * a );
    for( a = 0 ; a < CMD_UNIT_NUMUSED ; a++ )
      maind.networth += maind.totalunit[a] * cmdUnitStats[a][CMD_UNIT_STATS_NETWORTH];
    for( a = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
    {
     	maind.networth += 8 * maind.totalbuilding[a];
    }

    maind.networth += (long long int)(0.004 * maind.ressource[CMD_RESSOURCE_POPULATION]);

    for( a = 0 ; a < CMD_RESEARCH_NUMUSED ; a++ )
      maind.networth += (0.001 * maind.research[a]);

// spec ops
    for( a = specopnum-1 ; a >= 0 ; a-- )
    {
      specopd[a].time--;
      if( specopd[a].time <= 0 )
      {
      	//remove planet beacon flag on the planet
      	if(specopd[a].type == CMD_OPER_PLANETBEACON)
      	{
      		dbMapRetrievePlanet(specopd[a].plnid, &planetd);
    			planetd.flags ^= CMD_PLANET_FLAGS_BEACON;
    			dbMapSetPlanet(specopd[a].plnid, &planetd);
      	}
      	
        dbUserSpecOpRemove( user->id, a );
        continue;
      }
      dbUserSpecOpSet( user->id, a, &specopd[a] );
    }

    if( specopd )
      free( specopd );


svDebugTickPass = 13;

    dbUserMainSet( user->id, &maind );
  }


svDebugTickPass = 14;


// bids energy decay
  marketbid[DB_MARKETBID_ACTION] = 0;
  for( marketbid[DB_MARKETBID_RESSOURCE] = 0 ; marketbid[DB_MARKETBID_RESSOURCE] < 3 ; marketbid[DB_MARKETBID_RESSOURCE]++ )
  {
    for( marketbid[DB_MARKETBID_PRICE] = 0 ; marketbid[DB_MARKETBID_PRICE] < DB_MARKET_RANGE ; marketbid[DB_MARKETBID_PRICE]++ )
    {
      b = dbMarketListStart( marketbid );
      while( (unsigned int)b < 0x1000000 )
      {
        c = dbMarketListNext( b, bidresult );
        a = (int)ceil( CMD_ENERGY_DECAY * (float)bidresult[0] );
        if( (bidresult[0]-a) > 0 )
        {
          dbUserMarketQuantity( bidresult[1], b, bidresult[0] - a );
          dbMarketSetQuantity( marketbid, b, bidresult[0] - a, a );
        }
        else
        {
          dbUserMarketRemove( bidresult[1], b );
          dbMarketRemove( marketbid, b );
        }
        b = c;
      }
    }
  }


svDebugTickPass = 15;


// bids crystal decay
  marketbid[DB_MARKETBID_ACTION] = 1;
  marketbid[DB_MARKETBID_RESSOURCE] = CMD_RESSOURCE_CRYSTAL-1;
  for( marketbid[DB_MARKETBID_PRICE] = 0 ; marketbid[DB_MARKETBID_PRICE] < DB_MARKET_RANGE ; marketbid[DB_MARKETBID_PRICE]++ )
  {
    b = dbMarketListStart( marketbid );
    while( (unsigned int)b < 0x1000000 )
    {
      c = dbMarketListNext( b, bidresult );
      a = (int)ceil( CMD_CRYSTAL_DECAY * (float)bidresult[0] );
      
      if( (bidresult[0]-a) > 0 )
      {
        dbUserMarketQuantity( bidresult[1], b, bidresult[0] - a );
        dbMarketSetQuantity( marketbid, b, bidresult[0] - a, a );
      }
      else
      {
        dbUserMarketRemove( bidresult[1], b );
        dbMarketRemove( marketbid, b );
      }
      b = c;
    }
  }


svDebugTickPass = 16;


  return 1;
}


