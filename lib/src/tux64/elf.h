/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/elf.h - ELF types and header definitions.                    */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_ELF_H
#define _TUX64_ELF_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"

#define TUX64_ELF_HEADER_IDENT_INDEX_MAGIC \
   0u
#define TUX64_ELF_HEADER_IDENT_INDEX_CLASS \
   4u
#define TUX64_ELF_HEADER_IDENT_INDEX_DATA \
   5u
#define TUX64_ELF_HEADER_IDENT_INDEX_VERSION \
   6u
#define TUX64_ELF_HEADER_IDENT_INDEX_OSABI \
   7u
#define TUX64_ELF_HEADER_IDENT_INDEX_ABI_VERSION \
   8u
#define TUX64_ELF_HEADER_IDENT_INDEX_PAD \
   9u

#define TUX64_ELF_HEADER_IDENT_MAGIC_BYTES \
   4u
#define TUX64_ELF_HEADER_IDENT_BYTES \
   16u

#define TUX64_ELF_HEADER_IDENT_MAGIC \
   "\x7f\x45\x4c\x46" /* (0x7f) ELF */

enum Tux64ElfHeaderIdentClass {
   TUX64_ELF_HEADER_IDENT_CLASS_32 = 0x01u,
   TUX64_ELF_HEADER_IDENT_CLASS_64 = 0x02u
};

enum Tux64ElfHeaderIdentData {
   TUX64_ELF_HEADER_IDENT_DATA_ENDIAN_LITTLE = 0x01u,
   TUX64_ELF_HEADER_IDENT_DATA_ENDIAN_BIG    = 0x02u
};

enum Tux64ElfHeaderIdentVersion {
   TUX64_ELF_HEADER_IDENT_VERSION_1 = 0x01u
};

enum Tux64ElfHeaderIdentOSAbi {
   TUX64_ELF_HEADER_IDENT_OSABI_SYSTEM_V        = 0x00u,
   TUX64_ELF_HEADER_IDENT_OSABI_HP_UX           = 0x01u,
   TUX64_ELF_HEADER_IDENT_OSABI_NETBSD          = 0x02u,
   TUX64_ELF_HEADER_IDENT_OSABI_LINUX           = 0x03u,
   TUX64_ELF_HEADER_IDENT_OSABI_GNU_HURD        = 0x04u,
   TUX64_ELF_HEADER_IDENT_OSABI_SOLARIS         = 0x06u,
   TUX64_ELF_HEADER_IDENT_OSABI_AIX             = 0x07u,
   TUX64_ELF_HEADER_IDENT_OSABI_IRIX            = 0x08u,
   TUX64_ELF_HEADER_IDENT_OSABI_FREEBSD         = 0x09u,
   TUX64_ELF_HEADER_IDENT_OSABI_TRU64           = 0x0au,
   TUX64_ELF_HEADER_IDENT_OSABI_NOVELL_MODESTO  = 0x0bu,
   TUX64_ELF_HEADER_IDENT_OSABI_OPENBSD         = 0x0cu,
   TUX64_ELF_HEADER_IDENT_OSABI_OPENVMS         = 0x0du,
   TUX64_ELF_HEADER_IDENT_OSABI_NONSTOP_KERNEL  = 0x0eu,
   TUX64_ELF_HEADER_IDENT_OSABI_AROS            = 0x0fu,
   TUX64_ELF_HEADER_IDENT_OSABI_FENIXOS         = 0x10u,
   TUX64_ELF_HEADER_IDENT_OSABI_NUXI_CLOUDABI   = 0x11u,
   TUX64_ELF_HEADER_IDENT_OSABI_OPENVOS         = 0x12u
};

enum Tux64ElfHeaderType {
   TUX64_ELF_HEADER_TYPE_NONE    = 0x0000u,
   TUX64_ELF_HEADER_TYPE_REL     = 0x0001u,
   TUX64_ELF_HEADER_TYPE_EXEC    = 0x0002u,
   TUX64_ELF_HEADER_TYPE_DYN     = 0x0003u,
   TUX64_ELF_HEADER_TYPE_CORE    = 0x0004u,
   TUX64_ELF_HEADER_TYPE_LOOS    = 0xfe00u,
   TUX64_ELF_HEADER_TYPE_HIOS    = 0xfeffu,
   TUX64_ELF_HEADER_TYPE_LOPROC  = 0xff00u,
   TUX64_ELF_HEADER_TYPE_HIPROC  = 0xffffu
};

