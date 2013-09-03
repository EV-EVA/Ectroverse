
float battlePortalCalc( int tx, int ty, int *portals, int num, int research )
{
  int a, x, y;
  float range, dist, cover;
  range = 7.0 * ( 1.0 + 0.01*research );
  cover = 0.0;
  for( a = 0 ; a < num ; a++ )
  {
    x = ( ( portals[a] >> 8 ) & 0xFFF ) - tx;
    y = ( portals[a] >> 20 ) - ty;
    dist = sqrt( x*x + y*y );
    if( dist >= range )
      continue;
    cover += 1.0 - sqrt( dist / range );
  }
  if( cover >= 1.0 )
    cover = 1.0;
    
  return cover;
}

float battlePortalCoverage( int id, int tx, int ty, int research )
{
  int num;
  int *buffer;
  float cover;
  if( ( num = dbUserPortalsListCoords( id, &buffer ) ) < 0 )
    return 0.0;
/*
specopVortexListCalc expects another format
  num = specopVortexListCalc( id, num, &buffer );
*/
  cover = battlePortalCalc( tx, ty, buffer, num, research );
  free( buffer );
  
  return cover;
}


int battleReadinessLoss( dbUserMainPtr maind, dbUserMainPtr main2d )
{
  float fa, fb, fdiv, max, fFactor1, fFactor2;
  unsigned char *err;
  int nActive = 0;
  int nActive2 = 0;
  int i, curtime;
  int Info[10];
  int nMax;
  dbMainEmpireDef empired;
  dbMainEmpireDef empire2d;
  dbUserMainDef User;
  
  fFactor1 = 1;
  fFactor2 = 1;
  curtime = time( 0 );
  fa = (float)(1+maind->planets) / (float)(1+main2d->planets);
  if( maind->empire == main2d->empire )
  {
    fa = powf( fa, 1.3 );
    fb = 1.0;
    max = 16.0;
  }
  else
  {
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
    dbMapRetrieveMain(Info);
    nMax = Info[5];
    fFactor1 = (float)(nMax*2-nActive)/(float)nMax;
    fFactor2 = (float)(nMax*2-nActive2)/(float)nMax;
    
    fa = powf( fa, 1.3 );
    fb = (float)(1+empired.planets*fFactor1) / (float)(1+empire2d.planets*fFactor2);
    fb = powf( fb, 1.8 );
    max = 100.0;
  }


  fdiv = 0.5;
  if( fb < fa )
    fdiv = 0.5 * powf( fb / fa, 0.8 );

  fa = ( fdiv * fa ) + ( (1.0-fdiv) * fb );
  if( fa < 0.50 )
    fa = 0.50;


  fa *= 11.5;

  err = cmdErrorString;
  if( ( maind->empire == main2d->empire ) || ( cmdExecFindRelation( maind->empire, main2d->empire, 0, 0 ) == CMD_RELATION_WAR ) )
    fa /= 3.0;
  cmdErrorString = err;

	
	if((cmdRace[main2d->raceid].special & CMD_RACE_SPECIAL_CULPROTECT)&&(maind->empire != main2d->empire))
	{
    	fa *= log10(main2d->totalresearch[CMD_RESEARCH_WELFARE]+10);
	}

	if( fa >= max )
    return (int)( max * 65536.0 );
  return (int)( fa * 65536.0 );
}




void battlePhaseUpdate( int *unit, int *results )
{
  int a;
  float fa;
  if( results[CMD_UNIT_CARRIER] )
  {
    fa = (float)results[CMD_UNIT_CARRIER] / (float)unit[CMD_UNIT_CARRIER];
    results[CMD_UNIT_BOMBER] += fa * (float)unit[CMD_UNIT_BOMBER];
    results[CMD_UNIT_FIGHTER] += fa * (float)unit[CMD_UNIT_FIGHTER];
    results[CMD_UNIT_TRANSPORT] += fa * (float)unit[CMD_UNIT_TRANSPORT];
  }
  if( results[CMD_UNIT_TRANSPORT] )
  {
    fa = (float)results[CMD_UNIT_TRANSPORT] / (float)unit[CMD_UNIT_TRANSPORT];
    results[CMD_UNIT_SOLDIER] += fa * (float)unit[CMD_UNIT_SOLDIER];
    results[CMD_UNIT_DROID] += fa * (float)unit[CMD_UNIT_DROID];
    results[CMD_UNIT_GOLIATH] += fa * (float)unit[CMD_UNIT_GOLIATH];
  }
  for( a = 0 ; a < CMD_UNIT_FLEET ; a++ )
  {

    if( results[a] < 0 )
    {
printf( "CRAP %d %d\n", a, results[a] );
    results[a] = unit[a];
    }


    unit[a] -= results[a];
    if( unit[a] < 0 )
    {
      results[a] += unit[a];
      unit[a] = 0;
    }
  }
  return;
}


