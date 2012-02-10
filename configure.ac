#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([havoc], [0.2], [samo.penic@gmail.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
# Checks for programs.
AC_PROG_CC
AC_LANG([C])
AM_GNU_GETTEXT_VERSION([0.16])
AM_GNU_GETTEXT([external])

# This adds the option of compiling without using the pqsql library,
AC_ARG_ENABLE(pqsql,
  [ --disable-pqsql   Disable compilation with pqsql and database access],
  [case "${enableval}" in
     yes | no ) WITH_PQSQL="${enableval}" ;;
     *) AC_MSG_ERROR(bad value ${enableval} for --disable-pqsql) ;;
   esac],
  [WITH_PQSQL="yes"]
)

dnl Make sure we register this option with Automake
AM_CONDITIONAL([WITH_PQSLQ], [test "x$WITH_PQSQL" = "xyes"])

# Define CTEMPLATE in config.h if we're going to compile against it
if test "x$WITH_PQSQL" = "xyes"; then
    AC_DEFINE([PQSQL], [], ["build using pqsql library"])
    AC_SEARCH_LIBS([PQconnectdb],[pq],, echo 'Install libpq-dev';exit 1)
    AC_MSG_NOTICE([pqsql will be used, so database is enabled])
else
    AC_MSG_NOTICE([pqsql will not be used, so database is disabled])
fi

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h leptonica/allheaders.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T


# Checks for library functions.
AC_SEARCH_LIBS([atan2],[m])
AC_SEARCH_LIBS([TIFFOpen],[tiff],, echo 'Install libtiff-dev'; exit 1)
AC_SEARCH_LIBS([png_create_info_struct],[png],, echo 'Install libpng-dev'; exit
1)
AC_SEARCH_LIBS([pixMorphSequence], [lept],, echo 'Install libleptonica-dev'; exit 1)
AC_SEARCH_LIBS([zbar_image_scanner_create], [zbar],, echo 'Install libzbar-dev';exit 1)
AC_SEARCH_LIBS([cfg_init], [confuse],, echo 'Install libconfuse-dev';exit 1)
AC_SEARCH_LIBS([gzgets],[z],, echo 'Install zlib1g-dev'; exit 1)
# AC_FUNC_MALLOC

AC_MSG_CHECKING([libleptonica findFileFormat interface])
AC_LINK_IFELSE(
  AC_LANG_PROGRAM([[#include <leptonica/allheaders.h>]],
	[[      int i;
		findFileFormat("abc",NULL);
		return 0;]]),
  [AC_MSG_RESULT([Have to use new libleptonica findFileFormat]);AC_DEFINE([NEWLEPT],[],[Use new leptonica])],
  [AC_MSG_RESULT([Have to use old libleptonica findFileFormat])]
)

AC_CONFIG_FILES([Makefile
                src/Makefile
                src/ocrlib/Makefile
                share/Makefile
                etc/Makefile
                po/Makefile.in])
AC_OUTPUT
