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
#include <tux64/bitwise.h>

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

static void
tux64_arguments_parse_required_storage_initialize(
   const struct Tux64ArgumentsList * list,
   Tux64UInt8 * required_storage
) {
   Tux64UInt32 bytes;
   Tux64UInt8 sentinel;

   /* have to do a ceil-divide instead of floor-divide */
   bytes = (list->options_required_count + TUX64_LITERAL_UINT32(8u)) / TUX64_LITERAL_UINT32(8u);
   sentinel = 0x00;

   tux64_memory_fill(
      required_storage,
      &sentinel,
      bytes,
      TUX64_LITERAL_UINT32(sizeof(sentinel))
   );

   return;
}

static struct Tux64ArgumentsParseResult
tux64_arguments_parse_check_required_error_missing(
   const struct Tux64ArgumentsList * list,
   Tux64UInt32 argument_index,
   Tux64UInt32 flags
) {
   struct Tux64ArgumentsParseResult result;
   const struct Tux64ArgumentsOption * option;

   /* find the true argument index for the lowest set bit */
   while ((flags & TUX64_LITERAL_UINT32(1u)) == TUX64_LITERAL_UINT32(1u)) {
      argument_index++;
      flags >>= TUX64_LITERAL_UINT8(1u);
   }

   /* get the relevant option */
   option = &list->options_required[argument_index];

   /* choose the first available identifier and return */
   result.status = TUX64_ARGUMENTS_PARSE_STATUS_REQUIRED_MISSING;
   result.payload.required_missing.identifier = option->identifiers_long[0u];
   return result;
}

static struct Tux64ArgumentsParseResult
tux64_arguments_parse_check_required(
   const struct Tux64ArgumentsList * list,
   const Tux64UInt8 * required_storage
) {
   struct Tux64ArgumentsParseResult result;
   Tux64UInt32 full_bytes;
   Tux64UInt32 remaining_bits;
   Tux64UInt32 argument_index;
   Tux64UInt8 flags;
   Tux64UInt8 mask;
   const Tux64UInt8 * iter_required_storage;

   full_bytes              = list->options_required_count / TUX64_LITERAL_UINT32(8u);
   remaining_bits          = list->options_required_count % TUX64_LITERAL_UINT32(8u);
   iter_required_storage   = required_storage;
   argument_index          = TUX64_LITERAL_UINT32(0u);

   /* check full bytes at once*/
   while (full_bytes != TUX64_LITERAL_UINT32(0u)) {
      flags = *iter_required_storage;

      if (flags != TUX64_LITERAL_UINT8(0xff)) {
         return tux64_arguments_parse_check_required_error_missing(list, argument_index, flags);
      }

      argument_index += TUX64_LITERAL_UINT32(8u);
      iter_required_storage++;
      full_bytes--;
   }

   /* check any remaining bits */
   if (remaining_bits != TUX64_LITERAL_UINT32(0u)) {
      flags = *iter_required_storage;

      mask = tux64_bitwise_mask_set_all_low_uint8(remaining_bits);

      if (tux64_bitwise_flags_check_all_uint8(mask, flags) == TUX64_BOOLEAN_FALSE) {
         return tux64_arguments_parse_check_required_error_missing(list, argument_index, flags);
      }
   }

   result.status = TUX64_ARGUMENTS_PARSE_STATUS_OK;
   return result;
}

enum Tux64ArgumentsParsePrefixType {
   TUX64_ARGUMENTS_PARSE_PREFIX_TYPE_NONE,
   TUX64_ARGUMENTS_PARSE_PREFIX_TYPE_LONG,
   TUX64_ARGUMENTS_PARSE_PREFIX_TYPE_SHORT
};

static enum Tux64ArgumentsParsePrefixType
tux64_arguments_parse_prefix(
   const struct Tux64ArgumentsList * list,
   const struct Tux64String * argument
) {
   if (tux64_memory_compare_start(
      argument->ptr,
      list->prefix_long.ptr,
      argument->characters * TUX64_LITERAL_UINT32(sizeof(char)),
      list->prefix_long.characters * TUX64_LITERAL_UINT32(sizeof(char))
   ) == TUX64_BOOLEAN_TRUE) {
      return TUX64_ARGUMENTS_PARSE_PREFIX_TYPE_LONG;
   }
   if (tux64_memory_compare_start(
      argument->ptr,
      list->prefix_short.ptr,
      argument->characters * TUX64_LITERAL_UINT32(sizeof(char)),
      list->prefix_short.characters * TUX64_LITERAL_UINT32(sizeof(char))
   ) == TUX64_BOOLEAN_TRUE) {
      return TUX64_ARGUMENTS_PARSE_PREFIX_TYPE_SHORT;
   }

   return TUX64_ARGUMENTS_PARSE_PREFIX_TYPE_NONE;
}

