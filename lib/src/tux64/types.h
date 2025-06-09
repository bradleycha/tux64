/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/types.h - Global primitive types and relevant constants.     */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_TYPES_H
#define _TUX64_TYPES_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"

/* if we don't have stdint.h, we must use a small mountain of preprocessor */
/* and configure madness to get fixed-size types. */
#if TUX64_HAVE_STDINT_H
/*----------------------------------------------------------------------------*/

#include <stdint.h>

#define _TUX64_TYPES_UINT8    uint8_t
#define _TUX64_TYPES_UINT16   uint16_t
#define _TUX64_TYPES_UINT32   uint32_t
#define _TUX64_TYPES_UINT64   uint64_t
#define _TUX64_TYPES_SINT8    int8_t
#define _TUX64_TYPES_SINT16   int16_t
#define _TUX64_TYPES_SINT32   int32_t
#define _TUX64_TYPES_SINT64   int64_t

#ifndef _TUX64_TYPES_INTPTR_DEFINED
#if TUX64_SIZEOF_VOID_P == 1
#define _TUX64_TYPES_UINTPTR _TUX64_TYPES_UINT8
#define _TUX64_TYPES_SINTPTR _TUX64_TYPES_SINT8
#define _TUX64_TYPES_INTPTR_DEFINED
#endif /* TUX64_SIZEOF_VOID_P == 1 */
#endif /* _TUX64_TYPES_INTPTR_DEFINED */
#ifndef _TUX64_TYPES_INTPTR_DEFINED
#if TUX64_SIZEOF_VOID_P == 2
#define _TUX64_TYPES_UINTPTR _TUX64_TYPES_UINT16
#define _TUX64_TYPES_SINTPTR _TUX64_TYPES_SINT16
#define _TUX64_TYPES_INTPTR_DEFINED
#endif /* TUX64_SIZEOF_VOID_P == 2 */
#endif /* _TUX64_TYPES_INTPTR_DEFINED */
#ifndef _TUX64_TYPES_INTPTR_DEFINED
#if TUX64_SIZEOF_VOID_P == 4
#define _TUX64_TYPES_UINTPTR _TUX64_TYPES_UINT32
#define _TUX64_TYPES_SINTPTR _TUX64_TYPES_SINT32
#define _TUX64_TYPES_INTPTR_DEFINED
#endif /* TUX64_SIZEOF_VOID_P == 4 */
#endif /* _TUX64_TYPES_INTPTR_DEFINED */
#ifndef _TUX64_TYPES_INTPTR_DEFINED
#if TUX64_SIZEOF_VOID_P == 8
#define _TUX64_TYPES_UINTPTR _TUX64_TYPES_UINT64
#define _TUX64_TYPES_SINTPTR _TUX64_TYPES_SINT64
#define _TUX64_TYPES_INTPTR_DEFINED
#endif /* TUX64_SIZEOF_VOID_P == 8 */
#endif /* _TUX64_TYPES_INTPTR_DEFINED */

/*----------------------------------------------------------------------------*/
#else /* TUX64_HAVE_STDINT_H */
/*----------------------------------------------------------------------------*/
#ifndef _TUX64_TYPES_INT8
#if TUX64_SIZEOF_CHAR == 1
#define _TUX64_TYPES_INT8 char
#endif /* TUX64_SIZEOF_CHAR == 1 */
#endif /* _TUX64_TYPES_INT8 */
#ifndef _TUX64_TYPES_INT8
#if TUX64_SIZEOF_SHORT == 1
#define _TUX64_TYPES_INT8 short
#endif /* TUX64_SIZEOF_SHORT == 1 */
#endif /* _TUX64_TYPES_INT8 */
#ifndef _TUX64_TYPES_INT8
#if TUX64_SIZEOF_INT == 1
#define _TUX64_TYPES_INT8 int
#endif /* TUX64_SIZEOF_INT == 1 */
#endif /* _TUX64_TYPES_INT8 */
#ifndef _TUX64_TYPES_INT8
#if TUX64_SIZEOF_LONG == 1
#define _TUX64_TYPES_INT8 long
#endif /* TUX64_SIZEOF_LONG == 1 */
#endif /* _TUX64_TYPES_INT8 */
#ifndef _TUX64_TYPES_INT8
#if TUX64_SIZEOF_LONG_LONG == 1
#define _TUX64_TYPES_INT8 long long
#endif /* TUX64_SIZEOF_LONG_LONG == 1 */
#endif /* _TUX64_TYPES_INT8 */

