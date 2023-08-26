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
#include "../src/ASTChecker.hpp"
#include "../src/config.hpp"
#include "MockAST.hpp"
#include "utils.hpp"

class ASTCheckerTest : public testing::Test {
 protected:
  std::shared_ptr<AST> ast_;

  ASTChecker expr_ (const std::string &decl, const std::string &code) {
    auto actualDecl = decl.empty() ? "t := 1" : decl;
    auto parser = testing::NiceMock<MockParser>("main { " + actualDecl + "; " + code + " }");
    this->ast_ = std::make_shared<AST>(&parser);
    auto nodes = this->ast_->gen();
    auto nodeMain = std::get<ASTNodeMain>(*nodes[0].body);

    if (!std::holds_alternative<ASTNodeExpr>(*nodeMain.body.back().body)) {
      throw Error("Expected expressions node in ASTCheckerTest");
    }

    return ASTChecker(std::get<ASTNodeExpr>(*nodeMain.body.back().body));
  }

  ASTChecker node_ (const std::string &code) {
    auto parser = testing::NiceMock<MockParser>("main {" EOL + code + EOL "}" EOL);
    this->ast_ = std::make_shared<AST>(&parser);
    auto nodes = this->ast_->gen();
    auto nodeMain = std::get<ASTNodeMain>(*nodes[0].body);

    return ASTChecker(nodeMain.body);
  }
};

TEST_F(ASTCheckerTest, FlattenExprGlobalExprClosure) {
  auto nodes = testing::NiceMock<MockAST>("(a: int, b: int = 2, c := 3) -> int { return a + b + c }").gen();
  auto nodeExpr = std::get<ASTNodeExpr>(*nodes[0].body);

  EXPECT_EQ(ASTChecker::flattenExpr({ nodeExpr }, false).size(), 8);
}

TEST_F(ASTCheckerTest, FlattenNodeGlobalFn) {
  auto nodes = testing::NiceMock<MockAST>("fn test () int { a := 1; return a }").gen();
  EXPECT_EQ(ASTChecker::flattenNode(nodes, false).size(), 3);
}

TEST_F(ASTCheckerTest, FlattenNodeGlobalObjMethod) {
  auto nodes = testing::NiceMock<MockAST>("obj Test { fn test () int { a := 1; return a } }").gen();
  EXPECT_EQ(ASTChecker::flattenNode(nodes, false).size(), 3);
}

TEST_F(ASTCheckerTest, FlattenNodeExprsGlobalEnum) {
  auto nodes = testing::NiceMock<MockAST>("enum Test { Color = 1, T = 2 << 3, A = !2 }").gen();
  EXPECT_EQ(ASTChecker::flattenNodeExprs(nodes, false).size(), 3);
}

TEST_F(ASTCheckerTest, FlattenNodeExprsGlobalFn) {
  auto nodes = testing::NiceMock<MockAST>("fn test (a: int, b: int = 2, c := 3) int { return a + b + c + 2 }").gen();
  EXPECT_EQ(ASTChecker::flattenNodeExprs(nodes, false).size(), 3);
}

TEST_F(ASTCheckerTest, FlattenNodeExprsGlobalObjMethod) {
  auto nodes = testing::NiceMock<MockAST>("obj Test { fn test (a: int, b: int = 2, c := 3) int { return a + b + c + 2 } }").gen();
  EXPECT_EQ(ASTChecker::flattenNodeExprs(nodes, false).size(), 3);
}

TEST_F(ASTCheckerTest, Async) {
  EXPECT_FALSE(this->node_("async fn test () {}").async());
  EXPECT_FALSE(this->node_("async fn test () {}; test();").async());
  EXPECT_FALSE(this->expr_("async fn test () {}", "test()").async());
  EXPECT_TRUE(this->expr_("async fn test () {}", "await test()").async());
}

TEST_F(ASTCheckerTest, AsyncNonLocal) {
  EXPECT_FALSE(this->node_("async fn test () {}").async(false));
  EXPECT_TRUE(this->node_("async fn test () {}; test();").async(false));
  EXPECT_TRUE(this->expr_("async fn test () {}", "test()").async(false));
  EXPECT_TRUE(this->expr_("async fn test () {}", "await test()").async(false));
}

TEST_F(ASTCheckerTest, CtorWithExprs) {
  auto nodes = testing::NiceMock<MockAST>("main {a := 1; a + 1 a - 2}" EOL).gen();
  auto nodeMain = std::get<ASTNodeMain>(*nodes[0].body);

  EXPECT_NO_THROW({
    ASTChecker({
      std::get<ASTNodeExpr>(*nodeMain.body[1].body),
      std::get<ASTNodeExpr>(*nodeMain.body[2].body)
    });
  });
}

TEST_F(ASTCheckerTest, CtorWithPointers) {
  EXPECT_NO_THROW(ASTChecker(nullptr));

  auto nodes = testing::NiceMock<MockAST>("main {a := 1}" EOL).gen();
  auto nodeMain = std::get<ASTNodeMain>(*nodes[0].body);

  EXPECT_NO_THROW(ASTChecker(&nodeMain.body[0]));
}

TEST_F(ASTCheckerTest, ThrowsOnNonNode) {
  EXPECT_THROW_WITH_MESSAGE(this->expr_("a := 1", "a + 1").endsWith<ASTNodeReturn>(), "tried node method on non node");
  EXPECT_THROW_WITH_MESSAGE(this->expr_("a := 1", "a + 1").getNodeOfType<ASTNodeReturn>(), "tried node method on non node");
  EXPECT_THROW_WITH_MESSAGE(this->expr_("a := 1", "a + 1").has<ASTNodeReturn>(), "tried node method on non node");
  EXPECT_THROW_WITH_MESSAGE(this->expr_("a := 1", "a + 1").hoistingFriendly(), "tried node method on non node");
  EXPECT_THROW_WITH_MESSAGE(this->expr_("a := 1", "a + 1").insideMain(), "tried node method on non node");
  EXPECT_THROW_WITH_MESSAGE(this->expr_("a := 1", "a + 1").is<ASTNodeFnDecl>(), "tried node method on non node");
  EXPECT_THROW_WITH_MESSAGE(this->expr_("a := 1", "a + 1").isLast(), "tried node method on non node");
  EXPECT_THROW_WITH_MESSAGE(this->expr_("a := 1", "a + 1").parentIs<ASTNodeFnDecl>(), "tried node method on non node");
}

