#------------------------------------------------------------------------------#
#                        Copyright (C) Tux64 2025, 2026                        #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# rescompiler/imagecompiler/Makefile.am - GNU Automake script for					 #
#    tux64-imagecompiler. 	 	 																 #
#------------------------------------------------------------------------------#

bin_PROGRAMS = \
	tux64-imagecompiler

AM_CFLAGS = \
	-I $(top_srcdir)/src \
	-std=c99 \
	-Wall \
	-Wextra \
	-Wpedantic
LDADD = \
	$(libdir)/libtux64.a

tux64_imagecompiler_SOURCES = \
	src/tux64-imagecompiler/tux64-imagecompiler.c \
	src/tux64-imagecompiler/tux64-imagecompiler.h \
	src/tux64-imagecompiler/arguments.c \
	src/tux64-imagecompiler/arguments.h \
	src/tux64-imagecompiler/encoder.c \
	src/tux64-imagecompiler/encoder.h \
	src/tux64-imagecompiler/compressor.c \
	src/tux64-imagecompiler/compressor.h \
	src/tux64-imagecompiler/formatter.c \
	src/tux64-imagecompiler/formatter.h

