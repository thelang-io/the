/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef TEST_MOCK_AST_HPP
#define TEST_MOCK_AST_HPP

#include <gmock/gmock.h>
#include "../src/AST.hpp"
#include "MockParser.hpp"

class MockAST : public AST {
 public:
  explicit MockAST (const std::string &);
  virtual ~MockAST ();

  MOCK_METHOD(ASTBlock, gen, (), (override));
  MOCK_METHOD(std::string, xml, (), (override));

 private:
  testing::NiceMock<MockParser> _p;
};

#endif
