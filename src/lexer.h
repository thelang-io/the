/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_LEXER_H
#define SRC_LEXER_H

#include "token.h"

token_t *lexer_next (reader_t *reader);

#endif
