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
#include <filesystem>
#include "../src/AST.hpp"
#include "MockParser.hpp"
#include "utils.hpp"

class ASTTest : public testing::Test {
 protected:
  std::shared_ptr<AST> ast_;

  ASTNode node_ (const std::string &code) {
    auto p = testing::NiceMock<MockParser>("const TestExpr := 0; type Alias = str; " + code);
    this->ast_ = std::make_shared<AST>(&p);
    return this->ast_->gen()[2];
  }
};

TEST_F(ASTTest, GetExportCodeName) {
  EXPECT_EQ(AST::getExportCodeName(this->node_("1")), "");

  EXPECT_EQ(AST::getExportCodeName(this->node_("enum Test1 { One, Two, Three }")), "test_test_Test1_0");
  EXPECT_EQ(AST::getExportCodeName(this->node_("TestExpr")), "TestExpr_0");
  EXPECT_EQ(AST::getExportCodeName(this->node_("fn test3 () {}")), "test_test_test3_0");
  EXPECT_EQ(AST::getExportCodeName(this->node_("obj Test4 { a: int }")), "test_test_Test4_0");
  EXPECT_EQ(AST::getExportCodeName(this->node_("type Test5 = str")), "test_test_Test5_0");
  EXPECT_EQ(AST::getExportCodeName(this->node_("const a: int")), "a_0");

  EXPECT_EQ(AST::getExportCodeName(this->node_("export enum Test1 { One, Two, Three }")), "test_test_Test1_0");
  EXPECT_EQ(AST::getExportCodeName(this->node_("export TestExpr")), "TestExpr_0");
  EXPECT_EQ(AST::getExportCodeName(this->node_("export Alias")), "test_test_Alias_0");
  EXPECT_EQ(AST::getExportCodeName(this->node_("export fn test3 () {}")), "test_test_test3_0");
  EXPECT_EQ(AST::getExportCodeName(this->node_("export obj Test4 { a: int }")), "test_test_Test4_0");
  EXPECT_EQ(AST::getExportCodeName(this->node_("export type Test5 = str")), "test_test_Test5_0");
  EXPECT_EQ(AST::getExportCodeName(this->node_("export const a: int")), "a_0");
}

TEST_F(ASTTest, GetExportName) {
  EXPECT_EQ(AST::getExportName(this->node_("1")), "");

  EXPECT_EQ(AST::getExportName(this->node_("enum Test1 { One, Two, Three }")), "Test1");
  EXPECT_EQ(AST::getExportName(this->node_("TestExpr")), "TestExpr");
  EXPECT_EQ(AST::getExportName(this->node_("fn test3 () {}")), "test3");
  EXPECT_EQ(AST::getExportName(this->node_("obj Test4 { a: int }")), "Test4");
  EXPECT_EQ(AST::getExportName(this->node_("type Test5 = str")), "Test5");
  EXPECT_EQ(AST::getExportName(this->node_("const a: int")), "a");

  EXPECT_EQ(AST::getExportName(this->node_("export enum Test1 { One, Two, Three }")), "Test1");
  EXPECT_EQ(AST::getExportName(this->node_("export TestExpr")), "TestExpr");
  EXPECT_EQ(AST::getExportName(this->node_("export Alias")), "Alias");
  EXPECT_EQ(AST::getExportName(this->node_("export fn test3 () {}")), "test3");
  EXPECT_EQ(AST::getExportName(this->node_("export obj Test4 { a: int }")), "Test4");
  EXPECT_EQ(AST::getExportName(this->node_("export type Test5 = str")), "Test5");
  EXPECT_EQ(AST::getExportName(this->node_("export const a: int")), "a");
}

