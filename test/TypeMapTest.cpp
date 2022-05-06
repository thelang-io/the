/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include "../src/TypeMap.hpp"
#include "utils.hpp"

class TypeMapTest : public testing::Test {
 protected:
  TypeMap tm_;

  void SetUp () override {
    this->tm_.init();
  }
};

TEST_F(TypeMapTest, AddInsertsFunction) {
  this->tm_.add("Test1", {});

  this->tm_.add("Test2", {
    {"a", this->tm_.get("int")}
  });

  this->tm_.add("Test3", {
    {"b", this->tm_.get("any")},
    {"c", this->tm_.get("str")}
  });

  EXPECT_NO_THROW(this->tm_.get("Test1"));
  EXPECT_NO_THROW(this->tm_.get("Test2"));
  EXPECT_NO_THROW(this->tm_.get("Test3"));
  EXPECT_EQ(this->tm_.get("Test1")->name, "Test1");
  EXPECT_FALSE(this->tm_.get("Test1")->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeObj>(this->tm_.get("Test1")->body));

  auto obj1Body = std::get<TypeObj>(this->tm_.get("Test1")->body);
  auto obj2Body = std::get<TypeObj>(this->tm_.get("Test2")->body);
  auto obj3Body = std::get<TypeObj>(this->tm_.get("Test3")->body);

  EXPECT_EQ(obj1Body.fields.size(), 0);
  EXPECT_EQ(obj2Body.fields.size(), 1);
  EXPECT_EQ(obj3Body.fields.size(), 2);
  EXPECT_EQ(obj2Body.fields[0].name, "a");
  EXPECT_TRUE(this->tm_.get("int")->match(obj2Body.fields[0].type));
  EXPECT_EQ(obj3Body.fields[0].name, "b");
  EXPECT_TRUE(this->tm_.get("any")->match(obj3Body.fields[0].type));
  EXPECT_EQ(obj3Body.fields[1].name, "c");
  EXPECT_TRUE(this->tm_.get("str")->match(obj3Body.fields[1].type));
}

TEST_F(TypeMapTest, AddInsertsObject) {
  this->tm_.add("test1", {}, this->tm_.get("void"));

  this->tm_.add("test2", {
    {"a", this->tm_.get("int"), true, false}
  }, this->tm_.get("void"));

  this->tm_.add("test3", {
    {"b", this->tm_.get("int"), false, false},
    {"c", this->tm_.get("str"), false, true}
  }, this->tm_.get("str"));

  EXPECT_NO_THROW(this->tm_.get("test1"));
  EXPECT_NO_THROW(this->tm_.get("test2"));
  EXPECT_NO_THROW(this->tm_.get("test3"));
  EXPECT_EQ(this->tm_.get("test1")->name, "test1");
  EXPECT_FALSE(this->tm_.get("test1")->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeFn>(this->tm_.get("test1")->body));

  auto fn1Body = std::get<TypeFn>(this->tm_.get("test1")->body);
  auto fn2Body = std::get<TypeFn>(this->tm_.get("test2")->body);
  auto fn3Body = std::get<TypeFn>(this->tm_.get("test3")->body);

  EXPECT_TRUE(this->tm_.get("void")->match(fn1Body.returnType));
  EXPECT_TRUE(this->tm_.get("void")->match(fn2Body.returnType));
  EXPECT_TRUE(this->tm_.get("str")->match(fn3Body.returnType));
  EXPECT_EQ(fn1Body.params.size(), 0);
  EXPECT_EQ(fn2Body.params.size(), 1);
  EXPECT_EQ(fn3Body.params.size(), 2);
  EXPECT_EQ(fn2Body.params[0].name, "a");
  EXPECT_TRUE(this->tm_.get("int")->match(fn2Body.params[0].type));
  EXPECT_TRUE(fn2Body.params[0].required);
  EXPECT_FALSE(fn2Body.params[0].variadic);
  EXPECT_EQ(fn3Body.params[0].name, "b");
  EXPECT_TRUE(this->tm_.get("int")->match(fn3Body.params[0].type));
  EXPECT_FALSE(fn3Body.params[0].required);
  EXPECT_FALSE(fn3Body.params[0].variadic);
  EXPECT_EQ(fn3Body.params[1].name, "c");
  EXPECT_TRUE(this->tm_.get("str")->match(fn3Body.params[1].type));
  EXPECT_FALSE(fn3Body.params[1].required);
  EXPECT_TRUE(fn3Body.params[1].variadic);
}

