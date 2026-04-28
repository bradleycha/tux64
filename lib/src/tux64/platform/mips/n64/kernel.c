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

static struct Tux64PlatformMipsN64KernelParseResult
tux64_platform_mips_n64_kernel_parse_elf_header_common(
   const union Tux64ElfHeader * elf_header
) {
   struct Tux64PlatformMipsN64KernelParseResult result;
   Tux64UInt16 type;
   Tux64UInt16 machine;
   Tux64UInt32 version;

   type = tux64_endian_convert_uint16(elf_header->elf32.e_type, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);
   machine = tux64_endian_convert_uint16(elf_header->elf32.e_machine, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);
   version = tux64_endian_convert_uint32(elf_header->elf32.e_version, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);

   /* here, we can take a pointer from either a 32-bit or 64-bit ELF header */
   /* because for everything in this function, the definitions match. */

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

   result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_OK;
   return result;
}

static Tux64Boolean
tux64_platform_mips_n64_kernel_program_headers_present(
   Tux64UInt32 elf_bytes,
   Tux64UInt32 phoff,
   Tux64UInt16 phnum,
   Tux64UInt16 phsize
) {
   if (phoff > elf_bytes) {
      return TUX64_BOOLEAN_FALSE;
   }
   elf_bytes -= phoff;

   if (phnum * phsize > elf_bytes) {
      return TUX64_BOOLEAN_FALSE;
   }

   return TUX64_BOOLEAN_TRUE;
}

static struct Tux64PlatformMipsN64KernelParseResult
tux64_platform_mips_n64_kernel_parse_program_headers_32(
   const Tux64UInt8 * elf_data,
   Tux64UInt32 elf_bytes,
   Tux64UInt32 addr_entry,
   Tux64UInt32 phoff,
   Tux64UInt16 phnum
) {
   struct Tux64PlatformMipsN64KernelParseResult result;
   Tux64Boolean main_segment_found;
   const Tux64UInt8 * iter_program_headers;
   struct Tux64ElfProgramHeader32 program_header;
   Tux64UInt32 segment_type;
   Tux64UInt32 segment_file_offset;
   Tux64UInt32 image_bytes;
   Tux64UInt32 image_memory;
   Tux64UInt32 addr_load;
   Tux64UInt32 alignment;

   main_segment_found = TUX64_BOOLEAN_FALSE;
   iter_program_headers = &elf_data[phoff];

   /* iterate through all program headers to locate the main load segment */
   while (phnum != TUX64_LITERAL_UINT16(0u)) {
      tux64_memory_copy(
         &program_header,
         iter_program_headers,
         TUX64_LITERAL_UINT32(sizeof(program_header))
      );
      iter_program_headers += sizeof(program_header);
      phnum--;

      segment_type = tux64_endian_convert_uint32(program_header.p_type, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);

      /* we only care if this is a load segment */
      if (segment_type != TUX64_ELF_PROGRAM_HEADER_TYPE_LOAD) {
         continue;
      }

      /* make sure only the first load segment is present */
      if (main_segment_found == TUX64_BOOLEAN_TRUE) {
         result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_MAIN_SEGMENT_DUPLICATE;
         return result;
      }
      main_segment_found = TUX64_BOOLEAN_TRUE;

      /* load the relevant information about the segment */
      segment_file_offset = tux64_endian_convert_uint32(program_header.p_offset, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);
      image_bytes = tux64_endian_convert_uint32(program_header.p_filesz, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);
      image_memory = tux64_endian_convert_uint32(program_header.p_memsz, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);
      addr_load = tux64_endian_convert_uint32(program_header.p_vaddr, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);
      alignment = tux64_endian_convert_uint32(program_header.p_align, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);

      /* verify the segment data offset/length */
      if (segment_file_offset + image_bytes > elf_bytes) {
         result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
         return result;
      }
   }

   /* done like this so we can both detect duplicate and missing main segments. */
   if (main_segment_found == TUX64_BOOLEAN_FALSE) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_MAIN_SEGMENT_MISSING;
      return result;
   }

   /* great success! */
   result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_OK;
   result.payload.ok.image.offset = segment_file_offset;
   result.payload.ok.image.bytes = image_bytes;
   result.payload.ok.memory = image_memory;
   result.payload.ok.addr_load = addr_load;
   result.payload.ok.addr_entry = addr_entry;
   result.payload.ok.alignment = alignment;
   return result;
}

