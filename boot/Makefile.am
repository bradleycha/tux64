#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2025                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# boot/Makefile.am - GNU Automake script for the bootloader.			 			 #
#------------------------------------------------------------------------------#

# TODO: remove this once we install N64 system files using another project
LDADD = \
	-L $(top_srcdir)/src

bin_PROGRAMS = \
	stage0

stage0_LDFLAGS = \
	-T $(top_srcdir)/src/stage0/stage0.ld

stage0_SOURCES = \
	src/stage0/stage0.s \
	src/stage0/stage0.ld

stage0_ELF = \
	$(top_builddir)/stage0
stage0_BIN = \
	$(top_builddir)/stage0.bin
stage0_BIN_CIC = \
	$(top_builddir)/stage0.bin.cic

$(stage0_BIN) : $(stage0_ELF)
	$(OBJCOPY) $< --dump-section .stage0.rsp_dmem=$@
	chmod -x $@

$(stage0_BIN_CIC) : $(stage0_ELF)
	$(OBJCOPY) $< --dump-section .stage0.cic=$@
	chmod -x $@

all-local : $(stage0_BIN) $(stage0_BIN_CIC)

clean-local :
	rm -f $(top_builddir)/*.bin*

# TODO: install .bin files instead of ELF files

