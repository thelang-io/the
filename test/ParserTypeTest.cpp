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
#include "../src/ParserType.hpp"
#include "../src/config.hpp"
#include "MockParser.hpp"

ParserType parserTypeTestGen (const std::string &code) {
  auto block = testing::NiceMock<MockParser>("const a: " + code).next(true, false);
  auto stmtVarDecl = std::get<ParserStmtVarDecl>(*block.body);
  return *stmtVarDecl.type;
}

TEST(ParserTypeTest, StringifyParenthesized) {
  EXPECT_EQ(parserTypeTestGen("( int )").stringify(), "(int)");
  EXPECT_EQ(parserTypeTestGen("( int,int ) -> ( int )").stringify(), "(int, int) -> (int)");
}

TEST(ParserTypeTest, StringifyArray) {
  EXPECT_EQ(parserTypeTestGen("int [ ]").stringify(), "int[]");
  EXPECT_EQ(parserTypeTestGen("Test [ ]").stringify(), "Test[]");
}

TEST(ParserTypeTest, StringifyFn) {
  EXPECT_EQ(parserTypeTestGen("( )->void").stringify(), "() -> void");
  EXPECT_EQ(parserTypeTestGen("( int )->int").stringify(), "(int) -> int");
  EXPECT_EQ(parserTypeTestGen("( int ... )->int").stringify(), "(int...) -> int");
  EXPECT_EQ(parserTypeTestGen("( a:int )->int").stringify(), "(a: int) -> int");
  EXPECT_EQ(parserTypeTestGen("( mut  a:int )->int").stringify(), "(mut a: int) -> int");
  EXPECT_EQ(parserTypeTestGen("( int,int )->int").stringify(), "(int, int) -> int");
  EXPECT_EQ(parserTypeTestGen("( int ...,int ... )->int").stringify(), "(int..., int...) -> int");
  EXPECT_EQ(parserTypeTestGen("( a:int,b:int )->int").stringify(), "(a: int, b: int) -> int");
  EXPECT_EQ(parserTypeTestGen("( a:int,mut  b:int )->int").stringify(), "(a: int, mut b: int) -> int");
  EXPECT_EQ(parserTypeTestGen("( a:int ...,mut  b:int ... )->int").stringify(), "(a: int..., mut b: int...) -> int");
}

TEST(ParserTypeTest, StringifyId) {
  EXPECT_EQ(parserTypeTestGen("int").stringify(), "int");
  EXPECT_EQ(parserTypeTestGen("Test").stringify(), "Test");
}

TEST(ParserTypeTest, StringifyMap) {
  EXPECT_EQ(parserTypeTestGen("int [ str ]").stringify(), "int[str]");
  EXPECT_EQ(parserTypeTestGen("Test1 [ Test2 ]").stringify(), "Test1[Test2]");
}

TEST(ParserTypeTest, StringifyOptional) {
  EXPECT_EQ(parserTypeTestGen("int ?").stringify(), "int?");
  EXPECT_EQ(parserTypeTestGen("Test ?").stringify(), "Test?");
}

TEST(ParserTypeTest, StringifyRef) {
  EXPECT_EQ(parserTypeTestGen("ref  int").stringify(), "ref int");
  EXPECT_EQ(parserTypeTestGen("ref  Test").stringify(), "ref Test");
}

TEST(ParserTypeTest, StringifyUnion) {
  EXPECT_EQ(parserTypeTestGen("int|str").stringify(), "int | str");
  EXPECT_EQ(parserTypeTestGen("Test1|Test2").stringify(), "Test1 | Test2");
}
