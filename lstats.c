
#include <sys/utsname.h>
#include <asm/param.h>

#ifndef CT_TO_SECS
#define CT_TO_SECS(x) ((x)/HZ)
#endif

void lstatsConvertTime( unsigned char *buffer, int eltime )
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


int lstatsUptime( unsigned char *buffer, float *uptime )
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
    if( buffer )
      iohttpFuncConvertTime( buffer, (int)uptime_seconds );
    return 1;
  }
  return 0;
}

int lstatsLoadavg( unsigned char *buffer )
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

int lstatsCpuinfo( unsigned char *buffer )
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


/*
Keep jiffties executed for last X ticks
Keep total
*/

#define LSTATS_TICKS_MAX (12*6);

typedef struct
{
  int utime;
  int stime;
} lstatsTickDef, *lstatsTickPtr;

lstatsTickDef lstatsTick[LSTATS_TICKS_MAX]
int lstatsTickNum;
int lstatsLastUtime = 0;
int lstatsLastStime = 0;
float lstatsLastRunTime = 0;


void lstatsTick()
{
  int pid;
  FILE *file;
  unsigned char fname[256];
  int stutime, ststime, stpriority, ststarttime, stvsize, strss;
  float boottime, runtime;

  pid = getpid();
  sprintf( fname, "/proc/%d/stat", pid );
  if( ( file = fopen( fname, "r" ) ) )
  {
    fscanf( file, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %d %d %*d %*d %d %*d %*u %*u %u %u %u", &stutime, &ststime, &stpriority, &ststarttime, &stvsize, &strss );
    fclose( file );
  }

  lstatsUptime( 0, &boottime );
  lstatsLastRunTime = boottime - CT_TO_SECS( ( (float)ststarttime ) );

  memmove( &lstatsTick[1], &lstatsTick[0], (LSTATS_TICKS_MAX-1)*sizeof(lstatsTickDef) );
  lstatsTick[0].utime = stutime - lstatsLastUtime;
  lstatsTick[0].stime = ststime - lstatsLastStime;

  lstatsLastUtime = stutime;
  lstatsLastStime = ststime;
  if( lstatsTickNum < LSTATS_TICKS_MAX )
    lstatsTickNum++;

  return 0;
}


float lstatsLoadUser[4];
float lstatsLoadKern[4];

// calc loads
void lstatsUpdate()
{
  int a, b, utime, stime;

  // ten minutes
  lstatsLoadUser[0] = 100.0 * ( CT_TO_SECS( (float)lstatsTick[0].utime ) / (1*SV_TICK_TIME) );
  lstatsLoadKern[0] = 100.0 * ( CT_TO_SECS( (float)lstatsTick[0].stime ) / (1*SV_TICK_TIME) );

  // total
  lstatsLoadUser[3] = 100.0 * ( CT_TO_SECS( lstatsLastUtime ) / lstatsLastRunTime );
  lstatsLoadKern[3] = 100.0 * ( CT_TO_SECS( lstatsLastStime ) / lstatsLastRunTime );

  // one hour
  utime = stime = 0;
  for( a = 0 ; a < 6 ; a++ )
  {
    utime += lstatsTick[a].utime;
    stime += lstatsTick[a].stime;
  }
  lstatsLoadUser[1] = 100.0 * ( CT_TO_SECS( (float)utime ) / (6*SV_TICK_TIME) );
  lstatsLoadKern[1] = 100.0 * ( CT_TO_SECS( (float)stime ) / (6*SV_TICK_TIME) );

  // twelve hours
  utime = stime = 0;
  for( a = 0 ; a < LSTATS_TICKS_MAX ; a++ )
  {
    utime += lstatsTick[a].utime;
    stime += lstatsTick[a].stime;
  }
  lstatsLoadUser[2] = 100.0 * ( CT_TO_SECS( (float)utime ) / (LSTATS_TICKS_MAX*SV_TICK_TIME) );
  lstatsLoadKern[2] = 100.0 * ( CT_TO_SECS( (float)stime ) / (LSTATS_TICKS_MAX*SV_TICK_TIME) );



/*
  linux_get_proc_uptime( 0, &boottime );
  runtime = boottime - CT_TO_SECS( ( (float)ststarttime ) );
  userload = 100.0 * ( CT_TO_SECS( ( (float)stutime ) ) / runtime );
  kernelload = 100.0 * ( CT_TO_SECS( ( (float)ststime ) ) / runtime );
*/
}




