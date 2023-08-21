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
#include "../src/codegen-ast/CodegenAST.hpp"
#include "../src/config.hpp"

TEST(CodegenASTTest, StmtInlineStr) {
  EXPECT_EQ(CodegenASTStmt::create(CodegenASTStmtEnumDecl{})->str(), "undefined");
  EXPECT_EQ(CodegenASTStmt::create(CodegenASTStmtFnDecl{})->str(), "undefined");
  EXPECT_EQ(CodegenASTStmt::create(CodegenASTStmtStructDecl{})->str(), "undefined");
}

TEST(CodegenASTTest, StmtEndsWithOnEmpty) {
  auto stmt = CodegenASTStmtCompound::create();
  EXPECT_FALSE(stmt->endsWith<CodegenASTStmtReturn>());
}

TEST(CodegenASTTest, StmtEndsWithOnInlineStmt) {
  auto stmt = CodegenASTStmtBreak::create();
  EXPECT_FALSE(stmt->endsWith<CodegenASTStmtReturn>());
}

TEST(CodegenASTTest, StmtEndsWithActualStmtSingleElement) {
  auto stmt = CodegenASTStmtCompound::create({
    CodegenASTStmtReturn::create()
  });

  EXPECT_TRUE(stmt->endsWith<CodegenASTStmtReturn>());
}

TEST(CodegenASTTest, StmtEndsWithActualStmtMultipleElements) {
  auto stmt = CodegenASTStmtCompound::create({
    CodegenASTStmtNull::create(),
    CodegenASTExprAccess::create("test")->stmt(),
    CodegenASTStmtReturn::create()
  });

  EXPECT_TRUE(stmt->endsWith<CodegenASTStmtReturn>());
}

TEST(CodegenASTTest, StmtEndsWithActualStmtBetweenElements) {
  auto stmt = CodegenASTStmtCompound::create({
    CodegenASTStmtNull::create(),
    CodegenASTStmtReturn::create(),
    CodegenASTExprAccess::create("test")->stmt()
  });

  EXPECT_FALSE(stmt->endsWith<CodegenASTStmtReturn>());
}

TEST(CodegenASTTest, StmtAsMethods) {
  EXPECT_NO_THROW(CodegenASTStmtBreak::create()->asBreak());
  EXPECT_NO_THROW(CodegenASTStmtCase::create(nullptr, CodegenASTStmtCompound::create())->asCase());
  EXPECT_NO_THROW(CodegenASTStmtCompound::create()->asCompound());
  EXPECT_NO_THROW(CodegenASTStmtContinue::create()->asContinue());
  EXPECT_NO_THROW(CodegenASTStmt::create(CodegenASTStmtEnumDecl{})->asEnumDecl());
  EXPECT_NO_THROW(CodegenASTExprLiteral::create("1")->stmt()->asExpr());
  EXPECT_NO_THROW(CodegenASTStmt::create(CodegenASTStmtFnDecl{})->asFnDecl());
  EXPECT_NO_THROW(CodegenASTStmtFor::create()->asFor());
  EXPECT_NO_THROW(CodegenASTStmtGoto::create("L0")->asGoto());
  EXPECT_NO_THROW(CodegenASTStmtIf::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtReturn::create())->asIf());
  EXPECT_NO_THROW(CodegenASTStmtLabel::create("L0")->asLabel());
  EXPECT_NO_THROW(CodegenASTStmtNull::create()->asNull());
  EXPECT_NO_THROW(CodegenASTStmtReturn::create()->asReturn());
  EXPECT_NO_THROW(CodegenASTStmt::create(CodegenASTStmtStructDecl{})->asStructDecl());
  EXPECT_NO_THROW(CodegenASTStmtSwitch::create(CodegenASTExprLiteral::create("1"))->asSwitch());
  EXPECT_NO_THROW(CodegenASTStmtVarDecl::create(CodegenASTType::create("int"), CodegenASTExprAccess::create("a"))->asVarDecl());
  EXPECT_NO_THROW(CodegenASTStmtWhile::create(CodegenASTExprLiteral::create("1"))->asWhile());

  auto testBreak = static_cast<std::shared_ptr<const CodegenASTStmt>>(CodegenASTStmtBreak::create());
  auto testCase = static_cast<std::shared_ptr<const CodegenASTStmt>>(CodegenASTStmtCase::create(nullptr, CodegenASTStmtCompound::create()));
  auto testCompound = static_cast<std::shared_ptr<const CodegenASTStmt>>(CodegenASTStmtCompound::create());
  auto testContinue = static_cast<std::shared_ptr<const CodegenASTStmt>>(CodegenASTStmtContinue::create());
  auto testEnumDecl = static_cast<std::shared_ptr<const CodegenASTStmt>>(CodegenASTStmt::create(CodegenASTStmtEnumDecl{}));
  auto testExpr = static_cast<std::shared_ptr<const CodegenASTStmt>>(CodegenASTExprLiteral::create("1")->stmt());
  auto testFnDecl = static_cast<std::shared_ptr<const CodegenASTStmt>>(CodegenASTStmt::create(CodegenASTStmtFnDecl{}));
  auto testFor = static_cast<std::shared_ptr<const CodegenASTStmt>>(CodegenASTStmtFor::create());
  auto testGoto = static_cast<std::shared_ptr<const CodegenASTStmt>>(CodegenASTStmtGoto::create("L0"));
  auto testIf = static_cast<std::shared_ptr<const CodegenASTStmt>>(CodegenASTStmtIf::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtReturn::create()));
  auto testLabel = static_cast<std::shared_ptr<const CodegenASTStmt>>(CodegenASTStmtLabel::create("L0"));
  auto testNull = static_cast<std::shared_ptr<const CodegenASTStmt>>(CodegenASTStmtNull::create());
  auto testReturn = static_cast<std::shared_ptr<const CodegenASTStmt>>(CodegenASTStmtReturn::create());
  auto testStructDecl = static_cast<std::shared_ptr<const CodegenASTStmt>>(CodegenASTStmt::create(CodegenASTStmtStructDecl{}));
  auto testSwitch = static_cast<std::shared_ptr<const CodegenASTStmt>>(CodegenASTStmtSwitch::create(CodegenASTExprLiteral::create("1")));
  auto testVarDecl = static_cast<std::shared_ptr<const CodegenASTStmt>>(CodegenASTStmtVarDecl::create(CodegenASTType::create("int"), CodegenASTExprAccess::create("a")));
  auto testWhile = static_cast<std::shared_ptr<const CodegenASTStmt>>(CodegenASTStmtWhile::create(CodegenASTExprLiteral::create("1")));

  EXPECT_NO_THROW(testBreak->asBreak());
  EXPECT_NO_THROW(testCase->asCase());
  EXPECT_NO_THROW(testCompound->asCompound());
  EXPECT_NO_THROW(testContinue->asContinue());
  EXPECT_NO_THROW(testEnumDecl->asEnumDecl());
  EXPECT_NO_THROW(testExpr->asExpr());
  EXPECT_NO_THROW(testFnDecl->asFnDecl());
  EXPECT_NO_THROW(testFor->asFor());
  EXPECT_NO_THROW(testGoto->asGoto());
  EXPECT_NO_THROW(testIf->asIf());
  EXPECT_NO_THROW(testLabel->asLabel());
  EXPECT_NO_THROW(testNull->asNull());
  EXPECT_NO_THROW(testReturn->asReturn());
  EXPECT_NO_THROW(testStructDecl->asStructDecl());
  EXPECT_NO_THROW(testSwitch->asSwitch());
  EXPECT_NO_THROW(testVarDecl->asVarDecl());
  EXPECT_NO_THROW(testWhile->asWhile());
}

