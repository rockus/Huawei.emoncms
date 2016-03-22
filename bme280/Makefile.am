# http://www.gnu.org/software/make/manual/make.html
# http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
# http://mij.oltrelinux.com/devel/autoconf-automake/
# http://inti.sourceforge.net/tutorial/libinti/autotoolsproject.html

#######

AM_LDFLAGS = -lconfig -L /usr/local/lib -lwiringPi

sbin_PROGRAMS = bme280_emoncms

bme280_emoncms_SOURCES = bme280_emoncms.c gatherAndSend.c
