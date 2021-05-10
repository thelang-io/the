/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_LEXER_LIT_CHAR_H
#define SRC_LEXER_LIT_CHAR_H

#include "../lexer.h"

bool lex_lit_char (file_t *file, lexer_t *lexer, size_t pos);
bool lexer_lit_char_is_escape (unsigned char ch);
void lexer_lit_char_process_ (file_t *file, lexer_t *lexer, size_t *len);

#endif