TEST(CodegenASTTest, StmtIsMethods) {
  EXPECT_TRUE(CodegenASTStmtBreak::create()->isBreak());
  EXPECT_TRUE(CodegenASTStmtCase::create(nullptr, CodegenASTStmtCompound::create())->isCase());
  EXPECT_TRUE(CodegenASTStmtCompound::create()->isCompound());
  EXPECT_TRUE(CodegenASTStmtContinue::create()->isContinue());
  EXPECT_TRUE(CodegenASTStmt::create(CodegenASTStmtEnumDecl{})->isEnumDecl());
  EXPECT_TRUE(CodegenASTExprLiteral::create("1")->stmt()->isExpr());
  EXPECT_TRUE(CodegenASTStmt::create(CodegenASTStmtFnDecl{})->isFnDecl());
  EXPECT_TRUE(CodegenASTStmtFor::create()->isFor());
  EXPECT_TRUE(CodegenASTStmtGoto::create("L0")->isGoto());
  EXPECT_TRUE(CodegenASTStmtIf::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtReturn::create())->isIf());
  EXPECT_TRUE(CodegenASTStmtLabel::create("L0")->isLabel());
  EXPECT_TRUE(CodegenASTStmtNull::create()->isNull());
  EXPECT_TRUE(CodegenASTStmtReturn::create()->isReturn());
  EXPECT_TRUE(CodegenASTStmt::create(CodegenASTStmtStructDecl{})->isStructDecl());
  EXPECT_TRUE(CodegenASTStmtSwitch::create(CodegenASTExprLiteral::create("1"))->isSwitch());
  EXPECT_TRUE(CodegenASTStmtVarDecl::create(CodegenASTType::create("int"), CodegenASTExprAccess::create("a"))->isVarDecl());
  EXPECT_TRUE(CodegenASTStmtWhile::create(CodegenASTExprLiteral::create("1"))->isWhile());
}

TEST(CodegenASTTest, StmtAppend) {
  EXPECT_NE(CodegenASTStmtCase::create(nullptr, CodegenASTStmtReturn::create())->append(CodegenASTStmtReturn::create()), nullptr);
  EXPECT_NE(CodegenASTStmtCase::create(nullptr, CodegenASTStmtReturn::create())->append(CodegenASTStmtCompound::create()), nullptr);
  EXPECT_NE(CodegenASTStmtCase::create(nullptr, CodegenASTStmtCompound::create())->append(CodegenASTStmtReturn::create()), nullptr);
  EXPECT_NE(CodegenASTStmtCase::create(nullptr, CodegenASTStmtCompound::create())->append(CodegenASTStmtCompound::create()), nullptr);
  EXPECT_NE(CodegenASTStmtCompound::create()->append(CodegenASTStmtReturn::create()), nullptr);
  EXPECT_NE(CodegenASTStmtCompound::create()->append(CodegenASTStmtCompound::create()), nullptr);
  EXPECT_NE(CodegenASTStmtFor::create(nullptr, nullptr, nullptr, CodegenASTStmtReturn::create())->append(CodegenASTStmtReturn::create()), nullptr);
  EXPECT_NE(CodegenASTStmtFor::create(nullptr, nullptr, nullptr, CodegenASTStmtReturn::create())->append(CodegenASTStmtCompound::create()), nullptr);
  EXPECT_NE(CodegenASTStmtFor::create(nullptr, nullptr, nullptr, CodegenASTStmtCompound::create())->append(CodegenASTStmtReturn::create()), nullptr);
  EXPECT_NE(CodegenASTStmtFor::create(nullptr, nullptr, nullptr, CodegenASTStmtCompound::create())->append(CodegenASTStmtCompound::create()), nullptr);
  EXPECT_NE(CodegenASTStmtIf::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtReturn::create())->append(CodegenASTStmtReturn::create()), nullptr);
  EXPECT_NE(CodegenASTStmtIf::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtReturn::create())->append(CodegenASTStmtCompound::create()), nullptr);
  EXPECT_NE(CodegenASTStmtIf::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtCompound::create())->append(CodegenASTStmtReturn::create()), nullptr);
  EXPECT_NE(CodegenASTStmtIf::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtCompound::create())->append(CodegenASTStmtCompound::create()), nullptr);
  EXPECT_NE(CodegenASTStmtSwitch::create(CodegenASTExprLiteral::create("1"))->append(CodegenASTStmtCompound::create()), nullptr);
  EXPECT_NE(CodegenASTStmtSwitch::create(CodegenASTExprLiteral::create("1"))->append(CodegenASTStmtReturn::create()), nullptr);
  EXPECT_NE(CodegenASTStmtWhile::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtReturn::create())->append(CodegenASTStmtReturn::create()), nullptr);
  EXPECT_NE(CodegenASTStmtWhile::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtReturn::create())->append(CodegenASTStmtCompound::create()), nullptr);
  EXPECT_NE(CodegenASTStmtWhile::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtCompound::create())->append(CodegenASTStmtReturn::create()), nullptr);
  EXPECT_NE(CodegenASTStmtWhile::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtCompound::create())->append(CodegenASTStmtCompound::create()), nullptr);
}

