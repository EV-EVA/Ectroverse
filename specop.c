int specopAgentsAllowed( int specop, int raceid )
{
  if( !( cmdRace[raceid].operations & ( 1 << specop ) ) )
    return 0;
  return 1;
}

int specopPsychicsAllowed( int specop, int raceid )
{
  if( !( cmdRace[raceid].spells & ( 1 << specop ) ) )
    return 0;
  return 1;
}

int specopGhostsAllowed( int specop, int raceid )
{
  if( !( cmdRace[raceid].incantations & ( 1 << specop ) ) )
    return 0;
  return 1;
}


int specopAgentsReadiness( int specop, dbUserMainPtr maind, dbUserMainPtr main2d )
{
	int penalty, rel;
	float fa, fb, fFactor1, fFactor2;
	int nActive = 0;
	int nActive2 = 0;
	int i, curtime;
	int nMax;
	int Info[10];
	dbMainEmpireDef empired;
	dbMainEmpireDef empire2d;
	dbUserMainDef User;
	
	fFactor1 = 1;
	fFactor2 = 1;
	curtime = time( 0 );
	
	penalty = cmdGetOpPenalty( maind->totalresearch[CMD_RESEARCH_OPERATIONS], cmdAgentopTech[specop] );
	if( penalty == -1 )
	return -1;
	if( !( main2d ) )
		return (int)( cmdAgentopReadiness[specop] * 65536.0 );
	
	if( ( dbMapRetrieveEmpire( maind->empire, &empired ) < 0 ) || ( dbMapRetrieveEmpire( main2d->empire, &empire2d ) < 0 ) )
		return -1;

for(i=0;i<empired.numplayers;i++)
{
  	dbUserMainRetrieve(empired.player[i], &User);
  	//										1080 mean 18 hours this can be change the time is in min
  	if(((float)(curtime - User.lasttime)/60) <= 1080)
  		nActive++;
  }
  for(i=0;i<empire2d.numplayers;i++)
  {
  	dbUserMainRetrieve(empire2d.player[i], &User);
  	//										1080 mean 18 hours this can be change the time is in min
  	if(((float)(curtime - User.lasttime)/60) <= 1080)
  		nActive2++;
  }
  
  //we get the factor doing the number of max player in the emp * 2 - the active one / by the number of max player
  //so 1 active in a emp of one do (7*2-1)/7 = 1.85
  //2 active in a emp of 3 do (7*2-2)/7 = 1.7
  //7 active in a emp of 7(max) do (7*2-7)/7 = 1
  dbMapRetrieveMain(Info);
  nMax = Info[5];
  fFactor1 = (float)(nMax*2-nActive)/(float)nMax;
  fFactor2 = (float)(nMax*2-nActive2)/(float)nMax;
  
  
  fa = (float)(1+maind->planets) / (float)(1+main2d->planets);
  fb = (float)(1+empired.planets * fFactor1) / (float)(1+empire2d.planets * fFactor2);
  fa = pow( fa, 1.8 );
  fb = pow( fb, 1.2 );
  
  fa = 0.5 * ( fa + fb );

  if( fa < 0.75 )
    fa = 0.75;
  fa = ( 1.0 + 0.01*(float)penalty ) * cmdAgentopReadiness[specop] * fa;
  rel = cmdExecFindRelation( maind->empire, main2d->empire, 0, 0 );
  if( ( maind->empire == main2d->empire ) || ( rel == CMD_RELATION_WAR ) || ( rel == CMD_RELATION_ALLY ) )
    fa /= 3.0;

	if( fa >= 300.0 )
    return (int)( 300.0 * 65536.0 );
  return (int)( fa * 65536.0 );
}


void specopCalcMarketBids( int id, int *resources )
{
  int a, b, c;
  int *buffer;
  memset( resources, 0, sizeof(int)*CMD_RESSOURCE_NUMUSED );
  b = dbUserMarketList( id, &buffer );
  if( b <= 0 )
    return;
  for( a = c = 0 ; a < b ; a++, c += 5 )
  {
    if( !( buffer[c+DB_MARKETBID_ACTION] ) )
      resources[0] += buffer[c+DB_MARKETBID_QUANTITY] * buffer[c+DB_MARKETBID_PRICE];
    else
      resources[ buffer[c+DB_MARKETBID_RESSOURCE]+1 ] += buffer[c+DB_MARKETBID_QUANTITY];
  }
  if( buffer )
    free( buffer );
  return;
}

void specopReduceEnergyBids( int id, float loss )
{
  int a, b, c, d;
  int *buffer;
  b = dbUserMarketList( id, &buffer );
  if( b < 0 )
    return;
  for( a = c = 0 ; a < b ; a++, c += 5 )
  {
    if( buffer[c+DB_MARKETBID_ACTION] )
      continue;
    d = (int)ceil( (float)buffer[c+DB_MARKETBID_QUANTITY] * loss );
    if( buffer[c+DB_MARKETBID_QUANTITY] - d > 0 )
    {
      dbUserMarketQuantity( id, buffer[c+DB_MARKETBID_BIDID], buffer[c+DB_MARKETBID_QUANTITY] - d );
      dbMarketSetQuantity( &buffer[c], buffer[c+DB_MARKETBID_BIDID], buffer[c+DB_MARKETBID_QUANTITY] - d, d );
    }
    else
    {
      dbUserMarketRemove( id, buffer[c+DB_MARKETBID_BIDID] );
      dbMarketRemove( &buffer[c], buffer[c+DB_MARKETBID_BIDID] );
    }
  }
  if( buffer )
    free( buffer );
  return;
}

void specopReduceEctroliumBids( int id, float loss )
{
  int a, b, c, d;
  int *buffer;
  b = dbUserMarketList( id, &buffer );
  if( b < 0 )
    return;
  for( a = c = 0 ; a < b ; a++, c += 5 )
  {
    if( !( buffer[c+DB_MARKETBID_ACTION] ) )
      continue;
    if( buffer[c+DB_MARKETBID_RESSOURCE] != CMD_RESSOURCE_ECTROLIUM-1 )
      continue;
    d = (int)ceil( (float)buffer[c+DB_MARKETBID_QUANTITY] * loss );
    if( buffer[c+DB_MARKETBID_QUANTITY] - d > 0 )
    {
      dbUserMarketQuantity( id, buffer[c+DB_MARKETBID_BIDID], buffer[c+DB_MARKETBID_QUANTITY] - d );
      dbMarketSetQuantity( &buffer[c], buffer[c+DB_MARKETBID_BIDID], buffer[c+DB_MARKETBID_QUANTITY] - d, d );
    }
    else
    {
      dbUserMarketRemove( id, buffer[c+DB_MARKETBID_BIDID] );
      dbMarketRemove( &buffer[c], buffer[c+DB_MARKETBID_BIDID] );
    }
  }
  if( buffer )
    free( buffer );
  return;
}


