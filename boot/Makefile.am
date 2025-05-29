#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2025                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# boot/Makefile.am - GNU Automake script for the bootloader.			 			 #
#------------------------------------------------------------------------------#

bin_PROGRAMS = \
	stage0 \
	stage1 \
	stage2

AM_CFLAGS = \
	-I $(top_srcdir)/src \
	-std=c99 \
	-ffreestanding \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Werror

stage0_SOURCES = \
	src/stage0/stage0.ld \
	src/stage0/stage0.s
stage1_SOURCES = \
	src/stage1/stage1.ld \
	src/stage1/stage1.c
stage2_SOURCES = \
	src/stage2/stage2.ld \
	src/stage2/stage2.c

stage0_LDFLAGS = \
	-T $(top_srcdir)/src/stage0/stage0.ld
stage1_LDFLAGS = \
	-T $(top_srcdir)/src/stage1/stage1.ld
stage2_LDFLAGS = \
	-T $(top_srcdir)/src/stage2/stage2.ld

stage0_ELF = \
	$(top_builddir)/stage0
stage1_ELF = \
	$(top_builddir)/stage1
stage2_ELF = \
	$(top_builddir)/stage2

stage0_BIN = \
	$(top_builddir)/stage0.bin
stage0_BIN_CIC = \
	$(top_builddir)/stage0.bin.cic
stage1_BIN = \
	$(top_builddir)/stage1.bin
stage2_BIN = \
	$(top_builddir)/stage2.bin

BIN_FILES = \
	$(stage0_BIN) \
	$(stage0_BIN_CIC) \
	$(stage1_BIN) \
	$(stage2_BIN)

$(stage0_BIN) : $(stage0_ELF)
	$(OBJCOPY) $< --dump-section .stage0.rsp_dmem=$@
	chmod -x $@

$(stage0_BIN_CIC) : $(stage0_ELF)
	$(OBJCOPY) $< --dump-section .stage0.cic=$@
	chmod -x $@

$(stage1_BIN) : $(stage1_ELF)
	$(OBJCOPY) $< --dump-section .stage1.rdram=$@
	chmod -x $@

$(stage2_BIN) : $(stage2_ELF)
	$(OBJCOPY) $< --dump-section .stage2.rsp_dmem=$@
	chmod -x $@

all-local : $(BIN_FILES)

clean-local :
	rm -f $(BIN_FILES)

# TODO: install .bin files instead of ELF files