TEST(CodegenASTTest, StmtEmptyVector) {
  EXPECT_TRUE(CodegenASTStmt::emptyVector({}));
  EXPECT_FALSE(CodegenASTStmt::emptyVector({CodegenASTStmtReturn::create()}));
  EXPECT_TRUE(CodegenASTStmt::emptyVector({CodegenASTStmtNull::create(), CodegenASTStmtNull::create()}));
  EXPECT_FALSE(CodegenASTStmt::emptyVector({CodegenASTStmtNull::create(), CodegenASTStmtReturn::create()}));
  EXPECT_FALSE(CodegenASTStmt::emptyVector({CodegenASTStmtReturn::create(), CodegenASTStmtNull::create()}));
}

TEST(CodegenASTTest, StmtExits) {
  auto stmt = CodegenASTStmtCompound::create({});
  stmt = stmt->append(CodegenASTStmtSwitch::create(CodegenASTExprLiteral::create("1")));
  stmt = stmt->append(CodegenASTStmtCase::create(nullptr, CodegenASTStmtCompound::create()));
  EXPECT_NO_THROW(stmt->exit()->exit()->exit()->exit());
}

TEST(CodegenASTTest, StmtHasBody) {
  EXPECT_FALSE(CodegenASTStmtBreak::create()->hasBody());
  EXPECT_FALSE(CodegenASTStmtCase::create(nullptr, CodegenASTStmtReturn::create())->hasBody());
  EXPECT_TRUE(CodegenASTStmtCase::create(nullptr, CodegenASTStmtCompound::create())->hasBody());
  EXPECT_TRUE(CodegenASTStmtCompound::create()->hasBody());
  EXPECT_FALSE(CodegenASTStmtContinue::create()->hasBody());
  EXPECT_FALSE(CodegenASTStmt::create(CodegenASTStmtEnumDecl{})->hasBody());
  EXPECT_FALSE(CodegenASTExprLiteral::create("1")->stmt()->hasBody());
  EXPECT_FALSE(CodegenASTStmt::create(CodegenASTStmtFnDecl{})->hasBody());
  EXPECT_FALSE(CodegenASTStmtFor::create()->hasBody());
  EXPECT_TRUE(CodegenASTStmtFor::create(nullptr, nullptr, nullptr, CodegenASTStmtCompound::create())->hasBody());
  EXPECT_FALSE(CodegenASTStmtFor::create(nullptr, nullptr, nullptr, CodegenASTStmtReturn::create())->hasBody());
  EXPECT_FALSE(CodegenASTStmtGoto::create("L0")->hasBody());
  EXPECT_FALSE(CodegenASTStmtIf::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtReturn::create())->hasBody());
  EXPECT_TRUE(CodegenASTStmtIf::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtCompound::create())->hasBody());
  EXPECT_FALSE(CodegenASTStmtLabel::create("L0")->hasBody());
  EXPECT_FALSE(CodegenASTStmtNull::create()->hasBody());
  EXPECT_FALSE(CodegenASTStmtReturn::create()->hasBody());
  EXPECT_FALSE(CodegenASTStmt::create(CodegenASTStmtStructDecl{})->hasBody());
  EXPECT_FALSE(CodegenASTStmtSwitch::create(CodegenASTExprLiteral::create("1"))->hasBody());
  EXPECT_FALSE(CodegenASTStmtVarDecl::create(CodegenASTType::create("int"), CodegenASTExprAccess::create("a"))->hasBody());
  EXPECT_FALSE(CodegenASTStmtWhile::create(CodegenASTExprLiteral::create("1"))->hasBody());
  EXPECT_TRUE(CodegenASTStmtWhile::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtCompound::create())->hasBody());
  EXPECT_FALSE(CodegenASTStmtWhile::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtReturn::create())->hasBody());
}

TEST(CodegenASTTest, StmtIncreaseAsyncCounterOnEmpty) {
  auto asyncCounter = static_cast<std::size_t>(0);
  auto stmt = CodegenASTStmtCompound::create();

  EXPECT_EQ(stmt->increaseAsyncCounter(asyncCounter), stmt);
  EXPECT_EQ(asyncCounter, 0);
}

TEST(CodegenASTTest, StmtIncreaseAsyncCounterComplex) {
  auto asyncCounter = static_cast<std::size_t>(0);
  auto stmt = CodegenASTStmtCompound::create();
  stmt = stmt->append(CodegenASTStmtSwitch::create(CodegenASTExprAccess::create("test")));
  stmt = stmt->append(CodegenASTStmtCase::create(CodegenASTExprLiteral::create("0"), CodegenASTStmtCompound::create()));

  EXPECT_EQ(stmt->increaseAsyncCounter(asyncCounter), stmt);
  EXPECT_EQ(asyncCounter, 0);
  stmt->append(CodegenASTStmtReturn::create());
  EXPECT_NE(stmt->increaseAsyncCounter(asyncCounter), nullptr);
  EXPECT_EQ(asyncCounter, 1);
}

TEST(CodegenASTTest, StmtIsExprNull) {
  EXPECT_TRUE(CodegenASTExprNull::create()->stmt()->isExprNull());
}

TEST(CodegenASTTest, StmtIsNullable) {
  EXPECT_TRUE(CodegenASTExprNull::create()->stmt()->isNullable());
  EXPECT_TRUE(CodegenASTStmtNull::create()->isNullable());
}

