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
tux64_arguments_iterator_initialize_command_line(
   struct Tux64ArgumentsIterator * self,
   Tux64UInt8 argc,
   const char * const * argv
) {
   self->type = TUX64_ARGUMENTS_ITERATOR_TYPE_COMMAND_LINE;
   self->implementation.command_line.argv = argv;
   self->implementation.command_line.argc = argc;
   self->implementation.command_line.index = TUX64_LITERAL_UINT8(0u);
   return;
}

typedef const char * (*Tux64ArgumentsIteratorNextFunction)(
   struct Tux64ArgumentsIterator * self
);

static const char *
tux64_arguments_iterator_next_command_line(
   struct Tux64ArgumentsIterator * self
) {
   struct Tux64ArgumentsIteratorImplementationCommandLine * command_line;

   command_line = &self->implementation.command_line;

   if (command_line->index == command_line->argc) {
      return TUX64_NULLPTR;
   }

   return command_line->argv[command_line->index++];
}

static const Tux64ArgumentsIteratorNextFunction
tux64_arguments_iterator_next_functions [TUX64_ARGUMENTS_ITERATOR_TYPE_COUNT] = {
   tux64_arguments_iterator_next_command_line
};

const char *
tux64_arguments_iterator_next(
   struct Tux64ArgumentsIterator * self
) {
   return tux64_arguments_iterator_next_functions[(Tux64UInt8)self->type](self);
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

