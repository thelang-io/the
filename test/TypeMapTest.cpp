/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include "../src/TypeMap.hpp"

class TypeMapTest : public testing::Test {
 protected:
  TypeMap tm_;

  void SetUp () override {
    this->tm_.init();
  }
};

TEST_F(TypeMapTest, ArrayInserts) {
  auto type1 = this->tm_.arrayOf(this->tm_.get("int"));
  auto type2 = this->tm_.arrayOf(this->tm_.get("str"));

  EXPECT_NO_THROW(this->tm_.get("array_int"));
  EXPECT_NO_THROW(this->tm_.get("array_str"));
  EXPECT_FALSE(type1->builtin);
  EXPECT_FALSE(type2->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeArray>(type1->body));
  EXPECT_TRUE(std::holds_alternative<TypeArray>(type2->body));

  auto arr1Body = std::get<TypeArray>(type1->body);
  auto arr2Body = std::get<TypeArray>(type2->body);

  EXPECT_TRUE(this->tm_.get("int")->matchExact(arr1Body.elementType));
  EXPECT_TRUE(this->tm_.get("str")->matchExact(arr2Body.elementType));
}

TEST_F(TypeMapTest, ArrayDoesNotInsertExact) {
  auto type1 = this->tm_.arrayOf(this->tm_.get("int"));
  auto type2 = this->tm_.arrayOf(this->tm_.get("int"));

  EXPECT_EQ(type1->name, "array_int");
  EXPECT_EQ(type1->name, type2->name);
  EXPECT_EQ(type1, type2);
}

TEST_F(TypeMapTest, FunctionInserts) {
  auto type1 = this->tm_.fn("test1_0", {}, this->tm_.get("void"));

  auto type2 = this->tm_.fn("test2_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"));

  auto type3 = this->tm_.fn("test3_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, false, false},
    TypeFnParam{"b", this->tm_.get("str"), false, false, true}
  }, this->tm_.get("str"));

  EXPECT_NO_THROW(this->tm_.get("fn$0"));
  EXPECT_NO_THROW(this->tm_.get("fn$1"));
  EXPECT_NO_THROW(this->tm_.get("fn$2"));
  EXPECT_FALSE(type1->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeFn>(type1->body));

  auto fn1Body = std::get<TypeFn>(type1->body);
  auto fn2Body = std::get<TypeFn>(type2->body);
  auto fn3Body = std::get<TypeFn>(type3->body);

  EXPECT_TRUE(this->tm_.get("void")->match(fn1Body.returnType));
  EXPECT_TRUE(this->tm_.get("void")->match(fn2Body.returnType));
  EXPECT_TRUE(this->tm_.get("str")->match(fn3Body.returnType));
  EXPECT_EQ(fn1Body.params.size(), 0);
  EXPECT_EQ(fn2Body.params.size(), 1);
  EXPECT_EQ(fn3Body.params.size(), 2);
  EXPECT_TRUE(this->tm_.get("int")->match(fn2Body.params[0].type));
  EXPECT_FALSE(fn2Body.params[0].mut);
  EXPECT_TRUE(fn2Body.params[0].required);
  EXPECT_FALSE(fn2Body.params[0].variadic);
  EXPECT_TRUE(this->tm_.get("int")->match(fn3Body.params[0].type));
  EXPECT_FALSE(fn3Body.params[0].mut);
  EXPECT_FALSE(fn3Body.params[0].required);
  EXPECT_FALSE(fn3Body.params[0].variadic);
  EXPECT_TRUE(this->tm_.get("str")->match(fn3Body.params[1].type));
  EXPECT_FALSE(fn3Body.params[1].mut);
  EXPECT_FALSE(fn3Body.params[1].required);
  EXPECT_TRUE(fn3Body.params[1].variadic);
}

