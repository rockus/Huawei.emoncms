# http://www.gnu.org/software/make/manual/make.html
# http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

CC = gcc
CFLAGS = -I.
LIBS = -lconfig

#######

.PHONY: clean all

all: raspi_internal_emoncms

#######

raspi_internal_emoncms: raspi_internal_emoncms.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

#######

clean:
	rm -f *.o *.d *~ core raspi_internal_emoncms 
