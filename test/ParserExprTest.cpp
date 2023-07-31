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
#include "../src/config.hpp"
#include "MockParser.hpp"

ParserStmtExpr parserExprTestGen (const std::string &code) {
  auto block = testing::NiceMock<MockParser>("main {" EOL + code + EOL "}" EOL).next(true, false);
  auto stmtMain = std::get<ParserStmtMain>(*block.body);
  return std::get<ParserStmtExpr>(*stmtMain.body[0].body);
}

TEST(ParserExprTest, StringifyParenthesized) {
  EXPECT_EQ(parserExprTestGen("( 1+1 )").stringify(), "(1 + 1)");
  EXPECT_EQ(parserExprTestGen("( 1+2 )+3").stringify(), "(1 + 2) + 3");
}

TEST(ParserExprTest, StringifyAccess) {
  EXPECT_EQ(parserExprTestGen("a").stringify(), "a");
  EXPECT_EQ(parserExprTestGen("a . prop").stringify(), "a.prop");
  EXPECT_EQ(parserExprTestGen("a [ 100 ]").stringify(), "a[100]");
  EXPECT_EQ(parserExprTestGen(". Test").stringify(), ".Test");
}

TEST(ParserExprTest, StringifyArray) {
  EXPECT_EQ(parserExprTestGen("[]").stringify(), "[]");
  EXPECT_EQ(parserExprTestGen("[1,2,3]").stringify(), "[1, 2, 3]");
}

TEST(ParserExprTest, StringifyAs) {
  EXPECT_EQ(parserExprTestGen("a as  int").stringify(), "a as int");
}

TEST(ParserExprTest, StringifyAssign) {
  EXPECT_EQ(parserExprTestGen("a=2").stringify(), "a = 2");
  EXPECT_EQ(parserExprTestGen("a=b=2").stringify(), "a = b = 2");
}

TEST(ParserExprTest, StringifyAwait) {
  EXPECT_EQ(parserExprTestGen("await test ()").stringify(), "await test()");
  EXPECT_EQ(parserExprTestGen("await( await test () ) ( )").stringify(), "await (await test())()");
}

TEST(ParserExprTest, StringifyBinary) {
  EXPECT_EQ(parserExprTestGen("1+1").stringify(), "1 + 1");
  EXPECT_EQ(parserExprTestGen("1+2+3").stringify(), "1 + 2 + 3");
}

TEST(ParserExprTest, StringifyCall) {
  EXPECT_EQ(parserExprTestGen("test ()").stringify(), "test()");
  EXPECT_EQ(parserExprTestGen("test (1,2,3)").stringify(), "test(1, 2, 3)");
  EXPECT_EQ(parserExprTestGen("test (1,a:2,b:3)").stringify(), "test(1, a: 2, b: 3)");
}

TEST(ParserExprTest, StringifyClosure) {
  EXPECT_EQ(
    parserExprTestGen(
      "async  ( a : int , mut  b : str   ...  )->int{ " EOL
      "  print ( a ) " EOL
      "}"
    ).stringify(),
    "async (a: int, mut b: str...) -> int {" EOL
    "  print(a)" EOL
    "}" EOL
  );
}

TEST(ParserExprTest, StringifyCond) {
  EXPECT_EQ(parserExprTestGen("true?1:2").stringify(), "true ? 1 : 2");
  EXPECT_EQ(parserExprTestGen("true?false?1:2:3").stringify(), "true ? false ? 1 : 2 : 3");
  EXPECT_EQ(parserExprTestGen("true?1:false?2:3").stringify(), "true ? 1 : false ? 2 : 3");
}

TEST(ParserExprTest, StringifyIs) {
  EXPECT_EQ(parserExprTestGen("a  is  int").stringify(), "a is int");
}

TEST(ParserExprTest, StringifyLit) {
  EXPECT_EQ(parserExprTestGen("true").stringify(), "true");
  EXPECT_EQ(parserExprTestGen("false").stringify(), "false");
  EXPECT_EQ(parserExprTestGen("nil").stringify(), "nil");
  EXPECT_EQ(parserExprTestGen("'a'").stringify(), "'a'");
  EXPECT_EQ(parserExprTestGen("3.14").stringify(), "3.14");
  EXPECT_EQ(parserExprTestGen("100").stringify(), "100");
  EXPECT_EQ(parserExprTestGen("0b1010").stringify(), "0b1010");
  EXPECT_EQ(parserExprTestGen("0xFF").stringify(), "0xFF");
  EXPECT_EQ(parserExprTestGen("0o777").stringify(), "0o777");
  EXPECT_EQ(parserExprTestGen(R"("string")").stringify(), R"("string")");
}

TEST(ParserExprTest, StringifyMap) {
  EXPECT_EQ(parserExprTestGen("{}").stringify(), "{}");
  EXPECT_EQ(parserExprTestGen(R"({"key":1})").stringify(), R"({"key": 1})");
  EXPECT_EQ(parserExprTestGen(R"({"key1":1,"key2":2})").stringify(), R"({"key1": 1, "key2": 2})");
}

TEST(ParserExprTest, StringifyObj) {
  EXPECT_EQ(parserExprTestGen("Test{}").stringify(), "Test{}");
  EXPECT_EQ(parserExprTestGen("Test{a:1}").stringify(), "Test{a: 1}");
  EXPECT_EQ(parserExprTestGen("Test{a:1,b:2}").stringify(), "Test{a: 1, b: 2}");
}

TEST(ParserExprTest, StringifyRef) {
  EXPECT_EQ(parserExprTestGen("ref  a").stringify(), "ref a");
}

TEST(ParserExprTest, StringifyUnary) {
  EXPECT_EQ(parserExprTestGen("! 10").stringify(), "!10");
  EXPECT_EQ(parserExprTestGen("- 10").stringify(), "-10");
  EXPECT_EQ(parserExprTestGen("-- a").stringify(), "--a");
  EXPECT_EQ(parserExprTestGen("+ 10").stringify(), "+10");
  EXPECT_EQ(parserExprTestGen("++ a").stringify(), "++a");
  EXPECT_EQ(parserExprTestGen("~ 10").stringify(), "~10");
  EXPECT_EQ(parserExprTestGen("a --").stringify(), "a--");
  EXPECT_EQ(parserExprTestGen("a ++").stringify(), "a++");
}
