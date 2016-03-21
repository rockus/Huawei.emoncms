
# https://robots.thoughtbot.com/the-magic-behind-configure-make-make-install
# https://autotools.io/autoconf/arguments.html
# https://www.gnu.org/software/automake/manual/html_node/Subdirectories-with-AM_005fCONDITIONAL.html#Subdirectories-with-AM_005fCONDITIONAL
# https://github.com/securestate/kraken/blob/master/configure.ac

#    aclocal && automake --add-missing && autoconf && ./configure && make

AC_INIT(emoncms.modules, 1.0, rockus@rockus.at)
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_CONFIG_FILES([Makefile huawei/Makefile raspi_internal/Makefile raspi_pulsecount/Makefile wlan/Makefile banana_dht22/Makefile bme280/Makefile])

dnl Banana-only code enabled by default
AC_ARG_ENABLE([banana], AS_HELP_STRING(--disable-banana, [disable Banana-only modules]))
AM_CONDITIONAL([banana_cond], [test "x$enable_banana" != xno])

AC_OUTPUT
