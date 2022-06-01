/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include "../src/ASTChecker.hpp"
#include "../src/config.hpp"
#include "MockAST.hpp"

ASTChecker astCheckerTestGen (const std::string &code) {
  auto nodes = testing::NiceMock<MockAST>("main {" EOL + code + EOL "}" EOL).gen();
  auto nodeMain = std::get<ASTNodeMain>(*nodes[0].body);

  return ASTChecker(nodeMain.body);
}

TEST(ASTCheckerTest, EndsWithReturn) {
  EXPECT_FALSE(astCheckerTestGen("").endsWith<ASTNodeReturn>());
  EXPECT_TRUE(astCheckerTestGen("return").endsWith<ASTNodeReturn>());

  EXPECT_TRUE(astCheckerTestGen(
    "if 1 > 2 {" EOL
    "  print(\"Test\")" EOL
    "}" EOL
    "return" EOL
  ).endsWith<ASTNodeReturn>());

  EXPECT_TRUE(astCheckerTestGen(
    "if 1 > 2 {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
    "print(\"Test\")" EOL
    "return" EOL
  ).endsWith<ASTNodeReturn>());

  EXPECT_FALSE(astCheckerTestGen(
    "if 1 > 2 {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
  ).endsWith<ASTNodeReturn>());
}

TEST(ASTCheckerTest, HasReturn) {
  EXPECT_FALSE(astCheckerTestGen("").has<ASTNodeReturn>());
  EXPECT_TRUE(astCheckerTestGen("return").has<ASTNodeReturn>());

  EXPECT_TRUE(astCheckerTestGen(
    "if 1 > 2 {" EOL
    "  print(\"Test\")" EOL
    "}" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(astCheckerTestGen(
    "if 1 > 2 {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
    "print(\"Test\")" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(astCheckerTestGen(
    "if 1 > 2 {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
  ).has<ASTNodeReturn>());
}

TEST(ASTCheckerTest, IsFnDecl) {
  EXPECT_FALSE(astCheckerTestGen("").is<ASTNodeFnDecl>());
  EXPECT_FALSE(astCheckerTestGen("1 > 2").has<ASTNodeFnDecl>());
  EXPECT_TRUE(astCheckerTestGen("fn test () {}").has<ASTNodeFnDecl>());
  EXPECT_FALSE(astCheckerTestGen("if 1 > 2 {}").has<ASTNodeFnDecl>());
  EXPECT_FALSE(astCheckerTestGen("loop {}").has<ASTNodeFnDecl>());
  EXPECT_FALSE(astCheckerTestGen("obj Test {a: int}").has<ASTNodeFnDecl>());
  EXPECT_FALSE(astCheckerTestGen("return").has<ASTNodeFnDecl>());
  EXPECT_FALSE(astCheckerTestGen("test := 1").has<ASTNodeFnDecl>());
}

TEST(ASTCheckerTest, IsLast) {
  auto ast = testing::NiceMock<MockAST>(
    "main {" EOL
    "  a := 1" EOL
    "  if 1 > 2 {" EOL
    "    return" EOL
    "  }" EOL
    "  return" EOL
    "}" EOL
  );

  auto nodes = ast.gen();
  auto nodeMain = std::get<ASTNodeMain>(*nodes[0].body);

  EXPECT_FALSE(ASTChecker(nodeMain.body[0]).isLast());
  EXPECT_FALSE(ASTChecker(nodeMain.body[1]).isLast());
  EXPECT_TRUE(ASTChecker(nodeMain.body[2]).isLast());
}
