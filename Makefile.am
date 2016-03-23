# https://robots.thoughtbot.com/the-magic-behind-configure-make-make-install
# http://mij.oltrelinux.com/devel/autoconf-automake/
# https://www.gnu.org/software/automake/manual/html_node/Subdirectories-with-AM_005fCONDITIONAL.html#Subdirectories-with-AM_005fCONDITIONAL

AUTOMAKE_OPTIONS = foreign

if banana_dht22_cond
    banana_dht22_dir = banana_dht22
endif
if raspi_pulsecount_cond
    raspi_pulsecount_dir = raspi_pulsecount
endif
if bme280_cond
    bme280_dir = bme280
endif
if axp209_cond
    axp209_dir = axp209
endif

SUBDIRS = huawei raspi_internal $(raspi_pulsecount_dir) wlan $(banana_dht22_dir) $(bme280_dir) $(axp209_dir)