enum Tux64ElfHeaderMachine {
   TUX64_ELF_HEADER_MACHINE_UNSPECIFIED         = 0x0000u,
   TUX64_ELF_HEADER_MACHINE_ATNT_WE_32100       = 0x0001u,
   TUX64_ELF_HEADER_MACHINE_SPARC               = 0x0002u,
   TUX64_ELF_HEADER_MACHINE_X86_32              = 0x0003u,
   TUX64_ELF_HEADER_MACHINE_M68K                = 0x0004u,
   TUX64_ELF_HEADER_MACHINE_M88K                = 0x0005u,
   TUX64_ELF_HEADER_MACHINE_INTEL_MCU           = 0x0006u,
   TUX64_ELF_HEADER_MACHINE_INTEL_80860         = 0x0007u,
   TUX64_ELF_HEADER_MACHINE_MIPS                = 0x0008u,
   TUX64_ELF_HEADER_MACHINE_IBM_SYSTEM370       = 0x0009u,
   TUX64_ELF_HEADER_MACHINE_MIPS_RS3000_LE      = 0x000au,
   TUX64_ELF_HEADER_MACHINE_HP_PA_RISC          = 0x000fu,
   TUX64_ELF_HEADER_MACHINE_INTEL_80960         = 0x0013u,
   TUX64_ELF_HEADER_MACHINE_POWERPC_32          = 0x0014u,
   TUX64_ELF_HEADER_MACHINE_POWERPC_64          = 0x0015u,
   TUX64_ELF_HEADER_MACHINE_S390                = 0x0016u,
   TUX64_ELF_HEADER_MACHINE_IBM_SPU             = 0x0017u,
   TUX64_ELF_HEADER_MACHINE_NEC_V800            = 0x0024u,
   TUX64_ELF_HEADER_MACHINE_FUJITSU_FR20        = 0x0025u,
   TUX64_ELF_HEADER_MACHINE_TRW_RH_32           = 0x0026u,
   TUX64_ELF_HEADER_MACHINE_MOTOROLA_RCE        = 0x0027u,
   TUX64_ELF_HEADER_MACHINE_ARM_32              = 0x0028u,
   TUX64_ELF_HEADER_MACHINE_DIGITAL_ALPHA       = 0x0029u,
   TUX64_ELF_HEADER_MACHINE_SUPERH              = 0x002au,
   TUX64_ELF_HEADER_MACHINE_SPARC_V9            = 0x002bu,
   TUX64_ELF_HEADER_MACHINE_SIEMENS_TRICORE     = 0x002cu,
   TUX64_ELF_HEADER_MACHINE_ARGONAUT_RISC_CORE  = 0x002du,
   TUX64_ELF_HEADER_MACHINE_HITACHI_H8_300      = 0x002eu,
   TUX64_ELF_HEADER_MACHINE_HITACHI_H8_300H     = 0x002fu,
   TUX64_ELF_HEADER_MACHINE_HITACHI_H8S         = 0x0030u,
   TUX64_ELF_HEADER_MACHINE_HITACHI_H8_500      = 0x0031u,
   TUX64_ELF_HEADER_MACHINE_IA_64               = 0x0032u,
   TUX64_ELF_HEADER_MACHINE_STANFORD_MIPS_X     = 0x0033u,
   TUX64_ELF_HEADER_MACHINE_MOTOROLA_COLDFIRE   = 0x0034u,
   TUX64_ELF_HEADER_MACHINE_MOTOROLA_M68HC12    = 0x0035u,
   TUX64_ELF_HEADER_MACHINE_FUJITSU_MMA         = 0x0036u,
   TUX64_ELF_HEADER_MACHINE_SIEMENS_PCP         = 0x0037u,
   TUX64_ELF_HEADER_MACHINE_SONY_NCPU_RISC      = 0x0038u,
   TUX64_ELF_HEADER_MACHINE_DENSO_NDR1          = 0x0039u,
   TUX64_ELF_HEADER_MACHINE_MOTOROLA_STARCORE   = 0x003au,
   TUX64_ELF_HEADER_MACHINE_TOYOTA_ME16         = 0x003bu,
   TUX64_ELF_HEADER_MACHINE_STM_ST100           = 0x003cu,
   TUX64_ELF_HEADER_MACHINE_ALC_TINYJ           = 0x003du,
   TUX64_ELF_HEADER_MACHINE_X86_64              = 0x003eu,
   TUX64_ELF_HEADER_MACHINE_SONY_DSP            = 0x003fu,
   TUX64_ELF_HEADER_MACHINE_PDP_10              = 0x0040u,
   TUX64_ELF_HEADER_MACHINE_PDP_11              = 0x0041u,
   TUX64_ELF_HEADER_MACHINE_SIEMENS_FX66        = 0x0042u,
   TUX64_ELF_HEADER_MACHINE_STM_ST9             = 0x0043u,
   TUX64_ELF_HEADER_MACHINE_STM_ST7             = 0x0044u,
   TUX64_ELF_HEADER_MACHINE_MOTOROLA_MC86HC16   = 0x0045u,
   TUX64_ELF_HEADER_MACHINE_MOTOROLA_MC86HC11   = 0x0046u,
   TUX64_ELF_HEADER_MACHINE_MOTOROLA_MC86HC08   = 0x0047u,
   TUX64_ELF_HEADER_MACHINE_MOTOROLA_MC86HC05   = 0x0048u,
   TUX64_ELF_HEADER_MACHINE_SGI_SVX             = 0x0049u,
   TUX64_ELF_HEADER_MACHINE_STM_ST19            = 0x004au,
   TUX64_ELF_HEADER_MACHINE_DIGITAL_VAX         = 0x004bu,
   TUX64_ELF_HEADER_MACHINE_AXIS_COM_32         = 0x004cu,
   TUX64_ELF_HEADER_MACHINE_INFINEON_TECH_32    = 0x004du,
   TUX64_ELF_HEADER_MACHINE_ELEMENT_14_DSP_64   = 0x004eu,
   TUX64_ELF_HEADER_MACHINE_LSI_LOGIC_DSP_16    = 0x004fu,
   TUX64_ELF_HEADER_MACHINE_TMS320C6000         = 0x008cu,
   TUX64_ELF_HEADER_MACHINE_MCST_ELBRUS_E2K     = 0x00afu,
   TUX64_ELF_HEADER_MACHINE_ARM_64              = 0x00b7u,
   TUX64_ELF_HEADER_MACHINE_ZILOG_Z80           = 0x00dcu,
   TUX64_ELF_HEADER_MACHINE_RISC_V              = 0x00f3u,
   TUX64_ELF_HEADER_MACHINE_BPF                 = 0x00f7u,
   TUX64_ELF_HEADER_MACHINE_WDC_65C816          = 0x0101u,
   TUX64_ELF_HEADER_MACHINE_LOONGARCH           = 0x0102u
};

