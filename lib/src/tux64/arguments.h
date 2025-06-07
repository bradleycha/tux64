/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/arguments.h - Header for arguments parsing functions.        */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_ARGUMENTS_H
#define _TUX64_ARGUMENTS_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"

enum Tux64ArgumentsParseStatus {
   _TUX64_ARGUMENTS_PARSE_STATUS_START_OFFSET = -1,

   TUX64_ARGUMENTS_PARSE_STATUS_OK,
   TUX64_ARGUMENTS_PARSE_STATUS_EXIT,
   TUX64_ARGUMENTS_PARSE_STATUS_UNKNOWN_NAME,
   TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_MISSING,
   TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_UNEXPECTED,
   TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_INVALID,
   TUX64_ARGUMENTS_PARSE_STATUS_REQUIRED_MISSING,

   _TUX64_ARGUMENTS_PARSE_STATUS_END_OFFSET
};

struct Tux64ArgumentsParsePayloadUnknownName {
   const char * name;
};

struct Tux64ArgumentsParsePayloadParameterMissing {
   const char * name;
};

struct Tux64ArgumentsParsePayloadParameterUnexpected {
   const char * name;
};

struct Tux64ArgumentsParsePayloadParameterInvalid {
   const char * name;
   const char * parameter;
};

struct Tux64ArgumentsParsePayloadRequiredMissing {
   const char * name;
};

union Tux64ArgumentsParsePayload {
   struct Tux64ArgumentsParsePayloadUnknownName unknown_name;
   struct Tux64ArgumentsParsePayloadParameterMissing parameter_missing;
   struct Tux64ArgumentsParsePayloadParameterUnexpected parameter_unexpected;
   struct Tux64ArgumentsParsePayloadParameterInvalid parameter_invalid;
   struct Tux64ArgumentsParsePayloadRequiredMissing required_missing;
};

struct Tux64ArgumentsParseResult {
   enum Tux64ArgumentsParseStatus status;
   union Tux64ArgumentsParsePayload payload;
};

typedef enum Tux64ArgumentsParseStatus (*Tux64ArgumentsParseFunction)(
   const char * parameter,
   void * context
);

/* stop parsing arguments and exit the program when encountered */
#define TUX64_ARGUMENTS_OPTION_FLAG_EXIT\
   TUX64_LITERAL_UINT8(1u << 0u)

/* defines a single option */
struct Tux64ArgumentsOption {
   const char * name;
   Tux64ArgumentsParseFunction parser;
   Tux64UInt8 flags;
};

/* defines a list of arguments in which to parse */
struct Tux64ArgumentsList {
   const struct Tux64ArgumentsOption * options_required;
   const struct Tux64ArgumentsOption * options_optional;
   Tux64UInt32 options_required_count;
   Tux64UInt32 options_optional_count;
};


#define TUX64_ARGUMENTS_ITERATOR_TYPE_COUNT 1u
enum Tux64ArgumentsIteratorType {
   TUX64_ARGUMENTS_ITERATOR_TYPE_COMMAND_LINE = 0u
};

struct Tux64ArgumentsIteratorImplementationCommandLine {
   const char * const * argv;
   Tux64UInt8 argc;
   Tux64UInt8 index;
};

union Tux64ArgumentsIteratorImplementation {
   struct Tux64ArgumentsIteratorImplementationCommandLine command_line;
};

struct Tux64ArgumentsIterator {
   enum Tux64ArgumentsIteratorType type;
   union Tux64ArgumentsIteratorImplementation implementation;
};

/*----------------------------------------------------------------------------*/
/* Initializes the arguments iterator from command-line arguments.  Lifetimes */
/* of arguments will be the same as 'argv'.                                   */
/*----------------------------------------------------------------------------*/
void
tux64_arguments_iterator_initialize_command_line(
   struct Tux64ArgumentsIterator * self,
   Tux64UInt8 argc,
   const char * const * argv
);

/*----------------------------------------------------------------------------*/
/* Attempts to get the next argument, returning nullptr if no more exist.     */
/*----------------------------------------------------------------------------*/
const char *
tux64_arguments_iterator_next(
   struct Tux64ArgumentsIterator * self
);

/*----------------------------------------------------------------------------*/
/* Attempts to parse arguments according to the arguments list and context.   */
/* The lifetimes of all pointers will either be the same as 'iterator' or     */
/* will come from the context struct.                                         */
/*----------------------------------------------------------------------------*/
struct Tux64ArgumentsParseResult
tux64_arguments_parse(
   const struct Tux64ArgumentsList * list,
   struct Tux64ArgumentsIterator * iterator,
   void * context
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_ARGUMENTS_H */

