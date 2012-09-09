dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(http2, 1.0)


AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE()

AC_ARG_ENABLE(debug, [--enable-debug	Enable debugging infomation],
			USE_DEBUG="$enablevar", USE_DEBUG="no")
if [ $USE_DEBUG = yes ] ; then
	DEBUG=1
	CFLAGS=`echo "$CFLAGS" | sed -e 's/-O[0-9s]*//g'`
	CFLAGS="$CFLAGS -g -O0 -Wall"
else 
	DEBUG=1
fi

CFLAGS=`echo "$CFLAGS" | sed -e 's/-O[0-9s]*//g'`
CFLAGS="$CFLAGS -g -O0 -Wall"

AC_PROG_CC
AM_PROG_CC_C_O
AC_CONFIG_FILES(Makefile \
	src/Makefile \
	curl/Makefile \
	lib/Makefile
)
AC_OUTPUT

