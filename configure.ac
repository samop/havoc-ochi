#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([havoc], [0.1], [samo.penic@gmail.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
# Checks for programs.
AC_PROG_CC
AM_GNU_GETTEXT_VERSION([0.16])
AM_GNU_GETTEXT([external])

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
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile
                src/Makefile
                src/ocrlib/Makefile
                po/Makefile.in])
AC_OUTPUT
