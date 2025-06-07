/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/arguments.c - Implementations for argument parsing           */
/*    functions.                                                              */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/arguments.h"

#include <tux64/memory.h>

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

typedef struct Tux64ArgumentsIteratorNextResult (*Tux64ArgumentsIteratorNextFunction)(
   struct Tux64ArgumentsIterator * self
);

static struct Tux64ArgumentsIteratorNextResult
tux64_arguments_iterator_next_command_line(
   struct Tux64ArgumentsIterator * self
) {
   struct Tux64ArgumentsIteratorImplementationCommandLine * command_line;
   struct Tux64ArgumentsIteratorNextResult result;
   struct Tux64MemoryFindResult result_string_length;
   char terminator;

   command_line = &self->implementation.command_line;

   if (command_line->index == command_line->argc) {
      result.status = TUX64_ARGUMENTS_ITERATOR_NEXT_STATUS_END_OF_STREAM;
      return result;
   }

   result.status = TUX64_ARGUMENTS_ITERATOR_NEXT_STATUS_OK;
   result.payload.ok.ptr = command_line->argv[command_line->index++];

   /* implements strlen() by finding the null terminator.  assumes the null */
   /* terminator is always present. */
   terminator = '\0';
   result_string_length = tux64_memory_find(
      result.payload.ok.ptr,
      &terminator,
      TUX64_UINT32_MAX,
      TUX64_LITERAL_UINT32(sizeof(terminator))
   );

   result.payload.ok.characters = result_string_length.payload.found.position;
   return result;
}

static const Tux64ArgumentsIteratorNextFunction
tux64_arguments_iterator_next_functions [TUX64_ARGUMENTS_ITERATOR_TYPE_COUNT] = {
   tux64_arguments_iterator_next_command_line
};

struct Tux64ArgumentsIteratorNextResult
tux64_arguments_iterator_next(
   struct Tux64ArgumentsIterator * self
) {
   return tux64_arguments_iterator_next_functions[(Tux64UInt8)self->type](self);
}

struct Tux64ArgumentsParseResult
tux64_arguments_parse(
   const struct Tux64ArgumentsList * list,
   struct Tux64ArgumentsIterator * iterator,
   void * context,
   Tux64UInt8 * required_storage
) {
   struct Tux64ArgumentsParseResult result;

   /* TODO: implement*/
   (void)list;
   (void)iterator;
   (void)context;
   (void)required_storage;
   result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_INVALID;
   result.payload.parameter_invalid.identifier = "--fuck-rust";
   result.payload.parameter_invalid.parameter = "true";
   result.payload.parameter_invalid.reason = "assignment to tautology is redundant";
   return result;
}

