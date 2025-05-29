/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* develop/headers/types.h - Global primitive types and relevant constants.   */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_TYPES_H
#define _TUX64_TYPES_H
/*----------------------------------------------------------------------------*/

typedef  unsigned char        Tux64UInt8;
typedef  unsigned short       Tux64UInt16;
typedef  unsigned int         Tux64UInt32;
typedef  unsigned long long   Tux64UInt64;
typedef  signed char          Tux64SInt8;
typedef  signed short         Tux64SInt16;
typedef  signed int           Tux64SInt32;
typedef  signed long long     Tux64SInt64;
typedef  float                Tux64Float32;
typedef  double               Tux64Float64;

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

typedef Tux64UInt8 Tux64Boolean;

#define TUX64_BOOLEAN_FALSE   ((Tux64Boolean)(!(!(0u))))
#define TUX64_BOOLEAN_TRUE    ((Tux64Boolean)(!(0u)))

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_TYPES_H */