int battle( int id, int fltid, int *results )
{
  int a, b, flee, stationleft, defid;
  float fa, fb, cover, attfactor, deffactor;
  double attdam, defdam;
  int *attunit;
  int attunitbase[CMD_UNIT_NUMUSED];
  int defunit[CMD_UNIT_NUMUSED];
  int defunitbase[CMD_UNIT_NUMUSED];
  dbUserMainDef maind, main2d;
  dbUserFleetDef fleetd, fleet2d;
  dbMainPlanetDef planetd;
  dbUserBuildPtr buildd;
  long long int newd[DB_USER_NEWS_BASE];
  dbUserPtr user;
  double attstats[CMD_UNIT_FLEET][CMD_UNIT_STATS_BATTLE];
  double defstats[CMD_UNIT_FLEET][CMD_UNIT_STATS_BATTLE];

  int shields;
  int defsats, defsatsbase;
  long long int hpcarrier, hptransport, hpcruiser, hpbomber, hpfighter, hpsoldier, hpdroid, hpgoliath, hpphantom, hptotal, hpsats;
  double damcarrier, damtransport, damcruiser, dambomber, damfighter, damsoldier, damdroid, damgoliath, damphantom, damsats;

  // get attacking fleet
  if( dbUserFleetRetrieve( id, fltid, &fleetd ) < 0 )
  {
    cmdErrorString = "This fleet doesn't exist!";
    return -3;
  }
  if( fleetd.order != CMD_FLEET_ORDER_ATTACK )
  {
    cmdErrorString = "The order of this fleet is not to attack";
    return -3;
  }
  if( fleetd.time != -1 )
  {
    cmdErrorString = "Your fleet isn't ready to attack";
    return -3;
  }

  // check attack
  if( dbMapRetrievePlanet( fleetd.destid, &planetd ) < 0 )
  {
    sprintf( cmdErrorBuffer, "Could not retrieve planet data ( %d )", fleetd.destid );
    cmdErrorString = cmdErrorBuffer;
    return -3;
  }
  if( planetd.owner == -1 )
  {
    cmdErrorString = "This planet has not been explored yet, you must send an exploration ship if you wish to colonize it.";
    return -3;
  }
  if( planetd.owner == id )
  {
    cmdErrorString = "This planet is already yours!";
    return -3;
  }
	

  defid = planetd.owner;
  if( dbUserMainRetrieve( id, &maind ) < 0 )
    return -3;
  if( dbUserMainRetrieve( defid, &main2d ) < 0 )
    return -3;

	if( planetd.flags & CMD_PLANET_FLAGS_HOME )
  {
    if( !( user = dbUserLinkID( defid ) ) )
      return -3;
    if( !( user->flags & CMD_USER_FLAGS_INDEPENDENT ) || ( main2d.planets > 1 ) || ( main2d.createtime+3600*24 > time( 0 ) ) )
    {
      cmdErrorString = "You can attack a home planet only if the faction is declared independent, does not own any other planet and the account has been created since more than 24 hours.";
      return -3;
    }
  }


  // portals coverage
  if( planetd.flags & CMD_PLANET_FLAGS_PORTAL )
    cover = 1.0;
  else
    cover = battlePortalCoverage( defid, ( planetd.position >> 8 ) & 0xFFF, planetd.position >> 20, main2d.totalresearch[CMD_RESEARCH_PORTALS] );
    
  a = specopForcefieldCalc( defid, fleetd.destid );
  if( a )
    cover /= ( 1.0 + 0.01*(float)a );


  if( maind.readiness[0] < -65536*100 )
  {
    cmdErrorString = "Your forces require time to recover and prepare before engaging a new battle. Forcing them to attack would have more than disastrous consequences.";
    return -3;
  }
  else if( maind.readiness[0] < -65536*60 )
    cmdErrorString = "<b>Your forces are completely unprepared for another battle! Their effectiveness will be greatly reduced, you can also expect desertions.</b>";
  else if( maind.readiness[0] < -65536*40 )
    cmdErrorString = "<b>Your forces are exhausted, their effectiveness in battle will be very low.</b>";
  else if( maind.readiness[0] < -65536*20 )
    cmdErrorString = "<b>Your forces are very tired and certainly won't fight well.</b>";
  else if( maind.readiness[0] < 0 )
    cmdErrorString = "<b>Your forces seem to require rest, they won't be fighting as well as they could in different circumstances.</b>";


  // global factors
  fa = 1.0;
  if( maind.readiness[0] < 0 )
    fa = 1.0 - ( (float)maind.readiness[0] / (-65536.0*130.0) );

  attfactor = ( ( cmdRace[maind.raceid].attack / cmdRace[main2d.raceid].defense ) * fa ) / specopWarIllusionsCalc( defid );
  deffactor = ( ( cmdRace[main2d.raceid].attack / cmdRace[maind.raceid].defense ) / fa ) / specopWarIllusionsCalc( id );
          
	if(planetd.flags & CMD_PLANET_FLAGS_BEACON)	//Ops planetary beacon give more att/def to the guy owning the planet and less attack
	{
		deffactor *= 1.1;
		attfactor *= 0.9;
	}
	
	// CODE_ARTEFACT
  if( main2d.artefacts & ARTEFACT_2_BIT )
    deffactor *= 1.25;
	
  // main portals fleet
  if( dbUserFleetRetrieve( defid, 0, &fleet2d ) < 0 )
    return -3;

  // fleets
  attunit = fleetd.unit;
  for( a = 0 ; a < CMD_UNIT_FLEET ; a++ )
    defunit[a] = planetd.unit[a] + (int)( cover * (float)fleet2d.unit[a] );
  memcpy( attunitbase, attunit, CMD_UNIT_FLEET*sizeof(int) );
  memcpy( defunitbase, defunit, CMD_UNIT_FLEET*sizeof(int) );
  defsatsbase = defsats = planetd.building[CMD_BUILDING_SATS];
  
  shields = CMD_SHIELD_ABSORB * planetd.building[CMD_BUILDING_SHIELD]  +  specopShieldingCalc( defid, fleetd.destid );

  memset( results, 0, (4+8*CMD_UNIT_FLEET)*sizeof(int) );
  results[0] = defid;
  results[1] = main2d.empire;
  results[2] = (int)( 100.0 * cover );


  for( a = 0 ; a < CMD_UNIT_FLEET ; a++ )
  {
    for( b = 0 ; b < CMD_UNIT_STATS_BATTLE ; b++ )
    {
      attstats[a][b] = (double)(cmdUnitStats[a][b]) * cmdRace[maind.raceid].unit[a];
      defstats[a][b] = (double)(cmdUnitStats[a][b]) * cmdRace[main2d.raceid].unit[a];
    }
  }


/*** PHASE 1 ***/

  flee = 0;
  attdam = attunit[CMD_UNIT_CRUISER] * attstats[CMD_UNIT_CRUISER][CMD_UNIT_STATS_AIRATTACK]  +  attunit[CMD_UNIT_PHANTOM] * attstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_AIRATTACK];
  defdam = defunit[CMD_UNIT_CRUISER] * defstats[CMD_UNIT_CRUISER][CMD_UNIT_STATS_AIRATTACK]  +  defunit[CMD_UNIT_PHANTOM] * defstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_AIRATTACK]  +  defsats * CMD_SATS_ATTACK;
	
  attdam = attdam * attfactor * ( ( 1.0 + 0.005*maind.totalresearch[CMD_RESEARCH_MILITARY] ) / ( 1.0 + 0.005*main2d.totalresearch[CMD_RESEARCH_MILITARY] ) );
  defdam = defdam * deffactor * ( ( 1.0 + 0.005*main2d.totalresearch[CMD_RESEARCH_MILITARY] ) / ( 1.0 + 0.005*maind.totalresearch[CMD_RESEARCH_MILITARY] ) );

  if( attdam >= 1.0 )
    attdam -= attdam * ( 1.0 - pow( 2.5, -( shields / attdam ) ) );

  if( ( defdam < 1.0 ) || ( ( attdam / defdam ) * 10.0 >= main2d.config_flee[0] ) )
  {
    results[3] |= 0x100;
    goto battleDefFlee1;
  }
  if( ( attdam / defdam ) * 100.0 >= main2d.config_flee[0] )
  {
    defdam = (int)( 0.15 * defdam );
    attdam = (int)( 0.10 * attdam );
    
    results[3] |= 0x100;
  }
  if( ( attdam >= 1.0 ) && ( ( defdam / attdam ) * 100.0 >= maind.config_flee[0] ) )
  {
    defdam = (int)( 0.20 * defdam );
    attdam = (int)( 0.10 * attdam );
    flee = 1;
  }


  // attacker
  hpcarrier = attunit[CMD_UNIT_CARRIER] * attstats[CMD_UNIT_CARRIER][CMD_UNIT_STATS_AIRDEFENSE];
  /*
  //ARTI CODE
	if(maind.artefacts & ARTEFACT_SEAT_BIT)
		hpcarrier += attunit[CMD_UNIT_CARRIER] * attstats[CMD_UNIT_CARRIER][CMD_UNIT_STATS_AIRDEFENSE];
	*/	
  hpcruiser = attunit[CMD_UNIT_CRUISER] * attstats[CMD_UNIT_CRUISER][CMD_UNIT_STATS_AIRDEFENSE];
  
  hpphantom = attunit[CMD_UNIT_PHANTOM] * attstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_AIRDEFENSE];
  hptotal = hpcarrier + hpcruiser + hpphantom;
  damcarrier = damcruiser = damphantom = 0.0;
  if( hptotal )
  {
    damcarrier = (double)hpcarrier / (double)hptotal;
    damcruiser = (double)hpcruiser / (double)hptotal;
    damphantom = (double)hpphantom / (double)hptotal;
  }

  fa = 0.0;
  if( defunit[CMD_UNIT_CRUISER] )
    fa = (float)( attunit[CMD_UNIT_CRUISER] ) / (float)( defunit[CMD_UNIT_CRUISER] );
  damcarrier *= pow( 1.50, -fa );

  fb = (float)(damcarrier + damcruiser + damphantom);
  if( fb >= 0.00001 )
  {
    fa = defdam / fb;
    damcarrier *= fa;
    damcruiser *= fa;
    damphantom *= fa;
  }

  if( damcarrier > hpcarrier )
    damcruiser += damcarrier - hpcarrier;
  if( damcruiser > hpcruiser )
    damphantom += damcruiser - hpcruiser;
  if( damphantom > hpphantom )
    damcruiser += damphantom - hpphantom;
  results[4+0*CMD_UNIT_FLEET+CMD_UNIT_CARRIER] = damcarrier / attstats[CMD_UNIT_CARRIER][CMD_UNIT_STATS_AIRDEFENSE];
  results[4+0*CMD_UNIT_FLEET+CMD_UNIT_CRUISER] = damcruiser / attstats[CMD_UNIT_CRUISER][CMD_UNIT_STATS_AIRDEFENSE];
  results[4+0*CMD_UNIT_FLEET+CMD_UNIT_PHANTOM] = damphantom / attstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_AIRDEFENSE];


  // defender
  hpcruiser = defunit[CMD_UNIT_CRUISER] * defstats[CMD_UNIT_CRUISER][CMD_UNIT_STATS_AIRDEFENSE];
  hpphantom = defunit[CMD_UNIT_PHANTOM] * defstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_AIRDEFENSE];
  hpsats = defsats * CMD_SATS_DEFENCE;
  hptotal = hpcruiser + hpphantom + hpsats;
  damcruiser = damphantom = damsats = 0.0;
  if( hptotal )
  {
    damcruiser = (double)hpcruiser / (double)hptotal;
    damphantom = (double)hpphantom / (double)hptotal;
    damsats = (double)hpsats / (double)hptotal;
  }

  fa = 0.0;
  if( attunit[CMD_UNIT_CRUISER] )
    fa = (float)( defunit[CMD_UNIT_CRUISER] ) / (float)( attunit[CMD_UNIT_CRUISER] );
  damsats *= pow( 2.80, -fa );

  fb = (float)(damcruiser + damphantom + damsats);
  if( fb >= 0.00001 )
  {
    fa = attdam / fb;
    damcruiser *= fa;
    damphantom *= fa;
    damsats *= fa;
  }

  if( damcruiser > hpcruiser )
    damphantom += damcruiser - hpcruiser;
  if( damphantom > hpphantom )
    damsats += damphantom - hpphantom;
  if( damsats > hpsats )
    damcruiser += damsats - hpsats;
  results[4+1*CMD_UNIT_FLEET+CMD_UNIT_CRUISER] = damcruiser / defstats[CMD_UNIT_CRUISER][CMD_UNIT_STATS_AIRDEFENSE];
  results[4+1*CMD_UNIT_FLEET+CMD_UNIT_PHANTOM] = damphantom / defstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_AIRDEFENSE];
  a = damsats / CMD_SATS_DEFENCE;
  if( a > defsats )
    a = defsats;
  defsats -= a;
  results[4+8*CMD_UNIT_FLEET+0] = a;

  battlePhaseUpdate( attunit, &results[4+0*CMD_UNIT_FLEET] );
  battlePhaseUpdate( defunit, &results[4+1*CMD_UNIT_FLEET] );

  if( flee )
    goto battleAttFlee;
  battleDefFlee1:

