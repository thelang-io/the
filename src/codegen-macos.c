/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include <string.h>
#include "codegen-macos.h"

duc_binary_t *cgm_binary (const cgm_t *cgm) {
  duc_binary_t *bin = duc_binary_new();
  duc_binary_append_data(bin, &cgm->header, sizeof(cgm_header_t));

  for (size_t i = 0, size = duc_array_length(cgm->cmds); i < size; i++) {
    cgm_cmd_t *cmd = duc_array_at(cgm->cmds, i);
    duc_binary_append_data(bin, cmd, cmd->size);
  }

  size_t zero = CGM_DATA_INFILE_OFFSET - duc_binary_size(cgm->sec_text) - duc_binary_size(bin);
  duc_binary_append_times(bin, 0x00, zero);
  duc_binary_append_binary(bin, cgm->sec_text);
  duc_binary_append_binary(bin, cgm->sec_data);

  zero = CGM_DATA_INFILE_OFFSET + CGM_DATA_INFILE_SIZE - duc_binary_size(bin);
  duc_binary_append_times(bin, 0x00, zero);
  duc_binary_append_binary(bin, cgm->dyld_info);

  for (size_t i = 0, size = duc_array_length(cgm->syms); i < size; i++) {
    cgm_sym_t *sym = duc_array_at(cgm->syms, i);
    duc_binary_append_data(bin, sym, sizeof(cgm_sym_t));
  }

  duc_binary_append_binary(bin, cgm->strs);
  return bin;
}

void cgm_free (cgm_t *cgm) {
  duc_array_free(cgm->cmds, duc_free_simple);
  duc_binary_free(cgm->dyld_info);
  duc_binary_free(cgm->sec_data);
  duc_binary_free(cgm->sec_text);
  duc_binary_free(cgm->strs);
  duc_array_free(cgm->syms, duc_free_simple);
  free(cgm);
}

