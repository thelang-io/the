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
#include "../src/VarMap.hpp"

class VarMapTest : public testing::Test {
 protected:
  VarMap vm_;
  TypeMap tm_;

  void SetUp () override {
    this->tm_.init("test");
    this->vm_.init(this->tm_);
  }
};

TEST_F(VarMapTest, AddInsertsVar) {
  this->vm_.add("test1", "test1_0", this->tm_.get("int"), false);
  this->vm_.add("test2", "test2_0", this->tm_.get("any"), true);
  this->vm_.add("test3", "test3_0", this->tm_.get("str"), false, true);
  this->vm_.add("test4", "test4_0", this->tm_.get("float"), false, false, true);

  EXPECT_NO_THROW(this->vm_.get("test1"));
  EXPECT_NO_THROW(this->vm_.get("test2"));
  EXPECT_NO_THROW(this->vm_.get("test3"));
  EXPECT_NO_THROW(this->vm_.get("test4"));

  EXPECT_EQ(this->vm_.get("test1")->name, "test1");
  EXPECT_EQ(this->vm_.get("test1")->codeName, "test1_0");
  EXPECT_TRUE(this->tm_.get("int")->matchStrict(this->vm_.get("test1")->type));
  EXPECT_FALSE(this->vm_.get("test1")->mut);
  EXPECT_FALSE(this->vm_.get("test1")->constant);
  EXPECT_FALSE(this->vm_.get("test1")->builtin);
  EXPECT_FALSE(this->vm_.get("test1")->ctxIgnored);
  EXPECT_EQ(this->vm_.get("test1")->frame, 0);

  EXPECT_EQ(this->vm_.get("test2")->name, "test2");
  EXPECT_EQ(this->vm_.get("test2")->codeName, "test2_0");
  EXPECT_TRUE(this->tm_.get("any")->matchStrict(this->vm_.get("test2")->type));
  EXPECT_TRUE(this->vm_.get("test2")->mut);
  EXPECT_FALSE(this->vm_.get("test2")->constant);
  EXPECT_FALSE(this->vm_.get("test2")->builtin);
  EXPECT_FALSE(this->vm_.get("test2")->ctxIgnored);
  EXPECT_EQ(this->vm_.get("test2")->frame, 0);

  EXPECT_EQ(this->vm_.get("test3")->name, "test3");
  EXPECT_EQ(this->vm_.get("test3")->codeName, "test3_0");
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(this->vm_.get("test3")->type));
  EXPECT_FALSE(this->vm_.get("test3")->mut);
  EXPECT_TRUE(this->vm_.get("test3")->constant);
  EXPECT_FALSE(this->vm_.get("test3")->builtin);
  EXPECT_FALSE(this->vm_.get("test3")->ctxIgnored);
  EXPECT_EQ(this->vm_.get("test3")->frame, 0);

  EXPECT_EQ(this->vm_.get("test4")->name, "test4");
  EXPECT_EQ(this->vm_.get("test4")->codeName, "test4_0");
  EXPECT_TRUE(this->tm_.get("float")->matchStrict(this->vm_.get("test4")->type));
  EXPECT_FALSE(this->vm_.get("test4")->mut);
  EXPECT_FALSE(this->vm_.get("test4")->constant);
  EXPECT_FALSE(this->vm_.get("test4")->builtin);
  EXPECT_TRUE(this->vm_.get("test4")->ctxIgnored);
  EXPECT_EQ(this->vm_.get("test4")->frame, 0);

  this->vm_.save();
  this->vm_.add("test1", "test1_1", this->tm_.get("str"), true);

  EXPECT_NO_THROW(this->vm_.get("test1"));

  EXPECT_EQ(this->vm_.get("test1")->name, "test1");
  EXPECT_EQ(this->vm_.get("test1")->codeName, "test1_1");
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(this->vm_.get("test1")->type));
  EXPECT_TRUE(this->vm_.get("test1")->mut);
  EXPECT_FALSE(this->vm_.get("test1")->builtin);
  EXPECT_EQ(this->vm_.get("test1")->frame, 1);
}

TEST_F(VarMapTest, AddsNamespace) {
  auto typeNamespace = this->tm_.createNamespace("TestNamespace", {});
  this->vm_.addNamespace("Test", typeNamespace);

  EXPECT_NO_THROW(this->vm_.get("Test"));
  EXPECT_EQ(this->vm_.get("Test")->name, "Test");
  EXPECT_EQ(this->vm_.get("Test")->codeName, "Test_0");
  EXPECT_TRUE(typeNamespace->matchStrict(this->vm_.get("Test")->type));
  EXPECT_FALSE(this->vm_.get("Test")->mut);
  EXPECT_FALSE(this->vm_.get("Test")->constant);
  EXPECT_FALSE(this->vm_.get("Test")->builtin);
  EXPECT_TRUE(this->vm_.get("Test")->ctxIgnored);
  EXPECT_EQ(this->vm_.get("Test")->frame, 0);
}

