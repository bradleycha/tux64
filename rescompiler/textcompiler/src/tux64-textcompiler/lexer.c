/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* rescompiler/textcompiler/src/tux64-textcompiler/lexer.c - Implementations  */
/*    for the text file parser.                                               */
/*----------------------------------------------------------------------------*/

#include "tux64-textcompiler/tux64-textcompiler.h"
#include "tux64-textcompiler/lexer.h"

#include <tux64/string.h>
#include <tux64/memory.h>
#include <tux64/bitwise.h>

#include <stdlib.h>

enum Tux64TextCompilerLexerStringArrayAllocStatus {
   TUX64_TEXTCOMPILER_LEXER_STRING_ARRAY_ALLOC_STATUS_OK,
   TUX64_TEXTCOMPILER_LEXER_STRING_ARRAY_ALLOC_STATUS_OUT_OF_MEMORY
};

static enum Tux64TextCompilerLexerStringArrayAllocStatus
tux64_textcompiler_lexer_string_array_initialize(
   struct Tux64TextCompilerLexerStringArray * string_array
) {
   string_array->ptr = (struct Tux64TextCompilerLexerString *)malloc(sizeof(struct Tux64TextCompilerLexerString) * 1u);
   if (string_array->ptr == NULL) {
      return TUX64_TEXTCOMPILER_LEXER_STRING_ARRAY_ALLOC_STATUS_OUT_OF_MEMORY;
   }

   string_array->length    = TUX64_LITERAL_UINT32(0u);
   string_array->capacity  = TUX64_LITERAL_UINT32(1u);

   return TUX64_TEXTCOMPILER_LEXER_STRING_ARRAY_ALLOC_STATUS_OK;
}

static enum Tux64TextCompilerLexerStringArrayAllocStatus
tux64_textcompiler_lexer_string_array_push(
   struct Tux64TextCompilerLexerStringArray * string_array
) {
   if (string_array->length != string_array->capacity - TUX64_LITERAL_UINT32(1u)) {
      /* space complexity of O(2^n) to try and reduce number of reallocations */
      string_array->capacity = string_array->capacity * TUX64_LITERAL_UINT32(2u);
      
      string_array->ptr = (struct Tux64TextCompilerLexerString *)realloc(
         string_array->ptr,
         sizeof(struct Tux64TextCompilerLexerString) * string_array->capacity
      );

      if (string_array->ptr == NULL) {
         return TUX64_TEXTCOMPILER_LEXER_STRING_ARRAY_ALLOC_STATUS_OUT_OF_MEMORY;
      }
   }

   string_array->length++;

   return TUX64_TEXTCOMPILER_LEXER_STRING_ARRAY_ALLOC_STATUS_OK;
}

void
tux64_textcompiler_lexer_string_array_free(
   struct Tux64TextCompilerLexerStringArray * string_array
) {
   free(string_array->ptr);
   return;
}

static const struct Tux64TextCompilerLexerString *
tux64_textcompiler_lexer_string_array_get_top(
   const struct Tux64TextCompilerLexerStringArray * string_array
) {
   Tux64UInt32 idx;

   idx = string_array->length - TUX64_LITERAL_UINT32(1u);

   return &string_array->ptr[idx];
}

static struct Tux64TextCompilerLexerString *
tux64_textcompiler_lexer_string_array_get_top_mut(
   struct Tux64TextCompilerLexerStringArray * string_array
) {
   Tux64UInt32 idx;

   idx = string_array->length - TUX64_LITERAL_UINT32(1u);

   return &string_array->ptr[idx];
}

/* this lexer works by using a finite state machine combined with an iterator */
/* that keeps track of the current line and column, as well as removes all    */
/* newline characters.  this is good since we can treat the input, with       */
/* respect to our state machine, as a single long string.  the finite state   */
/* machine also massively simplifies the implementation, as well as allowing  */
/* only a single iteration to parse the entire file.                          */
/*                                                                            */
/* despite these simplifications, this is still lexifying a code file, so     */
/* here be dragons, only the truly mighty should even attempt to understand   */
/* this schizo bullshit.                                                      */

struct Tux64TextCompilerLexerFileIterator {
   struct Tux64String slice;
   struct Tux64TextCompilerLexerLocation location;
   struct Tux64TextCompilerLexerLocation location_prev;
};

static void
tux64_textcompiler_lexer_file_iterator_initialize(
   struct Tux64TextCompilerLexerFileIterator * iter,
   const struct Tux64String * contents
) {
   iter->slice = *contents;
   iter->location.line = TUX64_LITERAL_UINT32(0u);
   iter->location.column = TUX64_LITERAL_UINT32(0u);
   iter->location_prev.line = TUX64_LITERAL_UINT32(0u);
   iter->location_prev.column = TUX64_LITERAL_UINT32(0u);
   return;
}

enum Tux64TextCompilerLexerFileIteratorNextStatus {
   TUX64_TEXTCOMPILER_LEXER_FILE_ITERATOR_NEXT_STATUS_OK,
   TUX64_TEXTCOMPILER_LEXER_FILE_ITERATOR_NEXT_STATUS_END_OF_FILE
};

