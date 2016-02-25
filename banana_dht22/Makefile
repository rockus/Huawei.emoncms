# http://www.gnu.org/software/make/manual/make.html
# http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

CC = gcc
CFLAGS = -I.
LIBS = -lconfig

#######

.PHONY: clean all

all: banana_dht22_emoncms

#######

banana_dht22_emoncms: gatherAndSend.c banana_dht22_emoncms.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -L /usr/local/lib -lwiringPi

#######

clean:
	rm -f *.o *.d *~ core banana_dht22/banana_dht22_emoncms