TEST_F(ASTCheckerTest, EndsWithReturn) {
  EXPECT_FALSE(this->node_("").endsWith<ASTNodeReturn>());
  EXPECT_TRUE(this->node_("return").endsWith<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "if 1 > 2 {" EOL
    "  print(\"Test\")" EOL
    "}" EOL
    "return" EOL
  ).endsWith<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "if 1 > 2 {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
    "print(\"Test\")" EOL
    "return" EOL
  ).endsWith<ASTNodeReturn>());

  EXPECT_FALSE(this->node_(
    "if 1 > 2 {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
  ).endsWith<ASTNodeReturn>());
}

TEST_F(ASTCheckerTest, GetExprOfType) {
  EXPECT_EQ(this->expr_("a := 1", "a").getExprOfType<ASTExprAccess>().size(), 1);
  EXPECT_EQ(this->expr_("", "[1, 2, 3]").getExprOfType<ASTExprArray>().size(), 1);
  EXPECT_EQ(this->expr_("a := 1", "a as int").getExprOfType<ASTExprAs>().size(), 1);
  EXPECT_EQ(this->expr_("a := 1", "a = 2").getExprOfType<ASTExprAssign>().size(), 1);
  EXPECT_EQ(this->expr_("", "1+1").getExprOfType<ASTExprBinary>().size(), 1);
  EXPECT_EQ(this->expr_("", "print()").getExprOfType<ASTExprCall>().size(), 1);
  EXPECT_EQ(this->expr_("", "() -> void {}").getExprOfType<ASTExprClosure>().size(), 1);
  EXPECT_EQ(this->expr_("a := 1", "a ? 1 : 2").getExprOfType<ASTExprCond>().size(), 1);
  EXPECT_EQ(this->expr_("a := 1", "a is int").getExprOfType<ASTExprIs>().size(), 1);
  EXPECT_EQ(this->expr_("", "1").getExprOfType<ASTExprLit>().size(), 1);
  EXPECT_EQ(this->expr_("", "{\"key\": \"val\"}").getExprOfType<ASTExprMap>().size(), 1);
  EXPECT_EQ(this->expr_("", "fs_Stats{mode: 1}").getExprOfType<ASTExprObj>().size(), 1);
  EXPECT_EQ(this->expr_("a := 1", "ref a").getExprOfType<ASTExprRef>().size(), 1);
  EXPECT_EQ(this->expr_("", "-2").getExprOfType<ASTExprUnary>().size(), 1);

  EXPECT_EQ(this->expr_("arr := [1, 2, 3]", "arr[1]").getExprOfType<ASTExprLit>().size(), 1);
  EXPECT_EQ(this->expr_("obj Test { arr: int[] }; test: Test", "test.arr[1]").getExprOfType<ASTExprLit>().size(), 1);
  EXPECT_EQ(this->expr_("", "[1, 2, 3]").getExprOfType<ASTExprLit>().size(), 3);
  EXPECT_EQ(this->expr_("a := 1", "a as int").getExprOfType<ASTExprAccess>().size(), 1);
  EXPECT_EQ(this->expr_("a := 1", "a = 2").getExprOfType<ASTExprAccess>().size(), 1);
  EXPECT_EQ(this->expr_("a := 1", "a = 2").getExprOfType<ASTExprLit>().size(), 1);
  EXPECT_EQ(this->expr_("", "2 + 2").getExprOfType<ASTExprLit>().size(), 2);
  EXPECT_EQ(this->expr_("", "print()").getExprOfType<ASTExprAccess>().size(), 1);
  EXPECT_EQ(this->expr_("", "print(1, 2, 3)").getExprOfType<ASTExprLit>().size(), 3);
  EXPECT_EQ(this->expr_("", "(a: int, b := 2) -> int { return a + b }").getExprOfType<ASTExprAccess>().size(), 0);
  EXPECT_EQ(this->expr_("a := 1", "a ? 1 : 2").getExprOfType<ASTExprAccess>().size(), 1);
  EXPECT_EQ(this->expr_("a := 1", "a ? 1 : 2").getExprOfType<ASTExprLit>().size(), 2);
  EXPECT_EQ(this->expr_("a := 1", "a is int").getExprOfType<ASTExprAccess>().size(), 1);
  EXPECT_EQ(this->expr_("", "{\"key\": 1, \"key2\": 2}").getExprOfType<ASTExprLit>().size(), 2);
  EXPECT_EQ(this->expr_("obj Test { a: int; arr: int[] }", "Test{a: 1, arr: [1, 2, 3]}").getExprOfType<ASTExprLit>().size(), 4);
  EXPECT_EQ(this->expr_("a := 1", "ref a").getExprOfType<ASTExprAccess>().size(), 1);
  EXPECT_EQ(this->expr_("a := 1", "-a").getExprOfType<ASTExprAccess>().size(), 1);
}

