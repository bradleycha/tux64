#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2025                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# boot/src/stage0/stage0.s - Stage-0 bootloader code and CIC data.             #
#------------------------------------------------------------------------------#

# The stage-0 bootloader runs from RSP DMEM + 0x40, directly after the ROM
# header.  At this point, RDRAM is uninitialized, and the CPU's cache is
# uninitialized.  The main goal of the stage-0 bootloader is as follows:
# 
#    1. Initialize RDRAM and detect how much memory is available.
# 
#    2. Initialize the CPU cache
# 
#    3. Read the bootloader's header to get the location and length of the
#       stage-1 bootloader.
# 
#    4. Use RSP DMA to efficiently load the stage-1 bootloader into the start of
#       RDRAM, as well as the bootloader's header and our memory configuration.
# 
#    5. Run the stage-1 bootloader.
# 
# This process is already complex, especially RDRAM initialization.  However,
# this is further complexified by the CIC-6102.  The IPL2, which is baked into
# the PIF ROM and cannot be bypassed, first checks to make sure the checksum of
# the IPL3 matches what is expected from the cartridge's CIC.  This prevents
# running custom IPL3 replacements.  However, we can use ipl3hasher to quickly
# find ways to add data to our own IPL3 in order to get a checksum collision.
# This is the purpose of stage0.bin.cic.  We append stage0.bin.cic to the end
# of RSP DMEM, padding stage0.bin with zeroes along the way, and this will
# ensure that stage-0's CIC-6102 checksum matches Nintendo's 6102 IPL3, and the
# console will boot.
# 
# Also, it's worth noting that the reason we write the entire stage-0 bootloader
# in assembly is to ensure that the stage-0 machine code is 100% consistent no
# matter what.  If we were to use a C-compiler instead, different compiler flags
# and compiler versions could change the machine code, this requiring
# stage0.bin.cic to be recalculated for every single build.  The CIC doesn't
# touch stage-1 and stage-2, so these can be written in compiled languages.

   .section .start
tux64_boot_stage0_start:
   # TODO: implement
   j     tux64_boot_stage0_start
#tux64_boot_stage0_start

   .section .cic
tux64_boot_stage0_cic:
   # TODO: implement
   .word 0xfee1dead
#tux64_boot_stage0_cic

