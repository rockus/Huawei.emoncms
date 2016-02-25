# http://www.gnu.org/software/make/manual/make.html
# http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

CC = gcc
CFLAGS = -I.
LIBS = -lconfig

#######

.PHONY: clean all

all: huawei_emoncms raspi_internal_emoncms raspi_pulsecount_emoncms wlan_emoncms banana_dht22_emoncms

#######

raspi_internal_emoncms: recursive
	make -C raspi_internal/

raspi_pulsecount_emoncms: recursive
	make -C raspi_pulsecount/

huawei_emoncms: recursive
	make -C huawei/

wlan_emoncms: recursive
	make -C wlan/

#banana_dht22_emoncms: banana_dht22/gatherAndSend.c banana_dht22/banana_dht22_emoncms.c
#	$(CC) $(CFLAGS) -o banana_dht22/$@ $^ $(LIBS) -L /usr/local/lib -lwiringPi
banana_dht22_emoncms: recursive
	make -C banana_dht22/

#######

clean:
	rm -f *.o *.d *~ core huawei/huawei_emoncms raspi_internal_emoncms/raspi_internal_emoncms raspi_pulsecount_emoncms/raspi_pulsecount_emoncms banana_dht22/banana_dht22_emoncms wlan/wlan_emoncms

#######

recursive:
	true
