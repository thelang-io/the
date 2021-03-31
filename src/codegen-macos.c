/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/helpers.h>
#include <stdlib.h>
#include <string.h>
#include "codegen-macos.h"

duc_binary_t *cgm (DUC_UNUSED const ast_t *ast) {
  cgm_t *cgm = malloc(sizeof(cgm_t));

  // CMD: SEGMENT (__PAGEZERO)
  cgm_cmd_seg_t *cmd_seg_pagezero = malloc(sizeof(cgm_cmd_seg_t));

  cmd_seg_pagezero->cmd = CGM_CMD_SEGMENT;
  cmd_seg_pagezero->size = sizeof(cgm_cmd_seg_t);
  strcpy(cmd_seg_pagezero->name, CGM_SEG_PAGEZERO);
  cmd_seg_pagezero->vm_address = 0;
  cmd_seg_pagezero->vm_size = 0; // TODO
  cmd_seg_pagezero->file_offset = 0; // TODO
  cmd_seg_pagezero->file_size = 0; // TODO
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
  cmd_seg_text->vm_address = 0; // TODO
  cmd_seg_text->vm_size = 0; // TODO
  cmd_seg_text->file_offset = 0; // TODO
  cmd_seg_text->file_size = 0; // TODO
  cmd_seg_text->prot_max = CGM_PROT_READ | CGM_PROT_EXECUTE;
  cmd_seg_text->prot_initial = CGM_PROT_READ | CGM_PROT_EXECUTE;
  cmd_seg_text->sects_count = 2; // TODO remove unwind_info
  cmd_seg_text->flags = 0;

  duc_array_push(cgm->cmds, cmd_seg_text);

  // CMD: SEGMENT (__LINKEDIT)
  cgm_cmd_seg_t *cmd_seg_linkedit = malloc(sizeof(cgm_cmd_seg_t));

  cmd_seg_linkedit->cmd = CGM_CMD_SEGMENT;
  cmd_seg_linkedit->size = sizeof(cgm_cmd_seg_t);
  strcpy(cmd_seg_linkedit->name, CGM_SEG_LINKEDIT);
  cmd_seg_linkedit->vm_address = 0; // TODO
  cmd_seg_linkedit->vm_size = 0; // TODO
  cmd_seg_linkedit->file_offset = 0; // TODO
  cmd_seg_linkedit->file_size = 0; // TODO
  cmd_seg_linkedit->prot_max = CGM_PROT_READ;
  cmd_seg_linkedit->prot_initial = CGM_PROT_READ;
  cmd_seg_linkedit->sects_count = 0;
  cmd_seg_linkedit->flags = 0;

  duc_array_push(cgm->cmds, cmd_seg_linkedit);

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
  cmd_dyld_info->export_offset = 0; // TODO
  cmd_dyld_info->export_size = 0; // TODO

  duc_array_push(cgm->cmds, cmd_dyld_info);

  // CMD: SYMTAB
  cgm_cmd_symtab_t *cmd_symtab = malloc(sizeof(cgm_cmd_symtab_t));

  cmd_symtab->cmd = CGM_CMD_SYMTAB;
  cmd_symtab->size = sizeof(cgm_cmd_symtab_t);
  cmd_symtab->sym_offset = 0; // TODO
  cmd_symtab->sym_count = 0; // TODO
  cmd_symtab->str_offset = 0; // TODO
  cmd_symtab->str_size = 0; // TODO

  duc_array_push(cgm->cmds, cmd_symtab);

  // CMD: DYSYMTAB
  cgm_cmd_dysymtab_t *cmd_dysymtab = malloc(sizeof(cgm_cmd_dysymtab_t));

  cmd_dysymtab->cmd = CGM_CMD_DYSYMTAB;
  cmd_dysymtab->size = sizeof(cgm_cmd_dysymtab_t);
  cmd_dysymtab->loc_idx = 0;
  cmd_dysymtab->loc_count = 0;
  cmd_dysymtab->extdef_idx = 0; // TODO
  cmd_dysymtab->extdef_count = 0; // TODO
  cmd_dysymtab->undef_idx = 0; // TODO
  cmd_dysymtab->undef_count = 0; // TODO
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

  // CMD: LOAD_DYLINKER
  cgm_cmd_dylinker_t *cmd_load_dylinker = malloc(sizeof(cgm_cmd_dylinker_t));

  cmd_load_dylinker->cmd = CGM_CMD_LOAD_DYLINKER;
  cmd_load_dylinker->size = sizeof(cgm_cmd_dylinker_t);
  cgm_str((cgm_cmd_t **) &cmd_load_dylinker, &cmd_load_dylinker->name, "/usr/lib/dyld");

  duc_array_push(cgm->cmds, cmd_load_dylinker);

  // CMD: VERSION_MIN_MACOS
  cgm_cmd_ver_min_t *cmd_ver_min_macos = malloc(sizeof(cgm_cmd_ver_min_t));

  cmd_ver_min_macos->cmd = CGM_CMD_VERSION_MIN_MACOS;
  cmd_ver_min_macos->size = sizeof(cgm_cmd_ver_min_t);
  cmd_ver_min_macos->version = 0; // TODO in nibbles
  cmd_ver_min_macos->sdk_version = 0; // TODO in nibbles

  duc_array_push(cgm->cmds, cmd_ver_min_macos);

  // CMD: SOURCE_VERSION
  cgm_cmd_src_ver_t *cmd_src_ver = malloc(sizeof(cgm_cmd_src_ver_t));

  cmd_src_ver->cmd = CGM_CMD_SOURCE_VERSION;
  cmd_src_ver->size = sizeof(cgm_cmd_src_ver_t);
  cmd_src_ver->version = 0; // TODO packed as a24.b10.c10.d10.e10

  duc_array_push(cgm->cmds, cmd_src_ver);

  // CMD: MAIN
  cgm_cmd_main_t *cmd_main = malloc(sizeof(cgm_cmd_main_t));

  cmd_main->cmd = CGM_CMD_MAIN;
  cmd_main->size = sizeof(cgm_cmd_main_t);
  cmd_main->entry_offset = 0; // TODO
  cmd_main->stack_size = 0;

  duc_array_push(cgm->cmds, cmd_main);

  // CMD: DYLIB (libSystem.B.dylib)
  cgm_cmd_dylib_t *cmd_dylib_system = malloc(sizeof(cgm_cmd_dylib_t));

  cmd_dylib_system->cmd = CGM_CMD_LOAD_DYLIB;
  cmd_dylib_system->size = sizeof(cgm_cmd_dylib_t);
  cmd_dylib_system->dylib.timestamp = 0x02;
  cmd_dylib_system->dylib.current_version = 0x0; // TODO
  cmd_dylib_system->dylib.compatibility_version = 0x010000;
  cgm_str((cgm_cmd_t **) &cmd_dylib_system, &cmd_dylib_system->dylib.name, "/usr/lib/libSystem.B.dylib");

  duc_array_push(cgm->cmds, cmd_dylib_system);

  // CMD: DATA_IN_CODE // TODO Remove
  cgm_cmd_linkedit_data_t *cmd_data_in_code = malloc(sizeof(cgm_cmd_linkedit_data_t));

  cmd_data_in_code->cmd = CGM_CMD_LOAD_DYLIB;
  cmd_data_in_code->size = sizeof(cgm_cmd_linkedit_data_t);
  cmd_data_in_code->data_offset = 0; // TODO
  cmd_data_in_code->data_size = 0;

  duc_array_push(cgm->cmds, cmd_data_in_code);

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

  free(cgm);
  duc_binary_t *bin = duc_binary_new();
  return bin;
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
