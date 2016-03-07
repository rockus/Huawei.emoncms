# https://robots.thoughtbot.com/the-magic-behind-configure-make-make-install
# http://mij.oltrelinux.com/devel/autoconf-automake/
# https://www.gnu.org/software/automake/manual/html_node/Subdirectories-with-AM_005fCONDITIONAL.html#Subdirectories-with-AM_005fCONDITIONAL

AUTOMAKE_OPTIONS = foreign

if banana_cond
    banana_dir = banana_dht22
endif
SUBDIRS = huawei raspi_internal raspi_pulsecount wlan $(banana_dir)
