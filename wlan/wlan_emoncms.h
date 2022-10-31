#include <ctype.h>	// for isprint
#include <libconfig.h>		// for config_t
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
#include <time.h>	// for ctime, time, gettimeofday
#include <signal.h>	// for sigaction, SIGINT

#include <sys/socket.h>	// for socket
#include <arpa/inet.h>	// for sockaddr_in
#include <netdb.h>	// for gethostbyname

#define WLAN_VERSION "v1.01"
#define TOOLNAME "WLANEmonCMS"
#define COPYRIGHT "(c)2015,2016 Oliver Gerler (rockus@rockus.at)"

struct data
{
	int LinkQuality;
	int SignalLevel;
	int NoiseLevel;
};
