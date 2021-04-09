/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include <string.h>
#include "codegen-macos.h"
#include "version.h"

duc_binary_t *codegen_macos (DUC_UNUSED const ast_t *ast) {
  cgm_t *cgm = cgm_new_();

  duc_binary_append_uint8(cgm->sec_data, 'H', 'e', 'l', 'l', 'o', ',', ' ');
  duc_binary_append_uint8(cgm->sec_data, 'W', 'o', 'r', 'l', 'd', '!', '\n');

  // TODO Instructions to macros
  duc_binary_append_uint8(cgm->sec_text, 0xE8); // call
  duc_binary_append_uint32(cgm->sec_text, 0x01);
  duc_binary_append_uint8(cgm->sec_text, 0xC3); // ret

  duc_binary_append_uint8(cgm->sec_text, 0xB8); // mov eax
  duc_binary_append_uint32(cgm->sec_text, 0x02000004);
  duc_binary_append_uint8(cgm->sec_text, 0xBF); // mov edi
  duc_binary_append_uint32(cgm->sec_text, 0x01);
  duc_binary_append_uint16(cgm->sec_text, 0xBE48); // movabsq rsi
  duc_binary_append_uint64(cgm->sec_text, 0x0000000100004000);
  duc_binary_append_uint8(cgm->sec_text, 0xBA); // mov edx
  duc_binary_append_uint32(cgm->sec_text, 0x0E);
  duc_binary_append_uint16(cgm->sec_text, 0x050F); // syscall
  duc_binary_append_uint8(cgm->sec_text, 0xC3); // ret

  // TODO Move to some kind of container
  duc_binary_append_string(cgm->strs, " ");
  duc_binary_append_string(cgm->strs, "_main");
  duc_binary_append_string(cgm->strs, "puts");

  size_t carry = duc_binary_size(cgm->strs) % 8;
  duc_binary_append_times(cgm->strs, 0x00, carry == 0 ? 0 : 8 - carry);

  // TODO Move to calc / init
  cgm_sym_t *sym0 = malloc(sizeof(cgm_sym_t));
  sym0->strtab_idx = 0x02; // TODO
  sym0->type = CGM_SYM_TYPE_EXT | CGM_SYM_TYPE_SECT;
  sym0->sect_idx = 1;
  sym0->description = 0;
  sym0->value = CGM_DATA_INMEM_OFFSET - duc_binary_size(cgm->sec_text); // TODO

  duc_array_push(cgm->syms, sym0);

  cgm_sym_t *sym1 = malloc(sizeof(cgm_sym_t));
  sym1->strtab_idx = 0x08; // TODO
  sym1->type = CGM_SYM_TYPE_EXT | CGM_SYM_TYPE_SECT;
  sym1->sect_idx = 1;
  sym1->description = 0;
  sym1->value = CGM_DATA_INMEM_OFFSET - duc_binary_size(cgm->sec_text) + 0x06; // TODO

  duc_array_push(cgm->syms, sym1);

  cgm_init_dyld_info_export_(cgm);
  cgm_init_header_(cgm);
  cgm_init_cmd_dyld_info_(cgm);
  cgm_init_cmd_dylib_(cgm);
  cgm_init_cmd_dylinker_(cgm);
  cgm_init_cmd_dysymtab_(cgm);
  cgm_init_cmd_seg_data_(cgm);
  cgm_init_cmd_seg_linkedit_(cgm);
  cgm_init_cmd_seg_pagezero_(cgm);
  cgm_init_cmd_seg_text_(cgm);
  cgm_init_cmd_main_(cgm);
  cgm_init_cmd_src_ver_(cgm);
  cgm_init_cmd_symtab_(cgm);
  cgm_init_cmd_ver_min_macos_(cgm);
  cgm_init_cmd_uuid_(cgm);

  cgm_calc_header_(cgm);
  cgm_calc_cmd_dyld_info_(cgm);
  cgm_calc_cmd_dylib_(cgm);
  cgm_calc_cmd_dylinker_(cgm);
  cgm_calc_cmd_dysymtab_(cgm);
  cgm_calc_cmd_seg_data_(cgm);
  cgm_calc_cmd_seg_linkedit_(cgm);
  cgm_calc_cmd_seg_pagezero_(cgm);
  cgm_calc_cmd_seg_text_(cgm);
  cgm_calc_cmd_main_(cgm);
  cgm_calc_cmd_src_ver_(cgm);
  cgm_calc_cmd_symtab_(cgm);
  cgm_calc_cmd_ver_min_macos_(cgm);
  cgm_calc_cmd_uuid_(cgm);

  cgm_calc_dyld_info_export_(cgm);

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
  duc_binary_append_data(bin, &cgm->dyld_info_export, sizeof(cgm_dyld_info_export_t));

  for (size_t i = 0, size = duc_array_length(cgm->syms); i < size; i++) {
    cgm_sym_t *sym = duc_array_at(cgm->syms, i);
    duc_binary_append_data(bin, sym, sizeof(cgm_sym_t));
  }

  duc_binary_append_binary(bin, cgm->strs);
  cgm_free_(cgm);

  return bin;
}

