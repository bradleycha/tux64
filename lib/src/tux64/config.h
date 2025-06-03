/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/config.h - Configuration macros for tux64-lib headers.             */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_CONFIG_H
#define _TUX64_CONFIG_H
/*------------------------------------------------------------------------------*/

#include "tux64/types.h"

/* these are a list of global macros used to configure the Tux64 headers. */
/* these should be defined before including tux64.h so that we can detect if */
/* configuration macros are overridden or not. */

/* is the target platform the N64? if so, include declarations to stuff */
/* like MMIO registers and other stuff that won't compile on non-N64 */
/* platforms. */
#ifndef TUX64_CONFIG_PLATFORM_IS_N64
#define TUX64_CONFIG_PLATFORM_IS_N64 0
#endif /* TUX64_CONFIG_PLATFORM_IS_N64 */

/*------------------------------------------------------------------------------*/
#endif /* _TUX64_CONFIG_H */

