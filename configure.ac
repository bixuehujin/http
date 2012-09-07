dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(http2, 1.0)


AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE()

AC_ARG_ENABLE(debug, [--enable-debug	Enable debugging infomation],
			USE_DEBUG="$enablevar", USE_DEBUG="no")
if [ $USE_DEBUG = yes ] ; then
	DEBUG=1
	CFLAGS="$CFLAGS -g"
else 
	DEBUG=1
fi

AC_PROG_CC

AC_CONFIG_FILES(Makefile \
	src/Makefile \
	curl/Makefile \
	lib/Makefile
)
AC_OUTPUT

