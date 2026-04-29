/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64-lib/platform/mips/n64/kernel.c - Implementations for N64     */
/*    kernel image functions.                                                 */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/platform/mips/n64/kernel.h"

#include "tux64/elf.h"
#include "tux64/memory.h"
#include "tux64/endian.h"

#define TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT \
   TUX64_ENDIAN_FORMAT_BIG

/* since 32-bit and 64-bit ELF headers have different offsets/types, we have  */
/* to parse them seperately.  to avoid code duplication, we use a vtable to   */
/* handle reading types at different offsets and then upcast for 32-bit.      */

typedef Tux64UInt16 (*Tux64PlatformMipsN64KernelElfVTableFunctionEEhsize)(
   const union Tux64ElfHeader * elf_header
);
typedef Tux64UInt16 (*Tux64PlatformMipsN64KernelElfVTableFunctionEPhentsize)(
   const union Tux64ElfHeader * elf_header
);
typedef Tux64UInt64 (*Tux64PlatformMipsN64KernelElfVTableFunctionEEntry)(
   const union Tux64ElfHeader * elf_header
);
typedef Tux64UInt64 (*Tux64PlatformMipsN64KernelElfVTableFunctionEPhoff)(
   const union Tux64ElfHeader * elf_header
);
typedef Tux64UInt16 (*Tux64PlatformMipsN64KernelElfVTableFunctionEPhnum)(
   const union Tux64ElfHeader * elf_header
);
typedef Tux64UInt64 (*Tux64PlatformMipsN64KernelElfVTableFunctionPOffset)(
   const union Tux64ElfProgramHeader * program_header
);
typedef Tux64UInt64 (*Tux64PlatformMipsN64KernelElfVTableFunctionPFilesz)(
   const union Tux64ElfProgramHeader * program_header
);
typedef Tux64UInt64 (*Tux64PlatformMipsN64KernelElfVTableFunctionPMemsz)(
   const union Tux64ElfProgramHeader * program_header
);
typedef Tux64UInt64 (*Tux64PlatformMipsN64KernelElfVTableFunctionPVaddr)(
   const union Tux64ElfProgramHeader * program_header
);
typedef Tux64UInt64 (*Tux64PlatformMipsN64KernelElfVTableFunctionPAlign)(
   const union Tux64ElfProgramHeader * program_header
);

struct Tux64PlatformMipsN64KernelElfVTableSize {
   Tux64UInt16 elf_header;
   Tux64UInt16 program_header;
};

struct Tux64PlatformMipsN64KernelElfVTable {
   Tux64PlatformMipsN64KernelElfVTableFunctionEEhsize    e_ehsize;
   Tux64PlatformMipsN64KernelElfVTableFunctionEPhentsize e_phentsize;
   Tux64PlatformMipsN64KernelElfVTableFunctionEEntry     e_entry;
   Tux64PlatformMipsN64KernelElfVTableFunctionEPhoff     e_phoff;
   Tux64PlatformMipsN64KernelElfVTableFunctionEPhnum     e_phnum;
   Tux64PlatformMipsN64KernelElfVTableFunctionPOffset    p_offset;
   Tux64PlatformMipsN64KernelElfVTableFunctionPFilesz    p_filesz;
   Tux64PlatformMipsN64KernelElfVTableFunctionPMemsz     p_memsz;
   Tux64PlatformMipsN64KernelElfVTableFunctionPVaddr     p_vaddr;
   Tux64PlatformMipsN64KernelElfVTableFunctionPAlign     p_align;
   struct Tux64PlatformMipsN64KernelElfVTableSize        size;
};

static Tux64UInt64 tux64_platform_mips_n64_kernel_elf_upcast_uint32(
   Tux64UInt32 address
) {
   /* since we save endian conversion for later, this needs to be done to */
   /* avoid having the hi and lo 32-bit words backwards. */
   if (TUX64_ENDIAN_FORMAT_NATIVE != TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT) {
      return (Tux64UInt64)address << TUX64_LITERAL_UINT8(32u);
   }

   return (Tux64UInt64)address;
}