#ifndef _TUX64_TYPES_INT16
#if TUX64_SIZEOF_CHAR == 2
#define _TUX64_TYPES_INT16 char
#endif /* TUX64_SIZEOF_CHAR == 2 */
#endif /* _TUX64_TYPES_INT16 */
#ifndef _TUX64_TYPES_INT16
#if TUX64_SIZEOF_SHORT == 2
#define _TUX64_TYPES_INT16 short
#endif /* TUX64_SIZEOF_SHORT == 2 */
#endif /* _TUX64_TYPES_INT16 */
#ifndef _TUX64_TYPES_INT16
#if TUX64_SIZEOF_INT == 2
#define _TUX64_TYPES_INT16 int
#endif /* TUX64_SIZEOF_INT == 2 */
#endif /* _TUX64_TYPES_INT16 */
#ifndef _TUX64_TYPES_INT16
#if TUX64_SIZEOF_LONG == 2
#define _TUX64_TYPES_INT16 long
#endif /* TUX64_SIZEOF_LONG == 2 */
#endif /* _TUX64_TYPES_INT16 */
#ifndef _TUX64_TYPES_INT16
#if TUX64_SIZEOF_LONG_LONG == 2
#define _TUX64_TYPES_INT16 long long
#endif /* TUX64_SIZEOF_LONG_LONG == 2 */
#endif /* _TUX64_TYPES_INT16 */

#ifndef _TUX64_TYPES_INT32
#if TUX64_SIZEOF_CHAR == 4
#define _TUX64_TYPES_INT32 char
#endif /* TUX64_SIZEOF_CHAR == 4 */
#endif /* _TUX64_TYPES_INT32 */
#ifndef _TUX64_TYPES_INT32
#if TUX64_SIZEOF_SHORT == 4
#define _TUX64_TYPES_INT32 short
#endif /* TUX64_SIZEOF_SHORT == 4 */
#endif /* _TUX64_TYPES_INT32 */
#ifndef _TUX64_TYPES_INT32
#if TUX64_SIZEOF_INT == 4
#define _TUX64_TYPES_INT32 int
#endif /* TUX64_SIZEOF_INT == 4 */
#endif /* _TUX64_TYPES_INT32 */
#ifndef _TUX64_TYPES_INT32
#if TUX64_SIZEOF_LONG == 4
#define _TUX64_TYPES_INT32 long
#endif /* TUX64_SIZEOF_LONG == 4 */
#endif /* _TUX64_TYPES_INT32 */
#ifndef _TUX64_TYPES_INT32
#if TUX64_SIZEOF_LONG_LONG == 4
#define _TUX64_TYPES_INT32 long long
#endif /* TUX64_SIZEOF_LONG_LONG == 4 */
#endif /* _TUX64_TYPES_INT32 */

#ifndef _TUX64_TYPES_INT64
#if TUX64_SIZEOF_CHAR == 8
#define _TUX64_TYPES_INT64 char
#endif /* TUX64_SIZEOF_CHAR == 8 */
#endif /* _TUX64_TYPES_INT64 */
#ifndef _TUX64_TYPES_INT64
#if TUX64_SIZEOF_SHORT == 8
#define _TUX64_TYPES_INT64 short
#endif /* TUX64_SIZEOF_SHORT == 8 */
#endif /* _TUX64_TYPES_INT64 */
#ifndef _TUX64_TYPES_INT64
#if TUX64_SIZEOF_INT == 8
#define _TUX64_TYPES_INT64 int
#endif /* TUX64_SIZEOF_INT == 8 */
#endif /* _TUX64_TYPES_INT64 */
#ifndef _TUX64_TYPES_INT64
#if TUX64_SIZEOF_LONG == 8
#define _TUX64_TYPES_INT64 long
#endif /* TUX64_SIZEOF_LONG == 8 */
#endif /* _TUX64_TYPES_INT64 */
#ifndef _TUX64_TYPES_INT64
#if TUX64_SIZEOF_LONG_LONG == 8
#define _TUX64_TYPES_INT64 long long
#endif /* TUX64_SIZEOF_LONG_LONG == 8 */
#endif /* _TUX64_TYPES_INT64 */