TEST_F(ASTTest, GetExportType) {
  EXPECT_EQ(AST::getExportType(this->node_("1")), nullptr);

  EXPECT_EQ(AST::getExportType(this->node_("enum Test1 { One, Two, Three }"))->name, "Test1");
  EXPECT_EQ(AST::getExportType(this->node_("TestExpr"))->name, "int");
  EXPECT_EQ(AST::getExportType(this->node_("fn test3 () {}"))->name, "fn_sFRvoidFE");
  EXPECT_EQ(AST::getExportType(this->node_("obj Test4 { a: int }"))->name, "Test4");
  EXPECT_EQ(AST::getExportType(this->node_("type Test5 = str"))->name, "Test5");
  EXPECT_EQ(AST::getExportType(this->node_("const a: int"))->name, "int");

  EXPECT_EQ(AST::getExportType(this->node_("export enum Test1 { One, Two, Three }"))->name, "Test1");
  EXPECT_EQ(AST::getExportType(this->node_("export TestExpr"))->name, "int");
  EXPECT_EQ(AST::getExportType(this->node_("export Alias"))->name, "Alias");
  EXPECT_EQ(AST::getExportType(this->node_("export fn test3 () {}"))->name, "fn_sFRvoidFE");
  EXPECT_EQ(AST::getExportType(this->node_("export obj Test4 { a: int }"))->name, "Test4");
  EXPECT_EQ(AST::getExportType(this->node_("export type Test5 = str"))->name, "Test5");
  EXPECT_EQ(AST::getExportType(this->node_("export const a: int"))->name, "int");
}

TEST_F(ASTTest, GetExportVar) {
  EXPECT_EQ(AST::getExportVar(this->node_("1")), nullptr);
  EXPECT_EQ(AST::getExportVar(this->node_("enum Test1 { One, Two, Three }")), nullptr);
  EXPECT_EQ(AST::getExportVar(this->node_("obj Test4 { a: int }")), nullptr);
  EXPECT_EQ(AST::getExportVar(this->node_("type Test5 = str")), nullptr);
  EXPECT_EQ(AST::getExportVar(this->node_("export enum Test1 { One, Two, Three }")), nullptr);
  EXPECT_EQ(AST::getExportVar(this->node_("export Alias")), nullptr);
  EXPECT_EQ(AST::getExportVar(this->node_("export obj Test4 { a: int }")), nullptr);
  EXPECT_EQ(AST::getExportVar(this->node_("export type Test5 = str")), nullptr);

  EXPECT_EQ(AST::getExportVar(this->node_("TestExpr"))->name, "TestExpr");
  EXPECT_EQ(AST::getExportVar(this->node_("fn test3 () {}"))->name, "test3");
  EXPECT_EQ(AST::getExportVar(this->node_("const a: int"))->name, "a");

  EXPECT_EQ(AST::getExportVar(this->node_("export TestExpr"))->name, "TestExpr");
  EXPECT_EQ(AST::getExportVar(this->node_("export fn test3 () {}"))->name, "test3");
  EXPECT_EQ(AST::getExportVar(this->node_("export const a: int"))->name, "a");
}

class ASTPassTest : public testing::TestWithParam<const char *> {
 protected:
  std::filesystem::path initialCwd_;

  void SetUp () override {
    this->initialCwd_ = std::filesystem::current_path();
  }

  void TearDown () override {
    std::filesystem::current_path(this->initialCwd_);
  }
};

class ASTThrowTest : public testing::TestWithParam<const char *> {
 protected:
  std::filesystem::path initialCwd_;

  void SetUp () override {
    this->initialCwd_ = std::filesystem::current_path();
  }

  void TearDown () override {
    std::filesystem::current_path(this->initialCwd_);
  }
};

TEST_P(ASTPassTest, Passes) {
  auto param = testing::TestWithParam<const char *>::GetParam();
  auto sections = readTestFile("ast", param, {"stdin", "stdout"});
  auto parser = testing::NiceMock<MockParser>(sections["stdin"]);
  std::filesystem::current_path(this->initialCwd_ / "test");
  auto ast = AST(&parser);

  EXPECT_EQ(sections["stdout"], prepareTestOutputFrom(ast.xml()));
}

TEST_P(ASTThrowTest, Throws) {
  auto param = testing::TestWithParam<const char *>::GetParam();
  auto sections = readTestFile("ast", param, {"stdin", "stderr"});
  auto parser = testing::NiceMock<MockParser>(sections["stdin"]);
  std::filesystem::current_path(this->initialCwd_ / "test");
  auto ast = AST(&parser);

  EXPECT_THROW_WITH_MESSAGE(ast.xml(), prepareTestOutput(sections["stderr"]));
}

