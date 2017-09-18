#include "bme280_emoncms.h"
#include "../emoncms.h"
#include <wiringPi.h>
#include <wiringPiI2C.h>

volatile int keepRunning;

void intHandler(int sig)
{
    if (sig==SIGINT)				// only quit on CTRL-C
	{
		keepRunning = 0;
    }
}

int main(int argc, char **argv)
{
	// handling Crl-C
    struct 	sigaction act;

	// config file handling
    char	configFilePath[MAXPATHLEN];
    config_t cfg;
    struct	config config;
    int		c;						// for getopt

	// network communication
    int		socket_fd_cms;
    struct	sockaddr_in	info_cms;
    struct	hostent	*he_cms;
    struct	in_addr ipv4addr;

	// data gathering
	struct	data data;
//	char	command[256];
//	FILE	*fileDescriptor;		// for reading from /proc/net/wireless
//	char 	fileContent[256];


    if (argc==1)
    {
		printHelp();
		return 1;
    }

    opterr = 0;
    while ((c=getopt(argc, argv, "c:")) != -1) {
		switch (c) {
		    case 'c': strcpy(configFilePath, optarg); break;
		    case '?': if (optopt=='d' || optopt=='h')
				fprintf(stderr, "Option -%c requires an argument.\n", optopt);
			      else if (isprint (optopt))
				fprintf(stderr, "Unknown option '-%c'.\n", optopt);
		    	  else
				fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
			      return 1;
	    	default: abort();
		}
    }

    act.sa_handler = intHandler;
    sigaction(SIGINT, &act, NULL);	// catch Ctrl-C


    // read config file
    config_init(&cfg);
    /* Read the file. If there is an error, report it and exit. */
    if(! config_read_file(&cfg, configFilePath))
    {
		fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
	    config_error_line(&cfg), config_error_text(&cfg));
		config_destroy(&cfg);
		return(EXIT_FAILURE);
    }
    // node, host, apikey, wlaninterface
    if (!(config_lookup_string(&cfg, "node", &(config.pNodeName))))
    {
		fprintf(stderr, "No 'node' setting in configuration file.\n");
		config_destroy(&cfg);
		return(EXIT_FAILURE);
    }
    if (!(config_lookup_string(&cfg, "host", &(config.pHostName))))
    {
		fprintf(stderr, "No 'host' setting in configuration file.\n");
		config_destroy(&cfg);
		return(EXIT_FAILURE);
    }
    if (!(config_lookup_string(&cfg, "apikey", &(config.pApiKey))))
    {
		fprintf(stderr, "No 'apikey' setting in configuration file.\n");
		config_destroy(&cfg);
		return(EXIT_FAILURE);
    }
    if (!(config_lookup_string(&cfg, "i2cbus", &(config.pi2cBus))))
    {
		fprintf(stderr, "No 'i2cbus' setting in configuration file.\n");
		config_destroy(&cfg);
		return(EXIT_FAILURE);
    }

printf ("conf file: %s\n", configFilePath);
printf ("host: %s\n", config.pHostName);
printf ("node: %s\n", config.pNodeName);
printf ("API key: %s\n", config.pApiKey);
printf ("i2c bus: %s\n", config.pi2cBus);

    if (!(he_cms = gethostbyname(config.pHostName)))
    {
        fprintf(stderr, "Could not resolve host name, err %d.\n", h_errno);
		config_destroy(&cfg);
        exit(1);
    }

  keepRunning = 0;
  do {
	if (!(gatherData(&config, &data)))
	{
		fprintf(stderr, "Could not gather data.\n");
		config_destroy(&cfg);
		return(EXIT_FAILURE);
	}
/*
  } while (keepRunning);
config_destroy(&cfg);
return (EXIT_FAILURE);
do {
*/
    if ((socket_fd_cms = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        fprintf(stderr, "Could not allocate socket, err %d.\n", errno);
		config_destroy(&cfg);
        exit(1);
    }
    info_cms.sin_family = AF_INET;
    info_cms.sin_port = htons(80);
    info_cms.sin_addr = *((struct in_addr *)he_cms->h_addr);
    if (connect(socket_fd_cms, (struct sockaddr *)&info_cms, sizeof(struct sockaddr)) < 0)
    {
        fprintf(stderr, "Could not connect to server, err%d.\n", errno);
        close(socket_fd_cms);
		config_destroy(&cfg);
        exit(1);
    }

    if (!(sendToEmonCMS(&config, &data, socket_fd_cms))) {
        printf("Could not send data.\n");
    }

    close(socket_fd_cms);

//    sleep (10);
  } while (keepRunning);

    printf ("Closing down.\n");
    config_destroy(&cfg);
    return EX_OK;
}

void printHelp(void)
{
	printf ("\n");
	printf ("%s %s %s\n", TOOLNAME, BME280_VERSION, COPYRIGHT);
	printf ("\n");
	printf ("options:\n");
	printf ("  -c config : specify config file\n");
	printf ("\n");
}
