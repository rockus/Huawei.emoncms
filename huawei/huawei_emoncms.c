#include "huawei_emoncms.h"
#include "../emoncms.h"

double T;
volatile int keepRunning;

void intHandler(int sig) {
    if (sig==SIGINT) {			// only quit on CTRL-C
	keepRunning = 0;
    }
}

int main(int argc, char **argv)
{
    char	configFilePath[MAXPATHLEN];
    char	hostName[1024];
    int		fileDescriptor;
    int		c;
    struct 	sigaction act;

    int			socket_fd_cms;
    int                 socket_fd_modem;
    struct sockaddr_in	info_cms;
    struct sockaddr_in	info_modem;
    struct hostent	*he_cms;
    struct hostent	*he_modem;
    struct in_addr      ipv4addr;

    config_t cfg;
    struct config config;
    struct data data;

    if (argc==1)
    {
	printHelp();
	return 1;
    }

    opterr = 0;
    while ((c=getopt(argc, argv, "c:")) != -1) {
	switch (c) {
	    case 'c': strcpy(configFilePath, optarg); break;
//	    case 'd': strcpy(deviceFilePath, optarg); break;
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

    // read temperature before anything else: avoids heating due to this code here (improbable, but still)
    char Tbuf[32];
    fileDescriptor = open ("/sys/class/thermal/thermal_zone0/temp", O_RDONLY);
    if (!(fileDescriptor))
        fprintf (stderr, "Could not open /sys/class/thermal/thermal_zone0/temp for read.\n");
    read (fileDescriptor, Tbuf, sizeof(Tbuf)-1);
    close (fileDescriptor);
    T = strtof(Tbuf, NULL) / 1000.0;
//    printf ("%6.3f\n", T);

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
    // modemIP, node, host, apikey
    if (!(config_lookup_string(&cfg, "modemIP", &(config.pModemIP))))
    {
	fprintf(stderr, "No 'modemIP' setting in configuration file.\n");
	config_destroy(&cfg);
	return(EXIT_FAILURE);
    }
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

printf ("conf file: %s\n", configFilePath);
printf ("modem IP: %s\n", config.pModemIP);
printf ("host: %s\n", config.pHostName);
printf ("node: %s\n", config.pNodeName);
printf ("API key: %s\n", config.pApiKey);

    if (!config.pHostName[0])
    {
        printf("No host name found. Did you specify the '-h hostname' option?\n");
	config_destroy(&cfg);
        return EX_UNAVAILABLE;
    }
    if (!(he_cms = gethostbyname(config.pHostName)))
    {
        fprintf(stderr, "Could not resolve host name, err %d.\n", h_errno);
	config_destroy(&cfg);
        exit(1);
    }
    inet_pton(AF_INET, config.pModemIP, &ipv4addr);
    if (!(he_modem = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET)))
    {
        fprintf(stderr, "Could not resolve modem IP, err %d.\n", h_errno);
	config_destroy(&cfg);
        exit(1);
    }

ipv4addr.s_addr = *(u_long *)he_cms->h_addr_list[0];
printf ("host: %s %s\n", he_cms->h_name, inet_ntoa(ipv4addr));
ipv4addr.s_addr = *(u_long *)he_modem->h_addr_list[0];
printf ("modem: %s %s\n", he_modem->h_name, inet_ntoa(ipv4addr));

  keepRunning = 0;
  do {
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

    if ((socket_fd_modem = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        fprintf(stderr, "Could not allocate socket, err %d.\n", errno);
		config_destroy(&cfg);
		close(socket_fd_cms);
        exit(1);
    }
    info_modem.sin_family = AF_INET;
    info_modem.sin_port = htons(80);
    info_modem.sin_addr = *((struct in_addr *)he_modem->h_addr);
    if (connect(socket_fd_modem, (struct sockaddr *)&info_modem, sizeof(struct sockaddr)) < 0)
    {
        fprintf(stderr, "Could not connect to server, err%d.\n", errno);
        close(socket_fd_modem);
        close(socket_fd_cms);
		config_destroy(&cfg);
        exit(1);
    }
    if (!(readModemData(&config, &data, socket_fd_modem))) {
        printf("Could not read data.\n");
    }

    if (!(sendToEmonCMS(&config, &data, socket_fd_cms))) {
        printf("Could not send data.\n");
    }

    close(socket_fd_modem);
    close(socket_fd_cms);

//    sleep (5);
  } while (keepRunning);

    printf ("Closing down.\n");
    config_destroy(&cfg);
    return EX_OK;
}


// send data to emonCMS
int sendToEmonCMS (struct config *config, struct data *data, int socket_fd)
{
    char tcp_buffer[1024];
    int num;

//    printf ("socket_fd: %d\n", socket_fd);

    // generate json string for emonCMS
    sprintf (tcp_buffer, "GET /input/post.json?node=\"%s\"&json={mcc:%d,mnc:%d,totdown:%ld,totup:%ld,monthdown:%ld,monthup:%ld,monthlimit:%ld}&apikey=%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s %s\r\nConnection: keep-alive\r\n\r\n", config->pNodeName, data->mcc, data->mnc, data->totDown, data->totUp, data->monthDown, data->monthUp, data->dataLimit, config->pApiKey, config->pHostName, TOOLNAME, VERSION);
    printf ("-----\nbuflen: %ld\n%s\n", strlen(tcp_buffer), tcp_buffer);
    printf ("sent: %ld\n", send(socket_fd, tcp_buffer, strlen(tcp_buffer), 0));

    // generate json string for emonCMS
    sprintf (tcp_buffer, "GET /input/post.json?node=\"Raspi\"&json={Tcore:%6.3f}&apikey=%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s %s\r\nConnection: keep-alive\r\n\r\n", T, config->pApiKey, config->pHostName, TOOLNAME, VERSION);
    printf ("-----\nbuflen: %ld\n%s\n", strlen(tcp_buffer), tcp_buffer);
    printf ("sent: %ld\n", send(socket_fd, tcp_buffer, strlen(tcp_buffer), 0));

    return 1;	// 0 - fail; 1 - success
}

// read data from Huawei GSM/UMTS modem
int readModemData(struct config *config, struct data *data, int socket_fd)
{
    char request[255];
    char response[1024];

    sprintf (request, "GET /api/net/current-plmn/ HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s %s\r\nConnection: keep-alive\r\n\r\n", config->pModemIP, TOOLNAME, VERSION);
    if (send(socket_fd, request, strlen(request), 0) < strlen(request))
    {
	printf ("send error.\n");
    }
    if (recv(socket_fd, response, sizeof(response), 0) == 0)
    {
	printf ("receive error.\n");
    }

    data->mcc = strtol(strstr(response, "<Numeric>")+9, NULL, 0)/100;				// first 3 digits
    data->mnc = strtol(strstr(response, "<Numeric>")+9, NULL, 0)-100*data->mcc;		// last 2 digits
//    printf ("%d %d\n", data->mcc, data->mnc);

    sprintf (request, "GET /api/monitoring/traffic-statistics HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s %s\r\nConnection: keep-alive\r\n\r\n", config->pModemIP, TOOLNAME, VERSION);
    if (send(socket_fd, request, strlen(request), 0) < strlen(request))
    {
	printf ("send error.\n");
    }
    sleep (2);	// to wait until all data available
    int rNum;	// number of received bytes
    rNum = recv(socket_fd, response, sizeof(response)-1, 0);
//    printf("rNum: %d\n", rNum);
    if (rNum == 0)
    {
	printf ("receive error.\n");
    }
//    printf ("%s\n", request);
//    printf ("%s\n", response);
    data->totDown = strtol(strstr(response, "<TotalDownload>")+15, NULL, 0);
    data->totUp = strtol(strstr(response, "<TotalUpload>")+13, NULL, 0);

    sprintf (request, "GET /api/monitoring/month_statistics HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s %s\r\nConnection: keep-alive\r\n\r\n", config->pModemIP, TOOLNAME, VERSION);
    if (send(socket_fd, request, strlen(request), 0) < strlen(request))
    {
	printf ("send error.\n");
    }
    sleep (2);	// to wait until all data available
    rNum = recv(socket_fd, response, sizeof(response)-1, 0);
//    printf("rNum: %d\n", rNum);
    if (rNum == 0)
    {
	printf ("receive error.\n");
    }
//    printf ("%s\n", request);
//    printf ("%s\n", response);

    data->monthDown = strtol(strstr(response, "<CurrentMonthDownload>")+22, NULL, 0);
    data->monthUp = strtol(strstr(response, "<CurrentMonthUpload>")+20, NULL, 0);

    sprintf (request, "GET /api/monitoring/start_date HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s %s\r\nConnection: keep-alive\r\n\r\n", config->pModemIP, TOOLNAME, VERSION);
    if (send(socket_fd, request, strlen(request), 0) < strlen(request))
    {
	printf ("send error.\n");
    }
    sleep (2);	// to wait until all data available
    rNum = recv(socket_fd, response, sizeof(response)-1, 0);
//    printf("rNum: %d\n", rNum);
    if (rNum == 0)
    {
	printf ("receive error.\n");
    }
//    printf ("%s\n", request);
    printf ("%s\n", response);

    data->dataLimit = strtol(strstr(response, "<DataLimit>")+11, NULL, 0);

    printf ("month down/up/limit: %ld/%ld/%ld\n", data->monthDown, data->monthUp, data->dataLimit);

    return 1;	// 0 - fail; 1 - success
}

void printHelp(void)
{
	printf ("\n");
	printf ("%s %s %s\n", TOOLNAME, VERSION, COPYRIGHT);
	printf ("\n");
	printf ("options:\n");
	printf ("  -c config : specify config file\n");
	printf ("\n");
}
