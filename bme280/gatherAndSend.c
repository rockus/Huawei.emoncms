#include "bme280_emoncms.h"
#include "../emoncms.h"

// https://github.com/nahidalam/raspberryPi/blob/master/i2ctest.c

// return file descriptor of first BME280 found
int find_bme280(void)
{
  int fd, byte;

  fd = wiringPiI2CSetup (0x76);
  byte = wiringPiI2CReadReg8(fd,0xd0);
  if (byte == 0x60)
  {
    printf ("BME280 ID found at 0x76.\n");
  }
  else
  {
    fd = wiringPiI2CSetup (0x77);
    byte = wiringPiI2CReadReg8(fd,0xd0);
    if (byte == 0x60)
	  printf ("BME280 ID found at 0x77.\n");
	else
	  printf ("No BME280 ID found.\n");
  }

  return fd;
}

void do_single_measurement(int fd)
{
  int address, byte;
  // configure bme280, execute single measurement and wait for completion
  address = 0xf2; // ctrl_hum;
  byte = 0b00000001;	// osrs_h=1x
  wiringPiI2CWriteReg8(fd, address, byte);
  address = 0xf4; // ctrl_meas;
  byte = 0b00100101;	// osrs_t=1x, osrs_p=1x, mode=forced
  wiringPiI2CWriteReg8(fd, address, byte);
  do {
    address = 0xf3; // status
    byte = wiringPiI2CReadReg8(fd, address);
  } while (byte & 0b00001000);	// bit 3: measuring[0]
}

static int read_bme280_dat(int fd, struct data *data)
{
  int address, byte;

  address = 0xfd; // hum_msb
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("msb/lsb: %02x", byte);
  data->hum_raw = (uint16_t)(byte << 8);
  address = 0xfe; // hum_lsb
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("%02x ", byte);
  data->hum_raw |= (uint16_t)byte;
//  printf ("  hum_raw: 0x%04x\n", data->hum_raw);

  address = 0xfa; // temp_msb
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("msb/lsb/xlsb: 0x%02x", byte);
  data->temp_raw = (uint32_t)(byte << 12);
  address = 0xfb; // temp_lsb
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("%02x", byte);
  data->temp_raw |= (uint32_t)(byte << 4);
  address = 0xfc; // temp_xlsb
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("%01x ", byte>>4);
  data->temp_raw |= (uint32_t)(byte >> 4);
//  printf (" temp_raw: 0x%08x\n", data->temp_raw);

  address = 0xf7; // press_msb
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("msb/lsb/xlsb: 0x%02x", byte);
  data->press_raw = (uint32_t)(byte << 12);
  address = 0xf8; // press_lsb
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("%02x", byte);
  data->press_raw |= (uint32_t)(byte << 4);
  address = 0xf9; // press_xlsb
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("%01x ", byte>>4);
  data->press_raw |= (uint32_t)(byte >> 4);
//  printf ("press_raw: 0x%08x\n", data->press_raw);

  return 1;
}

