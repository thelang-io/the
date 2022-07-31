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
#include "utils.hpp"

ASTChecker astCheckerTestGen (const std::string &code) {
  auto nodes = testing::NiceMock<MockAST>("main {" EOL + code + EOL "}" EOL).gen();
  auto nodeMain = std::get<ASTNodeMain>(*nodes[0].body);

  return ASTChecker(nodeMain.body);
}

ASTChecker astCheckerTestExprGen () {
  auto nodes = testing::NiceMock<MockAST>("main {a := 1 a + 1}" EOL).gen();
  auto nodeMain = std::get<ASTNodeMain>(*nodes[0].body);

  return ASTChecker(*std::get<ASTNodeExpr>(*nodeMain.body[1].body).body);
}

TEST(ASTCheckerTest, CtorWithExprs) {
  auto nodes = testing::NiceMock<MockAST>("main {a := 1 a + 1 a - 2}" EOL).gen();
  auto nodeMain = std::get<ASTNodeMain>(*nodes[0].body);

  EXPECT_NO_THROW({
    ASTChecker({
      *std::get<ASTNodeExpr>(*nodeMain.body[1].body).body,
      *std::get<ASTNodeExpr>(*nodeMain.body[2].body).body
    });
  });
}

TEST(ASTCheckerTest, CtorWithPointers) {
  EXPECT_NO_THROW(ASTChecker(nullptr));

  auto nodes = testing::NiceMock<MockAST>("main {a := 1}" EOL).gen();
  auto nodeMain = std::get<ASTNodeMain>(*nodes[0].body);

  EXPECT_NO_THROW(ASTChecker(&nodeMain.body[0]));
}

TEST(ASTCheckerTest, ThrowsOnNonNode) {
  EXPECT_THROW_WITH_MESSAGE(astCheckerTestExprGen().endsWith<ASTNodeReturn>(), "tried node method on non node");
  EXPECT_THROW_WITH_MESSAGE(astCheckerTestExprGen().has<ASTNodeReturn>(), "tried node method on non node");
  EXPECT_THROW_WITH_MESSAGE(astCheckerTestExprGen().is<ASTNodeFnDecl>(), "tried node method on non node");
  EXPECT_THROW_WITH_MESSAGE(astCheckerTestExprGen().isLast(), "tried node method on non node");
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
    "fn test () {" EOL
    "  print(\"Test\")" EOL
    "}" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(astCheckerTestGen(
    "fn test () {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
    "print(\"Test\")" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(astCheckerTestGen(
    "fn test () {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
  ).has<ASTNodeReturn>());

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

  EXPECT_TRUE(astCheckerTestGen(
    "loop i := 0;; {" EOL
    "}" EOL
  ).has<ASTNodeVarDecl>());

  EXPECT_TRUE(astCheckerTestGen(
    "loop i := 0;; {" EOL
    "  print(\"Test\")" EOL
    "}" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(astCheckerTestGen(
    "loop i := 0;; {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
    "print(\"Test\")" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(astCheckerTestGen(
    "loop i := 0;; {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(astCheckerTestGen(
    "main {" EOL
    "  print(\"Test\")" EOL
    "}" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(astCheckerTestGen(
    "main {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
    "print(\"Test\")" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(astCheckerTestGen(
    "main {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
  ).has<ASTNodeReturn>());
}

TEST(ASTCheckerTest, IsFnDecl) {
  EXPECT_FALSE(astCheckerTestGen("").is<ASTNodeFnDecl>());
  EXPECT_FALSE(astCheckerTestGen("1 > 2").is<ASTNodeFnDecl>());
  EXPECT_TRUE(astCheckerTestGen("fn test () {}").is<ASTNodeFnDecl>());
  EXPECT_FALSE(astCheckerTestGen("if 1 > 2 {}").is<ASTNodeFnDecl>());
  EXPECT_FALSE(astCheckerTestGen("loop {}").is<ASTNodeFnDecl>());
  EXPECT_FALSE(astCheckerTestGen("obj Test {a: int}").is<ASTNodeFnDecl>());
  EXPECT_FALSE(astCheckerTestGen("return").is<ASTNodeFnDecl>());
  EXPECT_FALSE(astCheckerTestGen("test := 1").is<ASTNodeFnDecl>());
  EXPECT_FALSE(astCheckerTestGen("fn test () {} test2 := 2").is<ASTNodeFnDecl>());
  EXPECT_FALSE(astCheckerTestGen("test2 := 2 fn test () {}").is<ASTNodeFnDecl>());
}

TEST(ASTCheckerTest, IsLastOnFnDecl) {
  auto ast = testing::NiceMock<MockAST>(
    "fn test () {" EOL
    "  a := 1" EOL
    "  if 1 > 2 {" EOL
    "    return" EOL
    "  }" EOL
    "  return" EOL
    "}" EOL
  );

  auto nodes = ast.gen();
  auto node = std::get<ASTNodeFnDecl>(*nodes[0].body);

  EXPECT_FALSE(ASTChecker(node.body[0]).isLast());
  EXPECT_FALSE(ASTChecker(node.body[1]).isLast());
  EXPECT_TRUE(ASTChecker(node.body[2]).isLast());
}

TEST(ASTCheckerTest, IsLastOnIf) {
  auto ast = testing::NiceMock<MockAST>(
    "if true {" EOL
    "  a := 1" EOL
    "  if 1 > 2 {" EOL
    "    return" EOL
    "  }" EOL
    "  return" EOL
    "}" EOL
  );

  auto nodes = ast.gen();
  auto node = std::get<ASTNodeIf>(*nodes[0].body);

  EXPECT_FALSE(ASTChecker(node.body[0]).isLast());
  EXPECT_FALSE(ASTChecker(node.body[1]).isLast());
  EXPECT_TRUE(ASTChecker(node.body[2]).isLast());
}

TEST(ASTCheckerTest, IsLastOnLoop) {
  auto ast = testing::NiceMock<MockAST>(
    "loop {" EOL
    "  a := 1" EOL
    "  if 1 > 2 {" EOL
    "    return" EOL
    "  }" EOL
    "  return" EOL
    "}" EOL
  );

  auto nodes = ast.gen();
  auto node = std::get<ASTNodeLoop>(*nodes[0].body);

  EXPECT_FALSE(ASTChecker(node.body[0]).isLast());
  EXPECT_FALSE(ASTChecker(node.body[1]).isLast());
  EXPECT_TRUE(ASTChecker(node.body[2]).isLast());
}

TEST(ASTCheckerTest, IsLastOnMain) {
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
  auto node = std::get<ASTNodeMain>(*nodes[0].body);

  EXPECT_FALSE(ASTChecker(node.body[0]).isLast());
  EXPECT_FALSE(ASTChecker(node.body[1]).isLast());
  EXPECT_TRUE(ASTChecker(node.body[2]).isLast());
}

TEST(ASTCheckerTest, ThrowsOnManyIsLast) {
  auto code =
    "  a := 1" EOL
    "  if 1 > 2 {" EOL
    "    return" EOL
    "  }" EOL
    "  return" EOL;

  EXPECT_THROW_WITH_MESSAGE(astCheckerTestGen(code).isLast(), "tried isLast on many nodes");
}

TEST(ASTCheckerTest, ThrowsOnRootIsLast) {
  auto nodes = testing::NiceMock<MockAST>("main {}").gen();
  EXPECT_THROW_WITH_MESSAGE(ASTChecker(nodes[0]).isLast(), "tried isLast on root node");
}