TEST(CodegenASTTest, StmtGetPtr) {
  EXPECT_NE(CodegenASTStmtNull::create()->getptr(), nullptr);
  EXPECT_NE(CodegenASTStmtReturn::create()->getptr(), nullptr);
}

TEST(CodegenASTTest, StmtGetBody) {
  EXPECT_NE(CodegenASTStmtCase::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtReturn::create())->getBody(), nullptr);
  EXPECT_NE(CodegenASTStmtCase::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtCompound::create())->getBody(), nullptr);
  EXPECT_NE(CodegenASTStmtFor::create(nullptr, nullptr, nullptr, CodegenASTStmtReturn::create())->getBody(), nullptr);
  EXPECT_NE(CodegenASTStmtFor::create(nullptr, nullptr, nullptr, CodegenASTStmtCompound::create())->getBody(), nullptr);
  EXPECT_NE(CodegenASTStmtIf::create(CodegenASTExprAccess::create("test"), CodegenASTStmtReturn::create())->getBody(), nullptr);
  EXPECT_NE(CodegenASTStmtIf::create(CodegenASTExprAccess::create("test"), CodegenASTStmtCompound::create())->getBody(), nullptr);
  EXPECT_NE(CodegenASTStmtWhile::create(CodegenASTExprAccess::create("test"), CodegenASTStmtReturn::create())->getBody(), nullptr);
  EXPECT_NE(CodegenASTStmtWhile::create(CodegenASTExprAccess::create("test"), CodegenASTStmtCompound::create())->getBody(), nullptr);
  EXPECT_NE(CodegenASTStmtReturn::create()->getBody(), nullptr);
}

TEST(CodegenASTTest, StmtMerge) {
  auto stmt = CodegenASTStmtCompound::create();

  stmt->merge({
    CodegenASTStmtReturn::create(),
    CodegenASTStmtNull::create()
  });

  EXPECT_EQ(stmt->asCompound().body.size(), 2);
}

TEST(CodegenASTTest, StmtPrepend) {
  EXPECT_NE(CodegenASTStmtCase::create(nullptr, CodegenASTStmtReturn::create())->prepend(CodegenASTStmtReturn::create()), nullptr);
  EXPECT_NE(CodegenASTStmtCase::create(nullptr, CodegenASTStmtReturn::create())->prepend(CodegenASTStmtCompound::create()), nullptr);
  EXPECT_NE(CodegenASTStmtCase::create(nullptr, CodegenASTStmtCompound::create())->prepend(CodegenASTStmtReturn::create()), nullptr);
  EXPECT_NE(CodegenASTStmtCase::create(nullptr, CodegenASTStmtCompound::create())->prepend(CodegenASTStmtCompound::create()), nullptr);
  EXPECT_NE(CodegenASTStmtCompound::create()->prepend(CodegenASTStmtReturn::create()), nullptr);
  EXPECT_NE(CodegenASTStmtCompound::create()->prepend(CodegenASTStmtCompound::create()), nullptr);
  EXPECT_NE(CodegenASTStmtFor::create(nullptr, nullptr, nullptr, CodegenASTStmtReturn::create())->prepend(CodegenASTStmtReturn::create()), nullptr);
  EXPECT_NE(CodegenASTStmtFor::create(nullptr, nullptr, nullptr, CodegenASTStmtReturn::create())->prepend(CodegenASTStmtCompound::create()), nullptr);
  EXPECT_NE(CodegenASTStmtFor::create(nullptr, nullptr, nullptr, CodegenASTStmtCompound::create())->prepend(CodegenASTStmtReturn::create()), nullptr);
  EXPECT_NE(CodegenASTStmtFor::create(nullptr, nullptr, nullptr, CodegenASTStmtCompound::create())->prepend(CodegenASTStmtCompound::create()), nullptr);
  EXPECT_NE(CodegenASTStmtIf::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtReturn::create())->prepend(CodegenASTStmtReturn::create()), nullptr);
  EXPECT_NE(CodegenASTStmtIf::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtReturn::create())->prepend(CodegenASTStmtCompound::create()), nullptr);
  EXPECT_NE(CodegenASTStmtIf::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtCompound::create())->prepend(CodegenASTStmtReturn::create()), nullptr);
  EXPECT_NE(CodegenASTStmtIf::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtCompound::create())->prepend(CodegenASTStmtCompound::create()), nullptr);
  EXPECT_NE(CodegenASTStmtSwitch::create(CodegenASTExprLiteral::create("1"))->prepend(CodegenASTStmtCompound::create()), nullptr);
  EXPECT_NE(CodegenASTStmtSwitch::create(CodegenASTExprLiteral::create("1"))->prepend(CodegenASTStmtReturn::create()), nullptr);
  EXPECT_NE(CodegenASTStmtWhile::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtReturn::create())->prepend(CodegenASTStmtReturn::create()), nullptr);
  EXPECT_NE(CodegenASTStmtWhile::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtReturn::create())->prepend(CodegenASTStmtCompound::create()), nullptr);
  EXPECT_NE(CodegenASTStmtWhile::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtCompound::create())->prepend(CodegenASTStmtReturn::create()), nullptr);
  EXPECT_NE(CodegenASTStmtWhile::create(CodegenASTExprLiteral::create("1"), CodegenASTStmtCompound::create())->prepend(CodegenASTStmtCompound::create()), nullptr);
}

TEST(CodegenASTTest, StmtSetIfAlt) {
  auto stmt = CodegenASTStmtIf::create(CodegenASTExprAccess::create("test"), CodegenASTStmtReturn::create());
  EXPECT_EQ(stmt->asIf().alt, nullptr);
  EXPECT_NO_THROW(stmt->setIfAlt(CodegenASTStmtCompound::create()));
  EXPECT_NE(stmt->asIf().alt, nullptr);
}

TEST(CodegenASTTest, StmtBreak) {
  EXPECT_EQ(CodegenASTStmtBreak::create()->str(0, false), "break;");
  EXPECT_EQ(CodegenASTStmtBreak::create()->str(2), "  break;" EOL);
}