static Tux64UInt16 tux64_platform_mips_n64_kernel_elf_vtable_32_function_e_ehsize(
   const union Tux64ElfHeader * elf_header
) {
   return elf_header->elf32.e_ehsize;
}

static Tux64UInt16 tux64_platform_mips_n64_kernel_elf_vtable_32_function_e_phentsize(
   const union Tux64ElfHeader * elf_header
) {
   return elf_header->elf32.e_phentsize;
}

static Tux64UInt64 tux64_platform_mips_n64_kernel_elf_vtable_32_function_e_entry(
   const union Tux64ElfHeader * elf_header
) {
   return tux64_platform_mips_n64_kernel_elf_upcast_uint32(elf_header->elf32.e_entry);
}

static Tux64UInt64 tux64_platform_mips_n64_kernel_elf_vtable_32_function_e_phoff(
   const union Tux64ElfHeader * elf_header
) {
   return tux64_platform_mips_n64_kernel_elf_upcast_uint32(elf_header->elf32.e_phoff);
}

static Tux64UInt16 tux64_platform_mips_n64_kernel_elf_vtable_32_function_e_phnum(
   const union Tux64ElfHeader * elf_header
) {
   return elf_header->elf32.e_phnum;
}

static Tux64UInt64 tux64_platform_mips_n64_kernel_elf_vtable_32_function_p_offset(
   const union Tux64ElfProgramHeader * program_header
) {
   return tux64_platform_mips_n64_kernel_elf_upcast_uint32(program_header->elf32.p_offset);
}

static Tux64UInt64 tux64_platform_mips_n64_kernel_elf_vtable_32_function_p_filesz(
   const union Tux64ElfProgramHeader * program_header
) {
   return tux64_platform_mips_n64_kernel_elf_upcast_uint32(program_header->elf32.p_filesz);
}

static Tux64UInt64 tux64_platform_mips_n64_kernel_elf_vtable_32_function_p_memsz(
   const union Tux64ElfProgramHeader * program_header
) {
   return tux64_platform_mips_n64_kernel_elf_upcast_uint32(program_header->elf32.p_memsz);
}

static Tux64UInt64 tux64_platform_mips_n64_kernel_elf_vtable_32_function_p_vaddr(
   const union Tux64ElfProgramHeader * program_header
) {
   return tux64_platform_mips_n64_kernel_elf_upcast_uint32(program_header->elf32.p_vaddr);
}

static Tux64UInt64 tux64_platform_mips_n64_kernel_elf_vtable_32_function_p_align(
   const union Tux64ElfProgramHeader * program_header
) {
   return tux64_platform_mips_n64_kernel_elf_upcast_uint32(program_header->elf32.p_align);
}

static Tux64UInt16 tux64_platform_mips_n64_kernel_elf_vtable_64_function_e_ehsize(
   const union Tux64ElfHeader * elf_header
) {
   return elf_header->elf64.e_ehsize;
}

static Tux64UInt16 tux64_platform_mips_n64_kernel_elf_vtable_64_function_e_phentsize(
   const union Tux64ElfHeader * elf_header
) {
   return elf_header->elf64.e_phentsize;
}

static Tux64UInt64 tux64_platform_mips_n64_kernel_elf_vtable_64_function_e_entry(
   const union Tux64ElfHeader * elf_header
) {
   return elf_header->elf64.e_entry;
}

static Tux64UInt64 tux64_platform_mips_n64_kernel_elf_vtable_64_function_e_phoff(
   const union Tux64ElfHeader * elf_header
) {
   return elf_header->elf64.e_phoff;
}

static Tux64UInt16 tux64_platform_mips_n64_kernel_elf_vtable_64_function_e_phnum(
   const union Tux64ElfHeader * elf_header
) {
   return elf_header->elf64.e_phnum;
}

static Tux64UInt64 tux64_platform_mips_n64_kernel_elf_vtable_64_function_p_offset(
   const union Tux64ElfProgramHeader * program_header
) {
   return program_header->elf64.p_offset;
}

static Tux64UInt64 tux64_platform_mips_n64_kernel_elf_vtable_64_function_p_filesz(
   const union Tux64ElfProgramHeader * program_header
) {
   return program_header->elf64.p_filesz;
}

