#!/bin/sh

# ----------------------------------------------------------------------------
#                          Copyright (c) Tux64 2025
# ----------------------------------------------------------------------------
# usetoolchain.sh - Configures the toolchain for a GNU autoconf script.
# ----------------------------------------------------------------------------

CC=${1}-gcc
CXX=${1}-g++
AR=${1}-ar
AS=${1}-as
DSYMUTIL=${1}-dsymutil
LD=${1}-ld
NM=${1}-nm
RANLIB=${1}-ranlib
STRIP=${1}-strip
OBJCOPY=${1}-objcopy
OBJDUMP=${1}-objdump
OTOOL=${1}-otool
READELF=${1}-readelf

if [ -v 2 ]; then
   CC_FOR_TARGET=${2}-gcc
   CXX_FOR_TARGET=${2}-g++
   AR_FOR_TARGET=${2}-ar
   AS_FOR_TARGET=${2}-as
   DSYMUTIL_FOR_TARGET=${2}-dsymutil
   LD_FOR_TARGET=${2}-ld
   NM_FOR_TARGET=${2}-nm
   RANLIB_FOR_TARGET=${2}-ranlib
   STRIP_FOR_TARGET=${2}-strip
   OBJCOPY_FOR_TARGET=${2}-objcopy
   OBJDUMP_FOR_TARGET=${2}-objdump
   OTOOL_FOR_TARGET=${2}-otool
   READELF_FOR_TARGET=${2}-readelf;
fi

