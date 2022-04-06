/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef TEST_MOCK_PARSER_HPP
#define TEST_MOCK_PARSER_HPP

#include <gmock/gmock.h>
#include "../src/Parser.hpp"
#include "MockLexer.hpp"

class MockParser : public Parser {
 public:
  explicit MockParser (const std::string &);
  virtual ~MockParser ();

  MOCK_METHOD(ParserStmt, next, (), (override));

 private:
  testing::NiceMock<MockLexer> _l;
};

#endif