static Tux64UInt64 tux64_platform_mips_n64_kernel_elf_vtable_64_function_p_memsz(
   const union Tux64ElfProgramHeader * program_header
) {
   return program_header->elf64.p_memsz;
}

static Tux64UInt64 tux64_platform_mips_n64_kernel_elf_vtable_64_function_p_vaddr(
   const union Tux64ElfProgramHeader * program_header
) {
   return program_header->elf64.p_vaddr;
}

static Tux64UInt64 tux64_platform_mips_n64_kernel_elf_vtable_64_function_p_align(
   const union Tux64ElfProgramHeader * program_header
) {
   return program_header->elf64.p_align;
}

static const struct Tux64PlatformMipsN64KernelElfVTable
tux64_platform_mips_n64_kernel_elf_vtable_32 = {
   tux64_platform_mips_n64_kernel_elf_vtable_32_function_e_ehsize,
   tux64_platform_mips_n64_kernel_elf_vtable_32_function_e_phentsize,
   tux64_platform_mips_n64_kernel_elf_vtable_32_function_e_entry,
   tux64_platform_mips_n64_kernel_elf_vtable_32_function_e_phoff,
   tux64_platform_mips_n64_kernel_elf_vtable_32_function_e_phnum,
   tux64_platform_mips_n64_kernel_elf_vtable_32_function_p_offset,
   tux64_platform_mips_n64_kernel_elf_vtable_32_function_p_filesz,
   tux64_platform_mips_n64_kernel_elf_vtable_32_function_p_memsz,
   tux64_platform_mips_n64_kernel_elf_vtable_32_function_p_vaddr,
   tux64_platform_mips_n64_kernel_elf_vtable_32_function_p_align,
   {
      TUX64_LITERAL_UINT16(sizeof(struct Tux64ElfHeader32)),
      TUX64_LITERAL_UINT16(sizeof(struct Tux64ElfProgramHeader32)),
   }
};

static const struct Tux64PlatformMipsN64KernelElfVTable
tux64_platform_mips_n64_kernel_elf_vtable_64 = {
   tux64_platform_mips_n64_kernel_elf_vtable_64_function_e_ehsize,
   tux64_platform_mips_n64_kernel_elf_vtable_64_function_e_phentsize,
   tux64_platform_mips_n64_kernel_elf_vtable_64_function_e_entry,
   tux64_platform_mips_n64_kernel_elf_vtable_64_function_e_phoff,
   tux64_platform_mips_n64_kernel_elf_vtable_64_function_e_phnum,
   tux64_platform_mips_n64_kernel_elf_vtable_64_function_p_offset,
   tux64_platform_mips_n64_kernel_elf_vtable_64_function_p_filesz,
   tux64_platform_mips_n64_kernel_elf_vtable_64_function_p_memsz,
   tux64_platform_mips_n64_kernel_elf_vtable_64_function_p_vaddr,
   tux64_platform_mips_n64_kernel_elf_vtable_64_function_p_align,
   {
      TUX64_LITERAL_UINT16(sizeof(struct Tux64ElfHeader64)),
      TUX64_LITERAL_UINT16(sizeof(struct Tux64ElfProgramHeader64)),
   }
};

static Tux64UInt16
tux64_platform_mips_n64_kernel_elf_read_uint16(
   Tux64UInt16 value
) {
   return tux64_endian_convert_uint16(value, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);
}

static Tux64UInt32
tux64_platform_mips_n64_kernel_elf_read_uint32(
   Tux64UInt32 value
) {
   return tux64_endian_convert_uint32(value, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);
}

static Tux64UInt64
tux64_platform_mips_n64_kernel_elf_read_uint64(
   Tux64UInt64 value
) {
   return tux64_endian_convert_uint64(value, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);
}

static Tux64UInt16
tux64_platform_mips_n64_kernel_elf_header_read_e_type(
   const union Tux64ElfHeader * elf_header
) {
   return tux64_platform_mips_n64_kernel_elf_read_uint16(elf_header->elf32.e_type);
}