/*
3: planet ID
4: planet position
5: faction ID
6: faction empire
7: attacker agents losses
8: defender agents losses
9: operation ID
*/
void specopAgentsPerformOp( int id, int fltid, dbUserFleetPtr fleetd, long long int *newd )
{
  int a, b, c, d, x, y, plnx, plny, specop, penalty, attack, defense, stealth, postnew;
  int i, nCancel = 0;
  float fa, fb, success, refatt, refdef, tlosses, dist;
  dbUserFleetDef fleet2d;
  dbMainPlanetDef planetd, planet2d;
  dbMainSystemDef systemd;
  dbUserMainDef maind, main2d;
  dbUserSpecOpDef specopd;
  dbUserBuildPtr buildd;
  int *plnlist;
  int resources[CMD_RESSOURCE_NUMUSED];

  postnew = 0;
  if(newd)
  {
	  if( newd[1] == CMD_NEWS_FLAGS_NEW )
	    postnew = CMD_NEWS_FLAGS_ATTACK;
	
		memset(&newd[10], -1, 11);
	  specop = fleetd->order - CMD_FLEET_ORDER_FIRSTOP;
	  newd[2] = CMD_NEWS_OPCANCEL;
	  if( (unsigned int)specop > CMD_AGENTOP_NUMUSED )
	    return;
	  if( dbMapRetrievePlanet( fleetd->destid, &planetd ) < 0 )
	    return;
	  if( dbUserMainRetrieve( id, &maind ) < 0 )
	    return;
	  if( ( maind.readiness[2] < 0 ) || (( planetd.owner == id )&&(fleetd->order!=CMD_FLEET_ORDER_PLANETBEACON)) ||  !( svTickStatus ) )
	  {
	  	cmdUserNewsAdd( id, newd, postnew );
	    return;
	  }
	  
	  //ARTI CODE Nuker
	 // if(!((maind.artefacts & ARTEFACT_NUK_BIT)&&(specop == CMD_OPER_NUKEPLANET )))
	//  {
	  	if( !( specopAgentsAllowed( specop, maind.raceid ) ) )
		  {
		    cmdUserNewsAdd( id, newd, postnew );
		    return;
		  }
	//	}
		penalty = cmdGetOpPenalty( maind.totalresearch[CMD_RESEARCH_OPERATIONS], cmdAgentopTech[specop] );
	  if( penalty == -1 )
	    return;
	  fa = 0.6 + (0.8/255.0) * (float)( rand() & 255 );
	
	  attack = (int)( fa * cmdRace[maind.raceid].unit[CMD_UNIT_AGENT] * (float)(fleetd->unit[CMD_UNIT_AGENT]) * ( 1.0 + 0.01*maind.totalresearch[CMD_RESEARCH_OPERATIONS] ) );
	
	  if( penalty )
	    attack = (float)attack / ( 1.0 + 0.01*(float)penalty );
	  stealth = cmdAgentopStealth[specop];
	  newd[7] = -1;
	  newd[8] = -1;
	
	
	
	  if( planetd.owner != -1 )
	  {
	  	if( dbUserMainRetrieve( planetd.owner, &main2d ) < 0 )
	      return;
	    if( !( dbUserFleetRetrieve( planetd.owner, 0, &fleet2d ) ) )
	      return;
	
/*				//ARTI CODE
		  if(maind.artefacts & ARTEFACT_ANTI_BIT)
		  	defense = (int)( cmdAgentopDifficulty[specop]*0.7 * cmdRace[main2d.raceid].unit[CMD_UNIT_AGENT] * (float)(fleet2d.unit[CMD_UNIT_AGENT]) * ( 1.0 + 0.01*main2d.totalresearch[CMD_RESEARCH_OPERATIONS] ) );	
		  else*/
		  	defense = (int)( cmdAgentopDifficulty[specop] * cmdRace[main2d.raceid].unit[CMD_UNIT_AGENT] * (float)(fleet2d.unit[CMD_UNIT_AGENT]) * ( 1.0 + 0.01*main2d.totalresearch[CMD_RESEARCH_OPERATIONS] ) );	
	  }
	  else
	  {
	  	defense = 50;
	    if( specop != CMD_OPER_OBSERVEPLANET )
	    {
	      cmdUserNewsAdd( id, newd, CMD_NEWS_FLAGS_ATTACK );
	      return;
	    }
	  }
		
		success = (float)attack / (float)( defense + 1 );
	
	  // calculate losses
	  if( ( success < 2.0 ) && ( planetd.owner != -1 ) )
	  {
	    refdef = 0.5 * pow( ( 0.5 * success ), 1.1 );
	    refatt = 1.0 - refdef;
	    tlosses = 1.0 - pow( ( 0.5 * success ), 0.2 );
	    
	    newd[7] = (int)( refatt * tlosses * (float)(fleetd->unit[CMD_UNIT_AGENT]) );
	    if( newd[7] >= fleetd->unit[CMD_UNIT_AGENT] )
	      newd[7] = fleetd->unit[CMD_UNIT_AGENT];
	    fleetd->unit[CMD_UNIT_AGENT] -= newd[7];
	    
	    newd[8] = (int)( refdef * tlosses * (float)(fleet2d.unit[CMD_UNIT_AGENT]) );
	    if( newd[8] >= fleet2d.unit[CMD_UNIT_AGENT] )
	      newd[8] = fleet2d.unit[CMD_UNIT_AGENT];
	    fleet2d.unit[CMD_UNIT_AGENT] -= newd[8];
	    stealth = 0;
	    dbUserFleetSet( id, fltid, fleetd );
	    dbUserFleetSet( planetd.owner, 0, &fleet2d );
	  }

		newd[3] = fleetd->destid;
	  newd[4] = planetd.position;
	  newd[9] = specop;
	  for( a = 10 ; a < DB_USER_NEWS_BASE ; a++ )
	    newd[a] = -1;
	
	  if( fleetd->order == CMD_FLEET_ORDER_SPYTARGET )
	  {  	
	    newd[2] = CMD_NEWS_OPSPYTARGET;
	    if( success >= 0.5 )
	      newd[10] = main2d.readiness[0];
	    if( success >= 0.7 )
	      newd[11] = main2d.readiness[1];
	    if( success >= 0.9 )
	      newd[12] = main2d.readiness[2];
	    specopCalcMarketBids( planetd.owner, resources );
	    if( success >= 1.0 )
	      newd[13] = main2d.ressource[0] + resources[0];
	    if( success >= 0.6 )
	      newd[14] = main2d.ressource[1] + resources[1];
	    if( success >= 0.4 )
	      newd[15] = main2d.ressource[2] + resources[2];
	    if( success >= 0.8 )
	      newd[16] = main2d.ressource[3] + resources[3];
	  }
	  else if( fleetd->order == CMD_FLEET_ORDER_OBSERVEPLANET )
	  {
	    newd[2] = CMD_NEWS_OPOBSERVEPLANET;
	    if( success >= 0.4 )
	      newd[10] = planetd.size;
	    if( success >= 1.0 )
	      newd[15+CMD_BLDG_NUMUSED] = artefactPrecense( &planetd );
	    if( success >= 0.9 )
	    {
	      newd[15+1+CMD_BLDG_NUMUSED] = (long long int)planetd.special[0];
	      newd[15+2+CMD_BLDG_NUMUSED] = (long long int)planetd.special[1];
	    }
	    if( planetd.owner == -1 )
	    {
	      memset( &planetd, 0, sizeof(dbMainPlanetDef) );
	      planetd.owner = -1;
	    }
	    if( success >= 0.5 )
	      newd[11] = planetd.population;
	    if( success >= 0.6 )
	      newd[12] = planetd.maxpopulation;
	    if( success >= 0.7 )
	      newd[13] = planetd.protection;
	    if( success >= 1.0 )
	      newd[14] = planetd.flags & ( CMD_PLANET_FLAGS_PORTAL | CMD_PLANET_FLAGS_PORTAL_BUILD );
	    if( success >= 0.8 )
	    {
	    	for(i=0;i<CMD_BLDG_NUMUSED;i++)
	    		newd[15+i] = planetd.building[i];
	    }
	  }
	  else if( fleetd->order == CMD_FLEET_ORDER_NETWORKVIRUS )
	  {
	    newd[2] = CMD_NEWS_OPNETWORKVIRUS;
	    if( success >= 1.0 )
	      a = 3;
	    else
	      a = (int)( (3.0/0.6) * ( success - 0.4 ) );
	    if( a > 0 )
	    {
	      newd[10] = a;
	      fa = 1.0 - 0.01 * (float)a;
	      for( a = 0 ; a < CMD_RESEARCH_NUMUSED ; a++ )
	        main2d.research[a] = main2d.research[a] * fa;
	      dbUserMainSet( planetd.owner, &main2d );
	    }
	    if( success >= 1.0 )
	      a = 32;
	    else
	      a = (int)( (32.0/0.4) * ( success - 0.6 ) );
	    if( a > 0 )
	    {
	      newd[11] = a;
	      specopd.type = CMD_OPER_NETWORKVIRUS;
	      specopd.plnid = fleetd->destid;
	      specopd.plnpos = planetd.position;
	      specopd.time = (int)newd[11];
	      specopd.factionid = planetd.owner;
	      dbUserSpecOpAdd( id, &specopd );
	      if( newd[7] != -1 )
	        specopd.factionid = id;
	      else
	        specopd.factionid = -1;
	      specopd.type |= 0x10000;
	      dbUserSpecOpAdd( planetd.owner, &specopd );
	    }
	  }
	  else if( fleetd->order == CMD_FLEET_ORDER_INFILTRATION )
	  {
	  	newd[2] = CMD_NEWS_OPINFILTRATION;
	    specopCalcMarketBids( planetd.owner, resources );
	    if( success >= 0.6 )
	      newd[10] = main2d.ressource[0] + resources[0];
	    if( success >= 0.3 )
	      newd[11] = main2d.ressource[1] + resources[1];
	    if( success >= 0.2 )
	      newd[12] = main2d.ressource[2] + resources[2];
	    if( success >= 0.4 )
	      newd[13] = main2d.ressource[3] + resources[3];
	    if( success >= 0.9 )
	    {
	    	//14 to 20
	   		for(i=0;i<CMD_RESEARCH_NUMUSED;i++)
	   			newd[14+i] = main2d.totalresearch[i];
	    }
	    if( success >= 1.0 )
	    //21 to 29
	    for(i=0;i<CMD_BLDG_NUMUSED;i++)
	    	newd[14+CMD_RESEARCH_NUMUSED+i] = main2d.totalbuilding[i];
	  }
	  else if( fleetd->order == CMD_FLEET_ORDER_BIOINFECTION )
	  {
	    newd[2] = CMD_NEWS_OPBIOINFECTION;
	    if( ( b = dbUserPlanetListIndices( planetd.owner, &plnlist ) ) < 0 )
	      return;
	    plnx = ( planetd.position >> 8 ) & 0xFFF;
	    plny = planetd.position >> 20;
	    if( success >= 1.0 )
	      fa = 0.7;
	    else
	      fa = (int)( (0.7/0.4) * ( success - 0.6 ) );
	    a = 100.0 * fa;
	    c = 0;
	    if( a > 0 )
	    {
	      for( a = 0 ; a < b ; a++ )
	      {
	        if( dbMapRetrievePlanet( plnlist[a], &planet2d ) < 0 )
	          continue;
	        if( plnlist[a] == fleetd->destid )
	          continue;
	        x = ( ( planet2d.position >> 8 ) & 0xFFF ) - plnx;
	        y = ( planet2d.position >> 20 ) - plny;
	        dist = sqrt( (float)(x*x) + (float)(y*y) );
	        if( dist >= 16.0 )
	          continue;
	        fb = 1.0 - ( dist / 16.0 );
	        d = (int)( (float)(planet2d.population) * fa * fb );
	        c += d;
	        planet2d.population -= d;
	        dbMapSetPlanet( plnlist[a], &planet2d );
	      }
	      dbMapRetrievePlanet( fleetd->destid, &planet2d );
	      d = (int)( (float)(planet2d.population) * fa * fa );
	      c += planet2d.population - d;
	      planet2d.population = d;
	      dbMapSetPlanet( fleetd->destid, &planet2d );
	      newd[10] = c;
	    }
	    if( plnlist )
	      free( plnlist );
	    
	    newd[11] = rand()%42;
	    if(newd[11] < 5)
	    	newd[11] = 5;
	      
	    specopd.type = CMD_OPER_BIOINFECTION;
	    specopd.plnid = fleetd->destid;
	    specopd.plnpos = planetd.position;
	    specopd.time = newd[11];
	    specopd.factionid = planetd.owner;
	    dbUserSpecOpAdd( id, &specopd );
	    if( newd[7] != -1 )
	      specopd.factionid = id;
	    else
	      specopd.factionid = -1;
	    specopd.type |= 0x10000;
	    dbUserSpecOpAdd( planetd.owner, &specopd );
	  }
	  else if( fleetd->order == CMD_FLEET_ORDER_ENERGYTRANSFER )
	  {
	    newd[2] = CMD_NEWS_OPENERGYTRANSFER;
	    if( success >= 1.0 )
	      a = 15;
	    else
	      a = (int)( (15.0/0.6) * ( success - 0.4 ) );
	    if( a > 0 )
	    {
	      specopCalcMarketBids( planetd.owner, resources );
	      fa = 0.01 * (float)a;
	      newd[10] = (long long int)( fa * (float)( main2d.ressource[0] + resources[0] ) );
	      specopReduceEnergyBids( planetd.owner, fa );
	      main2d.ressource[0] = (long long int)( (float)(main2d.ressource[0]) * ( 1.0 - fa ) );
	      fb = 0.3 + (0.7/255.0) * (float)( rand() & 255 );
	      newd[11] = (long long int)( fb * (float)newd[10] );
	      maind.ressource[0] += newd[11];
	      dbUserMainSet( id, &maind );
	      dbUserMainSet( planetd.owner, &main2d );
	    }
	  }
	  else if( fleetd->order == CMD_FLEET_ORDER_MILITARYSAB )
	  {
	    newd[2] = CMD_NEWS_OPMILITARYSAB;
	    if( success >= 1.0 )
	      a = 8;
	    else
	      a = (int)( (8.0/0.5) * ( success - 0.5 ) );
	    if( a > 0 )
	    {
	      if( !( planetd.flags & CMD_PLANET_FLAGS_PORTAL ) )
	        newd[10] = -2;
	      else
	      {
	        for( b = 0 ; b < CMD_UNIT_FLEET ; b++ )
	        {
	          fa = 0.01 * (float)( a + (rand()&3) );
	          c = (int)( (float)(fleet2d.unit[b]) * fa );
	          newd[10+b] = c;
	          fleet2d.unit[b] -= c;
	        }
	        dbUserFleetSet( planetd.owner, 0, &fleet2d );
	      }
	    }
	  }
	  else if( fleetd->order == CMD_FLEET_ORDER_NUKEPLANET )
	  {
	    if( planetd.flags & CMD_PLANET_FLAGS_HOME )
	    {
	      cmdUserNewsAdd( id, newd, CMD_NEWS_FLAGS_ATTACK );
	      return;
	    }
	    newd[2] = CMD_NEWS_OPNUKEPLANET;
	    if( success >= 1.0 )
	    {
	      if( !( dbUserPlanetRemove( planetd.owner, fleetd->destid ) ) )
	        return;
	      if( planetd.construction )
	      {
	        b = dbUserBuildList( planetd.owner, &buildd );
	        for( a = b-1 ; a >= 0 ; a-- )
	        {
	          if( buildd[a].plnid == fleetd->destid )
	            dbUserBuildRemove( planetd.owner, a );
	        }
	        if( buildd )
	          free( buildd );
	      }
	      newd[10] = 1;
	      planetd.flags &= CMD_PLANET_FLAGS_HOME;
	      a = planetd.owner;
	      planetd.owner = -1;
	      planetd.construction = 0;
	      planetd.population = planetd.size * CMD_POPULATION_BASE_FACTOR;
	      memset( planetd.building, 0, 16*sizeof(int) );
	      memset( planetd.unit, 0, 16*sizeof(int) );
	      dbMapSetPlanet( fleetd->destid, &planetd );
	      planetd.owner = a;
	      dbMapRetrieveSystem( planetd.system, &systemd );
	      systemd.unexplored++;
	      dbMapSetSystem( planetd.system, &systemd );
	    }
	  }
	  else if( fleetd->order == CMD_FLEET_ORDER_HIGHINFIL )
	  {
	    a = 0;
	    newd[2] = CMD_NEWS_OPHIGHINFIL;
	    
	    if( success >= 0.6 )
	    {
	      if( success >= 0.8 )
	      {
	        a |= 1;
	        if( success >= 1.0 )
	          a |= 2;
	      }
	      newd[10] = a;
	      specopd.vars[0] = a;
	      specopd.type = CMD_OPER_HIGHINFIL;
	      specopd.plnid = fleetd->destid;
	      specopd.plnpos = planetd.position;
	      specopd.time = 104;
	      specopd.factionid = planetd.owner;
	      dbUserSpecOpAdd( id, &specopd );
	      if( newd[7] != -1 )
	        specopd.factionid = id;
	      else
	        specopd.factionid = -1;
	      specopd.type |= 0x10000;
	      dbUserSpecOpAdd( planetd.owner, &specopd );
	    }
	  }
	  else if( fleetd->order == CMD_FLEET_ORDER_PLANETBEACON )
	  {
	    dbMapRetrievePlanet(fleetd->destid, &planetd);
	   	if((planetd.flags & CMD_PLANET_FLAGS_BEACON) == 0) //Not stackable its a flags
	   	{
	   		a = 0;
	    	newd[2] = CMD_NEWS_OPPLANETBEACON;
	    	if( success >= 1 )
		    {
		    	//Set the flags of planetary on this planet
		    	planetd.flags |= CMD_PLANET_FLAGS_BEACON;
		    	
		      newd[10] = success;
		      specopd.vars[0] = a;
		      specopd.type = CMD_OPER_PLANETBEACON;
		      specopd.plnid = fleetd->destid;
		      specopd.plnpos = planetd.position;
		      specopd.time = 24;
		      specopd.factionid = planetd.owner;
		      dbUserSpecOpAdd( id, &specopd );
		      if( newd[7] != -1 )
		        specopd.factionid = id;
		      else
		        specopd.factionid = -1;
		      specopd.type |= 0x10000;
		  		if(id != planetd.owner)
		   			dbUserSpecOpAdd( planetd.owner, &specopd );
		  
		  	}
				dbMapSetPlanet(fleetd->destid, &planetd);
			}
			else
				nCancel++;	
		}
		if( !( stealth ) && ( planetd.owner != -1 ) )
		{
		  a = newd[1];
		  newd[1] = CMD_NEWS_FLAGS_NEW;
		  newd[2] += CMD_NEWS_OPSPYTARGET_TARGET - CMD_NEWS_OPSPYTARGET;
		  newd[5] = id;
		  newd[6] = maind.empire;
		  
		  //ARTI CODE Cloak Of Invisibility
			/*if(maind.artefacts & ARTEFACT_8_BIT)
			{
				newd[5] = 0;  //Report ops by admin
				newd[6] = 0;
			}*/
			
		  if(id != planetd.owner)
		  	cmdUserNewsAdd( planetd.owner, newd, CMD_NEWS_FLAGS_ATTACK );
		  newd[1] = a;
		  newd[2] -= CMD_NEWS_OPSPYTARGET_TARGET - CMD_NEWS_OPSPYTARGET;
		}
	  newd[5] = planetd.owner;
	  newd[6] = main2d.empire;
	  cmdUserNewsAdd( id, newd, postnew );
		
		if(!nCancel)
	  {
	  	a = specopAgentsReadiness( specop, &maind, ( planetd.owner != -1 ) ? ( &main2d ) : ( 0 ) );
	 		maind.readiness[2] -= a;
	  	dbUserMainSet( id, &maind );
	  }
	}
	else
	{
		if( dbMapRetrievePlanet( fleetd->destid, &planetd ) < 0 )
	   	return;	
	  if(planetd.owner != -1)
	  {
	    if( !( dbUserPlanetRemove( planetd.owner, fleetd->destid ) ) )
	    	return;
	    if( planetd.construction )
	    {
	      b = dbUserBuildList( planetd.owner, &buildd );
	      for( a = b-1 ; a >= 0 ; a-- )
	      {
	        if( buildd[a].plnid == fleetd->destid )
	          dbUserBuildRemove( planetd.owner, a );
	      }
	      if( buildd )
	        free( buildd );
	    }
	    a = planetd.owner;
	    planetd.owner = -1;
	    planetd.construction = 0;
	    planetd.population = planetd.size * CMD_POPULATION_BASE_FACTOR;
	    memset( planetd.building, 0, 16*sizeof(int) );
	    memset( planetd.unit, 0, 16*sizeof(int) );
	    dbMapSetPlanet( fleetd->destid, &planetd );
	    planetd.owner = a;
	    dbMapRetrieveSystem( planetd.system, &systemd );
	    systemd.unexplored++;
	    dbMapSetSystem( planetd.system, &systemd );
	  }
	}
	
	return;
}




