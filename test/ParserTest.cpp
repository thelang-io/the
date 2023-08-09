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
#include "../src/Parser.hpp"
#include "MockLexer.hpp"
#include "utils.hpp"

class ParserPassTest : public testing::TestWithParam<const char *> {
};

class ParserThrowTest : public testing::TestWithParam<const char *> {
};

TEST_P(ParserPassTest, Passes) {
  auto param = testing::TestWithParam<const char *>::GetParam();
  auto sections = readTestFile("parser", param, {"stdin", "stdout"});
  auto lexer = testing::NiceMock<MockLexer>(sections["stdin"]);
  auto parser = Parser(&lexer);

  EXPECT_EQ(sections["stdout"], parser.xml());
}

TEST_P(ParserThrowTest, Throws) {
  auto param = testing::TestWithParam<const char *>::GetParam();
  auto sections = readTestFile("parser", param, {"stdin", "stderr"});
  auto lexer = testing::NiceMock<MockLexer>(sections["stdin"]);
  auto parser = Parser(&lexer);

  EXPECT_THROW_WITH_MESSAGE(parser.xml(), sections["stderr"]);
}

INSTANTIATE_TEST_SUITE_P(General, ParserPassTest, testing::Values(
  "semi",
  "stmt-empty",
  "stmt-eof",
  "stmt-expr",
  "stmt-break",
  "stmt-continue"
));

INSTANTIATE_TEST_SUITE_P(StmtEnumDecl, ParserPassTest, testing::Values(
  "stmt-enum-decl"
));

INSTANTIATE_TEST_SUITE_P(StmtExport, ParserPassTest, testing::Values(
  "stmt-export"
));

INSTANTIATE_TEST_SUITE_P(StmtFnDecl, ParserPassTest, testing::Values(
  "stmt-fn-decl-empty",
  "stmt-fn-decl-body",
  "stmt-fn-decl",
  "stmt-fn-decl-param-init",
  "stmt-fn-decl-recursive",
  "stmt-fn-decl-nested",
  "stmt-fn-decl-scoped",
  "stmt-fn-decl-param-obj",
  "stmt-fn-decl-param-fn",
  "stmt-fn-decl-param-mut",
  "stmt-fn-decl-param-mut-obj",
  "stmt-fn-decl-param-mut-fn",
  "stmt-fn-decl-variadic",
  "stmt-fn-decl-async"
));

INSTANTIATE_TEST_SUITE_P(StmtIf, ParserPassTest, testing::Values(
  "stmt-if-empty",
  "stmt-if",
  "stmt-if-single-elif",
  "stmt-if-else",
  "stmt-if-multi-elif",
  "stmt-if-nested",
  "stmt-if-complex"
));

INSTANTIATE_TEST_SUITE_P(StmtImport, ParserPassTest, testing::Values(
  "stmt-import",
  "stmt-import-namespace",
  "stmt-import-no-specifiers",
  "stmt-import-multiple-specifiers",
  "stmt-import-multiple-namespaces",
  "stmt-import-mixed"
));

INSTANTIATE_TEST_SUITE_P(StmtLoop, ParserPassTest, testing::Values(
  "stmt-loop-for-empty",
  "stmt-loop",
  "stmt-loop-mut",
  "stmt-loop-empty-init-condition",
  "stmt-loop-empty-init-update",
  "stmt-loop-empty-update",
  "stmt-loop-empty-body",
  "stmt-loop-parenthesized",
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
  "stmt-obj-decl-empty",
  "stmt-obj-decl-recursive",
  "stmt-obj-decl-scoped",
  "stmt-obj-decl-field-fn",
  "stmt-obj-decl-mut-field",
  "stmt-obj-decl-method"
));

INSTANTIATE_TEST_SUITE_P(StmtReturn, ParserPassTest, testing::Values(
  "stmt-return",
  "stmt-return-no-arg"
));

INSTANTIATE_TEST_SUITE_P(StmtThrow, ParserPassTest, testing::Values(
  "stmt-throw"
));

INSTANTIATE_TEST_SUITE_P(StmtTry, ParserPassTest, testing::Values(
  "stmt-try"
));

INSTANTIATE_TEST_SUITE_P(StmtTypeDecl, ParserPassTest, testing::Values(
  "stmt-type-decl"
));

INSTANTIATE_TEST_SUITE_P(StmtVarDecl, ParserPassTest, testing::Values(
  "stmt-var-decl",
  "stmt-var-decl-const",
  "stmt-var-decl-mut",
  "stmt-var-decl-no-init",
  "stmt-var-decl-no-init-mut",
  "stmt-var-decl-short",
  "stmt-var-decl-short-mut",
  "stmt-var-decl-fn"
));

