/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include "../src/AST.hpp"
#include "MockParser.hpp"
#include "utils.hpp"

class ASTPassTest : public testing::TestWithParam<const char *> {
};

class ASTThrowTest : public testing::TestWithParam<const char *> {
};

TEST_P(ASTPassTest, Passes) {
  auto testContent = readTestFile("ast", testing::TestWithParam<const char *>::GetParam());

  if (testContent.substr(0, 21) != "======= stdin =======") {
    throw Error("AST pass test file doesn't look like an actual test");
  }

  auto delimiter = std::string("\n======= stdout =======\n");
  auto delimiterPos = testContent.find(delimiter);

  if (delimiterPos == std::string::npos) {
    throw Error("AST pass test file doesn't have a delimiter");
  }

  auto testStdin = delimiterPos > 22 ? testContent.substr(22, delimiterPos - 22) : "";
  auto expectedOutput = testContent.substr(delimiterPos + delimiter.size());
  auto parser = testing::NiceMock<MockParser>(testStdin);
  auto ast = AST(&parser);
  auto actualOutput = std::string();

  for (const auto &node : ast.gen()) {
    actualOutput += node.xml() + "\n";
  }

  EXPECT_EQ(expectedOutput, actualOutput);
}

TEST_P(ASTThrowTest, Throws) {
  auto testContent = readTestFile("ast", testing::TestWithParam<const char *>::GetParam());

  if (testContent.substr(0, 21) != "======= stdin =======") {
    throw Error("AST throw test file doesn't look like an actual test");
  }

  auto delimiter = std::string("\n======= stderr =======\n");
  auto delimiterPos = testContent.find(delimiter);

  if (delimiterPos == std::string::npos) {
    throw Error("AST throw test file doesn't have a delimiter");
  }

  auto delimiterEndPos = delimiterPos + delimiter.size();
  auto testStdin = testContent.substr(22, delimiterPos - 22);
  auto expectedOutput = testContent.substr(delimiterEndPos, testContent.size() - delimiterEndPos - 1);
  auto parser = testing::NiceMock<MockParser>(testStdin);
  auto ast = AST(&parser);

  EXPECT_THROW_WITH_MESSAGE(ast.gen(), expectedOutput);
}

INSTANTIATE_TEST_SUITE_P(Node, ASTPassTest, testing::Values(
  "empty",
  "node-break",
  "node-continue",
  "node-expr",
  "node-fn-decl",
  "node-if",
  "node-loop",
  "node-main",
  "node-obj-decl",
  "node-return",
  "node-var-decl",
  "node-var-decl-type-only",
  "node-obj-decl-forward-decl"
));

INSTANTIATE_TEST_SUITE_P(Expr, ASTPassTest, testing::Values(
  "expr-access",
  "expr-assign",
  "expr-binary",
  "expr-call",
  "expr-cond",
  "expr-lit",
  "expr-obj",
  "expr-unary"
));

INSTANTIATE_TEST_SUITE_P(, ASTThrowTest, testing::Values(
  "throw-E1000-expr-access-non-obj-prop",
  "throw-E1001-expr-access-non-existing-prop",
  "throw-E1002-expr-call-extraneous-arg",
  "throw-E1003-expr-binary-wrong-str-operation",
  "throw-E1004-expr-cond-incompatible-operand-types",
  "throw-E1005-expr-call-extra-arg",
  "throw-E1005-expr-call-extra-named-arg",
  "throw-E1006-expr-call-variadic-arg-by-name",
  "throw-E1007-expr-call-regular-after-named-args",
  "throw-E1008-expr-call-incorrect-arg-type",
  "throw-E1008-expr-call-incorrect-variadic-arg-type",
  "throw-E1009-expr-call-missing-required-args",
  "throw-E1010-node-obj-decl-undeclared-type"
));
