/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform/mips/n64/sp.h - RSP registers header.               */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_MIPS_N64_SP_H
#define _TUX64_PLATFORM_MIPS_N64_SP_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"

/* various bitflags based on the SP_STATUS register */
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_HALTED \
   (1u <<  0u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_BROKE \
   (1u <<  1u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_DMA_BUSY \
   (1u <<  2u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_DMA_FULL \
   (1u <<  3u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_IO_BUSY \
   (1u <<  4u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SSTEP \
   (1u <<  5u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_INTBREAK \
   (1u <<  6u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SIG0 \
   (1u <<  7u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SIG1 \
   (1u <<  8u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SIG2 \
   (1u <<  9u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SIG3 \
   (1u << 10u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SIG4 \
   (1u << 11u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SIG5 \
   (1u << 12u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SIG6 \
   (1u << 13u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SIG7 \
   (1u << 14u)

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_MIPS_N64_SP_H */

