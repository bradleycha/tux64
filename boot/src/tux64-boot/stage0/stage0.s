#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2025                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# boot/src/tux64-boot/stage0/stage0.s - Stage-0 bootloader code and CIC data.  #
#------------------------------------------------------------------------------#

# The stage-0 bootloader runs from RSP DMEM + 0x40, directly after the ROM
# header.  At this point, RDRAM is uninitialized, and the CPU's cache is
# uninitialized.  The main goal of the stage-0 bootloader is to load the
# stage-1 bootloader into RSP IMEM.  We leave initialization of RDRAM and the
# CPU's cache to stage-1 because stage-0 must match the CIC-6102 checksum.
# This is some copy-protection crap where the IPL2 uses the cartridge's CIC
# chip to compute a checksum of the expected IPL3.  If they don't match, the
# CPU will have NMI permanently held down until the console reboots.
# 
# We use assembly here because we must have 100% replicable machine code, as
# to bypass the IPL2's and CIC's protection, we brute-force the checksum to
# match, and thus require consistent machine code.  The rest of the boot
# process uses C instead.

   .section .start
tux64_boot_stage0_start:
   # TODO: implement
   j     tux64_boot_stage0_start
#tux64_boot_stage0_start

   .section .cic
tux64_boot_stage0_cic:
   .word 0x00001431
   .word 0xf0f44dff
#tux64_boot_stage0_cic

