# http://www.gnu.org/software/make/manual/make.html
# http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

CC = gcc
CFLAGS = -I.
LIBS = -lconfig

#######

.PHONY: clean all

all: huawei_emoncms raspi_internal_emoncms raspi_pulsecount_emoncms

#######

raspi_internal/raspi_internal_emoncms: raspi_internal/raspi_internal_emoncms.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

raspi_pulsecount/raspi_pulsecount_emoncms: raspi_pulsecount/raspi_pulsecount_emoncms.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -lpigpio -lpthread -lrt

huawei_emoncms: huawei/huawei_emoncms.c
	$(CC) $(CFLAGS) -o huawei/$@ $^ $(LIBS)

#######

clean:
	rm -f *.o *.d *~ core huawei/huawei_emoncms raspi_internal_emoncms/raspi_internal_emoncms raspi_pulsecount_emoncms/raspi_pulsecount_emoncms
