/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "MockParser.hpp"

MockParser::MockParser (const std::string &content) : _l(content), Parser(&this->_l) {
  ON_CALL(*this, next).WillByDefault([this] () {
    return this->Parser::next();
  });
}

MockParser::~MockParser () {
}