TEST_F(TypeMapTest, FunctionInsertsSimilar) {
  auto type1 = this->tm_.fn("test1_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"));

  auto type2 = this->tm_.fn("test2_0", {
    TypeFnParam{"b", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"));

  auto type3 = this->tm_.fn("test3_0", {
    TypeFnParam{"a", this->tm_.get("str"), true, true, false}
  }, this->tm_.get("int"));

  auto type4 = this->tm_.fn("test4_0", {
    TypeFnParam{"a", this->tm_.get("str"), false, true, false}
  }, this->tm_.get("i8"));

  EXPECT_EQ(type1->name, "fn$0");
  EXPECT_EQ(type1->name, type2->name);
  EXPECT_EQ(type3->name, "fn$1");
  EXPECT_EQ(type3->name, type4->name);
}

TEST_F(TypeMapTest, FunctionDoesNotInsertExact) {
  auto type1 = this->tm_.fn("test1_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"));

  auto type2 = this->tm_.fn("test2_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"));

  auto type3 = this->tm_.fn(std::nullopt, {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"));

  auto type4 = this->tm_.fn(std::nullopt, {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"));

  EXPECT_EQ(type1->name, "fn$0");
  EXPECT_EQ(type1->name, type2->name);
  EXPECT_EQ(type3->name, "fn$0");
  EXPECT_EQ(type3, type4);
}

TEST_F(TypeMapTest, GetReturnsItem) {
  this->tm_.fn("test", {}, this->tm_.get("void"));
  this->tm_.obj("Test", "Test");

  EXPECT_NO_THROW(this->tm_.get("fn$0"));
  EXPECT_NO_THROW(this->tm_.get("Test"));
}

TEST_F(TypeMapTest, GetReturnsNull) {
  EXPECT_EQ(this->tm_.get("test"), nullptr);
}

TEST_F(TypeMapTest, HasExisting) {
  this->tm_.fn("test", {}, this->tm_.get("void"));
  this->tm_.obj("Test", "Test");

  EXPECT_TRUE(this->tm_.has("fn$0"));
  EXPECT_TRUE(this->tm_.has("Test"));
}

TEST_F(TypeMapTest, HasNotExisting) {
  EXPECT_FALSE(this->tm_.has("test"));
}

TEST_F(TypeMapTest, NameGeneratesValid) {
  EXPECT_EQ(this->tm_.name("test"), "test_0");
  this->tm_.obj("test", "test_0");
  EXPECT_EQ(this->tm_.name("test"), "test_1");

  this->tm_.stack.emplace_back("main");

  EXPECT_EQ(this->tm_.name("test"), "mainSDtest_0");
  this->tm_.obj("test", "mainSDtest_0");
  EXPECT_EQ(this->tm_.name("test"), "mainSDtest_1");

  this->tm_.stack.emplace_back("hello");

  EXPECT_EQ(this->tm_.name("world"), "mainSDhelloSDworld_0");
  this->tm_.obj("world", "mainSDhelloSDworld_0");
  EXPECT_EQ(this->tm_.name("world"), "mainSDhelloSDworld_1");

  this->tm_.stack.emplace_back("world");

  EXPECT_EQ(this->tm_.name("test"), "mainSDhelloSDworldSDtest_0");
  this->tm_.obj("test", "mainSDhelloSDworldSDtest_0");
  EXPECT_EQ(this->tm_.name("test"), "mainSDhelloSDworldSDtest_1");
}

TEST_F(TypeMapTest, ObjectInserts) {
  auto type1 = this->tm_.obj("Test1", "Test1_0");

  auto type2 = this->tm_.obj("Test2", "Test2_0", {
    TypeField{"a", this->tm_.get("int")}
  });

  auto type3 = this->tm_.obj("Test3", "Test3_0", {
    TypeField{"b", this->tm_.get("any")},
    TypeField{"c", this->tm_.get("str")}
  });

  EXPECT_NO_THROW(this->tm_.get("Test1"));
  EXPECT_NO_THROW(this->tm_.get("Test2"));
  EXPECT_NO_THROW(this->tm_.get("Test3"));
  EXPECT_EQ(type1->name, "Test1");
  EXPECT_EQ(type2->name, "Test2");
  EXPECT_EQ(type3->name, "Test3");
  EXPECT_FALSE(type1->builtin);
  EXPECT_FALSE(type2->builtin);
  EXPECT_FALSE(type3->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeObj>(type1->body));
  EXPECT_TRUE(std::holds_alternative<TypeObj>(type2->body));
  EXPECT_TRUE(std::holds_alternative<TypeObj>(type3->body));
  EXPECT_EQ(type1->fields.size(), 1);
  EXPECT_EQ(type2->fields.size(), 2);
  EXPECT_EQ(type3->fields.size(), 3);
  EXPECT_EQ(type2->fields[0].name, "a");
  EXPECT_TRUE(this->tm_.get("int")->matchExact(type2->fields[0].type));
  EXPECT_EQ(type3->fields[0].name, "b");
  EXPECT_TRUE(this->tm_.get("any")->matchExact(type3->fields[0].type));
  EXPECT_EQ(type3->fields[1].name, "c");
  EXPECT_TRUE(this->tm_.get("str")->matchExact(type3->fields[1].type));
}

TEST_F(TypeMapTest, ReferenceInserts) {
  auto type1 = this->tm_.ref(this->tm_.get("int"));
  auto type2 = this->tm_.ref(this->tm_.get("int"));

  EXPECT_FALSE(type1->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeRef>(type1->body));

  auto ref1Body = std::get<TypeRef>(type1->body);

  EXPECT_TRUE(this->tm_.get("int")->matchExact(ref1Body.refType));
  EXPECT_EQ(type1, type2);
}
