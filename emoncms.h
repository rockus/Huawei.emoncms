#ifndef EMONCMS_H
#define EMONCMS_H

#include <libconfig.h>
#include <syslog.h>

// configuration items
struct config
{
	// config common to all modules
	const char *pNodeName;
	const char *pHostName;
	const char *pApiKey;
	// config options particular to single modules
	const char *pModemIP;
	const char *pWlanInterface;
	const char *pi2cBus;
};

// common function declarations
extern int sendToEmonCMS (struct config *config, struct data *data, int socket_fd);
extern int gatherData (struct config *config, struct data *data);
static void printHelp(void);

// function declaration for huawei_emoncms
static int readModemData (struct config *config, struct data *data, int socket_fd);

#endif
