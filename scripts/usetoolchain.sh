#!/bin/sh

# ----------------------------------------------------------------------------
#                          Copyright (c) Tux64 2025
# ----------------------------------------------------------------------------
# This file is licensed under the GPLv3 license.  For more information, see
# LICENSE.
# ----------------------------------------------------------------------------
# usetoolchain.sh - Configures the toolchain for a GNU autoconf script.
# ----------------------------------------------------------------------------

export CC=${1}-gcc
export CXX=${1}-g++
export AR=${1}-ar
export AS=${1}-as
export DSYMUTIL=${1}-dsymutil
export LD=${1}-ld
export NM=${1}-nm
export RANLIB=${1}-ranlib
export STRIP=${1}-strip
export OBJCOPY=${1}-objcopy
export OBJDUMP=${1}-objdump
export OTOOL=${1}-otool
export READELF=${1}-readelf

if [ -v 2 ]; then
   export CC_FOR_TARGET=${2}-gcc
   export CXX_FOR_TARGET=${2}-g++
   export AR_FOR_TARGET=${2}-ar
   export AS_FOR_TARGET=${2}-as
   export DSYMUTIL_FOR_TARGET=${2}-dsymutil
   export LD_FOR_TARGET=${2}-ld
   export NM_FOR_TARGET=${2}-nm
   export RANLIB_FOR_TARGET=${2}-ranlib
   export STRIP_FOR_TARGET=${2}-strip
   export OBJCOPY_FOR_TARGET=${2}-objcopy
   export OBJDUMP_FOR_TARGET=${2}-objdump
   export OTOOL_FOR_TARGET=${2}-otool
   export READELF_FOR_TARGET=${2}-readelf;
fi

