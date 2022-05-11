/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include "../src/Parser.hpp"
#include "../src/config.hpp"
#include "MockLexer.hpp"
#include "utils.hpp"

class ParserPassTest : public testing::TestWithParam<const char *> {
};

class ParserThrowTest : public testing::TestWithParam<const char *> {
};

TEST_P(ParserPassTest, Passes) {
  auto testContent = readTestFile("parser", testing::TestWithParam<const char *>::GetParam());
  auto stdinDelimiter = std::string("======= stdin =======" EOL);

  if (!testContent.starts_with(stdinDelimiter)) {
    throw Error("Parser pass test file doesn't look like an actual test");
  }

  testContent.erase(0, stdinDelimiter.size());
  auto stdoutDelimiter = std::string("======= stdout =======" EOL);
  auto stdoutDelimiterPos = testContent.find(stdoutDelimiter);

  if (stdoutDelimiterPos == std::string::npos) {
    throw Error("Parser pass test file doesn't have a delimiter");
  }

  auto testStdin = testContent.substr(0, stdoutDelimiterPos);
  auto expectedStdout = testContent.substr(stdoutDelimiterPos + stdoutDelimiter.size());
  auto lexer = testing::NiceMock<MockLexer>(testStdin);
  auto parser = Parser(&lexer);

  EXPECT_EQ(expectedStdout, parser.xml());
}

TEST_P(ParserThrowTest, Throws) {
  auto testContent = readTestFile("parser", testing::TestWithParam<const char *>::GetParam());
  auto stdinDelimiter = std::string("======= stdin =======" EOL);

  if (!testContent.starts_with(stdinDelimiter)) {
    throw Error("Parser throw test file doesn't look like an actual test");
  }

  testContent.erase(0, stdinDelimiter.size());
  auto stderrDelimiter = std::string("======= stderr =======" EOL);
  auto stderrDelimiterPos = testContent.find(stderrDelimiter);

  if (stderrDelimiterPos == std::string::npos) {
    throw Error("Parser throw test file doesn't have a delimiter");
  }

  auto testStdin = testContent.substr(0, stderrDelimiterPos - std::string(EOL).size());
  testContent.erase(0, stderrDelimiterPos + stderrDelimiter.size());
  auto expectedStderr = testContent.substr(0, testContent.size() - std::string(EOL).size());
  auto lexer = testing::NiceMock<MockLexer>(testStdin);
  auto parser = Parser(&lexer);

  EXPECT_THROW_WITH_MESSAGE(parser.xml(), expectedStderr);
}

INSTANTIATE_TEST_SUITE_P(General, ParserPassTest, testing::Values(
  "stmt-eof",
  "stmt-expr",
  "stmt-break",
  "stmt-continue"
));

INSTANTIATE_TEST_SUITE_P(StmtFnDecl, ParserPassTest, testing::Values(
  "stmt-fn-decl-empty",
  "stmt-fn-decl",
  "stmt-fn-decl-param-init",
  "stmt-fn-decl-obj",
  "stmt-fn-decl-recursive",
  "stmt-fn-decl-nested",
  "stmt-fn-decl-scoped",
  "stmt-fn-decl-param-fn",
  "stmt-fn-decl-variadic"
));

INSTANTIATE_TEST_SUITE_P(StmtIf, ParserPassTest, testing::Values(
  "stmt-if-empty",
  "stmt-if",
  "stmt-if-single-elif",
  "stmt-if-else",
  "stmt-if-multi-elif",
  "stmt-if-nested"
));

INSTANTIATE_TEST_SUITE_P(StmtLoop, ParserPassTest, testing::Values(
  "stmt-loop-for-empty",
  "stmt-loop",
  "stmt-loop-mut",
  "stmt-loop-empty-init-condition",
  "stmt-loop-empty-init-update",
  "stmt-loop-empty-update",
  "stmt-loop-empty-body",
  "stmt-loop-nested",
  "stmt-loop-while-empty",
  "stmt-loop-while",
  "stmt-loop-while-nested"
));

INSTANTIATE_TEST_SUITE_P(StmtMain, ParserPassTest, testing::Values(
  "stmt-main-empty",
  "stmt-main"
));

INSTANTIATE_TEST_SUITE_P(StmtObjDecl, ParserPassTest, testing::Values(
  "stmt-obj-decl",
  "stmt-obj-decl-recursive",
  "stmt-obj-decl-scoped",
  "stmt-obj-decl-field-fn"
));

INSTANTIATE_TEST_SUITE_P(StmtReturn, ParserPassTest, testing::Values(
  "stmt-return",
  "stmt-return-no-arg"
));

INSTANTIATE_TEST_SUITE_P(StmtVarDecl, ParserPassTest, testing::Values(
  "stmt-var-decl",
  "stmt-var-decl-mut",
  "stmt-var-decl-no-init",
  "stmt-var-decl-no-init-mut",
  "stmt-var-decl-short",
  "stmt-var-decl-short-mut",
  "stmt-var-decl-fn"
));

