#
# Jones: A basic rule-engine system
# Copyright (c) 2015 David Martinez Oliveira
#
# This file is part of Jones
#
# Jones is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Jones is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Jones.  If not, see <http://www.gnu.org/licenses/>.
#

AM_CFLAGS=${PFLAGS}

bin_PROGRAMS=jones


jones_SOURCES=jones.c 
jones_LDADD=${BUILD_LIBS} ./libjones.la
jones_la_CFLAGS=${PFLAGS}

lib_LTLIBRARIES = libjones.la
libjones_la_SOURCES = facts.c objs.c rules.c lena.c kb.c ipython.c 
libjones_la_LIBADD=${PLIBS} ${BUILD_LIBS}
libjones_la_CFLAGS=${PFLAGS}


EXTRA_DIST= facts.h objs.h rules.h ipython.h lena.h kb.h jones.py test.py
include_HEADERS= facts.h objs.h rules.h ipython.h lena.h kb.h

