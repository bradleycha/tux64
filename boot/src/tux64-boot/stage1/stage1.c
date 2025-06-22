/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/stage1.c - Stage-1 bootloader main entrypoint.  */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/status.h"

/*----------------------------------------------------------------------------*/
/* BIG FAT TODO: we need to patch GCC to not use non 32-bit memory accesses.  */
/* This is because of the same hardware quirk mentioned in stage-0 with the   */
/* RCP SysAD bus.  We could eliminate 64-bit access by using -mabi=32, but    */
/* that doesn't solve the problem for 8-bit and 16-bit accesses, plus now we  */
/* can't make use of 64-bit registers.  This causes sub-optimal code for      */
/* instances such as passing 64-bit arguments to functions or doing 64-bit    */
/* math.  The only good option is to use a patched version of GCC which gives */
/* an option to fix this issue.  I'm thinking the option could be something   */
/* like "-mfix-rcp-sysad", which would be a MIPS-specific machine option      */
/* which does the following:                                                  */
/*                                                                            */
/* lb/lh    r0,offset(r1) --> lw    r0,ALIGN32(r1 + offset)                   */
/*                            sll   r0,r0,DISCARD_UPPER                       */
/*                            sra   r0,r0,DISCARD_UPPER + DISCARD_LOWER       */
/*                                                                            */
/* lbu/lhu  r0,offset(r1) --> lw    r0,ALIGN32(r1 + offset)                   */
/*                            sll   r0,r0,DISCARD_UPPER                       */
/*                            srl   r0,r0,DISCARD_UPPER + DISCARD_LOWER       */
/*                                                                            */
/* ld       r0,offset(r1) --> lw    r0,offset+0x00(r1)                        */
/*                            dsll  r0,r0,32                                  */
/*                            lwr   $at,offset+0x04(r1)                       */
/*                            or    r0,r0,$at                                 */
/*                                                                            */
/* sb/sh    r0,offset(r1) --> lw    $at,ALIGN32(r1 + offset)                  */
/*                            andi  $at,$at,BIT_SLOT_MASK                     */
/*                            andi  r0,r0,~BIT_SLOT_MASK                      */
/*                            or    $at,$at,r0                                */
/*                            sw    $at,ALIGN32(r1 + offset)                  */
/*                                                                            */
/* sd       r0,offset(r1) --> sw    r0,offset+0x00(r1)                        */
/*                            dsrl  r0,r0,32                                  */
/*                            sw    r0,offset+0x04(r1)                        */
/*                                                                            */
/* Note that these substitutions are not final, and there may be more optimal */
/* ways to do the above (lwl? lwr? RTFM!).  This is more to show that we can  */
/* in fact emulate these instructions using only 32-bit accesses.             */
/*                                                                            */
/* ..and yes, I would rather create and use a patched C compiler than write   */
/* this in assembly.  Mostly for readability/maintainability and avoiding     */
/* code duplication, but also because I'd rather give myself 10 paper cuts    */
/* on my ballsack and then dip it in salted lemon juice and battery acid for  */
/* an hour than write the RDRAM initialization code in pure assembly.         */
/*----------------------------------------------------------------------------*/

void
tux64_boot_stage1_start(void)
__attribute__((noreturn, section(".start")));

void
tux64_boot_stage1_start(void) {
   tux64_boot_stage1_status_initialize();
   tux64_boot_stage1_status_code_set(TUX64_BOOT_STAGE1_STATUS_CODE_BEGIN);

   /* TODO: implement */
   TUX64_HALT;
}

