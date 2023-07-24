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
#include "../src/CodegenCleanUp.hpp"
#include "../src/config.hpp"
#include "utils.hpp"

std::string testCodegenCleanUpGen (const CodegenCleanUp &n) {
  auto c = CodegenASTStmtCompound::create();
  n.gen(&c);
  auto s = c->str(0, false);
  return s.substr(1 + std::string(EOL).size(), s.size() - 2 - std::string(EOL).size());
}

TEST(CodegenCleanUpTest, InitialSetValues) {
  auto n0 = CodegenCleanUp();
  EXPECT_EQ(n0.labelIdx, 0);
  EXPECT_EQ(n0.breakVarIdx, 0);
  n0.labelIdx = 1;
  n0.breakVarIdx = 1;

  auto n1 = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &n0);
  EXPECT_EQ(n1.type, CODEGEN_CLEANUP_BLOCK);
  EXPECT_NE(n1.parent, nullptr);
  EXPECT_EQ(n1.labelIdx, 1);
  EXPECT_EQ(n1.breakVarIdx, 1);
}

TEST(CodegenCleanUpTest, AddsEmpty) {
  auto n = CodegenCleanUp();
  n.add();
  EXPECT_EQ(testCodegenCleanUpGen(n), "");
}

TEST(CodegenCleanUpTest, Adds) {
  auto n = CodegenCleanUp();
  n.add(CodegenASTExprAccess::create("test")->stmt());
  EXPECT_EQ(testCodegenCleanUpGen(n), "  test;" EOL);
}

TEST(CodegenCleanUpTest, AddsToNonLabeled) {
  auto n = CodegenCleanUp();
  n.add(CodegenASTExprAccess::create("test1")->stmt());
  n.add(CodegenASTExprAccess::create("test2")->stmt());

  EXPECT_EQ(testCodegenCleanUpGen(n), "  test2;" EOL "  test1;" EOL);
}

TEST(CodegenCleanUpTest, AddsAfterLabelling) {
  auto n = CodegenCleanUp();
  n.add(CodegenASTExprAccess::create("test1")->stmt());
  n.add(CodegenASTExprAccess::create("test2")->stmt());
  auto l = n.currentLabel();
  n.add(CodegenASTExprAccess::create("test3")->stmt());

  EXPECT_EQ(testCodegenCleanUpGen(n), "  test3;" EOL + l + ":" EOL "  test2;" EOL "  test1;" EOL);
}

TEST(CodegenCleanUpTest, AddsAfterLabellingToNonLabeled) {
  auto n = CodegenCleanUp();
  n.add(CodegenASTExprAccess::create("test1")->stmt());
  n.add(CodegenASTExprAccess::create("test2")->stmt());
  auto l = n.currentLabel();
  n.add(CodegenASTExprAccess::create("test3")->stmt());
  n.add(CodegenASTExprAccess::create("test4")->stmt());

  EXPECT_EQ(testCodegenCleanUpGen(n), "  test4;" EOL "  test3;" EOL + l + ":" EOL "  test2;" EOL "  test1;" EOL);
}

TEST(CodegenCleanUpTest, AddsToParent) {
  auto n0 = CodegenCleanUp();
  auto n1 = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &n0);
  n1.add(CodegenASTExprAccess::create("test1")->stmt());
  n1.add(CodegenASTExprAccess::create("test2")->stmt());
  auto l = n1.currentLabel();
  n1.add(CodegenASTExprAccess::create("test3")->stmt());
  n1.add(CodegenASTExprAccess::create("test4")->stmt());

  EXPECT_EQ(n0.labelIdx, 2);
  EXPECT_EQ(n1.labelIdx, 2);
}

TEST(CodegenCleanUpTest, CurrentBreakVarOnLoop) {
  auto n0 = CodegenCleanUp();
  auto n1 = CodegenCleanUp(CODEGEN_CLEANUP_LOOP, &n0);
  n1.breakVarIdx++;

  EXPECT_EQ(n1.currentBreakVar(), "b1");
}

TEST(CodegenCleanUpTest, CurrentBreakVarOnChild) {
  auto n0 = CodegenCleanUp();
  auto n1 = CodegenCleanUp(CODEGEN_CLEANUP_LOOP, &n0);
  n1.breakVarIdx++;
  auto n2 = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &n1);

  EXPECT_EQ(n2.currentBreakVar(), "b1");
}

