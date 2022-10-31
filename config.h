#ifndef CONFIG_H
#define CONFIG_H

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
	int pDHTpin;
};

#endif