static struct Tux64PlatformMipsN64KernelParseResult
tux64_platform_mips_n64_kernel_parse_32(
   const Tux64UInt8 * elf_data,
   Tux64UInt32 elf_bytes
) {
   struct Tux64PlatformMipsN64KernelParseResult result;
   struct Tux64ElfHeader32 elf_header;
   Tux64UInt16 ehsize;
   Tux64UInt16 phsize;
   Tux64UInt32 addr_entry;
   Tux64UInt32 phoff;
   Tux64UInt16 phnum;

   /* load the entire ELF header.  this needs to be copied into the stack due */
   /* to byte alignment requirements. */
   if (elf_bytes < TUX64_LITERAL_UINT32(sizeof(elf_header))) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
      return result;
   }
   tux64_memory_copy(
      &elf_header,
      &elf_data[0u],
      TUX64_LITERAL_UINT32(sizeof(elf_header))
   );

   /* perform common header checks */
   result = tux64_platform_mips_n64_kernel_parse_elf_header_common((const union Tux64ElfHeader *)&elf_header);
   if (result.status != TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_OK) {
      return result;
   }

   ehsize = tux64_endian_convert_uint16(elf_header.e_ehsize, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);
   phsize = tux64_endian_convert_uint16(elf_header.e_phentsize, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);

   /* verify the provided ELF header size and program header size is correct */
   if (ehsize != TUX64_LITERAL_UINT16(sizeof(elf_header))) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
      return result;
   }
   if (phsize != TUX64_LITERAL_UINT16(sizeof(struct Tux64ElfProgramHeader32))) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
      return result;
   }

   /* get the entrypoint, program headers count, and program headers offset */
   addr_entry = tux64_endian_convert_uint32(elf_header.e_entry, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);
   phoff = tux64_endian_convert_uint32(elf_header.e_phoff, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);
   phnum = tux64_endian_convert_uint16(elf_header.e_phnum, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);

   /* verify the program headers don't run off the end of the file */
   if (tux64_platform_mips_n64_kernel_program_headers_present(
      elf_bytes,
      phoff,
      phnum,
      TUX64_LITERAL_UINT8(sizeof(struct Tux64ElfProgramHeader32))
   ) == TUX64_BOOLEAN_FALSE) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
      return result;
   }

   /* move on to parsing the program headers */
   return tux64_platform_mips_n64_kernel_parse_program_headers_32(
      elf_data,
      elf_bytes,
      addr_entry,
      phoff,
      phnum
   );
}

/* everything below is very similar to 32-bit, but unfortunately has to be */
/* copy+pasted due to differing struct definitions. */

static struct Tux64PlatformMipsN64KernelParseResult
tux64_platform_mips_n64_kernel_parse_program_headers_64(
   const Tux64UInt8 * elf_data,
   Tux64UInt32 elf_bytes,
   Tux64UInt32 addr_entry,
   Tux64UInt32 phoff,
   Tux64UInt16 phnum
) {
   struct Tux64PlatformMipsN64KernelParseResult result;
   Tux64Boolean main_segment_found;
   const Tux64UInt8 * iter_program_headers;
   struct Tux64ElfProgramHeader64 program_header;
   Tux64UInt32 segment_type;
   Tux64UInt64 segment_file_offset;
   Tux64UInt64 image_bytes;
   Tux64UInt64 image_memory;
   Tux64UInt64 addr_load;
   Tux64UInt64 alignment;

   main_segment_found = TUX64_BOOLEAN_FALSE;
   iter_program_headers = &elf_data[phoff];

   while (phnum != TUX64_LITERAL_UINT16(0u)) {
      tux64_memory_copy(
         &program_header,
         iter_program_headers,
         TUX64_LITERAL_UINT32(sizeof(program_header))
      );
      iter_program_headers += sizeof(program_header);
      phnum--;

      segment_type = tux64_endian_convert_uint32(program_header.p_type, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);

      if (segment_type != TUX64_ELF_PROGRAM_HEADER_TYPE_LOAD) {
         continue;
      }

      if (main_segment_found == TUX64_BOOLEAN_TRUE) {
         result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_MAIN_SEGMENT_DUPLICATE;
         return result;
      }
      main_segment_found = TUX64_BOOLEAN_TRUE;

      segment_file_offset = tux64_endian_convert_uint64(program_header.p_offset, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);
      image_bytes = tux64_endian_convert_uint64(program_header.p_filesz, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);
      image_memory = tux64_endian_convert_uint64(program_header.p_memsz, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);
      addr_load = tux64_endian_convert_uint64(program_header.p_vaddr, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);
      alignment = tux64_endian_convert_uint64(program_header.p_align, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);

      /* since we're now in 64-bit, we make sure to...not be in 64-bit! */
      /* skips addr_load since that's relative to the N64's address space, */
      /* which truncates the high 32-bits. */
      if (segment_file_offset > TUX64_LITERAL_UINT64(TUX64_UINT32_MAX)) {
         result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
         return result;
      }
      if (image_bytes > TUX64_LITERAL_UINT64(TUX64_UINT32_MAX)) {
         result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
         return result;
      }
      if (image_memory > TUX64_LITERAL_UINT64(TUX64_UINT32_MAX)) {
         result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
         return result;
      }
      if (alignment > TUX64_LITERAL_UINT64(TUX64_UINT32_MAX)) {
         result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
         return result;
      }

      /* ...and now the usual safety check */
      if (segment_file_offset + image_bytes > elf_bytes) {
         result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
         return result;
      }
   }

   /* done like this so we can both detect duplicate and missing main segments. */
   if (main_segment_found == TUX64_BOOLEAN_FALSE) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_MAIN_SEGMENT_MISSING;
      return result;
   }

   /* great success! */
   result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_OK;
   result.payload.ok.image.offset = (Tux64UInt32)segment_file_offset;
   result.payload.ok.image.bytes = (Tux64UInt32)image_bytes;
   result.payload.ok.memory = (Tux64UInt32)image_memory;
   result.payload.ok.addr_load = (Tux64UInt32)addr_load;
   result.payload.ok.addr_entry = (Tux64UInt32)addr_entry;
   result.payload.ok.alignment = (Tux64UInt32)alignment;
   return result;
}