static Tux64UInt16
tux64_platform_mips_n64_kernel_elf_header_read_e_machine(
   const union Tux64ElfHeader * elf_header
) {
   return tux64_platform_mips_n64_kernel_elf_read_uint16(elf_header->elf32.e_machine);
}

static Tux64UInt32
tux64_platform_mips_n64_kernel_elf_header_read_e_version(
   const union Tux64ElfHeader * elf_header
) {
   return tux64_platform_mips_n64_kernel_elf_read_uint32(elf_header->elf32.e_version);
}

static Tux64UInt16
tux64_platform_mips_n64_kernel_elf_header_read_e_ehsize(
   const union Tux64ElfHeader * elf_header,
   const struct Tux64PlatformMipsN64KernelElfVTable * vtable
) {
   return tux64_platform_mips_n64_kernel_elf_read_uint16(vtable->e_ehsize(elf_header));
}

static Tux64UInt16
tux64_platform_mips_n64_kernel_elf_header_read_e_phentsize(
   const union Tux64ElfHeader * elf_header,
   const struct Tux64PlatformMipsN64KernelElfVTable * vtable
) {
   return tux64_platform_mips_n64_kernel_elf_read_uint16(vtable->e_phentsize(elf_header));
}

static Tux64UInt64
tux64_platform_mips_n64_kernel_elf_header_read_e_entry(
   const union Tux64ElfHeader * elf_header,
   const struct Tux64PlatformMipsN64KernelElfVTable * vtable
) {
   return tux64_platform_mips_n64_kernel_elf_read_uint64(vtable->e_entry(elf_header));
}

static Tux64UInt64
tux64_platform_mips_n64_kernel_elf_header_read_e_phoff(
   const union Tux64ElfHeader * elf_header,
   const struct Tux64PlatformMipsN64KernelElfVTable * vtable
) {
   return tux64_platform_mips_n64_kernel_elf_read_uint64(vtable->e_phoff(elf_header));
}

static Tux64UInt16
tux64_platform_mips_n64_kernel_elf_header_read_e_phnum(
   const union Tux64ElfHeader * elf_header,
   const struct Tux64PlatformMipsN64KernelElfVTable * vtable
) {
   return tux64_platform_mips_n64_kernel_elf_read_uint16(vtable->e_phnum(elf_header));
}

static Tux64UInt32
tux64_platform_mips_n64_kernel_program_header_read_p_type(
   const union Tux64ElfProgramHeader * program_header
) {
   return tux64_platform_mips_n64_kernel_elf_read_uint32(program_header->elf32.p_type);
}

static Tux64UInt64
tux64_platform_mips_n64_kernel_program_header_read_p_offset(
   const union Tux64ElfProgramHeader * program_header,
   const struct Tux64PlatformMipsN64KernelElfVTable * vtable
) {
   return tux64_platform_mips_n64_kernel_elf_read_uint64(vtable->p_offset(program_header));
}

static Tux64UInt64
tux64_platform_mips_n64_kernel_program_header_read_p_filesz(
   const union Tux64ElfProgramHeader * program_header,
   const struct Tux64PlatformMipsN64KernelElfVTable * vtable
) {
   return tux64_platform_mips_n64_kernel_elf_read_uint64(vtable->p_filesz(program_header));
}

static Tux64UInt64
tux64_platform_mips_n64_kernel_program_header_read_p_memsz(
   const union Tux64ElfProgramHeader * program_header,
   const struct Tux64PlatformMipsN64KernelElfVTable * vtable
) {
   return tux64_platform_mips_n64_kernel_elf_read_uint64(vtable->p_memsz(program_header));
}

static Tux64UInt64
tux64_platform_mips_n64_kernel_program_header_read_p_vaddr(
   const union Tux64ElfProgramHeader * program_header,
   const struct Tux64PlatformMipsN64KernelElfVTable * vtable
) {
   return tux64_platform_mips_n64_kernel_elf_read_uint64(vtable->p_vaddr(program_header));
}

