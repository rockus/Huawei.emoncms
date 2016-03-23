#include "axp209_emoncms.h"
#include "../emoncms.h"

// https://github.com/nahidalam/raspberryPi/blob/master/i2ctest.c

// from i2c-dev.h from i2c-tools from https://fossies.org/dox/i2c-tools-3.1.2/i2c-dev_8h.html#afb1764d4b58fb542306c715ad6a28a42
#define I2C_SLAVE 0x0703
#define I2C_SLAVE_FORCE 0x0706

static int find_axp209(void)
{
  int fd;

// check existence of i2c bus
    wiringPiSetup();

//  wiringPiI2CSetupInterface ("/dev/i2c-0", 0x34);
        // this stops execution on error. BAD!
 if ((fd = open ("/dev/i2c-0", O_RDWR)) < 0)    // bus: /dev/i2c-0
    printf ("device open failed\n");
 if (ioctl (fd, I2C_SLAVE_FORCE, 0x34) < 0)         // address: 0x34
    printf ("slave select failed\n");

  return fd;
}

static int read_axp209_dat(int fd, struct data *data)
{
  int address, byte;
  uint16_t buf;

  // battery voltage:	0x000: 0.0000V
  // (78/29)			0xfff: 4.5045V
  //					step: 1.1mV
  // ACIN voltage:	0x000: 0.0000V
  // (56/67)		0xfff: 6.9615V
  //				step: 1.7mV
  // VBUS voltage:	0x000: 0.0000V
  // (5a/5b)		0xfff: 6.9615V
  //				step: 1.7mV
  // battery discharge current:	0x000: 0.0000V
  // (7c/7d)					0xfff: 4.095A
  //							step: 0.5mA
  // battery charge current:	0x000: 0.0000V
  // (7a/7b)					0xfff: 4.095A
  //							step: 0.5mA
  // ACIN current:	0x000: 0.0000V
  // (58/59)		0xfff: 2.5594A
  //				step: 0.625mA
  // VBUS current:	0x000: 0.0000V
  // (5c/5d)		0xfff: 1.5356A
  //				step: 0.375mA
  // internal temperature:	0x000: -144.7°C
  // (5e/5f)				0xfff:  264.8°C
  //						step: 0.1°C

  address = 0x56; // ACinV MSB
  byte = wiringPiI2CReadReg8(fd, address);
  printf ("msb/lsb: %02x", byte);
  buf = (uint16_t)(byte << 4);
  address = 0xf57; // ACinV LSB
  byte = wiringPiI2CReadReg8(fd, address);
  printf ("%02x ", byte);
  buf |= (uint16_t)(byte & 0x0f);
  data->ACIN_voltage = (float)buf * 1.7;
  printf (" buf: %03x ACinV: %fmV\n", buf, data->ACIN_voltage);

  address = 0x58; // ACinA MSB
  byte = wiringPiI2CReadReg8(fd, address);
  printf ("msb/lsb: %02x", byte);
  buf = (uint16_t)(byte << 4);
  address = 0x59; // ACinA LSB
  byte = wiringPiI2CReadReg8(fd, address);
  printf ("%02x ", byte);
  buf |= (uint16_t)(byte & 0x0f);
  data->ACIN_current = (float)buf * 0.625;
  printf (" buf: %03x ACinA: %fmA\n", buf, data->ACIN_current);

  printf ("AC power: %fW\n", data->ACIN_voltage * data->ACIN_current / 1e6);

  address = 0x5e; // int.temp MSB
  byte = wiringPiI2CReadReg8(fd, address);
  printf ("msb/lsb: %02x", byte);
  buf = (uint16_t)(byte << 4);
  address = 0x5f; // int.temp LSB
  byte = wiringPiI2CReadReg8(fd, address);
  printf ("%02x ", byte);
  buf |= (uint16_t)(byte & 0x0f);
  data->internal_temperature = (float)buf * 0.1 - 144.7;
  printf (" buf: %03x int.temp: %f°C\n", buf, data->internal_temperature);

  return 1;
}

int gatherData(struct config *config, struct data *data)
// return 0 on error
{
	int fd;

	// open axp209
	fd = find_axp209();

	// read out raw data
	read_axp209_dat(fd, data);

	printf ("ACin: %5.2fmV %5.2fmA\ninternal temp: %5.2f°C\n", data->ACIN_voltage, data->ACIN_current, data->internal_temperature);

	return 1;
}

// send data to emonCMS
int sendToEmonCMS (struct config *config, struct data *data, int socket_fd)
{
    char tcp_buffer[1024];
    int num;

//    printf ("socket_fd: %d\n", socket_fd);

    // generate json string for emonCMS
    sprintf (tcp_buffer, "GET /input/post.json?node=\"%s\"&json={Voltage-AXP209-ACin:%4.2f,Current-AXP209-ACin:%4.2f,Temperature-AXP209-internal:%4.2f}&apikey=%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s %s\r\nConnection: keep-alive\r\n\r\n", config->pNodeName, data->ACIN_voltage, data->ACIN_current, data->internal_temperature, config->pApiKey, config->pHostName, TOOLNAME, AXP209_VERSION);

    printf ("-----\nbuflen: %ld\n%s\n", strlen(tcp_buffer), tcp_buffer);
    printf ("sent: %ld\n", send(socket_fd, tcp_buffer, strlen(tcp_buffer), 0));

    return 1;   // 0 - fail; 1 - success
}