struct Tux64TextCompilerLexerFileIteratorNextResult {
   enum Tux64TextCompilerLexerFileIteratorNextStatus status;
   char character;
};

static struct Tux64TextCompilerLexerFileIteratorNextResult
tux64_textcompiler_lexer_file_iterator_next(
   struct Tux64TextCompilerLexerFileIterator * iter
) {
   struct Tux64TextCompilerLexerFileIteratorNextResult result;
   char character;

   iter->location_prev = iter->location;

   while (iter->slice.characters != TUX64_LITERAL_UINT32(0u)) {
      character = iter->slice.ptr[0u];
      iter->slice.ptr++;
      iter->slice.characters--;

      if (character != '\n') {
         iter->location.column++;
         result.status = TUX64_TEXTCOMPILER_LEXER_FILE_ITERATOR_NEXT_STATUS_OK;
         result.character = character;
         return result;
      }

      iter->location.line++;
      iter->location.column = TUX64_LITERAL_UINT32(0u);
   }

   result.status = TUX64_TEXTCOMPILER_LEXER_FILE_ITERATOR_NEXT_STATUS_END_OF_FILE;
   return result;
}

struct Tux64TextCompilerLexerIdentifierHash {
   Tux64UInt32 value;
};

static void
tux64_textcompiler_lexer_identifier_hash_initialize(
   struct Tux64TextCompilerLexerIdentifierHash * hash
) {
   hash->value = TUX64_LITERAL_UINT32(0x00000000u);
   return;
}

static void
tux64_textcompiler_lexer_identifier_hash_digest(
   struct Tux64TextCompilerLexerIdentifierHash * hash,
   char character
) {
   Tux64UInt32 v;
   union Tux64Int8 c;

   /* this doesn't need to be super duper amazing, just enough to prevent */
   /* basic collisions. */

   v = hash->value;
   c.sint = character;
   
   v = tux64_bitwise_rotate_left_uint32(v, TUX64_LITERAL_UINT8(sizeof(c) * 8u));
   v = (v ^ (Tux64UInt32)c.uint);

   hash->value = v;

   return;
}

static Tux64UInt32
tux64_textcompiler_lexer_identifier_hash_finalize(
   struct Tux64TextCompilerLexerIdentifierHash * hash
) {
   return hash->value;
}

struct Tux64TextCompilerLexerFsm;

typedef enum Tux64TextCompilerLexerParseStatus (*Tux64TextCompilerLexerFsmStatePfnFunc)(
   struct Tux64TextCompilerLexerFsm * fsm
);

struct Tux64TextCompilerLexerFsmStateMemComment {
   Tux64TextCompilerLexerFsmStatePfnFunc state_prev;
};

struct Tux64TextCompilerFsmStateMemIdentifier {
   struct Tux64TextCompilerLexerIdentifierHash hash;
};

struct Tux64TextCompilerFsmStateMemValueIdentifier {
   struct Tux64String * field;
};

struct Tux64TextCompilerFsmStateMemValueText {
   struct Tux64TextCompilerLexerText * field;
};

union Tux64TextCompilerFsmStateMemValue {
   struct Tux64TextCompilerFsmStateMemValueIdentifier identifier;
   struct Tux64TextCompilerFsmStateMemValueText text;
};

enum Tux64TextCompilerFsmStateStringEntryValueParser {
   TUX64_TEXTCOMPILER_FSM_STATE_STRING_ENTRY_VALUE_PARSER_IDENTIFIER,
   TUX64_TEXTCOMPILER_FSM_STATE_STRING_ENTRY_VALUE_PARSER_TEXT
};

struct Tux64TextCompilerFsmStateMemValueTransition {
   union Tux64TextCompilerFsmStateMemValue value;
   enum Tux64TextCompilerFsmStateStringEntryValueParser parser;
};

union Tux64TextCompilerLexerFsmStateMemShared {
   struct Tux64TextCompilerFsmStateMemIdentifier identifier;
   struct Tux64TextCompilerFsmStateMemValueTransition value_transition;
   union Tux64TextCompilerFsmStateMemValue value;
};

struct Tux64TextCompilerLexerFsmStateMemExclusive {
   struct Tux64TextCompilerLexerFsmStateMemComment comment;
};

struct Tux64TextCompilerLexerFsmStateMem {
   union Tux64TextCompilerLexerFsmStateMemShared shared;
   struct Tux64TextCompilerLexerFsmStateMemExclusive exclusive;
};

struct Tux64TextCompilerLexerFsmState {
   Tux64TextCompilerLexerFsmStatePfnFunc func;
   struct Tux64TextCompilerLexerFsmStateMem mem;
};

struct Tux64TextCompilerLexerFsm {
   struct Tux64TextCompilerLexerFsmState state;
   struct Tux64TextCompilerLexerFileIterator iter_file;
   struct Tux64TextCompilerLexerStringArray string_array;
};