INSTANTIATE_TEST_SUITE_P(Expr, ParserPassTest, testing::Values(
  "expr-access",
  "expr-access-prop",
  "expr-access-elem",
  "expr-access-member",
  "expr-access-associativity",
  "expr-array",
  "expr-as",
  "expr-as-precedence",
  "expr-as-associativity",
  "expr-assign",
  "expr-assign-member",
  "expr-assign-op",
  "expr-assign-precedence",
  "expr-assign-associativity",
  "expr-await",
  "expr-await-precedence",
  "expr-await-associativity",
  "expr-binary",
  "expr-binary-str",
  "expr-binary-nested",
  "expr-binary-precedence",
  "expr-binary-associativity",
  "expr-call",
  "expr-call-member",
  "expr-call-precedence",
  "expr-closure",
  "expr-closure-precedence",
  "expr-cond",
  "expr-cond-nested",
  "expr-cond-precedence",
  "expr-cond-associativity",
  "expr-is",
  "expr-is-precedence",
  "expr-is-associativity",
  "expr-lit",
  "expr-lit-esc",
  "expr-lit-nil",
  "expr-map",
  "expr-map-nested",
  "expr-map-precedence",
  "expr-obj",
  "expr-obj-empty",
  "expr-obj-nested",
  "expr-obj-precedence",
  "expr-ref",
  "expr-unary",
  "expr-unary-nested",
  "expr-unary-precedence",
  "expr-unary-associativity"
));

INSTANTIATE_TEST_SUITE_P(Type, ParserPassTest, testing::Values(
  "type",
  "type-array",
  "type-fn",
  "type-fn-async",
  "type-map",
  "type-optional",
  "type-ref",
  "type-union",
  "type-variadic"
));

INSTANTIATE_TEST_SUITE_P(General, ParserThrowTest, testing::Values(
  "throw-E0100-unexpected-statement",
  "throw-E0100-unexpected-mut-keyword",
  "throw-E0103-missing-lbrace",
  "throw-E0104-missing-rbrace",
  "throw-E0136-stmt-expr-missing-expr"
));

INSTANTIATE_TEST_SUITE_P(StmtEnumDecl, ParserThrowTest, testing::Values(
  "throw-E0154-stmt-enum-decl-missing-id",
  "throw-E0155-stmt-enum-decl-missing-lbrace",
  "throw-E0156-stmt-enum-decl-missing-member-name",
  "throw-E0157-stmt-enum-decl-missing-member-init",
  "throw-E0158-stmt-enum-decl-empty"
));

INSTANTIATE_TEST_SUITE_P(StmtExport, ParserThrowTest, testing::Values(
  "throw-E0190-stmt-export-invalid-declaration"
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
  "throw-E0130-stmt-fn-decl-missing-param-init",
  "throw-E0130-stmt-fn-decl-missing-param-init-after-type",
  "throw-E0153-stmt-fn-decl-missing-arrow"
));

INSTANTIATE_TEST_SUITE_P(StmtIf, ParserThrowTest, testing::Values(
  "throw-E0103-stmt-if-missing-lbrace",
  "throw-E0143-stmt-if-expected-expr"
));

INSTANTIATE_TEST_SUITE_P(StmtImport, ParserThrowTest, testing::Values(
  "throw-E0185-stmt-import-missing-as-after-namespace",
  "throw-E0186-stmt-import-missing-id-after-namespace-as",
  "throw-E0186-stmt-import-missing-id-after-specifier-as",
  "throw-E0187-stmt-import-missing-specifier",
  "throw-E0188-stmt-import-missing-from",
  "throw-E0189-stmt-import-missing-source"
));

INSTANTIATE_TEST_SUITE_P(StmtLoop, ParserThrowTest, testing::Values(
  "throw-E0103-stmt-loop-parenthesized-missing-lbrace",
  "throw-E0105-stmt-loop-parenthesized-unexpected-init",
  "throw-E0105-stmt-loop-unexpected-init",
  "throw-E0105-stmt-loop-unexpected-init-stmt-loop",
  "throw-E0106-stmt-loop-parenthesized-semi-after-init",
  "throw-E0106-stmt-loop-semi-after-init",
  "throw-E0108-stmt-loop-parenthesized-semi-after-condition",
  "throw-E0108-stmt-loop-parenthesized-semi-after-empty-init-condition",
  "throw-E0108-stmt-loop-semi-after-condition",
  "throw-E0108-stmt-loop-semi-after-empty-init-condition",
  "throw-E0109-stmt-loop-parenthesized-missing-rpar",
  "throw-E0144-stmt-loop-parenthesized-unexpected-mut",
  "throw-E0144-stmt-loop-unexpected-mut"
));

INSTANTIATE_TEST_SUITE_P(StmtObjDecl, ParserThrowTest, testing::Values(
  "throw-E0121-stmt-obj-decl-missing-id",
  "throw-E0122-stmt-obj-decl-missing-lbrace",
  "throw-E0123-stmt-obj-decl-unexpected-statement",
  "throw-E0123-stmt-obj-decl-expected-member",
  "throw-E0173-stmt-obj-decl-unexpected-field-init",
  "throw-E0173-stmt-obj-decl-unexpected-field-short"
));

