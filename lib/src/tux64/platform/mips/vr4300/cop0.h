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
/* String literals which define a co-processor register, for use with inline  */
/* assembly template strings.                                                 */
/*----------------------------------------------------------------------------*/
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_INDEX\
   "0"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_RANDOM\
   "1"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_ENTRYLO0\
   "2"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_ENTRYLO1\
   "3"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_CONTEXT\
   "4"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_PAGEMASK\
   "5"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WIRED\
   "6"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_BADVADDR\
   "8"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_COUNT\
   "9"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_ENTRYHI\
   "10"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_COMPARE\
   "11"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_STATUS\
   "12"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_CAUSE\
   "13"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_EPC\
   "14"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_PRID\
   "15"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_CONFIG\
   "16"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_LLADDR\
   "17"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WATCHLO\
   "18"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WATCHHI\
   "19"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_XCONTEXT\
   "20"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_PARITY_ERROR\
   "26"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_CACHE_ERROR\
   "27"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_TAGLO\
   "28"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_TAGHI\
   "29"
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_ERROREPC\
   "30"

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

enum Tux64PlatformMipsVr4300Cop0ExceptionCode {
   TUX64_PLATFORM_MIPS_VR4300_COP0_EXCEPTION_CODE_INTERRUPT = ( 0 << 5u),
   TUX64_PLATFORM_MIPS_VR4300_COP0_EXCEPTION_CODE_MOD       = ( 1 << 5u),
   TUX64_PLATFORM_MIPS_VR4300_COP0_EXCEPTION_CODE_TLBL      = ( 2 << 5u),
   TUX64_PLATFORM_MIPS_VR4300_COP0_EXCEPTION_CODE_TLBS      = ( 3 << 5u),
   TUX64_PLATFORM_MIPS_VR4300_COP0_EXCEPTION_CODE_ADEL      = ( 4 << 5u),
   TUX64_PLATFORM_MIPS_VR4300_COP0_EXCEPTION_CODE_ADES      = ( 5 << 5u),
   TUX64_PLATFORM_MIPS_VR4300_COP0_EXCEPTION_CODE_IBE       = ( 6 << 5u),
   TUX64_PLATFORM_MIPS_VR4300_COP0_EXCEPTION_CODE_DBE       = ( 7 << 5u),
   TUX64_PLATFORM_MIPS_VR4300_COP0_EXCEPTION_CODE_SYS       = ( 8 << 5u),
   TUX64_PLATFORM_MIPS_VR4300_COP0_EXCEPTION_CODE_BP        = ( 9 << 5u),
   TUX64_PLATFORM_MIPS_VR4300_COP0_EXCEPTION_CODE_RI        = (10 << 5u),
   TUX64_PLATFORM_MIPS_VR4300_COP0_EXCEPTION_CODE_CPU       = (11 << 5u),
   TUX64_PLATFORM_MIPS_VR4300_COP0_EXCEPTION_CODE_OV        = (12 << 5u),
   TUX64_PLATFORM_MIPS_VR4300_COP0_EXCEPTION_CODE_TR        = (13 << 5u),
   TUX64_PLATFORM_MIPS_VR4300_COP0_EXCEPTION_CODE_FPE       = (15 << 5u),
   TUX64_PLATFORM_MIPS_VR4300_COP0_EXCEPTION_CODE_WATCH     = (23 << 5u)
};

/*----------------------------------------------------------------------------*/
/* Reads the Exc Code bits from the given Cause register copy. 'cause' must   */
/* be a valid copy of the Causes register.                                    */
/*----------------------------------------------------------------------------*/
enum Tux64PlatformMipsVr4300Cop0ExceptionCode
tux64_platform_mips_vr4300_cop0_cause_exception_code(
   Tux64UInt32 cause
);

/*----------------------------------------------------------------------------*/
/* Various bitflags for various co-processor 0 registers.                     */
/*----------------------------------------------------------------------------*/
#define TUX64_PLATFORM_MIPS_VR4300_COP0_STATUS_BIT_IE\
   (1u << 0u)
#define TUX64_PLATFORM_MIPS_VR4300_COP0_STATUS_BIT_RP\
   (1u << 27u)

#define TUX64_PLATFORM_MIPS_VR4300_COP0_CAUSE_BIT_IP0\
   (1u << 8u)
#define TUX64_PLATFORM_MIPS_VR4300_COP0_CAUSE_BIT_IP1\
   (1u << 9u)
#define TUX64_PLATFORM_MIPS_VR4300_COP0_CAUSE_BIT_IP2\
   (1u << 10u)
#define TUX64_PLATFORM_MIPS_VR4300_COP0_CAUSE_BIT_IP3\
   (1u << 11u)
#define TUX64_PLATFORM_MIPS_VR4300_COP0_CAUSE_BIT_IP4\
   (1u << 12u)
#define TUX64_PLATFORM_MIPS_VR4300_COP0_CAUSE_BIT_IP5\
   (1u << 13u)
#define TUX64_PLATFORM_MIPS_VR4300_COP0_CAUSE_BIT_IP6\
   (1u << 14u)
#define TUX64_PLATFORM_MIPS_VR4300_COP0_CAUSE_BIT_IP7\
   (1u << 15u)

/*----------------------------------------------------------------------------*/
#endif /* TUX64_PLATFORM_MIPS_VR4300_ENABLE */

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_MIPS_VR4300_COP0_H */

