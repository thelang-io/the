/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "MockAST.hpp"

MockAST::MockAST (const std::string &content) : _p(content), AST(&this->_p)  {
  this->reader = this->_p.reader;

  ON_CALL(*this, gen).WillByDefault([this] () {
    return this->AST::gen();
  });
}

MockAST::~MockAST () {
}