static enum Tux64TextCompilerLexerStringArrayAllocStatus
tux64_textcompiler_lexer_fsm_initialize(
   struct Tux64TextCompilerLexerFsm * fsm,
   const struct Tux64String * text
) {
   tux64_textcompiler_lexer_file_iterator_initialize(
      &fsm->iter_file,
      text
   );

   switch (tux64_textcompiler_lexer_string_array_initialize(&fsm->string_array)) {
      case TUX64_TEXTCOMPILER_LEXER_STRING_ARRAY_ALLOC_STATUS_OK:
         break;

      case TUX64_TEXTCOMPILER_LEXER_STRING_ARRAY_ALLOC_STATUS_OUT_OF_MEMORY:
         return TUX64_TEXTCOMPILER_LEXER_STRING_ARRAY_ALLOC_STATUS_OUT_OF_MEMORY;

      default:
         TUX64_UNREACHABLE;
   }

   return TUX64_TEXTCOMPILER_LEXER_STRING_ARRAY_ALLOC_STATUS_OK;
}

static void
tux64_textcompiler_lexer_fsm_state_set(
   struct Tux64TextCompilerLexerFsm * fsm,
   Tux64TextCompilerLexerFsmStatePfnFunc state
) {
   fsm->state.func = state;
   return;
}

static void
tux64_textcompiler_lexer_fsm_state_exit(
   struct Tux64TextCompilerLexerFsm * fsm
) {
   tux64_textcompiler_lexer_fsm_state_set(fsm, TUX64_NULLPTR);
   return;
}

static enum Tux64TextCompilerLexerParseStatus
tux64_textcompiler_lexer_fsm_state_execute(
   struct Tux64TextCompilerLexerFsm * fsm
) {
   return fsm->state.func(fsm);
}

static Tux64Boolean
tux64_textcompiler_lexer_fsm_state_has_exited(
   struct Tux64TextCompilerLexerFsm * fsm
) {
   return (fsm->state.func == TUX64_NULLPTR) ?
      TUX64_BOOLEAN_TRUE :
      TUX64_BOOLEAN_FALSE;
}

/* use of funky-looking macros to reduce copy+pasting work */
#define TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DECLARATION(identifier)\
   static enum Tux64TextCompilerLexerParseStatus \
   identifier (struct Tux64TextCompilerLexerFsm *)
#define TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DEFINITION(identifier)\
   static enum Tux64TextCompilerLexerParseStatus \
   identifier (struct Tux64TextCompilerLexerFsm * fsm)

TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DECLARATION(tux64_textcompiler_lexer_fsm_state_initial);
TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DECLARATION(tux64_textcompiler_lexer_fsm_state_comment_start);
TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DECLARATION(tux64_textcompiler_lexer_fsm_state_comment);
TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DECLARATION(tux64_textcompiler_lexer_fsm_state_comment_end);
TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DECLARATION(tux64_textcompiler_lexer_fsm_state_string_entry_initial);
TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DECLARATION(tux64_textcompiler_lexer_fsm_state_string_entry_identifier);
TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DECLARATION(tux64_textcompiler_lexer_fsm_state_string_entry_wait_separator);
TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DECLARATION(tux64_textcompiler_lexer_fsm_state_string_entry_wait_value);
TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DECLARATION(tux64_textcompiler_lexer_fsm_state_string_entry_value_identifier);
TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DECLARATION(tux64_textcompiler_lexer_fsm_state_string_entry_value_text);
TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DECLARATION(tux64_textcompiler_lexer_fsm_state_string_entry_value_wait_newline);

static void
tux64_textcompiler_lexer_fsm_state_set_comment_start(
   struct Tux64TextCompilerLexerFsm * fsm
) {
   fsm->state.mem.exclusive.comment.state_prev = fsm->state.func;
   tux64_textcompiler_lexer_fsm_state_set(fsm, tux64_textcompiler_lexer_fsm_state_comment_start);
   return;
}

static enum Tux64TextCompilerLexerParseStatus
tux64_textcompiler_lexer_fsm_state_set_new_string_entry(
   struct Tux64TextCompilerLexerFsm * fsm
) {
   struct Tux64TextCompilerLexerString * new_string;

   switch (tux64_textcompiler_lexer_string_array_push(&fsm->string_array)) {
      case TUX64_TEXTCOMPILER_LEXER_STRING_ARRAY_ALLOC_STATUS_OK:
         break;

      case TUX64_TEXTCOMPILER_LEXER_STRING_ARRAY_ALLOC_STATUS_OUT_OF_MEMORY:
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OUT_OF_MEMORY;

      default:
         TUX64_UNREACHABLE;
   }

   new_string = tux64_textcompiler_lexer_string_array_get_top_mut(&fsm->string_array);

   new_string->identifiers.data.characters   = TUX64_LITERAL_UINT32(0u);
   new_string->identifiers.length.characters = TUX64_LITERAL_UINT32(0u);
   new_string->text.characters               = TUX64_LITERAL_UINT8 (0u);

   tux64_textcompiler_lexer_fsm_state_set(fsm, tux64_textcompiler_lexer_fsm_state_string_entry_initial);
   return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;
}

