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
  auto param = testing::TestWithParam<const char *>::GetParam();
  auto sections = readTestFile("ast", param, {"stdin", "stdout"});
  auto parser = testing::NiceMock<MockParser>(sections["stdin"]);
  auto ast = AST(&parser);

  EXPECT_EQ(sections["stdout"], ast.xml());
}

TEST_P(ASTThrowTest, Throws) {
  auto param = testing::TestWithParam<const char *>::GetParam();
  auto sections = readTestFile("ast", param, {"stdin", "stderr"});
  auto parser = testing::NiceMock<MockParser>(sections["stdin"]);
  auto ast = AST(&parser);

  EXPECT_THROW_WITH_MESSAGE(ast.xml(), sections["stderr"]);
}

INSTANTIATE_TEST_SUITE_P(Node, ASTPassTest, testing::Values(
  "empty",
  "node-break",
  "node-continue",
  "node-expr",
  "node-fn-decl",
  "node-fn-decl-param-init",
  "node-fn-decl-param-mut",
  "node-fn-decl-param-mut-obj",
  "node-fn-decl-param-mut-fn",
  "node-fn-decl-stack",
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
  "expr-call-args",
  "expr-cond",
  "expr-lit",
  "expr-obj",
  "expr-unary"
));

INSTANTIATE_TEST_SUITE_P(Type, ASTPassTest, testing::Values(
  "type",
  "type-fn",
  "type-variadic"
));

INSTANTIATE_TEST_SUITE_P(, ASTThrowTest, testing::Values(
  "throw-E1001-expr-access-non-existing-prop",
  "throw-E1001-expr-access-non-existing-fn-prop",
  "throw-E1001-expr-access-non-existing-obj-prop",
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
