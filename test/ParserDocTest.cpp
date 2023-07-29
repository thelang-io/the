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
#include "../src/utils.hpp"
#include "MockLexer.hpp"
#include "utils.hpp"

class ParserDocTest : public testing::TestWithParam<const char *> {
};

TEST_P(ParserDocTest, Passes) {
  auto param = testing::TestWithParam<const char *>::GetParam();
  auto sections = readTestFile("parser-doc", param, {"stdin", "stdout"});
  auto lexer = testing::NiceMock<MockLexer>(sections["stdin"]);
  auto parser = Parser(&lexer);
  auto actualStdout = str_trim(parser.doc()) + EOL;

  EXPECT_EQ(sections["stdout"], actualStdout);
}

INSTANTIATE_TEST_SUITE_P(General, ParserDocTest, testing::Values(
  "expr-parenthesized",
  "expr-access",
  "expr-array",
  "expr-as",
  "expr-assign",
  "expr-await",
  "expr-binary",
  "expr-call",
  "expr-cond",
  "expr-is",
  "expr-lit",
  "expr-map",
  "expr-obj",
  "expr-ref",
  "expr-unary",
  "stmt-comment",
  "stmt-fn-decl",
  "stmt-fn-decl-async",
  "stmt-fn-decl-empty",
  "stmt-fn-decl-body",
  "stmt-if-body",
  "stmt-loop-body",
  "stmt-main-body",
  "stmt-obj-decl",
  "stmt-obj-decl-empty",
  "stmt-obj-decl-member",
  "stmt-obj-decl-member-empty",
  "stmt-var-decl",
  "stmt-var-decl-empty",
  "type-parenthesized",
  "type-array",
  "type-fn",
  "type-fn-async",
  "type-id",
  "type-map",
  "type-optional",
  "type-ref",
  "type-union"
));
