/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/log.h - Header for logging functions.                        */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_LOG_H
#define _TUX64_LOG_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"

#define _TUX64_LOG_ENABLE\
   (TUX64_CONFIG_LOG && TUX64_HAVE_STDIO_H && TUX64_HAVE_STDARG_H)

#if _TUX64_LOG_ENABLE
/*----------------------------------------------------------------------------*/

#define _TUX64_LOG_LEVEL_COUNT 3u
enum _Tux64LogLevel {
   _TUX64_LOG_LEVEL_INFO = 0u,
   _TUX64_LOG_LEVEL_WARNING,
   _TUX64_LOG_LEVEL_ERROR
};

#if TUX64_CONFIG_LOG_ORIGIN
/*----------------------------------------------------------------------------*/

void
_tux64_log_origin(
   const char * file,
   Tux64UInt32 file_bytes,
   int location,
   enum _Tux64LogLevel level,
   const char * fmt,
   ...
);

#define _TUX64_LOG(level, message)\
   _tux64_log_origin(\
      __FILE__,\
      TUX64_LITERAL_UINT32(sizeof(__FILE__) - 1),\
      __LINE__,\
      level,\
      message\
   )
#define _TUX64_LOG_FMT(level, fmt, ...)\
   _tux64_log_origin(\
      __FILE__,\
      TUX64_LITERAL_UINT32(sizeof(__FILE__) - 1),\
      __LINE__,\
      level,\
      fmt,\
      __VA_ARGS__\
   )

/*----------------------------------------------------------------------------*/
#else /* TUX64_CONFIG_LOG_ORIGIN */
/*----------------------------------------------------------------------------*/

void
_tux64_log_plain(
   enum _Tux64LogLevel level,
   const char * fmt,
   ...
);

#define _TUX64_LOG(level, message)\
   _tux64_log_plain(level, message)
#define _TUX64_LOG_FMT(level, fmt, ...)\
   _tux64_log_plain(level, fmt, __VA_ARGS__)

/*----------------------------------------------------------------------------*/
#endif /* TUX64_CONFIG_LOG_ORIGIN */

/*----------------------------------------------------------------------------*/
/* Prints a log message to the console.                                       */
/*----------------------------------------------------------------------------*/
#define TUX64_LOG_INFO(message)\
   _TUX64_LOG(_TUX64_LOG_LEVEL_INFO, message)
#define TUX64_LOG_WARNING(message)\
   _TUX64_LOG(_TUX64_LOG_LEVEL_WARNING, message)
#define TUX64_LOG_ERROR(message)\
   _TUX64_LOG(_TUX64_LOG_LEVEL_ERROR, message)
#define TUX64_LOG_INFO_FMT(fmt, ...)\
   _TUX64_LOG_FMT(_TUX64_LOG_LEVEL_INFO, fmt, __VA_ARGS__)
#define TUX64_LOG_WARNING_FMT(fmt, ...)\
   _TUX64_LOG_FMT(_TUX64_LOG_LEVEL_WARNING, fmt, __VA_ARGS__)
#define TUX64_LOG_ERROR_FMT(fmt, ...)\
   _TUX64_LOG_FMT(_TUX64_LOG_LEVEL_ERROR, fmt, __VA_ARGS__)

/*----------------------------------------------------------------------------*/
#else /* _TUX64_LOG_ENABLE */
/*----------------------------------------------------------------------------*/

#define TUX64_LOG_INFO(...)
#define TUX64_LOG_WARNING(...)
#define TUX64_LOG_ERROR(...)
#define TUX64_LOG_INFO_FMT(...)
#define TUX64_LOG_WARNING_FMT(...)
#define TUX64_LOG_ERROR_FMT(...)

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_LOG_ENABLE */

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_LOG_H */

