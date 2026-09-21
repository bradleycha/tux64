#------------------------------------------------------------------------------#
#                        Copyright (c) Tux64 2025, 2026                        #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# mkrom/Makefile.am - GNU Automake script for tux64-mkrom.			 			 	 #
#------------------------------------------------------------------------------#

ACLOCAL_AMFLAGS = -I configdir/m4

bin_PROGRAMS = \
	tux64-mkrom

AM_CFLAGS = \
	-I $(top_srcdir)/src \
	-std=c99 \
	-Wall \
	-Wextra \
	-Wpedantic
LDADD = \
	$(libdir)/libtux64.la \
	$(libdir)/libtux64-core.a

tux64_mkrom_SOURCES = \
	src/tux64-mkrom/tux64-mkrom.c \
	src/tux64-mkrom/tux64-mkrom.h \
	src/tux64-mkrom/arguments.c \
	src/tux64-mkrom/arguments.h \
	src/tux64-mkrom/file.c \
	src/tux64-mkrom/file.h \
	src/tux64-mkrom/load.c \
	src/tux64-mkrom/load.h \
	src/tux64-mkrom/image.c \
	src/tux64-mkrom/image.h