TEST_F(VarMapTest, GetThrowOnEmpty) {
  auto vm = VarMap();
  EXPECT_EQ(vm.get("test1"), nullptr);
}

TEST_F(VarMapTest, GetReturnsItem) {
  this->vm_.add("test1", "test1_0", this->tm_.get("int"), false);
  EXPECT_EQ(this->vm_.get("test1")->codeName, "test1_0");
  this->vm_.save();
  this->vm_.add("test1", "test1_1", this->tm_.get("int"), false);
  EXPECT_EQ(this->vm_.get("test1")->codeName, "test1_1");
  this->vm_.save();
  this->vm_.add("test1", "test1_2", this->tm_.get("int"), false);
  EXPECT_EQ(this->vm_.get("test1")->codeName, "test1_2");
}

TEST_F(VarMapTest, GetThrowOnNotExisting) {
  EXPECT_EQ(this->vm_.get("test1"), nullptr);
}

TEST_F(VarMapTest, HasWorks) {
  this->vm_.add("test1", "test1_0", this->tm_.get("int"), false);
  EXPECT_TRUE(this->vm_.has("test1"));
  EXPECT_FALSE(this->vm_.has("test2"));
  this->vm_.save();
  this->vm_.add("test1", "test1_1", this->tm_.get("int"), false);
  EXPECT_TRUE(this->vm_.has("test1"));
  EXPECT_FALSE(this->vm_.has("test2"));
}

TEST_F(VarMapTest, InsertsVar) {
  auto var1 = std::make_shared<Var>(Var{"test1", "test1_0", this->tm_.get("int"), false, false, false, false, 0});
  auto var2 = std::make_shared<Var>(Var{"test2", "test2_0", this->tm_.get("any"), true, false, false, false, 0});
  auto var3 = std::make_shared<Var>(Var{"test3", "test3_0", this->tm_.get("str"), false, true, false, false, 0});
  auto var4 = std::make_shared<Var>(Var{"test4", "test4_0", this->tm_.get("float"), false, false, false, true, 0});

  this->vm_.insert(var1);
  this->vm_.insert(var2);
  this->vm_.insert(var3);
  this->vm_.insert(var4);

  EXPECT_NO_THROW(this->vm_.get("test1"));
  EXPECT_NO_THROW(this->vm_.get("test2"));
  EXPECT_NO_THROW(this->vm_.get("test3"));
  EXPECT_NO_THROW(this->vm_.get("test4"));

  EXPECT_EQ(this->vm_.get("test1")->name, "test1");
  EXPECT_EQ(this->vm_.get("test1")->codeName, "test1_0");
  EXPECT_TRUE(this->tm_.get("int")->matchStrict(this->vm_.get("test1")->type));
  EXPECT_FALSE(this->vm_.get("test1")->mut);
  EXPECT_FALSE(this->vm_.get("test1")->constant);
  EXPECT_FALSE(this->vm_.get("test1")->builtin);
  EXPECT_FALSE(this->vm_.get("test1")->ctxIgnored);
  EXPECT_EQ(this->vm_.get("test1")->frame, 0);

  EXPECT_EQ(this->vm_.get("test2")->name, "test2");
  EXPECT_EQ(this->vm_.get("test2")->codeName, "test2_0");
  EXPECT_TRUE(this->tm_.get("any")->matchStrict(this->vm_.get("test2")->type));
  EXPECT_TRUE(this->vm_.get("test2")->mut);
  EXPECT_FALSE(this->vm_.get("test2")->constant);
  EXPECT_FALSE(this->vm_.get("test2")->builtin);
  EXPECT_FALSE(this->vm_.get("test2")->ctxIgnored);
  EXPECT_EQ(this->vm_.get("test2")->frame, 0);

  EXPECT_EQ(this->vm_.get("test3")->name, "test3");
  EXPECT_EQ(this->vm_.get("test3")->codeName, "test3_0");
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(this->vm_.get("test3")->type));
  EXPECT_FALSE(this->vm_.get("test3")->mut);
  EXPECT_TRUE(this->vm_.get("test3")->constant);
  EXPECT_FALSE(this->vm_.get("test3")->builtin);
  EXPECT_FALSE(this->vm_.get("test3")->ctxIgnored);
  EXPECT_EQ(this->vm_.get("test3")->frame, 0);

  EXPECT_EQ(this->vm_.get("test4")->name, "test4");
  EXPECT_EQ(this->vm_.get("test4")->codeName, "test4_0");
  EXPECT_TRUE(this->tm_.get("float")->matchStrict(this->vm_.get("test4")->type));
  EXPECT_FALSE(this->vm_.get("test4")->mut);
  EXPECT_FALSE(this->vm_.get("test4")->constant);
  EXPECT_FALSE(this->vm_.get("test4")->builtin);
  EXPECT_TRUE(this->vm_.get("test4")->ctxIgnored);
  EXPECT_EQ(this->vm_.get("test4")->frame, 0);

  this->vm_.save();
  auto var5 = std::make_shared<Var>(Var{"test1", "test1_1", this->tm_.get("str"), true, false, false, false, 1});
  this->vm_.insert(var5);

  EXPECT_NO_THROW(this->vm_.get("test1"));

  EXPECT_EQ(this->vm_.get("test1")->name, "test1");
  EXPECT_EQ(this->vm_.get("test1")->codeName, "test1_1");
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(this->vm_.get("test1")->type));
  EXPECT_TRUE(this->vm_.get("test1")->mut);
  EXPECT_FALSE(this->vm_.get("test1")->builtin);
  EXPECT_EQ(this->vm_.get("test1")->frame, 1);
}