enum Tux64ElfHeaderVersion {
   TUX64_ELF_HEADER_VERSION_1 = 0x00000001u
};

struct Tux64ElfHeader32 {
   Tux64UInt8  e_ident [TUX64_ELF_HEADER_IDENT_BYTES];
   Tux64UInt16 e_type;
   Tux64UInt16 e_machine;
   Tux64UInt32 e_version;
   Tux64UInt32 e_entry;
   Tux64UInt32 e_phoff;
   Tux64UInt32 e_shoff;
   Tux64UInt32 e_flags;
   Tux64UInt16 e_ehsize;
   Tux64UInt16 e_phentsize;
   Tux64UInt16 e_phnum;
   Tux64UInt16 e_shentsize;
   Tux64UInt16 e_shnum;
   Tux64UInt16 e_shstrndx;
};

struct Tux64ElfHeader64 {
   Tux64UInt8  e_ident [TUX64_ELF_HEADER_IDENT_BYTES];
   Tux64UInt16 e_type;
   Tux64UInt16 e_machine;
   Tux64UInt32 e_version;
   Tux64UInt64 e_entry;
   Tux64UInt64 e_phoff;
   Tux64UInt64 e_shoff;
   Tux64UInt32 e_flags;
   Tux64UInt16 e_ehsize;
   Tux64UInt16 e_phentsize;
   Tux64UInt16 e_phnum;
   Tux64UInt16 e_shentsize;
   Tux64UInt16 e_shnum;
   Tux64UInt16 e_shstrndx;
};

