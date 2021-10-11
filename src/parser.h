/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_PARSER_H
#define SRC_PARSER_H

#include "stmt.h"

stmt_t *parser_next (reader_t *reader);

#endif