static Tux64UInt64
tux64_platform_mips_n64_kernel_program_header_read_p_align(
   const union Tux64ElfProgramHeader * program_header,
   const struct Tux64PlatformMipsN64KernelElfVTable * vtable
) {
   return tux64_platform_mips_n64_kernel_elf_read_uint64(vtable->p_align(program_header));
}

static Tux64Boolean
tux64_platform_mips_n64_kernel_segment_alignment_verify(
   Tux64UInt64 offset,
   Tux64UInt64 addr_load,
   Tux64UInt64 alignment
) {
   Tux64UInt64 alignment_mask;

   /* if the alignment is 0 or 1, then we don't care about alignment. */
   if (alignment <= TUX64_LITERAL_UINT64(1u)) {
      return TUX64_BOOLEAN_TRUE;
   }

   alignment_mask = alignment - TUX64_LITERAL_UINT64(1u);

   /* verify the alignment is a power of two */
   if ((alignment & alignment_mask) != TUX64_LITERAL_UINT64(0u)) {
      return TUX64_BOOLEAN_FALSE;
   }

   /* verify the load address is at the aligned offset as specified by the */
   /* ELF header spec. */
   if (((addr_load - offset) & alignment_mask) != TUX64_LITERAL_UINT64(0u)) {
      return TUX64_BOOLEAN_FALSE;
   }

   return TUX64_BOOLEAN_TRUE;
}

static Tux64Boolean
tux64_platform_mips_n64_kernel_program_headers_present(
   Tux64UInt32 elf_bytes,
   Tux64UInt64 phoff,
   Tux64UInt16 phnum,
   Tux64UInt16 phentsize
) {
   if (phoff > (Tux64UInt64)elf_bytes) {
      return TUX64_BOOLEAN_FALSE;
   }
   elf_bytes -= phoff;

   if (phnum * phentsize > (Tux64UInt64)elf_bytes) {
      return TUX64_BOOLEAN_FALSE;
   }

   return TUX64_BOOLEAN_TRUE;
}

static struct Tux64PlatformMipsN64KernelParseResult
tux64_platform_mips_n64_kernel_parse_main_segment(
   Tux64UInt64 offset,
   Tux64UInt64 filesz,
   Tux64UInt64 memsz,
   Tux64UInt64 vaddr,
   Tux64UInt64 entry
) {
   struct Tux64PlatformMipsN64KernelParseResult result;
   struct Tux64PlatformMipsN64Kernel * kernel;

   /* since we're targetting a 32-bit address space, we now have to make sure */
   /* the image can bit in 32 bits.  note that we ignore the N64 address      */
   /* space.  this is because the console truncates those anyways, so we are  */
   /* okay to allow this.                                                     */
   if (filesz > TUX64_LITERAL_UINT64(TUX64_UINT32_MAX))  goto hell;
   if (memsz > TUX64_LITERAL_UINT64(TUX64_UINT32_MAX))   goto hell;

   /* just a few more sanity checks and we're done! */
   if (memsz < filesz)           goto hell;
   if (entry < vaddr)            goto hell;
   if (entry - vaddr > filesz)   goto hell;

   /* woot woot! */
   kernel = &result.payload.ok;
   kernel->image.offset = (Tux64UInt32)offset;
   kernel->image.bytes  = (Tux64UInt32)filesz;
   kernel->memory       = (Tux64UInt32)memsz;
   kernel->addr_load    = (Tux64UInt32)vaddr;
   kernel->addr_entry   = (Tux64UInt32)entry;

   result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_OK;
   return result;

hell:
   result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
   return result;
}