int specopPsychicsReadiness( int specop, dbUserMainPtr maind, dbUserMainPtr main2d )
{
  int penalty, rel;
  float fa, fb, fFactor1, fFactor2;
  int nActive = 0;
  int nActive2 = 0;
  int i, curtime;
  int Info[10];
  int nMax;
  dbUserMainDef User;
  dbMainEmpireDef empired;
  dbMainEmpireDef empire2d;
  
  fFactor1 = 1;
  fFactor2 = 1;
  curtime = time( 0 );
  penalty = cmdGetOpPenalty( maind->totalresearch[CMD_RESEARCH_WELFARE], cmdPsychicopTech[specop] );
  if( penalty == -1 )
    return -1;
  else if( !( main2d ) )
    return (int)( cmdPsychicopReadiness[specop] * 65536.0 );

  if( ( dbMapRetrieveEmpire( maind->empire, &empired ) < 0 ) || ( dbMapRetrieveEmpire( main2d->empire, &empire2d ) < 0 ) )
    return -1;
   
  for(i=0;i<empired.numplayers;i++)
  {
  	dbUserMainRetrieve(empired.player[i], &User);
  	//										1080 mean 18 hours this can be change the time is in min
  	if(((float)(curtime - User.lasttime)/60) <= 1080)
  		nActive++;
  }
  for(i=0;i<empire2d.numplayers;i++)
  {
  	dbUserMainRetrieve(empire2d.player[i], &User);
  	//										1080 mean 18 hours this can be change the time is in min
  	if(((float)(curtime - User.lasttime)/60) <= 1080)
  		nActive2++;
  }  
  //we get the factor doing the number of max player in the emp * 2 - the active one / by the number of max player
  //so 1 active in a emp of one do (7*2-1)/7 = 1.85
  //2 active in a emp of 3 do (7*2-2)/7 = 1.7
  //7 active in a emp of 7(max) do (7*2-7)/7 = 1
  dbMapRetrieveMain(Info);
  nMax = Info[5];
  fFactor1 = (float)(nMax*2-nActive)/(float)nMax;
  fFactor2 = (float)(nMax*2-nActive2)/(float)nMax;  
    
    
  fa = (float)(1+maind->planets) / (float)(1+main2d->planets);
  fb = (float)(1+empired.planets * fFactor1) / (float)(1+empire2d.planets * fFactor2);
  fa = pow( fa, 1.8 );
  fb = pow( fb, 1.2 );
  fa = 0.5 * ( fa + fb );

  if( fa < 0.75 )
    fa = 0.75;
  fa = ( 1.0 + 0.01*(float)penalty ) * cmdPsychicopReadiness[specop] * fa;
  rel = cmdExecFindRelation( maind->empire, main2d->empire, 0, 0 );
  if( ( maind->empire == main2d->empire ) || ( rel == CMD_RELATION_WAR ) || ( rel == CMD_RELATION_ALLY ) )
    fa /= 3.0;
  
	if( fa >= 300.0 )
    return (int)( 300.0 * 65536.0 );
  return (int)( fa * 65536.0 );
}