int compensate_data(int fd, struct data *data)
{
  int address, byte;
  unsigned short dig_T1, dig_T2, dig_T3;
  unsigned char dig_H1, dig_H3;
  signed short dig_H2, dig_H4, dig_H5;
  signed char dig_H6;

  // dig_T1
  address = 0x89; // dig_T1[15:8]
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("%02x", byte);
  dig_T1 = (unsigned short)(byte<<8);
  address = 0x88; // dig_T1[7:0]
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("%02x ", byte);
  dig_T1 |= (unsigned short)byte;
//  printf ("dig_T1: %04x\n", dig_T1);

  // dig_T2
  address = 0x8b; // dig_T2[15:8]
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("%02x", byte);
  dig_T2 = (unsigned short)(byte<<8);
  address = 0x8a; // dig_T2[7:0]
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("%02x ", byte);
  dig_T2 |= (unsigned short)byte;
//  printf ("dig_T2: %04x\n", dig_T2);

  // dig_T3
  address = 0x8d; // dig_T3[15:8]
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("%02x", byte);
  dig_T3 = (unsigned short)(byte<<8);
  address = 0x8c; // dig_T3[7:0]
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("%02x ", byte);
  dig_T3 |= (unsigned short)byte;
//  printf ("dig_T3: %04x\n", dig_T3);

  // compensate temp
  double var1, var2, t_fine, T;
  var1 = (((double)data->temp_raw)/16384.0 - ((double)dig_T1)/1024.0) * ((double)dig_T2);
  var2 = ((((double)data->temp_raw)/131072.0 - ((double)dig_T1)/8192.0) * (((double)data->temp_raw)/131072.0 - ((double)dig_T1)/8192.0)) * ((double)dig_T3);
  t_fine = var1 + var2;
  T = t_fine / 5120.0;
//  printf ("var1: %f\n", var1);
//  printf ("var2: %f\n", var2);
//  printf ("t_fine: %f\n", t_fine);
//  printf ("T: %f\n", T);

/*
  long signed int ivar1, ivar2, it_fine, iT;
  ivar1 = ((((data->temp_raw>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
  ivar2 = (((((data->temp_raw>>4) - ((int32_t)dig_T1)) * ((data->temp_raw>>4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
  it_fine = ivar1 + ivar2;
  iT = (it_fine * 5 + 128) >> 8;
  printf ("ivar1: %ld\n", ivar1);
  printf ("ivar2: %ld\n", ivar2);
  printf("it_fine: %ld\n", it_fine);
  printf ("iT: %5.2fC\n", data->Temperature);
*/

  // dig_H1
  address = 0xa1; // dig_H1[7:0]
  byte = wiringPiI2CReadReg8(fd, address);
  dig_H1 |= (unsigned char)byte;
//  printf ("dig_H1: %02x\n", dig_H1);

  // dig_H2
  address = 0xe2; // dig_H2[15:8]
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("%02x", byte);
  dig_H2 = (signed short)(byte<<8);
  address = 0xe1; // dig_H2[7:0]
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("%02x ", byte);
  dig_H2 |= (signed short)byte;
//  printf ("dig_H2: %04x\n", dig_H2);

  // dig_H3
  address = 0xe3; // dig_H3[7:0]
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("%02x ", byte);
  dig_H3 |= (unsigned char)byte;
//  printf ("dig_H3: %04x\n", dig_H3);

  // dig_H4
  address = 0xe4; // dig_H4[11:4]
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("%02x", byte);
  dig_H4 = (signed short)(byte<<4);
  address = 0xe5; // dig_H4[3:0]
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("%02x ", byte);
  dig_H4 |= (signed short)(byte & 0x0f);
//  printf ("dig_H4: %04x\n", dig_H4);

  // dig_H5
  address = 0xe5; // dig_H5[3:0]
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("%02x", byte);
  dig_H5 = (signed short)(byte>>4 & 0x0f);
  address = 0xe6; // dig_H5[11:4]
  byte = wiringPiI2CReadReg8(fd, address);
//  printf ("%02x ", byte);
  dig_H5 |= (signed short)(byte<<4);
//  printf ("dig_H5: %04x\n", dig_H5);

  // dig_H6
  address = 0xe7; // dig_H6[7:0]
  byte = wiringPiI2CReadReg8(fd, address);
  dig_H6 |= (signed char)byte;
//  printf ("dig_H6: %02x\n", dig_H6);

  // compensate hum
  double H, H1, H2;
  H = (((double)t_fine) - 76800.0);
//  printf ("H: %5.2f\n", H);
  H = (data->hum_raw - (((double)dig_H4) * 64.0 + ((double)dig_H5) / 16384.0 * H)) * (((double)dig_H2) / 65536.0 * (1.0 + ((double)dig_H6) / 67108864.0 * H * (1.0 + ((double)dig_H3) / 67108864.0 * H)));
//  printf ("H: %5.2f%%\n", H);
  H = H * (1.0 - ((double)dig_H1) * H / 524288.0);
//  printf ("H: %5.2f%%\n", H);


    printf("Humidity:%.2f%% Temperature:%.2f*C Pressure:%.2fPa\n", H, T, 0.0 );
    syslog(LOG_INFO, "Humidity:%.2f%% Temperature:%.2f*C Pressure:%.2fPa\n", H, T, 0.0 );

	data->Humidity = H;
	data->Temperature = T;

#undef DOHERE
#ifdef DOHERE
	// simple plausibility check
	if (t < -100.0)
	{
		syslog(LOG_INFO, "error: temp < -100.0.");
		return 0;
	}
#endif
    return 1;
}

int gatherData(struct config *config, struct data *data)
// return 0 on error
{
	int fd;

	// check for device present on both addresses (0x76 and 0x77),
	// use first found
	fd = find_bme280();

	// config and execute one-shot measurement
    do_single_measurement(fd);

	// read out raw data
	read_bme280_dat(fd, data);

	// read calibration data and compensat
    compensate_data(fd, data);

	printf ("h: %5.2f%%\nt: %5.2fC\np: %5.2fPa\n", data->Humidity, data->Temperature, data->Pressure);

	return 1;
}

// send data to emonCMS
int sendToEmonCMS (struct config *config, struct data *data, int socket_fd)
{
    char tcp_buffer[1024];
    int num;

//    printf ("socket_fd: %d\n", socket_fd);

    // generate json string for emonCMS
    sprintf (tcp_buffer, "GET /input/post.json?node=\"%s-env\"&json={Humidity:%4.2f,Temperature:%4.2f}&apikey=%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s %s\r\nConnection: keep-alive\r\n\r\n", config->pNodeName, data->Humidity, data->Temperature, config->pApiKey, config->pHostName, TOOLNAME, BME280_VERSION);

    printf ("-----\nbuflen: %ld\n%s\n", strlen(tcp_buffer), tcp_buffer);
    printf ("sent: %ld\n", send(socket_fd, tcp_buffer, strlen(tcp_buffer), 0));

    return 1;   // 0 - fail; 1 - success
}
