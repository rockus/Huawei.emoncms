
# https://robots.thoughtbot.com/the-magic-behind-configure-make-make-install
# https://autotools.io/autoconf/arguments.html
# https://www.gnu.org/software/automake/manual/html_node/Subdirectories-with-AM_005fCONDITIONAL.html#Subdirectories-with-AM_005fCONDITIONAL
# https://github.com/securestate/kraken/blob/master/configure.ac

#    aclocal && automake --add-missing && autoconf && ./configure && make

AC_INIT(emoncms.modules, 2.0, rockus@rockus.at)
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_CONFIG_FILES([Makefile huawei/Makefile raspi_internal/Makefile raspi_pulsecount/Makefile wlan/Makefile dht22/Makefile bme280/Makefile axp209/Makefile])

dnl dht22 code enabled by default
AC_ARG_ENABLE([dht22], AS_HELP_STRING(--disable-dht22, [disable dht22 module]))
AM_CONDITIONAL([dht22_cond], [test "x$enable_dht22" != xno])

dnl raspi_pulsecount code enabled by default
AC_ARG_ENABLE([raspi_pulsecount], AS_HELP_STRING(--disable-raspi_pulsecount, [disable raspi_pulsecount module]))
AM_CONDITIONAL([raspi_pulsecount_cond], [test "x$enable_raspi_pulsecount" != xno])

dnl bme280 code enabled by default
AC_ARG_ENABLE([bme280], AS_HELP_STRING(--disable-bme280, [disable bme280 module]))
AM_CONDITIONAL([bme280_cond], [test "x$enable_bme280" != xno])

dnl axp209 code enabled by default
AC_ARG_ENABLE([axp209], AS_HELP_STRING(--disable-axp209, [disable axp209 module]))
AM_CONDITIONAL([axp209_cond], [test "x$enable_axp209" != xno])



dnl all modules need libconfig
AC_CHECK_LIB([config], [config_init], [], [AC_MSG_ERROR([Library config9 not found.])])


dnl WiringPi required by dht22, bme280, axp209
dnl PiGPIO reqired by raspi_pulsecount

dnl for libpigpio
LIBS="$LIBS -lrt"

need_libwiringpi="no"
need_libpigpio="no"

if test "x$enable_raspi_pulsecount" != "xno"; then
dnl  need_libwiringpi="yes"
  need_libpigpio="yes"
fi
if test "x$enable_dht22" != "xno"; then
  need_libwiringpi="yes"
fi
if test "x$enable_bme280" != "xno"; then
  need_libwiringpi="yes"
fi
if test "x$enable_axp209" != "xno"; then
  need_libwiringpi="yes"
fi


if test "x$need_libwiringpi" != "xno"; then
  AC_CHECK_LIB([wiringPi], [wiringPiSetup], [], [AC_MSG_ERROR([Library wiringPi not found.])])
fi
if test "x$need_libpigpio" != "xno"; then
  AC_CHECK_LIB([pigpio], [gpioInitialise], [], [AC_MSG_ERROR([Library PiGpio not found.])])
fi

AC_OUTPUT
