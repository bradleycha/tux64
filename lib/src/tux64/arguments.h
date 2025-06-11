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

enum Tux64ArgumentsIteratorNextStatus {
   TUX64_ARGUMENTS_ITERATOR_NEXT_STATUS_OK,
   TUX64_ARGUMENTS_ITERATOR_NEXT_STATUS_END_OF_STREAM
};

union Tux64ArgumentsIteratorNextPayload {
   struct Tux64String ok;
};

struct Tux64ArgumentsIteratorNextResult {
   enum Tux64ArgumentsIteratorNextStatus status;
   union Tux64ArgumentsIteratorNextPayload payload;
};

/*----------------------------------------------------------------------------*/
/* Attempts to get the next argument.                                         */
/*----------------------------------------------------------------------------*/
struct Tux64ArgumentsIteratorNextResult
tux64_arguments_iterator_next(
   struct Tux64ArgumentsIterator * self
);

enum Tux64ArgumentsParseStatus {
   _TUX64_ARGUMENTS_PARSE_STATUS_START_OFFSET = -1,

   TUX64_ARGUMENTS_PARSE_STATUS_OK,
   TUX64_ARGUMENTS_PARSE_STATUS_EXIT,
   TUX64_ARGUMENTS_PARSE_STATUS_NOT_AN_ARGUMENT,
   TUX64_ARGUMENTS_PARSE_STATUS_UNKNOWN_IDENTIFIER,
   TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_MISSING,
   TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_UNEXPECTED,
   TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_INVALID,
   TUX64_ARGUMENTS_PARSE_STATUS_REQUIRED_MISSING,

   _TUX64_ARGUMENTS_PARSE_STATUS_END_OFFSET
};

struct Tux64ArgumentsParsePayloadNotAnArgument {
   struct Tux64String argument;
};

struct Tux64ArgumentsParsePayloadUnknownIdentifier {
   struct Tux64String identifier;
};

struct Tux64ArgumentsParsePayloadParameterMissing {
   struct Tux64String identifier;
};

struct Tux64ArgumentsParsePayloadParameterUnexpected {
   struct Tux64String identifier;
   struct Tux64String parameter;
};

struct Tux64ArgumentsParsePayloadParameterInvalid {
   struct Tux64String identifier;
   struct Tux64String parameter;
   struct Tux64String reason;
};

struct Tux64ArgumentsParsePayloadRequiredMissing {
   struct Tux64String identifier;
};

union Tux64ArgumentsParsePayload {
   struct Tux64ArgumentsParsePayloadNotAnArgument not_an_argument;
   struct Tux64ArgumentsParsePayloadUnknownIdentifier unknown_identifier;
   struct Tux64ArgumentsParsePayloadParameterMissing parameter_missing;
   struct Tux64ArgumentsParsePayloadParameterUnexpected parameter_unexpected;
   struct Tux64ArgumentsParsePayloadParameterInvalid parameter_invalid;
   struct Tux64ArgumentsParsePayloadRequiredMissing required_missing;
};

struct Tux64ArgumentsParseResult {
   enum Tux64ArgumentsParseStatus status;
   union Tux64ArgumentsParsePayload payload;
};

struct Tux64ArgumentsParseOptionPayloadParameterInvalid {
   struct Tux64String reason;
};

union Tux64ArgumentsParseOptionPayload {
   struct Tux64ArgumentsParseOptionPayloadParameterInvalid parameter_invalid;
};

struct Tux64ArgumentsParseOptionResult {
   enum Tux64ArgumentsParseStatus status;
   union Tux64ArgumentsParseOptionPayload payload;
};

typedef struct Tux64ArgumentsParseOptionResult (*Tux64ArgumentsParseFunction)(
   const struct Tux64String * parameter,
   void * context
);

/* defines a single option */
struct Tux64ArgumentsOption {
   const struct Tux64String * identifiers_long;
   const char * identifiers_short;
   Tux64UInt32 identifiers_long_count;
   Tux64UInt32 identifiers_short_count;
   Tux64ArgumentsParseFunction parser;
};

/* defines a list of arguments in which to parse */
struct Tux64ArgumentsList {
   const struct Tux64ArgumentsOption * options_required;
   const struct Tux64ArgumentsOption * options_optional;
   Tux64UInt32 options_required_count;
   Tux64UInt32 options_optional_count;
   struct Tux64String prefix_long;
   struct Tux64String prefix_short;
   char identifier_parameter_split_token;
};

/*----------------------------------------------------------------------------*/
/* Attempts to parse arguments according to the arguments list and context.   */
/* The lifetimes of all pointers will either be the same as 'iterator' or     */
/* will come from the context struct.                                         */
/*                                                                            */
/* 'required_storage' is an array used to store which required arguments have */
/* been parsed.  This should be of size ceil(|required| / 8).                 */
/*----------------------------------------------------------------------------*/
struct Tux64ArgumentsParseResult
tux64_arguments_parse(
   const struct Tux64ArgumentsList * list,
   struct Tux64ArgumentsIterator * iterator,
   void * context,
   Tux64UInt8 * required_storage
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_ARGUMENTS_H */