TEST_F(ASTCheckerTest, GetExprOfTypeFromNodes) {
  EXPECT_EQ(this->node_("enum Color { Red = 1, Green = 2, Blue = 3 }").getExprOfType<ASTExprLit>().size(), 0);
  EXPECT_EQ(this->node_("fn test (a := 2, b := 3) {}").getExprOfType<ASTExprLit>().size(), 0);
  EXPECT_EQ(this->node_("fn test () { a := 2; b := 3 }").getExprOfType<ASTExprLit>().size(), 0);
  EXPECT_EQ(this->node_("1 + 2").getExprOfType<ASTExprLit>().size(), 2);
  EXPECT_EQ(this->node_("if 1 == 1 {}").getExprOfType<ASTExprLit>().size(), 2);
  EXPECT_EQ(this->node_("if true { 1 + 1 }").getExprOfType<ASTExprLit>().size(), 3);
  EXPECT_EQ(this->node_("if true { 1 + 1 } else { 2 + 2 }").getExprOfType<ASTExprLit>().size(), 5);
  EXPECT_EQ(this->node_("if true { 1 + 1 } elif false { 2 + 2 }").getExprOfType<ASTExprLit>().size(), 6);
  EXPECT_EQ(this->node_("if true { 1 + 1 } elif false { 2 + 2 } else { 3 + 3 }").getExprOfType<ASTExprLit>().size(), 8);
  EXPECT_EQ(this->node_("if true { 1 + 1 } elif false { 2 + 2 } elif true { 3 + 3 }").getExprOfType<ASTExprLit>().size(), 9);
  EXPECT_EQ(this->node_("if true { 1 + 1 } elif false { 2 + 2 } elif true { 3 + 3 } else { 4 + 4 }").getExprOfType<ASTExprLit>().size(), 11);
  EXPECT_EQ(this->node_("loop { 1 + 1 }").getExprOfType<ASTExprLit>().size(), 2);
  EXPECT_EQ(this->node_("loop true { 1 + 1 }").getExprOfType<ASTExprLit>().size(), 3);
  EXPECT_EQ(this->node_("loop a := 1; a < 2; a++ { 1 + 1 }").getExprOfType<ASTExprAccess>().size(), 2);
  EXPECT_EQ(this->node_("loop a := 1; a < 2; a++ { 1 + 1 }").getExprOfType<ASTExprLit>().size(), 4);
  EXPECT_EQ(this->node_("obj Test { fn test (a := 1, b := 2) }").getExprOfType<ASTExprLit>().size(), 0);
  EXPECT_EQ(this->node_("obj Test { fn test () {a := 1; b := 2} }").getExprOfType<ASTExprLit>().size(), 0);
  EXPECT_EQ(this->node_("fn test () { return 1 }").getExprOfType<ASTExprLit>().size(), 0);
  EXPECT_EQ(this->node_("fn test () { return 1 + 1 }").getExprOfType<ASTExprLit>().size(), 0);
  EXPECT_EQ(this->node_("throw error_NewError(\"message\")").getExprOfType<ASTExprAccess>().size(), 1);
  EXPECT_EQ(this->node_("throw error_NewError(\"message\")").getExprOfType<ASTExprLit>().size(), 1);
  EXPECT_EQ(this->node_("try { 1 + 1 } catch err: error_Error {}").getExprOfType<ASTExprLit>().size(), 2);
  EXPECT_EQ(this->node_("try {} catch err: error_Error { 1 + 1 }").getExprOfType<ASTExprLit>().size(), 2);
  EXPECT_EQ(this->node_("try {} catch err: error_Error { 1 + 1 } catch err: error_Error { 2 + 2 }").getExprOfType<ASTExprLit>().size(), 4);
  EXPECT_EQ(this->node_("a := 1").getExprOfType<ASTExprLit>().size(), 1);
  EXPECT_EQ(this->node_("a := 1 + 1").getExprOfType<ASTExprLit>().size(), 2);
}

TEST_F(ASTCheckerTest, GetExprOfTypeFromNodeMain) {
  auto nodes = testing::NiceMock<MockAST>("main { 1 + 1 }").gen();
  EXPECT_EQ(ASTChecker(nodes).getExprOfType<ASTExprLit>().size(), 2);
}

TEST_F(ASTCheckerTest, GetNodeOfType) {
  EXPECT_EQ(this->node_("loop { break }").getNodeOfType<ASTNodeBreak>().size(), 1);
  EXPECT_EQ(this->node_("fn test () { loop { break } }").getNodeOfType<ASTNodeBreak>().size(), 0);
  EXPECT_EQ(this->node_("export fn test () { loop { break } }").getNodeOfType<ASTNodeBreak>().size(), 0);
  EXPECT_EQ(this->node_("fn test () { loop { break } }").getNodeOfType<ASTNodeBreak>(false).size(), 1);
  EXPECT_EQ(this->node_("export fn test () { loop { break } }").getNodeOfType<ASTNodeBreak>(false).size(), 1);
}

TEST_F(ASTCheckerTest, HasAwait) {
  EXPECT_TRUE(this->expr_("async fn test () {}", "await test()").hasAwait());
  EXPECT_FALSE(this->expr_("fn test () {}", "test()").hasAwait());
}

