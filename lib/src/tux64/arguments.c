/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/arguments.c - Implementations for argument parsing           */
/*    functions.                                                              */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/arguments.h"

void
tux64_arguments_iterator_initialize(
   struct Tux64ArgumentsIterator * self,
   Tux64UInt8 argc,
   const char * const * argv
) {
   self->argv = argv;
   self->argc = argc;
   self->index = TUX64_LITERAL_UINT8(0u);
   return;
}

const char *
tux64_arguments_iterator_next(
   struct Tux64ArgumentsIterator * self
) {
   if (self->index == self->argc) {
      return TUX64_NULLPTR;
   }

   return self->argv[self->index++];
}

struct Tux64ArgumentsParseResult
tux64_arguments_parse(
   const struct Tux64ArgumentsList * list,
   struct Tux64ArgumentsIterator * iterator,
   void * context
) {
   struct Tux64ArgumentsParseResult result;

   /* TODO: implement*/
   (void)list;
   (void)iterator;
   (void)context;
   result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_INVALID;
   result.payload.parameter_invalid.name = "fuck-downstream";
   result.payload.parameter_invalid.parameter = "enable";
   return result;
}

