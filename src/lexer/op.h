/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_LEXER_OP_H
#define SRC_LEXER_OP_H

#include "../lexer.h"

#define LEX_OP_EQ2(tok1, tok2) \
  do { if (!duc_file_eof(file)) { \
    size_t bu_pos = duc_file_position(file); \
    ch2 = duc_file_readchar(file); \
    if (ch2 != '=') { \
      lexer->token = tok1; \
      duc_file_seek(file, bu_pos); \
    } else { \
      lexer->token = tok2; \
      len += 1; \
    } \
  } else { \
    lexer->token = tok1; \
  } } while (0)

#define LEX_OP_EQ3(ch, tok1, tok2, tok3, tok4) \
  do { if (!duc_file_eof(file)) { \
    size_t bu_pos2 = duc_file_position(file); \
    ch2 = duc_file_readchar(file); \
    if (ch2 != ch && ch2 != '=') { \
      lexer->token = tok1; \
      duc_file_seek(file, bu_pos2); \
    } else if (ch2 == '=') { \
      lexer->token = tok2; \
      len += 1; \
    } else if (!duc_file_eof(file)) { \
      size_t bu_pos3 = duc_file_position(file); \
      ch3 = duc_file_readchar(file); \
      if (ch3 != '=') { \
        lexer->token = tok3; \
        duc_file_seek(file, bu_pos3); \
        len += 1; \
      } else { \
        lexer->token = tok4; \
        len += 2; \
      } \
    } else { \
      lexer->token = tok3; \
      len += 1; \
    } \
  } else { \
    lexer->token = tok1; \
  } } while (0)

bool lex_op (duc_file_t *file, lexer_t *lexer, size_t pos);

#endif
