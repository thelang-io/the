/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_CODEGEN_MACOS_H
#define SRC_CODEGEN_MACOS_H

#include <duc/binary.h>
#include "ast.h"

#define CGM_CMD_REQ_DYLD 0x80000000
#define CGM_CMD_SYMTAB 0x02
#define CGM_CMD_DYSYMTAB 0x0B
#define CGM_CMD_LOAD_DYLIB 0x0C
#define CGM_CMD_LOAD_DYLINKER 0x0E
#define CGM_CMD_SEGMENT 0x19
#define CGM_CMD_DYLD_INFO_ONLY (0x22 | CGM_CMD_REQ_DYLD)
#define CGM_CMD_VERSION_MIN_MACOS 0x24
#define CGM_CMD_MAIN (0x28 | CGM_CMD_REQ_DYLD)
#define CGM_CMD_DATA_IN_CODE 0x29
#define CGM_CMD_SOURCE_VERSION 0x2A

#define CGM_CPU_ARCH_ABI64 0x01000000
#define CGM_CPU_SUBTYPE_X86_64 0x03
#define CGM_CPU_TYPE_X86 0x07
#define CGM_CPU_TYPE_X86_64 (CGM_CPU_TYPE_X86 | CGM_CPU_ARCH_ABI64)

#define CGM_FILE_TYPE_EXECUTE 0x02

#define CGM_FLAG_NOUNDEFS 0x01
#define CGM_FLAG_DYLDLINK 0x04
#define CGM_FLAG_TWOLEVEL 0x80

#define CGM_MAGIC 0xFEEDFACF

#define CGM_DATA_INFILE_OFFSET 0x4000
#define CGM_DATA_INFILE_SIZE 0x4000
#define CGM_DATA_INMEM_OFFSET 0x0000000100004000
#define CGM_DATA_INMEM_SIZE 0x4000
#define CGM_LINKEDIT_INFILE_OFFSET 0x8000
#define CGM_LINKEDIT_INFILE_SIZE 0x4000
#define CGM_LINKEDIT_INMEM_OFFSET 0x0000000100008000
#define CGM_LINKEDIT_INMEM_SIZE 0x4000
#define CGM_PAGEZERO_INMEM_OFFSET 0x0000000000000000
#define CGM_PAGEZERO_INMEM_SIZE 0x0000000100000000

#define CGM_PROT_NONE 0x00
#define CGM_PROT_READ 0x01
#define CGM_PROT_WRITE 0x02
#define CGM_PROT_EXECUTE 0x04
#define CGM_PROT_DEFAULT (CGM_PROT_READ | CGM_PROT_WRITE)

#define CGM_SECT_ATTR_PURE_INSTRUCTIONS 0x80000000
#define CGM_SECT_ATTR_SOME_INSTRUCTIONS 0x00000400
#define CGM_SECT_FLAG_REGULAR 0x00

#define CGM_SECT_DATA "__data"
#define CGM_SECT_TEXT "__text"

#define CGM_SEG_DATA "__DATA"
#define CGM_SEG_LINKEDIT "__LINKEDIT"
#define CGM_SEG_PAGEZERO "__PAGEZERO"
#define CGM_SEG_TEXT "__TEXT"

#define CGM_SYM_EXECUTE "__mh_execute_header"

#define CGM_SYM_TYPE_UNDF 0x00
#define CGM_SYM_TYPE_EXT 0x01
#define CGM_SYM_TYPE_ABS 0x02
#define CGM_SYM_TYPE_SECT 0x0E

typedef struct cgm_s cgm_t;
typedef struct cgm_cmd_s cgm_cmd_t;
typedef struct cgm_cmd_dyld_info_s cgm_cmd_dyld_info_t;
typedef struct cgm_cmd_dylib_s cgm_cmd_dylib_t;
typedef struct cgm_cmd_dylinker_s cgm_cmd_dylinker_t;
typedef struct cgm_cmd_dysymtab_s cgm_cmd_dysymtab_t;
typedef struct cgm_cmd_linkedit_data_s cgm_cmd_linkedit_data_t;
typedef struct cgm_cmd_main_s cgm_cmd_main_t;
typedef struct cgm_cmd_seg_s cgm_cmd_seg_t;
typedef struct cgm_cmd_src_ver_s cgm_cmd_src_ver_t;
typedef struct cgm_cmd_symtab_s cgm_cmd_symtab_t;
typedef struct cgm_cmd_ver_min_s cgm_cmd_ver_min_t;
typedef struct cgm_dylib_s cgm_dylib_t;
typedef struct cgm_header_s cgm_header_t;
typedef struct cgm_sect_s cgm_sect_t;
typedef struct cgm_sym_s cgm_sym_t;
typedef union cgm_str_u cgm_str_t;