static struct Tux64PlatformMipsN64KernelParseResult
tux64_platform_mips_n64_kernel_parse_program_headers(
   const Tux64UInt8 * elf_data,
   Tux64UInt32 elf_bytes,
   const struct Tux64PlatformMipsN64KernelElfVTable * vtable,
   Tux64UInt64 entry,
   Tux64UInt64 phoff,
   Tux64UInt16 phnum
) {
   struct Tux64PlatformMipsN64KernelParseResult result;
   Tux64Boolean main_segment_found;
   const Tux64UInt8 * iter_program_headers;
   union Tux64ElfProgramHeader program_header;
   Tux64UInt32 type;
   Tux64UInt64 offset;
   Tux64UInt64 filesz;
   Tux64UInt64 memsz;
   Tux64UInt64 vaddr;
   Tux64UInt64 align;

   main_segment_found = TUX64_BOOLEAN_FALSE;
   iter_program_headers = &elf_data[phoff];

   while (phnum != TUX64_LITERAL_UINT16(0u)) {
      tux64_memory_copy(
         &program_header,
         iter_program_headers,
         vtable->size.program_header
      );
      iter_program_headers += vtable->size.program_header;
      phnum--;

      type = tux64_platform_mips_n64_kernel_program_header_read_p_type(&program_header);

      /* we ignore all segments except for loadable segments */
      if (type != TUX64_ELF_PROGRAM_HEADER_TYPE_LOAD) {
         continue;
      }

      /* if we already found the main segment, bail out */
      if (main_segment_found == TUX64_BOOLEAN_TRUE) {
         result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_MAIN_SEGMENT_DUPLICATE;
         return result;
      }
      main_segment_found = TUX64_BOOLEAN_TRUE;

      offset   = tux64_platform_mips_n64_kernel_program_header_read_p_offset(&program_header, vtable);
      filesz   = tux64_platform_mips_n64_kernel_program_header_read_p_filesz(&program_header, vtable);
      memsz    = tux64_platform_mips_n64_kernel_program_header_read_p_memsz(&program_header, vtable);
      vaddr    = tux64_platform_mips_n64_kernel_program_header_read_p_vaddr(&program_header, vtable);
      align    = tux64_platform_mips_n64_kernel_program_header_read_p_align(&program_header, vtable);
   }

   /* now make sure we found the main segment */
   if (main_segment_found == TUX64_BOOLEAN_FALSE) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_MAIN_SEGMENT_MISSING;
      return result;
   }

   /* make sure the segment is contained in the file */
   if (offset + filesz > (Tux64UInt64)elf_bytes) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
      return result;
   }

   /* verify the alignment is correct */
   if (tux64_platform_mips_n64_kernel_segment_alignment_verify(offset, vaddr, align) == TUX64_BOOLEAN_FALSE) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
      return result;
   }

   /* and now proceed to the final verification */
   return tux64_platform_mips_n64_kernel_parse_main_segment(
      offset,
      filesz,
      memsz,
      vaddr,
      entry
   );
}

static struct Tux64PlatformMipsN64KernelParseResult
tux64_platform_mips_n64_kernel_parse_elf_header(
   const Tux64UInt8 * elf_data,
   Tux64UInt32 elf_bytes,
   const struct Tux64PlatformMipsN64KernelElfVTable * vtable
) {
   struct Tux64PlatformMipsN64KernelParseResult result;
   union Tux64ElfHeader elf_header;
   Tux64UInt16 type;
   Tux64UInt16 machine;
   Tux64UInt32 version;
   Tux64UInt16 ehsize;
   Tux64UInt16 phentsize;
   Tux64UInt64 entry;
   Tux64UInt64 phoff;
   Tux64UInt16 phnum;

   /* load the elf header onto the stack to avoid alignment issues with a */
   /* pointer cast. */
   if (elf_bytes < vtable->size.elf_header) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
      return result;
   }
   tux64_memory_copy(
      &elf_header,
      &elf_data[0u],
      vtable->size.elf_header
   );

   type     = tux64_platform_mips_n64_kernel_elf_header_read_e_type(&elf_header);
   machine  = tux64_platform_mips_n64_kernel_elf_header_read_e_machine(&elf_header);
   version  = tux64_platform_mips_n64_kernel_elf_header_read_e_version(&elf_header);

   /* verify the ELF is an executable */
   if (type != TUX64_ELF_HEADER_TYPE_EXEC) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_INVALID_TYPE;
      result.payload.invalid_type.type = type;
      return result;
   }

   /* verify the ELF is for MIPS */
   if (machine != TUX64_ELF_HEADER_MACHINE_MIPS) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_INVALID_MACHINE;
      result.payload.invalid_machine.machine = machine;
      return result;
   }

   /* verify the ELF is the correct version (again) */
   if (version != TUX64_ELF_HEADER_VERSION_1) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_BAD_VERSION;
      result.payload.bad_version.version = version;
      return result;
   }

   ehsize      = tux64_platform_mips_n64_kernel_elf_header_read_e_ehsize(&elf_header, vtable);
   phentsize   = tux64_platform_mips_n64_kernel_elf_header_read_e_phentsize(&elf_header, vtable);

   /* verify the given elf and program header sizes are correct */
   if (ehsize != vtable->size.elf_header) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
      return result;
   }
   if (phentsize != vtable->size.program_header) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
      return result;
   }

   entry = tux64_platform_mips_n64_kernel_elf_header_read_e_entry(&elf_header, vtable);
   phoff = tux64_platform_mips_n64_kernel_elf_header_read_e_phoff(&elf_header, vtable);
   phnum = tux64_platform_mips_n64_kernel_elf_header_read_e_phnum(&elf_header, vtable);

   /* verify we have the memory present for all program headers */
   if (tux64_platform_mips_n64_kernel_program_headers_present(
      elf_bytes,
      phoff,
      phnum,
      TUX64_LITERAL_UINT8(sizeof(struct Tux64ElfProgramHeader64))
   ) == TUX64_BOOLEAN_FALSE) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
      return result;
   }

   /* now continue on to parsing the program headers */
   return tux64_platform_mips_n64_kernel_parse_program_headers(
      elf_data,
      elf_bytes,
      vtable,
      entry,
      phoff,
      phnum
   );
}