TEST_F(TypeMapTest, FunctionGenerates) {
  auto fn1 = this->tm_.fn({}, this->tm_.get("void"));

  auto fn2 = this->tm_.fn({
    {"a", this->tm_.get("int"), true, false}
  }, this->tm_.get("void"));

  auto fn3 = this->tm_.fn({
    {"b", this->tm_.get("int"), false, false},
    {"c", this->tm_.get("str"), false, true}
  }, this->tm_.get("str"));

  EXPECT_EQ(fn1->name, "@");
  EXPECT_FALSE(fn1->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeFn>(fn1->body));

  auto fn1Body = std::get<TypeFn>(fn1->body);
  auto fn2Body = std::get<TypeFn>(fn2->body);
  auto fn3Body = std::get<TypeFn>(fn3->body);

  EXPECT_TRUE(this->tm_.get("void")->match(fn1Body.returnType));
  EXPECT_TRUE(this->tm_.get("void")->match(fn2Body.returnType));
  EXPECT_TRUE(this->tm_.get("str")->match(fn3Body.returnType));
  EXPECT_EQ(fn1Body.params.size(), 0);
  EXPECT_EQ(fn2Body.params.size(), 1);
  EXPECT_EQ(fn3Body.params.size(), 2);
  EXPECT_EQ(fn2Body.params[0].name, "a");
  EXPECT_TRUE(this->tm_.get("int")->match(fn2Body.params[0].type));
  EXPECT_TRUE(fn2Body.params[0].required);
  EXPECT_FALSE(fn2Body.params[0].variadic);
  EXPECT_EQ(fn3Body.params[0].name, "b");
  EXPECT_TRUE(this->tm_.get("int")->match(fn3Body.params[0].type));
  EXPECT_FALSE(fn3Body.params[0].required);
  EXPECT_FALSE(fn3Body.params[0].variadic);
  EXPECT_EQ(fn3Body.params[1].name, "c");
  EXPECT_TRUE(this->tm_.get("str")->match(fn3Body.params[1].type));
  EXPECT_FALSE(fn3Body.params[1].required);
  EXPECT_TRUE(fn3Body.params[1].variadic);
}

TEST_F(TypeMapTest, GetReturnsItem) {
  this->tm_.add("Test", {});
  this->tm_.add("test", {}, this->tm_.get("void"));

  EXPECT_NO_THROW(this->tm_.get("Test"));
  EXPECT_NO_THROW(this->tm_.get("test"));
}

TEST_F(TypeMapTest, GetReturnsNull) {
  EXPECT_EQ(this->tm_.get("test"), nullptr);
}

TEST_F(TypeMapTest, NameGeneratesValid) {
  EXPECT_EQ(this->tm_.name("test"), "test_0");
  this->tm_.add("test_0", {});
  EXPECT_EQ(this->tm_.name("test"), "test_1");

  this->tm_.stack.emplace_back("main");

  EXPECT_EQ(this->tm_.name("test"), "mainSDtest_0");
  this->tm_.add("mainSDtest_0", {});
  EXPECT_EQ(this->tm_.name("test"), "mainSDtest_1");

  this->tm_.stack.emplace_back("hello");

  EXPECT_EQ(this->tm_.name("world"), "mainSDhelloSDworld_0");
  this->tm_.add("mainSDhelloSDworld_0", {});
  EXPECT_EQ(this->tm_.name("world"), "mainSDhelloSDworld_1");

  this->tm_.stack.emplace_back("world");

  EXPECT_EQ(this->tm_.name("test"), "mainSDhelloSDworldSDtest_0");
  this->tm_.add("mainSDhelloSDworldSDtest_0", {});
  EXPECT_EQ(this->tm_.name("test"), "mainSDhelloSDworldSDtest_1");
}
