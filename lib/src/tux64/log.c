/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/log.c - Implementations for logging functions.               */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/log.h"

#if _TUX64_LOG_ENABLE
/*----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>

#define TUX64_LOG_STREAM\
   stderr

#if TUX64_CONFIG_LOG_ANSI
/*----------------------------------------------------------------------------*/

#define TUX64_LOG_ANSI_COLOR(color)\
   "\033[" color "m"

#define TUX64_LOG_ANSI_COLOR_PREFIX_INFO\
   TUX64_LOG_ANSI_COLOR("1;32")
#define TUX64_LOG_ANSI_COLOR_PREFIX_WARNING\
   TUX64_LOG_ANSI_COLOR("1;33")
#define TUX64_LOG_ANSI_COLOR_PREFIX_ERROR\
   TUX64_LOG_ANSI_COLOR("1;31")
#define TUX64_LOG_ANSI_COLOR_ORIGIN\
   TUX64_LOG_ANSI_COLOR("0;36")
#define TUX64_LOG_ANSI_COLOR_FORMAT\
   TUX64_LOG_ANSI_COLOR("0;37")
#define TUX64_LOG_ANSI_COLOR_RESET\
   TUX64_LOG_ANSI_COLOR("0")

#define TUX64_LOG_FORMAT_PREFIX_INFO(s)\
   TUX64_LOG_ANSI_COLOR_PREFIX_INFO s TUX64_LOG_ANSI_COLOR_FORMAT
#define TUX64_LOG_FORMAT_PREFIX_WARNING(s)\
   TUX64_LOG_ANSI_COLOR_PREFIX_WARNING s TUX64_LOG_ANSI_COLOR_FORMAT
#define TUX64_LOG_FORMAT_PREFIX_ERROR(s)\
   TUX64_LOG_ANSI_COLOR_PREFIX_ERROR s TUX64_LOG_ANSI_COLOR_FORMAT
#define TUX64_LOG_FORMAT_ORIGIN(s)\
   TUX64_LOG_ANSI_COLOR_ORIGIN s TUX64_LOG_ANSI_COLOR_RESET
#define TUX64_LOG_FORMAT_NEWLINE(s) \
   s TUX64_LOG_ANSI_COLOR_RESET

/*----------------------------------------------------------------------------*/
#else /* TUX64_CONFIG_LOG_ANSI */
/*----------------------------------------------------------------------------*/

#define TUX64_LOG_FORMAT_PREFIX_INFO(s) s
#define TUX64_LOG_FORMAT_PREFIX_WARNING(s) s
#define TUX64_LOG_FORMAT_PREFIX_ERROR(s) s
#define TUX64_LOG_FORMAT_ORIGIN(s) s
#define TUX64_LOG_FORMAT_NEWLINE(s) s

/*----------------------------------------------------------------------------*/
#endif /* TUX64_CONFIG_LOG_ANSI */

#define TUX64_LOG_FORMAT_PREFIX(s)\
   "[" s "] "

#define TUX64_LOG_PREFIX_INFO\
   TUX64_LOG_FORMAT_PREFIX(TUX64_LOG_FORMAT_PREFIX_INFO("info"))
#define TUX64_LOG_PREFIX_WARNING\
   TUX64_LOG_FORMAT_PREFIX(TUX64_LOG_FORMAT_PREFIX_WARNING("warning"))
#define TUX64_LOG_PREFIX_ERROR\
   TUX64_LOG_FORMAT_PREFIX(TUX64_LOG_FORMAT_PREFIX_ERROR("error"))

static const char * const
tux64_log_prefix_strings [_TUX64_LOG_LEVEL_COUNT] = {
   TUX64_LOG_PREFIX_INFO,
   TUX64_LOG_PREFIX_WARNING,
   TUX64_LOG_PREFIX_ERROR
};

static const Tux64UInt8
tux64_log_prefix_string_lengths [_TUX64_LOG_LEVEL_COUNT] = {
   TUX64_LITERAL_UINT8(sizeof(TUX64_LOG_PREFIX_INFO)),
   TUX64_LITERAL_UINT8(sizeof(TUX64_LOG_PREFIX_WARNING)),
   TUX64_LITERAL_UINT8(sizeof(TUX64_LOG_PREFIX_ERROR))
};

static void
tux64_log_prefix(
   enum _Tux64LogLevel level
) {
   const char * string;
   Tux64UInt8 length;

   string = tux64_log_prefix_strings[(Tux64UInt8)level];
   length = tux64_log_prefix_string_lengths[(Tux64UInt8)level];
   (void)fwrite(string, (size_t)1u, (size_t)length, TUX64_LOG_STREAM);

   return;
}

#define TUX64_LOG_NEWLINE\
   TUX64_LOG_FORMAT_NEWLINE("\n")

static void
tux64_log(
   enum _Tux64LogLevel level,
   const char * fmt,
   va_list args
) {
   tux64_log_prefix(level);
   (void)vfprintf(TUX64_LOG_STREAM, fmt, args);
   (void)fwrite(
      TUX64_LOG_NEWLINE,
      (size_t)1u,
      (size_t)sizeof(TUX64_LOG_NEWLINE),
      TUX64_LOG_STREAM
   );

   return;
}

#if TUX64_CONFIG_LOG_ORIGIN
/*----------------------------------------------------------------------------*/

static void
tux64_log_write_origin(
   const char * file,
   Tux64UInt32 file_bytes,
   int location
) {
   (void)fprintf(
      TUX64_LOG_STREAM,
      TUX64_LOG_FORMAT_ORIGIN("%.*s:%d "),
      file_bytes,
      file,
      location
   );
   return;
}

static void
tux64_log_origin(
   const char * file,
   Tux64UInt32 file_bytes,
   int location,
   enum _Tux64LogLevel level,
   const char * fmt,
   va_list args
) {
   tux64_log_write_origin(file, file_bytes, location);
   tux64_log(level, fmt, args);
   return;
}

void
_tux64_log_origin(
   const char * file,
   Tux64UInt32 file_bytes,
   int location,
   enum _Tux64LogLevel level,
   const char * fmt,
   ...
) {
   va_list args;
   va_start(args, fmt);

   tux64_log_origin(file, file_bytes, location, level, fmt, args);

   va_end(args);
   return;
}

/*----------------------------------------------------------------------------*/
#else /* TUX64_CONFIG_LOG_ORIGIN */
/*----------------------------------------------------------------------------*/

void
_tux64_log_plain(
   enum _Tux64LogLevel level,
   const char * fmt,
   ...
) {
   va_list args;
   va_start(args, fmt);

   tux64_log(level, fmt, args);

   va_end(args);
   return;
}

/*----------------------------------------------------------------------------*/
#endif /* TUX64_CONFIG_LOG_ORIGIN */

/*----------------------------------------------------------------------------*/
#else /* _TUX64_LOG_ENABLE */
/*----------------------------------------------------------------------------*/

/* these need to be defined so we can properly dispose of inputs.  without */
/* these, it's possible to get warnings (and errors under -Werror) about */
/* unused variables. */

void
_tux64_log_stub(const char * fmt) {
   (void)fmt;
   return;
}

void
_tux64_log_stub_fmt(const char * fmt, ...) {
   (void)fmt;
   return;
}

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_LOG_ENABLE */

