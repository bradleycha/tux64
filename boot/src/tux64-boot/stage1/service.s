#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2025                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# boot/src/tux64-boot/stage1/service.s - Low-level interrupt servicing code.   #
#------------------------------------------------------------------------------#

# we want to prevent clobbering of registers, specifically $at, and we also want
# precise control, so we disable assembler sugar.
.set noreorder
.set noat
.set nomacro

# the high-level interrupt handler code which does the real work.
.extern tux64_boot_stage1_interrupt_handler

# the number of general-purpose registers to preserve.  we only care about
# volatile integer registers, because we shouldn't be using floating-point in
# the interrupt handler.  if you do, you're dumb as fuck and deserve to crash.
# this includes $t0-$t9, $v0-$v1, $a0-$a3, $at, $ra, LO, and HI.
.equ TUX64_BOOT_STAGE1_SERVICE_REGISTER_COUNT,20

   .section .text
   .global tux64_boot_stage1_service_exception
tux64_boot_stage1_service_exception:
   # make stack space for volatile general-purpose registers
   addiu $sp,$sp,-(TUX64_BOOT_STAGE1_SERVICE_REGISTER_COUNT*8)

   # preserve all the volatile registers
   sd    $t0,  0($sp)
   sd    $t1,  8($sp)
   sd    $t2, 16($sp)
   sd    $t3, 24($sp)
   sd    $t4, 32($sp)
   sd    $t5, 40($sp)
   sd    $t6, 48($sp)
   sd    $t7, 56($sp)
   sd    $t8, 64($sp)
   sd    $t9, 72($sp)
   mflo  $t0
   mfhi  $t1
   sd    $v0, 80($sp)
   sd    $v1, 88($sp)
   sd    $a0, 96($sp)
   sd    $a1,104($sp)
   sd    $a2,112($sp)
   sd    $a3,120($sp)
   sd    $at,128($sp)
   sd    $ra,136($sp)
   sd    $t0,144($sp) # lo

   # we can now safely proceed to the high-level interrupt handler
   jal   tux64_boot_stage1_interrupt_handler
   sd    $t1,152($sp) # hi, executed in branch delay slot

   # restore the volatile registers
   ld    $t1,152($sp) # hi
   ld    $t0,144($sp) # lo
   ld    $ra,136($sp)
   ld    $at,128($sp)
   ld    $a3,120($sp)
   ld    $a2,112($sp)
   ld    $a1,104($sp)
   ld    $a0, 96($sp)
   ld    $v1, 88($sp)
   ld    $v0, 80($sp)
   mthi  $t1
   mtlo  $t0
   ld    $t9, 72($sp)
   ld    $t8, 64($sp)
   ld    $t7, 56($sp)
   ld    $t6, 48($sp)
   ld    $t5, 40($sp)
   ld    $t4, 32($sp)
   ld    $t3, 24($sp)
   ld    $t2, 16($sp)
   ld    $t1,  8($sp)
   ld    $t0,  0($sp)

   # restore old stack and get the hell out of here!
   addiu $sp,$sp,(TUX64_BOOT_STAGE1_SERVICE_REGISTER_COUNT*8)
   eret
#tux64_boot_stage1_service_exception