TEST_F(ASTCheckerTest, HasExpr) {
  EXPECT_TRUE(this->expr_("a := 1", "a").hasExpr<ASTExprAccess>());
  EXPECT_TRUE(this->expr_("", "[1, 2, 3]").hasExpr<ASTExprArray>());
  EXPECT_TRUE(this->expr_("a := 1", "a as int").hasExpr<ASTExprAs>());
  EXPECT_TRUE(this->expr_("a := 1", "a = 2").hasExpr<ASTExprAssign>());
  EXPECT_TRUE(this->expr_("", "1+1").hasExpr<ASTExprBinary>());
  EXPECT_TRUE(this->expr_("", "print()").hasExpr<ASTExprCall>());
  EXPECT_TRUE(this->expr_("", "() -> void {}").hasExpr<ASTExprClosure>());
  EXPECT_TRUE(this->expr_("a := 1", "a ? 1 : 2").hasExpr<ASTExprCond>());
  EXPECT_TRUE(this->expr_("a := 1", "a is int").hasExpr<ASTExprIs>());
  EXPECT_TRUE(this->expr_("", "1").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->expr_("", "{\"key\": \"val\"}").hasExpr<ASTExprMap>());
  EXPECT_TRUE(this->expr_("", "fs_Stats{mode: 1}").hasExpr<ASTExprObj>());
  EXPECT_TRUE(this->expr_("a := 1", "ref a").hasExpr<ASTExprRef>());
  EXPECT_TRUE(this->expr_("", "-2").hasExpr<ASTExprUnary>());

  EXPECT_TRUE(this->expr_("arr := [1, 2, 3]", "arr[1]").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->expr_("obj Test { arr: int[] }; test: Test", "test.arr[1]").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->expr_("", "[1, 2, 3]").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->expr_("a := 1", "a as int").hasExpr<ASTExprAccess>());
  EXPECT_TRUE(this->expr_("a := 1", "a = 2").hasExpr<ASTExprAccess>());
  EXPECT_TRUE(this->expr_("a := 1", "a = 2").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->expr_("", "2 + 2").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->expr_("", "print()").hasExpr<ASTExprAccess>());
  EXPECT_TRUE(this->expr_("", "print(1, 2, 3)").hasExpr<ASTExprLit>());
  EXPECT_FALSE(this->expr_("", "(a: int, b := 2) -> int { return 1 + b }").hasExpr<ASTExprAccess>());
  EXPECT_TRUE(this->expr_("a := 1", "a ? 1 : 2").hasExpr<ASTExprAccess>());
  EXPECT_TRUE(this->expr_("a := 1", "a ? 1 : 2").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->expr_("a := 1", "a is int").hasExpr<ASTExprAccess>());
  EXPECT_TRUE(this->expr_("", "{\"key\": 1, \"key2\": 2}").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->expr_("obj Test { a: int; arr: int[] }", "Test{a: 1, arr: [1, 2, 3]}").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->expr_("a := 1", "ref a").hasExpr<ASTExprAccess>());
  EXPECT_TRUE(this->expr_("a := 1", "-a").hasExpr<ASTExprAccess>());

  EXPECT_FALSE(this->node_("enum Color { Red = 1, Green = 2, Blue = 3 }").hasExpr<ASTExprLit>());
  EXPECT_FALSE(this->node_("fn test (a := 2, b := 3) {}").hasExpr<ASTExprLit>());
  EXPECT_FALSE(this->node_("fn test () { a := 2; b := 3 }").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->node_("1 + 2").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->node_("if 1 == 1 {}").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->node_("if true { 1 + 1 }").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->node_("if true { 1 + 1 } else { 2 + 2 }").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->node_("if true { 1 + 1 } elif false { 2 + 2 }").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->node_("if true { 1 + 1 } elif false { 2 + 2 } else { 3 + 3 }").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->node_("if true { 1 + 1 } elif false { 2 + 2 } elif true { 3 + 3 }").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->node_("if true { 1 + 1 } elif false { 2 + 2 } elif true { 3 + 3 } else { 4 + 4 }").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->node_("loop { 1 + 1 }").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->node_("loop true { 1 + 1 }").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->node_("loop a := 1; a < 2; a++ { 1 + 1 }").hasExpr<ASTExprAccess>());
  EXPECT_TRUE(this->node_("loop a := 1; a < 2; a++ { 1 + 1 }").hasExpr<ASTExprLit>());
  EXPECT_FALSE(this->node_("obj Test { fn test (a := 1, b := 2) }").hasExpr<ASTExprLit>());
  EXPECT_FALSE(this->node_("obj Test { fn test () {a := 1; b := 2} }").hasExpr<ASTExprLit>());
  EXPECT_FALSE(this->node_("fn test () { return 1 }").hasExpr<ASTExprLit>());
  EXPECT_FALSE(this->node_("fn test () { return 1 + 1 }").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->node_("throw error_NewError(\"message\")").hasExpr<ASTExprAccess>());
  EXPECT_TRUE(this->node_("throw error_NewError(\"message\")").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->node_("try { 1 + 1 } catch err: error_Error {}").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->node_("try {} catch err: error_Error { 1 + 1 }").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->node_("try {} catch err: error_Error { 1 + 1 } catch err: error_Error { 2 + 2 }").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->node_("a := 1").hasExpr<ASTExprLit>());
  EXPECT_TRUE(this->node_("a := 1 + 1").hasExpr<ASTExprLit>());
}

