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
#include "../src/CParser.hpp"
#include "../src/config.hpp"

TEST(CParserTest, ParsesEmpty) {
  auto cParser = cParse("void test () {" EOL "}");

  EXPECT_EQ(cParser.decl(), "void test ()");
  EXPECT_EQ(cParser.returnType, "void ");
  EXPECT_EQ(cParser.name, "test");
  EXPECT_EQ(cParser.params.size(), 0);
}

TEST(CParserTest, ParsesEmptyParams) {
  auto cParser = cParse("void test () {" EOL "  printf(\"Test\");" EOL "}");

  EXPECT_EQ(cParser.decl(), "void test ()");
  EXPECT_EQ(cParser.returnType, "void ");
  EXPECT_EQ(cParser.name, "test");
  EXPECT_EQ(cParser.params.size(), 0);
}

TEST(CParserTest, ParsesComplexReturnType) {
  auto cParser = cParse("__unused __stdcall void test () {" EOL "}");

  EXPECT_EQ(cParser.decl(), "__unused __stdcall void test ()");
  EXPECT_EQ(cParser.returnType, "__unused __stdcall void ");
  EXPECT_EQ(cParser.name, "test");
  EXPECT_EQ(cParser.params.size(), 0);
}

TEST(CParserTest, ParsesComplexName) {
  auto cParser = cParse("void test_$1 () {" EOL "}");

  EXPECT_EQ(cParser.decl(), "void test_$1 ()");
  EXPECT_EQ(cParser.returnType, "void ");
  EXPECT_EQ(cParser.name, "test_$1");
  EXPECT_EQ(cParser.params.size(), 0);
}

TEST(CParserTest, ParsesOneParam) {
  auto cParser = cParse("void test (int x) {" EOL "}");

  EXPECT_EQ(cParser.decl(), "void test (int)");
  EXPECT_EQ(cParser.returnType, "void ");
  EXPECT_EQ(cParser.name, "test");
  EXPECT_EQ(cParser.params.size(), 1);
  EXPECT_EQ(cParser.params[0].type, "int ");
  EXPECT_EQ(cParser.params[0].name, "x");
}

TEST(CParserTest, ParsesTwoParams) {
  auto cParser = cParse("void test (int x, char var) {" EOL "}");

  EXPECT_EQ(cParser.decl(), "void test (int, char)");
  EXPECT_EQ(cParser.returnType, "void ");
  EXPECT_EQ(cParser.name, "test");
  EXPECT_EQ(cParser.params.size(), 2);
  EXPECT_EQ(cParser.params[0].type, "int ");
  EXPECT_EQ(cParser.params[0].name, "x");
  EXPECT_EQ(cParser.params[1].type, "char ");
  EXPECT_EQ(cParser.params[1].name, "var");
}

TEST(CParserTest, ParsesThreeParams) {
  auto cParser = cParse("void test (int x, char var, string param) {" EOL "}");

  EXPECT_EQ(cParser.decl(), "void test (int, char, string)");
  EXPECT_EQ(cParser.returnType, "void ");
  EXPECT_EQ(cParser.name, "test");
  EXPECT_EQ(cParser.params.size(), 3);
  EXPECT_EQ(cParser.params[0].type, "int ");
  EXPECT_EQ(cParser.params[0].name, "x");
  EXPECT_EQ(cParser.params[1].type, "char ");
  EXPECT_EQ(cParser.params[1].name, "var");
  EXPECT_EQ(cParser.params[2].type, "string ");
  EXPECT_EQ(cParser.params[2].name, "param");
}

TEST(CParserTest, ParsesParamComplexName) {
  auto cParser = cParse("void test (int test_$1) {" EOL "}");

  EXPECT_EQ(cParser.decl(), "void test (int)");
  EXPECT_EQ(cParser.returnType, "void ");
  EXPECT_EQ(cParser.name, "test");
  EXPECT_EQ(cParser.params.size(), 1);
  EXPECT_EQ(cParser.params[0].type, "int ");
  EXPECT_EQ(cParser.params[0].name, "test_$1");
}