struct Tux64ArgumentsParseTokens {
   struct Tux64String identifier;
   struct Tux64String parameter;
};

static struct Tux64ArgumentsParseTokens
tux64_arguments_parse_argument_tokenize(
   const struct Tux64ArgumentsList * list,
   const struct Tux64String * argument
) {
   struct Tux64ArgumentsParseTokens tokens;
   struct Tux64MemoryFindResult token_find_result;

   token_find_result = tux64_memory_find(
      argument->ptr,
      &list->identifier_parameter_split_token,
      argument->characters,
      TUX64_LITERAL_UINT32(sizeof(list->identifier_parameter_split_token))
   );
   if (token_find_result.status != TUX64_MEMORY_FIND_STATUS_FOUND) {
      tokens.identifier = *argument;
      tokens.parameter.characters = TUX64_LITERAL_UINT32(0u);
      return tokens;
   }

   tokens.identifier.ptr = argument->ptr;
   tokens.identifier.characters = token_find_result.payload.found.position;

   tokens.parameter.ptr =
      tokens.identifier.ptr +
      tokens.identifier.characters +
      TUX64_LITERAL_UINT32(1u);
   tokens.parameter.characters =
      argument->characters -
      tokens.identifier.characters -
      TUX64_LITERAL_UINT32(1u);

   return tokens;
}

static struct Tux64ArgumentsParseResult
tux64_arguments_parse_argument_execute(
   const struct Tux64String * identifier,
   const struct Tux64String * parameter,
   const struct Tux64ArgumentsOption * option,
   void * context
) {
   struct Tux64ArgumentsParseResult result;
   struct Tux64ArgumentsParseOptionResult parse_result;

   parse_result = option->parser(parameter, context);
   switch (parse_result.status) {
      case TUX64_ARGUMENTS_PARSE_STATUS_OK:
         break;

      case TUX64_ARGUMENTS_PARSE_STATUS_EXIT:
         break;

      case TUX64_ARGUMENTS_PARSE_STATUS_UNKNOWN_IDENTIFIER:
         /* don't return this, otherwise we're all dead! */
         TUX64_UNREACHABLE;

      case TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_MISSING:
         result.payload.parameter_missing.identifier = *identifier;
         break;

      case TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_UNEXPECTED:
         result.payload.parameter_unexpected.identifier = *identifier;
         result.payload.parameter_unexpected.parameter = *parameter;
         break;

      case TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_INVALID:
         result.payload.parameter_invalid.identifier = *identifier;
         result.payload.parameter_invalid.parameter = *parameter;
         result.payload.parameter_invalid.reason = parse_result.payload.parameter_invalid.reason;
         break;

      case TUX64_ARGUMENTS_PARSE_STATUS_REQUIRED_MISSING:
         result.payload.required_missing.identifier = *identifier;
         break;

      default:
         TUX64_UNREACHABLE;
   }
   
   result.status = parse_result.status;
   return result;
}

static struct Tux64ArgumentsParseResult
tux64_arguments_parse_argument_digest(
   struct Tux64ArgumentsIterator * iterator,
   const struct Tux64ArgumentsParseTokens * tokens,
   const struct Tux64ArgumentsOption * option,
   void * context
) {
   struct Tux64ArgumentsParseResult result;
   struct Tux64ArgumentsIteratorNextResult next_argument;

   result = tux64_arguments_parse_argument_execute(
      &tokens->identifier,
      &tokens->parameter,
      option,
      context
   );

   /* if we are missing the parameter, try one more time with the next argument */
   if (result.status != TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_MISSING) {
      return result;
   }

   next_argument = tux64_arguments_iterator_next(iterator);
   if (next_argument.status != TUX64_ARGUMENTS_ITERATOR_NEXT_STATUS_OK) {
      return result;
   }

   return tux64_arguments_parse_argument_execute(
      &tokens->identifier,
      &next_argument.payload.ok,
      option,
      context
   );
}

