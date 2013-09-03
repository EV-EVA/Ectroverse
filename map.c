
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <fcntl.h>
#include <math.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>



/*
#define MAP_SIZEX (100)
#define MAP_SIZEY (100)
#define MAP_SYSTEMS (1100)
#define MAP_FAMILIES (100)
#define MAP_FAMMEMBERS (7)

#define MAP_GENBORDER (20)
#define MAP_GENANGLEVAR (1024.0)

#define MAP_GEN_LNKNUM (80)
#define MAP_GEN_LNKRADIUS (8.0)
#define MAP_GEN_LNKLENGHTBASE (2)
#define MAP_GEN_LNKLENGHTVAR (48)
*/


#define MAP_SIZEX (105)
#define MAP_SIZEY (105)
#define MAP_SYSTEMS (709)

#define MAP_FAMILIES (61)
#define MAP_FAMMEMBERS (7)

#define MAP_ARTEFACTS (9)

#define MAP_GENBORDER (20)
#define MAP_GENANGLEVAR (1024.0)

#define MAP_GEN_LNKNUM (60)
#define MAP_GEN_LNKRADIUS (8.0)
#define MAP_GEN_LNKLENGHTBASE (2)
#define MAP_GEN_LNKLENGHTVAR (64)



#define MAP_RESOURCES (25+20+10+8)

// solar, mineral, crystal, ectrolium
int map_resources_gen[4] = { 24, 23, 17, 10 };
int map_resources_posx[MAP_RESOURCES];
int map_resources_posy[MAP_RESOURCES];
int map_resources_type[MAP_RESOURCES];




int mapdata[MAP_SIZEX*MAP_SIZEY];
int system_pos[MAP_SYSTEMS];
int system_planets[MAP_SYSTEMS];
int system_pbase[MAP_SYSTEMS];
int system_home[MAP_SYSTEMS];


int empire_system[MAP_FAMILIES];

int artefact_planet[MAP_ARTEFACTS];



#define CMD_PLANET_FLAGS_HOME 4
#define BASE_PATH "data/"



int mapfactor[MAP_SIZEX*MAP_SIZEY];


void mapGetRandnorm( float *fvars )
{
  float norm;
  for( ; ; )
  {
    fvars[0] = (float)( ( rand() & 0xFF ) - 0x80 );
    fvars[1] = (float)( ( rand() & 0xFF ) - 0x80 );
    norm = sqrt( fvars[0]*fvars[0] + fvars[1]*fvars[1] );
    if( norm < 10.0 )
      continue;
    fvars[0] /= norm;
    fvars[1] /= norm;
    break;
  }
  return;
}


#define DEF_PI 3.14159265358979323846
#define ANG_SIN(x) sin((x*2*DEF_PI)/360.0)
#define ANG_COS(x) cos((x*2*DEF_PI)/360.0)

void mapCalcFactors()
{
  int a, b, c, x, y, index;
  float fx, fy, dist, mindist;
  float fpos[2];
  float angle, anglevar;
  float fdir[2];
  float pts[65536][2];
  int ptsnum;
  FILE *file;

  ptsnum = 0;
  memset( mapfactor, 0, sizeof(int)*MAP_SIZEX*MAP_SIZEY );
  for( a = 0 ; a < MAP_GEN_LNKNUM ; a++ )
  {
    fpos[0] = (float)( MAP_GENBORDER + ( rand() % ( MAP_SIZEX-2*MAP_GENBORDER ) ) );
    fpos[1] = (float)( MAP_GENBORDER + ( rand() % ( MAP_SIZEY-2*MAP_GENBORDER ) ) );
    angle = rand() % 360;
    anglevar = (float)( ( rand() & 0xFFF ) - 0x800 ) / MAP_GENANGLEVAR;

    c = MAP_GEN_LNKLENGHTBASE + ( rand() % MAP_GEN_LNKLENGHTVAR );
    for( b = 0 ; b < c ; b++ )
    {
      pts[ptsnum+b][0] = fpos[0];
      pts[ptsnum+b][1] = fpos[1];
      fdir[0] = ANG_COS( angle );
      fdir[1] = ANG_SIN( angle );
      fpos[0] += fdir[0];
      fpos[1] += fdir[1];
      angle += anglevar;
    }
    ptsnum += c;
  }

  for( y = index = 0 ; y < MAP_SIZEY ; y++ )
  {
    for( x = 0 ; x < MAP_SIZEY ; x++, index++ )
    {
      mindist = 0;
      for( a = 0 ; a < ptsnum ; a++ )
      {
        fx = (float)x - pts[a][0];
        fy = (float)y - pts[a][1];
        dist = sqrt( fx*fx + fy*fy );
        if( dist >= MAP_GEN_LNKRADIUS )
          continue;
        mindist += MAP_GEN_LNKRADIUS - dist;
      }
      mapfactor[index] += (int)floor( mindist );
    }
  }

  file = fopen( "zzz.raw", "wb" );
  for( a = 0 ; a < MAP_SIZEX*MAP_SIZEY ; a++ )
  {
    if( mapfactor[a] > 0xFF )
      mapfactor[a] = 0xFF;
    fputc( mapfactor[a], file );
  }
  fclose( file );

  return;
}






