/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <duc/common.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

bool lexer_char_is_id (unsigned char ch) {
  const char *chs =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
  return strchr(chs, ch) != NULL;
}

bool lexer_char_is_id_start (unsigned char ch) {
  const char *chs = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";
  return strchr(chs, ch) != NULL;
}

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
    lexer_is_keyword_(file, lexer, pos) ||
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
    case '{': {
      lexer->token = LEXER_LBRACE;
      break;
    }
    case '[': {
      lexer->token = LEXER_LBRACK;
      break;
    }
    case '(': {
      lexer->token = LEXER_LPAR;
      break;
    }
    case '}': {
      lexer->token = LEXER_RBRACE;
      break;
    }
    case ']': {
      lexer->token = LEXER_RBRACK;
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
  memcpy(lexer->str, lexer->raw, 2);

  return true;
}

bool lexer_is_id_ (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch = duc_file_readchar(file);

  if (lexer_char_is_id_start(ch)) {
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

    if (!lexer_char_is_id(ch)) {
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

bool lexer_is_keyword_ (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch1 = duc_file_readchar(file);

  if (ch1 == 'f' && !duc_file_eof(file)) {
    unsigned char ch2 = duc_file_readchar(file);

    if (ch2 == 'n') {
      if (!duc_file_eof(file)) {
        size_t bu_pos = duc_file_position(file);
        unsigned char ch_id = duc_file_readchar(file);
        duc_file_seek(file, bu_pos);

        if (!lexer_char_is_id(ch_id)) {
          goto lexer_is_keyword_fn;
        }
      } else {
lexer_is_keyword_fn:
        lexer->raw = malloc(3);
        lexer->str = malloc(3);
        lexer->raw[0] = ch1;
        lexer->raw[1] = ch2;
        lexer->raw[2] = '\0';
        memcpy(lexer->str, lexer->raw, 3);
        lexer->token = LEXER_FN;

        return true;
      }
    }
  } else if (ch1 == 'i' && !duc_file_eof(file)) {
    unsigned char ch2 = duc_file_readchar(file);

    if (ch2 == 'n') {
      if (!duc_file_eof(file)) {
        size_t bu_pos = duc_file_position(file);
        unsigned char ch_id = duc_file_readchar(file);
        duc_file_seek(file, bu_pos);

        if (!lexer_char_is_id(ch_id)) {
          goto lexer_is_keyword_in;
        }
      } else {
lexer_is_keyword_in:
        lexer->raw = malloc(3);
        lexer->str = malloc(3);
        lexer->raw[0] = ch1;
        lexer->raw[1] = ch2;
        lexer->raw[2] = '\0';
        memcpy(lexer->str, lexer->raw, 3);
        lexer->token = LEXER_IN;

        return true;
      }
    }
  } else if (ch1 == 'l' && !duc_file_eof(file)) {
    unsigned char ch2 = duc_file_readchar(file);

    if (ch2 == 'o' && !duc_file_eof(file)) {
      unsigned char ch3 = duc_file_readchar(file);

      if (ch3 == 'o' && !duc_file_eof(file)) {
        unsigned char ch4 = duc_file_readchar(file);

        if (ch4 == 'p') {
          if (!duc_file_eof(file)) {
            size_t bu_pos = duc_file_position(file);
            unsigned char ch_id = duc_file_readchar(file);
            duc_file_seek(file, bu_pos);

            if (!lexer_char_is_id(ch_id)) {
              goto lexer_is_keyword_loop;
            }
          } else {
lexer_is_keyword_loop:
            lexer->raw = malloc(5);
            lexer->str = malloc(5);
            lexer->raw[0] = ch1;
            lexer->raw[1] = ch2;
            lexer->raw[2] = ch3;
            lexer->raw[3] = ch4;
            lexer->raw[4] = '\0';
            memcpy(lexer->str, lexer->raw, 5);
            lexer->token = LEXER_LOOP;

            return true;
          }
        }
      }
    }
  } else if (ch1 == 'm' && !duc_file_eof(file)) {
    unsigned char ch2 = duc_file_readchar(file);

    if (ch2 == 'a' && !duc_file_eof(file)) {
      unsigned char ch3 = duc_file_readchar(file);

      if (ch3 == 'i' && !duc_file_eof(file)) {
        unsigned char ch4 = duc_file_readchar(file);

        if (ch4 == 'n') {
          if (!duc_file_eof(file)) {
            size_t bu_pos = duc_file_position(file);
            unsigned char ch_id = duc_file_readchar(file);
            duc_file_seek(file, bu_pos);

            if (!lexer_char_is_id(ch_id)) {
              goto lexer_is_keyword_main;
            }
          } else {
lexer_is_keyword_main:
            lexer->raw = malloc(5);
            lexer->str = malloc(5);
            lexer->raw[0] = ch1;
            lexer->raw[1] = ch2;
            lexer->raw[2] = ch3;
            lexer->raw[3] = ch4;
            lexer->raw[4] = '\0';
            memcpy(lexer->str, lexer->raw, 5);
            lexer->token = LEXER_MAIN;

            return true;
          }
        }
      }
    } else if (ch2 == 'u' && !duc_file_eof(file)) {
      unsigned char ch3 = duc_file_readchar(file);

      if (ch3 == 't') {
        if (!duc_file_eof(file)) {
          size_t bu_pos = duc_file_position(file);
          unsigned char ch_id = duc_file_readchar(file);
          duc_file_seek(file, bu_pos);

          if (!lexer_char_is_id(ch_id)) {
            goto lexer_is_keyword_mut;
          }
        } else {
lexer_is_keyword_mut:
          lexer->raw = malloc(4);
          lexer->str = malloc(4);
          lexer->raw[0] = ch1;
          lexer->raw[1] = ch2;
          lexer->raw[2] = ch3;
          lexer->raw[3] = '\0';
          memcpy(lexer->str, lexer->raw, 4);
          lexer->token = LEXER_MUT;

          return true;
        }
      }
    }
  } else if (ch1 == 'r' && !duc_file_eof(file)) {
    unsigned char ch2 = duc_file_readchar(file);

    if (ch2 == 'e' && !duc_file_eof(file)) {
      unsigned char ch3 = duc_file_readchar(file);

      if (ch3 == 't' && !duc_file_eof(file)) {
        unsigned char ch4 = duc_file_readchar(file);

        if (ch4 == 'u' && !duc_file_eof(file)) {
          unsigned char ch5 = duc_file_readchar(file);

          if (ch5 == 'r' && !duc_file_eof(file)) {
            unsigned char ch6 = duc_file_readchar(file);

            if (ch6 == 'n') {
              if (!duc_file_eof(file)) {
                size_t bu_pos = duc_file_position(file);
                unsigned char ch_id = duc_file_readchar(file);
                duc_file_seek(file, bu_pos);

                if (!lexer_char_is_id(ch_id)) {
                  goto lexer_is_keyword_return;
                }
              } else {
lexer_is_keyword_return:
                lexer->raw = malloc(7);
                lexer->str = malloc(7);
                lexer->raw[0] = ch1;
                lexer->raw[1] = ch2;
                lexer->raw[2] = ch3;
                lexer->raw[3] = ch4;
                lexer->raw[4] = ch5;
                lexer->raw[5] = ch6;
                lexer->raw[6] = '\0';
                memcpy(lexer->str, lexer->raw, 7);
                lexer->token = LEXER_RETURN;

                return true;
              }
            }
          }
        }
      }
    }
  }

  duc_file_seek(file, pos);
  return false;
}

bool lexer_is_litstr_ (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch = duc_file_readchar(file);

  if (ch != '"' || duc_file_eof(file)) {
    duc_file_seek(file, pos);
    return false;
  } else {
    lexer->token = LEXER_LITSTR;
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

    if (ch == '"') {
      break;
    } else if (duc_file_eof(file)) {
      duc_throw("SyntaxError: Unexpected end of file, expected end of string");
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
  memcpy(lexer->str, lexer->raw, 2);

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
