## Introduction

Welcome to the Tux64 contribution guide!  This will help those looking to get
their own work submitted and merged into Tux64.

## Code Formatting

Unless you have a good reason, generally the following code formatting
guidelines apply.  These mostly apply to C source code, but the same
principles also apply to other files.  If you're confused, take a look at
existing code to see how it's formatted.

 * Indentation should be 3 spaces

 * Lines should aim to be 80 characters wide or less, but if the code becomes less readable trying to adhere to this, ignore this point

 * Variables declarations should be in `snake_case` at the top of each function with only one declaration per line

```
Tux64UInt32
foo(void) {
    Tux64UInt32 a;
    Tux64UInt32 b;

    a = 42;
    b = 21;

    return a + b;
}
```

 * Pointer types should have one space before and after the asterisk

```
const Tux64UInt32 * p;
void * v;
```

 * Functions should be in `snake_case()`, where the return type and arguments are all on seperate lines

```
void
foo(void);

Tux64UInt32
bar(   
    Tux64UInt32 a,
    Tux64UInt32 b,
    Tux64UInt32 c
) {
    return a + b + c;
}
```
 * Functions with no parameters should have `void` in the parenthesis

```
void
foo(void);
```

 * Functions which return `void` should still have a `return` statement at the end, which is done for consistency with every other type of function

```
void
foo(void) {
   return;
}
```

 * Types should be in `PascalCase`

 * Constants and preprocessor macros should be in capitalized `SNAKE_CASE`

 * Avoid primitive types such as `int`, `long`, `float`, as these are of non-standard size.  Use fixed-width integer types in `lib/src/tux64-lib/types.h` such as `Tux64UInt32`, `Tux64Float32`, etc.

 * Brackets for function bodies, `if` statements, etc. should be one the same line as the statement

```
/* do NOT do this */
while(keep_running())
{
   ...
}

/* do this instead */
while(keep_running()) {
   ...
}
```

 * All identifiers should be namespaced according to their package name and file path

```
/* lib/src/tux64-lib/foo.h */
struct Tux64FooContext {
   ...
};

void
tux64_foo_do_thing(struct Tux64FooContext * ctx);

/* boot/src/tux64-boot/foo.h */
struct Tux64BootFooContext {
   ...
};

void
tux64_boot_foo_do_thing(struct Tux64BootFooContext * ctx);
```

 * All `#include` directives belong before all other declarations and definitions

```
#include <tux64/butthole.h>

void dialate(struct Tux64Butthole * butthole);
```

 * GNU C extensions are allowed unwrapped since GCC will be used for building all programs

 * MIPS-specific extensions and inline assembly are allowed unwrapped for N64-specific programs, such as `tux64-boot`.  Otherwise, all code should be either platform-independent or cleanly wrapped in platform-specific headers

 * `goto` is allowed as long as execution only carries forward, but should only be used when necessary

 * Wrap number constants with `TUX64_LITERAL_...()`, which is done to prevent weird issues on some platforms with 64-bit numbers, and also potentially catch constants which overflow at compile-time

 * Comments should be C89-style `/* block comments */`

 * Only use comments to document items in header files and explain _why_ you're doing something, not _what_ you're doing.

```
void
foo(void) {
   Tux64UInt8 idx;

   /* do NOT do this */

   /* initialize $gp to the small data sections pointer */
   __asm__ volatile (
      "addiu $gp,$zero,%lo(_gp)\n"
      "lui $gp,%hi(_gp)\n"
   );

   /* do this instead! I can read your code, but not your mind! */

   /* initialize $gp to the small data sections pointer first so that the */
   /* compiler can rely on small data sections for the rest of our code. */
   __asm__ volatile (
      "addiu $gp,$zero,%lo(_gp)\n"
      "lui $gp,%hi(_gp)\n"
   );

   /* even better if the code is self-documenting, so comments are redundant */
   tux64_boot_initialize_small_data_sections();

   return;
}
```

 * Avoid clever code.  Read through `tux64-lib` and use helper functions that exist in there, or write your own helpers so the code self-documents.

```
#include <tux64/tux64.h>
#include <tux64/bitwise.h>

#define BIT_A (1u << 2u)

void
foo(Tux64UInt32 bar) {
   /* do NOT do this */
   bar &= ~TUX64_LITERAL_UINT32(BIT_A);

   /* do this instead! */
   bar = tux64_bitwise_flags_clear_uint32(bar, TUX64_LITERAL_UINT32(BIT_A));

   /* and for the love of fuck, don't do something like this... */
   for (Tux64UInt32 i = 0; i < 8; ++i) {
      for (Tux64UInt32 j = 0; j < i / 2; j += 2) {
         (void)printf("%u%u", j - i, i - j);
      }
   }

   /* if you're concerned about optimization, we LTO all of this anyways, so there's zero cost to performance */
   return;
}
```

 * Use `Tux64String` slices when handling strings.  C-style null-terminated `const char *` strings should only ever be touched when interfacing with external APIs, such as `printf()`

 * Cast unused variables and return values to `void` to properly discard them, which avoids raising warnings

```
Tux64UInt32 unusued;

/* doing this will tell the compiler that we're intentionally discarding the */
/* variables, thus we won't get a warning, even on -Wall */
(void)unused;
```

As a rule of thumb, if your code is noticeably different looking to all other
code around it, there is a problem.  Don't submit code with wonky formatting
that blatantly sticks out from everything around it.

## Pull Requests

When submitting code, it is expected you are able to explain every line of code
you submit and _why_ you want your code accepted.  If it's not either obvious
from context what your code does or you provide no information, your pull
request will likely be rejected.

It is also expected to have a clean commit history.  Commits should be formatted
so that each commit does "one thing" and only does what it says it does.  You
should also make sure each commit message includes the subsystem it affects.

For example, an example of a **bad** commit history looks like this:

```
fix bug
fix bad formtating
(no commit message)
aaaaaaaaa
fix regression and add new helper function
```

An example of a **good** commit history looks like this:

```
tux64-boot: fix incorrect virtual to physical address translation in tux64_boot_stage1_video_swap_buffers()
tux64-lib: fix typo in field for Tux64ArgumentsParseContext
tux64-lib: create new API for parsing arguments from a single-line string
tux64-boot: fix regression in stage-0 after incorrectly reallocating registers
```

Be prepared for changes to be requested and for you to explain yourself.  This
doesn't mean you're a bad programmer and should bugger off necessarily, as you
could get your code submitted after a few simple changes, but you shouldn't
expect your code to always be graciously accepted immediately.  As of writing
(October 2025), I'm in my Senior year of a CS and Math major and I'm also taking
graduate CS classes, and I also have other projects I work on.  If I take some
time to review your pull request, please be patient.  I'm not deliberately
ignoring you, I'm probably just busy or burnt out on life.

## Issues

TODO

## AI Generated Code

TODO

## AI Generated Slop

TODO