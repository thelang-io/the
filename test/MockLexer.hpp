/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef TEST_MOCK_LEXER_HPP
#define TEST_MOCK_LEXER_HPP

#include <gmock/gmock.h>
#include "../src/Lexer.hpp"
#include "MockReader.hpp"

class MockLexer : public Lexer {
 public:
  explicit MockLexer (const std::string &);
  virtual ~MockLexer ();

  MOCK_METHOD((std::tuple<ReaderLocation, Token>), next, (), (override));
  MOCK_METHOD(void, seek, (ReaderLocation), (override));

 private:
  testing::NiceMock<MockReader> _r;
};

#endif
