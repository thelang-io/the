/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_LEXER_ID_H
#define SRC_LEXER_ID_H

#include "../lexer.h"

bool lexer_id (duc_file_t *file, lexer_t *lexer, size_t pos);
bool lexer_id_is_char (unsigned char ch);
bool lexer_id_is_char_start (unsigned char ch);

#endif
