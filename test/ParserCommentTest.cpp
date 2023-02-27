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
#include "../src/ParserComment.hpp"
#include "../src/config.hpp"

TEST(ParserCommentTest, Empty) {
  auto comment = parseComment("");

  EXPECT_EQ(comment.description, "");
  EXPECT_EQ(comment.image, "");
  EXPECT_EQ(comment.notes.size(), 0);
  EXPECT_EQ(comment.params.size(), 0);
  EXPECT_EQ(comment.ret, "");
  EXPECT_EQ(comment.sign, "");
}

TEST(ParserCommentTest, Description) {
  EXPECT_EQ(parseComment("*").description, "");
  EXPECT_EQ(parseComment("test").description, "test");
  EXPECT_EQ(parseComment("test description").description, "test description");
  EXPECT_EQ(parseComment("test\ndescription").description, "test description");
  EXPECT_EQ(parseComment("test\ndescription").description, "test description");
  EXPECT_EQ(parseComment("test\n description").description, "test description");
  EXPECT_EQ(parseComment("test\n *description").description, "test description");
  EXPECT_EQ(parseComment("test \n * \n description").description, "test description");
  EXPECT_EQ(parseComment("test\n * description").description, "test description");
  EXPECT_EQ(parseComment("test \n * description\n * multiline").description, "test description multiline");
  EXPECT_EQ(parseComment("test \n * \n description \n * \n multiline").description, "test description multiline");
}

TEST(ParserCommentTest, DescriptionMultiline) {
  EXPECT_EQ(parseComment("! description").description, "! description");
  EXPECT_EQ(parseComment("test \n ! description").description, "test" EOL "! description");
  EXPECT_EQ(parseComment("test \n ! description\n ! multiline").description, "test" EOL "! description" EOL "! multiline");
}

TEST(ParserCommentTest, Image) {
  auto comment1 = ParserComment{"test", "![test](/test.svg)"};
  auto comment2 = ParserComment{"", "![test](/test.svg)"};
  auto comment3 = ParserComment{"test", "![test](/test.svg)"};
  auto comment4 = ParserComment{"test test", "![test](/test.svg)"};

  EXPECT_EQ(parseComment("test \n ![test](/test.svg)"), comment1);
  EXPECT_EQ(parseComment("![test](/test.svg)"), comment2);
  EXPECT_EQ(parseComment("![test](/test.svg) \n test"), comment3);
  EXPECT_EQ(parseComment("test \n ![test](/test.svg) \n test"), comment4);
}

TEST(ParserCommentTest, Params) {
  auto comment1 = ParserComment{
    .params = {
      {"test", "Test"}
    }
  };

  auto comment2 = ParserComment{
    .params = {
      {"test", ""}
    }
  };

  auto comment3 = ParserComment{
    .params = {
      {"test1", "Test 1"},
      {"test2", "Test 2"}
    }
  };

  auto comment4 = ParserComment{
    .params = {
      {"test1", "Test 1"},
      {"test2", "Test 2"},
      {"test3", "Test 3"}
    }
  };

  auto comment5 = ParserComment{
    .params = {
      {"test", "Test 1 Test 2"}
    }
  };

  auto comment6 = ParserComment{
    .params = {
      {"test", "Test 1\n- Test 2"}
    }
  };

  auto comment7 = ParserComment{
    .params = {
      {"test", "Test 1\n  - Test 2"}
    }
  };

  EXPECT_EQ(parseComment("@param test - Test"), comment1);
  EXPECT_EQ(parseComment("@param test Test"), comment1);
  EXPECT_EQ(parseComment("@param test"), comment2);
  EXPECT_EQ(parseComment("@param test1 - Test 1\n @param test2 - Test 2"), comment3);
  EXPECT_EQ(parseComment("@param test1 Test 1\n @param test2 Test 2\n @param test3 Test 3"), comment4);
  EXPECT_EQ(parseComment("@param test - Test 1\n Test 2"), comment5);
  EXPECT_EQ(parseComment("@param test - Test 1\n *Test 2"), comment5);
  EXPECT_EQ(parseComment("@param test - Test 1\n * Test 2"), comment5);
  EXPECT_EQ(parseComment("@param test - Test 1\n - Test 2"), comment6);
  EXPECT_EQ(parseComment("@param test - Test 1\n *- Test 2"), comment6);
  EXPECT_EQ(parseComment("@param test - Test 1\n *   - Test 2"), comment7);
}