/*
3: faction ID
4: faction empire
5: attacker psychics losses
6: defender psychics losses
7: spell ID
*/
void specopPsychicsPerformOp( int id, int targetid, int specop, int psychics, long long int *newd )
{
  int a, penalty, defense, stealth, i, k;
  long long int j, attack;
  float fa, success, refatt, refdef, tlosses;
  dbUserFleetDef fleetd, fleet2d;
  dbUserMainDef maind, main2d;
  dbUserSpecOpDef specopd;
  dbUserSpecOpPtr specop2d;
  int resources[CMD_RESSOURCE_NUMUSED];

  newd[2] = CMD_NEWS_SPCANCEL;
  if( (unsigned int)specop > CMD_PSYCHICOP_NUMUSED )
    return;
  if( dbUserMainRetrieve( id, &maind ) < 0 )
    return;
  if( dbUserMainRetrieve( targetid, &main2d ) < 0 )
    return;
  if( !( dbUserFleetRetrieve( id, 0, &fleetd ) ) )
    return;
  if( ( (unsigned int)psychics > fleetd.unit[CMD_UNIT_WIZARD] ) || ( maind.readiness[1] < 0 ) ||  !( svTickStatus ) )
  {
    cmdUserNewsAdd( id, newd, 0 );
    return;
  }
  if( !( dbUserFleetRetrieve( targetid, 0, &fleet2d ) ) )
    return;
  if( !( main2d.planets ) )
    return;
  if( !( specopPsychicsAllowed( specop, maind.raceid ) ) || !( psychics ) )
  {
    cmdUserNewsAdd( id, newd, 0 );
    return;
  }

  penalty = cmdGetOpPenalty( maind.totalresearch[CMD_RESEARCH_WELFARE], cmdPsychicopTech[specop] );
  if( penalty == -1 )
    return;
  fa = 0.4 + (1.2/255.0) * (float)( rand() & 255 );

	attack = (long long int)( fa * cmdRace[maind.raceid].unit[CMD_UNIT_WIZARD] * (float)psychics * ( 1.0 + 0.005*maind.totalresearch[CMD_RESEARCH_WELFARE] ) / cmdPsychicopDifficulty[specop] );
/* //ARTI CODE
	  if(maind.artefacts & ARTEFACT_ANTI_BIT)
	  	attack *= 0.8;*/
  if( penalty )
    attack = (float)attack / ( 1.0 + 0.01*(float)penalty );
  stealth = cmdPsychicopFlags[specop] & 11;
  newd[5] = -1;
  newd[6] = -1;
  success = 0.0;



  if( id != targetid )
  {
    defense = (int)( cmdRace[main2d.raceid].unit[CMD_UNIT_WIZARD] * (float)(fleet2d.unit[CMD_UNIT_WIZARD]) * ( 1.0 + 0.005*main2d.totalresearch[CMD_RESEARCH_WELFARE] ) );
    
/*    //ARTI CODE
	  if(main2d.artefacts & ARTEFACT_ANTI_BIT)
	  	defense *= 0.8;
  */  
    success = (float)attack / (float)( defense + 1 );

    if( specop == CMD_SPELL_ASSAULT )
    {
      refdef = pow( (float)attack / (float)( attack + defense ), 1.1 );
      refatt = pow( (float)defense / (float)( attack + defense ), 1.1 );
      tlosses = 0.2;
      goto assault;
    }

    // calculate losses
    if( success < 2.0 )
    {
      refdef = 0.5 * pow( ( 0.5 * success ), 1.1 );
      
      refatt = 1.0 - refdef;
      tlosses = 1.0 - pow( ( 0.5 * success ), 0.2 );
      assault:

      newd[5] = (long long int)( refatt * tlosses * (float)psychics );
   	  if( ( refdef * tlosses ) > 1.0 ) 
             psychics = (newd[5] * 0.8);
      fleetd.unit[CMD_UNIT_WIZARD]  -= (int)newd[5];
		
      newd[6] = (long long int)( refdef * tlosses * (float)(fleet2d.unit[CMD_UNIT_WIZARD]) );
	  if( ( refdef * tlosses ) > 1.0 ) newd[6] = fleet2d.unit[CMD_UNIT_WIZARD];
      fleet2d.unit[CMD_UNIT_WIZARD] -= (int)newd[6];

	  if( fleetd.unit[CMD_UNIT_WIZARD] < 0 )
	  {
		printf( "Error #1, negative unit count : %d, %d", id, targetid );
		fleetd.unit[CMD_UNIT_WIZARD] = 0;
	  }

	  if( fleet2d.unit[CMD_UNIT_WIZARD] < 0 )
	  {
		printf( "Error #2, negative unit count : %d, %d", targetid, id );
		fleet2d.unit[CMD_UNIT_WIZARD] = 0;
	  }

      stealth = 0;
      dbUserFleetSet( id, 0, &fleetd );
      dbUserFleetSet( targetid, 0, &fleet2d );
    }
  }

  newd[7] = specop;
  for( a = 8 ; a < DB_USER_NEWS_BASE ; a++ )
    newd[a] = -1;

  if( cmdPsychicopFlags[specop] & 2 )
  {
    if( id != targetid )
    {
      cmdUserNewsAdd( id, newd, 0 );
      return;
    }
  }
  else
  {
    if( id == targetid )
    {
      cmdUserNewsAdd( id, newd, 0 );
      return;
    }
  }

  if( specop == CMD_SPELL_IRRAECTRO )
  {
    newd[2] = CMD_NEWS_SPIRRAECTRO;
    if( success >= 1.0 )
      a = 20;
    else
      a = (int)( (20.0/0.6) * ( success - 0.4 ) );
    if( a > 0 )
    {
      newd[8] = a;
      specopCalcMarketBids( targetid, resources );
      fa = 0.01 * (float)a;
      newd[8] = (int)( fa * (float)( main2d.ressource[CMD_RESSOURCE_ECTROLIUM] + resources[CMD_RESSOURCE_ECTROLIUM] ) );
      specopReduceEctroliumBids( targetid, fa );
      main2d.ressource[CMD_RESSOURCE_ECTROLIUM] = (int)( (float)(main2d.ressource[CMD_RESSOURCE_ECTROLIUM]) * ( 1.0 - fa ) );
      dbUserMainSet( targetid, &main2d );
    }
  }
  else if( specop == CMD_SPELL_DARKWEB )
  {
    newd[2] = CMD_NEWS_SPDARKWEB;
    fa = 100.0 * (float)attack / (float)maind.networth;
    a = (int)( fa * 3.5 );
    if( a >= 0 )
    {
      newd[8] = a;
      specopd.vars[0] = a;
      specopd.type = CMD_SPELL_DARKWEB | 0x1000;
      specopd.plnid = -1;
      specopd.plnpos = -1;
      specopd.time = 52;
      specopd.factionid = id;
      dbUserSpecOpAdd( id, &specopd );
    }
  }
  else if( specop == CMD_SPELL_INCANDESCENCE )
  {
    newd[2] = CMD_NEWS_SPINCANDESCENCE;
    j = attack * 5;
    if( j > maind.ressource[CMD_RESSOURCE_CRYSTAL] )
      j = maind.ressource[CMD_RESSOURCE_CRYSTAL];
    maind.ressource[CMD_RESSOURCE_CRYSTAL] -= j;
  
    newd[8] = j;
    newd[9] = (long long int)( (float)j * 24.0 * ( 1.0 + 0.01*maind.totalresearch[CMD_RESEARCH_WELFARE] ) );
    
    maind.ressource[CMD_RESSOURCE_ENERGY] += newd[9];
    
    dbUserMainSet( id, &maind );
  }
  else if( specop == CMD_SPELL_BLACKMIST )
  {
    newd[2] = CMD_NEWS_SPBLACKMIST;
    if( success >= 1.0 )
      a = 25;
    else
      a = (int)( (25.0/0.6) * ( success - 0.4 ) );
    if( a > 0 )
    {
      newd[8] = a;
      specopd.vars[0] = a;
      specopd.type = CMD_SPELL_BLACKMIST | 0x1000;
      specopd.plnid = -1;
      specopd.plnpos = -1;
      specopd.time = 26 + ( rand() & 0x1F );
      specopd.factionid = targetid;
      dbUserSpecOpAdd( id, &specopd );
      if( newd[5] != -1 )
        specopd.factionid = id;
      else
        specopd.factionid = -1;
      specopd.type |= 0x10000;
      dbUserSpecOpAdd( targetid, &specopd );
    }
  }
  else if( specop == CMD_SPELL_WARILLUSIONS )
  {
    
    newd[2] = CMD_NEWS_SPWARILLUSIONS;
    fa = 100.0 * (float)attack / (float)maind.networth;
    a = (int)( fa * 4.5 );
    if( a >= 0 )
    {
			a += a * rand()%20;
    	k = 0;
    	j = dbUserSpecOpList(id, &specop2d);
    	for(i=0;i<j;i++)
    	{
    		if (specop2d[i].type == (CMD_SPELL_WARILLUSIONS | 0x1000))
    		{
    			dbUserSpecOpRemove(id, i);
    		}
    	}
    	newd[8] = a;
      specopd.vars[0] = a;
      specopd.type = CMD_SPELL_WARILLUSIONS | 0x1000;
      specopd.plnid = -1;
      specopd.plnpos = -1;
      newd[9] = 32 + ( rand() & 31 );
      specopd.time = (int)newd[9];
      specopd.factionid = id;
      dbUserSpecOpAdd( id, &specopd );
	    
    }
  }



  else if( specop == CMD_SPELL_ASSAULT )
  {
    newd[2] = CMD_NEWS_SPASSAULT;
  }



  else if( specop == CMD_SPELL_PHANTOMS )
  {
    newd[2] = CMD_NEWS_SPPHANTOMS;



    j = attack / 2;
    
    newd[8] = j;
    fleetd.unit[CMD_UNIT_PHANTOM] += j;
    dbUserFleetSet( id, 0, &fleetd );
    maind.totalunit[CMD_UNIT_PHANTOM] += j;
    dbUserMainSet( id, &maind );
  }
  else
  {
    cmdUserNewsAdd( id, newd, 0 );
    return;
  }


  if( !( stealth ) && ( id != targetid ) )
  {
    a = (int)newd[1];
    newd[1] = CMD_NEWS_FLAGS_NEW;
    newd[2] += CMD_NEWS_SPIRRAECTRO_TARGET - CMD_NEWS_SPIRRAECTRO;
    newd[3] = id;
    newd[4] = maind.empire;
    cmdUserNewsAdd( targetid, newd, CMD_NEWS_FLAGS_ATTACK );
    newd[1] = a;
    newd[2] -= CMD_NEWS_SPIRRAECTRO_TARGET - CMD_NEWS_SPIRRAECTRO;
  }
  newd[3] = targetid;
  newd[4] = main2d.empire;
  cmdUserNewsAdd( id, newd, 0 );

  dbUserMainRetrieve( id, &maind );
  a = specopPsychicsReadiness( specop, &maind, ( id != targetid ) ? ( &main2d ) : ( 0 ) );
  maind.readiness[1] -= a;
  dbUserMainSet( id, &maind );

  return;
}





