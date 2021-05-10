/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_LEXER_OP_H
#define SRC_LEXER_OP_H

#include "../lexer.h"

#define LEX_OP_EQ(tok1, tok2) \
  do { if (!file_eof(file)) { \
    size_t bu_pos = file_position(file); \
    ch2 = file_readchar(file); \
    if (ch2 != '=') { \
      file_seek(file, bu_pos); \
      lexer->token = tok1; \
    } else { \
      lexer->token = tok2; \
      len += 1; \
    } \
  } else { \
    lexer->token = tok1; \
  } } while (0)

#define LEX_OP_EQ_BEFORE(ch, tok1, tok2, tok3) \
  do { if (!file_eof(file)) { \
    size_t bu_pos2 = file_position(file); \
    ch2 = file_readchar(file); \
    if (ch2 != ch && ch2 != '=') { \
      file_seek(file, bu_pos2); \
      lexer->token = tok1; \
    } else if (ch2 == '=') { \
      lexer->token = tok2; \
      len += 1; \
    } else { \
      lexer->token = tok3; \
      len += 1; \
    } \
  } else { \
    lexer->token = tok1; \
  } } while (0)

#define LEX_OP_EQ2(ch, tok1, tok2, tok3, tok4) \
  do { if (!file_eof(file)) { \
    size_t bu_pos2 = file_position(file); \
    ch2 = file_readchar(file); \
    if (ch2 != ch && ch2 != '=') { \
      file_seek(file, bu_pos2); \
      lexer->token = tok1; \
    } else if (ch2 == '=') { \
      lexer->token = tok2; \
      len += 1; \
    } else if (!file_eof(file)) { \
      size_t bu_pos3 = file_position(file); \
      ch3 = file_readchar(file); \
      if (ch3 != '=') { \
        file_seek(file, bu_pos3); \
        lexer->token = tok3; \
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

bool lex_op (file_t *file, lexer_t *lexer, size_t pos);

#endif
