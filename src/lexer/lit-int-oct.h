/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_LEXER_LIT_INT_OCT_H
#define SRC_LEXER_LIT_INT_OCT_H

#include "../lexer.h"

bool lex_lit_int_oct (duc_file_t *file, lexer_t *lexer, size_t pos);
bool lexer_lit_int_oct_is_special (unsigned char ch);

#endif
