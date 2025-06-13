/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/arguments.c - Implementations for argument parsing           */
/*    functions.                                                              */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/arguments.h"

#include "tux64/memory.h"
#include "tux64/bitwise.h"

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
   struct Tux64String prefix_identifier;
   struct Tux64String identifier;
   struct Tux64String parameter;
   enum Tux64ArgumentsParsePrefixType prefix_type;
};

static struct Tux64ArgumentsParseTokens
tux64_arguments_parse_argument_tokenize(
   const struct Tux64ArgumentsList * list,
   const struct Tux64String * argument
) {
   struct Tux64ArgumentsParseTokens tokens;
   struct Tux64MemoryFindResult token_find_result;

   /* initialize prefix identifier to zero-length string slice of argument */
   tokens.prefix_identifier.ptr = argument->ptr;
   tokens.prefix_identifier.characters = TUX64_LITERAL_UINT32(0u);

   /* decide on the prefix type and count its characters */
   tokens.prefix_type = tux64_arguments_parse_prefix(list, argument);
   switch (tokens.prefix_type) {
      case TUX64_ARGUMENTS_PARSE_PREFIX_TYPE_LONG:
         tokens.prefix_identifier.characters += list->prefix_long.characters;
         break;
      case TUX64_ARGUMENTS_PARSE_PREFIX_TYPE_SHORT:
         tokens.prefix_identifier.characters += list->prefix_short.characters;
         break;
      case TUX64_ARGUMENTS_PARSE_PREFIX_TYPE_NONE:
         break;
      default:
         TUX64_UNREACHABLE;
   }

   /* isolate the identifier's ptr */
   tokens.identifier.ptr =
      tokens.prefix_identifier.ptr +
      tokens.prefix_identifier.characters;

   /* attempt to split the remaining argument, skipping the prefix */
   token_find_result = tux64_memory_find(
      &argument->ptr[tokens.prefix_identifier.characters],
      &list->identifier_parameter_split_token,
      argument->characters - tokens.prefix_identifier.characters,
      TUX64_LITERAL_UINT32(sizeof(list->identifier_parameter_split_token))
   );
   if (token_find_result.status != TUX64_MEMORY_FIND_STATUS_FOUND) {
      /* isolate the identifier */
      tokens.identifier.characters =
         argument->characters -
         tokens.prefix_identifier.characters;

      /* include the identifier in the prefix identifier */
      tokens.prefix_identifier.characters += tokens.identifier.characters;

      /* write empty string to parameter and get out! */
      tokens.parameter.characters = TUX64_LITERAL_UINT32(0u);
      return tokens;
   }

   /* store the final string lengths for the identifier and its prefix variant */
   tokens.identifier.characters = token_find_result.payload.found.position;
   tokens.prefix_identifier.characters += tokens.identifier.characters;

   /* calculate the string slice for the parameter */
   tokens.parameter.ptr =
      tokens.identifier.ptr +
      tokens.identifier.characters +
      TUX64_LITERAL_UINT32(1u);
   tokens.parameter.characters =
      argument->characters -
      tokens.prefix_identifier.characters -
      TUX64_LITERAL_UINT32(1u);

   /* done! */
   return tokens;
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

static struct Tux64ArgumentsParseResult
tux64_arguments_parse_argument_execute(
   const struct Tux64String * prefix_identifier,
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
         result.payload.parameter_missing.identifier = *prefix_identifier;
         break;

      case TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_UNEXPECTED:
         result.payload.parameter_unexpected.identifier = *prefix_identifier;
         result.payload.parameter_unexpected.parameter = *parameter;
         break;

      case TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_INVALID:
         result.payload.parameter_invalid.identifier = *prefix_identifier;
         result.payload.parameter_invalid.parameter = *parameter;
         result.payload.parameter_invalid.reason = parse_result.payload.parameter_invalid.reason;
         break;

      case TUX64_ARGUMENTS_PARSE_STATUS_REQUIRED_MISSING:
         result.payload.required_missing.identifier = *prefix_identifier;
         break;

      default:
         TUX64_UNREACHABLE;
   }
   
   result.status = parse_result.status;
   return result;
}

