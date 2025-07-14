/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform/mips/vr4300/cop0.h - Header for VR4300 co-processor */
/*    0 control functions.                                                    */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_MIPS_VR4300_COP0_H
#define _TUX64_PLATFORM_MIPS_VR4300_COP0_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/platform/mips/vr4300/vr4300.h"

#if TUX64_PLATFORM_MIPS_VR4300_ENABLE
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Reads directly from or writes directly to various co-processor 0           */
/* registers.                                                                 */
/*----------------------------------------------------------------------------*/
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_index(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_random(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_entrylo0(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_entrylo1(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_context(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_pagemask(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_wired(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_badvaddr(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_count(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_entryhi(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_compare(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_status(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_cause(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_epc(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_prid(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_config(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_lladdr(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_watchlo(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_watchhi(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_xcontext(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_parity_error(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_cache_error(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_taglo(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_taghi(void);
Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_errorepc(void);
void
tux64_platform_mips_vr4300_cop0_register_write_index(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_random(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_entrylo0(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_entrylo1(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_context(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_pagemask(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_wired(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_badvaddr(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_count(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_entryhi(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_compare(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_status(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_cause(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_epc(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_prid(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_config(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_lladdr(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_watchlo(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_watchhi(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_xcontext(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_parity_error(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_cache_error(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_taglo(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_taghi(
   Tux64UInt32 value
);
void
tux64_platform_mips_vr4300_cop0_register_write_errorepc(
   Tux64UInt32 value
);

/*----------------------------------------------------------------------------*/
#endif /* TUX64_PLATFORM_MIPS_VR4300_ENABLE */

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_MIPS_VR4300_COP0_H */