static enum Tux64TextCompilerLexerParseStatus
tux64_textcompiler_lexer_fsm_set_finalize_string_entry(
   struct Tux64TextCompilerLexerFsm * fsm
) {
   Tux64Boolean missing_field;
   const struct Tux64TextCompilerLexerString * s;

   s = tux64_textcompiler_lexer_string_array_get_top(&fsm->string_array);

   missing_field = TUX64_BOOLEAN_FALSE;

   missing_field |= (s->identifiers.data.characters   == TUX64_LITERAL_UINT32(0u));
   missing_field |= (s->identifiers.length.characters == TUX64_LITERAL_UINT32(0u));
   missing_field |= (s->text.characters               == TUX64_LITERAL_UINT8 (0u));
   
   switch (missing_field) {
      case TUX64_BOOLEAN_FALSE:
         tux64_textcompiler_lexer_fsm_state_set(fsm, tux64_textcompiler_lexer_fsm_state_initial);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;

      case TUX64_BOOLEAN_TRUE:
         tux64_textcompiler_lexer_fsm_state_exit(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_MISSING_REQUIRED_FIELD;

      default:
         TUX64_UNREACHABLE;
   }

   TUX64_UNREACHABLE;
}

static void
tux64_textcompiler_lexer_fsm_state_set_string_entry_identifier(
   struct Tux64TextCompilerLexerFsm * fsm
) {
   tux64_textcompiler_lexer_identifier_hash_initialize(&fsm->state.mem.shared.identifier.hash);
   tux64_textcompiler_lexer_fsm_state_set(fsm, tux64_textcompiler_lexer_fsm_state_string_entry_identifier);
   return;
}

/* there's no easy way around this.  we have to hard-code hash values for our */
/* identifiers.  the only other way is to meta-program our meta-programming   */
/* tool, or calculate these at runtime.                                       */
#define TUX64_TEXTCOMPILER_LEXER_STRING_ENTRY_IDENTIFIER_HASH_IDENTIFIER_DATA \
   0x63190b3du
#define TUX64_TEXTCOMPILER_LEXER_STRING_ENTRY_IDENTIFIER_HASH_IDENTIFIER_LENGTH \
   0x113b1f75u
#define TUX64_TEXTCOMPILER_LEXER_STRING_ENTRY_IDENTIFIER_HASH_TEXT \
   0x74657874u

static enum Tux64TextCompilerLexerParseStatus
tux64_textcompiler_lexer_fsm_set_string_entry_post_identifier(
   struct Tux64TextCompilerLexerFsm * fsm,
   Tux64TextCompilerLexerFsmStatePfnFunc next_state
) {
   struct Tux64TextCompilerLexerString * string;
   struct Tux64TextCompilerFsmStateMemValueTransition * mem;
   Tux64UInt32 hash;

   string   = tux64_textcompiler_lexer_string_array_get_top_mut(&fsm->string_array);
   mem      = &fsm->state.mem.shared.value_transition;

   hash = tux64_textcompiler_lexer_identifier_hash_finalize(&fsm->state.mem.shared.identifier.hash);

   switch (hash) {
      case TUX64_TEXTCOMPILER_LEXER_STRING_ENTRY_IDENTIFIER_HASH_IDENTIFIER_DATA:
         mem->value.identifier.field = &string->identifiers.data;
         mem->parser = TUX64_TEXTCOMPILER_FSM_STATE_STRING_ENTRY_VALUE_PARSER_IDENTIFIER;
         break;

      case TUX64_TEXTCOMPILER_LEXER_STRING_ENTRY_IDENTIFIER_HASH_IDENTIFIER_LENGTH:
         mem->value.identifier.field = &string->identifiers.length;
         mem->parser = TUX64_TEXTCOMPILER_FSM_STATE_STRING_ENTRY_VALUE_PARSER_IDENTIFIER;
         break;

      case TUX64_TEXTCOMPILER_LEXER_STRING_ENTRY_IDENTIFIER_HASH_TEXT:
         mem->value.text.field = &string->text;
         mem->parser = TUX64_TEXTCOMPILER_FSM_STATE_STRING_ENTRY_VALUE_PARSER_TEXT;
         break;

      default:
         tux64_textcompiler_lexer_fsm_state_exit(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_UNKNOWN_FIELD;
   }

   tux64_textcompiler_lexer_fsm_state_set(fsm, next_state);
   return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;
}

static enum Tux64TextCompilerLexerParseStatus
tux64_textcompiler_lexer_fsm_set_string_entry_separator(
   struct Tux64TextCompilerLexerFsm * fsm
) {
   return tux64_textcompiler_lexer_fsm_set_string_entry_post_identifier(
      fsm,
      tux64_textcompiler_lexer_fsm_state_string_entry_wait_separator
   );
}

static enum Tux64TextCompilerLexerParseStatus
tux64_textcompiler_lexer_fsm_set_string_entry_value_initial(
   struct Tux64TextCompilerLexerFsm * fsm
) {
   return tux64_textcompiler_lexer_fsm_set_string_entry_post_identifier(
      fsm,
      tux64_textcompiler_lexer_fsm_state_string_entry_wait_value
   );
}

static enum Tux64TextCompilerLexerParseStatus
tux64_textcompiler_lexer_fsm_set_exit_string_entry_value_identifier(
   struct Tux64TextCompilerLexerFsm * fsm,
   Tux64TextCompilerLexerFsmStatePfnFunc next_state
) {
   struct Tux64String * identifier;

   identifier = fsm->state.mem.shared.value.identifier.field;

   if (identifier->characters == TUX64_LITERAL_UINT32(0u)) {
      tux64_textcompiler_lexer_fsm_state_exit(fsm);
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_MISSING_VALUE;
   }

   tux64_textcompiler_lexer_fsm_state_set(fsm, next_state);
   return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;
}

static enum Tux64TextCompilerLexerParseStatus
tux64_textcompiler_lexer_fsm_digest_string_entry_value_identifier_character(
   struct Tux64TextCompilerLexerFsm * fsm,
   char character
) {
   struct Tux64String * identifier;

   identifier = fsm->state.mem.shared.value.identifier.field;

   if (tux64_string_character_is_whitespace(character)) {
      return tux64_textcompiler_lexer_fsm_set_exit_string_entry_value_identifier(fsm, tux64_textcompiler_lexer_fsm_state_string_entry_value_wait_newline);
   }

   switch (character) {
      case '{':
      case '}':
      case '/':
      case '*':
      case '\"':
         tux64_textcompiler_lexer_fsm_state_exit(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_MALFORMED_FIELD;

      case ';':
         return tux64_textcompiler_lexer_fsm_set_exit_string_entry_value_identifier(fsm, tux64_textcompiler_lexer_fsm_state_string_entry_initial);

      default:
         identifier->characters++;
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;
   }

   TUX64_UNREACHABLE;
}

typedef enum Tux64TextCompilerLexerParseStatus (*Tux64TextCompilerLexerFsmStatePfnValueTransition)(
   struct Tux64TextCompilerLexerFsm * fsm,
   char initial_character
);

static enum Tux64TextCompilerLexerParseStatus
tux64_textcompiler_lexer_fsm_set_string_entry_value_identifier(
   struct Tux64TextCompilerLexerFsm * fsm,
   char initial_character
) {
   struct Tux64String * identifier;

   identifier = fsm->state.mem.shared.value.identifier.field;

   /* we set the ptr preemptively, also rewinding by 1 character to account */
   /* for the 1 iteration. also, we don't have to initialize 'characters' */
   /* because it's already zero-initialized */
   identifier->ptr = fsm->iter_file.slice.ptr - 1u;

   /* additional check to give better error reporting.  without this, we just */
   /* return a generic malformed field error. */
   if (initial_character == '\"') {
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_INCORRECT_TYPE_IDENTIFIER;
   }

   tux64_textcompiler_lexer_fsm_state_set(fsm, tux64_textcompiler_lexer_fsm_state_string_entry_value_identifier);
   return tux64_textcompiler_lexer_fsm_digest_string_entry_value_identifier_character(fsm, initial_character);
}

static enum Tux64TextCompilerLexerParseStatus
tux64_textcompiler_lexer_fsm_set_string_entry_value_text(
   struct Tux64TextCompilerLexerFsm * fsm,
   char initial_character
) {
   struct Tux64TextCompilerLexerText * text;

   text = fsm->state.mem.shared.value.text.field;

   switch (initial_character) {
      case '{':
      case '}':
      case '/':
      case '*':
      case '=':
      case '.':
         tux64_textcompiler_lexer_fsm_state_exit(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_MALFORMED_FIELD;

      case ';':
         tux64_textcompiler_lexer_fsm_state_exit(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_MISSING_VALUE;

      case '\"':
         /* 'characters' is zero-initialized, so we don't have to set it again. */
         text->ptr = fsm->iter_file.slice.ptr;
         tux64_textcompiler_lexer_fsm_state_set(fsm, tux64_textcompiler_lexer_fsm_state_string_entry_value_text);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;      

      default:
         tux64_textcompiler_lexer_fsm_state_exit(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_INCORRECT_TYPE_TEXT;
   }

   TUX64_UNREACHABLE;
}

static enum Tux64TextCompilerLexerParseStatus
tux64_textcompiler_lexer_fsm_set_string_entry_value(
   struct Tux64TextCompilerLexerFsm * fsm,
   char initial_character
) {
   const struct Tux64TextCompilerFsmStateMemValueTransition * value_transition;
   union Tux64TextCompilerFsmStateMemValue value_copy;
   Tux64TextCompilerLexerFsmStatePfnValueTransition value_class;

   value_transition = &fsm->state.mem.shared.value_transition;

   /* we have to store this locally to avoid possibly clobbering state due to */
   /* pointer aliasing and other weird issues. */
   value_copy = value_transition->value;

   switch (value_transition->parser) {
      case TUX64_TEXTCOMPILER_FSM_STATE_STRING_ENTRY_VALUE_PARSER_IDENTIFIER:
         value_class = tux64_textcompiler_lexer_fsm_set_string_entry_value_identifier;
         break;

      case TUX64_TEXTCOMPILER_FSM_STATE_STRING_ENTRY_VALUE_PARSER_TEXT:
         value_class = tux64_textcompiler_lexer_fsm_set_string_entry_value_text;
         break;

      default:
         TUX64_UNREACHABLE;
   }

   fsm->state.mem.shared.value = value_copy;
   return value_class(fsm, initial_character);
}

static Tux64Boolean
tux64_textcompiler_lexer_fsm_string_entry_character_is_valid(
   char c
) {
   if (c >= 'A' && c <= 'Z') {
      return TUX64_BOOLEAN_TRUE;
   }
   if (c >= 'a' && c <= 'z') {
      return TUX64_BOOLEAN_TRUE;
   }
   if (c >= '0' && c <= '9') {
      return TUX64_BOOLEAN_TRUE;
   }

   switch (c) {
      case '%':
      case '.':
      case ' ':
         return TUX64_BOOLEAN_TRUE;

      default:
         return TUX64_BOOLEAN_FALSE;
   }

   TUX64_UNREACHABLE;
}

static enum Tux64TextCompilerLexerParseStatus
tux64_textcompiler_lexer_fsm_set_string_entry_value_text_exit(
   struct Tux64TextCompilerLexerFsm * fsm
) {
   const struct Tux64TextCompilerLexerText * text;

   text = fsm->state.mem.shared.value.text.field;

   if (text->characters == TUX64_LITERAL_UINT32(0u)) {
      tux64_textcompiler_lexer_fsm_state_exit(fsm);
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_VALUE_EMPTY;
   }

   tux64_textcompiler_lexer_fsm_state_set(fsm, tux64_textcompiler_lexer_fsm_state_string_entry_value_wait_newline);
   return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;
}

TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DEFINITION(tux64_textcompiler_lexer_fsm_state_initial) {
   struct Tux64TextCompilerLexerFileIteratorNextResult iter_result;
   char c;

   iter_result = tux64_textcompiler_lexer_file_iterator_next(&fsm->iter_file);
   if (iter_result.status == TUX64_TEXTCOMPILER_LEXER_FILE_ITERATOR_NEXT_STATUS_END_OF_FILE) {
      tux64_textcompiler_lexer_fsm_state_exit(fsm);

      if (fsm->string_array.length == TUX64_LITERAL_UINT32(0u)) {
         tux64_textcompiler_lexer_fsm_state_exit(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_EMPTY_FILE;
      }

      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;
   }
   c = iter_result.character;

   if (tux64_string_character_is_whitespace(c)) {
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;
   }

   switch (c) {
      case '{':
         return tux64_textcompiler_lexer_fsm_state_set_new_string_entry(fsm);

      case '/':
         tux64_textcompiler_lexer_fsm_state_set_comment_start(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;

      case '*':
         tux64_textcompiler_lexer_fsm_state_exit(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_COMMENT_END_UNMATCHED;

      default:
         tux64_textcompiler_lexer_fsm_state_exit(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_MALFORMED;
   }

   TUX64_UNREACHABLE;
}

TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DEFINITION(tux64_textcompiler_lexer_fsm_state_comment_start) {
   struct Tux64TextCompilerLexerFileIteratorNextResult iter_result;
   char c;

   iter_result = tux64_textcompiler_lexer_file_iterator_next(&fsm->iter_file);
   if (iter_result.status == TUX64_TEXTCOMPILER_LEXER_FILE_ITERATOR_NEXT_STATUS_END_OF_FILE) {
      tux64_textcompiler_lexer_fsm_state_exit(fsm);
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_COMMENT_START_INVALID;
   }
   c = iter_result.character;

   if (c != '*') {
      tux64_textcompiler_lexer_fsm_state_exit(fsm);
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_COMMENT_START_INVALID;
   }

   tux64_textcompiler_lexer_fsm_state_set(fsm, tux64_textcompiler_lexer_fsm_state_comment);
   return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;
}

TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DEFINITION(tux64_textcompiler_lexer_fsm_state_comment) {
   struct Tux64TextCompilerLexerFileIteratorNextResult iter_result;
   char c;

   iter_result = tux64_textcompiler_lexer_file_iterator_next(&fsm->iter_file);
   if (iter_result.status == TUX64_TEXTCOMPILER_LEXER_FILE_ITERATOR_NEXT_STATUS_END_OF_FILE) {
      tux64_textcompiler_lexer_fsm_state_exit(fsm);
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_COMMENT_UNTERMINATED;
   }
   c = iter_result.character;

   if (c == '*') {
      tux64_textcompiler_lexer_fsm_state_set(fsm, tux64_textcompiler_lexer_fsm_state_comment_end);
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;
   }

   return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;
}

TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DEFINITION(tux64_textcompiler_lexer_fsm_state_comment_end) {
   struct Tux64TextCompilerLexerFileIteratorNextResult iter_result;
   char c;

   iter_result = tux64_textcompiler_lexer_file_iterator_next(&fsm->iter_file);
   if (iter_result.status == TUX64_TEXTCOMPILER_LEXER_FILE_ITERATOR_NEXT_STATUS_END_OF_FILE) {
      tux64_textcompiler_lexer_fsm_state_exit(fsm);
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_COMMENT_UNTERMINATED;
   }
   c = iter_result.character;

   switch (c) {
      case '/':
         tux64_textcompiler_lexer_fsm_state_set(fsm, fsm->state.mem.exclusive.comment.state_prev);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;

      case '*':
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;

      default:
         tux64_textcompiler_lexer_fsm_state_set(fsm, tux64_textcompiler_lexer_fsm_state_comment);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;
   }

   TUX64_UNREACHABLE;
}

TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DEFINITION(tux64_textcompiler_lexer_fsm_state_string_entry_initial) {
   struct Tux64TextCompilerLexerFileIteratorNextResult iter_result;
   char c;

   iter_result = tux64_textcompiler_lexer_file_iterator_next(&fsm->iter_file);
   if (iter_result.status == TUX64_TEXTCOMPILER_LEXER_FILE_ITERATOR_NEXT_STATUS_END_OF_FILE) {
      tux64_textcompiler_lexer_fsm_state_exit(fsm);
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_UNTERMINATED;
   }
   c = iter_result.character;

   if (tux64_string_character_is_whitespace(c)) {
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;
   }

   switch (c) {
      case '/':
         tux64_textcompiler_lexer_fsm_state_set_comment_start(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;
      
      case '*':
         tux64_textcompiler_lexer_fsm_state_exit(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_COMMENT_END_UNMATCHED;

      case '.':
         tux64_textcompiler_lexer_fsm_state_set_string_entry_identifier(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;

      case '=':
      case ';':
         tux64_textcompiler_lexer_fsm_state_exit(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_MISSING_IDENTIFIER;

      case '}':
         return tux64_textcompiler_lexer_fsm_set_finalize_string_entry(fsm);

      default:
         tux64_textcompiler_lexer_fsm_state_exit(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_MALFORMED_FIELD;
   }

   TUX64_UNREACHABLE;
}

TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DEFINITION(tux64_textcompiler_lexer_fsm_state_string_entry_identifier) {
   struct Tux64TextCompilerLexerFileIteratorNextResult iter_result;
   char c;

   iter_result = tux64_textcompiler_lexer_file_iterator_next(&fsm->iter_file);
   if (iter_result.status == TUX64_TEXTCOMPILER_LEXER_FILE_ITERATOR_NEXT_STATUS_END_OF_FILE) {
      tux64_textcompiler_lexer_fsm_state_exit(fsm);
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_MISSING_SEPARATOR;
   }
   c = iter_result.character;

   if (tux64_string_character_is_whitespace(c)) {
      return tux64_textcompiler_lexer_fsm_set_string_entry_separator(fsm);
   }

   switch (c) {
      case '{':
      case '}':
      case '/':
      case '*':
      case '.':
      case '\"':
         tux64_textcompiler_lexer_fsm_state_exit(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_MALFORMED_FIELD;

      case ';':
         tux64_textcompiler_lexer_fsm_state_exit(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_MISSING_SEPARATOR;

      case '=':
         return tux64_textcompiler_lexer_fsm_set_string_entry_value_initial(fsm);

      default:
         tux64_textcompiler_lexer_identifier_hash_digest(&fsm->state.mem.shared.identifier.hash, c);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;
   }

   TUX64_UNREACHABLE;
}

TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DEFINITION(tux64_textcompiler_lexer_fsm_state_string_entry_wait_separator) {
   struct Tux64TextCompilerLexerFileIteratorNextResult iter_result;
   char c;

   iter_result = tux64_textcompiler_lexer_file_iterator_next(&fsm->iter_file);
   if (iter_result.status == TUX64_TEXTCOMPILER_LEXER_FILE_ITERATOR_NEXT_STATUS_END_OF_FILE) {
      tux64_textcompiler_lexer_fsm_state_exit(fsm);
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_MISSING_SEPARATOR;
   }
   c = iter_result.character;

   if (tux64_string_character_is_whitespace(c)) {
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;
   }

   switch (c) {
      case '=':
         tux64_textcompiler_lexer_fsm_state_set(fsm, tux64_textcompiler_lexer_fsm_state_string_entry_wait_value);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;

      case '{':
      case '}':
      case '/':
      case '*':
      case ';':
      case '.':
      case '\"':
      default:
         tux64_textcompiler_lexer_fsm_state_exit(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_MISSING_SEPARATOR;
   }

   TUX64_UNREACHABLE;
}

TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DEFINITION(tux64_textcompiler_lexer_fsm_state_string_entry_wait_value) {
   struct Tux64TextCompilerLexerFileIteratorNextResult iter_result;
   char c;

   iter_result = tux64_textcompiler_lexer_file_iterator_next(&fsm->iter_file);
   if (iter_result.status == TUX64_TEXTCOMPILER_LEXER_FILE_ITERATOR_NEXT_STATUS_END_OF_FILE) {
      tux64_textcompiler_lexer_fsm_state_exit(fsm);
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_MISSING_VALUE;
   }
   c = iter_result.character;

   if (tux64_string_character_is_whitespace(c)) {
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;
   }

   switch (c) {
      case '{':
      case '}':
      case '/':
      case '*':
      case '.':
         tux64_textcompiler_lexer_fsm_state_exit(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_MALFORMED_FIELD;

      case ';':
         tux64_textcompiler_lexer_fsm_state_exit(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_MISSING_VALUE;

      default:
         return tux64_textcompiler_lexer_fsm_set_string_entry_value(fsm, c);
   }

   TUX64_UNREACHABLE;
}

TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DEFINITION(tux64_textcompiler_lexer_fsm_state_string_entry_value_identifier) {
   struct Tux64TextCompilerLexerFileIteratorNextResult iter_result;
   char c;

   iter_result = tux64_textcompiler_lexer_file_iterator_next(&fsm->iter_file);
   if (iter_result.status == TUX64_TEXTCOMPILER_LEXER_FILE_ITERATOR_NEXT_STATUS_END_OF_FILE) {
      tux64_textcompiler_lexer_fsm_state_exit(fsm);
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_MISSING_NEWLINE;
   }
   c = iter_result.character;

   return tux64_textcompiler_lexer_fsm_digest_string_entry_value_identifier_character(fsm, c);
}

TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DEFINITION(tux64_textcompiler_lexer_fsm_state_string_entry_value_text) {
   struct Tux64TextCompilerLexerText * text;
   struct Tux64TextCompilerLexerFileIteratorNextResult iter_result;
   char c;

   text = fsm->state.mem.shared.value.text.field;

   iter_result = tux64_textcompiler_lexer_file_iterator_next(&fsm->iter_file);
   if (iter_result.status == TUX64_TEXTCOMPILER_LEXER_FILE_ITERATOR_NEXT_STATUS_END_OF_FILE) {
      tux64_textcompiler_lexer_fsm_state_exit(fsm);
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_UNTERMINATED;
   }
   c = iter_result.character;

   if (c == '\"') {
      return tux64_textcompiler_lexer_fsm_set_string_entry_value_text_exit(fsm);
   }

   if (tux64_textcompiler_lexer_fsm_string_entry_character_is_valid(c) == TUX64_BOOLEAN_FALSE) {
      tux64_textcompiler_lexer_fsm_state_exit(fsm);
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_VALUE_BAD_CHARACTER;
   }

   if (text->characters == TUX64_LITERAL_UINT8(255u)) {
      tux64_textcompiler_lexer_fsm_state_exit(fsm);
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_VALUE_TOO_LONG;
   }

   text->characters++;
   return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;
}

TUX64_TEXTCOMPILER_LEXER_FSM_STATE_DEFINITION(tux64_textcompiler_lexer_fsm_state_string_entry_value_wait_newline) {
   struct Tux64TextCompilerLexerFileIteratorNextResult iter_result;
   char c;

   iter_result = tux64_textcompiler_lexer_file_iterator_next(&fsm->iter_file);
   if (iter_result.status == TUX64_TEXTCOMPILER_LEXER_FILE_ITERATOR_NEXT_STATUS_END_OF_FILE) {
      tux64_textcompiler_lexer_fsm_state_exit(fsm);
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_MISSING_NEWLINE;
   }
   c = iter_result.character;

   if (tux64_string_character_is_whitespace(c)) {
      return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;
   }

   switch (c) {
      case ';':
         tux64_textcompiler_lexer_fsm_state_set(fsm, tux64_textcompiler_lexer_fsm_state_string_entry_initial);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK;

      case '{':
      case '}':
      case '/':
      case '*':
      case '.':
      case '\"':
      default:
         tux64_textcompiler_lexer_fsm_state_exit(fsm);
         return TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_MALFORMED_FIELD;
   }

   TUX64_UNREACHABLE;
}

struct Tux64TextCompilerLexerParseResult
tux64_textcompiler_lexer_parse(
   const struct Tux64String * text
) {
   struct Tux64TextCompilerLexerFsm fsm;
   struct Tux64TextCompilerLexerParseResult result;

   switch (tux64_textcompiler_lexer_fsm_initialize(&fsm, text)) {
      case TUX64_TEXTCOMPILER_LEXER_STRING_ARRAY_ALLOC_STATUS_OK:
         break;

      case TUX64_TEXTCOMPILER_LEXER_STRING_ARRAY_ALLOC_STATUS_OUT_OF_MEMORY:
         result.status = TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OUT_OF_MEMORY;
         return result;

      default:
         TUX64_UNREACHABLE;
   }

   tux64_textcompiler_lexer_fsm_state_set(&fsm, tux64_textcompiler_lexer_fsm_state_initial);

   do {
      result.status = tux64_textcompiler_lexer_fsm_state_execute(&fsm);
   } while (!tux64_textcompiler_lexer_fsm_state_has_exited(&fsm));

   switch (result.status) {
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK:
         result.payload.ok = fsm.string_array;
         break;

      default:
         tux64_textcompiler_lexer_string_array_free(&fsm.string_array);
         result.payload.err.location = fsm.iter_file.location_prev;
         break;
   }

   return result;
}