cgm_t *cgm_new (DUC_UNUSED const ast_t *ast) {
  cgm_t *cgm = malloc(sizeof(cgm_t));
  cgm->cmds = duc_array_new();
  cgm->dyld_info = duc_binary_new();
  cgm->sec_data = duc_binary_new();
  cgm->sec_text = duc_binary_new();
  cgm->strs = duc_binary_new();
  cgm->syms = duc_array_new();

  duc_binary_append_uint8( // TODO Normalize into structure
    cgm->dyld_info,
    0x00, 0x01, 0x5F, 0x6D, 0x61, 0x69, 0x6E, 0x00,
    0x09, 0x03, 0x00, 0xE0, 0x10, 0x00, 0x00, 0x00
  );

  duc_binary_append_uint8(
    cgm->sec_data,
    0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x2C, 0x20, 0x57,
    0x6F, 0x72, 0x6C, 0x64, 0x21, 0x0A
  );

  duc_binary_append_uint8(
    cgm->sec_text,
    0xB8, 0x04, 0x00, 0x00, 0x02, // movl $0x200004, %eax
    0xBF, 0x01, 0x00, 0x00, 0x00, // movl $0x000001, %edi
    0x48, 0xBE, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // movabsq $0x0000000100004000, %rsi
    0xBA, 0x0E, 0x00, 0x00, 0x00, // movl $0x00000E, %edx
    0x0F, 0x05, // syscall

    0xB8, 0x01, 0x00, 0x00, 0x02, // movl $0x200001, %eax
    0xBF, 0x00, 0x00, 0x00, 0x00, // movl $0x000000, %edi
    0x0F, 0x05 // syscall
  );

  duc_binary_append_string(cgm->strs, "_main");
  duc_binary_append_string(cgm->strs, "dyld_stub_binder");
  duc_binary_append_times(cgm->strs, 0x00, 8 - duc_binary_size(cgm->strs) % 8);

  cgm_sym_t *sym1 = malloc(sizeof(cgm_sym_t));

  sym1->strtab_idx = 0; // TODO
  sym1->type = CGM_SYM_TYPE_EXT | CGM_SYM_TYPE_SECT;
  sym1->sect_idx = 1;
  sym1->description = 0x0000; // TODO
  sym1->value = 0;

  duc_array_push(cgm->syms, sym1);

  cgm_sym_t *sym2 = malloc(sizeof(cgm_sym_t));

  sym2->strtab_idx = 0x001C - 22; // TODO
  sym2->type = CGM_SYM_TYPE_UNDF | CGM_SYM_TYPE_EXT;
  sym2->sect_idx = 0;
  sym2->description = 0x0100; // TODO
  sym2->value = 0;

  duc_array_push(cgm->syms, sym2);

  // CMD: DYLD_INFO_ONLY
  cgm_cmd_dyld_info_t *cmd_dyld_info = malloc(sizeof(cgm_cmd_dyld_info_t));

  cmd_dyld_info->cmd = CGM_CMD_DYLD_INFO_ONLY;
  cmd_dyld_info->size = sizeof(cgm_cmd_dyld_info_t);
  cmd_dyld_info->rebase_offset = 0;
  cmd_dyld_info->rebase_size = 0;
  cmd_dyld_info->bind_offset = 0;
  cmd_dyld_info->bind_size = 0;
  cmd_dyld_info->weak_bind_offset = 0;
  cmd_dyld_info->weak_bind_size = 0;
  cmd_dyld_info->lazy_bind_offset = 0;
  cmd_dyld_info->lazy_bind_size = 0;
  cmd_dyld_info->export_offset = 0;
  cmd_dyld_info->export_size = 0;

  duc_array_push(cgm->cmds, cmd_dyld_info);

  // CMD: DYSYMTAB
  cgm_cmd_dysymtab_t *cmd_dysymtab = malloc(sizeof(cgm_cmd_dysymtab_t));

  cmd_dysymtab->cmd = CGM_CMD_DYSYMTAB;
  cmd_dysymtab->size = sizeof(cgm_cmd_dysymtab_t);
  cmd_dysymtab->loc_idx = 0;
  cmd_dysymtab->loc_count = 0;
  cmd_dysymtab->extdef_idx = 0;
  cmd_dysymtab->extdef_count = 0;
  cmd_dysymtab->undef_idx = 0;
  cmd_dysymtab->undef_count = 0;
  cmd_dysymtab->contents_offset = 0;
  cmd_dysymtab->contents_count = 0;
  cmd_dysymtab->module_offset = 0;
  cmd_dysymtab->module_count = 0;
  cmd_dysymtab->extref_offset = 0;
  cmd_dysymtab->extref_count = 0;
  cmd_dysymtab->indirect_offset = 0;
  cmd_dysymtab->indirect_count = 0;
  cmd_dysymtab->extrel_offset = 0;
  cmd_dysymtab->extrel_count = 0;
  cmd_dysymtab->locrel_offset = 0;
  cmd_dysymtab->locrel_count = 0;

  duc_array_push(cgm->cmds, cmd_dysymtab);

  // CMD: LOAD_DYLIB (libSystem.B.dylib)
  cgm_cmd_dylib_t *cmd_dylib_system = malloc(sizeof(cgm_cmd_dylib_t));

  cmd_dylib_system->cmd = CGM_CMD_LOAD_DYLIB;
  cmd_dylib_system->size = sizeof(cgm_cmd_dylib_t);
  cmd_dylib_system->dylib.timestamp = 0;
  cmd_dylib_system->dylib.current_version = 0x050C3C01; // TODO
  cmd_dylib_system->dylib.compatibility_version = 0x00010000; // TODO
  cgm_str((cgm_cmd_t **) &cmd_dylib_system, &cmd_dylib_system->dylib.name, "/usr/lib/libSystem.B.dylib");

  duc_array_push(cgm->cmds, cmd_dylib_system);

  // CMD: LOAD_DYLINKER
  cgm_cmd_dylinker_t *cmd_load_dylinker = malloc(sizeof(cgm_cmd_dylinker_t));

  cmd_load_dylinker->cmd = CGM_CMD_LOAD_DYLINKER;
  cmd_load_dylinker->size = sizeof(cgm_cmd_dylinker_t);
  cgm_str((cgm_cmd_t **) &cmd_load_dylinker, &cmd_load_dylinker->name, "/usr/lib/dyld");

  duc_array_push(cgm->cmds, cmd_load_dylinker);

  // CMD: MAIN
  cgm_cmd_main_t *cmd_main = malloc(sizeof(cgm_cmd_main_t));

  cmd_main->cmd = CGM_CMD_MAIN;
  cmd_main->size = sizeof(cgm_cmd_main_t);
  cmd_main->entry_offset = 0;
  cmd_main->stack_size = 0;

  duc_array_push(cgm->cmds, cmd_main);

  // CMD: SEGMENT (__DATA)
  cgm_cmd_seg_t *cmd_seg_data = malloc(sizeof(cgm_cmd_seg_t));

  cmd_seg_data->cmd = CGM_CMD_SEGMENT;
  cmd_seg_data->size = sizeof(cgm_cmd_seg_t);
  strcpy(cmd_seg_data->name, CGM_SEG_DATA);
  cmd_seg_data->vm_address = 0;
  cmd_seg_data->vm_size = 0;
  cmd_seg_data->file_offset = 0;
  cmd_seg_data->file_size = 0;
  cmd_seg_data->prot_max = CGM_PROT_DEFAULT;
  cmd_seg_data->prot_initial = CGM_PROT_DEFAULT;
  cmd_seg_data->sects_count = 0;
  cmd_seg_data->flags = 0;

  cgm_sect_t *cmd_seg_data_data = cgm_sect(&cmd_seg_data, CGM_SECT_DATA, CGM_SEG_DATA);

  cmd_seg_data_data->align = 0;
  cmd_seg_data_data->flags = CGM_SECT_FLAG_REGULAR;

  duc_array_push(cgm->cmds, cmd_seg_data);

  // CMD: SEGMENT (__LINKEDIT)
  cgm_cmd_seg_t *cmd_seg_linkedit = malloc(sizeof(cgm_cmd_seg_t));

  cmd_seg_linkedit->cmd = CGM_CMD_SEGMENT;
  cmd_seg_linkedit->size = sizeof(cgm_cmd_seg_t);
  strcpy(cmd_seg_linkedit->name, CGM_SEG_LINKEDIT);
  cmd_seg_linkedit->vm_address = 0;
  cmd_seg_linkedit->vm_size = 0;
  cmd_seg_linkedit->file_offset = 0;
  cmd_seg_linkedit->file_size = 0;
  cmd_seg_linkedit->prot_max = CGM_PROT_READ;
  cmd_seg_linkedit->prot_initial = CGM_PROT_READ;
  cmd_seg_linkedit->sects_count = 0;
  cmd_seg_linkedit->flags = 0;

  duc_array_push(cgm->cmds, cmd_seg_linkedit);

  // CMD: SEGMENT (__PAGEZERO)
  cgm_cmd_seg_t *cmd_seg_pagezero = malloc(sizeof(cgm_cmd_seg_t));

  cmd_seg_pagezero->cmd = CGM_CMD_SEGMENT;
  cmd_seg_pagezero->size = sizeof(cgm_cmd_seg_t);
  strcpy(cmd_seg_pagezero->name, CGM_SEG_PAGEZERO);
  cmd_seg_pagezero->vm_address = 0;
  cmd_seg_pagezero->vm_size = 0;
  cmd_seg_pagezero->file_offset = 0;
  cmd_seg_pagezero->file_size = 0;
  cmd_seg_pagezero->prot_max = CGM_PROT_NONE;
  cmd_seg_pagezero->prot_initial = CGM_PROT_NONE;
  cmd_seg_pagezero->sects_count = 0;
  cmd_seg_pagezero->flags = 0;

  duc_array_push(cgm->cmds, cmd_seg_pagezero);

  // CMD: SEGMENT (__TEXT)
  cgm_cmd_seg_t *cmd_seg_text = malloc(sizeof(cgm_cmd_seg_t));

  cmd_seg_text->cmd = CGM_CMD_SEGMENT;
  cmd_seg_text->size = sizeof(cgm_cmd_seg_t);
  strcpy(cmd_seg_text->name, CGM_SEG_TEXT);
  cmd_seg_text->vm_address = 0;
  cmd_seg_text->vm_size = 0;
  cmd_seg_text->file_offset = 0;
  cmd_seg_text->file_size = 0;
  cmd_seg_text->prot_max = CGM_PROT_READ | CGM_PROT_EXECUTE;
  cmd_seg_text->prot_initial = CGM_PROT_READ | CGM_PROT_EXECUTE;
  cmd_seg_text->sects_count = 0;
  cmd_seg_text->flags = 0;

  cgm_sect_t *cmd_seg_text_text = cgm_sect(&cmd_seg_text, CGM_SECT_TEXT, CGM_SEG_TEXT);

  cmd_seg_text_text->align = 0;
  cmd_seg_text_text->flags = CGM_SECT_FLAG_REGULAR | CGM_SECT_ATTR_PURE_INSTRUCTIONS | CGM_SECT_ATTR_SOME_INSTRUCTIONS;

  duc_array_push(cgm->cmds, cmd_seg_text);

  // CMD: SOURCE_VERSION
  cgm_cmd_src_ver_t *cmd_src_ver = malloc(sizeof(cgm_cmd_src_ver_t));

  cmd_src_ver->cmd = CGM_CMD_SOURCE_VERSION;
  cmd_src_ver->size = sizeof(cgm_cmd_src_ver_t);
  cmd_src_ver->version = 0; // TODO pack as a24.b10.c10.d10.e10

  duc_array_push(cgm->cmds, cmd_src_ver);

  // CMD: SYMTAB
  cgm_cmd_symtab_t *cmd_symtab = malloc(sizeof(cgm_cmd_symtab_t));

  cmd_symtab->cmd = CGM_CMD_SYMTAB;
  cmd_symtab->size = sizeof(cgm_cmd_symtab_t);
  cmd_symtab->sym_offset = 0;
  cmd_symtab->sym_count = 0;
  cmd_symtab->str_offset = 0;
  cmd_symtab->str_size = 0;

  duc_array_push(cgm->cmds, cmd_symtab);

  // CMD: VERSION_MIN_MACOS
  cgm_cmd_ver_min_t *cmd_ver_min_macos = malloc(sizeof(cgm_cmd_ver_min_t));

  cmd_ver_min_macos->cmd = CGM_CMD_VERSION_MIN_MACOS;
  cmd_ver_min_macos->size = sizeof(cgm_cmd_ver_min_t);
  cmd_ver_min_macos->version = 0x000A0A00; // TODO in nibbles
  cmd_ver_min_macos->sdk_version = 0x000B0000; // TODO in nibbles

  duc_array_push(cgm->cmds, cmd_ver_min_macos);

  // CMD: UUID TODO

  // HEADER
  cgm->header.magic = CGM_MAGIC;
  cgm->header.cpu_type = CGM_CPU_TYPE_X86_64;
  cgm->header.cpu_subtype = CGM_CPU_SUBTYPE_X86_64;
  cgm->header.file_type = CGM_FILE_TYPE_EXECUTE;
  cgm->header.cmds_count = (uint32_t) duc_array_length(cgm->cmds);
  cgm->header.cmds_size = 0;

  for (size_t i = 0, size = duc_array_length(cgm->cmds); i < size; i++) {
    cgm->header.cmds_size += ((cgm_cmd_t *) duc_array_at(cgm->cmds, i))->size;
  }

  cgm->header.flags = CGM_FLAG_NOUNDEFS | CGM_FLAG_DYLDLINK | CGM_FLAG_TWOLEVEL;
  cgm->header.reserved = 0;

  // Calculations TODO Optimize and simplify calculations
  sym1->value = CGM_DATA_INMEM_OFFSET - duc_binary_size(cgm->sec_text);

  cmd_seg_pagezero->vm_address = CGM_PAGEZERO_INMEM_OFFSET;
  cmd_seg_pagezero->vm_size = CGM_PAGEZERO_INMEM_SIZE;
  cmd_seg_pagezero->file_offset = 0;
  cmd_seg_pagezero->file_size = 0;

  cmd_seg_text->vm_address = 0x0000000100000000;
  cmd_seg_text->vm_size = CGM_DATA_INMEM_SIZE;
  cmd_seg_text->file_offset = 0;
  cmd_seg_text->file_size = CGM_DATA_INFILE_SIZE;

  cmd_seg_text_text->address = CGM_DATA_INMEM_OFFSET - duc_binary_size(cgm->sec_text);
  cmd_seg_text_text->size = duc_binary_size(cgm->sec_text);
  cmd_seg_text_text->file_offset = (uint32_t) (CGM_DATA_INFILE_OFFSET - duc_binary_size(cgm->sec_text));

  cmd_seg_data->vm_address = CGM_DATA_INMEM_OFFSET;
  cmd_seg_data->vm_size = CGM_DATA_INMEM_SIZE;
  cmd_seg_data->file_offset = CGM_DATA_INFILE_OFFSET;
  cmd_seg_data->file_size = CGM_DATA_INFILE_SIZE;

  cmd_seg_data_data->address = CGM_DATA_INMEM_OFFSET;
  cmd_seg_data_data->size = duc_binary_size(cgm->sec_data);
  cmd_seg_data_data->file_offset = CGM_DATA_INFILE_OFFSET;

  cmd_seg_linkedit->vm_address = CGM_LINKEDIT_INMEM_OFFSET;
  cmd_seg_linkedit->vm_size = CGM_LINKEDIT_INMEM_SIZE;
  cmd_seg_linkedit->file_offset = CGM_LINKEDIT_INFILE_OFFSET;
  cmd_seg_linkedit->file_size = duc_binary_size(cgm->dyld_info) +
    duc_array_length(cgm->syms) * sizeof(cgm_sym_t) +
    duc_binary_size(cgm->strs);

  cmd_dyld_info->export_offset = CGM_LINKEDIT_INFILE_OFFSET;
  cmd_dyld_info->export_size = (uint32_t) duc_binary_size(cgm->dyld_info);

  cmd_symtab->sym_offset = (uint32_t) (CGM_LINKEDIT_INFILE_OFFSET + duc_binary_size(cgm->dyld_info));
  cmd_symtab->sym_count = (uint32_t) duc_array_length(cgm->syms);
  cmd_symtab->str_offset = cmd_symtab->sym_offset + cmd_symtab->sym_count * sizeof(cgm_sym_t);
  cmd_symtab->str_size = (uint32_t) duc_binary_size(cgm->strs);

  cmd_dysymtab->extdef_idx = 0;
  cmd_dysymtab->extdef_count = (uint32_t) (duc_array_length(cgm->syms) - 1);
  cmd_dysymtab->undef_idx = (uint32_t) (duc_array_length(cgm->syms) - 1);
  cmd_dysymtab->undef_count = 1;

  cmd_main->entry_offset = CGM_DATA_INFILE_OFFSET - duc_binary_size(cgm->sec_text);
  cmd_main->stack_size = 0;

  return cgm;
}

cgm_sect_t *cgm_sect (cgm_cmd_seg_t **cmd_seg, const char *sect_name, const char *seg_name) {
  uint32_t offset = (*cmd_seg)->size;
  (*cmd_seg)->size += sizeof(cgm_sect_t);
  (*cmd_seg)->sects_count += 1;

  *cmd_seg = realloc(*cmd_seg, (*cmd_seg)->size);
  cgm_sect_t *sect = (void *) ((uint8_t *) *cmd_seg + offset);

  strcpy(sect->sect_name, sect_name);
  strcpy(sect->seg_name, seg_name);
  sect->reloc_offset = 0;
  sect->reloc_count = 0;
  sect->reserved1 = 0;
  sect->reserved2 = 0;
  sect->reserved3 = 0;

  return sect;
}

void cgm_str (cgm_cmd_t **cmd, cgm_str_t *str, const char *data) {
  size_t len = strlen(data);
  str->offset = (*cmd)->size;
  (*cmd)->size += len;

  if ((*cmd)->size % 8 != 0) {
    (*cmd)->size += 8 - ((*cmd)->size % 8);
  }

  *cmd = realloc(*cmd, (*cmd)->size);
  memcpy((uint8_t *) *cmd + str->offset, data, len);
}
