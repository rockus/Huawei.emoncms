#ifndef AXP209_EMONCMS_H
#define AXP209_EMONCMS_H

#include <ctype.h>	// for isprint
#include <stdio.h>		// for sprintf, printf, fprintf
#include <string.h>		// for strerror, strlen, index, strncmp, strncpy, strcmp, strcpy
#include <stdlib.h>		// for strol, strtod, exit, abort
#include <unistd.h>		// for close, read, getopt, opterr
#include <fcntl.h>		// for open, fcntl, O_RDWR, O_NOCTTY, O_NONBLOCK, F_SETFL
#include <sys/ioctl.h>	// for ioctl
#include <errno.h>		// for errno
//#include <termios.h>	// for struct termios, tcgetattr, cfmakeraw, cfsetspeed, tcsetattr, VMIN, VTIMES, CSB, tcdrain
#include <sysexits.h>	// for EX_IOERR, EX_OK, EX_UNAVAILABLE
#include <sys/param.h>	// for MAXPATHLEN
#include <time.h>		// for ctime, time, gettimeofday
#include <signal.h>		// for sigaction, SIGINT

#include <sys/socket.h>	// for socket
#include <arpa/inet.h>	// for sockaddr_in
#include <netdb.h>		// for gethostbyname

#include <wiringPi.h>
#include <wiringPiI2C.h>

#define COPYRIGHT "(c)2016 Oliver Gerler (rockus@rockus.at)"
#define TOOLNAME "AXP209EmonCMS"
#define AXP209_VERSION "v1.00"

struct data
{
	float Batt_voltage;
	float ACIN_voltage;
	float VBUS_voltage;
	float APS_voltage;
	float Batt_discharge_current;
	float Batt_charge_current;
	float ACIN_current;
	float VBUS_current;
	float internal_temperature;
};

#endif
