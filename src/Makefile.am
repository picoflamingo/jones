#AM_CFLAGS=-I$(srcdir)/../include

AM_CFLAGS=${PFLAGS}

bin_PROGRAMS=jones


jones_SOURCES=jones.c facts.c objs.c rules.c list.c ipython.c
jones_LDADD=${PLIBS}
#jones_LDADD=./libmia.la



# mobiNET Libraries

EXTRA_DIST= facts.h objs.h rules.h list.h ipython.h
include_HEADERS= facts.h objs.h rules.h list.h ipython.h

#lib_LTLIBRARIES = libmia.la
#libmia_la_SOURCES= mia.c
#libmia_la_LIBADD=-lbluetooth