TEST(CodegenASTTest, StmtCase) {
  EXPECT_EQ(CodegenASTStmtCase::create(CodegenASTExprLiteral::create("0"), CodegenASTStmtReturn::create())->str(0, false), "case 0: return;");

  EXPECT_EQ(
    CodegenASTStmtCase::create(CodegenASTExprLiteral::create("0"), CodegenASTStmtCompound::create({CodegenASTStmtReturn::create()}))->str(2),
    "  case 0: {" EOL
    "    return;" EOL
    "  }" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtCase::create(nullptr, CodegenASTStmtCompound::create({CodegenASTStmtReturn::create()}))->str(2),
    "  default: {" EOL
    "    return;" EOL
    "  }" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtCase::create(CodegenASTExprNull::create(), CodegenASTStmtCompound::create({CodegenASTStmtReturn::create()}))->str(2),
    "  default: {" EOL
    "    return;" EOL
    "  }" EOL
  );
}

TEST(CodegenASTTest, StmtCompound) {
  EXPECT_EQ(CodegenASTStmtCompound::create()->str(0, false), "{" EOL "}");
  EXPECT_EQ(CodegenASTStmtCompound::create()->str(2), "  {" EOL "  }" EOL);

  EXPECT_EQ(
    CodegenASTStmtCompound::create({
      CodegenASTExprAssign::create(CodegenASTExprAccess::create("a"), "=", CodegenASTExprLiteral::create("2"))->stmt(),
      CodegenASTStmtReturn::create(CodegenASTExprAccess::create("a"))
    })->str(2),
    "  {" EOL
    "    a = 2;" EOL
    "    return a;" EOL
    "  }" EOL
  );
}

TEST(CodegenASTTest, StmtContinue) {
  EXPECT_EQ(CodegenASTStmtContinue::create()->str(0, false), "continue;");
  EXPECT_EQ(CodegenASTStmtContinue::create()->str(2), "  continue;" EOL);
}

TEST(CodegenASTTest, StmtExpr) {
  EXPECT_EQ(CodegenASTExprNull::create()->stmt()->str(0, false), "");
  EXPECT_EQ(CodegenASTExprAccess::create("a")->stmt()->str(0, false), "a;");
  EXPECT_EQ(CodegenASTExprAccess::create("a")->stmt()->str(2), "  a;" EOL);
}

TEST(CodegenASTTest, StmtFor) {
  EXPECT_EQ(CodegenASTStmtFor::create()->str(0, false), "for (;;);");

  EXPECT_EQ(
    CodegenASTStmtFor::create(
      CodegenASTStmtVarDecl::create(
        CodegenASTType::create("int"),
        CodegenASTExprAccess::create("a"),
        CodegenASTExprLiteral::create("1")
      )
    )->str(2),
    "  for (int a = 1;;);" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtFor::create(nullptr, CodegenASTExprAccess::create("a"))->str(2),
    "  for (; a;);" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtFor::create(nullptr, nullptr, CodegenASTExprAccess::create("a"))->str(2),
    "  for (;; a);" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtFor::create(
      CodegenASTStmtVarDecl::create(
        CodegenASTType::create("int"),
        CodegenASTExprAccess::create("i"),
        CodegenASTExprLiteral::create("0")
      ),
      CodegenASTExprBinary::create(
        CodegenASTExprAccess::create("i"),
        "<",
        CodegenASTExprLiteral::create("10")
      ),
      CodegenASTExprUnary::create(CodegenASTExprAccess::create("i"), "++")
    )->str(2),
    "  for (int i = 0; i < 10; i++);" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtFor::create(
      CodegenASTStmtVarDecl::create(
        CodegenASTType::create("int"),
        CodegenASTExprAccess::create("i"),
        CodegenASTExprLiteral::create("0")
      ),
      CodegenASTExprBinary::create(
        CodegenASTExprAccess::create("i"),
        "<",
        CodegenASTExprLiteral::create("10")
      ),
      CodegenASTExprUnary::create(CodegenASTExprAccess::create("i"), "++")
    )->str(2),
    "  for (int i = 0; i < 10; i++);" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtFor::create(
      CodegenASTStmtVarDecl::create(
        CodegenASTType::create("int"),
        CodegenASTExprAccess::create("i"),
        CodegenASTExprLiteral::create("0")
      ),
      CodegenASTExprBinary::create(
        CodegenASTExprAccess::create("i"),
        "<",
        CodegenASTExprLiteral::create("10")
      ),
      CodegenASTExprUnary::create(CodegenASTExprAccess::create("i"), "++"),
      CodegenASTStmtNull::create()
    )->str(2),
    "  for (int i = 0; i < 10; i++);" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtFor::create(
      CodegenASTStmtVarDecl::create(
        CodegenASTType::create("int"),
        CodegenASTExprAccess::create("i"),
        CodegenASTExprLiteral::create("0")
      ),
      CodegenASTExprBinary::create(
        CodegenASTExprAccess::create("i"),
        "<",
        CodegenASTExprLiteral::create("10")
      ),
      CodegenASTExprUnary::create(CodegenASTExprAccess::create("i"), "++"),
      CodegenASTStmtContinue::create()
    )->str(2),
    "  for (int i = 0; i < 10; i++) continue;" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtFor::create(
      CodegenASTStmtVarDecl::create(
        CodegenASTType::create("int"),
        CodegenASTExprAccess::create("i"),
        CodegenASTExprLiteral::create("0")
      ),
      CodegenASTExprBinary::create(
        CodegenASTExprAccess::create("i"),
        "<",
        CodegenASTExprLiteral::create("10")
      ),
      CodegenASTExprUnary::create(CodegenASTExprAccess::create("i"), "++"),
      CodegenASTStmtCompound::create({
        CodegenASTStmtContinue::create()
      })
    )->str(2),
    "  for (int i = 0; i < 10; i++) {" EOL
    "    continue;" EOL
    "  }" EOL
  );
}

TEST(CodegenASTTest, StmtGoto) {
  EXPECT_EQ(CodegenASTStmtGoto::create("L0")->str(0, false), "goto L0;");
  EXPECT_EQ(CodegenASTStmtGoto::create("L0")->str(2), "  goto L0;" EOL);
}