// TODO Optimize calculations

void cgm_calc_cmd_dyld_info_ (cgm_t *cgm) {
  cgm->cmd_dyld_info->export_offset = CGM_LINKEDIT_INFILE_OFFSET;
  cgm->cmd_dyld_info->export_size = sizeof(cgm_dyld_info_export_t);
}

void cgm_calc_cmd_dylib_ (DUC_UNUSED cgm_t *cgm) {
}

void cgm_calc_cmd_dylinker_ (DUC_UNUSED cgm_t *cgm) {
}

void cgm_calc_cmd_dysymtab_ (cgm_t *cgm) {
  cgm->cmd_dysymtab->extdef_idx = 0;
  cgm->cmd_dysymtab->extdef_count = (uint32_t) duc_array_length(cgm->syms);
}

void cgm_calc_cmd_main_ (cgm_t *cgm) {
  cgm->cmd_main->entry_offset = CGM_DATA_INFILE_OFFSET - duc_binary_size(cgm->sec_text);
}

void cgm_calc_cmd_seg_data_ (cgm_t *cgm) {
  cgm->cmd_seg_data->vm_address = CGM_DATA_INMEM_OFFSET;
  cgm->cmd_seg_data->vm_size = CGM_DATA_INMEM_SIZE;
  cgm->cmd_seg_data->file_offset = CGM_DATA_INFILE_OFFSET;
  cgm->cmd_seg_data->file_size = CGM_DATA_INFILE_SIZE;

  cgm->cmd_seg_data_data->vm_address = CGM_DATA_INMEM_OFFSET;
  cgm->cmd_seg_data_data->vm_size = duc_binary_size(cgm->sec_data);
  cgm->cmd_seg_data_data->file_offset = CGM_DATA_INFILE_OFFSET;
}

void cgm_calc_cmd_seg_linkedit_ (cgm_t *cgm) {
  cgm->cmd_seg_linkedit->vm_address = CGM_LINKEDIT_INMEM_OFFSET;
  cgm->cmd_seg_linkedit->vm_size = CGM_LINKEDIT_INMEM_SIZE;
  cgm->cmd_seg_linkedit->file_offset = CGM_LINKEDIT_INFILE_OFFSET;
  cgm->cmd_seg_linkedit->file_size = sizeof(cgm_dyld_info_export_t) +
    duc_array_length(cgm->syms) * sizeof(cgm_sym_t) +
    duc_binary_size(cgm->strs);
}

void cgm_calc_cmd_seg_pagezero_ (cgm_t *cgm) {
  cgm->cmd_seg_pagezero->vm_address = CGM_PAGEZERO_INMEM_OFFSET;
  cgm->cmd_seg_pagezero->vm_size = CGM_PAGEZERO_INMEM_SIZE;
  cgm->cmd_seg_pagezero->file_offset = 0;
  cgm->cmd_seg_pagezero->file_size = 0;
}

