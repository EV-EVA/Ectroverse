
enum
{
EVM_NULL=0,
EVM_WELCOME,
EVM_LOGIN,
EVM_QUIT,
EVM_STATUS,
EVM_MAP,
EVM_PLAYER,
EVM_SEARCH,
EVM_FAMILY,
EVM_TOTAL
};

#define EVMSIZE (0)
#define EVMMSG (1)
#define EVM (2)



void InitEvm()
{
  return;
}

void EndEvm()
{
  return;
}


void inNewEvm( svConnectionPtr cnt )
{
  if( !( cnt->iodata = malloc( sizeof(ioevmDataDef) ) ) )
  {
    cnt->flags |= SV_FLAGS_TO_CLOSE;
    return;
  }
  memset( cnt->iodata, 0, sizeof(ioevmDataDef) );

  cnt->flags |= SV_FLAGS_NEED_WRITE;

  return;
}


void inNewDataEvm( svConnectionPtr cnt )
{
  int isize;
  ioevmDataPtr evm;
  evm = cnt->iodata;

  if( cnt->recv_pos < 4 )
    return;
  isize = ((int *)cnt->recv)[0];
  if( (unsigned int)isize > 65536 )
  {
    cnt->flags |= SV_FLAGS_TO_CLOSE;

printf( "Error EVM isize: %d\n", isize );

    return;
  }
  if( cnt->recv_pos < isize+4 )
    return;

  cnt->flags |= SV_FLAGS_NEED_WRITE;
  return;
}