TEST(CodegenASTTest, StmtIf) {
  EXPECT_EQ(
    CodegenASTStmtIf::create(CodegenASTExprAccess::create("test"), CodegenASTStmtReturn::create())->str(0, false),
    "if (test) return;"
  );

  EXPECT_EQ(
    CodegenASTStmtIf::create(CodegenASTExprAccess::create("test"), CodegenASTStmtReturn::create())->str(2),
    "  if (test) return;" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtIf::create(
      CodegenASTExprAccess::create("test"),
      CodegenASTStmtCompound::create({
        CodegenASTStmtReturn::create()
      })
    )->str(2),
    "  if (test) {" EOL
    "    return;" EOL
    "  }" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtIf::create(
      CodegenASTExprAccess::create("test"),
      CodegenASTStmtCompound::create({
        CodegenASTStmtReturn::create()
      }),
      CodegenASTStmtCompound::create({
        CodegenASTStmtReturn::create()
      })
    )->str(2),
    "  if (test) {" EOL
    "    return;" EOL
    "  } else {" EOL
    "    return;" EOL
    "  }" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtIf::create(
      CodegenASTExprAccess::create("test1"),
      CodegenASTStmtCompound::create({
        CodegenASTStmtReturn::create()
      }),
      CodegenASTStmtIf::create(
        CodegenASTExprAccess::create("test2"),
        CodegenASTStmtCompound::create({
          CodegenASTStmtReturn::create()
        }),
        CodegenASTStmtCompound::create({
          CodegenASTStmtReturn::create()
        })
      )
    )->str(2),
    "  if (test1) {" EOL
    "    return;" EOL
    "  } else if (test2) {" EOL
    "    return;" EOL
    "  } else {" EOL
    "    return;" EOL
    "  }" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtIf::create(
      CodegenASTExprAccess::create("test"),
      CodegenASTStmtCompound::create({
        CodegenASTStmtReturn::create()
      }),
      CodegenASTStmtReturn::create()
    )->str(2),
    "  if (test) {" EOL
    "    return;" EOL
    "  } else return;" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtIf::create(
      CodegenASTExprAccess::create("test"),
      CodegenASTStmtReturn::create(),
      CodegenASTStmtReturn::create()
    )->str(2),
    "  if (test) return;" EOL
    "  else return;" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtIf::create(
      CodegenASTExprAccess::create("test1"),
      CodegenASTStmtReturn::create(),
      CodegenASTStmtIf::create(
        CodegenASTExprAccess::create("test2"),
        CodegenASTStmtReturn::create()
      )
    )->str(2),
    "  if (test1) return;" EOL
    "  else if (test2) return;" EOL
  );
}

TEST(CodegenASTTest, StmtLabel) {
  EXPECT_EQ(CodegenASTStmtLabel::create("L0")->str(0, false), "L0:");
  EXPECT_EQ(CodegenASTStmtLabel::create("L0")->str(2), "L0:" EOL);
}

TEST(CodegenASTTest, StmtNull) {
  EXPECT_EQ(CodegenASTStmtNull::create()->str(0, false), "");
  EXPECT_EQ(CodegenASTStmtNull::create()->str(2), "");
}

TEST(CodegenASTTest, StmtReturn) {
  EXPECT_EQ(CodegenASTStmtReturn::create()->str(0, false), "return;");
  EXPECT_EQ(CodegenASTStmtReturn::create()->str(2), "  return;" EOL);
  EXPECT_EQ(CodegenASTStmtReturn::create(CodegenASTExprNull::create())->str(2), "  return;" EOL);
  EXPECT_EQ(CodegenASTStmtReturn::create(CodegenASTExprLiteral::create("1"))->str(2), "  return 1;" EOL);
}

TEST(CodegenASTTest, StmtSwitch) {
  EXPECT_EQ(
    CodegenASTStmtSwitch::create(CodegenASTExprAccess::create("test"))->str(0, false),
    "switch (test) {" EOL
    "}"
  );

  EXPECT_EQ(
    CodegenASTStmtSwitch::create(CodegenASTExprAccess::create("test"))->str(2),
    "  switch (test) {" EOL
    "  }" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtSwitch::create(
      CodegenASTExprAccess::create("test"),
      {CodegenASTStmtCase::create(CodegenASTExprLiteral::create("0"), CodegenASTStmtBreak::create())}
    )->str(2),
    "  switch (test) {" EOL
    "    case 0: break;" EOL
    "  }" EOL
  );
}

TEST(CodegenASTTest, StmtVarDecl) {
  EXPECT_EQ(
    CodegenASTStmtVarDecl::create(
      CodegenASTType::create("int"),
      CodegenASTExprAccess::create("a")
    )->str(0, false),
    "int a;"
  );

  EXPECT_EQ(
    CodegenASTStmtVarDecl::create(
      CodegenASTType::create("int"),
      CodegenASTExprAccess::create("a")
    )->str(2),
    "  int a;" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtVarDecl::create(
      CodegenASTType::create("int *"),
      CodegenASTExprAccess::create("a")
    )->str(2),
    "  int *a;" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtVarDecl::create(
      CodegenASTType::create("int"),
      CodegenASTExprAccess::create("a"),
      CodegenASTExprNull::create()
    )->str(2),
    "  int a;" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtVarDecl::create(
      CodegenASTType::create("int"),
      CodegenASTExprAccess::create("a"),
      CodegenASTExprLiteral::create("1")
    )->str(2),
    "  int a = 1;" EOL
  );
}

