/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include "../src/config.hpp"
#include "MockAST.hpp"

ASTNodeExpr astExprTestGen (const std::string &code) {
  auto nodes = testing::NiceMock<MockAST>("main {" EOL + code + EOL "}" EOL).gen();
  auto nodeMain = std::get<ASTNodeMain>(*nodes[0].body);

  return std::get<ASTNodeExpr>(*nodeMain.body[0].body);
}

TEST(ASTExprTest, IsLit) {
  EXPECT_TRUE(astExprTestGen(R"("test")").isLit());
  EXPECT_TRUE(astExprTestGen(R"("test" + "test")").isLit());
}

TEST(ASTExprTest, IsNotLit) {
  EXPECT_FALSE(astExprTestGen(R"(!"test")").isLit());
  EXPECT_FALSE(astExprTestGen(R"(print())").isLit());
}

TEST(ASTExprTest, LitBody) {
  EXPECT_EQ(astExprTestGen(R"("test")").litBody(), R"("test")");
  EXPECT_EQ(astExprTestGen(R"("" + "")").litBody(), R"("")");
  EXPECT_EQ(astExprTestGen(R"("1" + "2")").litBody(), R"("12")");
  EXPECT_EQ(astExprTestGen(R"("Hello, " + "World!")").litBody(), R"("Hello, World!")");
}
