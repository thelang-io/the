/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_LEXER_HPP
#define SRC_LEXER_HPP

#include "Token.hpp"

struct Lexer {
  ReaderLocation start;
  std::string val;
};

Token lex (Reader *reader);

#endif