/*** PHASE 1 ***/



/*** PHASE 2 ***/

  flee = 0;


  attdam = attunit[CMD_UNIT_FIGHTER] * attstats[CMD_UNIT_FIGHTER][CMD_UNIT_STATS_AIRATTACK]  +  attunit[CMD_UNIT_CRUISER] * attstats[CMD_UNIT_CRUISER][CMD_UNIT_STATS_AIRATTACK]  +  attunit[CMD_UNIT_PHANTOM] * attstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_AIRATTACK];
  defdam = defunit[CMD_UNIT_FIGHTER] * defstats[CMD_UNIT_FIGHTER][CMD_UNIT_STATS_AIRATTACK]  +  defunit[CMD_UNIT_CRUISER] * defstats[CMD_UNIT_CRUISER][CMD_UNIT_STATS_AIRATTACK]  +  defunit[CMD_UNIT_PHANTOM] * defstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_AIRATTACK]  +  defsats * CMD_SATS_ATTACK;
  
  attdam = attdam * attfactor * ( ( 1.0 + 0.005*maind.totalresearch[CMD_RESEARCH_MILITARY] ) / ( 1.0 + 0.005*main2d.totalresearch[CMD_RESEARCH_MILITARY] ) );
  defdam = defdam * deffactor * ( ( 1.0 + 0.005*main2d.totalresearch[CMD_RESEARCH_MILITARY] ) / ( 1.0 + 0.005*maind.totalresearch[CMD_RESEARCH_MILITARY] ) );

  if( attdam >= 1.0 )
    attdam -= attdam * ( 1.0 - pow( 2.5, -( shields / attdam ) ) );

  if( ( defdam < 1.0 ) || ( ( attdam / defdam ) * 10.0 >= main2d.config_flee[1] ) )
  {
    results[3] |= 0x200;
    goto battleDefFlee2;
  }
  if( ( attdam / defdam ) * 100.0 >= main2d.config_flee[1] )
  {
    defdam = (int)( 0.15 * defdam );
    attdam = (int)( 0.10 * attdam );
    results[3] |= 0x200;
  }
  if( ( attdam >= 1.0 ) && ( ( defdam / attdam ) * 100.0 >= maind.config_flee[1] ) )
  {
    defdam = (int)( 0.50 * defdam );
    attdam = (int)( 0.25 * attdam );
    flee = 2;
  }


  // attacker
  hptransport = attunit[CMD_UNIT_TRANSPORT] * attstats[CMD_UNIT_TRANSPORT][CMD_UNIT_STATS_AIRDEFENSE];
  /*
  //ARTI CODE
	if(maind.artefacts & ARTEFACT_SEAT_BIT)
		hptransport += attunit[CMD_UNIT_TRANSPORT] * attstats[CMD_UNIT_TRANSPORT][CMD_UNIT_STATS_AIRDEFENSE];
  */
  hpcruiser = attunit[CMD_UNIT_CRUISER] * attstats[CMD_UNIT_CRUISER][CMD_UNIT_STATS_AIRDEFENSE];
  hpbomber = attunit[CMD_UNIT_BOMBER] * attstats[CMD_UNIT_BOMBER][CMD_UNIT_STATS_AIRDEFENSE];
  
  hpfighter = attunit[CMD_UNIT_FIGHTER] * attstats[CMD_UNIT_FIGHTER][CMD_UNIT_STATS_AIRDEFENSE];
  hpphantom = attunit[CMD_UNIT_PHANTOM] * attstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_AIRDEFENSE];
  hptotal = hptransport + hpcruiser + hpbomber + hpfighter + hpphantom;
  damtransport = damcruiser = dambomber = damfighter = damphantom = 0.0;
  if( hptotal )
  {
    damtransport = (double)hptransport / (double)hptotal;
    damcruiser = (double)hpcruiser / (double)hptotal;
    dambomber = (double)hpbomber / (double)hptotal;
    damfighter = (double)hpfighter / (double)hptotal;
    damphantom = (double)hpphantom / (double)hptotal;
  }

  fb = (float)( defunit[CMD_UNIT_FIGHTER] + 6*defunit[CMD_UNIT_CRUISER] );
  fa = 0.0;
  if( fb >= 0.00001 )
    fa = (float)( attunit[CMD_UNIT_FIGHTER] + 6*attunit[CMD_UNIT_CRUISER] ) / fb;
  damtransport *= pow( 2.50, -fa );

  fa = 0.0;
  if( defunit[CMD_UNIT_FIGHTER] )
    fa = (float)( attunit[CMD_UNIT_FIGHTER] ) / (float)( defunit[CMD_UNIT_FIGHTER] );
  damcruiser *= pow( 1.25, -fa );

  fb = (float)( defunit[CMD_UNIT_FIGHTER] + 3*defunit[CMD_UNIT_CRUISER] );
  fa = 0.0;
  if( fb >= 0.00001 )
    fa = (float)( attunit[CMD_UNIT_FIGHTER] + 3*attunit[CMD_UNIT_CRUISER] ) / fb;
  dambomber *= pow( 1.75, -fa );

  fb = (float)(damtransport + damcruiser + dambomber + damfighter + damphantom);
  if( fb >= 0.00001 )
  {
    fa = defdam / fb;
    damtransport *= fa;
    damcruiser *= fa;
    dambomber *= fa;
    damfighter *= fa;
    damphantom *= fa;
  }

  if( damtransport > hptransport )
    damcruiser += damtransport - hptransport;
  if( damcruiser > hpcruiser )
    dambomber += damcruiser - hpcruiser;
  if( dambomber > hpbomber )
    damfighter += dambomber - hpbomber;
  if( damfighter > hpfighter )
    damphantom += damfighter - hpfighter;
  if( damphantom > hpphantom )
    damcruiser += damphantom - hpphantom;
  results[4+2*CMD_UNIT_FLEET+CMD_UNIT_TRANSPORT] = damtransport / attstats[CMD_UNIT_TRANSPORT][CMD_UNIT_STATS_AIRDEFENSE];
  results[4+2*CMD_UNIT_FLEET+CMD_UNIT_CRUISER] = damcruiser / attstats[CMD_UNIT_CRUISER][CMD_UNIT_STATS_AIRDEFENSE];
  results[4+2*CMD_UNIT_FLEET+CMD_UNIT_BOMBER] = dambomber / attstats[CMD_UNIT_BOMBER][CMD_UNIT_STATS_AIRDEFENSE];
  results[4+2*CMD_UNIT_FLEET+CMD_UNIT_FIGHTER] = damfighter / attstats[CMD_UNIT_FIGHTER][CMD_UNIT_STATS_AIRDEFENSE];
  results[4+2*CMD_UNIT_FLEET+CMD_UNIT_PHANTOM] = damphantom / attstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_AIRDEFENSE];


  // defender
  hpcruiser = defunit[CMD_UNIT_CRUISER] * defstats[CMD_UNIT_CRUISER][CMD_UNIT_STATS_AIRDEFENSE];
  hpfighter = defunit[CMD_UNIT_FIGHTER] * defstats[CMD_UNIT_FIGHTER][CMD_UNIT_STATS_AIRDEFENSE];
  hpphantom = defunit[CMD_UNIT_PHANTOM] * defstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_AIRDEFENSE];
  hpsats = defsats * CMD_SATS_DEFENCE;
  hptotal = hpcruiser + hpfighter + hpphantom + hpsats;
  damcruiser = damfighter = damphantom = damsats = 0.0;
  if( hptotal )
  {
    damcruiser = (double)hpcruiser / (double)hptotal;
    damfighter = (double)hpfighter / (double)hptotal;
    damphantom = (double)hpphantom / (double)hptotal;
    damsats = (double)hpsats / (double)hptotal;
  }

  fa = 0.0;
  if( attunit[CMD_UNIT_FIGHTER] )
    fa = (float)( defunit[CMD_UNIT_FIGHTER] ) / (float)( attunit[CMD_UNIT_FIGHTER] );
  damcruiser *= pow( 1.25, -fa );

  fb = (float)(damcruiser + damfighter + damphantom + damsats);
  if( fb >= 0.00001 )
  {
    fa = attdam / fb;
    damcruiser *= fa;
    damfighter *= fa;
    damphantom *= fa;
    damsats *= fa;
  }

  if( damcruiser > hpcruiser )
    damfighter += damcruiser - hpcruiser;
  if( damfighter > hpfighter )
    damphantom += damfighter - hpfighter;
  if( damphantom > hpphantom )
    damsats += damphantom - hpphantom;
  if( damsats > hpsats )
    damcruiser += damsats - hpsats;
  results[4+3*CMD_UNIT_FLEET+CMD_UNIT_CRUISER] = damcruiser / defstats[CMD_UNIT_CRUISER][CMD_UNIT_STATS_AIRDEFENSE];
  results[4+3*CMD_UNIT_FLEET+CMD_UNIT_FIGHTER] = damfighter / defstats[CMD_UNIT_FIGHTER][CMD_UNIT_STATS_AIRDEFENSE];
  results[4+3*CMD_UNIT_FLEET+CMD_UNIT_PHANTOM] = damphantom / defstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_AIRDEFENSE];
  a = damsats / CMD_SATS_DEFENCE;
  if( a > defsats )
    a = defsats;
  defsats -= a;
  results[4+8*CMD_UNIT_FLEET+1] = a;

  battlePhaseUpdate( attunit, &results[4+2*CMD_UNIT_FLEET] );
  battlePhaseUpdate( defunit, &results[4+3*CMD_UNIT_FLEET] );

  if( flee )
    goto battleAttFlee;
  battleDefFlee2:

