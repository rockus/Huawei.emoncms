# http://www.gnu.org/software/make/manual/make.html
# http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

CC = gcc
CFLAGS = -I.
LIBS = -lconfig

#######

.PHONY: clean all

all: raspi_pulsecount_emoncms 

#######

raspi_pulsecount_emoncms: raspi_pulsecount_emoncms.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -lpigpio -lpthread -lrt

#######

clean:
	rm -f *.o *.d *~ core raspi_pulsecount_emoncms
