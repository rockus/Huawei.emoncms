#ifndef BME280_EMONCMS_H
#define BME280_EMONCMS_H

#include <ctype.h>		// for isprint
#include <libconfig.h>		// for config_t
#include <string.h>		// for strerror, strlen, index, strncmp, strncpy, strcmp, strcpy
#include <stdlib.h>		// for strol, strtod, exit, abort
#include <unistd.h>		// for close, read, getopt, opterr
#include <errno.h>		// for errno
#include <netdb.h>		// for gethostbyname
#include <sysexits.h>	// for EX_IOERR, EX_OK, EX_UNAVAILABLE
#include <sys/param.h>	// for MAXPATHLEN

#include "bme280.h"

#define COPYRIGHT "(c)2017 Oliver Gerler (rockus@rockus.at)"
#define TOOLNAME "BME280EmonCMS"
#define BME280_VERSION "v1.02"

#endif