unsigned char nullb[256];

int main()
{
  int a, b, c, d, e, i, p, x, y, x2, y2;
  long long int j;
  float dist, distmax;
  FILE *file;
  unsigned char fname[32];
  FILE *file2;

  srand( time( 0 ) );

  mapCalcFactors();

  for( a = 0 ; a < MAP_FAMILIES ; a++ )
  {
    mainL1:
    empire_system[a] = rand() % MAP_SYSTEMS;
    if( system_home[ empire_system[a] ] )
      goto mainL1;
    system_home[ empire_system[a] ] = a+1;
    system_planets[ empire_system[a] ] = MAP_FAMMEMBERS;
  }

  for( a = b = c = 0 ; a < MAP_RESOURCES ; a++, b++ )
  {
    map_resources_posx[a] = rand() % MAP_SIZEX;
    map_resources_posy[a] = rand() % MAP_SIZEY;
    if( b >= map_resources_gen[c] )
    {
      b -= map_resources_gen[c];
      c++;
    }
    map_resources_type[a] = c;
  }

  p = 0;
  for( a = 0 ; a < MAP_SYSTEMS ; a++ )
  {
    mainL0:
    for( ; ; )
    {
      x = rand() % MAP_SIZEX;
      y = rand() % MAP_SIZEY;
      i = ( y * MAP_SIZEX ) + x;
      if( ( rand() & 0xFF ) >= mapfactor[i] )
        continue;
      break;
    }
    if( mapdata[i] )
      goto mainL0;
    system_pos[a] = ( y << 16 ) + x;
    if( !( system_planets[a] ) )
    {

      system_planets[a] = 4 + rand() % 4;
      if( !( rand() & 7 ) )
        system_planets[a] += rand() % 12;
      if( !( rand() & 7 ) )
        system_planets[a] += rand() % 8;

    }
    system_pbase[a] = p;
    p += system_planets[a];
    mapdata[i] = 1;
  }


  for( a = 0 ; a < MAP_ARTEFACTS ; a++ )
  {
    mainL2:
    b = rand() % MAP_SYSTEMS;
    if( system_home[b] )
      goto mainL2;
    artefact_planet[a] = system_pbase[b] + ( rand() % system_planets[b] );
    printf( "%d->%d ( %d,%d )\n", a, artefact_planet[a], system_pos[b] & 0xFFFF, system_pos[b] >> 16 );
  }


  // headers
  file = fopen( BASE_PATH "map", "wb" );
  a = MAP_SIZEX;
  fwrite( &a, 1, sizeof(int), file );
  a = MAP_SIZEY;
  fwrite( &a, 1, sizeof(int), file );
  a = MAP_SYSTEMS;
  fwrite( &a, 1, sizeof(int), file );
  fwrite( &p, 1, sizeof(int), file );
  a = MAP_FAMILIES;
  fwrite( &a, 1, sizeof(int), file );
  a = MAP_FAMMEMBERS;
  fwrite( &a, 1, sizeof(int), file );
  a = MAP_FAMILIES * MAP_FAMMEMBERS;
  fwrite( &a, 1, sizeof(int), file );
  fwrite( nullb, 1, 32, file );

  // systems
  p = 0;
  for( a = 0 ; a < MAP_SYSTEMS ; a++ )
  {
    fwrite( &(system_pos[a]), 1, sizeof(int), file );
    fwrite( &p, 1, sizeof(int), file );
    p += system_planets[a];
    fwrite( &system_planets[a], 1, sizeof(int), file );
    if( system_home[a] )
      i = system_home[a] - 1;
    else
      i = -1;
    fwrite( &i, 1, sizeof(int), file );
    if( !( system_home[a] ) )
      fwrite( &system_planets[a], 1, sizeof(int), file );
    else
      fwrite( nullb, 1, sizeof(int), file );
  }

  // planets
  for( a = b = c = 0 ; a < p ; a++, b++ )
  {
    if( b >= system_planets[c] )
    {
      b -= system_planets[c];
      c++;
    }
    fwrite( &c, 1, sizeof(int), file );
    x = system_pos[c] & 0xFFFF;
    y = system_pos[c] >> 16;
    i = ( y << 20 ) + ( x << 8 ) + b;
    fwrite( &i, 1, sizeof(int), file );
    i = -1;
    fwrite( &i, 1, sizeof(int), file );
    i = 450;
    if( !( system_home[c] ) )
    {
      i = 128 + ( rand() % 192 );
      if( !( rand() & 7 ) )
        i += rand() & 255;
      if( !( rand() & 31 ) )
        i += rand() & 511;
    }
    fwrite( &i, 1, sizeof(int), file );
    i = 0;
    if( system_home[c] )
      i = CMD_PLANET_FLAGS_HOME;
    fwrite( &i, 1, sizeof(int), file );
    i = 5000;
    fwrite( &i, 1, sizeof(int), file );
    i = 90000;
    fwrite( &i, 1, sizeof(int), file );


    // resource bonus
/*
    i = 0;
    fwrite( &i, 1, sizeof(int), file );
    fwrite( &i, 1, sizeof(int), file );
*/

    e = i = 0;
    if( !( system_home[c] ) )
    {
      distmax = (float)0xFFFF;
      for( d = 0 ; d < MAP_RESOURCES ; d++ )
      {
        x2 = x - map_resources_posx[d];
        y2 = y - map_resources_posy[d];
        dist = sqrt( x2*x2 + y2*y2 );
        if( dist < distmax )
        {
          distmax = dist;
          e = map_resources_type[d];
        }
      }
      d = 160.0 * dist;
      if( !( d ) || ( ( rand() % d ) < 1024 ) )
      {
        i = 25 + rand() % 25;
        if( !( rand() & 7 ) )
          i += rand() % 100;
        if( !( rand() & 15 ) )
          i += rand() % 100;
      }
    }
    fwrite( &e, 1, sizeof(int), file );
    fwrite( &i, 1, sizeof(int), file );



    // artefacts
    i = 0;
    for( d = 0 ; d < MAP_ARTEFACTS ; d++ )
    {
      if( a != artefact_planet[d] )
        continue;
      i = d + 1;
      break;
    }
    fwrite( &i, 1, sizeof(int), file );



    fwrite( nullb, 1, 64+64+4+4, file );



    i = -1; // surrender
    fwrite( &i, 1, 4, file );

    i = 0;
    fwrite( &i, 1, 4, file );
  }

  // families
  for( a = 0 ; a < MAP_FAMILIES ; a++ )
  {
    i = 0;
    fwrite( &i, 1, sizeof(int), file );
    fwrite( nullb, 1, 32*sizeof(int), file );
    fwrite( &empire_system[a], 1, sizeof(int), file );
    fwrite( &system_pos[ empire_system[a] ], 1, sizeof(int), file );
    fwrite( nullb, 1, 64, file );
    i = -1;
    fwrite( &i, 1, sizeof(int), file );
    for( b = 0 ; b < 32 ; b++ )
      fputc( -1, file );
    fwrite( nullb, 1, 96, file );

    sprintf( fname, BASE_PATH "fam%dnews", a );
    file2 = fopen( fname, "wb" );
    j = 0;
    fwrite( &j, 1, sizeof(long long int), file2 );
    j = -1;
    fwrite( &j, 1, sizeof(long long int), file2 );
    fwrite( &j, 1, sizeof(long long int), file2 );
    j = 0;
    fwrite( &j, 1, sizeof(long long int), file2 );
    fwrite( &j, 1, sizeof(long long int), file2 );
    fclose( file2 );
  }

  fclose( file );


  file = fopen( "zzz2.raw", "wb" );
  for( a = 0 ; a < MAP_SIZEX*MAP_SIZEY ; a++ )
  {
    if( mapdata[a] )
      fputc( 0xFF, file );
    else
      fputc( 0x00, file );
  }
  fclose( file );


  return 1;
}



/*
map
  4:size X
  4:size Y
  4:number of systems
  4:number of planets
  4:number of MAP_FAMILIES
  4:number of players per empire
  4:reserved
 32:reserved
struct ( 20 )
  4:position ( y << 16 ) + x
  4:index first planet
  4:number of planets
  4:empire home system, -1:none
  4:unexplored count
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
 76:reserved


*/




