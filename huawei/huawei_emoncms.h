//#include <ctype.h>	// for isprint
#include <stdio.h>	// for sprintf, printf, fprintf
#include <string.h>	// for strerror, strlen, index, strncmp, strncpy, strcmp, strcpy
#include <stdlib.h>	// for strol, strtod, exit, abort
#include <unistd.h>	// for close, read, getopt, opterr
#include <fcntl.h>	// for open, fcntl, O_RDWR, O_NOCTTY, O_NONBLOCK, F_SETFL
//#include <sys/ioctl.h>	// for ioctl
#include <errno.h>	// for errno
//#include <termios.h>	// for struct termios, tcgetattr, cfmakeraw, cfsetspeed, tcsetattr, VMIN, VTIMES, CSB, tcdrain
#include <sysexits.h>	// for EX_IOERR, EX_OK, EX_UNAVAILABLE
#include <sys/param.h>	// for MAXPATHLEN
//#include <time.h>	// for ctime, time
#include <signal.h>	// for sigaction, SIGINT

#include <sys/socket.h>	// for socket
#include <arpa/inet.h>	// for sockaddr_in
#include <netdb.h>	// for gethostbyname

#include <libconfig.h>

#define VERSION "v1.00"
#define TOOLNAME "Huawei emonCMS"
#define COPYRIGHT "(c)2015 Oliver Gerler (rockus@rockus.at)"

// keep running until user hits Ctrl-C (also obviously works if only one frame to be printed)
volatile int keepRunning = 1;

// configuration items
struct config
{
	const char *pNodeName;
	const char *pModemIP;
	const char *pHostName;
	const char *pApiKey;
};

// data from GSM/UMTS stick
struct huawei
{
	char network[16];	// provider network
	int mcc;		// mobile country code
	int mnc;		// mobile network code
	long totDown;		// total download
	long totUp;		// total upload
	long monthDown;		// total download
	long monthUp;		// total upload
	long dataLimit;		// data limit per month (as configured in web interface of GSM stick)
};

static int readmModemData (struct config *config, struct huawei *huawei, int socket_fd);
static int sendToEmonCMS (struct config *config, struct huawei *huawei, int socket_fd);
static void printHelp(void);
