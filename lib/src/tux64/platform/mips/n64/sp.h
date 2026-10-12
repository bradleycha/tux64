/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
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
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_CLR_HALT \
   (1u <<  0u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SET_HALT \
   (1u <<  1u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_CLR_BROKE \
   (1u <<  2u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_CLR_INTR \
   (1u <<  3u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SET_INTR \
   (1u <<  4u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_CLR_SSTEP \
   (1u <<  5u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SET_SSTEP \
   (1u <<  6u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_CLR_INTBREAK \
   (1u <<  7u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SET_INTBREAK \
   (1u <<  8u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_CLR_SIG0 \
   (1u <<  9u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SET_SIG0 \
   (1u << 10u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_CLR_SIG1 \
   (1u << 11u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SET_SIG1 \
   (1u << 12u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_CLR_SIG2 \
   (1u << 13u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SET_SIG2 \
   (1u << 14u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_CLR_SIG3 \
   (1u << 15u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SET_SIG3 \
   (1u << 16u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_CLR_SIG4 \
   (1u << 17u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SET_SIG4 \
   (1u << 18u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_CLR_SIG5 \
   (1u << 19u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SET_SIG5 \
   (1u << 20u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_CLR_SIG6 \
   (1u << 21u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SET_SIG6 \
   (1u << 22u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_CLR_SIG7 \
   (1u << 23u)
#define TUX64_PLATFORM_MIPS_N64_SP_STATUS_BIT_SET_SIG7 \
   (1u << 24u)

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_MIPS_N64_SP_H */

