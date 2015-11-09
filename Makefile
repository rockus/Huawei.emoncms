# http://www.gnu.org/software/make/manual/make.html
# http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

CC = gcc
CFLAGS = -I.
LIBS = -lconfig

#######

.PHONY: clean all

all: huawei_emoncms raspi_internal_emoncms raspi_pulsecount_emoncms banana_dht22_emoncms wlan_emoncms

#######

raspi_internal_emoncms: raspi_internal/raspi_internal_emoncms.c
	$(CC) $(CFLAGS) -o raspi_internal/$@ $^ $(LIBS)

raspi_pulsecount_emoncms: raspi_pulsecount/raspi_pulsecount_emoncms.c
	$(CC) $(CFLAGS) -o raspi_pulsecount/$@ $^ $(LIBS) -lpigpio -lpthread -lrt

huawei_emoncms: huawei/huawei_emoncms.c
	$(CC) $(CFLAGS) -o huawei/$@ $^ $(LIBS)

banana_dht22_emoncms: banana_dht22/gatherAndSend.c banana_dht22/banana_dht22_emoncms.c
	$(CC) $(CFLAGS) -o banana_dht22/$@ $^ $(LIBS) -L /usr/local/lib -lwiringPi

wlan_emoncms: wlan/wlan_emoncms.c
	$(CC) $(CFLAGS) -o wlan/$@ $^ $(LIBS)

#######

clean:
	rm -f *.o *.d *~ core huawei/huawei_emoncms raspi_internal_emoncms/raspi_internal_emoncms raspi_pulsecount_emoncms/raspi_pulsecount_emoncms banana_dht22/banana_dht22_emoncms wlan/wlan_emoncms