TEST_F(VarMapTest, NameGeneratesValid) {
  EXPECT_EQ(this->vm_.name("test"), "test_0");
  this->vm_.add("test", this->vm_.name("test"), this->tm_.get("int"), false);
  EXPECT_EQ(this->vm_.name("test"), "test_1");
  this->vm_.add("test", this->vm_.name("test"), this->tm_.get("int"), false);
  EXPECT_EQ(this->vm_.name("test"), "test_2");
}

TEST_F(VarMapTest, SavesAndRestores) {
  this->vm_.add("test", this->vm_.name("test"), this->tm_.get("int"), false);
  EXPECT_EQ(this->vm_.get("test")->frame, 0);
  this->vm_.save();
  this->vm_.add("test", this->vm_.name("test"), this->tm_.get("int"), false);
  EXPECT_EQ(this->vm_.get("test")->frame, 1);
  this->vm_.save();
  this->vm_.add("test", this->vm_.name("test"), this->tm_.get("int"), false);
  EXPECT_EQ(this->vm_.get("test")->frame, 2);

  this->vm_.restore();
  EXPECT_EQ(this->vm_.get("test")->frame, 1);
  this->vm_.restore();
  EXPECT_EQ(this->vm_.get("test")->frame, 0);
}

TEST_F(VarMapTest, VarEmptyStackGeneratesValid) {
  auto vm = VarMap();
  EXPECT_EQ(vm.varStack().snapshot().size(), 0);
}

TEST_F(VarMapTest, VarStackGeneratesValid) {
  EXPECT_EQ(this->vm_.varStack().snapshot().size(), 0);
  this->vm_.save();
  this->vm_.add("test1", this->vm_.name("test1"), this->tm_.get("int"), false);

  auto varStack1 = this->vm_.varStack();
  varStack1.mark(this->vm_.get("test1"));
  auto s1 = varStack1.snapshot();

  EXPECT_EQ(s1.size(), 1);
  EXPECT_EQ(s1[0]->name, "test1");
  EXPECT_TRUE(this->tm_.get("int")->matchStrict(s1[0]->type));

  this->vm_.save();
  this->vm_.add("test2", this->vm_.name("test2"), this->tm_.get("str"), false);

  auto varStack2 = this->vm_.varStack();
  varStack2.mark(this->vm_.get("test1"));
  varStack2.mark(this->vm_.get("test2"));
  auto s2 = varStack2.snapshot();

  EXPECT_EQ(s2.size(), 2);
  EXPECT_EQ(s2[0]->name, "test2");
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(s2[0]->type));
  EXPECT_EQ(s2[1]->name, "test1");
  EXPECT_TRUE(this->tm_.get("int")->matchStrict(s2[1]->type));

  this->vm_.restore();

  auto varStack3 = this->vm_.varStack();
  varStack3.mark(this->vm_.get("test1"));
  auto s3 = varStack3.snapshot();

  EXPECT_EQ(s3.size(), 1);
  EXPECT_EQ(s3[0]->name, "test1");
  EXPECT_TRUE(this->tm_.get("int")->matchStrict(s3[0]->type));

  this->vm_.restore();
  auto s4 = this->vm_.varStack().snapshot();
  EXPECT_EQ(s4.size(), 0);

  this->vm_.add("test5", this->vm_.name("test5"), this->tm_.get("str"), false, false, true);

  auto varStack5 = this->vm_.varStack();
  varStack5.mark(this->vm_.get("test5"));

  auto s5 = varStack5.snapshot();
  EXPECT_EQ(s5.size(), 0);

  this->vm_.add("test6", this->vm_.name("test6"), this->tm_.get("float"), false, true);

  auto varStack6 = this->vm_.varStack();
  varStack6.mark(this->vm_.get("test6"));

  auto s6 = varStack6.snapshot();
  EXPECT_EQ(s5.size(), 0);
}
