dnl picoFlamingo
dnl Author: David Martinez Oliveira <dmo@papermint-designs.com>
dnl
dnl __LICENSE__
dnl


m4_include([version.m4])
AC_INIT([jones], VERSION_NUMBER, [dmo@papermint-designs.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_CONFIG_SRCDIR([src/jones.c])
AC_CONFIG_MACRO_DIR([m4])


# Checks for programs
AC_PROG_CC
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

# Checks for libraries
# FIXME: Here we should check for OpenScenegraph, xine, etc...
AC_CHECK_LIB(dl, main)

# Checks for header files
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

# Cheks for typedefs, structures and compiler characteristics
# Checks for library functions

# Configure options
AC_ARG_ENABLE(debug,
              [  --enable-debug          Enable debugging information],
              USE_DEBUG="$enableval", USE_DEBUG="no")


if test $USE_DEBUG = yes ; then 
  DEBUG=1
  BUILD_FLAGS="-Wall -g -O0 -DDEBUG "
else
  BUILD_FLAGS="-Wall -O3 "
  DEBUG=0
fi


CFLAGS=" ${BUILD_FLAGS}"
AC_SUBST(BUILD_FLAGS)

MY_TOP_DIR="`pwd`"

PFLAGS=`python-config --cflags`
PLIBS=`python-config --libs`

AC_SUBST(PFLAGS)
AC_SUBST(PLIBS)

AC_SUBST(MY_TOP_DIR)

dnl Uncomment for special management of prefix variable
dnl 
if test "$prefix" = "NONE"; then
	JONES_BASEDIR="/usr/local"
else
	JONES_BASEDIR="$prefix"
fi

dnl if test "$PLATFORM" = ""; then
dnl 	PLATFORM=x11
dnl	AC_SUBST(PLATFORM)
dnl fi

JONES_DATADIR="$JONES_BASEDIR/share/jones"

AC_SUBST(JONES_BASEDIR)
AC_SUBST(JONES_DATADIR)

# Output Files
AC_CONFIG_FILES([Makefile \
		doc/Makefile \
		src/Makefile])
AC_OUTPUT


AC_MSG_RESULT(--- Building for $build_cpu in a $build_os)
AC_MSG_RESULT(--- OS          : ${build_os})
AC_MSG_RESULT(--- HOST        : ${host})
AC_MSG_RESULT(--- Prefix      : ${JONES_BASEDIR})
AC_MSG_RESULT(--- Data dir    : ${JONES_DATADIR})
AC_MSG_RESULT(--- BUILD_FLAGS : ${BUILD_FLAGS})
AC_MSG_RESULT(--- PFLAGS      : ${PFLAGS})
AC_MSG_RESULT(--- PLIBS       : ${PLIBS})