TEST(CodegenASTTest, StmtWhile) {
  EXPECT_EQ(
    CodegenASTStmtWhile::create(CodegenASTExprAccess::create("true"))->str(0, false),
    "while (true);"
  );

  EXPECT_EQ(
    CodegenASTStmtWhile::create(CodegenASTExprAccess::create("true"))->str(2),
    "  while (true);" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtWhile::create(
      CodegenASTExprAccess::create("true"),
      CodegenASTStmtNull::create()
    )->str(2),
    "  while (true);" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtWhile::create(
      CodegenASTExprAccess::create("true"),
      CodegenASTStmtBreak::create()
    )->str(2),
    "  while (true) break;" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtWhile::create(
      CodegenASTExprAccess::create("true"),
      CodegenASTStmtCompound::create()
    )->str(2),
    "  while (true) {" EOL
    "  }" EOL
  );

  EXPECT_EQ(
    CodegenASTStmtWhile::create(
      CodegenASTExprAccess::create("true"),
      CodegenASTStmtCompound::create({
        CodegenASTStmtBreak::create()
      })
    )->str(2),
    "  while (true) {" EOL
    "    break;" EOL
    "  }" EOL
  );
}

TEST(CodegenASTTest, ExprAsMethods) {
  EXPECT_NO_THROW(CodegenASTExprAccess::create("a")->asAccess());
  EXPECT_NO_THROW(CodegenASTExprAssign::create(CodegenASTExprAccess::create("a"), "=", CodegenASTExprLiteral::create("1"))->asAssign());
  EXPECT_NO_THROW(CodegenASTExprBinary::create(CodegenASTExprAccess::create("a"), "==", CodegenASTExprAccess::create("b"))->asBinary());
  EXPECT_NO_THROW(CodegenASTExprCall::create(CodegenASTExprAccess::create("a"))->asCall());
  EXPECT_NO_THROW(CodegenASTExprCast::create(CodegenASTType::create("int"), CodegenASTExprAccess::create("a"))->asCast());
  EXPECT_NO_THROW(CodegenASTExprCond::create(CodegenASTExprAccess::create("a"), CodegenASTExprLiteral::create("1"), CodegenASTExprLiteral::create("2"))->asCond());
  EXPECT_NO_THROW(CodegenASTExprInitList::create()->asInitList());
  EXPECT_NO_THROW(CodegenASTExprLiteral::create("1")->asLiteral());
  EXPECT_NO_THROW(CodegenASTExprNull::create()->asNull());
  EXPECT_NO_THROW(CodegenASTExprUnary::create("!", CodegenASTExprAccess::create("a"))->asUnary());

  auto testAccess = static_cast<std::shared_ptr<const CodegenASTExpr>>(CodegenASTExprAccess::create("a"));
  auto testAssign = static_cast<std::shared_ptr<const CodegenASTExpr>>(CodegenASTExprAssign::create(CodegenASTExprAccess::create("a"), "=", CodegenASTExprLiteral::create("1")));
  auto testBinary = static_cast<std::shared_ptr<const CodegenASTExpr>>(CodegenASTExprBinary::create(CodegenASTExprAccess::create("a"), "==", CodegenASTExprAccess::create("b")));
  auto testCall = static_cast<std::shared_ptr<const CodegenASTExpr>>(CodegenASTExprCall::create(CodegenASTExprAccess::create("a")));
  auto testCast = static_cast<std::shared_ptr<const CodegenASTExpr>>(CodegenASTExprCast::create(CodegenASTType::create("int"), CodegenASTExprAccess::create("a")));
  auto testCond = static_cast<std::shared_ptr<const CodegenASTExpr>>(CodegenASTExprCond::create(CodegenASTExprAccess::create("a"), CodegenASTExprLiteral::create("1"), CodegenASTExprLiteral::create("2")));
  auto testInitList = static_cast<std::shared_ptr<const CodegenASTExpr>>(CodegenASTExprInitList::create());
  auto testLiteral = static_cast<std::shared_ptr<const CodegenASTExpr>>(CodegenASTExprLiteral::create("1"));
  auto testNull = static_cast<std::shared_ptr<const CodegenASTExpr>>(CodegenASTExprNull::create());
  auto testUnary = static_cast<std::shared_ptr<const CodegenASTExpr>>(CodegenASTExprUnary::create("!", CodegenASTExprAccess::create("a")));

  EXPECT_NO_THROW(testAccess->asAccess());
  EXPECT_NO_THROW(testAssign->asAssign());
  EXPECT_NO_THROW(testBinary->asBinary());
  EXPECT_NO_THROW(testCall->asCall());
  EXPECT_NO_THROW(testCast->asCast());
  EXPECT_NO_THROW(testCond->asCond());
  EXPECT_NO_THROW(testInitList->asInitList());
  EXPECT_NO_THROW(testLiteral->asLiteral());
  EXPECT_NO_THROW(testNull->asNull());
  EXPECT_NO_THROW(testUnary->asUnary());
}

TEST(CodegenASTTest, ExprIsMethods) {
  EXPECT_TRUE(CodegenASTExprAccess::create("a")->isAccess());
  EXPECT_TRUE(CodegenASTExprAssign::create(CodegenASTExprAccess::create("a"), "=", CodegenASTExprLiteral::create("1"))->isAssign());
  EXPECT_TRUE(CodegenASTExprBinary::create(CodegenASTExprAccess::create("a"), "==", CodegenASTExprAccess::create("b"))->isBinary());
  EXPECT_TRUE(CodegenASTExprCall::create(CodegenASTExprAccess::create("a"))->isCall());
  EXPECT_TRUE(CodegenASTExprCast::create(CodegenASTType::create("int"), CodegenASTExprAccess::create("a"))->isCast());
  EXPECT_TRUE(CodegenASTExprCond::create(CodegenASTExprAccess::create("a"), CodegenASTExprLiteral::create("1"), CodegenASTExprLiteral::create("2"))->isCond());
  EXPECT_TRUE(CodegenASTExprInitList::create()->isInitList());
  EXPECT_TRUE(CodegenASTExprLiteral::create("1")->isLiteral());
  EXPECT_TRUE(CodegenASTExprNull::create()->isNull());
  EXPECT_TRUE(CodegenASTExprUnary::create("!", CodegenASTExprAccess::create("a"))->isUnary());
}

TEST(CodegenASTTest, ExprGetPtr) {
  EXPECT_NE(CodegenASTExprAccess::create("a")->getptr(), nullptr);
  EXPECT_NE(CodegenASTExprNull::create()->getptr(), nullptr);
}