union Tux64ElfHeader {
   struct Tux64ElfHeader32 elf32;
   struct Tux64ElfHeader64 elf64;
};

enum Tux64ElfProgramHeaderType {
   TUX64_ELF_PROGRAM_HEADER_TYPE_NULL     = 0x00000000u,
   TUX64_ELF_PROGRAM_HEADER_TYPE_LOAD     = 0x00000001u,
   TUX64_ELF_PROGRAM_HEADER_TYPE_DYNAMIC  = 0x00000002u,
   TUX64_ELF_PROGRAM_HEADER_TYPE_INTERP   = 0x00000003u,
   TUX64_ELF_PROGRAM_HEADER_TYPE_NOTE     = 0x00000004u,
   TUX64_ELF_PROGRAM_HEADER_TYPE_SHLIB    = 0x00000005u,
   TUX64_ELF_PROGRAM_HEADER_TYPE_PHDR     = 0x00000006u,
   TUX64_ELF_PROGRAM_HEADER_TYPE_TLS      = 0x00000007u,
   TUX64_ELF_PROGRAM_HEADER_TYPE_LOOS     = 0x60000000u,
   TUX64_ELF_PROGRAM_HEADER_TYPE_HIOS     = 0x6fffffffu,
   TUX64_ELF_PROGRAM_HEADER_TYPE_LOPROC   = 0x70000000u,
   TUX64_ELF_PROGRAM_HEADER_TYPE_HIPROC   = 0x7fffffffu
};

#define TUX64_ELF_PROGRAM_HEADER_FLAG_X (1u << 0u)
#define TUX64_ELF_PROGRAM_HEADER_FLAG_W (1u << 1u)
#define TUX64_ELF_PROGRAM_HEADER_FLAG_R (1u << 2u)

struct Tux64ElfProgramHeader32 {
   Tux64UInt32 p_type;
   Tux64UInt32 p_offset;
   Tux64UInt32 p_vaddr;
   Tux64UInt32 p_paddr;
   Tux64UInt32 p_filesz;
   Tux64UInt32 p_memsz;
   Tux64UInt32 p_flags;
   Tux64UInt32 p_align;
};

struct Tux64ElfProgramHeader64 {
   Tux64UInt32 p_type;
   Tux64UInt32 p_flags;
   Tux64UInt64 p_offset;
   Tux64UInt64 p_vaddr;
   Tux64UInt64 p_paddr;
   Tux64UInt64 p_filesz;
   Tux64UInt64 p_memsz;
   Tux64UInt64 p_align;
};

union Tux64ElfProgramHeader {
   struct Tux64ElfProgramHeader32 elf32;
   struct Tux64ElfProgramHeader64 elf64;
};