void cgm_calc_cmd_seg_text_ (cgm_t *cgm) {
  cgm->cmd_seg_text->vm_address = CGM_PAGEZERO_INMEM_OFFSET + CGM_PAGEZERO_INMEM_SIZE;
  cgm->cmd_seg_text->vm_size = CGM_DATA_INMEM_SIZE;
  cgm->cmd_seg_text->file_offset = 0;
  cgm->cmd_seg_text->file_size = CGM_DATA_INFILE_SIZE;

  cgm->cmd_seg_text_text->vm_address = CGM_DATA_INMEM_OFFSET - duc_binary_size(cgm->sec_text);
  cgm->cmd_seg_text_text->vm_size = duc_binary_size(cgm->sec_text);
  cgm->cmd_seg_text_text->file_offset = (uint32_t) (CGM_DATA_INFILE_OFFSET - duc_binary_size(cgm->sec_text));
}

void cgm_calc_cmd_src_ver_ (DUC_UNUSED cgm_t *cgm) {
}

void cgm_calc_cmd_symtab_ (cgm_t *cgm) {
  cgm->cmd_symtab->sym_offset = CGM_LINKEDIT_INFILE_OFFSET + sizeof(cgm_dyld_info_export_t);
  cgm->cmd_symtab->sym_count = (uint32_t) duc_array_length(cgm->syms);
  cgm->cmd_symtab->str_offset = cgm->cmd_symtab->sym_offset +
    cgm->cmd_symtab->sym_count * (uint32_t) sizeof(cgm_sym_t);
  cgm->cmd_symtab->str_size = (uint32_t) duc_binary_size(cgm->strs);
}

void cgm_calc_cmd_ver_min_macos_ (DUC_UNUSED cgm_t *cgm) {
}

void cgm_calc_cmd_uuid_ (DUC_UNUSED cgm_t *cgm) {
}

void cgm_calc_dyld_info_export_ (cgm_t *cgm) {
  cgm->dyld_info_export.root_next = 0x17;
}

void cgm_calc_header_ (cgm_t *cgm) {
  cgm->header.cmds_count = (uint32_t) duc_array_length(cgm->cmds);

  for (size_t i = 0, size = duc_array_length(cgm->cmds); i < size; i++) {
    cgm->header.cmds_size += ((cgm_cmd_t *) duc_array_at(cgm->cmds, i))->size;
  }
}

void *cgm_cmd_ (uint32_t id, size_t size) {
  cgm_cmd_t *cmd = malloc(size);
  cmd->cmd = id;
  cmd->size = (uint32_t) size;
  return cmd;
}

void cgm_free_ (cgm_t *cgm) {
  duc_array_free(cgm->cmds, duc_free_simple);
  duc_binary_free(cgm->sec_data);
  duc_binary_free(cgm->sec_text);
  duc_binary_free(cgm->strs);
  duc_array_free(cgm->syms, duc_free_simple);
  free(cgm);
}

void cgm_init_cmd_dyld_info_ (cgm_t *cgm) {
  cgm->cmd_dyld_info = cgm_cmd_(CGM_CMD_DYLD_INFO_ONLY, sizeof(cgm_cmd_dyld_info_t));
  cgm->cmd_dyld_info->rebase_offset = 0;
  cgm->cmd_dyld_info->rebase_size = 0;
  cgm->cmd_dyld_info->bind_offset = 0;
  cgm->cmd_dyld_info->bind_size = 0;
  cgm->cmd_dyld_info->weak_bind_offset = 0;
  cgm->cmd_dyld_info->weak_bind_size = 0;
  cgm->cmd_dyld_info->lazy_bind_offset = 0;
  cgm->cmd_dyld_info->lazy_bind_size = 0;
  cgm->cmd_dyld_info->export_offset = 0;
  cgm->cmd_dyld_info->export_size = 0;

  duc_array_push(cgm->cmds, cgm->cmd_dyld_info);
}