TEST(CodegenASTTest, ExprIsBuiltinLiteral) {
  EXPECT_TRUE(CodegenASTExprAccess::create("THE_EOL")->isBuiltinLiteral());
  EXPECT_TRUE(CodegenASTExprAccess::create("THE_PATH_SEP")->isBuiltinLiteral());
}

TEST(CodegenASTTest, ExprIsEmptyString) {
  EXPECT_TRUE(CodegenASTExprLiteral::create(R"("")")->isEmptyString());
}

TEST(CodegenASTTest, ExprIsPointer) {
  EXPECT_TRUE(CodegenASTExprUnary::create("*", CodegenASTExprAccess::create("test"))->isPointer());
}

TEST(CodegenASTTest, ExprStmt) {
  EXPECT_NE(CodegenASTExprAccess::create("test")->stmt(), nullptr);
}

TEST(CodegenASTTest, ExprWrap) {
  EXPECT_FALSE(CodegenASTExprAccess::create("test")->parenthesized);
  EXPECT_TRUE(CodegenASTExprAccess::create("test")->wrap()->parenthesized);
}

TEST(CodegenASTTest, ExprAccess) {
  EXPECT_EQ(CodegenASTExprAccess::create("test")->str(), "test");
  EXPECT_EQ(CodegenASTExprAccess::create(CodegenASTExprAccess::create("test"), "field")->str(), "test.field");
  EXPECT_EQ(CodegenASTExprAccess::create(CodegenASTExprAccess::create("test"), "field", true)->str(), "test->field");

  EXPECT_EQ(
    CodegenASTExprAccess::create(CodegenASTExprAccess::create("test"), CodegenASTExprAccess::create("i"))->str(),
    "test[i]"
  );
}

TEST(CodegenASTTest, ExprAssign) {
  EXPECT_EQ(
    CodegenASTExprAssign::create(
      CodegenASTExprAccess::create("test"),
      "=",
      CodegenASTExprLiteral::create("1")
    )->str(),
    "test = 1"
  );
}

TEST(CodegenASTTest, ExprBinary) {
  EXPECT_EQ(
    CodegenASTExprBinary::create(
      CodegenASTExprAccess::create("test"),
      "==",
      CodegenASTExprLiteral::create("1")
    )->str(),
    "test == 1"
  );
}

TEST(CodegenASTTest, ExprCall) {
  EXPECT_EQ(CodegenASTExprCall::create(CodegenASTExprAccess::create("test"))->str(), "test()");

  EXPECT_EQ(
    CodegenASTExprCall::create(
      CodegenASTExprAccess::create("test"),
      {
        CodegenASTExprAccess::create("a"),
        CodegenASTExprAccess::create("b")
      }
    )->str(),
    "test(a, b)"
  );

  EXPECT_EQ(
    CodegenASTExprCall::create(
      CodegenASTExprAccess::create("test"),
      {
        CodegenASTExprNull::create(),
        CodegenASTExprNull::create()
      }
    )->str(),
    "test()"
  );

  EXPECT_EQ(
    CodegenASTExprCall::create(
      CodegenASTExprAccess::create("test"),
      {
        CodegenASTType::create("int"),
        CodegenASTType::create("char")
      }
    )->str(),
    "test(int, char)"
  );
}

TEST(CodegenASTTest, ExprCast) {
  EXPECT_EQ(
    CodegenASTExprCast::create(CodegenASTType::create("int"), CodegenASTExprAccess::create("a"))->str(),
    "(int) a"
  );

  EXPECT_EQ(
    CodegenASTExprCast::create(CodegenASTType::create("int "), CodegenASTExprAccess::create("a"))->str(),
    "(int) a"
  );

  EXPECT_EQ(
    CodegenASTExprCast::create(CodegenASTType::create("int *"), CodegenASTExprAccess::create("a"))->str(),
    "(int *) a"
  );
}

TEST(CodegenASTTest, ExprCond) {
  EXPECT_EQ(
    CodegenASTExprCond::create(
      CodegenASTExprAccess::create("a"),
      CodegenASTExprAccess::create("1"),
      CodegenASTExprAccess::create("2")
    )->str(),
    "a ? 1 : 2"
  );
}

TEST(CodegenASTTest, ExprInitList) {
  EXPECT_EQ(CodegenASTExprInitList::create()->str(), "{}");

  EXPECT_EQ(CodegenASTExprInitList::create({
    CodegenASTExprAccess::create("a")
  })->str(), "{a}");

  EXPECT_EQ(CodegenASTExprInitList::create({
    CodegenASTExprAccess::create("a"),
    CodegenASTExprAccess::create("b")
  })->str(), "{a, b}");

  EXPECT_EQ(CodegenASTExprInitList::create({
    CodegenASTExprNull::create(),
    CodegenASTExprNull::create()
  })->str(), "{}");
}

TEST(CodegenASTTest, ExprLiteral) {
  auto asyncCounter = std::make_shared<std::size_t>(0);
  EXPECT_EQ(CodegenASTExprLiteral::create("1")->str(), "1");
  EXPECT_EQ(CodegenASTExprLiteral::create(R"("string")")->str(), R"("string")");
  EXPECT_EQ(CodegenASTExprLiteral::create("'a'")->str(), "'a'");
  EXPECT_EQ(CodegenASTExprLiteral::create(asyncCounter)->str(), "0");
  *asyncCounter = 1;
  EXPECT_EQ(CodegenASTExprLiteral::create(asyncCounter)->str(), "1");
}

TEST(CodegenASTTest, ExprNull) {
  EXPECT_EQ(CodegenASTExprNull::create()->str(), "");
}

TEST(CodegenASTTest, ExprUnary) {
  EXPECT_EQ(CodegenASTExprUnary::create("++", CodegenASTExprAccess::create("a"))->str(), "++a");
  EXPECT_EQ(CodegenASTExprUnary::create(CodegenASTExprAccess::create("a"), "++")->str(), "a++");
}

TEST(CodegenASTTest, ExprParenthesized) {
  EXPECT_EQ(CodegenASTExprAccess::create("a")->wrap()->str(), "(a)");
}
