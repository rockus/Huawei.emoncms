#ifndef DHT22_EMONCMS_H
#define DHT22_EMONCMS_H

#include <ctype.h>	// for isprint
#include <libconfig.h>		// for config_t
#include <stdio.h>		// for sprintf, printf, fprintf
#include <string.h>		// for strerror, strlen, index, strncmp, strncpy, strcmp, strcpy
#include <stdlib.h>		// for strol, strtod, exit, abort
#include <unistd.h>		// for close, read, getopt, opterr
#include <fcntl.h>		// for open, fcntl, O_RDWR, O_NOCTTY, O_NONBLOCK, F_SETFL
//#include <sys/ioctl.h>	// for ioctl
#include <errno.h>		// for errno
//#include <termios.h>	// for struct termios, tcgetattr, cfmakeraw, cfsetspeed, tcsetattr, VMIN, VTIMES, CSB, tcdrain
#include <sysexits.h>	// for EX_IOERR, EX_OK, EX_UNAVAILABLE
#include <sys/param.h>	// for MAXPATHLEN
#include <time.h>		// for ctime, time, gettimeofday
#include <signal.h>		// for sigaction, SIGINT

#include <sys/socket.h>	// for socket
#include <arpa/inet.h>	// for sockaddr_in
#include <netdb.h>		// for gethostbyname

#include <wiringPi.h>	// for DHT22 communications

#define COPYRIGHT "(c)2017 Oliver Gerler (rockus@rockus.at)"
#define TOOLNAME "DHT22EmonCMS"
#define DHT22_VERSION "v2.0"

#define MAXTIMINGS 85

struct data
{
	float Humidity;
	float Temperature;
};

#endif
