
#define SERVER_SOFTWARE "Ectroverse"

#define SERVER_RECV_BUFSIZE (32768)

#define SERVER_PATH_BUFSIZE (512)

#define SERVER_SELECT_MSEC (1000)

#define SERVER_NAGLE_BUFFERING 0


#define AUTOVICTORYIN (16)
#define AUTOENDWARS (52)

#define SERVER_REPORT_CONNECT 0
#define SERVER_REPORT_CLOSE 0
#define SERVER_REPORT_IGNOREDHEADER 0

#define IRC_SERVER "irc.staticbox.net"

//this is to keep the time in forum at gmt
//1 = amsterdam 0 = gmt -4 = canada ... etc
#define SERVER_TIME_ZONE 1

#define SV_TICK_TIME (600)
#define START_TIME " Apr 16 20:00"
#define STOP_TIME "May 14 16:00"


#define ROUND_ID (9)
#define DELAY_TIME (00)

#define SV_TICK_FILE "/var/www/ectroverse.org/data/ticks"

#define DB_DIRECTORY "/var/www/ectroverse.org/data"

#define PUBLIC_FORUM_DIRECTORY "/var/www/ectroverse.org/data"

#define USER_DIRECTORY "/var/www/ectroverse.org/data"

#define IOHTTP_FILES_DIRECTORY "/var/www/ectroverse.org/images"

#define IOHTTP_READ_DIRECTORY "/var/www/ectroverse.org/read"

#define LOGS_DIRECTORY "/var/www/ectroverse.org/logs"

#define CMD_ADMIN_NUM 1
static unsigned char cmdAdminName[CMD_ADMIN_NUM] [32] = {
"admin" };
static unsigned char cmdAdminPass[CMD_ADMIN_NUM] [32] = {
"password" };

#define HTTP_PORT 9122
#define HTTP_PORT_MAP 9135