static struct Tux64ArgumentsParseResult
tux64_arguments_parse_argument_digest(
   const struct Tux64ArgumentsList * list,
   struct Tux64ArgumentsIterator * iterator,
   const struct Tux64ArgumentsParseTokens * tokens,
   const struct Tux64ArgumentsOption * option,
   void * context
) {
   struct Tux64ArgumentsParseResult result;
   struct Tux64ArgumentsIteratorNextResult next_argument;
   enum Tux64ArgumentsParsePrefixType next_argument_prefix;

   result = tux64_arguments_parse_argument_execute(
      &tokens->prefix_identifier,
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

   /* make sure the next argument isn't an identifier */
   next_argument_prefix = tux64_arguments_parse_prefix(
      list,
      &next_argument.payload.ok
   );
   if (next_argument_prefix != TUX64_ARGUMENTS_PARSE_PREFIX_TYPE_NONE) {
      return result;
   }

   return tux64_arguments_parse_argument_execute(
      &tokens->prefix_identifier,
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
   const struct Tux64ArgumentsParseTokens * tokens
) {
   struct Tux64ArgumentsParseResult result;
   const struct Tux64ArgumentsOption * iter_option;
   Tux64UInt32 index_option;

   index_option = TUX64_LITERAL_UINT32(0u);
   iter_option = list->options_required;
   do {
      if (tux64_arguments_parse_argument_long_is_match(
         iter_option,
         &tokens->identifier
      ) == TUX64_BOOLEAN_TRUE) {
         tux64_arguments_parse_argument_set_required(index_option, required_storage);
         return tux64_arguments_parse_argument_digest(
            list,
            iterator,
            tokens,
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
         &tokens->identifier
      ) == TUX64_BOOLEAN_TRUE) {
         return tux64_arguments_parse_argument_digest(
            list,
            iterator,
            tokens,
            iter_option,
            context
         );
      }

      index_option++;
      iter_option++;
   } while (index_option != list->options_optional_count);

   result.status = TUX64_ARGUMENTS_PARSE_STATUS_UNKNOWN_IDENTIFIER;
   result.payload.unknown_identifier.identifier = tokens->prefix_identifier;
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
   const struct Tux64ArgumentsParseTokens * tokens
) {
   struct Tux64ArgumentsParseResult result;
   char identifier;
   const struct Tux64ArgumentsOption * iter_option;
   Tux64UInt32 index_option;

   if (tokens->identifier.characters != TUX64_LITERAL_UINT32(1u)) {
      result.status = TUX64_ARGUMENTS_PARSE_STATUS_UNKNOWN_IDENTIFIER;
      result.payload.unknown_identifier.identifier = tokens->prefix_identifier;
      return result;
   }

   identifier = tokens->identifier.ptr[0];

   index_option = TUX64_LITERAL_UINT32(0u);
   iter_option = list->options_required;
   do {
      if (tux64_arguments_parse_argument_short_is_match(
         iter_option,
         identifier
      ) == TUX64_BOOLEAN_TRUE) {
         tux64_arguments_parse_argument_set_required(index_option, required_storage);
         return tux64_arguments_parse_argument_digest(
            list,
            iterator,
            tokens,
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
            list,
            iterator,
            tokens,
            iter_option,
            context
         );
      }

      index_option++;
      iter_option++;
   } while (index_option != list->options_optional_count);

   result.status = TUX64_ARGUMENTS_PARSE_STATUS_UNKNOWN_IDENTIFIER;
   result.payload.unknown_identifier.identifier = tokens->prefix_identifier;
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
   struct Tux64ArgumentsParseTokens tokens;

   tokens = tux64_arguments_parse_argument_tokenize(list, argument);

   switch (tokens.prefix_type) {
      case TUX64_ARGUMENTS_PARSE_PREFIX_TYPE_LONG:
         return tux64_arguments_parse_argument_long(
            list,
            iterator,
            context,
            required_storage,
            &tokens
         );

      case TUX64_ARGUMENTS_PARSE_PREFIX_TYPE_SHORT:
         return tux64_arguments_parse_argument_short(
            list,
            iterator,
            context,
            required_storage,
            &tokens
         );

      case TUX64_ARGUMENTS_PARSE_PREFIX_TYPE_NONE:
         result.status = TUX64_ARGUMENTS_PARSE_STATUS_NOT_AN_ARGUMENT;
         result.payload.not_an_argument.argument = *argument;
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