/*** PHASE 2 ***/




/*** PHASE 3 ***/

  flee = 0;
  attdam = attunit[CMD_UNIT_BOMBER] * attstats[CMD_UNIT_BOMBER][CMD_UNIT_STATS_GROUNDATTACK]  +  attunit[CMD_UNIT_CRUISER] * attstats[CMD_UNIT_CRUISER][CMD_UNIT_STATS_GROUNDATTACK]  +  attunit[CMD_UNIT_PHANTOM] * attstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_GROUNDATTACK];
  defdam = defunit[CMD_UNIT_GOLIATH] * defstats[CMD_UNIT_GOLIATH][CMD_UNIT_STATS_AIRATTACK]  +  defunit[CMD_UNIT_PHANTOM] * defstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_GROUNDATTACK];

	attdam = attdam * attfactor * ( ( 1.0 + 0.005*maind.totalresearch[CMD_RESEARCH_MILITARY] ) / ( 1.0 + 0.005*main2d.totalresearch[CMD_RESEARCH_MILITARY] ) );
  defdam = defdam * deffactor * ( ( 1.0 + 0.005*main2d.totalresearch[CMD_RESEARCH_MILITARY] ) / ( 1.0 + 0.005*maind.totalresearch[CMD_RESEARCH_MILITARY] ) );

  if( attdam >= 1.0 )
    attdam -= attdam * ( 1.0 - pow( 2.5, -( shields / attdam ) ) );

  if( ( defdam < 1.0 ) || ( ( attdam / defdam ) * 10.0 >= main2d.config_flee[2] ) )
  {
    results[3] |= 0x400;
    goto battleDefFlee3;
  }
  if( ( attdam / defdam ) * 100.0 >= main2d.config_flee[2] )
  {
    defdam = (int)( 0.15 * defdam );
    attdam = (int)( 0.10 * attdam );
    results[3] |= 0x400;
  }
  if( ( attdam >= 1.0 ) && ( ( defdam / attdam ) * 100.0 >= maind.config_flee[2] ) )
  {
    defdam = (int)( 0.30 * defdam );
    attdam = (int)( 0.15 * attdam );
    flee = 4;
  }


  // attacker
  hptransport = attunit[CMD_UNIT_TRANSPORT] * attstats[CMD_UNIT_TRANSPORT][CMD_UNIT_STATS_GROUNDDEFENSE];
  hpcruiser = attunit[CMD_UNIT_CRUISER] * attstats[CMD_UNIT_CRUISER][CMD_UNIT_STATS_GROUNDDEFENSE];
	hpbomber = attunit[CMD_UNIT_BOMBER] * attstats[CMD_UNIT_BOMBER][CMD_UNIT_STATS_GROUNDDEFENSE];
  hpphantom = attunit[CMD_UNIT_PHANTOM] * attstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_GROUNDDEFENSE];
  hptotal = hptransport + hpcruiser + hpbomber + hpphantom;
  damtransport = damcruiser = dambomber = damphantom = 0;
  if( hptotal )
  {
    damtransport = (double)hptransport / (double)hptotal;
    damcruiser = (double)hpcruiser / (double)hptotal;
    dambomber = (double)hpbomber / (double)hptotal;
    damphantom = (double)hpphantom / (double)hptotal;
  }

  fa = 0.0;
  if( defunit[CMD_UNIT_FIGHTER] )
    fa = (float)( attunit[CMD_UNIT_FIGHTER] ) / (float)( defunit[CMD_UNIT_FIGHTER] );
  damtransport *= pow( 1.25, -fa );

  fa = 0.0;
  if( defunit[CMD_UNIT_FIGHTER] )
    fa = (float)( attunit[CMD_UNIT_FIGHTER] ) / (float)( defunit[CMD_UNIT_FIGHTER] );
  damcruiser *= pow( 5.00, -fa );

  fb = (float)(damtransport + damcruiser + dambomber + damphantom);
  if( fb >= 0.00001 )
  {
    fa = defdam / fb;
    damtransport *= fa;
    damcruiser *= fa;
    dambomber *= fa;
    damphantom *= fa;
  }

  if( damtransport > hptransport )
    damcruiser += damtransport - hptransport;
  if( damcruiser > hpcruiser )
    dambomber += damcruiser - hpcruiser;
  if( dambomber > hpbomber )
    damphantom += dambomber - hpbomber;
  if( damphantom > hpphantom )
    damcruiser += damphantom - hpphantom;
  results[4+4*CMD_UNIT_FLEET+CMD_UNIT_TRANSPORT] = damtransport / attstats[CMD_UNIT_TRANSPORT][CMD_UNIT_STATS_GROUNDDEFENSE];
  results[4+4*CMD_UNIT_FLEET+CMD_UNIT_CRUISER] = damcruiser / attstats[CMD_UNIT_CRUISER][CMD_UNIT_STATS_GROUNDDEFENSE];
  results[4+4*CMD_UNIT_FLEET+CMD_UNIT_BOMBER] = dambomber / attstats[CMD_UNIT_BOMBER][CMD_UNIT_STATS_GROUNDDEFENSE];
  results[4+4*CMD_UNIT_FLEET+CMD_UNIT_PHANTOM] = damphantom / attstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_GROUNDDEFENSE];


  // defender
  hpgoliath = defunit[CMD_UNIT_GOLIATH] * defstats[CMD_UNIT_GOLIATH][CMD_UNIT_STATS_AIRDEFENSE];
  hpphantom = defunit[CMD_UNIT_PHANTOM] * defstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_GROUNDDEFENSE];
  hptotal = hpgoliath + hpphantom;
  damgoliath = damphantom = 0.0;
  if( hptotal )
  {
    damgoliath = (double)hpgoliath / (double)hptotal;
    damphantom = (double)hpphantom / (double)hptotal;
  }

  damgoliath *= attdam;
  damphantom *= attdam;

  if( damgoliath > hpgoliath )
    damphantom += damgoliath - hpgoliath;
  if( damphantom > hpphantom )
    damgoliath += damphantom - hpphantom;
  results[4+5*CMD_UNIT_FLEET+CMD_UNIT_GOLIATH] = damgoliath / defstats[CMD_UNIT_GOLIATH][CMD_UNIT_STATS_AIRDEFENSE];
  results[4+5*CMD_UNIT_FLEET+CMD_UNIT_PHANTOM] = damphantom / defstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_GROUNDDEFENSE];

  battlePhaseUpdate( attunit, &results[4+4*CMD_UNIT_FLEET] );
  battlePhaseUpdate( defunit, &results[4+5*CMD_UNIT_FLEET] );

  if( flee )
    goto battleAttFlee;
  battleDefFlee3:

/*** PHASE 3 ***/




/*** PHASE 4 ***/

  flee = 0;
  attdam = attunit[CMD_UNIT_SOLDIER] * attstats[CMD_UNIT_SOLDIER][CMD_UNIT_STATS_GROUNDATTACK]  +  attunit[CMD_UNIT_DROID] * attstats[CMD_UNIT_DROID][CMD_UNIT_STATS_GROUNDATTACK]  +  attunit[CMD_UNIT_GOLIATH] * attstats[CMD_UNIT_GOLIATH][CMD_UNIT_STATS_GROUNDATTACK]  +  attunit[CMD_UNIT_PHANTOM] * attstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_GROUNDATTACK];
  /*
  //ARTI CODE
	if(maind.artefacts & ARTEFACT_BOOK_BIT)
		attdam += attunit[CMD_UNIT_SOLDIER] * attstats[CMD_UNIT_SOLDIER][CMD_UNIT_STATS_GROUNDATTACK] * 0.2;
  */
  defdam = defunit[CMD_UNIT_SOLDIER] * defstats[CMD_UNIT_SOLDIER][CMD_UNIT_STATS_GROUNDATTACK]  +  defunit[CMD_UNIT_DROID] * defstats[CMD_UNIT_DROID][CMD_UNIT_STATS_GROUNDATTACK]  +  defunit[CMD_UNIT_GOLIATH] * defstats[CMD_UNIT_GOLIATH][CMD_UNIT_STATS_GROUNDATTACK]  +  defunit[CMD_UNIT_PHANTOM] * defstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_GROUNDATTACK];
	/*
  //ARTI CODE
	if(main2d.artefacts & ARTEFACT_BOOK_BIT)
		defdam += defunit[CMD_UNIT_SOLDIER] * defstats[CMD_UNIT_SOLDIER][CMD_UNIT_STATS_GROUNDATTACK] * 0.2;
  */
  
	a = attunit[CMD_UNIT_BOMBER] * attstats[CMD_UNIT_BOMBER][CMD_UNIT_STATS_GROUNDATTACK]  +  attunit[CMD_UNIT_CRUISER] * attstats[CMD_UNIT_CRUISER][CMD_UNIT_STATS_GROUNDATTACK];
	
  if( attdam >= 1.0 )
  {
    fa = a / attdam;
    if( a < 0.5 )
      attdam += a;
    else
      attdam += 0.5 * attdam * pow( 2.0 * fa, 0.35 );
  }

  attdam = attdam * attfactor * ( ( 1.0 + 0.005*maind.totalresearch[CMD_RESEARCH_MILITARY] ) / ( 1.0 + 0.005*main2d.totalresearch[CMD_RESEARCH_MILITARY] ) );
  defdam = defdam * deffactor * ( ( 1.0 + 0.005*main2d.totalresearch[CMD_RESEARCH_MILITARY] ) / ( 1.0 + 0.005*maind.totalresearch[CMD_RESEARCH_MILITARY] ) );

  if( attdam >= 1.0 )
    attdam -= attdam * ( 1.0 - pow( 2.5, -( shields / attdam ) ) );

  if( ( defdam < 1.0 ) || ( ( attdam / defdam ) * 10.0 >= main2d.config_flee[3] ) )
  {
    results[3] |= 0x800;
    goto battleDefFlee4;
  }
  if( ( attdam / defdam ) * 100.0 >= main2d.config_flee[3] )
  {
    defdam = (int)( 0.15 * defdam );
    attdam = (int)( 0.10 * attdam );
    results[3] |= 0x800;
  }
  if( ( attdam >= 1.0 ) && ( ( defdam / attdam ) * 100.0 >= maind.config_flee[3] ) )
  {
    defdam = (int)( 0.40 * defdam );
    attdam = (int)( 0.20 * attdam );
    flee = 8;
  }


  // attacker
  hpsoldier = attunit[CMD_UNIT_SOLDIER] * attstats[CMD_UNIT_SOLDIER][CMD_UNIT_STATS_GROUNDDEFENSE];
  hpdroid = attunit[CMD_UNIT_DROID] * attstats[CMD_UNIT_DROID][CMD_UNIT_STATS_GROUNDDEFENSE];
  hpgoliath = attunit[CMD_UNIT_GOLIATH] * attstats[CMD_UNIT_GOLIATH][CMD_UNIT_STATS_GROUNDDEFENSE];
  
  hpphantom = attunit[CMD_UNIT_PHANTOM] * attstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_GROUNDDEFENSE];
  hptotal = hpsoldier + hpdroid + hpgoliath + hpphantom;
  damsoldier = damdroid = damgoliath = damphantom = 0;
  if( hptotal )
  {
    damsoldier = (double)hpsoldier / (double)hptotal;
    damdroid = (double)hpdroid / (double)hptotal;
    damgoliath = (double)hpgoliath / (double)hptotal;
    damphantom = (double)hpphantom / (double)hptotal;
  }

  fb = (float)( defunit[CMD_UNIT_SOLDIER] + defunit[CMD_UNIT_DROID] );
  fa = 0.0;
  if( fb >= 0.00001 )
    fa = (float)( attunit[CMD_UNIT_SOLDIER] + attunit[CMD_UNIT_DROID] ) / fb;
  damgoliath *= pow( 1.50, -fa );

  fb = (float)(damsoldier + damdroid + damgoliath + damphantom);
  if( fb >= 0.00001 )
  {
    fa = defdam / fb;
    damsoldier *= fa;
    damdroid *= fa;
    damgoliath *= fa;
    damphantom *= fa;
  }

  if( damsoldier > hpsoldier )
    damdroid += damsoldier - hpsoldier;
  if( damdroid > hpdroid )
    damgoliath += damdroid - hpdroid;
  if( damgoliath > hpgoliath )
    damsoldier += damgoliath - hpgoliath;
  if( damsoldier > hpsoldier )
    damphantom += damsoldier - hpsoldier;
  if( damphantom > hpphantom )
    damdroid += damphantom - hpphantom;
  results[4+6*CMD_UNIT_FLEET+CMD_UNIT_SOLDIER] = damsoldier / attstats[CMD_UNIT_SOLDIER][CMD_UNIT_STATS_GROUNDDEFENSE];
  results[4+6*CMD_UNIT_FLEET+CMD_UNIT_DROID] = damdroid / attstats[CMD_UNIT_DROID][CMD_UNIT_STATS_GROUNDDEFENSE];
  results[4+6*CMD_UNIT_FLEET+CMD_UNIT_GOLIATH] = damgoliath / attstats[CMD_UNIT_GOLIATH][CMD_UNIT_STATS_GROUNDDEFENSE];
  results[4+6*CMD_UNIT_FLEET+CMD_UNIT_PHANTOM] = damphantom / attstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_GROUNDDEFENSE];


  // defender
  hpsoldier = defunit[CMD_UNIT_SOLDIER] * defstats[CMD_UNIT_SOLDIER][CMD_UNIT_STATS_GROUNDDEFENSE];
  hpdroid = defunit[CMD_UNIT_DROID] * defstats[CMD_UNIT_DROID][CMD_UNIT_STATS_GROUNDDEFENSE];
 	hpgoliath = defunit[CMD_UNIT_GOLIATH] * defstats[CMD_UNIT_GOLIATH][CMD_UNIT_STATS_GROUNDDEFENSE];

  hpphantom = defunit[CMD_UNIT_PHANTOM] * defstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_GROUNDDEFENSE];
  hptotal = hpsoldier + hpdroid + hpgoliath + hpphantom;
  damsoldier = damdroid = damgoliath = damphantom = 0;
  if( hptotal )
  {
    damsoldier = (double)hpsoldier / (double)hptotal;
    damdroid = (double)hpdroid / (double)hptotal;
    damgoliath = (double)hpgoliath / (double)hptotal;
    damphantom = (double)hpphantom / (double)hptotal;
  }

  fb = (float)( attunit[CMD_UNIT_SOLDIER] + attunit[CMD_UNIT_DROID] );
  fa = 0.0;
  if( fb >= 0.00001 )
    fa = (float)( defunit[CMD_UNIT_SOLDIER] + defunit[CMD_UNIT_DROID] ) / fb;
  damgoliath *= pow( 1.50, -fa );

  fb = (float)(damsoldier + damdroid + damgoliath + damphantom);
  if( fb >= 0.00001 )
  {
    fa = attdam / fb;
    damsoldier *= fa;
    damdroid *= fa;
    damgoliath *= fa;
    damphantom *= fa;
  }

  if( damsoldier > hpsoldier )
    damdroid += damsoldier - hpsoldier;
  if( damdroid > hpdroid )
    damgoliath += damdroid - hpdroid;
  if( damgoliath > hpgoliath )
    damsoldier += damgoliath - hpgoliath;
  if( damsoldier > hpsoldier )
    damphantom += damsoldier - hpsoldier;
  if( damphantom > hpphantom )
    damdroid += damphantom - hpphantom;
  results[4+7*CMD_UNIT_FLEET+CMD_UNIT_SOLDIER] = damsoldier / defstats[CMD_UNIT_SOLDIER][CMD_UNIT_STATS_GROUNDDEFENSE];
  results[4+7*CMD_UNIT_FLEET+CMD_UNIT_DROID] = damdroid / defstats[CMD_UNIT_DROID][CMD_UNIT_STATS_GROUNDDEFENSE];
  results[4+7*CMD_UNIT_FLEET+CMD_UNIT_GOLIATH] = damgoliath / defstats[CMD_UNIT_GOLIATH][CMD_UNIT_STATS_GROUNDDEFENSE];
  results[4+7*CMD_UNIT_FLEET+CMD_UNIT_PHANTOM] = damphantom / defstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_GROUNDDEFENSE];

  battlePhaseUpdate( attunit, &results[4+6*CMD_UNIT_FLEET] );
  battlePhaseUpdate( defunit, &results[4+7*CMD_UNIT_FLEET] );

  if( flee )
    goto battleAttFlee;
  battleDefFlee4:

/*** PHASE 4 ***/




  battleAttFlee:
  if( flee )
    results[3] |= flee << 4;

  for( a = b = stationleft = 0 ; a < CMD_UNIT_FLEET ; a++ )
  {
    fleetd.unit[a] = attunit[a];
    b |= fleetd.unit[a];
    planetd.unit[a] -= defunitbase[a] - defunit[a];
    if( planetd.unit[a] < 0 )
    {
      fleet2d.unit[a] += planetd.unit[a];
      planetd.unit[a] = 0;
    }
    stationleft |= planetd.unit[a];
  }

  if( defsats < 0 )
  {
    defsats = 0;
    printf( "CRAP!!!\n" );
  }

  planetd.building[CMD_BUILDING_SATS] = defsats;



  if( b )
    dbUserFleetSet( id, fltid, &fleetd );
  else
    dbUserFleetRemove( id, fltid );
  dbUserFleetSet( defid, 0, &fleet2d );
  dbMapSetPlanet( fleetd.destid, &planetd );

  attdam = attunit[CMD_UNIT_SOLDIER] * attstats[CMD_UNIT_SOLDIER][CMD_UNIT_STATS_GROUNDDEFENSE]  +  attunit[CMD_UNIT_DROID] * attstats[CMD_UNIT_DROID][CMD_UNIT_STATS_GROUNDDEFENSE]  +  attunit[CMD_UNIT_GOLIATH] * attstats[CMD_UNIT_GOLIATH][CMD_UNIT_STATS_GROUNDDEFENSE]  +  attunit[CMD_UNIT_PHANTOM] * attstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_GROUNDDEFENSE];
  defdam = defunit[CMD_UNIT_SOLDIER] * defstats[CMD_UNIT_SOLDIER][CMD_UNIT_STATS_GROUNDDEFENSE]  +  defunit[CMD_UNIT_DROID] * defstats[CMD_UNIT_DROID][CMD_UNIT_STATS_GROUNDDEFENSE]  +  defunit[CMD_UNIT_GOLIATH] * defstats[CMD_UNIT_GOLIATH][CMD_UNIT_STATS_GROUNDDEFENSE]  +  defunit[CMD_UNIT_PHANTOM] * defstats[CMD_UNIT_PHANTOM][CMD_UNIT_STATS_GROUNDDEFENSE];

	if( results[3] & 0x800 )
    defdam = 1;

  newd[0] = svTickNum;
  newd[1] = CMD_NEWS_FLAGS_NEW;
  newd[3] = id;
  newd[4] = maind.empire;
  newd[5] = fleetd.destid;
  newd[6] = fleetd.destination;
  newd[7] = results[3];

  for( a = 0 ; a < CMD_UNIT_FLEET ; a++ )
  {
    newd[8+a] = defunitbase[a] - defunit[a];
    main2d.totalunit[a] -= defunitbase[a] - defunit[a];
  }
  for( a = 0 ; a < CMD_UNIT_FLEET ; a++ )
  {
    newd[8+CMD_UNIT_FLEET+a] = attunitbase[a] - attunit[a];
    maind.totalunit[a] -= attunitbase[a] - attunit[a];		
  }
  newd[8+2*CMD_UNIT_FLEET] = defsatsbase - defsats;
	
	fb = specopDarkWebCalc( defid );
	
	if((cmdRace[maind.raceid].special & CMD_RACE_SPECIAL_IDW)||(planetd.flags & CMD_PLANET_FLAGS_BEACON))
    	fb = 1.0;
  
		
  a = (int)( (float)battleReadinessLoss( &maind, &main2d ) * fb );
  if( ( a >= 20*65536 ) && ( artefactPrecense( &planetd ) >= 0 ) )
    a = 20*65536;
  maind.readiness[0] -= a;

  if( ( flee ) || ( attdam <= defdam ) )
  {
    dbUserMainSet( id, &maind );
    dbUserMainSet( defid, &main2d );
    newd[2] = CMD_NEWS_ATTACK_FAILED;
    cmdUserNewsAdd( defid, newd, CMD_NEWS_FLAGS_ATTACK );
    newd[2] = CMD_NEWS_FAMATTACK_FAILED;
    newd[3] = defid;
    newd[4] = main2d.empire;
    cmdEmpireNewsAdd( maind.empire, id, newd );
    if( maind.config_fleet == 2 )
    {
      b = cmdFindDistPortal( id, ( fleetd.destination >> 8 ) & 0xFFF, fleetd.destination >> 20, &a, &fleetd.source );
      if( ( b >= -1 ) && ( ( ( a / CMD_FLEETS_TRAVEL_SPEED ) >> 8 ) == 0 ) )
      {
        fleetd.time = -1;
        fleetd.order = CMD_FLEET_ORDER_RECALL;
        dbUserFleetSet( id, fltid, &fleetd );
        cmdFleetAction( &fleetd, id, fltid, 0 );
      }
    }
    return 1;
  }
  maind.planets++;
  main2d.planets--;

  newd[2] = CMD_NEWS_ATTACK;
  cmdUserNewsAdd( defid, newd, CMD_NEWS_FLAGS_ATTACK );
  newd[2] = CMD_NEWS_FAMATTACK;
  newd[3] = defid;
  newd[4] = main2d.empire;
  cmdEmpireNewsAdd( maind.empire, id, newd );
  results[3] |= 1;


  if( planetd.construction )
  {
    b = dbUserBuildList( defid, &buildd );
    for( a = b-1 ; a >= 0 ; a-- )
    {
      if( buildd[a].plnid == fleetd.destid )
        dbUserBuildRemove( defid, a );
    }
    if( buildd )
      free( buildd );
  }

  if( stationleft )
  {
    memcpy( fleet2d.unit, planetd.unit, 16*sizeof(int) );
    fleet2d.order = CMD_FLEET_ORDER_CANCELED;
    fleet2d.destination = planetd.position;
    fleet2d.destid = fleetd.destid;
    fleet2d.sysid = planetd.system;
    fleet2d.source = planetd.position;
    fleet2d.time = 0;
    fleet2d.flags = 0;
    dbUserFleetAdd( defid, &fleet2d );
  }

  if( !( dbUserPlanetRemove( defid, fleetd.destid ) ) )
    return -3;
  planetd.maxpopulation = (float)( planetd.size * CMD_POPULATION_SIZE_FACTOR );
  planetd.construction = 0;
  planetd.population = planetd.size * CMD_POPULATION_BASE_FACTOR;

  if( planetd.flags & CMD_PLANET_FLAGS_HOME )
  {
    memset( planetd.building, 0, CMD_BLDG_NUMUSED*sizeof(int) );
    memset( planetd.unit, 0, CMD_UNIT_NUMUSED*sizeof(int) );
    planetd.owner = -1;
    dbMapSetPlanet( fleetd.destid, &planetd );
    dbUserMainSet( id, &maind );
    dbUserMainSet( defid, &main2d );
    if( !( main2d.planets ) )
      cmdExecUserDeactivate( defid, CMD_USER_FLAGS_KILLED );
    return 1;
  }
  planetd.owner = id;
  planetd.flags = 0;
  dbUserPlanetAdd( id, fleetd.destid, fleetd.sysid, fleetd.destination, 0 );

  fa = (float)((maind.networth) / (main2d.networth));
  if( fa >= 1.5 )
  {
    for( a = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
      main2d.totalbuilding[a] -= planetd.building[a];
    memset( planetd.building, 0, CMD_BLDG_NUMUSED*sizeof(int) );
  }
  else
  {
    fa = 2.0 * ( 1.5 - fa );
    if( fa >= 0.9 )
      fa = 0.9;
    for( a = 0 ; a < CMD_BLDG_NUMUSED ; a++ )
    {
      main2d.totalbuilding[a] -= planetd.building[a];
      planetd.building[a] = (int)floor( fa * (float)planetd.building[a] );
      maind.totalbuilding[a] += planetd.building[a];
    }
  }
  memset( planetd.unit, 0, CMD_UNIT_NUMUSED*sizeof(int) );

  fleetd.flags = 0;
  fleetd.time = -1;
  if( maind.config_fleet == 0 )
  {
    dbUserFleetRemove( id, fltid );
    memcpy( planetd.unit, fleetd.unit, CMD_UNIT_FLEET*sizeof(int) );
  }
  else if( maind.config_fleet == 1 )
  {
    fleetd.order = CMD_FLEET_ORDER_CANCELED;
    dbUserFleetSet( id, fltid, &fleetd );
  }
  else if( maind.config_fleet == 2 )
  {
    b = cmdFindDistPortal( id, ( fleetd.destination >> 8 ) & 0xFFF, fleetd.destination >> 20, &a, &fleetd.source );
    if( ( b >= -1 ) && ( ( ( a / CMD_FLEETS_TRAVEL_SPEED ) >> 8 ) == 0 ) )
    {
      dbUserFleetSet( id, fltid, &fleetd );
      fleetd.order = CMD_FLEET_ORDER_RECALL;
      cmdFleetAction( &fleetd, id, fltid, 0 );
    }
    else
    {
      fleetd.order = CMD_FLEET_ORDER_CANCELED;
      dbUserFleetSet( id, fltid, &fleetd );
    }
  }
  dbMapSetPlanet( fleetd.destid, &planetd );

  dbUserMainSet( id, &maind );
  dbUserMainSet( defid, &main2d );

  return 1;
}





/*
results
1 : defender id
1 : defender empire
1 : defender portal coverage
1 : results

*/