enum Tux64ElfSectionHeaderType {
   TUX64_ELF_SECTION_HEADER_TYPE_NULL           = 0x00000000u,
   TUX64_ELF_SECTION_HEADER_TYPE_PROGBITS       = 0x00000001u,
   TUX64_ELF_SECTION_HEADER_TYPE_SYMTAB         = 0x00000002u,
   TUX64_ELF_SECTION_HEADER_TYPE_STRTAB         = 0x00000003u,
   TUX64_ELF_SECTION_HEADER_TYPE_RELA           = 0x00000004u,
   TUX64_ELF_SECTION_HEADER_TYPE_HASH           = 0x00000005u,
   TUX64_ELF_SECTION_HEADER_TYPE_DYNAMIC        = 0x00000006u,
   TUX64_ELF_SECTION_HEADER_TYPE_NOTE           = 0x00000007u,
   TUX64_ELF_SECTION_HEADER_TYPE_NOBITS         = 0x00000008u,
   TUX64_ELF_SECTION_HEADER_TYPE_REL            = 0x00000009u,
   TUX64_ELF_SECTION_HEADER_TYPE_SHLIB          = 0x0000000au,
   TUX64_ELF_SECTION_HEADER_TYPE_DYNSYM         = 0x0000000bu,
   TUX64_ELF_SECTION_HEADER_TYPE_INIT_ARRAY     = 0x0000000eu,
   TUX64_ELF_SECTION_HEADER_TYPE_FINI_ARRAY     = 0x0000000fu,
   TUX64_ELF_SECTION_HEADER_TYPE_PREINIT_ARRAY  = 0x00000010u,
   TUX64_ELF_SECTION_HEADER_TYPE_GROUP          = 0x00000011u,
   TUX64_ELF_SECTION_HEADER_TYPE_SYMTAB_SHNDX   = 0x00000012u,
   TUX64_ELF_SECTION_HEADER_TYPE_NUM            = 0x00000013u,
   TUX64_ELF_SECTION_HEADER_TYPE_LOOS           = 0x60000000u
};

#define TUX64_ELF_SECTION_HEADER_FLAG_WRITE              (1u <<  0u)
#define TUX64_ELF_SECTION_HEADER_FLAG_ALLOC              (1u <<  1u)
#define TUX64_ELF_SECTION_HEADER_FLAG_EXECINSTR          (1u <<  2u)
#define TUX64_ELF_SECTION_HEADER_FLAG_MERGE              (1u <<  4u)
#define TUX64_ELF_SECTION_HEADER_FLAG_STRINGS            (1u <<  5u)
#define TUX64_ELF_SECTION_HEADER_FLAG_INFO_LINK          (1u <<  6u)
#define TUX64_ELF_SECTION_HEADER_FLAG_LINK_ORDER         (1u <<  7u)
#define TUX64_ELF_SECTION_HEADER_FLAG_OS_NONCONFORMING   (1u <<  8u)
#define TUX64_ELF_SECTION_HEADER_FLAG_GROUP              (1u <<  9u)
#define TUX64_ELF_SECTION_HEADER_FLAG_TLS                (1u << 10u)
#define TUX64_ELF_SECTION_HEADER_FLAG_MASK_OS            (0x0ff00000u)
#define TUX64_ELF_SECTION_HEADER_FLAG_MASK_PROC          (0xf0000000u)
#define TUX64_ELF_SECTION_HEADER_FLAG_ORDERED            (1u << 26u)
#define TUX64_ELF_SECTION_HEADER_FLAG_EXCLUDE            (1u << 27u)

struct Tux64ElfSectionHeader32 {
   Tux64UInt32 sh_name;
   Tux64UInt32 sh_type;
   Tux64UInt32 sh_flags;
   Tux64UInt32 sh_addr;
   Tux64UInt32 sh_offset;
   Tux64UInt32 sh_size;
   Tux64UInt32 sh_link;
   Tux64UInt32 sh_info;
   Tux64UInt32 sh_addralign;
   Tux64UInt32 sh_entsize;
};

struct Tux64ElfSectionHeader64 {
   Tux64UInt32 sh_name;
   Tux64UInt32 sh_type;
   Tux64UInt64 sh_flags;
   Tux64UInt64 sh_addr;
   Tux64UInt64 sh_offset;
   Tux64UInt64 sh_size;
   Tux64UInt32 sh_link;
   Tux64UInt32 sh_info;
   Tux64UInt64 sh_addralign;
   Tux64UInt64 sh_entsize;
};

union Tux64ElfSectionHeader {
   struct Tux64ElfSectionHeader32 elf32;
   struct Tux64ElfSectionHeader64 elf64;
};

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_ELF_H */