static struct Tux64PlatformMipsN64KernelParseResult
tux64_platform_mips_n64_kernel_parse_64(
   const Tux64UInt8 * elf_data,
   Tux64UInt32 elf_bytes
) {
   struct Tux64PlatformMipsN64KernelParseResult result;
   struct Tux64ElfHeader64 elf_header;
   Tux64UInt16 ehsize;
   Tux64UInt16 phsize;
   Tux64UInt32 addr_entry;
   Tux64UInt64 phoff64;
   Tux64UInt32 phoff;
   Tux64UInt16 phnum;

   if (elf_bytes < TUX64_LITERAL_UINT32(sizeof(elf_header))) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
      return result;
   }
   tux64_memory_copy(
      &elf_header,
      &elf_data[0u],
      TUX64_LITERAL_UINT32(sizeof(elf_header))
   );

   result = tux64_platform_mips_n64_kernel_parse_elf_header_common((const union Tux64ElfHeader *)&elf_header);
   if (result.status != TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_OK) {
      return result;
   }

   ehsize = tux64_endian_convert_uint16(elf_header.e_ehsize, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);
   phsize = tux64_endian_convert_uint16(elf_header.e_phentsize, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);

   if (ehsize != TUX64_LITERAL_UINT16(sizeof(elf_header))) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
      return result;
   }
   if (phsize != TUX64_LITERAL_UINT16(sizeof(struct Tux64ElfProgramHeader64))) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
      return result;
   }

   /* it's fine to truncate here, because the N64 does the same thing. */
   addr_entry = (Tux64UInt32)tux64_endian_convert_uint64(elf_header.e_entry, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);

   /* however, it's NOT fine to truncate here, as it could be due to either file limitations or bad data. */
   phoff64 = tux64_endian_convert_uint64(elf_header.e_phoff, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);
   phnum = tux64_endian_convert_uint16(elf_header.e_phnum, TUX64_PLATFORM_MIPS_N64_KERNEL_ENDIAN_FORMAT);
   if (phoff64 > TUX64_LITERAL_UINT64(TUX64_UINT32_MAX)) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
      return result;
   }
   phoff = (Tux64UInt32)phoff64;

   if (tux64_platform_mips_n64_kernel_program_headers_present(
      elf_bytes,
      phoff,
      phnum,
      TUX64_LITERAL_UINT8(sizeof(struct Tux64ElfProgramHeader64))
   ) == TUX64_BOOLEAN_FALSE) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
      return result;
   }

   return tux64_platform_mips_n64_kernel_parse_program_headers_64(
      elf_data,
      elf_bytes,
      addr_entry,
      phoff,
      phnum
   );
}

static Tux64Boolean
tux64_platform_mips_n64_kernel_validate_alignment(
   Tux64UInt32 alignment
) {
   /* we simply have to make sure the alignment is a power of two. */
   /* oh, and by the way, i stole this code.  hah. */
   return ((alignment & (alignment - TUX64_LITERAL_UINT32(1u))) == 0u)
      ? TUX64_BOOLEAN_TRUE
      : TUX64_BOOLEAN_FALSE;
}

struct Tux64PlatformMipsN64KernelParseResult
tux64_platform_mips_n64_kernel_parse(
   const Tux64UInt8 * elf_data,
   Tux64UInt32 elf_bytes
) {
   struct Tux64PlatformMipsN64KernelParseResult result;
   Tux64UInt8 elf_header_ident [TUX64_ELF_HEADER_IDENT_BYTES];
   const struct Tux64PlatformMipsN64Kernel * kernel;

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
      &elf_header_ident[TUX64_ELF_HEADER_IDENT_INDEX_MAGIC0],
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
         result = tux64_platform_mips_n64_kernel_parse_32(elf_data, elf_bytes);
         break;

      case TUX64_ELF_HEADER_IDENT_CLASS_64:
         result = tux64_platform_mips_n64_kernel_parse_64(elf_data, elf_bytes);
         break;

      default:
         result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
         break;
   }
   if (result.status != TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_OK) {
      return result;
   }

   kernel = &result.payload.ok;

   /* just a few more sanity checks and we're done! */
   if (kernel->addr_entry < kernel->addr_load) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
      return result;
   }
   if (kernel->memory < kernel->image.bytes) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
      return result;
   }
   if (tux64_platform_mips_n64_kernel_validate_alignment(kernel->alignment) == TUX64_BOOLEAN_FALSE) {
      result.status = TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE;
      return result;
   }

   /* :sunglasses: */
   return result;
}

