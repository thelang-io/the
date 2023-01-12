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
#include "../src/AST.hpp"
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
  "semi",
  "node-break",
  "node-continue",
  "node-empty",
  "node-enum-decl",
  "node-expr",
  "node-fn-decl",
  "node-fn-decl-param-init",
  "node-fn-decl-param-mut",
  "node-fn-decl-param-mut-fn",
  "node-fn-decl-param-mut-obj",
  "node-fn-decl-param-variadic",
  "node-fn-decl-stack",
  "node-if",
  "node-if-type-casts",
  "node-loop",
  "node-main",
  "node-obj-decl",
  "node-obj-decl-field-mut",
  "node-obj-decl-field-variadic",
  "node-obj-decl-forward-decl",
  "node-obj-decl-method",
  "node-obj-decl-method-variadic",
  "node-return",
  "node-var-decl",
  "node-var-decl-type-only"
));

INSTANTIATE_TEST_SUITE_P(Expr, ASTPassTest, testing::Values(
  "expr-access",
  "expr-access-prop",
  "expr-access-elem",
  "expr-access-member",
  "expr-array",
  "expr-array-root",
  "expr-assign",
  "expr-binary",
  "expr-call",
  "expr-call-args",
  "expr-cond",
  "expr-cond-any",
  "expr-cond-opt",
  "expr-cond-type-casts",
  "expr-lit",
  "expr-lit-nil",
  "expr-obj",
  "expr-ref",
  "expr-ref-fn",
  "expr-unary"
));

INSTANTIATE_TEST_SUITE_P(Type, ASTPassTest, testing::Values(
  "type",
  "type-array",
  "type-fn",
  "type-opt",
  "type-ref",
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
  "throw-E1011-node-fn-decl-undeclared-param-var",
  "throw-E1012-expr-access-expected-int",
  "throw-E1013-expr-access-expected-type-str",
  "throw-E1014-expr-call-expected-type-fn",
  "throw-E1015-expr-obj-not-existing-prop",
  "throw-E1016-expr-array-unknown-type",
  "throw-E1017-expr-array-incompatible-element-type",
  "throw-E1018-node-var-decl-unknown-nil-type",
  "throw-E1019-node-var-decl-not-assignable-to-nil",
  "throw-E1020-expr-binary-unknown-operand-type",
  "throw-E1020-expr-cond-unknown-operand-type",
  "throw-E1021-node-fn-decl-main",
  "throw-E1022-node-fn-decl-void-param-type",
  "throw-E1022-node-fn-decl-void-param-init",
  "throw-E1022-node-obj-decl-method-void-param-init",
  "throw-E1022-node-obj-decl-method-void-param-type",
  "throw-E1022-node-obj-decl-void-field-type",
  "throw-E1022-node-var-decl-void-type",
  "throw-E1022-node-var-decl-void-init",
  "throw-E1023-node-var-decl-unknown-member-type",
  "throw-E1024-expr-access-not-existing-member",
  "throw-E1024-expr-access-enum-not-existing-member"
));