static void
tux64_arguments_parse_argument_set_required(
   Tux64UInt32 index_option,
   Tux64UInt8 * required_storage
) {
   Tux64UInt32 index_byte;
   Tux64UInt32 index_bit;

   index_byte  = index_option / TUX64_LITERAL_UINT32(8u);
   index_bit   = index_option % TUX64_LITERAL_UINT32(8u);
   required_storage[index_byte] = tux64_bitwise_flags_set_uint8(
      required_storage[index_byte],
      TUX64_LITERAL_UINT8(1u) << index_bit
   );

   return;
}

static Tux64Boolean
tux64_arguments_parse_argument_long_is_match(
   const struct Tux64ArgumentsOption * option,
   const struct Tux64String * identifier
) {
   const struct Tux64String * iter_identifiers;
   Tux64UInt32 identifiers_count;

   iter_identifiers = option->identifiers_long;
   identifiers_count = option->identifiers_long_count;
   do {
      if (tux64_memory_compare(
         iter_identifiers->ptr,
         identifier->ptr,
         iter_identifiers->characters * TUX64_LITERAL_UINT32(sizeof(char)),
         identifier->characters * TUX64_LITERAL_UINT32(sizeof(char))
      ) == TUX64_BOOLEAN_TRUE) {
         return TUX64_BOOLEAN_TRUE;
      }

      iter_identifiers++;
      identifiers_count--;
   } while (identifiers_count != TUX64_LITERAL_UINT32(0u));

   return TUX64_BOOLEAN_FALSE;
}

static struct Tux64ArgumentsParseResult
tux64_arguments_parse_argument_long(
   const struct Tux64ArgumentsList * list,
   struct Tux64ArgumentsIterator * iterator,
   void * context,
   Tux64UInt8 * required_storage,
   const struct Tux64String * argument,
   const struct Tux64String * argument_noprefix
) {
   struct Tux64ArgumentsParseResult result;
   struct Tux64ArgumentsParseTokens tokens;
   const struct Tux64ArgumentsOption * iter_option;
   Tux64UInt32 index_option;

   tokens = tux64_arguments_parse_argument_tokenize(list, argument_noprefix);

   index_option = TUX64_LITERAL_UINT32(0u);
   iter_option = list->options_required;
   do {
      if (tux64_arguments_parse_argument_long_is_match(
         iter_option,
         &tokens.identifier
      ) == TUX64_BOOLEAN_TRUE) {
         tux64_arguments_parse_argument_set_required(index_option, required_storage);
         return tux64_arguments_parse_argument_digest(
            iterator,
            &tokens,
            iter_option,
            context
         );
      }

      index_option++;
      iter_option++;
   } while (index_option != list->options_required_count);

   index_option = TUX64_LITERAL_UINT32(0u);
   iter_option = list->options_optional;
   do {
      if (tux64_arguments_parse_argument_long_is_match(
         &list->options_optional[index_option],
         &tokens.identifier
      ) == TUX64_BOOLEAN_TRUE) {
         return tux64_arguments_parse_argument_digest(
            iterator,
            &tokens,
            iter_option,
            context
         );
      }

      index_option++;
      iter_option++;
   } while (index_option != list->options_optional_count);

   result.status = TUX64_ARGUMENTS_PARSE_STATUS_UNKNOWN_IDENTIFIER;
   result.payload.unknown_identifier.identifier = *argument;
   return result;
}

static Tux64Boolean
tux64_arguments_parse_argument_short_is_match(
   const struct Tux64ArgumentsOption * option,
   char identifier
) {
   struct Tux64MemoryFindResult find_result;

   find_result = tux64_memory_find(
      option->identifiers_short,
      &identifier,
      option->identifiers_short_count,
      TUX64_LITERAL_UINT32(sizeof(identifier))
   );

   return find_result.status == TUX64_MEMORY_FIND_STATUS_FOUND;
}

