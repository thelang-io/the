/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Lexer.hpp"

Lexer::Lexer (Reader *reader) {
  this->_reader = reader;
}

Token Lexer::next () {
  if (this->_reader->eof()) {
    return Token(eof, "\0");
  }

  return Token(eof, "");
}
