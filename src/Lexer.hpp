/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_LEXER_HPP
#define SRC_LEXER_HPP

#include "Reader.hpp"
#include "Token.hpp"

class Lexer {
 public:
  Lexer (Reader *reader);
  Token next ();

 private:
  Reader *_reader;
};

#endif
