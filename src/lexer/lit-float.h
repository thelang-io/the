/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_LEXER_LIT_FLOAT_H
#define SRC_LEXER_LIT_FLOAT_H

#include "../lexer.h"

bool lex_lit_float (file_t *file, lexer_t *lexer, size_t pos);
bool lexer_lit_float_is_char_special (unsigned char ch);

#endif