struct Tux64PlatformMipsN64KernelParseResult
tux64_platform_mips_n64_kernel_parse(
   const Tux64UInt8 * elf_data,
   Tux64UInt32 elf_bytes
) {
   struct Tux64PlatformMipsN64KernelParseResult result;
   Tux64UInt8 elf_header_ident [TUX64_ELF_HEADER_IDENT_BYTES];
   const struct Tux64PlatformMipsN64KernelElfVTable * vtable;

   /* first read in the "e_ident" field */
   if (elf_bytes < TUX64_LITERAL_UINT32(TUX64_ELF_HEADER_IDENT_BYTES)) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
      return result;
   }
   tux64_memory_copy(
      elf_header_ident,
      &elf_data[0u],
      TUX64_LITERAL_UINT32(TUX64_ELF_HEADER_IDENT_BYTES)
   );

   /* verify the 'magic' is present */
   if (tux64_memory_compare_with_equal_lengths(
      &elf_header_ident[TUX64_ELF_HEADER_IDENT_INDEX_MAGIC],
      TUX64_ELF_HEADER_IDENT_MAGIC,
      TUX64_STRING_CHARACTERS(TUX64_ELF_HEADER_IDENT_MAGIC)
   ) == TUX64_BOOLEAN_FALSE) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
      return result;
   }

   /* verify the header is big-endian */
   if (elf_data[TUX64_ELF_HEADER_IDENT_INDEX_DATA] != TUX64_ELF_HEADER_IDENT_DATA_ENDIAN_BIG) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_INVALID_ENDIANESS;
      return result;
   }

   /* verify the ELF is the correct version */
   if (elf_data[TUX64_ELF_HEADER_IDENT_INDEX_VERSION] != TUX64_ELF_HEADER_IDENT_VERSION_1) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_BAD_VERSION;
      result.payload.bad_version.version = elf_data[TUX64_ELF_HEADER_IDENT_INDEX_VERSION];
      return result;
   }

   /* figure out if the ELF is 32-bit or 64-bit and parse the ELF file from there */
   switch (elf_data[TUX64_ELF_HEADER_IDENT_INDEX_CLASS]) {
      case TUX64_ELF_HEADER_IDENT_CLASS_32:
         vtable = &tux64_platform_mips_n64_kernel_elf_vtable_32;
         break;

      case TUX64_ELF_HEADER_IDENT_CLASS_64:
         vtable = &tux64_platform_mips_n64_kernel_elf_vtable_64;
         break;

      default:
         result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
         return result;
   }

   return tux64_platform_mips_n64_kernel_parse_elf_header(elf_data, elf_bytes, vtable);
}

