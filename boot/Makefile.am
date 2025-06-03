#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2025                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# boot/Makefile.am - GNU Automake script for the bootloader.			 			 #
#------------------------------------------------------------------------------#

noinst_PROGRAMS = \
	stage0 \
	stage1 \
	stage2 \
	stage3

AM_CFLAGS = \
	-I $(top_srcdir)/src \
	-std=c99 \
	-ffreestanding \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Werror
LDADD = \
	-L $(top_srcdir)/src/tux64-boot \
	-L $(datadir) \
	$(libdir)/libtux64.a

stage0_SOURCES = \
	src/tux64-boot/stage0/stage0.s
stage1_SOURCES = \
	src/tux64-boot/stage1/stage1.c
stage2_SOURCES = \
	src/tux64-boot/stage2/stage2.c
stage3_SOURCES = \
	src/tux64-boot/stage3/stage3.c \
	src/tux64-boot/stage3/payload.h

noinst_HEADERS = \
	src/tux64-boot/tux64-boot.h

EXTRA_DIST = \
	src/tux64-boot/stage0/stage0.ld \
	src/tux64-boot/stage1/stage1.ld \
	src/tux64-boot/stage2/stage2.ld \
	src/tux64-boot/stage3/stage3.ld \
	src/tux64-boot/stage3/payload.ld

stage0_LDFLAGS = \
	-T $(top_srcdir)/src/tux64-boot/stage0/stage0.ld
stage1_LDFLAGS = \
	-T $(top_srcdir)/src/tux64-boot/stage1/stage1.ld
stage2_LDFLAGS = \
	-T $(top_srcdir)/src/tux64-boot/stage2/stage2.ld
stage3_LDFLAGS = \
	-T $(top_srcdir)/src/tux64-boot/stage3/stage3.ld

stage0_ELF = \
	$(top_builddir)/stage0
stage1_ELF = \
	$(top_builddir)/stage1
stage2_ELF = \
	$(top_builddir)/stage2
stage3_ELF = \
	$(top_builddir)/stage3

stage0_BIN = \
	$(top_builddir)/stage0.bin
stage0_BIN_CIC = \
	$(top_builddir)/stage0.bin.cic
stage1_BIN = \
	$(top_builddir)/stage1.bin
stage2_BIN = \
	$(top_builddir)/stage2.bin
stage2_BIN_BSS = \
	$(top_builddir)/stage2.bin.bss
stage3_BIN = \
	$(top_builddir)/stage3.bin

BIN_FILES = \
	$(stage0_BIN) \
	$(stage0_BIN_CIC) \
	$(stage1_BIN) \
	$(stage2_BIN) \
	$(stage2_BIN_BSS) \
	$(stage3_BIN)

pkgdata_DATA = \
	$(BIN_FILES)

$(stage0_BIN) : $(stage0_ELF)
	$(OBJCOPY) $< --dump-section .rsp_dmem=$@ && chmod -x $@

$(stage0_BIN_CIC) : $(stage0_ELF)
	$(OBJCOPY) $< --dump-section .cic=$@ && chmod -x $@

$(stage1_BIN) : $(stage1_ELF)
	$(OBJCOPY) $< --dump-section .rsp_imem=$@ && chmod -x $@

$(stage2_BIN) : $(stage2_ELF)
	$(OBJCOPY) $< --dump-section .rdram=$@ && chmod -x $@

# This is ridiculous, but it's the easiest way I could figure out how to do
# this, as in not requiring custom tools to be written.  If you know a better
# way, *please* let me know.
$(stage2_BIN_BSS) : $(stage2_ELF)
	$(OBJDUMP) -h $< | grep -e "\.rdram\.bss" | grep -e "[0-9]\{8\}" -o | head -n1 > $@

$(stage3_BIN) : $(stage3_ELF)
	$(OBJCOPY) $< --dump-section .rsp_imem=$@ && chmod -x $@

all-local : $(BIN_FILES)

clean-local :
	rm -f $(BIN_FILES)