#ifndef _TUX64_TYPES_INTPTR
#if TUX64_SIZEOF_VOID_P == 1
#define _TUX64_TYPES_INTPTR _TUX64_TYPES_INT8
#endif /* TUX64_SIZEOF_VOID_P == 1 */
#endif /* _TUX64_TYPES_INTPTR */
#ifndef _TUX64_TYPES_INTPTR
#if TUX64_SIZEOF_VOID_P == 2
#define _TUX64_TYPES_INTPTR _TUX64_TYPES_INT16
#endif /* TUX64_SIZEOF_VOID_P == 2 */
#endif /* _TUX64_TYPES_INTPTR */
#ifndef _TUX64_TYPES_INTPTR
#if TUX64_SIZEOF_VOID_P == 4
#define _TUX64_TYPES_INTPTR _TUX64_TYPES_INT32
#endif /* TUX64_SIZEOF_VOID_P == 4 */
#endif /* _TUX64_TYPES_INTPTR */
#ifndef _TUX64_TYPES_INTPTR
#if TUX64_SIZEOF_VOID_P == 8
#define _TUX64_TYPES_INTPTR _TUX64_TYPES_INT64
#endif /* TUX64_SIZEOF_VOID_P == 8 */
#endif /* _TUX64_TYPES_INTPTR */


#define _TUX64_TYPES_UINT8    unsigned _TUX64_TYPES_INT8
#define _TUX64_TYPES_UINT16   unsigned _TUX64_TYPES_INT16
#define _TUX64_TYPES_UINT32   unsigned _TUX64_TYPES_INT32
#define _TUX64_TYPES_UINT64   unsigned _TUX64_TYPES_INT64
#define _TUX64_TYPES_SINT8    signed   _TUX64_TYPES_INT8
#define _TUX64_TYPES_SINT16   signed   _TUX64_TYPES_INT16
#define _TUX64_TYPES_SINT32   signed   _TUX64_TYPES_INT32
#define _TUX64_TYPES_SINT64   signed   _TUX64_TYPES_INT64
#define _TUX64_TYPES_UINTPTR  unsigned _TUX64_TYPES_INTPTR
#define _TUX64_TYPES_SINTPTR  signed   _TUX64_TYPES_INTPTR
/*----------------------------------------------------------------------------*/
#endif /* TUX64_HAVE_STDINT_H */

#ifndef _TUX64_TYPES_FLOAT32
#if TUX64_SIZEOF_FLOAT == 4
#define _TUX64_TYPES_FLOAT32 float
#endif /* TUX64_SIZEOF_FLOAT == 4 */
#endif /* _TUX64_TYPES_FLOAT32 */
#ifndef _TUX64_TYPES_FLOAT32
#if TUX64_SIZEOF_DOUBLE == 4
#define _TUX64_TYPES_FLOAT32 double
#endif /* TUX64_SIZEOF_DOUBLE == 4 */
#endif /* _TUX64_TYPES_FLOAT32 */

#ifndef _TUX64_TYPES_FLOAT64
#if TUX64_SIZEOF_FLOAT == 8
#define _TUX64_TYPES_FLOAT64 float
#endif /* TUX64_SIZEOF_FLOAT == 8 */
#endif /* _TUX64_TYPES_FLOAT64 */
#ifndef _TUX64_TYPES_FLOAT64
#if TUX64_SIZEOF_DOUBLE == 8
#define _TUX64_TYPES_FLOAT64 double
#endif /* TUX64_SIZEOF_DOUBLE == 8 */
#endif /* _TUX64_TYPES_FLOAT64 */

typedef _TUX64_TYPES_UINT8    Tux64UInt8;
typedef _TUX64_TYPES_UINT16   Tux64UInt16;
typedef _TUX64_TYPES_UINT32   Tux64UInt32;
typedef _TUX64_TYPES_UINT64   Tux64UInt64;
typedef _TUX64_TYPES_UINTPTR  Tux64UIntPtr;
typedef _TUX64_TYPES_SINT8    Tux64SInt8;
typedef _TUX64_TYPES_SINT16   Tux64SInt16;
typedef _TUX64_TYPES_SINT32   Tux64SInt32;
typedef _TUX64_TYPES_SINT64   Tux64SInt64;
typedef _TUX64_TYPES_SINTPTR  Tux64SIntPtr;
typedef _TUX64_TYPES_FLOAT32  Tux64Float32;
typedef _TUX64_TYPES_FLOAT64  Tux64Float64;