int specopGhostsReadiness( int specop, dbUserMainPtr maind, dbUserMainPtr main2d )
{
  int penalty, rel;
  float fa, fb, fFactor1, fFactor2;
  int nActive = 0;
  int nActive2 = 0;
  int i, curtime;
  int Info[10];
  int nMax;
  dbUserMainDef User;
  dbMainEmpireDef empired;
  dbMainEmpireDef empire2d;
  
  curtime = time( 0 );
  
  penalty = cmdGetOpPenalty( maind->totalresearch[CMD_RESEARCH_WELFARE], cmdGhostopTech[specop] );
  if( penalty == -1 )
    return -1;
   if( !( main2d ) )
  	return (int)( cmdGhostopReadiness[specop] * 65536.0 );
  

  if( ( dbMapRetrieveEmpire( maind->empire, &empired ) < 0 ) || ( dbMapRetrieveEmpire( main2d->empire, &empire2d ) < 0 ) )
    return -1;
  
  for(i=0;i<empired.numplayers;i++)
  {
  	dbUserMainRetrieve(empired.player[i], &User);
  	//										1080 mean 18 hours this can be change the time is in min
  	if(((float)(curtime - User.lasttime)/60) <= 1080)
  		nActive++;
  }
  for(i=0;i<empire2d.numplayers;i++)
  {
  	dbUserMainRetrieve(empire2d.player[i], &User);
  	//										1080 mean 18 hours this can be change the time is in min
  	if(((float)(curtime - User.lasttime)/60) <= 1080)
  		nActive2++;
  }  
  //we get the factor doing the number of max player in the emp * 2 - the active one / by the number of max player
  //so 1 active in a emp of one do (7*2-1)/7 = 1.85
  //2 active in a emp of 3 do (7*2-2)/7 = 1.7
  //7 active in a emp of 7(max) do (7*2-7)/7 = 1
  dbMapRetrieveMain(Info);
  nMax = Info[5];
  fFactor1 = (float)(nMax*2-nActive)/(float)nMax;
  fFactor2 = (float)(nMax*2-nActive2)/(float)nMax;  
  
  fa = (float)(1+maind->planets) / (float)(1+main2d->planets);
  fb = (float)(1+empired.planets * fFactor1) / (float)(1+empire2d.planets * fFactor2);
  fa = pow( fa, 1.8 );
  fb = pow( fb, 1.2 );
  fa = 0.5 * ( fa + fb );

  if( fa < 0.75 )
    fa = 0.75;
  fa = ( 1.0 + 0.01*(float)penalty ) * cmdGhostopReadiness[specop] * fa;
  rel = cmdExecFindRelation( maind->empire, main2d->empire, 0, 0 );
  if( ( maind->empire == main2d->empire ) || ( rel == CMD_RELATION_WAR ) || ( rel == CMD_RELATION_ALLY ) )
    fa /= 3.0;

  if( fa >= 300.0 )
    return (int)( 300.0 * 65536.0 );
  return (int)( fa * 65536.0 );
}