INSTANTIATE_TEST_SUITE_P(Expr, ParserPassTest, testing::Values(
  "expr-access",
  "expr-access-member",
  "expr-assign",
  "expr-assign-member",
  "expr-assign-op",
  "expr-binary",
  "expr-binary-nested",
  "expr-binary-str",
  "expr-call",
  "expr-call-member",
  "expr-cond",
  "expr-cond-nested",
  "expr-lit",
  "expr-lit-esc",
  "expr-obj",
  "expr-obj-nested",
  "expr-unary",
  "expr-unary-nested"
));

INSTANTIATE_TEST_SUITE_P(General, ParserThrowTest, testing::Values(
  "throw-E0100-unexpected-statement",
  "throw-E0100-unexpected-mut-keyword",
  "throw-E0103-missing-lbrace",
  "throw-E0104-missing-rbrace",
  "throw-E0136-stmt-expr-missing-expr"
));

INSTANTIATE_TEST_SUITE_P(StmtFnDecl, ParserThrowTest, testing::Values(
  "throw-E0115-stmt-fn-decl-unexpected-id",
  "throw-E0116-stmt-fn-decl-missing-lpar",
  "throw-E0117-stmt-fn-decl-missing-param-name",
  "throw-E0117-stmt-fn-decl-multi-missing-param-name",
  "throw-E0118-stmt-fn-decl-missing-param-type",
  "throw-E0118-stmt-fn-decl-missing-type-param-type",
  "throw-E0119-stmt-fn-decl-missing-param-type",
  "throw-E0120-stmt-fn-decl-missing-type-return-type",
  "throw-E0127-stmt-fn-decl-missing-param-type-rpar",
  "throw-E0128-stmt-fn-decl-default-variadic-param",
  "throw-E0118-stmt-fn-decl-missing-param-parenthesized-type",
  "throw-E0118-stmt-fn-decl-missing-type-param-parenthesized-type",
  "throw-E0120-stmt-fn-decl-missing-type-parenthesized-return-type",
  "throw-E0129-stmt-fn-decl-missing-type-lpar",
  "throw-E0130-stmt-fn-decl-missing-param-init",
  "throw-E0130-stmt-fn-decl-missing-param-init-after-type"
));

INSTANTIATE_TEST_SUITE_P(StmtLoop, ParserThrowTest, testing::Values(
  "throw-E0105-stmt-loop-unexpected-init",
  "throw-E0106-stmt-loop-semi-after-init",
  "throw-E0107-stmt-loop-unexpected-token-after-init",
  "throw-E0108-stmt-loop-semi-after-condition"
));

INSTANTIATE_TEST_SUITE_P(StmtObjDecl, ParserThrowTest, testing::Values(
  "throw-E0121-stmt-obj-decl-missing-id",
  "throw-E0122-stmt-obj-decl-missing-lbrace",
  "throw-E0123-stmt-obj-decl-missing-field-name",
  "throw-E0123-stmt-obj-decl-missing-multi-field-name",
  "throw-E0124-stmt-obj-decl-missing-colon-after-field-name",
  "throw-E0125-stmt-obj-decl-missing-field-type",
  "throw-E0126-stmt-obj-decl-empty",
  "throw-E0127-stmt-obj-decl-missing-field-type-rpar",
  "throw-E0125-stmt-obj-decl-missing-field-parenthesized-type"
));

INSTANTIATE_TEST_SUITE_P(StmtVarDecl, ParserThrowTest, testing::Values(
  "throw-E0102-stmt-var-decl-missing-type",
  "throw-E0102-stmt-var-decl-mut-missing-type",
  "throw-E0127-stmt-var-decl-missing-type-rpar",
  "throw-E0102-stmt-var-decl-missing-parenthesized-type",
  "throw-E0102-stmt-var-decl-mut-missing-parenthesized-type",
  "throw-E0131-var-decl-missing-init",
  "throw-E0131-var-decl-missing-init-after-type",
  "throw-E0131-var-decl-mut-missing-init",
  "throw-E0131-var-decl-mut-missing-init-after-type"
));

INSTANTIATE_TEST_SUITE_P(Expr, ParserThrowTest, testing::Values(
  "throw-E0109-missing-rpar",
  "throw-E0110-expr-access-missing-dot",
  "throw-E0111-expr-cond-missing-colon",
  "throw-E0112-expr-obj-missing-prop-name",
  "throw-E0113-expr-obj-missing-colon",
  "throw-E0114-expr-obj-member-id",
  "throw-E0132-expr-unary-missing-arg",
  "throw-E0133-expr-assign-missing-value",
  "throw-E0134-expr-obj-missing-prop-init",
  "throw-E0135-expr-call-missing-arg",
  "throw-E0137-expr-binary-missing-right-expr",
  "throw-E0138-expr-cond-missing-body",
  "throw-E0139-expr-cond-missing-alt"
));