TEST(CParserTest, ParsesParamComplexType) {
  auto cParser = cParse("void test (__unused __stdcall int test) {" EOL "}");

  EXPECT_EQ(cParser.decl(), "void test (__unused __stdcall int)");
  EXPECT_EQ(cParser.returnType, "void ");
  EXPECT_EQ(cParser.name, "test");
  EXPECT_EQ(cParser.params.size(), 1);
  EXPECT_EQ(cParser.params[0].type, "__unused __stdcall int ");
  EXPECT_EQ(cParser.params[0].name, "test");
}

TEST(CParserTest, ParsesParamFunctionEmpty) {
  auto cParser = cParse("void test (void (*f) ()) {" EOL "}");

  EXPECT_EQ(cParser.decl(), "void test (void (*f) ())");
  EXPECT_EQ(cParser.returnType, "void ");
  EXPECT_EQ(cParser.name, "test");
  EXPECT_EQ(cParser.params.size(), 1);
  EXPECT_EQ(cParser.params[0].type, "void (*f) ()");
  EXPECT_EQ(cParser.params[0].name, "");
}

TEST(CParserTest, ParsesParamFunctionOneParam) {
  auto cParser = cParse("void test (void (*f) (char)) {" EOL "}");

  EXPECT_EQ(cParser.decl(), "void test (void (*f) (char))");
  EXPECT_EQ(cParser.returnType, "void ");
  EXPECT_EQ(cParser.name, "test");
  EXPECT_EQ(cParser.params.size(), 1);
  EXPECT_EQ(cParser.params[0].type, "void (*f) (char)");
  EXPECT_EQ(cParser.params[0].name, "");
}

TEST(CParserTest, ParsesParamFunctionOneParamWithName) {
  auto cParser = cParse("void test (void (*f) (char p1)) {" EOL "}");

  EXPECT_EQ(cParser.decl(), "void test (void (*f) (char p1))");
  EXPECT_EQ(cParser.returnType, "void ");
  EXPECT_EQ(cParser.name, "test");
  EXPECT_EQ(cParser.params.size(), 1);
  EXPECT_EQ(cParser.params[0].type, "void (*f) (char p1)");
  EXPECT_EQ(cParser.params[0].name, "");
}

TEST(CParserTest, ParsesParamFunctionTwoParams) {
  auto cParser = cParse("void test (void (*f) (char, int)) {" EOL "}");

  EXPECT_EQ(cParser.decl(), "void test (void (*f) (char, int))");
  EXPECT_EQ(cParser.returnType, "void ");
  EXPECT_EQ(cParser.name, "test");
  EXPECT_EQ(cParser.params.size(), 1);
  EXPECT_EQ(cParser.params[0].type, "void (*f) (char, int)");
  EXPECT_EQ(cParser.params[0].name, "");
}

TEST(CParserTest, ParsesParamFunctionTwoParamsWithNames) {
  auto cParser = cParse("void test (void (*f) (char p1, int p2)) {" EOL "}");

  EXPECT_EQ(cParser.decl(), "void test (void (*f) (char p1, int p2))");
  EXPECT_EQ(cParser.returnType, "void ");
  EXPECT_EQ(cParser.name, "test");
  EXPECT_EQ(cParser.params.size(), 1);
  EXPECT_EQ(cParser.params[0].type, "void (*f) (char p1, int p2)");
  EXPECT_EQ(cParser.params[0].name, "");
}

TEST(CParserTest, ParsesParamVariadicFirst) {
  auto cParser = cParse("void test (...) {" EOL "}");

  EXPECT_EQ(cParser.decl(), "void test (...)");
  EXPECT_EQ(cParser.returnType, "void ");
  EXPECT_EQ(cParser.name, "test");
  EXPECT_EQ(cParser.params.size(), 1);
  EXPECT_EQ(cParser.params[0].type, "...");
  EXPECT_EQ(cParser.params[0].name, "");
}

TEST(CParserTest, ParsesParamVariadicSecond) {
  auto cParser = cParse("void test (int a, ...) {" EOL "}");

  EXPECT_EQ(cParser.decl(), "void test (int, ...)");
  EXPECT_EQ(cParser.returnType, "void ");
  EXPECT_EQ(cParser.name, "test");
  EXPECT_EQ(cParser.params.size(), 2);
  EXPECT_EQ(cParser.params[0].type, "int ");
  EXPECT_EQ(cParser.params[0].name, "a");
  EXPECT_EQ(cParser.params[1].type, "...");
  EXPECT_EQ(cParser.params[1].name, "");
}
