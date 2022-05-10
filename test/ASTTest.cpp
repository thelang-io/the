/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include "../src/AST.hpp"
#include "../src/config.hpp"
#include "MockParser.hpp"
#include "utils.hpp"

class ASTPassTest : public testing::TestWithParam<const char *> {
};

class ASTThrowTest : public testing::TestWithParam<const char *> {
};

TEST_P(ASTPassTest, Passes) {
  auto testContent = readTestFile("ast", testing::TestWithParam<const char *>::GetParam());
  auto stdinDelimiter = std::string("======= stdin =======" EOL);

  if (!testContent.starts_with(stdinDelimiter)) {
    throw Error("AST pass test file doesn't look like an actual test");
  }

  testContent = testContent.substr(stdinDelimiter.size());
  auto stdoutDelimiter = std::string("======= stdout =======" EOL);
  auto stdoutDelimiterPos = testContent.find(stdoutDelimiter);

  if (stdoutDelimiterPos == std::string::npos) {
    throw Error("AST pass test file doesn't have a delimiter");
  }

  auto testStdin = testContent.substr(0, stdoutDelimiterPos);
  auto expectedOutput = testContent.substr(stdoutDelimiterPos + stdoutDelimiter.size());
  auto parser = testing::NiceMock<MockParser>(testStdin);
  auto ast = AST(&parser);

  EXPECT_EQ(expectedOutput, ast.xml());
}

TEST_P(ASTThrowTest, Throws) {
  auto testContent = readTestFile("ast", testing::TestWithParam<const char *>::GetParam());
  auto stdinDelimiter = std::string("======= stdin =======" EOL);

  if (!testContent.starts_with(stdinDelimiter)) {
    throw Error("AST throw test file doesn't look like an actual test");
  }

  testContent = testContent.substr(stdinDelimiter.size());
  auto stderrDelimiter = std::string("======= stderr =======" EOL);
  auto stderrDelimiterPos = testContent.find(stderrDelimiter);

  if (stderrDelimiterPos == std::string::npos) {
    throw Error("AST throw test file doesn't have a delimiter");
  }

  auto testStdin = testContent.substr(0, stderrDelimiterPos - std::string(EOL).size());
  testContent = testContent.substr(stderrDelimiterPos + stderrDelimiter.size());
  auto expectedOutput = testContent.substr(0, testContent.size() - std::string(EOL).size());
  auto parser = testing::NiceMock<MockParser>(testStdin);
  auto ast = AST(&parser);

  EXPECT_THROW_WITH_MESSAGE(ast.xml(), expectedOutput);
}

INSTANTIATE_TEST_SUITE_P(Node, ASTPassTest, testing::Values(
  "empty",
  "node-break",
  "node-continue",
  "node-expr",
  "node-fn-decl",
  "node-fn-decl-param-init",
  "node-if",
  "node-loop",
  "node-main",
  "node-obj-decl",
  "node-obj-decl-forward-decl",
  "node-return",
  "node-var-decl",
  "node-var-decl-type-only"
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
  "throw-E1000-expr-access-non-obj-prop-member",
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
  "throw-E1010-expr-obj-undeclared-type",
  "throw-E1010-node-fn-decl-undeclared-param-type",
  "throw-E1010-node-fn-decl-undeclared-return-type",
  "throw-E1010-node-obj-decl-undeclared-field-type",
  "throw-E1011-expr-assign-undeclared-var",
  "throw-E1011-expr-binary-undeclared-var",
  "throw-E1011-expr-call-undeclared-var",
  "throw-E1011-expr-cond-undeclared-alt-var",
  "throw-E1011-expr-cond-undeclared-body-var",
  "throw-E1011-expr-cond-undeclared-var",
  "throw-E1011-expr-unary-undeclared-var",
  "throw-E1011-node-fn-decl-undeclared-param-var"
));
