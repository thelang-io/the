/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "MockLexer.hpp"

MockLexer::MockLexer (const std::string &content) : _r(content), Lexer(&this->_r) {
  ON_CALL(*this, next).WillByDefault([this] () {
    return this->Lexer::next();
  });

  ON_CALL(*this, seek).WillByDefault([this] (ReaderLocation l) {
    return this->Lexer::seek(l);
  });
}

MockLexer::~MockLexer () {
}
