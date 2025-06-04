/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform-n64/mmio.h - Memory-mapped I/O (MMIO).              */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_N64_MMIO_H
#define _TUX64_PLATFORM_N64_MMIO_H
/*------------------------------------------------------------------------------*/

#include "tux64/tux64.h"

#if TUX64_CONFIG_PLATFORM_N64
/*------------------------------------------------------------------------------*/

#include "tux64/platform-n64/pi.h"

struct Tux64PlatformN64MmioRegistersMi {
   Tux64UInt32       mode;
   const Tux64UInt32 version;
   const Tux64UInt32 interrupt;
   Tux64UInt32       mask;
   Tux64UInt8        __pad_000c [0x04];
   Tux64UInt32       ique_secure_exception;
   Tux64UInt8        __pad_0018 [0x14];
   const Tux64UInt32 ique_random;
   Tux64UInt8        __pad_0030 [0x08];
   Tux64UInt32       ique_interrupt;
   Tux64UInt32       ique_mask;
};

struct Tux64PlatformN64MmioRegistersVi {
   Tux64UInt32 ctrl;
   Tux64UInt32 origin;
   Tux64UInt32 width;
   Tux64UInt32 v_intr;
   Tux64UInt32 v_current;
   Tux64UInt32 burst;
   Tux64UInt32 v_total;
   Tux64UInt32 h_total;
   Tux64UInt32 h_total_leap;
   Tux64UInt32 h_video;
   Tux64UInt32 v_video;
   Tux64UInt32 v_burst;
   Tux64UInt32 x_scale;
   Tux64UInt32 y_scale;
   Tux64UInt32 test_addr;
   Tux64UInt32 staged_data;
};

struct Tux64PlatformN64MmioRegistersAi {
   Tux64UInt32       dram_addr;
   Tux64UInt32       length;
   Tux64UInt32       control;
   const Tux64UInt32 status;
   Tux64UInt32       dac_rate;
   Tux64UInt32       bit_rate;
};

struct Tux64PlatformN64MmioRegistersPiBsdDom {
   Tux64UInt32 lat;
   Tux64UInt32 pwd;
   Tux64UInt32 pgs;
   Tux64UInt32 rls;
};

struct Tux64PlatformN64MmioRegistersPi {
   Tux64UInt32 dram_addr;
   Tux64UInt32 cart_addr;
   Tux64UInt32 rd_len;
   Tux64UInt32 wr_len;
   Tux64UInt32 status;
   struct Tux64PlatformN64MmioRegistersPiBsdDom
   bsd_dom [TUX64_PLATFORM_N64_PI_BSD_DOM_COUNT];
   Tux64UInt8 __pad_0030 [0x0c];
   Tux64UInt32 ique_atb_upper;
   Tux64UInt8 __pad_0034 [0x04];
   Tux64UInt32 ique_nand_ctrl;
   Tux64UInt32 ique_nand_cfg;
   Tux64UInt8 __pad_0050 [0x08];
   Tux64UInt32 ique_rd_len;
   Tux64UInt32 ique_wr_len;
   Tux64UInt32 ique_gpio;
   Tux64UInt8 __pad_0064 [0x0c];
   Tux64UInt32 ique_nand_addr;
   Tux64UInt8 __pad_0074 [0x0001048c];
   Tux64UInt32 ique_atb_lower [TUX64_PLATFORM_N64_PI_IQUE_ATB_LOWER_COUNT];
};

struct Tux64PlatformN64MmioRegistersRi {
   Tux64UInt32       mode;
   Tux64UInt32       config;
   Tux64UInt32       current_load;
   Tux64UInt32       select;
   Tux64UInt32       refresh;
   Tux64UInt32       latency;
   const Tux64UInt32 error;
   const Tux64UInt32 bank_status;
};

struct Tux64PlatformN64MmioRegistersSi {
   Tux64UInt32       dram_addr;
   Tux64UInt32       pif_ad_rd64b;
   Tux64UInt32       pif_ad_wr4b;
   Tux64UInt8        __pad_000c [0x04];
   Tux64UInt32       pif_ad_wr64b;
   Tux64UInt32       pif_ad_rd4b;
   const Tux64UInt32 status;
};

/* Memory-mapped I/O registers which can be directly accessed.  These will be */
/* defined and relocated to the correct memory address by the linker. */
extern volatile struct Tux64PlatformN64MmioRegistersMi
tux64_platform_n64_mmio_registers_mi
__attribute__((section(".tux64.platform_n64.mmio.mi")));
extern volatile struct Tux64PlatformN64MmioRegistersVi
tux64_platform_n64_mmio_registers_vi
__attribute__((section(".tux64.platform_n64.mmio.vi")));
extern volatile struct Tux64PlatformN64MmioRegistersAi
tux64_platform_n64_mmio_registers_ai
__attribute__((section(".tux64.platform_n64.mmio.ai")));
extern volatile struct Tux64PlatformN64MmioRegistersPi
tux64_platform_n64_mmio_registers_pi
__attribute__((section(".tux64.platform_n64.mmio.pi")));
extern volatile struct Tux64PlatformN64MmioRegistersRi
tux64_platform_n64_mmio_registers_ri
__attribute__((section(".tux64.platform_n64.mmio.ri")));
extern volatile struct Tux64PlatformN64MmioRegistersSi
tux64_platform_n64_mmio_registers_si
__attribute__((section(".tux64.platform_n64.mmio.si")));

/*------------------------------------------------------------------------------*/
#endif /* TUX64_CONFIG_PLATFORM_N64 */

/*------------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_N64_MMIO_H */