union Tux64Int8 {
   Tux64UInt8 uint;
   Tux64SInt8 sint;
   Tux64UInt8 bytes [sizeof(Tux64UInt8)];
};
union Tux64Int16 {
   Tux64UInt16 uint;
   Tux64SInt16 sint;
   Tux64UInt8 bytes [sizeof(Tux64UInt16)];
};
union Tux64Int32 {
   Tux64UInt32 uint;
   Tux64SInt32 sint;
   Tux64UInt8 bytes [sizeof(Tux64UInt32)];
};
union Tux64Int64 {
   Tux64UInt64 uint;
   Tux64SInt64 sint;
   Tux64UInt8 bytes [sizeof(Tux64UInt64)];
};
union Tux64IntPtr {
   Tux64UIntPtr uint;
   Tux64SIntPtr sint;
   Tux64UInt8 bytes [sizeof(Tux64UIntPtr)];
};

typedef Tux64UInt8 Tux64Boolean;

#define TUX64_BOOLEAN_FALSE   ((Tux64Boolean)(!(!(0u))))
#define TUX64_BOOLEAN_TRUE    ((Tux64Boolean)(!(0u)))

struct Tux64String {
   const char * ptr;
   Tux64UInt32 characters;
};

/*----------------------------------------------------------------------------*/
/* Defines a typed literal.  Do not use this with variables or constants      */
/* which can't be evaluated at compile-time.                                  */
/*----------------------------------------------------------------------------*/
#define TUX64_LITERAL_UINT8(value)\
   ((Tux64UInt8)(value))
#define TUX64_LITERAL_UINT16(value)\
   ((Tux64UInt16)(value))
#define TUX64_LITERAL_UINT32(value)\
   ((Tux64UInt32)(value))
#define TUX64_LITERAL_UINT64(value)\
   ((Tux64UInt64)(value))
#define TUX64_LITERAL_SINT8(value)\
   ((Tux64SInt8)(value))
#define TUX64_LITERAL_SINT16(value)\
   ((Tux64SInt16)(value))
#define TUX64_LITERAL_SINT32(value)\
   ((Tux64SInt32)(value))
#define TUX64_LITERAL_SINT64(value)\
   ((Tux64SInt64)(value))
#define TUX64_LITERAL_FLOAT32(value)\
   ((Tux64Float32)(value))
#define TUX64_LITERAL_FLOAT64(value)\
   ((Tux64Float64)(value))

