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

  auto ch = this->_reader->next();

  if (Token::isWhitespace(ch)) {
    auto val = std::string{ch};

    while (!this->_reader->eof()) {
      const auto loc = this->_reader->loc();
      ch = this->_reader->next();

      if (!Token::isWhitespace(ch)) {
        this->_reader->seek(loc);
        break;
      }

      val += ch;
    }

    return Token(whitespace, val);
  }

  return Token(unknown, "");
}