TEST_F(ASTCheckerTest, HasReturn) {
  EXPECT_FALSE(this->node_("").has<ASTNodeReturn>());
  EXPECT_TRUE(this->node_("return").has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "fn test () {" EOL
    "  print(\"Test\")" EOL
    "}" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "fn test () {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
    "print(\"Test\")" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_FALSE(this->node_(
    "fn test () {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "if 1 > 2 {" EOL
    "  print(\"Test\")" EOL
    "}" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "if 1 > 2 {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
    "print(\"Test\")" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "if 1 > 2 {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "if 1 > 2 {" EOL
    "} else {" EOL
    "  print(\"Test\")" EOL
    "}" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "if 1 > 2 {" EOL
    "} else {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
    "print(\"Test\")" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "if 1 > 2 {" EOL
    "} else {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "if 1 > 2 {" EOL
    "} elif 2 > 3 {" EOL
    "}" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "if 1 > 2 {" EOL
    "} elif 2 > 3 {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
    "print(\"Test\")" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "if 1 > 2 {" EOL
    "} elif 2 > 3 {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "loop i := 0;; {" EOL
    "}" EOL
  ).has<ASTNodeVarDecl>());

  EXPECT_TRUE(this->node_(
    "loop i := 0;; {" EOL
    "  print(\"Test\")" EOL
    "}" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "loop i := 0;; {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
    "print(\"Test\")" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "loop i := 0;; {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "main {" EOL
    "  print(\"Test\")" EOL
    "}" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "main {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
    "print(\"Test\")" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "main {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
  ).has<ASTNodeReturn>());

  EXPECT_FALSE(this->node_(
    "obj Test {" EOL
    "  fn test () {" EOL
    "    return 1;" EOL
    "  }" EOL
    "}" EOL
  ).has<ASTNodeReturn>());

  EXPECT_FALSE(this->node_(
    "obj Test {" EOL
    "  fn test1 () {" EOL
    "  }" EOL
    "  fn test2 () {" EOL
    "    return 1" EOL
    "  }" EOL
    "}" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "try {" EOL
    "  print(\"Test\")" EOL
    "} catch err: error_Error {" EOL
    "}" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "try {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "} catch err: error_Error {" EOL
    "}" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "try {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "} catch err: error_Error {" EOL
    "}" EOL
    "print(\"Test\")" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "try {" EOL
    "} catch err: error_Error {" EOL
    "  print(\"Test\")" EOL
    "}" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "try {" EOL
    "} catch err: error_Error {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "try {" EOL
    "} catch err: error_Error {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
    "print(\"Test\")" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "try {" EOL
    "} catch err: error_Error {" EOL
    "} catch err: error_Error {" EOL
    "  print(\"Test\")" EOL
    "}" EOL
    "return" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "try {" EOL
    "} catch err: error_Error {" EOL
    "} catch err: error_Error {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
  ).has<ASTNodeReturn>());

  EXPECT_TRUE(this->node_(
    "try {" EOL
    "} catch err: error_Error {" EOL
    "} catch err: error_Error {" EOL
    "  print(\"Test\")" EOL
    "  return" EOL
    "}" EOL
    "print(\"Test\")" EOL
    "return" EOL
  ).has<ASTNodeReturn>());
}

TEST_F(ASTCheckerTest, HoistingFriendly) {
  EXPECT_TRUE(this->node_("enum Test { One, Two, Three }").hoistingFriendly());
  EXPECT_TRUE(this->node_("fn test () {}").hoistingFriendly());
  EXPECT_TRUE(this->node_("obj Test { a: int }").hoistingFriendly());
  EXPECT_TRUE(this->node_("type Alias = int").hoistingFriendly());
  EXPECT_TRUE(this->node_("fn test () {} type Alias = int").hoistingFriendly());
  EXPECT_TRUE(this->node_("enum Test { One, Two, Three } obj Alias { a: int }").hoistingFriendly());

  EXPECT_TRUE(this->node_("export enum Test { One, Two, Three }").hoistingFriendly());
  EXPECT_TRUE(this->node_("export fn test () {}").hoistingFriendly());
  EXPECT_TRUE(this->node_("export obj Test { a: int }").hoistingFriendly());
  EXPECT_TRUE(this->node_("export type Alias = int").hoistingFriendly());

  EXPECT_TRUE(this->node_("enum Test { One, Two, Three } export Test").hoistingFriendly());
  EXPECT_TRUE(this->node_("fn test () {} export test").hoistingFriendly());
  EXPECT_TRUE(this->node_("obj Test { a: int } export Test").hoistingFriendly());
  EXPECT_TRUE(this->node_("type Alias = int export Alias").hoistingFriendly());
}

TEST_F(ASTCheckerTest, InsideMainOnNull) {
  auto nullNode = static_cast<ASTNode *>(nullptr);
  EXPECT_FALSE(ASTChecker(nullNode).insideMain());
}

TEST_F(ASTCheckerTest, InsideMainNormal) {
  auto nodes = testing::NiceMock<MockAST>("main {a := 1}" EOL).gen();
  auto nodeMain = std::get<ASTNodeMain>(*nodes[0].body);

  EXPECT_TRUE(ASTChecker(nodeMain.body[0]).insideMain());
}

TEST_F(ASTCheckerTest, InsideMainMultipleParent) {
  auto nodes = testing::NiceMock<MockAST>("main { loop { if true { a := 1 } } }" EOL).gen();
  auto nodeMain = std::get<ASTNodeMain>(*nodes[0].body);
  auto nodeLoop = std::get<ASTNodeLoop>(*nodeMain.body[0].body);
  auto nodeIf = std::get<ASTNodeIf>(*nodeLoop.body[0].body);

  EXPECT_TRUE(ASTChecker(nodeIf.body[0]).insideMain());
}

TEST_F(ASTCheckerTest, InsideMainAsync) {
  auto nodes = testing::NiceMock<MockAST>("main {async fn test () {}; await test()}" EOL).gen();
  auto nodeMain = std::get<ASTNodeMain>(*nodes[0].body);

  EXPECT_FALSE(ASTChecker(nodeMain.body[0]).insideMain());
}

TEST_F(ASTCheckerTest, InsideMainFn) {
  auto nodes = testing::NiceMock<MockAST>("main { fn test () { a := 1 } }" EOL).gen();
  auto nodeMain = std::get<ASTNodeMain>(*nodes[0].body);
  auto nodeFnDecl = std::get<ASTNodeFnDecl>(*nodeMain.body[0].body);

  EXPECT_FALSE(ASTChecker((*nodeFnDecl.body)[0]).insideMain());
}

TEST_F(ASTCheckerTest, InsideMainObjMethod) {
  auto nodes = testing::NiceMock<MockAST>("main { obj Test { fn test () { a := 1 } } }" EOL).gen();
  auto nodeMain = std::get<ASTNodeMain>(*nodes[0].body);
  auto nodeObjDecl = std::get<ASTNodeObjDecl>(*nodeMain.body[0].body);
  auto nodeObjDeclMethod = nodeObjDecl.methods[0];

  EXPECT_FALSE(ASTChecker((*nodeObjDeclMethod.body)[0]).insideMain());
}

TEST_F(ASTCheckerTest, InsideMainRoot) {
  auto nodes = testing::NiceMock<MockAST>("const A := 1" EOL).gen();
  EXPECT_TRUE(ASTChecker(nodes[0]).insideMain());
}

TEST_F(ASTCheckerTest, IsFnDecl) {
  EXPECT_FALSE(this->node_("").is<ASTNodeFnDecl>());
  EXPECT_FALSE(this->node_("1 > 2").is<ASTNodeFnDecl>());
  EXPECT_TRUE(this->node_("fn test () {}").is<ASTNodeFnDecl>());
  EXPECT_FALSE(this->node_("if 1 > 2 {}").is<ASTNodeFnDecl>());
  EXPECT_FALSE(this->node_("loop {}").is<ASTNodeFnDecl>());
  EXPECT_FALSE(this->node_("obj Test {a: int}").is<ASTNodeFnDecl>());
  EXPECT_FALSE(this->node_("return").is<ASTNodeFnDecl>());
  EXPECT_FALSE(this->node_("test := 1").is<ASTNodeFnDecl>());
  EXPECT_FALSE(this->node_("fn test () {} test2 := 2").is<ASTNodeFnDecl>());
  EXPECT_FALSE(this->node_("test2 := 2 fn test () {}").is<ASTNodeFnDecl>());
}

TEST_F(ASTCheckerTest, IsLastOnFnDecl) {
  auto ast = testing::NiceMock<MockAST>(
    "fn test () {" EOL
    "  a := 1" EOL
    "  if 1 > 2 {" EOL
    "    return" EOL
    "  }" EOL
    "  return" EOL
    "}" EOL
    "fn test () void" EOL
  );

  auto nodes = ast.gen();
  auto node1 = std::get<ASTNodeFnDecl>(*nodes[0].body);

  EXPECT_NE(node1.body, std::nullopt);
  EXPECT_FALSE(ASTChecker((*node1.body)[0]).isLast());
  EXPECT_FALSE(ASTChecker((*node1.body)[1]).isLast());
  EXPECT_TRUE(ASTChecker((*node1.body)[2]).isLast());

  auto node2 = std::get<ASTNodeFnDecl>(*nodes[1].body);
  EXPECT_EQ(node2.body, std::nullopt);

  auto node3 = (*node1.body)[0];
  node3.parent = &nodes[1];
  EXPECT_FALSE(ASTChecker(node3).isLast());
}

TEST_F(ASTCheckerTest, IsLastOnIf) {
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

TEST_F(ASTCheckerTest, IsLastOnElif) {
  auto ast = testing::NiceMock<MockAST>(
    "if true {" EOL
    "} elif false {" EOL
    "  a := 1" EOL
    "  if 1 > 2 {" EOL
    "    return" EOL
    "  }" EOL
    "  return" EOL
    "}" EOL
  );

  auto nodes = ast.gen();
  auto node = std::get<ASTNodeIf>(*nodes[0].body);
  auto nodeElif = std::get<ASTNodeIf>(*std::get<ASTNode>(*node.alt).body);

  EXPECT_FALSE(ASTChecker(nodeElif.body[0]).isLast());
  EXPECT_FALSE(ASTChecker(nodeElif.body[1]).isLast());
  EXPECT_TRUE(ASTChecker(nodeElif.body[2]).isLast());
}

TEST_F(ASTCheckerTest, IsLastOnElse) {
  auto ast = testing::NiceMock<MockAST>(
    "if true {" EOL
    "} else {" EOL
    "  a := 1" EOL
    "  if 1 > 2 {" EOL
    "    return" EOL
    "  }" EOL
    "  return" EOL
    "}" EOL
  );

  auto nodes = ast.gen();
  auto node = std::get<ASTNodeIf>(*nodes[0].body);
  auto nodeElse = std::get<ASTBlock>(*node.alt);

  EXPECT_FALSE(ASTChecker(nodeElse[0]).isLast());
  EXPECT_FALSE(ASTChecker(nodeElse[1]).isLast());
  EXPECT_TRUE(ASTChecker(nodeElse[2]).isLast());
}

TEST_F(ASTCheckerTest, IsLastOnIfWithElse) {
  auto ast = testing::NiceMock<MockAST>(
    "if true {" EOL
    "  a := 1" EOL
    "  if 1 > 2 {" EOL
    "    return" EOL
    "  }" EOL
    "  return" EOL
    "} else {" EOL
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

TEST_F(ASTCheckerTest, IsLastOnLoop) {
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

TEST_F(ASTCheckerTest, IsLastOnMain) {
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

TEST_F(ASTCheckerTest, IsLastOnObjDecl) {
  auto ast = testing::NiceMock<MockAST>(
    "obj Test {" EOL
    "  fn test1 () {" EOL
    "    a := 1" EOL
    "    if 1 > 2 {" EOL
    "      return" EOL
    "    }" EOL
    "    return" EOL
    "  }" EOL
    "  fn test2 () {" EOL
    "    a := 1" EOL
    "    if 1 > 2 {" EOL
    "      return" EOL
    "    }" EOL
    "    return" EOL
    "  }" EOL
    "  fn test1 () void" EOL
    "  fn test2 () void" EOL
    "}" EOL
    "obj Test2 {}" EOL
  );

  auto nodes = ast.gen();
  auto node = std::get<ASTNodeObjDecl>(*nodes[0].body);

  for (auto i = static_cast<std::size_t>(0); i < node.methods.size(); i++) {
    const auto &method = node.methods[i];

    if (i < 2) {
      EXPECT_NE(method.body, std::nullopt);
      EXPECT_FALSE(ASTChecker((*method.body)[0]).isLast());
      EXPECT_FALSE(ASTChecker((*method.body)[1]).isLast());
      EXPECT_TRUE(ASTChecker((*method.body)[2]).isLast());
    } else {
      EXPECT_EQ(method.body, std::nullopt);
    }
  }

  auto methodNode = (*node.methods[0].body)[0];
  methodNode.parent = &nodes[1];
  EXPECT_FALSE(ASTChecker(methodNode).isLast());
}

TEST_F(ASTCheckerTest, IsLastOnTry) {
  auto ast = testing::NiceMock<MockAST>(
    "try {" EOL
    "  a := 1" EOL
    "  if 1 > 2 {" EOL
    "    return" EOL
    "  }" EOL
    "  return" EOL
    "} catch err: error_Error {" EOL
    "}" EOL
  );

  auto nodes = ast.gen();
  auto node = std::get<ASTNodeTry>(*nodes[0].body);

  EXPECT_FALSE(ASTChecker(node.body[0]).isLast());
  EXPECT_FALSE(ASTChecker(node.body[1]).isLast());
  EXPECT_TRUE(ASTChecker(node.body[2]).isLast());
}

TEST_F(ASTCheckerTest, IsLastOnTryHandler) {
  auto ast = testing::NiceMock<MockAST>(
    "try {" EOL
    "} catch err: error_Error {" EOL
    "  a := 1" EOL
    "  if 1 > 2 {" EOL
    "    return" EOL
    "  }" EOL
    "  return" EOL
    "}" EOL
  );

  auto nodes = ast.gen();
  auto node = std::get<ASTNodeTry>(*nodes[0].body);

  EXPECT_FALSE(ASTChecker(node.handlers[0].body[0]).isLast());
  EXPECT_FALSE(ASTChecker(node.handlers[0].body[1]).isLast());
  EXPECT_TRUE(ASTChecker(node.handlers[0].body[2]).isLast());
}

TEST_F(ASTCheckerTest, IsLastWithBackup) {
  auto ast = testing::NiceMock<MockAST>("() -> int { a := 1; return a }");
  auto nodes = ast.gen();
  auto nodeExpr = std::get<ASTNodeExpr>(*nodes[0].body);
  auto exprClosure = std::get<ASTExprClosure>(*nodeExpr.body);

  EXPECT_FALSE(ASTChecker(exprClosure.body[0]).isLast(exprClosure.body));
  EXPECT_TRUE(ASTChecker(exprClosure.body[1]).isLast(exprClosure.body));
}

TEST_F(ASTCheckerTest, ParentIsEmpty) {
  auto nodes = testing::NiceMock<MockAST>("main {}" EOL).gen();
  auto nodeMain = std::get<ASTNodeMain>(*nodes[0].body);
  EXPECT_FALSE(ASTChecker(nodeMain.body).parentIs<ASTNodeFnDecl>());
}

TEST_F(ASTCheckerTest, ParentIsRoot) {
  auto nodes = testing::NiceMock<MockAST>("const A := 1" EOL).gen();
  EXPECT_FALSE(ASTChecker(nodes[0]).parentIs<ASTNodeFnDecl>());
}

TEST_F(ASTCheckerTest, ParentIsMultipleParent) {
  auto nodes = testing::NiceMock<MockAST>("main { loop { if true { a := 1 } } }" EOL).gen();
  auto nodeMain = std::get<ASTNodeMain>(*nodes[0].body);
  auto nodeLoop = std::get<ASTNodeLoop>(*nodeMain.body[0].body);
  auto nodeIf = std::get<ASTNodeIf>(*nodeLoop.body[0].body);

  EXPECT_FALSE(ASTChecker(nodeIf.body[0]).parentIs<ASTNodeFnDecl>());
}

TEST_F(ASTCheckerTest, ParentIsMultipleParent2) {
  auto nodes = testing::NiceMock<MockAST>("fn test () { loop { if true { a := 1 } } }" EOL).gen();
  auto nodeFnDecl = std::get<ASTNodeFnDecl>(*nodes[0].body);
  auto nodeLoop = std::get<ASTNodeLoop>(*(*nodeFnDecl.body)[0].body);
  auto nodeIf = std::get<ASTNodeIf>(*nodeLoop.body[0].body);

  EXPECT_TRUE(ASTChecker(nodeIf.body[0]).parentIs<ASTNodeFnDecl>());
}

TEST_F(ASTCheckerTest, ParentIsOnFn) {
  auto nodes = testing::NiceMock<MockAST>("fn test () int { return 1 }" EOL).gen();
  auto nodeFnDecl = std::get<ASTNodeFnDecl>(*nodes[0].body);

  EXPECT_TRUE(ASTChecker((*nodeFnDecl.body)[0]).parentIs<ASTNodeFnDecl>());
}

TEST_F(ASTCheckerTest, ParentIsOnMain) {
  auto nodes = testing::NiceMock<MockAST>("main { a := 1 }" EOL).gen();
  auto nodeMain = std::get<ASTNodeMain>(*nodes[0].body);

  EXPECT_FALSE(ASTChecker(nodeMain.body[0]).parentIs<ASTNodeFnDecl>());
}

TEST_F(ASTCheckerTest, ParentIsOnObjMethod) {
  auto nodes = testing::NiceMock<MockAST>("obj Test { fn test () int { return 1 } }" EOL).gen();
  auto nodeObjDecl = std::get<ASTNodeObjDecl>(*nodes[0].body);

  EXPECT_FALSE(ASTChecker((*nodeObjDecl.methods[0].body)[0]).parentIs<ASTNodeFnDecl>());
}

TEST_F(ASTCheckerTest, ThrowsOnManyIsLast) {
  auto code =
    "  a := 1" EOL
    "  if 1 > 2 {" EOL
    "    return" EOL
    "  }" EOL
    "  return" EOL;

  EXPECT_THROW_WITH_MESSAGE(this->node_(code).isLast(), "tried isLast on many nodes");
}

TEST_F(ASTCheckerTest, ThrowsOnRootIsLast) {
  auto nodes = testing::NiceMock<MockAST>("main {}").gen();
  EXPECT_THROW_WITH_MESSAGE(ASTChecker(nodes[0]).isLast(), "tried isLast on root node");
}

TEST_F(ASTCheckerTest, ThrowsOnUnknown) {
  auto ast = testing::NiceMock<MockAST>("() -> int { a := 1; return a }");
  auto nodes = ast.gen();
  auto nodeExpr = std::get<ASTNodeExpr>(*nodes[0].body);
  auto exprClosure = std::get<ASTExprClosure>(*nodeExpr.body);

  EXPECT_THROW_WITH_MESSAGE(ASTChecker(exprClosure.body[0]).isLast(), "tried isLast on unknown node");
}

TEST_F(ASTCheckerTest, ThrowsExpr) {
  EXPECT_TRUE(this->expr_("a := [0]", "[a.first, 2, 3]").throws());
  EXPECT_TRUE(this->expr_("a := [\"val1\"]", "{\"key1\": a.first, \"key2\": \"val2\"}").throws());
  EXPECT_TRUE(this->expr_("obj Test { a: int }; a := [1]", "Test{a: a.first}").throws());
  EXPECT_TRUE(this->expr_("test := [\"1\"]", "test.first.toInt").throws());

  EXPECT_TRUE(this->expr_("a: int[]", "a[1]").throws());
  EXPECT_FALSE(this->expr_("obj Test { b: str }; a := Test{}", "a.b").throws());
  EXPECT_FALSE(this->expr_("a: str; refA := ref a", "refA.empty").throws());
  EXPECT_TRUE(this->expr_("a: int[]; refA := ref a", "refA.first").throws());
  EXPECT_FALSE(this->expr_("obj Test { b: int }; a: Test", "a.b").throws());

  EXPECT_TRUE(this->expr_("obj Test { a: int }; test := Test{}; a := [1]", "test.a = a.first").throws());
  EXPECT_TRUE(this->expr_("obj Test { a: int[] }; test := Test{}; mut a := 0", "a = test.a.first").throws());

  EXPECT_TRUE(this->expr_("obj Test { a: int[] }; test := Test{}; mut a := 0", "a + test.a.first").throws());
  EXPECT_TRUE(this->expr_("obj Test { a: int[] }; test := Test{}; mut a := 0", "test.a.first + a").throws());

  EXPECT_TRUE(this->expr_("test: int[]", "test.contains(test.first)").throws());
  EXPECT_TRUE(this->expr_("testArr: str[] fn test (a: str, b: str) {}; t := ref test", "t(testArr.first, \"test2\")").throws());
  EXPECT_TRUE(this->expr_("a := [[\"1\"], [\"2\", \"3\"], [\"6\", \"7\", \"8\"]]", "a.first.reverse()").throws());

  EXPECT_TRUE(this->expr_("a: int?[]", "a.first ? 1 : 0").throws());
  EXPECT_TRUE(this->expr_("obj Test { b: int[] }; a := Test{}", "true ? a.b.first : 1").throws());
  EXPECT_TRUE(this->expr_("obj Test { b: int[] }; a := Test{}", "true ? 1 : a.b.first").throws());

  EXPECT_TRUE(this->expr_("a: int?[]", "a.first is int?").throws());
  EXPECT_TRUE(this->expr_("a: int[]", "ref a.first").throws());
  EXPECT_TRUE(this->expr_("a: int[]", "!a.first").throws());
}

TEST_F(ASTCheckerTest, ThrowsNode) {
  EXPECT_TRUE(this->node_("throw error_NewError(\"test\")").throws());
  EXPECT_TRUE(this->node_("try {} catch err: error_Error {}").throws());

  EXPECT_FALSE(this->node_("enum Color { Red = [1].first }").throws());
  EXPECT_FALSE(this->node_("fn test () { throw error_NewError(\"test\") }").throws());
  EXPECT_TRUE(this->node_("[1].first").throws());
  EXPECT_FALSE(this->node_("fn test (arr := [1], a := arr.first) {}").throws());
  EXPECT_TRUE(this->node_("if [1].first == 1 {}").throws());
  EXPECT_TRUE(this->node_("if true { a := [1].first }").throws());
  EXPECT_TRUE(this->node_("if true {} else { a := [1].first }").throws());
  EXPECT_TRUE(this->node_("if true {} elif [1].first == 1 {}").throws());
  EXPECT_TRUE(this->node_("if true {} elif false { a := [1].first }").throws());

  EXPECT_TRUE(this->node_("loop { [1].first }").throws());
  EXPECT_TRUE(this->node_("loop [1].first == 1 {}").throws());
  EXPECT_TRUE(this->node_("loop a := [1].first; a < 1; a++ {}").throws());
  EXPECT_TRUE(this->node_("loop a := [1]; a.first < 1; a += \"a\" {}").throws());
  EXPECT_TRUE(this->node_("loop a := 0; [1].first < 1; a++ {}").throws());
  EXPECT_TRUE(this->node_("loop a := 0; a < 1; a += [1].first {}").throws());

  EXPECT_FALSE(this->node_("obj Test { fn test (a := [1].first) {} }").throws());
  EXPECT_FALSE(this->node_("obj Test { fn test () { [1].first } }").throws());

  EXPECT_TRUE(this->node_("return [1].first").throws());

  EXPECT_TRUE(this->node_("a := [1].first").throws());
  EXPECT_TRUE(this->node_("a: int? = [1].first").throws());
}

TEST_F(ASTCheckerTest, ThrowsNodeMain) {
  auto parser = testing::NiceMock<MockParser>("main { throw error_NewError(\"test\") }");
  auto ast = AST(&parser);
  auto nodes = ast.gen();
  EXPECT_TRUE(ASTChecker(nodes).throws());
}

TEST_F(ASTCheckerTest, ThrowsPossibleExprAccess) {
  EXPECT_TRUE(this->expr_("", "print()").throwsPossible());
  EXPECT_TRUE(this->expr_("a: int[]", "a[0]").throwsPossible());
  EXPECT_TRUE(this->expr_("a: int[]", "a.first").throwsPossible());
  EXPECT_TRUE(this->expr_("fn test() {}", "test()").throwsPossible());
  EXPECT_TRUE(this->node_("print()").throwsPossible());
  EXPECT_TRUE(this->node_("a: int[]; a[0]").throwsPossible());
  EXPECT_TRUE(this->node_("a: int[]; a.first").throwsPossible());
  EXPECT_TRUE(this->node_("fn test() {}; test()").throwsPossible());
  EXPECT_FALSE(this->node_("async fn test() {}; await test()").throwsPossible());
}