union cgm_str_u {
  uint32_t offset;
};

struct cgm_dylib_s {
  cgm_str_t name;
  uint32_t timestamp;
  uint32_t current_version;
  uint32_t compatibility_version;
};

struct cgm_cmd_s {
  uint32_t cmd;
  uint32_t size;
};

struct cgm_cmd_dyld_info_s {
  uint32_t cmd;
  uint32_t size;
  uint32_t rebase_offset;
  uint32_t rebase_size;
  uint32_t bind_offset;
  uint32_t bind_size;
  uint32_t weak_bind_offset;
  uint32_t weak_bind_size;
  uint32_t lazy_bind_offset;
  uint32_t lazy_bind_size;
  uint32_t export_offset;
  uint32_t export_size;
};

struct cgm_cmd_dylib_s {
  uint32_t cmd;
  uint32_t size;
  cgm_dylib_t dylib;
};

struct cgm_cmd_dylinker_s {
  uint32_t cmd;
  uint32_t size;
  cgm_str_t name;
};

struct cgm_cmd_dysymtab_s {
  uint32_t cmd;
  uint32_t size;
  uint32_t loc_idx;
  uint32_t loc_count;
  uint32_t extdef_idx;
  uint32_t extdef_count;
  uint32_t undef_idx;
  uint32_t undef_count;
  uint32_t contents_offset;
  uint32_t contents_count;
  uint32_t module_offset;
  uint32_t module_count;
  uint32_t extref_offset;
  uint32_t extref_count;
  uint32_t indirect_offset;
  uint32_t indirect_count;
  uint32_t extrel_offset;
  uint32_t extrel_count;
  uint32_t locrel_offset;
  uint32_t locrel_count;
};

struct cgm_cmd_linkedit_data_s {
  uint32_t cmd;
  uint32_t size;
  uint32_t data_offset;
  uint32_t data_size;
};

struct cgm_cmd_main_s {
  uint32_t cmd;
  uint32_t size;
  uint64_t entry_offset;
  uint64_t stack_size;
};

struct cgm_cmd_seg_s {
  uint32_t cmd;
  uint32_t size;
  char name[16];
  uint64_t vm_address;
  uint64_t vm_size;
  uint64_t file_offset;
  uint64_t file_size;
  int32_t prot_max;
  int32_t prot_initial;
  uint32_t sects_count;
  uint32_t flags;
};

struct cgm_cmd_src_ver_s {
  uint32_t cmd;
  uint32_t size;
  uint64_t version;
};

struct cgm_cmd_symtab_s {
  uint32_t cmd;
  uint32_t size;
  uint32_t sym_offset;
  uint32_t sym_count;
  uint32_t str_offset;
  uint32_t str_size;
};

struct cgm_cmd_ver_min_s {
  uint32_t cmd;
  uint32_t size;
  uint32_t version;
  uint32_t sdk_version;
};

struct cgm_header_s {
  uint32_t magic;
  int32_t cpu_type;
  int32_t cpu_subtype;
  uint32_t file_type;
  uint32_t cmds_count;
  uint32_t cmds_size;
  uint32_t flags;
  uint32_t reserved;
};

struct cgm_sect_s {
  char sect_name[16];
  char seg_name[16];
  uint64_t address;
  uint64_t size;
  uint32_t file_offset;
  uint32_t align;
  uint32_t reloc_offset;
  uint32_t reloc_count;
  uint32_t flags;
  uint32_t reserved1;
  uint32_t reserved2;
  uint32_t reserved3;
};

struct cgm_sym_s {
  uint32_t strtab_idx;
  uint8_t type;
  uint8_t sect_idx;
  uint16_t description;
  uint64_t value;
};

struct cgm_s {
  duc_array_t *cmds;
  duc_binary_t *dyld_info;
  cgm_header_t header;
  duc_binary_t *sec_data;
  duc_binary_t *sec_text;
  duc_binary_t *strs;
  duc_array_t *syms;
};

duc_binary_t *cgm_binary (const cgm_t *cgm);
void cgm_free (cgm_t *cgm);
cgm_t *cgm_new (const ast_t *ast);
void cgm_sect (cgm_cmd_seg_t **cmd_seg, cgm_sect_t *sect);
void cgm_str (cgm_cmd_t **cmd, cgm_str_t *str, const char *data);

#endif