TEST(CodegenCleanUpTest, ThrowsOnCurrentBreakVarRoot) {
  auto n0 = CodegenCleanUp();
  EXPECT_THROW_WITH_MESSAGE(n0.currentBreakVar(), "tried getting break var on nullptr in CodegenCleanUp");
}

TEST(CodegenCleanUpTest, LabelsOnOneStatement) {
  auto n = CodegenCleanUp();
  n.add(CodegenASTExprAccess::create("test")->stmt());
  auto l = n.currentLabel();

  EXPECT_EQ(n.labelIdx, 1);
  EXPECT_EQ(testCodegenCleanUpGen(n), l + ":" EOL "  test;" EOL);
}

TEST(CodegenCleanUpTest, LabelsOnMultipleStatements) {
  auto n = CodegenCleanUp();
  n.add(CodegenASTExprAccess::create("test1")->stmt());
  n.add(CodegenASTExprAccess::create("test2")->stmt());
  n.add(CodegenASTExprAccess::create("test3")->stmt());
  auto l = n.currentLabel();

  EXPECT_EQ(n.labelIdx, 1);
  EXPECT_EQ(testCodegenCleanUpGen(n), l + ":" EOL "  test3;" EOL "  test2;" EOL "  test1;" EOL);
}

TEST(CodegenCleanUpTest, LabelsParent) {
  auto n0 = CodegenCleanUp();
  auto n1 = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &n0);
  n0.add(CodegenASTExprAccess::create("test")->stmt());
  auto l = n1.currentLabel();

  EXPECT_EQ(testCodegenCleanUpGen(n0), l + ":" EOL "  test;" EOL);
}

TEST(CodegenCleanUpTest, LabellingDoesNotAddValueToFunction) {
  auto n0 = CodegenCleanUp();
  auto n1 = CodegenCleanUp(CODEGEN_CLEANUP_FN, &n0);
  auto n2 = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &n1);

  EXPECT_EQ(testCodegenCleanUpGen(n1), "");
}

TEST(CodegenCleanUpTest, ThrowOnNothingToLabel) {
  auto n0 = CodegenCleanUp();
  EXPECT_THROW_WITH_MESSAGE(n0.currentLabel(), "tried getting current label on nullptr in CodegenCleanUp");
}

TEST(CodegenCleanUpTest, ThrowOnNothingToLabelWithParent) {
  auto n0 = CodegenCleanUp();
  auto n1 = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &n0);
  auto n2 = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &n1);

  EXPECT_THROW_WITH_MESSAGE(n2.currentLabel(), "tried getting current label on nullptr in CodegenCleanUp");
}

TEST(CodegenCleanUpTest, CurrentReturnVarOnFn) {
  auto n0 = CodegenCleanUp();
  auto n1 = CodegenCleanUp(CODEGEN_CLEANUP_FN, &n0);

  EXPECT_EQ(n1.currentReturnVar(), "r");
}

TEST(CodegenCleanUpTest, CurrentReturnVarOnChild) {
  auto n0 = CodegenCleanUp();
  auto n1 = CodegenCleanUp(CODEGEN_CLEANUP_FN, &n0);
  auto n2 = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &n1);

  EXPECT_EQ(n2.currentReturnVar(), "r");
}

TEST(CodegenCleanUpTest, ThrowsOnCurrentReturnVarRoot) {
  auto n0 = CodegenCleanUp();
  EXPECT_THROW_WITH_MESSAGE(n0.currentReturnVar(), "tried getting return var on nullptr in CodegenCleanUp");
}

TEST(CodegenCleanUpTest, CurrentValueVarOnFn) {
  auto n0 = CodegenCleanUp();
  auto n1 = CodegenCleanUp(CODEGEN_CLEANUP_FN, &n0);

  EXPECT_EQ(n1.currentValueVar(), "v");
}

TEST(CodegenCleanUpTest, CurrentValueVarOnChild) {
  auto n0 = CodegenCleanUp();
  auto n1 = CodegenCleanUp(CODEGEN_CLEANUP_FN, &n0);
  auto n2 = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &n1);

  EXPECT_EQ(n2.currentValueVar(), "v");
}

TEST(CodegenCleanUpTest, ThrowsOnCurrentValueVarRoot) {
  auto n0 = CodegenCleanUp();
  EXPECT_THROW_WITH_MESSAGE(n0.currentValueVar(), "tried getting value var on nullptr in CodegenCleanUp");
}

TEST(CodegenCleanUpTest, Empty) {
  EXPECT_TRUE(CodegenCleanUp().empty());
}