static struct Tux64ArgumentsParseResult
tux64_arguments_parse_argument_short(
   const struct Tux64ArgumentsList * list,
   struct Tux64ArgumentsIterator * iterator,
   void * context,
   Tux64UInt8 * required_storage,
   const struct Tux64String * argument,
   const struct Tux64String * argument_noprefix
) {
   struct Tux64ArgumentsParseResult result;
   struct Tux64ArgumentsParseTokens tokens;
   char identifier;
   const struct Tux64ArgumentsOption * iter_option;
   Tux64UInt32 index_option;

   tokens = tux64_arguments_parse_argument_tokenize(list, argument_noprefix);

   if (tokens.identifier.characters != TUX64_LITERAL_UINT32(1u)) {
      result.status = TUX64_ARGUMENTS_PARSE_STATUS_UNKNOWN_IDENTIFIER;
      result.payload.unknown_identifier.identifier = *argument;
      return result;
   }

   identifier = tokens.identifier.ptr[0];

   index_option = TUX64_LITERAL_UINT32(0u);
   iter_option = list->options_required;
   do {
      if (tux64_arguments_parse_argument_short_is_match(
         iter_option,
         identifier
      ) == TUX64_BOOLEAN_TRUE) {
         tux64_arguments_parse_argument_set_required(index_option, required_storage);
         return tux64_arguments_parse_argument_digest(
            iterator,
            &tokens,
            iter_option,
            context
         );
      }

      index_option++;
      iter_option++;
   } while (index_option != list->options_required_count);

   index_option = TUX64_LITERAL_UINT32(0u);
   iter_option = list->options_optional;
   do {
      if (tux64_arguments_parse_argument_short_is_match(
         iter_option,
         identifier
      ) == TUX64_BOOLEAN_TRUE) {
         return tux64_arguments_parse_argument_digest(
            iterator,
            &tokens,
            iter_option,
            context
         );
      }

      index_option++;
      iter_option++;
   } while (index_option != list->options_optional_count);

   result.status = TUX64_ARGUMENTS_PARSE_STATUS_UNKNOWN_IDENTIFIER;
   result.payload.unknown_identifier.identifier = *argument;
   return result;
}

static struct Tux64ArgumentsParseResult
tux64_arguments_parse_argument(
   const struct Tux64ArgumentsList * list,
   struct Tux64ArgumentsIterator * iterator,
   void * context,
   Tux64UInt8 * required_storage,
   const struct Tux64String * argument
) {
   struct Tux64ArgumentsParseResult result;
   struct Tux64String argument_noprefix;

   switch (tux64_arguments_parse_prefix(list, argument)) {
      case TUX64_ARGUMENTS_PARSE_PREFIX_TYPE_LONG:
         argument_noprefix.ptr         = argument->ptr + list->prefix_long.characters;
         argument_noprefix.characters  = argument->characters - list->prefix_long.characters;
         return tux64_arguments_parse_argument_long(
            list,
            iterator,
            context,
            required_storage,
            argument,
            &argument_noprefix
         );

      case TUX64_ARGUMENTS_PARSE_PREFIX_TYPE_SHORT:
         argument_noprefix.ptr         = argument->ptr + list->prefix_short.characters;
         argument_noprefix.characters  = argument->characters - list->prefix_short.characters;
         return tux64_arguments_parse_argument_short(
            list,
            iterator,
            context,
            required_storage,
            argument,
            &argument_noprefix
         );

      case TUX64_ARGUMENTS_PARSE_PREFIX_TYPE_NONE:
         result.status = TUX64_ARGUMENTS_PARSE_STATUS_UNKNOWN_IDENTIFIER;
         result.payload.unknown_identifier.identifier = *argument;
         return result;

      default:
         TUX64_UNREACHABLE;
   }

   TUX64_UNREACHABLE;
}

struct Tux64ArgumentsParseResult
tux64_arguments_parse(
   const struct Tux64ArgumentsList * list,
   struct Tux64ArgumentsIterator * iterator,
   void * context,
   Tux64UInt8 * required_storage
) {
   struct Tux64ArgumentsParseResult result;
   struct Tux64ArgumentsIteratorNextResult argument;

   /* zero-initialize required_storage */
   tux64_arguments_parse_required_storage_initialize(list, required_storage);

   /* parse each argument individually */
   argument = tux64_arguments_iterator_next(iterator);
   while (argument.status == TUX64_ARGUMENTS_ITERATOR_NEXT_STATUS_OK) {
      result = tux64_arguments_parse_argument(
         list,
         iterator,
         context,
         required_storage,
         &argument.payload.ok
      );
      if (result.status != TUX64_ARGUMENTS_PARSE_STATUS_OK) {
         return result;
      }

      argument = tux64_arguments_iterator_next(iterator);
   }

   /* check that all required arguments were present and GTFO */
   result = tux64_arguments_parse_check_required(list, required_storage);
   return result;
}

