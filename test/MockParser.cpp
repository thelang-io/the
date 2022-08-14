/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "MockParser.hpp"

MockParser::MockParser (const std::string &content) : _l(content), Parser(&this->_l) {
  this->reader = this->_l.reader;

  ON_CALL(*this, next).WillByDefault([this] (bool _1) {
    return this->Parser::next(_1);
  });
}

MockParser::~MockParser () {
}
