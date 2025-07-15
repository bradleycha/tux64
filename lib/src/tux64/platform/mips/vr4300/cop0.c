/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform/mips/vr4300/cop0.c - Implementations for VR4300     */
/*    co-processor 0 control functions.                                       */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/platform/mips/vr4300/cop0.h"

#include "tux64/platform/mips/vr4300/vr4300.h"

#if TUX64_PLATFORM_MIPS_VR4300_ENABLE
/*----------------------------------------------------------------------------*/

#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(output, identifier)\
   __asm__ (\
      "mfc0 %0,$" identifier\
      : "=r" (output)\
   );
#define TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(value, identifier)\
   __asm__ volatile (\
      "mtc0 %0,$" identifier\
      :: "r" (value)\
   );

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_index(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_INDEX
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_random(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_RANDOM
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_entrylo0(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_ENTRYLO0
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_entrylo1(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_ENTRYLO1
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_context(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_CONTEXT
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_pagemask(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_PAGEMASK
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_wired(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WIRED
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_badvaddr(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_BADVADDR
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_count(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_COUNT
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_entryhi(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_ENTRYHI
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_compare(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_COMPARE
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_status(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_STATUS
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_cause(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_CAUSE
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_epc(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_EPC
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_prid(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_PRID
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_config(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_CONFIG
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_lladdr(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_LLADDR
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_watchlo(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WATCHLO
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_watchhi(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WATCHHI
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_xcontext(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_XCONTEXT
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_parity_error(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_PARITY_ERROR
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_cache_error(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_CACHE_ERROR
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_taglo(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_TAGLO
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_taghi(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_TAGHI
   );
   
   return output;
}

Tux64UInt32
tux64_platform_mips_vr4300_cop0_register_read_errorepc(void) {
   Tux64UInt32 output;

   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_READ(
      output,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_ERROREPC
   );
   
   return output;
}

void
tux64_platform_mips_vr4300_cop0_register_write_index(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_INDEX
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_random(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_RANDOM
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_entrylo0(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_ENTRYLO0
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_entrylo1(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_ENTRYLO1
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_context(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_CONTEXT
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_pagemask(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_PAGEMASK
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_wired(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WIRED
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_badvaddr(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_BADVADDR
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_count(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_COUNT
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_entryhi(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_ENTRYHI
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_compare(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_COMPARE
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_status(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_STATUS
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_cause(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_CAUSE
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_epc(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_EPC
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_prid(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_PRID
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_config(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_CONFIG
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_lladdr(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_LLADDR
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_watchlo(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WATCHLO
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_watchhi(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WATCHHI
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_xcontext(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_XCONTEXT
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_parity_error(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_PARITY_ERROR
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_cache_error(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_CACHE_ERROR
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_taglo(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_TAGLO
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_taghi(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_TAGHI
   );

   return;
}

void
tux64_platform_mips_vr4300_cop0_register_write_errorepc(
   Tux64UInt32 value
) {
   TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_WRITE(
      value,
      TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_ERROREPC
   );

   return;
}

#define TUX64_PLATFORM_MIPS_VR4300_COP0_CAUSE_MASK_EXC_CODE\
   (0x0000007cu)

enum Tux64PlatformMipsVr4300Cop0ExceptionCode
tux64_platform_mips_vr4300_cop0_cause_exception_code(
   Tux64UInt32 cause
) {
   Tux64UInt32 code;

   code = cause;
   code = code & TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_VR4300_COP0_CAUSE_MASK_EXC_CODE);

   return (enum Tux64PlatformMipsVr4300Cop0ExceptionCode)code;
}

/*----------------------------------------------------------------------------*/
#endif /* TUX64_PLATFORM_MIPS_VR4300_ENABLE */

