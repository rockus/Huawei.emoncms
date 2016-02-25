# http://www.gnu.org/software/make/manual/make.html
# http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

CC = gcc
CFLAGS = -I.
LIBS = -lconfig

#######

.PHONY: clean all

all: wlan_emoncms

#######

wlan_emoncms: wlan_emoncms.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

#######

clean:
	rm -f *.o *.d *~ core wlan_emoncms
