#include "raspi_pulsecount_emoncms.h"
#include "../emoncms.h"
#include <pigpio.h>

#define GPIO 4	// GPIO 4, P1_07 -- S0+
				// GND,    P1_09 -- S0-

int clean_up=0;

void intHandler (int sig)
{
    if (sig==SIGINT || sig==SIGTERM)	// Ctrl-C, SIGTERM
	clean_up=1;
}

void myAlert (int gpio, int level, uint32_t tick)
{
    if (gpio != GPIO)
    {
		printf ("different alert, not for us.\n");
		return;
    }

    printf ("gpio %d became %d at %d.\n", gpio, level, tick);
    if (level == 0)	// falling edge
    {
        kill(0, SIGUSR1);	// wake up from pause()
    }
}

int main (int argc, char **argv)
{
	struct	sigaction act;

	char	configFilePath[MAXPATHLEN];
	char	hostName[1024];
	int	c;				// for getopt

	int	socket_fd_cms;

	config_t cfg;
	struct config config;
	struct sockaddr_in info_cms;
	struct hostent *he_cms;

//	struct data data;

    struct timeval time_last, time_now;

    char tcp_buffer[1024];
    int num;

    // print help if no arguments given
    if (argc==1)
    {
		printHelp();
		return 1;
    }

    // init library
    if (gpioInitialise()<0)
		return 1;

    // load and parse config file
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
					  gpioTerminate();
		      		  return 1;
	    	default: gpioTerminate(); abort();
		}
    }

    config_init(&cfg);
    // Read the file. If there is an error, report it and exit.
    if(! config_read_file(&cfg, configFilePath))
    {
		fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
	    config_error_line(&cfg), config_error_text(&cfg));
		config_destroy(&cfg);
		gpioTerminate();
		return(EXIT_FAILURE);
    }
    // node, host, apikey
    if (!(config_lookup_string(&cfg, "node", &(config.pNodeName))))
    {
		fprintf(stderr, "No 'node' setting in configuration file.\n");
		config_destroy(&cfg);
		gpioTerminate();
		return(EXIT_FAILURE);
    }
    if (!(config_lookup_string(&cfg, "host", &(config.pHostName))))
    {
		fprintf(stderr, "No 'host' setting in configuration file.\n");
		config_destroy(&cfg);
		gpioTerminate();
		return(EXIT_FAILURE);
    }
    if (!(config_lookup_string(&cfg, "apikey", &(config.pApiKey))))
    {
		fprintf(stderr, "No 'apikey' setting in configuration file.\n");
		config_destroy(&cfg);
		gpioTerminate();
		return(EXIT_FAILURE);
    }

    if (!config.pHostName[0])
    {
        printf("No host name found. Did you specify the '-h hostname' option or mention it in the config file?\n");
		config_destroy(&cfg);
		gpioTerminate();
        return EX_UNAVAILABLE;
    }
    if (!(he_cms = gethostbyname(config.pHostName)))
    {
        fprintf(stderr, "Could not resolve host name '%s', err %d.\n", config.pHostName, h_errno);
		config_destroy(&cfg);
		gpioTerminate();
        exit(1);
    }
    info_cms.sin_family = AF_INET;
    info_cms.sin_port = htons(80);
    info_cms.sin_addr = *((struct in_addr *)he_cms->h_addr);
/*    if ((socket_fd_cms = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        fprintf(stderr, "Could not allocate socket, err %d.\n", errno);
		config_destroy(&cfg);
		gpioTerminate();
		exit(1);
    }
    if (connect(socket_fd_cms, (struct sockaddr *)&info_cms, sizeof(struct sockaddr)) < 0)
    {
        fprintf(stderr, "Could not connect to server, err%d.\n", errno);
		close (socket_fd_cms);
		config_destroy(&cfg);
		gpioTerminate();
		exit(1);
    }
*/
    // config GPIO
    gpioSetMode (GPIO, PI_INPUT);
    gpioSetPullUpDown (GPIO, PI_PUD_UP);
    gpioSetAlertFunc (GPIO, myAlert);

    act.sa_handler = intHandler;
    sigaction(SIGINT, &act, NULL);	// catch Ctrl-C
    sigaction(SIGTERM, &act, NULL);	// catch kill

	gettimeofday(&time_last, NULL);
	gettimeofday(&time_now, NULL);

	unsigned long elapsedtime_us;		// [us]
	float elapsedtime_s;				// [s]
	float power;						// [W]

	syslog(LOG_INFO, "started. waiting for pulse...");

    while (!clean_up)
    {
		pause();
		if (!clean_up)	// to avoid running even once if SIGINT arrives during pause()
		{
//	    	printf ("back.\n"); fflush(stdout);

			gettimeofday(&time_now, NULL);
			elapsedtime_us = (time_now.tv_sec - time_last.tv_sec)*1000000 - time_last.tv_usec + time_now.tv_usec;
			printf("elapsed microseconds: %lu\n", elapsedtime_us);
			elapsedtime_s = elapsedtime_us / 1000000.0;
			printf("elapsed seconds: %f\n", elapsedtime_s);
			power = 1.0 * 3600.0 / elapsedtime_s;				// 1.0Wh per pulse * 3600 s/h / seconds = Whs/hs = W
			printf("current power: %f\n", power);

			syslog(LOG_INFO, "elapsed seconds: %f, current power: %f", elapsedtime_s, power);

			if (power > 10000.0)
	    	{
	    		syslog(LOG_INFO, "error: power >10kW.");
	    	}
	    	else
			{
			time_last.tv_sec = time_now.tv_sec;
			time_last.tv_usec = time_now.tv_usec;

		    // generate json string for emonCMS
		    sprintf (tcp_buffer, "GET /input/post.json?node=\"%s\"&json={pulseBoiler:1,powerBoiler:%5.3f}&apikey=%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s %s\r\nConnection: keep-alive\r\n\r\n", config.pNodeName, power, config.pApiKey, config.pHostName, TOOLNAME, RASPI_PULSECOUNT_VERSION);
	    	printf ("-----\nbuflen: %ld\n%s\n", strlen(tcp_buffer), tcp_buffer);

		    if ((socket_fd_cms = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		    {
				fprintf(stderr, "Could not allocate socket, err %d.\n", errno);
		    }
		    else
	    	{
				if (connect(socket_fd_cms, (struct sockaddr *)&info_cms, sizeof(struct sockaddr)) < 0)	// re-connect each time
				{
					fprintf(stderr, "Could not connect to server, err %d.\n", errno);
					syslog(LOG_WARNING,"Could not connect to server, err %d.", errno);
				}
			    else
	    		{
					num=send(socket_fd_cms, tcp_buffer, strlen(tcp_buffer), 0);
					if (num <0)
			    		printf ("send error: %d\n", errno);
					else
					    printf ("sent: %ld\n", num);
	    		}
		    	close (socket_fd_cms);
	    	}
	    	}
		}
    }

    // clean up (not usually executed until Ctrl-C)
    printf ("clean up.\n");
	syslog(LOG_INFO, "clean up.");
    close (socket_fd_cms);
    config_destroy(&cfg);
    gpioTerminate();
    return (EX_OK);
}

void printHelp(void)
{
	printf ("\n");
	printf ("%s %s %s\n", TOOLNAME, RASPI_PULSECOUNT_VERSION, COPYRIGHT);
	printf ("\n");
	printf ("options:\n");
	printf ("  -c config : specify config file\n");
	printf ("\n");
}