TEST(CodegenCleanUpTest, NonEmpty) {
  auto n1 = CodegenCleanUp();
  auto n2 = CodegenCleanUp();

  n1.add(CodegenASTExprAccess::create("test")->stmt());
  n2.add(CodegenASTExprAccess::create("test")->stmt());
  n2.currentLabel();

  EXPECT_FALSE(n1.empty());
  EXPECT_FALSE(n2.empty());
}

TEST(CodegenCleanUpTest, GeneratesOnEmpty) {
  EXPECT_EQ(testCodegenCleanUpGen(CodegenCleanUp()), "");
}

TEST(CodegenCleanUpTest, GeneratesOnNonEmpty) {
  auto n = CodegenCleanUp();
  n.add(CodegenASTExprAccess::create("test")->stmt());
  EXPECT_EQ(testCodegenCleanUpGen(n), "  test;" EOL);
}

TEST(CodegenCleanUpTest, GeneratesOnNonEmptyWithIndent) {
  auto n = CodegenCleanUp();
  n.add(CodegenASTExprAccess::create("test1")->stmt());
  n.add(CodegenASTExprAccess::create("test2")->stmt());
  n.add(CodegenASTExprAccess::create("test3")->stmt());

  EXPECT_EQ(testCodegenCleanUpGen(n), "  test3;" EOL"  test2;" EOL"  test1;" EOL);
}

TEST(CodegenCleanUpTest, GeneratesOnNonEmptyWithOneLabel) {
  auto n = CodegenCleanUp();
  n.add(CodegenASTExprAccess::create("test1")->stmt());
  n.add(CodegenASTExprAccess::create("test2")->stmt());
  n.add(CodegenASTExprAccess::create("test3")->stmt());
  auto l = n.currentLabel();

  EXPECT_EQ(testCodegenCleanUpGen(n), l + ":" EOL "  test3;" EOL "  test2;" EOL "  test1;" EOL);
}

TEST(CodegenCleanUpTest, GeneratesOnNonEmptyWithMultipleLabels) {
  auto n = CodegenCleanUp();
  n.add(CodegenASTExprAccess::create("test1")->stmt());
  n.add(CodegenASTExprAccess::create("test2")->stmt());
  n.add(CodegenASTExprAccess::create("test3")->stmt());
  auto l1 = n.currentLabel();
  n.add(CodegenASTExprAccess::create("test4")->stmt());
  n.add(CodegenASTExprAccess::create("test5")->stmt());
  n.add(CodegenASTExprAccess::create("test6")->stmt());
  auto l2 = n.currentLabel();
  n.add(CodegenASTExprAccess::create("test7")->stmt());
  n.add(CodegenASTExprAccess::create("test8")->stmt());
  n.add(CodegenASTExprAccess::create("test9")->stmt());
  auto l3 = n.currentLabel();

  EXPECT_EQ(
    testCodegenCleanUpGen(n),
    l3 + ":" EOL "  test9;" EOL "  test8;" EOL "  test7;" EOL +
    l2 + ":" EOL "  test6;" EOL "  test5;" EOL "  test4;" EOL +
    l1 + ":" EOL "  test3;" EOL "  test2;" EOL "  test1;" EOL
  );
}

TEST(CodegenCleanUpTest, HasCleanUpOnEmpty) {
  auto n0 = CodegenCleanUp();
  auto n1 = CodegenCleanUp();
  n1.add(CodegenASTExprAccess::create("test")->stmt());

  EXPECT_FALSE(n0.hasCleanUp(CODEGEN_CLEANUP_BLOCK));
  EXPECT_TRUE(n1.hasCleanUp(CODEGEN_CLEANUP_BLOCK));
}

TEST(CodegenCleanUpTest, HasCleanUpOnType) {
  auto n0 = CodegenCleanUp();
  auto n1 = CodegenCleanUp(CODEGEN_CLEANUP_LOOP, &n0);
  n1.add(CodegenASTExprAccess::create("test")->stmt());
  auto n2 = CodegenCleanUp(CODEGEN_CLEANUP_FN, &n1);

  EXPECT_FALSE(n0.hasCleanUp(CODEGEN_CLEANUP_BLOCK));
  EXPECT_TRUE(n1.hasCleanUp(CODEGEN_CLEANUP_LOOP));
  EXPECT_FALSE(n2.hasCleanUp(CODEGEN_CLEANUP_FN));
  EXPECT_TRUE(n2.hasCleanUp(CODEGEN_CLEANUP_LOOP));
}