void cgm_init_cmd_dylib_ (cgm_t *cgm) {
  cgm->cmd_dylib = cgm_cmd_(CGM_CMD_LOAD_DYLIB, sizeof(cgm_cmd_dylib_t));
  cgm->cmd_dylib->dylib.timestamp = 0;
  cgm->cmd_dylib->dylib.current_version = cgm_ver32_("1292.60.1");
  cgm->cmd_dylib->dylib.compatibility_version = cgm_ver32_("1.0.0");
  cgm_str_((cgm_cmd_t **) &cgm->cmd_dylib, &cgm->cmd_dylib->dylib.name, "/usr/lib/libSystem.B.dylib");

  duc_array_push(cgm->cmds, cgm->cmd_dylib);
}

void cgm_init_cmd_dylinker_ (cgm_t *cgm) {
  cgm->cmd_dylinker = cgm_cmd_(CGM_CMD_LOAD_DYLINKER, sizeof(cgm_cmd_dylinker_t));
  cgm_str_((cgm_cmd_t **) &cgm->cmd_dylinker, &cgm->cmd_dylinker->name, "/usr/lib/dyld");

  duc_array_push(cgm->cmds, cgm->cmd_dylinker);
}

void cgm_init_cmd_dysymtab_ (cgm_t *cgm) {
  cgm->cmd_dysymtab = cgm_cmd_(CGM_CMD_DYSYMTAB, sizeof(cgm_cmd_dysymtab_t));
  cgm->cmd_dysymtab->loc_idx = 0;
  cgm->cmd_dysymtab->loc_count = 0;
  cgm->cmd_dysymtab->extdef_idx = 0;
  cgm->cmd_dysymtab->extdef_count = 0;
  cgm->cmd_dysymtab->undef_idx = 0;
  cgm->cmd_dysymtab->undef_count = 0;
  cgm->cmd_dysymtab->contents_offset = 0;
  cgm->cmd_dysymtab->contents_count = 0;
  cgm->cmd_dysymtab->module_offset = 0;
  cgm->cmd_dysymtab->module_count = 0;
  cgm->cmd_dysymtab->extref_offset = 0;
  cgm->cmd_dysymtab->extref_count = 0;
  cgm->cmd_dysymtab->indirect_offset = 0;
  cgm->cmd_dysymtab->indirect_count = 0;
  cgm->cmd_dysymtab->extrel_offset = 0;
  cgm->cmd_dysymtab->extrel_count = 0;
  cgm->cmd_dysymtab->locrel_offset = 0;
  cgm->cmd_dysymtab->locrel_count = 0;

  duc_array_push(cgm->cmds, cgm->cmd_dysymtab);
}

void cgm_init_cmd_main_ (cgm_t *cgm) {
  cgm->cmd_main = cgm_cmd_(CGM_CMD_MAIN, sizeof(cgm_cmd_main_t));
  cgm->cmd_main->entry_offset = 0;
  cgm->cmd_main->stack_size = 0;

  duc_array_push(cgm->cmds, cgm->cmd_main);
}

