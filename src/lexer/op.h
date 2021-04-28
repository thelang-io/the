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
  do { if (!duc_file_eof(file)) { \
    size_t bu_pos = duc_file_position(file); \
    ch2 = duc_file_readchar(file); \
    if (ch2 != '=') { \
      duc_file_seek(file, bu_pos); \
      lexer->token = tok1; \
    } else { \
      lexer->token = tok2; \
      len += 1; \
    } \
  } else { \
    lexer->token = tok1; \
  } } while (0)

#define LEX_OP_EQ_AFTER(ch, tok1, tok2, tok3) \
  do { if (!duc_file_eof(file)) { \
    size_t bu_pos2 = duc_file_position(file); \
    ch2 = duc_file_readchar(file); \
    if (ch2 != ch) { \
      duc_file_seek(file, bu_pos2); \
      lexer->token = tok1; \
    } else if (!duc_file_eof(file)) { \
      size_t bu_pos3 = duc_file_position(file); \
      ch3 = duc_file_readchar(file); \
      if (ch3 != '=') { \
        duc_file_seek(file, bu_pos3); \
        lexer->token = tok2; \
        len += 1; \
      } else { \
        lexer->token = tok3; \
        len += 2; \
      } \
    } else { \
      lexer->token = tok2; \
      len += 1; \
    } \
  } else { \
    lexer->token = tok1; \
  } } while (0)

#define LEX_OP_EQ_BEFORE(ch, tok1, tok2, tok3) \
  do { if (!duc_file_eof(file)) { \
    size_t bu_pos2 = duc_file_position(file); \
    ch2 = duc_file_readchar(file); \
    if (ch2 != ch && ch2 != '=') { \
      duc_file_seek(file, bu_pos2); \
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
  do { if (!duc_file_eof(file)) { \
    size_t bu_pos2 = duc_file_position(file); \
    ch2 = duc_file_readchar(file); \
    if (ch2 != ch && ch2 != '=') { \
      duc_file_seek(file, bu_pos2); \
      lexer->token = tok1; \
    } else if (ch2 == '=') { \
      lexer->token = tok2; \
      len += 1; \
    } else if (!duc_file_eof(file)) { \
      size_t bu_pos3 = duc_file_position(file); \
      ch3 = duc_file_readchar(file); \
      if (ch3 != '=') { \
        duc_file_seek(file, bu_pos3); \
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

bool lex_op (duc_file_t *file, lexer_t *lexer, size_t pos);

#endif
