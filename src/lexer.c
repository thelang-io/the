#include <duc/common.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

void lexer_free (lexer_t *lexer) {
  free(lexer->raw);
  free(lexer->str);
  free(lexer);
}

bool lexer_is_bracket (duc_file_t *file, lexer_t *lexer, long long pos) {
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

  lexer->raw = malloc(1);
  lexer->str = malloc(1);
  lexer->raw[0] = ch;
  lexer->str[0] = ch;

  return true;
}

bool lexer_is_id (duc_file_t *file, lexer_t *lexer, long long pos) {
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
    long long cur_pos = duc_file_position(file);
    ch = duc_file_readchar(file);

    if (strchr(chs_end, ch) == NULL) {
      duc_file_seek(file, cur_pos);
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

bool lexer_is_litstr (duc_file_t *file, lexer_t *lexer, long long pos) {
  unsigned char ch = duc_file_readchar(file);

  if (ch != '\'' && ch != '"') {
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

  if (duc_file_eof(file)) {
    duc_throw("Unexpected end of file, expected end of string");
  }

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
      duc_throw("Unexpected end of file, expected end of string");
    }
  }

  lexer->str = malloc(len - 1);
  memcpy(lexer->str, lexer->raw + 1, len - 2);
  lexer->str[len - 2] = '\0';

  return true;
}

bool lexer_is_ws (duc_file_t *file, lexer_t *lexer, long long pos) {
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
    long long cur_pos = duc_file_position(file);
    ch = duc_file_readchar(file);

    if (ch != ' ' && ch != '\r' && ch != '\n' && ch != '\t') {
      duc_file_seek(file, cur_pos);
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

lexer_t *lexer_new (duc_file_t *file) {
  lexer_t *lexer = malloc(sizeof(lexer_t));

  lexer->raw = NULL;
  lexer->str = NULL;
  lexer->token = LEXER_UNKNOWN;

  long long pos = duc_file_position(file);

  if (
    lexer_is_ws(file, lexer, pos) ||
    lexer_is_bracket(file, lexer, pos) ||
    lexer_is_litstr(file, lexer, pos) ||
    lexer_is_id(file, lexer, pos)
  ) {
    return lexer;
  }

  free(lexer);
  return NULL;
}

char *lexer_token_to_string (lexer_token token) {
  switch (token) {
    case LEXER_ID: {
      return "Identifier";
    }
    case LEXER_LITSTR_DQ: {
      return "Double-Quoted String Literal";
    }
    case LEXER_LITSTR_SQ: {
      return "Single-Quoted String Literal";
    }
    case LEXER_LPAR: {
      return "Left Parenthesis";
    }
    case LEXER_RPAR: {
      return "Right Parenthesis";
    }
    case LEXER_WS: {
      return "Whitespace";
    }
    default: {
      return "Unknown";
    }
  }
}
