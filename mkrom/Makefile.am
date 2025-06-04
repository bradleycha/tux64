#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2025                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# mkrom/Makefile.am - GNU Automake script for tux64-mkrom.			 			 	 #
#------------------------------------------------------------------------------#

bin_PROGRAMS = \
	tux64-mkrom

AM_CFLAGS = \
	-I $(top_srcdir)/src \
	-std=c99 \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Werror
LDADD = \
	$(libdir)/libtux64.a

tux64_mkrom_SOURCES = \
	src/tux64-mkrom/tux64-mkrom.c \
	src/tux64-mkrom/tux64-mkrom.h