/*
3: planet or system ID, or -1
4: planet or system position
5: faction ID
6: faction empire
7: attacker ghosts losses
8: defender psychics losses
9: defender ghosts losses
10: operation ID
*/
void specopGhostsPerformOp( int id, int fltid, dbUserFleetPtr fleetd, long long int *newd )
{
  int a, b, c, d, x, y, dx, dy, specop, penalty, attack, defense, defenseghosts, stealth, postnew, plntarget;
  double fb, success, successghosts, refatt, refdef, tlosses;//, dist;
  double fa, ent, en[6], endiv[6];
  dbUserFleetDef fleet2d;
  dbMainPlanetDef planetd, planet2d;
  dbMainSystemDef systemd;
  dbUserMainDef maind, main2d;
  dbUserSpecOpDef specopd;
  dbUserBuildPtr buildd;
  int *plnlist;
//  int resources[CMD_RESSOURCE_NUMUSED];

  postnew = 0;
  if( newd[1] == CMD_NEWS_FLAGS_NEW )
    postnew = CMD_NEWS_FLAGS_ATTACK;

  specop = fleetd->order - CMD_FLEET_ORDER_FIRSTINCANT;
  newd[2] = CMD_NEWS_INCANCEL;
  if( (unsigned int)specop > CMD_GHOSTOP_NUMUSED )
    return;
  plntarget = 0;
  newd[3] = -1;
  newd[5] = -1;
  newd[6] = -1;
  if( !( cmdGhostopFlags[specop] & 4 ) )
  {
    if( dbMapRetrievePlanet( fleetd->destid, &planetd ) < 0 )
      return;
    if( planetd.owner == -1 )
    {
      cmdUserNewsAdd( id, newd, postnew );
      return;
    }
    if( !( cmdGhostopFlags[specop] & 2 ) )
    {
      if( id == planetd.owner )
      {
        cmdUserNewsAdd( id, newd, 0 );
        return;
      }
    }
    if( dbUserMainRetrieve( planetd.owner, &main2d ) < 0 )
      return;
    newd[6] = main2d.empire;
    plntarget = 1;
    newd[3] = fleetd->destid;
    newd[5] = planetd.owner;
  }
  if( dbUserMainRetrieve( id, &maind ) < 0 )
    return;
  if( ( maind.readiness[1] < 0 ) || !( svTickStatus ) )
  {
    cmdUserNewsAdd( id, newd, postnew );
    return;
  }
  if( !( specopGhostsAllowed( specop, maind.raceid ) ) )
  {
    cmdUserNewsAdd( id, newd, postnew );
    return;
  }

  penalty = cmdGetOpPenalty( maind.totalresearch[CMD_RESEARCH_WELFARE], cmdGhostopTech[specop] );
  if( penalty == -1 )
    return;
  fa = 0.6 + (0.8/255.0) * (double)( rand() & 255 );
 
	attack = (long long int)( ( fa * cmdRace[maind.raceid].unit[CMD_UNIT_GHOST] * (double)(fleetd->unit[CMD_UNIT_GHOST]) * ( 1.0 + 0.01*maind.totalresearch[CMD_RESEARCH_WELFARE] ) ) / (double)cmdGhostopDifficulty[specop] ); 
  
	if( penalty )
    attack = (double)attack / ( 1.0 + 0.01*(double)penalty );
  stealth = cmdGhostopFlags[specop] & 1;
  newd[7] = -1;
  newd[8] = -1;
  newd[9] = -1;


  success = 1.0;
  if( ( plntarget ) && ( id != planetd.owner ) && !( cmdGhostopFlags[specop] & 8 ) )
  {
    if( !( dbUserFleetRetrieve( planetd.owner, 0, &fleet2d ) ) )
      return;
    defense = (int)( (1.0/7.0) * cmdRace[main2d.raceid].unit[CMD_UNIT_WIZARD] * (double)(fleet2d.unit[CMD_UNIT_WIZARD]) * ( 1.0 + 0.01*main2d.totalresearch[CMD_RESEARCH_WELFARE] ) );
    
    defenseghosts = (int)( cmdRace[main2d.raceid].unit[CMD_UNIT_GHOST] * (double)(fleet2d.unit[CMD_UNIT_GHOST]) * ( 1.0 + 0.01*main2d.totalresearch[CMD_RESEARCH_WELFARE] ) );
    
    success = (double)attack / (double)( defense + 1 );
    if( success < 2.0 )
    {
      refdef = 0.5 * pow( ( 0.5 * success ), 1.1 );
      tlosses = 1.0 - pow( ( 0.5 * success ), 0.2 );
      newd[8] = (long long int)( refdef * tlosses * (double)(fleet2d.unit[CMD_UNIT_WIZARD]) );
      if( newd[8] >= fleet2d.unit[CMD_UNIT_WIZARD] )
        newd[8] = fleet2d.unit[CMD_UNIT_WIZARD];
      fleet2d.unit[CMD_UNIT_WIZARD] -= (int)newd[8];
      stealth = 0;
      dbUserFleetSet( planetd.owner, 0, &fleet2d );
    }
    if( defenseghosts )
    {
      successghosts = (double)attack / (double)( defenseghosts + 1 );
      if( successghosts < 2.0 )
      {
        refdef = 0.5 * pow( ( 0.5 * successghosts ), 1.1 );
        refatt = 1.0 - refdef;
        tlosses = 1.0 - pow( ( 0.5 * successghosts ), 0.2 );
        
        //This is where attacker losse their ghost ship
        newd[7] = (long long int)( refatt * tlosses * (double)(fleetd->unit[CMD_UNIT_GHOST]) );
        if( newd[7] >= fleetd->unit[CMD_UNIT_GHOST] )
          newd[7] = fleetd->unit[CMD_UNIT_GHOST];
        fleetd->unit[CMD_UNIT_GHOST] -= (int)newd[7];

        newd[9] = (long long int)( refdef * tlosses * (double)(fleet2d.unit[CMD_UNIT_GHOST]) );

        if( newd[9] >= fleet2d.unit[CMD_UNIT_GHOST] )
          newd[9] = fleet2d.unit[CMD_UNIT_GHOST];

        if( newd[9] >= fleet2d.unit[CMD_UNIT_GHOST] )
          fleet2d.unit[CMD_UNIT_GHOST]= 0;
        else
          fleet2d.unit[CMD_UNIT_GHOST]-= (int)newd[9];

				stealth = 0;
        dbUserFleetSet( id, fltid, fleetd );
        dbUserFleetSet( planetd.owner, 0, &fleet2d );
      }
    }
  }
  newd[4] = fleetd->destination;
  newd[10] = specop;
  for( a = 11 ; a < DB_USER_NEWS_BASE ; a++ )
    newd[a] = -1;


  if( fleetd->order == CMD_FLEET_ORDER_SENSE )
  {
    newd[2] = CMD_NEWS_INSENSE;

    // fa = maximum search distance for artefact
    fa = 2.0 * (double)log10( 10.0 * (double)attack );
    fa *= ( (double)( rand() & 0xFF ) ) / 256.0;
		newd[11] = 0;
    x = ( fleetd->destination >> 8 ) & 0xFFF;
    y = fleetd->destination >> 20;
    for( a = 0 ; a < ARTEFACT_NUMUSED ; a++ )
    {
      dx = ( ( dbArtefactPos[a] >> 8 ) & 0xFFF ) - x;
      dy = ( dbArtefactPos[a] >> 20 ) - y;
      fb = sqrt( (double)( 1 + dx*dx + dy*dy ) );
      if( fb >= fa )
        continue;
      newd[11] = 0x10000 | (int)( fa / fb );
      if( fa >= 4 )
        newd[12] = dbArtefactPos[a] & 0xFFFFFFFF;
      else if( fa >= 3 )
        newd[12] = dbArtefactPos[a] & 0xFFFFFF00;
      break;
    }
  }
  else if( fleetd->order == CMD_FLEET_ORDER_SURVEY )
  {
   specopd.vars[0] = 0;
    newd[2] = CMD_NEWS_INSURVEY;
    if( ( ( b = dbMapFindSystem( ( fleetd->destination >> 8 ) & 0xFFF, fleetd->destination >> 20 ) ) < 0 ) || ( dbMapRetrieveSystem( b, &systemd ) < 0 ) )
      c = d = 0;
    else
    {
      c = systemd.numplanets;
      if( c > 32 )
        c = 32;
      d = 0;
      for( a = 0 ; a < c ; a++ )
      {
        if( dbMapRetrievePlanet( systemd.indexplanet + a, &planetd ) < 0 )
          return;
        if( planetd.owner == -1 )
          defense = 100;
        else
        {
          if( dbUserMainRetrieve( planetd.owner, &main2d ) < 0 )
            continue;
          defense = (int)( (1.0/7.0) * cmdRace[main2d.raceid].unit[CMD_UNIT_WIZARD] * (double)(main2d.totalunit[CMD_UNIT_WIZARD]) * ( 1.0 + 0.01*main2d.totalresearch[CMD_RESEARCH_WELFARE] ) );
        }
        
        success = (double)attack / (double)( defense + 1 );
        if( success < 1.0 )
          continue;
        specopd.vars[0] |= 1 << a;
        d++;
      }
    }
    newd[11] = d;
    if( d > 0 )
    {
      specopd.type = CMD_INCANT_SURVEY | 0x2000;
      specopd.plnid = b;
      specopd.plnpos = fleetd->destination;
      newd[12] = 20 + ( rand() & 31 );
      specopd.time = (int)newd[12];
      specopd.factionid = id;
      dbUserSpecOpAdd( id, &specopd );
    }
  }
  else if( fleetd->order == CMD_FLEET_ORDER_SHIELDING )
  {
    newd[2] = CMD_NEWS_INSHIELDING;
    fa = 500.0 * (double)attack * ( ( (double)( 128 + ( rand() & 0x7F ) ) ) / 256.0 );
    a = (int)fa;
    if( a > 0 )
    {
      newd[11] = a;
      specopd.vars[0] = a;
      specopd.type = CMD_INCANT_SHIELDING | 0x2000;
      specopd.plnid = fleetd->destid;
      specopd.plnpos = fleetd->destination;
      specopd.time = 10 + ( rand() & 0x1F );
      newd[12] = specopd.time;
      specopd.factionid = planetd.owner;
      dbUserSpecOpAdd( id, &specopd );
      specopd.factionid = id;
      specopd.type |= 0x10000;
      dbUserSpecOpAdd( planetd.owner, &specopd );
    }
  }
  else if( fleetd->order == CMD_FLEET_ORDER_FORCEFIELD )
  {
    newd[2] = CMD_NEWS_INFORCEFIELD;
    fa = 0.0;
    if( success >= 0.5 )
      fa = 200.0 * ( success - 0.5 );
    a = (int)fa;
    if( a > 0 )
    {
      newd[11] = a;
      specopd.vars[0] = a;
      specopd.type = CMD_INCANT_FORCEFIELD | 0x2000;
      specopd.plnid = fleetd->destid;
      specopd.plnpos = fleetd->destination;
      specopd.time = 16 + ( rand() & 0x1F );
      newd[12] = specopd.time;
      specopd.factionid = planetd.owner;
      dbUserSpecOpAdd( id, &specopd );
      specopd.factionid = id;
      specopd.type |= 0x10000;
      dbUserSpecOpAdd( planetd.owner, &specopd );
    }
  }
  else if( fleetd->order == CMD_FLEET_ORDER_VORTEX )
  {
    newd[2] = CMD_NEWS_INVORTEX;
    fa = 7.0 * (double)attack / (double)maind.networth;
    a = (int)( 120.0 * fa );
    if( a > 2 )
    {
      specopd.type = CMD_INCANT_VORTEX | 0x2000;
      specopd.plnid = fleetd->destid;
      specopd.plnpos = fleetd->destination;
      specopd.time = a;
      newd[11] = specopd.time;
      specopd.factionid = id;
      dbUserSpecOpAdd( id, &specopd );
    }
  }
  else if( fleetd->order == CMD_FLEET_ORDER_MINDCONTROL )
  {
    if( planetd.flags & CMD_PLANET_FLAGS_HOME )
    {
      cmdUserNewsAdd( id, newd, CMD_NEWS_FLAGS_ATTACK );
      return;
    }
    newd[2] = CMD_NEWS_INMINDCONTROL;
    if( success >= 1.0 )
    {
      if( !( dbUserPlanetRemove( planetd.owner, fleetd->destid ) ) )
        return;
      if( planetd.construction )
      {
        b = dbUserBuildList( planetd.owner, &buildd );
        for( a = b-1 ; a >= 0 ; a-- )
        {
          if( buildd[a].plnid == fleetd->destid )
            dbUserBuildRemove( planetd.owner, a );
        }
        if( buildd )
          free( buildd );
      }
      newd[11] = 1;
      a = planetd.owner;
      planetd.flags = 0;
      planetd.owner = id;
      planetd.construction = 0;
      planetd.population = planetd.size * CMD_POPULATION_BASE_FACTOR;
      memset( planetd.unit, 0, 16*sizeof(int) );
      dbMapSetPlanet( fleetd->destid, &planetd );
      dbUserPlanetAdd( id, fleetd->destid, planetd.system, planetd.position, planetd.flags );
      planetd.owner = a;
    }
  }
  else if( fleetd->order == CMD_FLEET_ORDER_ENERGYSURGE )
  {
    newd[2] = CMD_NEWS_INENERGYSURGE;


    if( success >= 1.0 )
    {
//      newd[11] = main2d.ressource[CMD_RESSOURCE_ENERGY];


        ent = 320.0*(double)attack + (double)main2d.ressource[CMD_RESSOURCE_ENERGY];



/*
Energy use :
Kill 1 RP takes 3 energy
Kill 1 resource takes 6 energy
Kill 1 Solar takes 30 energy
Kill 1 Fission takes 400 energy
*/


      en[0] = main2d.ressource[CMD_RESSOURCE_MINERAL];
      en[1] = main2d.ressource[CMD_RESSOURCE_CRYSTAL];
      en[2] = main2d.ressource[CMD_RESSOURCE_ECTROLIUM];
      en[3] = main2d.totalbuilding[CMD_BUILDING_SOLAR];
      en[4] = main2d.totalbuilding[CMD_BUILDING_FISSION];
      for( a = 0 ; a < CMD_RESEARCH_NUMUSED ; a++ )
        en[5] += main2d.research[a];

      en[0] *= 6;
      en[1] *= 6;
      en[2] *= 6;
      en[3] *= 30;
      en[4] *= 400;
      en[5] *= 3;
      fa = en[0] + en[1] + en[2] + en[3] + en[4] + en[5];
      if( ent > fa )
        ent = fa;

      // endiv : factor for each
      endiv[0] = en[0] / fa;
      endiv[1] = en[1] / fa;
      endiv[2] = en[2] / fa;
      endiv[3] = en[3] / fa;
      endiv[4] = en[4] / fa;
      endiv[5] = en[5] / fa;

      for( a = 0 ; a < 3 ; a++ )
      {
        if( en[a] < 0.0001 )
          continue;
	fa = ( main2d.ressource[1+a] * ( ent * endiv[a] ) ) / en[a];
	if( fa > (float)main2d.ressource[1+a] )
		fa = (float)main2d.ressource[1+a];
	b = (int)fa;
	newd[11+a] = b;
	main2d.ressource[1+a] -= b;
      }

      if( ( b = dbUserPlanetListIndices( planetd.owner, &plnlist ) ) > 0 )
      {
        fa = fb = 0.0;
        if( en[3] > 0.0001 )
          fa = ( ent * endiv[3] ) / en[3];
        if( en[4] > 0.0001 )
          fb = ( ent * endiv[4] ) / en[4];
        newd[14] = newd[15] = 0;
        for( a = 0 ; a < b ; a++ )
        {
          if( dbMapRetrievePlanet( plnlist[a], &planet2d ) < 0 )
            continue;
          c = planetd.building[CMD_BUILDING_SOLAR] * fa;
          if( c > planetd.building[CMD_BUILDING_SOLAR] )
            c = planetd.building[CMD_BUILDING_SOLAR];
          planetd.building[CMD_BUILDING_SOLAR] -= c;
          newd[14] += c;
          c = planetd.building[CMD_BUILDING_FISSION] * fb;
          if( c > planetd.building[CMD_BUILDING_FISSION] )
            c = planetd.building[CMD_BUILDING_FISSION];
          planetd.building[CMD_BUILDING_FISSION] -= c;
          newd[15] += c;
          dbMapSetPlanet( plnlist[a], &planet2d );
        }
      }

      if( plnlist )
        free( plnlist );

      if( en[5] > 0.0001 )
      {
        newd[16] = 0;
        fa = ( ent * endiv[5] ) / en[5];
        for( a = 0 ; a < CMD_RESEARCH_NUMUSED ; a++ )
        {
          b = main2d.research[a] * fa;
          if( b > main2d.research[a] )
            b = main2d.research[a];
          main2d.research[a] -= b;
          newd[16] += b;
        }
      }

      newd[17] = main2d.ressource[CMD_RESSOURCE_ENERGY];
      main2d.ressource[CMD_RESSOURCE_ENERGY] = 0;
      dbUserMainSet( planetd.owner, &main2d );
    }



  }



/*
Psychic Assault - attemps to kill the psychics of another faction, causing psychics casualities on both sides.

Sense Artefact - Attempts to locate the nearest artefact, from a particular system
Survey System - Attempt to determine the size, resource bonus and the precense of portals for each planet of a solar system
Planetary Shielding - Create temporary shielding protecting defensive units for a specific planet
Portal Force Field - Create a force field around a specific planet, making portal travel very difficult, reducing the owner capabilities to protect it
Vortex Portal - Create a temporary portal in the targeted system from which fleets can attack and return to the main fleet
Mind Control - Take control of an enemy planet with all its infrastructure, most of the population is killed
Energy Surge - Spreads a destructive wave in an faction network, feeding on the faction's energy reserves ; anihilating resource reserves and research
*/


  if( ( !( stealth ) && ( plntarget ) ) || ( ( cmdGhostopFlags[specop] & 8 ) && ( planetd.owner != id ) ) )
  {
    a = (int)newd[1];
    newd[1] = CMD_NEWS_FLAGS_NEW;
    newd[2] += CMD_NEWS_INSENSE_TARGET - CMD_NEWS_INSENSE;
    newd[5] = id;
    newd[6] = maind.empire;
    cmdUserNewsAdd( planetd.owner, newd, CMD_NEWS_FLAGS_ATTACK );
    newd[1] = a;
    newd[2] -= CMD_NEWS_INSENSE_TARGET - CMD_NEWS_INSENSE;
  }
  if( plntarget )
  {
    newd[5] = planetd.owner;
    newd[6] = main2d.empire;
  }
  cmdUserNewsAdd( id, newd, postnew );

  a = specopGhostsReadiness( specop, &maind, ( plntarget ) ? ( &main2d ) : ( 0 ) );
  	
  maind.readiness[1] -= a;
  dbUserMainSet( id, &maind );

  return;
}