void cgm_init_cmd_seg_data_ (cgm_t *cgm) {
  cgm->cmd_seg_data = cgm_cmd_(CGM_CMD_SEGMENT, sizeof(cgm_cmd_seg_t));
  strcpy(cgm->cmd_seg_data->name, CGM_SEG_DATA);
  cgm->cmd_seg_data->vm_address = 0;
  cgm->cmd_seg_data->vm_size = 0;
  cgm->cmd_seg_data->file_offset = 0;
  cgm->cmd_seg_data->file_size = 0;
  cgm->cmd_seg_data->prot_max = CGM_PROT_DEFAULT;
  cgm->cmd_seg_data->prot_initial = CGM_PROT_DEFAULT;
  cgm->cmd_seg_data->sects_count = 0;
  cgm->cmd_seg_data->flags = 0;

  cgm->cmd_seg_data_data = cgm_sect_(&cgm->cmd_seg_data, CGM_SECT_DATA, CGM_SEG_DATA);
  cgm->cmd_seg_data_data->vm_address = 0;
  cgm->cmd_seg_data_data->vm_size = 0;
  cgm->cmd_seg_data_data->file_offset = 0;
  cgm->cmd_seg_data_data->align = 0;
  cgm->cmd_seg_data_data->reloc_offset = 0;
  cgm->cmd_seg_data_data->reloc_count = 0;
  cgm->cmd_seg_data_data->flags = CGM_SECT_FLAG_REGULAR;
  cgm->cmd_seg_data_data->reserved1 = 0;
  cgm->cmd_seg_data_data->reserved2 = 0;
  cgm->cmd_seg_data_data->reserved3 = 0;

  duc_array_push(cgm->cmds, cgm->cmd_seg_data);
}

void cgm_init_cmd_seg_linkedit_ (cgm_t *cgm) {
  cgm->cmd_seg_linkedit = cgm_cmd_(CGM_CMD_SEGMENT, sizeof(cgm_cmd_seg_t));
  strcpy(cgm->cmd_seg_linkedit->name, CGM_SEG_LINKEDIT);
  cgm->cmd_seg_linkedit->vm_address = 0;
  cgm->cmd_seg_linkedit->vm_size = 0;
  cgm->cmd_seg_linkedit->file_offset = 0;
  cgm->cmd_seg_linkedit->file_size = 0;
  cgm->cmd_seg_linkedit->prot_max = CGM_PROT_READ;
  cgm->cmd_seg_linkedit->prot_initial = CGM_PROT_READ;
  cgm->cmd_seg_linkedit->sects_count = 0;
  cgm->cmd_seg_linkedit->flags = 0;

  duc_array_push(cgm->cmds, cgm->cmd_seg_linkedit);
}

void cgm_init_cmd_seg_pagezero_ (cgm_t *cgm) {
  cgm->cmd_seg_pagezero = cgm_cmd_(CGM_CMD_SEGMENT, sizeof(cgm_cmd_seg_t));
  strcpy(cgm->cmd_seg_pagezero->name, CGM_SEG_PAGEZERO);
  cgm->cmd_seg_pagezero->vm_address = 0;
  cgm->cmd_seg_pagezero->vm_size = 0;
  cgm->cmd_seg_pagezero->file_offset = 0;
  cgm->cmd_seg_pagezero->file_size = 0;
  cgm->cmd_seg_pagezero->prot_max = CGM_PROT_NONE;
  cgm->cmd_seg_pagezero->prot_initial = CGM_PROT_NONE;
  cgm->cmd_seg_pagezero->sects_count = 0;
  cgm->cmd_seg_pagezero->flags = 0;

  duc_array_push(cgm->cmds, cgm->cmd_seg_pagezero);
}

