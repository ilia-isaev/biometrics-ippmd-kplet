AC_PREREQ([2.69])
AC_INIT([kplet], [0.0.4], [dev@isaev.com.br])
AC_CANONICAL_HOST
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_SRCDIR([src/ippmd_acmp.hpp])
AC_CONFIG_HEADERS([config.h])

#PKG_CHECK_MODULES([CHECK], [check >= 0.9.10])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX

# Checks for libraries.
AC_CHECK_LIB([m],[cos])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AM_PROG_AR
LT_INIT
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects foreign])
AM_PROG_CC_C_O
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