/////////////////////////////////////////////////////////////////////////////////////////////////////////////////



float specopDarkWebCalc( int id )
{
  int a, b;
  float fr;
  dbUserSpecOpPtr specopd;
  fr = 1.0;
  if( ( b = dbUserSpecOpList( id, &specopd ) ) >= 0 )
  {
    for( a = 0 ; a < b ; a++ )
    {
      if( specopd[a].type != ( CMD_SPELL_DARKWEB | 0x1000 ) )
        continue;
      fr *= 1.0 + 0.01 * (float)specopd[a].vars[0];
    }
    free( specopd );
  }
  return fr;
}




float specopSolarCalc( int id )
{
  int a, b;
  float sc;
  dbUserSpecOpPtr specopd;
  sc = 1.0;
  if( ( b = dbUserSpecOpList( id, &specopd ) ) >= 0 )
  {
    for( a = 0 ; a < b ; a++ )
    {
      if( specopd[a].type == ( CMD_SPELL_DARKWEB | 0x1000 ) )
        sc *= 1.0 + 0.005 * (float)specopd[a].vars[0];
      else if( specopd[a].type == ( CMD_SPELL_BLACKMIST | 0x1000 | 0x10000 ) )
        sc *= 1.0 + 0.01 * (float)specopd[a].vars[0];
    }
    free( specopd );
  }
  return sc;
}