/*----------------------------------------------------------------------------*/
/* Untyped integer limits.  These should only be used in preprocessor         */
/* statements.                                                                */
/*----------------------------------------------------------------------------*/
#define TUX64_UINT8_MAX_LITERAL  (0xffu)
#define TUX64_UINT16_MAX_LITERAL (0xffffu)
#define TUX64_UINT32_MAX_LITERAL (0xffffffffu)
#define TUX64_UINT64_MAX_LITERAL (0xffffffffffffffffllu)
#define TUX64_UINT8_MIN_LITERAL  (0x00u)
#define TUX64_UINT16_MIN_LITERAL (0x0000u)
#define TUX64_UINT32_MIN_LITERAL (0x00000000u)
#define TUX64_UINT64_MIN_LITERAL (0x0000000000000000llu)
#if TUX64_CONFIG_PLATFORM_CPU_SIGNED_INTEGER_FORMAT_SIGN_MAGNITUDE
#define TUX64_SINT8_MAX_LITERAL  (0x7f)
#define TUX64_SINT16_MAX_LITERAL (0x7fff)
#define TUX64_SINT32_MAX_LITERAL (0x7fffffff)
#define TUX64_SINT64_MAX_LITERAL (0x7fffffffffffffffll)
#define TUX64_SINT8_MIN_LITERAL  (-0x7f)
#define TUX64_SINT16_MIN_LITERAL (-0x7fff)
#define TUX64_SINT32_MIN_LITERAL (-0x7fffffff)
#define TUX64_SINT64_MIN_LITERAL (-0x7fffffffffffffffll)
#endif /* TUX64_CONFIG_PLATFORM_CPU_SIGNED_INTEGER_FORMAT_SIGN_MAGNITUDE */
#if TUX64_CONFIG_PLATFORM_CPU_SIGNED_INTEGER_FORMAT_ONES_COMPLEMENT
#define TUX64_SINT8_MAX_LITERAL  (0x7f)
#define TUX64_SINT16_MAX_LITERAL (0x7fff)
#define TUX64_SINT32_MAX_LITERAL (0x7fffffff)
#define TUX64_SINT64_MAX_LITERAL (0x7fffffffffffffffll)
#define TUX64_SINT8_MIN_LITERAL  (-0x7f)
#define TUX64_SINT16_MIN_LITERAL (-0x7fff)
#define TUX64_SINT32_MIN_LITERAL (-0x7fffffff)
#define TUX64_SINT64_MIN_LITERAL (-0x7fffffffffffffffll)
#endif /* TUX64_CONFIG_PLATFORM_CPU_SIGNED_INTEGER_FORMAT_ONES_COMPLEMENT */
#if TUX64_CONFIG_PLATFORM_CPU_SIGNED_INTEGER_FORMAT_TWOS_COMPLEMENT
#define TUX64_SINT8_MAX_LITERAL  (0x7f)
#define TUX64_SINT16_MAX_LITERAL (0x7fff)
#define TUX64_SINT32_MAX_LITERAL (0x7fffffff)
#define TUX64_SINT64_MAX_LITERAL (0x7fffffffffffffffll)
#define TUX64_SINT8_MIN_LITERAL  (-0x80)
#define TUX64_SINT16_MIN_LITERAL (-0x8000)
#define TUX64_SINT32_MIN_LITERAL (-0x80000000)
#define TUX64_SINT64_MIN_LITERAL (-0x8000000000000000ll)
#endif /* TUX64_CONFIG_PLATFORM_CPU_SIGNED_INTEGER_FORMAT_TWOS_COMPLEMENT */

/*----------------------------------------------------------------------------*/
/* Typed integer limits.  These should be used everywhere except for          */
/* preprocessor statements.                                                   */
/*----------------------------------------------------------------------------*/
#define TUX64_UINT8_MAX    TUX64_LITERAL_UINT8(TUX64_UINT8_MAX_LITERAL)
#define TUX64_UINT16_MAX   TUX64_LITERAL_UINT16(TUX64_UINT16_MAX_LITERAL)
#define TUX64_UINT32_MAX   TUX64_LITERAL_UINT32(TUX64_UINT32_MAX_LITERAL)
#define TUX64_UINT64_MAX   TUX64_LITERAL_UINT64(TUX64_UINT64_MAX_LITERAL)
#define TUX64_UINT8_MIN    TUX64_LITERAL_UINT8(TUX64_UINT8_MIN_LITERAL)
#define TUX64_UINT16_MIN   TUX64_LITERAL_UINT16(TUX64_UINT16_MIN_LITERAL)
#define TUX64_UINT32_MIN   TUX64_LITERAL_UINT32(TUX64_UINT32_MIN_LITERAL)
#define TUX64_UINT64_MIN   TUX64_LITERAL_UINT64(TUX64_UINT64_MIN_LITERAL)
#define TUX64_SINT8_MAX    TUX64_LITERAL_SINT8(TUX64_SINT8_MAX_LITERAL)
#define TUX64_SINT16_MAX   TUX64_LITERAL_SINT16(TUX64_SINT16_MAX_LITERAL)
#define TUX64_SINT32_MAX   TUX64_LITERAL_SINT32(TUX64_SINT32_MAX_LITERAL)
#define TUX64_SINT64_MAX   TUX64_LITERAL_SINT64(TUX64_SINT64_MAX_LITERAL)
#define TUX64_SINT8_MIN    TUX64_LITERAL_SINT8(TUX64_SINT8_MIN_LITERAL)
#define TUX64_SINT16_MIN   TUX64_LITERAL_SINT16(TUX64_SINT16_MIN_LITERAL)
#define TUX64_SINT32_MIN   TUX64_LITERAL_SINT32(TUX64_SINT32_MIN_LITERAL)
#define TUX64_SINT64_MIN   TUX64_LITERAL_SINT64(TUX64_SINT64_MIN_LITERAL)

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_TYPES_H */

