#include <libconfig.h>

// configuration items
struct config
{
	const char *pNodeName;
	const char *pModemIP;
	const char *pHostName;
	const char *pApiKey;
};

// keep running until user hits Ctrl-C (also obviously works if only one frame to be printed)
volatile int keepRunning = 1;

// common function declarations
static int sendToEmonCMS (struct config *config, struct data *data, int socket_fd);
static void printHelp(void);

// function declaration for huawei_emoncms
static int readModemData (struct config *config, struct data *data, int socket_fd);