void outSendReplyEvm( svConnectionPtr cnt )
{
  int a, b, num, id, isize, evmsg, stmsg[64];
  unsigned char pass[32];
  unsigned char buf0[64], buf1[64];
  int *idata;
  unsigned char *bdata;
  ioevmDataPtr evm;
  dbUserPtr user, userfound;
  dbMainSystemDef systemd;
  dbUserMainDef maind;
  dbMainEmpireDef empired;
  int *plist;

  evm = cnt->iodata;
  if( !( evm->status ) )
  {
    stmsg[EVMSIZE] = 8;
    stmsg[EVMMSG] = EVM_WELCOME;
    a = 1 + sprintf( (unsigned char *)(&stmsg[EVM+1]), "Welcome to the EVmap Server v0.0" );
    stmsg[EVM+0] = a;
    stmsg[EVMSIZE] += a;
    svSend( cnt, stmsg, 4+stmsg[EVMSIZE] );
    evm->status = 1;
    return;
  }

  if( evm->status == 2 )
  {
    stmsg[EVMSIZE] = 8;
    stmsg[EVMMSG] = EVM_QUIT;
    stmsg[EVM+0] = 0;
    svSend( cnt, stmsg, 3*sizeof(int) );
    evm->status = 3;
    return;
  }

  for( ; ; )
  {
    if( cnt->recv_pos < 4 )
      break;
    isize = ((int *)cnt->recv)[0];
    if( (unsigned int)(isize) > 65536 )
    {
      cnt->flags |= SV_FLAGS_TO_CLOSE;

printf( "Error EVM isize: %d\n", isize );

      return;
    }
    if( cnt->recv_pos < isize+4 )
      break;
    evmsg = ((int *)cnt->recv)[1];
    idata = &((int *)cnt->recv)[2];
    bdata = (unsigned char *)idata;


    if( evmsg == EVM_NULL )
    {
      if( isize != 4 )
        goto next;
    }
    else if( evmsg == EVM_LOGIN )
    {
      if( isize != 68 )
        goto next;
      bdata[31] = bdata[63] = 0;

printf( "LOGIN\n" );

      if( ( id = dbUserSearch( &bdata[0] ) ) < 0 )
        goto loginfailed;
      if( dbUserRetrievePassword( id, pass ) < 0 )
        goto loginfailed;
      if( !( ioCompareExact( &bdata[32], pass ) ) )
        goto loginfailed;
      if( dbUserLinkDatabase( cnt, id ) < 0 )
        goto loginfailed;

      stmsg[0] = 8;
      stmsg[1] = EVM_LOGIN;
      stmsg[2] = id;
      svSend( cnt, stmsg, 3*sizeof(int) );
      goto next;

      loginfailed:
      stmsg[0] = 8;
      stmsg[1] = EVM_LOGIN;
      stmsg[2] = -1;
      svSend( cnt, stmsg, 3*sizeof(int) );
    }
    else if( evmsg == EVM_STATUS )
    {
      if( isize != 4 )
        goto next;

printf( "STATUS\n" );

      stmsg[0] = 24;
      stmsg[1] = EVM_STATUS;
      stmsg[2] = ROUND_ID;
      stmsg[3] = svTickStatus;
      stmsg[4] = svTickNum;
      stmsg[5] = (int)( svTickTime - time(0) );
      stmsg[6] = SV_TICK_TIME;
      svSend( cnt, stmsg, 7*sizeof(int) );
    }
    else if( evmsg == EVM_MAP )
    {
      if( !( cnt->dbuser ) )
        goto next;
      if( isize != 4 )
        goto next;

printf( "MAP\n" );

      stmsg[0] = 7*sizeof(int) + dbMapBInfoStatic[2] * 5*sizeof(int);
      stmsg[1] = EVM_MAP;
      stmsg[2] = 0;
      stmsg[3] = dbMapBInfoStatic[0];
      stmsg[4] = dbMapBInfoStatic[1];
      stmsg[5] = dbMapBInfoStatic[2];
      stmsg[6] = dbMapBInfoStatic[3];
      stmsg[7] = dbMapBInfoStatic[4];
      svSend( cnt, stmsg, 8*sizeof(int) );
      for( a = 0 ; a < dbMapBInfoStatic[2] ; a++ )
      {
        dbMapRetrieveSystem( a, &systemd );
        stmsg[0] = systemd.position;
        stmsg[1] = systemd.indexplanet;
        stmsg[2] = systemd.numplanets;
        stmsg[3] = systemd.empire;
        stmsg[4] = systemd.unexplored;
        svSend( cnt, stmsg, 5*sizeof(int) );
      }
    }
    else if( evmsg == EVM_PLAYER )
    {
      if( !( cnt->dbuser ) )
        goto next;
      if( isize != 8 )
        goto next;

printf( "PLAYER %d\n", idata[0] );

      stmsg[1] = EVM_PLAYER;
      stmsg[2] = idata[0];
      if( ( dbUserMainRetrieve( idata[0], &maind ) < 0 ) || ( maind.planets <= 0 ) || ( ( num = dbUserPlanetListFull( idata[0], &plist ) ) < 0 ) )
      {
        stmsg[0] = 4 + 48 + 0;
        svSend( cnt, stmsg, 3*sizeof(int) );
        memset( stmsg, 0, 32+12 );
        svSend( cnt, stmsg, 32+3*sizeof(int) );
      }
      else
      {
        stmsg[0] = 4 + 48 + 4*num*sizeof(int);
        svSend( cnt, stmsg, 3*sizeof(int) );
        svSend( cnt, maind.faction, 32 );
        stmsg[0] = maind.empire;
        stmsg[1] = num;
        stmsg[2] = (int)maind.networth;
        svSend( cnt, stmsg, 3*sizeof(int) );
        if( idata[0] != (cnt->dbuser)->id )
        {
          for( a = 0, b = 3 ; a < num ; a++, b += 4 )
            plist[b] = 0;
        }
        svSend( cnt, plist, 4*num*sizeof(int) );
        free( plist );
      }
    }
    else if( evmsg == EVM_SEARCH )
    {
      if( !( cnt->dbuser ) )
        goto next;
      if( isize != 4+32 )
        goto next;

printf( "SEARCH %s\n", bdata );

      iohttpCaseLow( buf0, bdata );
      a = 0;
      userfound = 0;
      for( user = dbUserList ; user ; user = user->next )
      {
        if( !( user->flags & CMD_USER_FLAGS_ACTIVATED ) )
          continue;
        iohttpCaseLow( buf1, user->faction );
        if( !( ioCompareFindWords( buf1, buf0 ) ) )
          continue;
        userfound = user;
        a++;
        if( !( ioCompareExact( bdata, user->faction ) ) )
          continue;
        goto searchfound;
      }
      if( a != 1 )
        goto next;

      searchfound:
      stmsg[0] = 4 + 32 + 4;
      stmsg[1] = EVM_SEARCH;
      svSend( cnt, stmsg, 2*sizeof(int) );
      svSend( cnt, bdata, 32 );
      svSend( cnt, &userfound->id, sizeof(int) );
    }
    else if( evmsg == EVM_FAMILY )
    {
      if( !( cnt->dbuser ) )
        goto next;
      if( isize != 8 )
        goto next;

printf( "FAMILY %d\n", idata[0] );

      if( dbMapRetrieveEmpire( idata[0], &empired ) < 0 )
        goto next;

      stmsg[0] = 4 + 8 + empired.numplayers*sizeof(int) + 84;
      stmsg[1] = EVM_FAMILY;
      stmsg[2] = idata[0];
      stmsg[3] = empired.numplayers;
      svSend( cnt, stmsg, 4*sizeof(int) );
      svSend( cnt, empired.player, empired.numplayers*sizeof(int) );
      stmsg[0] = empired.homeid;
      stmsg[1] = empired.leader;
      stmsg[2] = empired.planets;
      stmsg[3] = empired.networth;
      stmsg[4] = empired.artefacts;
      svSend( cnt, stmsg, 5*sizeof(int) );
      svSend( cnt, &empired.name, 64 );
    }


    next:

    a = cnt->recv_pos - ( isize + 4 );
    if( a )
      memmove( cnt->recv, &cnt->recv[ isize + 4 ], a );
    cnt->recv_pos -= isize + 4;
  }

  return;
}


void inSendCompleteEvm( svConnectionPtr cnt )
{
  ioevmDataPtr evm;
  evm = cnt->iodata;
  if( evm->status == 3 )
    cnt->flags |= SV_FLAGS_TO_CLOSE;
  return;
}


void inClosedEvm( svConnectionPtr cnt )
{
  ioevmDataPtr evm;
  evm = cnt->iodata;
  if( evm )
  {
    free( evm );
    cnt->iodata = 0;
  }
  return;
}


void inErrorEvm( svConnectionPtr cnt, int type )
{
  ioevmDataPtr evm;
  evm = cnt->iodata;
  if( type == 0 )
  {
    evm->status = 2;
    cnt->flags |= SV_FLAGS_NEED_WRITE;
  }
  return;
}



void TickStartEvm()
{
  return;
}


void TickEndEvm()
{
  return;
}