TEST(ParserCommentTest, Notes) {
  auto comment1 = ParserComment{.notes = {"test"}};
  auto comment2 = ParserComment{.notes = {"test description"}};
  auto comment3 = ParserComment{.notes = {"test description multiline"}};
  auto comment4 = ParserComment{.notes = {"test\\\ndescription multiline"}};
  auto comment5 = ParserComment{.notes = {"test\\\ndescription\\\nmultiline"}};
  auto comment6 = ParserComment{.notes = {"test1", "test2"}};

  EXPECT_EQ(parseComment("@note test"), comment1);
  EXPECT_EQ(parseComment("@note test description"), comment2);
  EXPECT_EQ(parseComment("@note test\ndescription"), comment2);
  EXPECT_EQ(parseComment("@note test\ndescription"), comment2);
  EXPECT_EQ(parseComment("@note test\n description"), comment2);
  EXPECT_EQ(parseComment("@note test\n *description"), comment2);
  EXPECT_EQ(parseComment("@note test \n * \n description"), comment2);
  EXPECT_EQ(parseComment("@note test\n * description"), comment2);
  EXPECT_EQ(parseComment("@note test \n * description\n * multiline"), comment3);
  EXPECT_EQ(parseComment("@note test \n * \n description \n * \n multiline"), comment3);
  EXPECT_EQ(parseComment("@note test\\\n * description\n * multiline"), comment4);
  EXPECT_EQ(parseComment("@note test\\\n * description\\\n * multiline"), comment5);
  EXPECT_EQ(parseComment("@note test1\n @note test2"), comment6);
  EXPECT_EQ(parseComment("@note test1\n *@note test2"), comment6);
  EXPECT_EQ(parseComment("@note test1\n * @note test2"), comment6);
}

TEST(ParserCommentTest, Ret) {
  auto comment1 = ParserComment{.description = "test", .ret = "type"};
  auto comment2 = ParserComment{.description = ""};
  auto comment3 = ParserComment{.description = "", .ret = "type"};
  auto comment4 = ParserComment{.description = "", .ret = "type"};
  auto comment5 = ParserComment{.description = "test", .ret = "type"};

  EXPECT_EQ(parseComment("test \n @return type"), comment1);
  EXPECT_EQ(parseComment("@return"), comment2);
  EXPECT_EQ(parseComment("@return type"), comment3);
  EXPECT_EQ(parseComment("@return type \n test"), comment4);
  EXPECT_EQ(parseComment("test \n @return type \n test"), comment5);
}

TEST(ParserCommentTest, Sign) {
  auto comment1 = ParserComment{.description = "test", .sign = "[idx: int]"};
  auto comment2 = ParserComment{.description = ""};
  auto comment3 = ParserComment{.description = "", .sign = "() void"};
  auto comment4 = ParserComment{.description = "", .sign = "(a: int, b: int) int"};
  auto comment5 = ParserComment{.description = "test", .sign = "[idx: int]"};

  EXPECT_EQ(parseComment("test \n @signature [idx: int]"), comment1);
  EXPECT_EQ(parseComment("@signature"), comment2);
  EXPECT_EQ(parseComment("@signature () void"), comment3);
  EXPECT_EQ(parseComment("@signature (a: int, b: int) int \n test"), comment4);
  EXPECT_EQ(parseComment("test \n @signature [idx: int] \n test"), comment5);
}
