/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <stdlib.h>
#include <string.h>
#include "lit-id.h"

bool lex_lit_id (duc_file_t *file, lexer_t *lexer, size_t pos) {
  unsigned char ch = duc_file_readchar(file);

  if (lexer_lit_id_is_char_start(ch)) {
    lexer->token = LEXER_LIT_ID;
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

    if (!lexer_lit_id_is_char(ch)) {
      duc_file_seek(file, bu_pos);
      break;
    }

    lexer->raw = realloc(lexer->raw, ++len + 1);
    lexer->raw[len - 1] = ch;
    lexer->raw[len] = '\0';
  }

  lexer->str = malloc(len + 1);
  memcpy(lexer->str, lexer->raw, len + 1);

  switch (len) {
    case 2: {
      if (memcmp(lexer->str, "as", len + 1) == 0) {
        if (!duc_file_eof(file)) {
          size_t bu_pos = duc_file_position(file);
          unsigned char ch_safe = duc_file_readchar(file);

          if (ch_safe == '?') {
            lexer->raw = realloc(lexer->raw, ++len + 1);
            lexer->str = realloc(lexer->str, len + 1);
            lexer->raw[len - 1] = lexer->str[len - 1] = ch_safe;
            lexer->raw[len] = lexer->str[len] = '\0';
            lexer->token = LEXER_KW_AS_SAFE;
          } else {
            duc_file_seek(file, bu_pos);
            lexer->token = LEXER_KW_AS;
          }
        } else {
          lexer->token = LEXER_KW_AS;
        }
      } else if (memcmp(lexer->str, "fn", len + 1) == 0) {
        lexer->token = LEXER_KW_FN;
      } else if (memcmp(lexer->str, "if", len + 1) == 0) {
        lexer->token = LEXER_KW_IF;
      } else if (memcmp(lexer->str, "in", len + 1) == 0) {
        lexer->token = LEXER_KW_IN;
      } else if (memcmp(lexer->str, "is", len + 1) == 0) {
        lexer->token = LEXER_KW_IS;
      } else if (memcmp(lexer->str, "op", len + 1) == 0) {
        lexer->token = LEXER_KW_OP;
      }

      break;
    }
    case 3: {
      if (memcmp(lexer->str, "mut", len + 1) == 0) {
        lexer->token = LEXER_KW_MUT;
      } else if (memcmp(lexer->str, "new", len + 1) == 0) {
        lexer->token = LEXER_KW_NEW;
      } else if (memcmp(lexer->str, "nil", len + 1) == 0) {
        lexer->token = LEXER_KW_NIL;
      } else if (memcmp(lexer->str, "obj", len + 1) == 0) {
        lexer->token = LEXER_KW_OBJ;
      } else if (memcmp(lexer->str, "pub", len + 1) == 0) {
        lexer->token = LEXER_KW_PUB;
      } else if (memcmp(lexer->str, "try", len + 1) == 0) {
        lexer->token = LEXER_KW_TRY;
      }

      break;
    }
    case 4: {
      if (memcmp(lexer->str, "case", len + 1) == 0) {
        lexer->token = LEXER_KW_CASE;
      } else if (memcmp(lexer->str, "elif", len + 1) == 0) {
        lexer->token = LEXER_KW_ELIF;
      } else if (memcmp(lexer->str, "else", len + 1) == 0) {
        lexer->token = LEXER_KW_ELSE;
      } else if (memcmp(lexer->str, "enum", len + 1) == 0) {
        lexer->token = LEXER_KW_ENUM;
      } else if (memcmp(lexer->str, "from", len + 1) == 0) {
        lexer->token = LEXER_KW_FROM;
      } else if (memcmp(lexer->str, "init", len + 1) == 0) {
        lexer->token = LEXER_KW_INIT;
      } else if (memcmp(lexer->str, "loop", len + 1) == 0) {
        lexer->token = LEXER_KW_LOOP;
      } else if (memcmp(lexer->str, "main", len + 1) == 0) {
        lexer->token = LEXER_KW_MAIN;
      } else if (memcmp(lexer->str, "priv", len + 1) == 0) {
        lexer->token = LEXER_KW_PRIV;
      } else if (memcmp(lexer->str, "prot", len + 1) == 0) {
        lexer->token = LEXER_KW_PROT;
      } else if (memcmp(lexer->str, "this", len + 1) == 0) {
        lexer->token = LEXER_KW_THIS;
      } else if (memcmp(lexer->str, "true", len + 1) == 0) {
        lexer->token = LEXER_KW_TRUE;
      }

      break;
    }
    case 5: {
      if (memcmp(lexer->str, "async", len + 1) == 0) {
        lexer->token = LEXER_KW_ASYNC;
      } else if (memcmp(lexer->str, "await", len + 1) == 0) {
        lexer->token = LEXER_KW_AWAIT;
      } else if (memcmp(lexer->str, "break", len + 1) == 0) {
        lexer->token = LEXER_KW_BREAK;
      } else if (memcmp(lexer->str, "catch", len + 1) == 0) {
        lexer->token = LEXER_KW_CATCH;
      } else if (memcmp(lexer->str, "class", len + 1) == 0) {
        lexer->token = LEXER_KW_CLASS;
      } else if (memcmp(lexer->str, "const", len + 1) == 0) {
        lexer->token = LEXER_KW_CONST;
      } else if (memcmp(lexer->str, "false", len + 1) == 0) {
        lexer->token = LEXER_KW_FALSE;
      } else if (memcmp(lexer->str, "match", len + 1) == 0) {
        lexer->token = LEXER_KW_MATCH;
      } else if (memcmp(lexer->str, "super", len + 1) == 0) {
        lexer->token = LEXER_KW_SUPER;
      } else if (memcmp(lexer->str, "throw", len + 1) == 0) {
        lexer->token = LEXER_KW_THROW;
      } else if (memcmp(lexer->str, "union", len + 1) == 0) {
        lexer->token = LEXER_KW_UNION;
      }

      break;
    }
    case 6: {
      if (memcmp(lexer->str, "deinit", len + 1) == 0) {
        lexer->token = LEXER_KW_DEINIT;
      } else if (memcmp(lexer->str, "export", len + 1) == 0) {
        lexer->token = LEXER_KW_EXPORT;
      } else if (memcmp(lexer->str, "import", len + 1) == 0) {
        lexer->token = LEXER_KW_IMPORT;
      } else if (memcmp(lexer->str, "return", len + 1) == 0) {
        lexer->token = LEXER_KW_RETURN;
      } else if (memcmp(lexer->str, "static", len + 1) == 0) {
        lexer->token = LEXER_KW_STATIC;
      }

      break;
    }
    case 7: {
      if (memcmp(lexer->str, "default", len + 1) == 0) {
        lexer->token = LEXER_KW_DEFAULT;
      }

      break;
    }
    case 8: {
      if (memcmp(lexer->str, "continue", len + 1) == 0) {
        lexer->token = LEXER_KW_CONTINUE;
      } else if (memcmp(lexer->str, "override", len + 1) == 0) {
        lexer->token = LEXER_KW_OVERRIDE;
      }

      break;
    }
    case 9: {
      if (memcmp(lexer->str, "interface", len + 1) == 0) {
        lexer->token = LEXER_KW_INTERFACE;
      }

      break;
    }
    case 11: {
      if (memcmp(lexer->str, "fallthrough", len + 1) == 0) {
        lexer->token = LEXER_KW_FALLTHROUGH;
      }

      break;
    }
  }

  return true;
}

bool lexer_lit_id_is_char (unsigned char ch) {
  const char *chs =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
  return strchr(chs, ch) != NULL;
}

bool lexer_lit_id_is_char_start (unsigned char ch) {
  const char *chs = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";
  return strchr(chs, ch) != NULL;
}