float specopWarIllusionsCalc( int id )
{
  int a, b, c = 0;
  float pr;
  dbUserSpecOpPtr specopd;
  pr = 1.0;
  if( ( b = dbUserSpecOpList( id, &specopd ) ) >= 0 )
  {
    for( a = 0 ; a < b ; a++ )
    {
      if( specopd[a].type != ( CMD_SPELL_WARILLUSIONS | 0x1000 ) )
        continue;
      c++;
      if(c == 3)
      	break;
      pr *= 1.0 + 0.01 * (float)specopd[a].vars[0];
    }
    free( specopd );
  }
  return pr;
}



int specopShieldingCalc( int id, int plnid )
{
  int a, b, pr;
  dbUserSpecOpPtr specopd;
  pr = 0;
  if( ( b = dbUserSpecOpList( id, &specopd ) ) >= 0 )
  {
    for( a = 0 ; a < b ; a++ )
    {
      if( specopd[a].type != ( CMD_INCANT_SHIELDING | 0x2000 | 0x10000 ) )
        continue;
      if( specopd[a].plnid != plnid )
        continue;
      pr += specopd[a].vars[0];
    }
    free( specopd );
  }
  return pr;
}



int specopForcefieldCalc( int id, int plnid )
{
  int a, b, ff;
  dbUserSpecOpPtr specopd;
  ff = 0;
  if( ( b = dbUserSpecOpList( id, &specopd ) ) >= 0 )
  {
    for( a = 0 ; a < b ; a++ )
    {
      if( specopd[a].type != ( CMD_INCANT_FORCEFIELD | 0x2000 | 0x10000 ) )
        continue;
      if( specopd[a].plnid != plnid )
        continue;
      ff += specopd[a].vars[0];
    }
    free( specopd );
  }
  return ff;
}



int specopVortexListCalc( int id, int num, int **buffer )
{
  int a, b, vnum, totalnum;
  int *list, *newlist;
  dbUserSpecOpPtr specopd = 0;
  vnum = 0;
  if( ( b = dbUserSpecOpList( id, &specopd ) ) >= 0 )
  {
    for( a = 0 ; a < b ; a++ )
    {
      if( specopd[a].type != ( CMD_INCANT_VORTEX | 0x2000 ) )
        continue;
      vnum++;
    }
  }
  if( !( vnum ) )
  {
    if( specopd )
      free( specopd );
    return num;
  }

  list = *buffer;
  totalnum = num + vnum;

  if( !( newlist = malloc( 3*totalnum*sizeof(int) ) ) )
  {
    if( specopd )
      free( specopd );
    return num;
  }

  *buffer = newlist;
  if( list )
  {
    memcpy( newlist, list, 3*num*sizeof(int) );
    free( list );
    newlist += num*3;
  }

  for( a = 0 ; a < b ; a++ )
  {
    if( specopd[a].type != ( CMD_INCANT_VORTEX | 0x2000 ) )
      continue;
    newlist[0] = -1;
    newlist[1] = -1;
    newlist[2] = specopd[a].plnpos;
    newlist += 3;
    num++;
  }

  if( specopd )
    free( specopd );
  return totalnum;
}










