#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2026                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# init/Makefile.am - GNU Automake script for tux64-init.			 			 	 	 #
#------------------------------------------------------------------------------#

ACLOCAL_AMFLAGS = -I configdir/m4

bin_PROGRAMS = \
	tux64-init

AM_CFLAGS = \
	-I $(top_srcdir)/src \
	-std=c99 \
	-Wall \
	-Wextra \
	-Wpedantic
LDADD = \
	$(libdir)/libtux64.la \
	$(libdir)/libtux64-core.a

tux64_init_SOURCES = \
	src/tux64-init/tux64-init.c \
	src/tux64-init/tux64-init.h \
	src/tux64-init/system-info.c \
	src/tux64-init/system-info.h \
	src/tux64-init/mount.c \
	src/tux64-init/mount.h