INSTANTIATE_TEST_SUITE_P(Node, ASTPassTest, testing::Values(
  "empty",
  "semi",
  "node-break",
  "node-continue",
  "node-empty",
  "node-enum-decl",
  "node-export",
  "node-expr",
  "node-expr-lit",
  "node-fn-decl",
  "node-fn-decl-body",
  "node-fn-decl-param-init",
  "node-fn-decl-param-mut",
  "node-fn-decl-param-mut-fn",
  "node-fn-decl-param-mut-map",
  "node-fn-decl-param-mut-obj",
  "node-fn-decl-param-variadic",
  "node-fn-decl-stack",
  "node-fn-decl-async",
  "node-fn-decl-throw",
  "node-if",
  "node-if-type-casts",
  "node-if-type-casts-elif",
  "node-import",
  "node-import-package",
  "node-loop",
  "node-main",
  "node-obj-decl",
  "node-obj-decl-empty",
  "node-obj-decl-field-mut",
  "node-obj-decl-field-self",
  "node-obj-decl-field-variadic",
  "node-obj-decl-forward-decl",
  "node-obj-decl-method",
  "node-obj-decl-method-body",
  "node-obj-decl-method-variadic",
  "node-obj-decl-method-async",
  "node-obj-decl-method-throw",
  "node-return",
  "node-throw",
  "node-try",
  "node-type-decl",
  "node-var-decl",
  "node-var-decl-const",
  "node-var-decl-type-only"
));

INSTANTIATE_TEST_SUITE_P(Expr, ASTPassTest, testing::Values(
  "expr-access",
  "expr-access-prop",
  "expr-access-elem",
  "expr-access-member",
  "expr-array",
  "expr-array-root",
  "expr-as",
  "expr-assign",
  "expr-await",
  "expr-binary",
  "expr-binary-type-casts",
  "expr-call",
  "expr-call-args",
  "expr-closure",
  "expr-closure-stack",
  "expr-closure-call",
  "expr-cond",
  "expr-cond-any",
  "expr-cond-map",
  "expr-cond-opt",
  "expr-cond-union",
  "expr-cond-type-casts",
  "expr-is",
  "expr-is-type-casts",
  "expr-is-type-casts2",
  "expr-lit",
  "expr-lit-nil",
  "expr-map",
  "expr-obj",
  "expr-obj-member",
  "expr-ref",
  "expr-ref-fn",
  "expr-unary"
));

INSTANTIATE_TEST_SUITE_P(Type, ASTPassTest, testing::Values(
  "type",
  "type-array",
  "type-fn",
  "type-fn-async",
  "type-map",
  "type-opt",
  "type-ref",
  "type-union",
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
  "throw-E1022-node-var-decl-const-void-type",
  "throw-E1022-node-var-decl-const-void-init",
  "throw-E1022-expr-closure-void-param-type",
  "throw-E1022-expr-closure-void-param-init",
  "throw-E1023-node-var-decl-unknown-member-type",
  "throw-E1024-expr-access-not-existing-member",
  "throw-E1024-expr-access-enum-not-existing-member",
  "throw-E1026-node-var-decl-root",
  "throw-E1025-node-var-decl-const-non-root",
  "throw-E1027-expr-map-unknown-type",
  "throw-E1028-node-throw-invalid-order-type",
  "throw-E1028-node-throw-invalid-type",
  "throw-E1028-node-throw-partial-type",
  "throw-E1029-node-try-handler-invalid-order-type",
  "throw-E1029-node-try-handler-invalid-type",
  "throw-E1029-node-try-handler-partial-type",
  "throw-E1030-expr-await-non-async",
  "throw-E1031-expr-as-incorrect-type-any",
  "throw-E1031-expr-as-incorrect-type-bool",
  "throw-E1031-expr-as-incorrect-type-enum",
  "throw-E1031-expr-as-incorrect-type-fn",
  "throw-E1031-expr-as-incorrect-type-number",
  "throw-E1031-expr-as-incorrect-type-opt",
  "throw-E1031-expr-as-incorrect-type-ref",
  "throw-E1031-expr-as-incorrect-type-union",
  "throw-E1032-node-import-circular",
  "throw-E1033-node-import-not-exported",
  "throw-E1034-expr-obj-non-existing-namespace-member",
  "throw-E1035-node-import-non-existing-package",
  "throw-E1036-node-import-outside-cwd"
));
