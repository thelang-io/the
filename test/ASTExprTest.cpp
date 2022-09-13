/*!
 * Copyright (c) 2018 Aaron Delasy
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