INSTANTIATE_TEST_SUITE_P(StmtThrow, ParserThrowTest, testing::Values(
  "throw-E0178-stmt-throw-missing-arg"
));

INSTANTIATE_TEST_SUITE_P(StmtTry, ParserThrowTest, testing::Values(
  "throw-E0174-stmt-try-invalid-handler-param",
  "throw-E0176-stmt-try-missing-handler",
  "throw-E0177-stmt-try-mutable-handler-param"
));

INSTANTIATE_TEST_SUITE_P(StmtTypeDecl, ParserThrowTest, testing::Values(
  "throw-E0160-stmt-type-decl-missing-id",
  "throw-E0161-stmt-type-decl-missing-eq",
  "throw-E0162-stmt-type-decl-missing-type"
));

INSTANTIATE_TEST_SUITE_P(StmtVarDecl, ParserThrowTest, testing::Values(
  "throw-E0102-stmt-var-decl-missing-type",
  "throw-E0102-stmt-var-decl-mut-missing-type",
  "throw-E0127-stmt-var-decl-missing-type-rpar",
  "throw-E0102-stmt-var-decl-missing-parenthesized-type",
  "throw-E0102-stmt-var-decl-mut-missing-parenthesized-type",
  "throw-E0131-stmt-var-decl-missing-init",
  "throw-E0131-stmt-var-decl-missing-init-after-type",
  "throw-E0131-stmt-var-decl-mut-missing-init",
  "throw-E0131-stmt-var-decl-mut-missing-init-after-type",
  "throw-E0165-stmt-var-decl-const-missing-name",
  "throw-E0166-stmt-var-decl-const-missing-type",
  "throw-E0167-stmt-var-decl-const-missing-type-init",
  "throw-E0167-stmt-var-decl-const-missing-init"
));

INSTANTIATE_TEST_SUITE_P(Expr, ParserThrowTest, testing::Values(
  "throw-E0109-missing-rpar",
  "throw-E0110-expr-access-missing-dot",
  "throw-E0111-expr-cond-missing-colon",
  "throw-E0112-expr-obj-missing-prop-name",
  "throw-E0113-expr-obj-missing-colon",
  "throw-E0132-expr-unary-missing-arg",
  "throw-E0133-expr-assign-missing-value",
  "throw-E0134-expr-obj-missing-prop-init",
  "throw-E0135-expr-call-missing-arg",
  "throw-E0137-expr-binary-missing-right-expr",
  "throw-E0138-expr-cond-missing-body",
  "throw-E0139-expr-cond-missing-alt",
  "throw-E0140-expr-assign-expected-lvalue-assign",
  "throw-E0140-expr-assign-expected-lvalue-binary",
  "throw-E0140-expr-assign-expected-lvalue-call",
  "throw-E0140-expr-assign-expected-lvalue-cond",
  "throw-E0140-expr-assign-expected-lvalue-lit",
  "throw-E0140-expr-assign-expected-lvalue-obj",
  "throw-E0140-expr-assign-expected-lvalue-unary",
  "throw-E0140-expr-assign-expected-lvalue-unary-prefix",
  "throw-E0142-expr-unary-expected-lval-assign",
  "throw-E0142-expr-unary-expected-lval-binary",
  "throw-E0142-expr-unary-expected-lval-call",
  "throw-E0142-expr-unary-expected-lval-cond",
  "throw-E0142-expr-unary-expected-lval-lit",
  "throw-E0142-expr-unary-expected-lval-obj",
  "throw-E0142-expr-unary-prefix-expected-lval",
  "throw-E0147-expr-ref-expected-body",
  "throw-E0148-expr-ref-invalid-rvalue",
  "throw-E0148-expr-ref-invalid-unary-rvalue",
  "throw-E0150-expr-access-elem-missing-expr",
  "throw-E0151-expr-access-missing-rbrack",
  "throw-E0152-expr-array-missing-element",
  "throw-E0159-expr-access-missing-prop-name",
  "throw-E0164-expr-is-missing-type",
  "throw-E0168-expr-map-missing-name",
  "throw-E0169-expr-map-missing-colon",
  "throw-E0170-expr-map-missing-initializer",
  "throw-E0181-expr-await-missing-arg",
  "throw-E0182-expr-as-missing-type",
  "throw-E0183-expr-closure-missing-arrow",
  "throw-E0184-expr-closure-missing-return-type"
));

INSTANTIATE_TEST_SUITE_P(Type, ParserThrowTest, testing::Values(
  "throw-E0145-type-fn-param-name-after-mut",
  "throw-E0146-type-fn-missing-colon-after-param-name",
  "throw-E0149-type-ref-missing-type",
  "throw-E0151-type-array-missing-rbrack",
  "throw-E0163-type-union-missing-subtype",
  "throw-E0171-type-map-illegal-key-type",
  "throw-E0172-type-map-missing-rbrack",
  "throw-E0180-type-fn-async-missing-type",
  "throw-E0180-type-fn-async-invalid-type"
));
