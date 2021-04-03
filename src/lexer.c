/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include <string.h>
#include "lexer.h"

void lexer_free (lexer_t *lexer) {
  free(lexer->raw);
  free(lexer->str);
  free(lexer);
}

void lexer_free_cb (void *it) {
  lexer_free((lexer_t *) it);
}

lexer_t *lexer_new (duc_file_t *file) {
  lexer_t *lexer = malloc(sizeof(lexer_t));

  lexer->raw = NULL;
  lexer->str = NULL;
  lexer->token = LEXER_UNKNOWN;

  size_t pos = duc_file_position(file);

  if (
    lexer_is_ws_(file, lexer, pos) ||
    lexer_is_bracket_(file, lexer, pos) ||
    lexer_is_litstr_(file, lexer, pos) ||
    lexer_is_mark_(file, lexer, pos) ||
    lexer_is_id_(file, lexer, pos)
  ) {
    return lexer;
  }

  free(lexer);
  return NULL;
}

bool lexer_is_bracket_ (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch = duc_file_readchar(file);

  switch (ch) {
    case '(': {
      lexer->token = LEXER_LPAR;
      break;
    }
    case ')': {
      lexer->token = LEXER_RPAR;
      break;
    }
    default: {
      duc_file_seek(file, pos);
      return false;
    }
  }

  lexer->raw = malloc(2);
  lexer->str = malloc(2);
  lexer->raw[0] = ch;
  lexer->raw[1] = '\0';
  lexer->str[0] = ch;
  lexer->str[1] = '\0';

  return true;
}

bool lexer_is_id_ (duc_file_t *file, lexer_t *lexer, size_t pos) {
  const char *chs_begin = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz_$";
  const char *chs_end = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz0123456789_$";

  unsigned char ch = duc_file_readchar(file);

  if (strchr(chs_begin, ch) != NULL) {
    lexer->token = LEXER_ID;
  } else {
    duc_file_seek(file, pos);
    return false;
  }

  size_t len = 1;
  lexer->raw = malloc(len + 1);
  lexer->raw[len - 1] = ch;
  lexer->raw[len] = '\0';

  while (!duc_file_eof(file)) {
    size_t bu_pos = duc_file_position(file);
    ch = duc_file_readchar(file);

    if (strchr(chs_end, ch) == NULL) {
      duc_file_seek(file, bu_pos);
      break;
    }

    lexer->raw = realloc(lexer->raw, ++len + 1);
    lexer->raw[len - 1] = ch;
    lexer->raw[len] = '\0';
  }

  lexer->str = malloc(len + 1);
  memcpy(lexer->str, lexer->raw, len + 1);

  return true;
}

bool lexer_is_litstr_ (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch = duc_file_readchar(file);

  if ((ch != '\'' && ch != '"') || duc_file_eof(file)) {
    duc_file_seek(file, pos);
    return false;
  } else if (ch == '"') {
    lexer->token = LEXER_LITSTR_DQ;
  } else {
    lexer->token = LEXER_LITSTR_SQ;
  }

  size_t len = 1;
  lexer->raw = malloc(len + 1);
  lexer->raw[len - 1] = ch;
  lexer->raw[len] = '\0';

  while (true) {
    ch = duc_file_readchar(file);

    lexer->raw = realloc(lexer->raw, ++len + 1);
    lexer->raw[len - 1] = ch;
    lexer->raw[len] = '\0';

    bool dq_end = lexer->token == LEXER_LITSTR_DQ && ch == '"';
    bool sq_end = lexer->token == LEXER_LITSTR_SQ && ch == '\'';

    if (dq_end || sq_end) {
      break;
    } else if (duc_file_eof(file)) {
      lexer->token = LEXER_UNKNOWN;
      free(lexer->raw);
      lexer->raw = NULL;
      duc_file_seek(file, pos);

      return false;
    }
  }

  lexer->str = malloc(len - 1);
  memcpy(lexer->str, lexer->raw + 1, len - 2);
  lexer->str[len - 2] = '\0';

  return true;
}

bool lexer_is_mark_ (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch = duc_file_readchar(file);

  switch (ch) {
    case ',': {
      lexer->token = LEXER_COMMA;
      break;
    }
    default: {
      duc_file_seek(file, pos);
      return false;
    }
  }

  lexer->raw = malloc(2);
  lexer->str = malloc(2);
  lexer->raw[0] = ch;
  lexer->raw[1] = '\0';
  lexer->str[0] = ch;
  lexer->str[1] = '\0';

  return true;
}

bool lexer_is_ws_ (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch = duc_file_readchar(file);

  if (ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t') {
    lexer->token = LEXER_WS;
  } else {
    duc_file_seek(file, pos);
    return false;
  }

  size_t len = 1;
  lexer->raw = malloc(len + 1);
  lexer->raw[len - 1] = ch;
  lexer->raw[len] = '\0';

  while (!duc_file_eof(file)) {
    size_t bu_pos = duc_file_position(file);
    ch = duc_file_readchar(file);

    if (ch != ' ' && ch != '\r' && ch != '\n' && ch != '\t') {
      duc_file_seek(file, bu_pos);
      break;
    }

    lexer->raw = realloc(lexer->raw, ++len + 1);
    lexer->raw[len - 1] = ch;
    lexer->raw[len] = '\0';
  }

  lexer->str = malloc(len + 1);
  memcpy(lexer->str, lexer->raw, len + 1);

  return true;
}