void cgm_init_cmd_seg_text_ (cgm_t *cgm) {
  cgm->cmd_seg_text = cgm_cmd_(CGM_CMD_SEGMENT, sizeof(cgm_cmd_seg_t));
  strcpy(cgm->cmd_seg_text->name, CGM_SEG_TEXT);
  cgm->cmd_seg_text->vm_address = 0;
  cgm->cmd_seg_text->vm_size = 0;
  cgm->cmd_seg_text->file_offset = 0;
  cgm->cmd_seg_text->file_size = 0;
  cgm->cmd_seg_text->prot_max = CGM_PROT_READ | CGM_PROT_EXECUTE;
  cgm->cmd_seg_text->prot_initial = CGM_PROT_READ | CGM_PROT_EXECUTE;
  cgm->cmd_seg_text->sects_count = 0;
  cgm->cmd_seg_text->flags = 0;

  cgm->cmd_seg_text_text = cgm_sect_(&cgm->cmd_seg_text, CGM_SECT_TEXT, CGM_SEG_TEXT);
  cgm->cmd_seg_text_text->vm_address = 0;
  cgm->cmd_seg_text_text->vm_size = 0;
  cgm->cmd_seg_text_text->file_offset = 0;
  cgm->cmd_seg_text_text->align = 0;
  cgm->cmd_seg_text_text->reloc_offset = 0;
  cgm->cmd_seg_text_text->reloc_count = 0;
  cgm->cmd_seg_text_text->flags = CGM_SECT_ATTR_PURE_INSTRUCTIONS | CGM_SECT_ATTR_SOME_INSTRUCTIONS;
  cgm->cmd_seg_text_text->reserved1 = 0;
  cgm->cmd_seg_text_text->reserved2 = 0;
  cgm->cmd_seg_text_text->reserved3 = 0;

  duc_array_push(cgm->cmds, cgm->cmd_seg_text);
}

void cgm_init_cmd_src_ver_ (cgm_t *cgm) {
  cgm->cmd_src_ver = cgm_cmd_(CGM_CMD_SOURCE_VERSION, sizeof(cgm_cmd_src_ver_t));
  cgm->cmd_src_ver->version = cgm_ver64_(VERSION);

  duc_array_push(cgm->cmds, cgm->cmd_src_ver);
}

void cgm_init_cmd_symtab_ (cgm_t *cgm) {
  cgm->cmd_symtab = cgm_cmd_(CGM_CMD_SYMTAB, sizeof(cgm_cmd_symtab_t));
  cgm->cmd_symtab->sym_offset = 0;
  cgm->cmd_symtab->sym_count = 0;
  cgm->cmd_symtab->str_offset = 0;
  cgm->cmd_symtab->str_size = 0;

  duc_array_push(cgm->cmds, cgm->cmd_symtab);
}

void cgm_init_cmd_ver_min_macos_ (cgm_t *cgm) {
  cgm->cmd_ver_min_macos = cgm_cmd_(CGM_CMD_VERSION_MIN_MACOS, sizeof(cgm_cmd_ver_min_t));
  cgm->cmd_ver_min_macos->version = cgm_ver32_("10.10");
  cgm->cmd_ver_min_macos->sdk_version = cgm_ver32_("10.16");

  duc_array_push(cgm->cmds, cgm->cmd_ver_min_macos);
}

void cgm_init_cmd_uuid_ (cgm_t *cgm) {
  cgm->cmd_uuid = cgm_cmd_(CGM_CMD_UUID, sizeof(cgm_cmd_uuid_t));
  cgm->cmd_uuid->uuid[0] = 0;

  duc_array_push(cgm->cmds, cgm->cmd_uuid);
}

void cgm_init_dyld_info_export_ (cgm_t *cgm) {
  cgm->dyld_info_export.root_terminal_size = 0;
  cgm->dyld_info_export.root_child_count = 1;
  strcpy(cgm->dyld_info_export.root_label, "__mh_execute_header");
  cgm->dyld_info_export.root_next = 0;
  cgm->dyld_info_export.child_terminal_size = 2;
  cgm->dyld_info_export.child_flags = CGM_DYLD_INFO_FLAG_REGULAR;
  cgm->dyld_info_export.child_sym_offset = 0;
  cgm->dyld_info_export.child_child_count = 0;
  cgm->dyld_info_export.reserved = 0;
}

void cgm_init_header_ (cgm_t *cgm) {
  cgm->header.magic = CGM_MAGIC;
  cgm->header.cpu_type = CGM_CPU_TYPE_X86_64;
  cgm->header.cpu_subtype = CGM_CPU_SUBTYPE_X86_64;
  cgm->header.file_type = CGM_FILE_TYPE_EXECUTE;
  cgm->header.cmds_count = 0;
  cgm->header.cmds_size = 0;
  cgm->header.flags = CGM_FLAG_NOUNDEFS | CGM_FLAG_DYLDLINK | CGM_FLAG_TWOLEVEL;
  cgm->header.reserved = 0;
}

