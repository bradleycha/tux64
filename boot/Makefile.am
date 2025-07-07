#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2025                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# boot/Makefile.am - GNU Automake script for the bootloader.			 			 #
#------------------------------------------------------------------------------#

noinst_PROGRAMS = \
	stage0 \
	stage1 \
	stage2

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
stage1_BIN_BSS = \
	$(top_builddir)/stage1.bin.bss
stage2_BIN = \
	$(top_builddir)/stage2.bin

stage0_SYM = \
	$(top_builddir)/stage0.sym
stage1_SYM = \
	$(top_builddir)/stage1.sym
stage2_SYM = \
	$(top_builddir)/stage2.sym

BIN_FILES = \
	$(stage0_BIN) \
	$(stage0_BIN_CIC) \
	$(stage1_BIN) \
	$(stage1_BIN_BSS) \
	$(stage2_BIN)

SYM_FILES = \
	$(stage0_SYM) \
	$(stage1_SYM) \
	$(stage2_SYM)

AM_CFLAGS = \
	-I $(top_srcdir)/src \
	-std=c99 \
	-ffreestanding \
	-Wall \
	-Wextra \
	-Wpedantic
AM_LDFLAGS = \
	-nostartfiles

LDADD = \
	-L $(top_srcdir)/src \
	-L $(datadir) \
	$(libdir)/libtux64.a

stage0_SOURCES = \
	src/tux64-boot/stage0/stage0.s
stage1_SOURCES = \
	src/tux64-boot/stage1/stage1.c \
	src/tux64-boot/stage1/boot-header.h
stage2_SOURCES = \
	src/tux64-boot/stage2/stage2.c

noinst_HEADERS = \
	src/tux64-boot/tux64-boot.h

EXTRA_DIST = \
	src/tux64-boot/stage0/stage0.ld \
	src/tux64-boot/stage1/stage1.ld \
	src/tux64-boot/stage1/stack.ld \
	src/tux64-boot/stage1/boot-header.ld \
	src/tux64-boot/stage2/stage2.ld

stage0_LDFLAGS = \
	$(AM_LDFLAGS) \
	-T $(top_srcdir)/src/tux64-boot/stage0/stage0.ld
stage1_LDFLAGS = \
	$(AM_LDFLAGS) \
	-T $(top_srcdir)/src/tux64-boot/stage1/stage1.ld
stage2_LDFLAGS = \
	$(AM_LDFLAGS) \
	-T $(top_srcdir)/src/tux64-boot/stage2/stage2.ld

pkgdata_DATA = \
	$(BIN_FILES) $(SYM_FILES)

$(stage0_BIN) : $(stage0_ELF)
	$(OBJCOPY) $< --dump-section .rsp_dmem=$@ && chmod -x $@

$(stage0_BIN_CIC) : $(stage0_ELF)
	$(OBJCOPY) $< --dump-section .cic=$@ && chmod -x $@

$(stage1_BIN) : $(stage1_ELF)
	$(OBJCOPY) $< --dump-section .rdram=$@ && chmod -x $@

$(stage1_BIN_BSS) : $(stage1_ELF)
	$(OBJDUMP) -h $< | grep -e "\.rdram\.bss" | grep -e "[0-9]\{8\}" -o | head -n1 > $@

$(stage2_BIN) : $(stage2_ELF)
	$(OBJCOPY) $< --dump-section .rsp_imem=$@ && chmod -x $@

$(stage0_SYM) : $(stage0_ELF)
	$(OBJCOPY) --only-keep-debug $< $@ && chmod -x $@

$(stage1_SYM) : $(stage1_ELF)
	$(OBJCOPY) --only-keep-debug $< $@ && chmod -x $@

$(stage2_SYM) : $(stage2_ELF)
	$(OBJCOPY) --only-keep-debug $< $@ && chmod -x $@

all-local : $(BIN_FILES) $(SYM_FILES)

clean-local :
	rm -f $(BIN_FILES) $(SYM_FILES)