cgm_t *cgm_new_ () {
  cgm_t *cgm = malloc(sizeof(cgm_t));

  cgm->cmd_dyld_info = NULL;
  cgm->cmd_dylib = NULL;
  cgm->cmd_dylinker = NULL;
  cgm->cmd_dysymtab = NULL;
  cgm->cmd_main = NULL;
  cgm->cmd_seg_data = NULL;
  cgm->cmd_seg_data_data = NULL;
  cgm->cmd_seg_pagezero = NULL;
  cgm->cmd_seg_text = NULL;
  cgm->cmd_seg_text_text = NULL;
  cgm->cmd_seg_linkedit = NULL;
  cgm->cmd_src_ver = NULL;
  cgm->cmd_symtab = NULL;
  cgm->cmd_ver_min_macos = NULL;
  cgm->cmd_uuid = NULL;
  cgm->cmds = duc_array_new();
  cgm->sec_data = duc_binary_new();
  cgm->sec_text = duc_binary_new();
  cgm->strs = duc_binary_new();
  cgm->syms = duc_array_new();

  return cgm;
}

cgm_sect_t *cgm_sect_ (cgm_cmd_seg_t **cmd_seg, const char *sect_name, const char *seg_name) {
  uint32_t offset = (*cmd_seg)->size;
  (*cmd_seg)->size += (uint32_t) sizeof(cgm_sect_t);
  (*cmd_seg)->sects_count += 1;

  *cmd_seg = realloc(*cmd_seg, (*cmd_seg)->size);
  cgm_sect_t *sect = (void *) ((uint8_t *) *cmd_seg + offset);

  strcpy(sect->sect_name, sect_name);
  strcpy(sect->seg_name, seg_name);

  return sect;
}

void cgm_str_ (cgm_cmd_t **cmd, cgm_str_t *str, const char *data) {
  uint32_t len = (uint32_t) strlen(data);
  str->offset = (*cmd)->size;
  (*cmd)->size += len;

  if ((*cmd)->size % 8 != 0) {
    (*cmd)->size += 8 - ((*cmd)->size % 8);
  }

  *cmd = realloc(*cmd, (*cmd)->size);
  memcpy((uint8_t *) *cmd + str->offset, data, len);
}

uint32_t cgm_ver32_ (const char *ver) {
  uint16_t chunks[3] = { 0x00, 0x00, 0x00 };
  size_t idx = 0;

  for (size_t i = 0, size = strlen(ver); i < size; i++) {
    if (ver[i] == '.') {
      idx++;
    } else {
      chunks[idx] = (uint16_t) (chunks[idx] * 10 + (uint8_t) (ver[i] - '0'));
    }
  }

  return (uint32_t) ((chunks[0] << 16) + (chunks[1] << 8) + chunks[2]);
}

uint64_t cgm_ver64_ (const char *ver) {
  uint64_t chunks[5] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
  size_t idx = 0;

  for (size_t i = 0, size = strlen(ver); i < size; i++) {
    if (ver[i] == '.') {
      idx++;
    } else {
      chunks[idx] = (uint64_t) (chunks[idx] * 10 + (uint8_t) (ver[i] - '0'));
    }
  }

  return (uint64_t) ((chunks[0] << 40) + (chunks[1] << 30) + (chunks[2] << 20) + (chunks[3] << 10) + chunks[4]);
}

uint64_t cgm_uleb128_ (uint64_t value) {
  uint64_t result = 0;
  size_t bit = 0;

  do {
    uint8_t byte = value & 0x7F;
    value &= (uint64_t) ~0x7F;

    if (value != 0) {
      byte |= 0x80;
    }

    result |= (uint64_t) (byte << bit);
    value >>= 7;
    bit += 8;
  } while (value != 0);

  return result;
}
