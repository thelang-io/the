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

  EXPECT_NE(this->tm_.get("array_int"), nullptr);
  EXPECT_NE(this->tm_.get("array_str"), nullptr);
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

TEST_F(TypeMapTest, EnumeratorInserts) {
  auto type1 = this->tm_.enumerator("Brown", this->tm_.name("Brown"));
  auto type2 = this->tm_.enumerator("Red", this->tm_.name("Red"));

  EXPECT_NE(this->tm_.get("Brown"), nullptr);
  EXPECT_NE(this->tm_.get("Red"), nullptr);
  EXPECT_FALSE(type1->builtin);
  EXPECT_FALSE(type2->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeEnumerator>(type1->body));
  EXPECT_TRUE(std::holds_alternative<TypeEnumerator>(type2->body));
}

TEST_F(TypeMapTest, EnumeratorDoesNotInsertExact) {
  auto codeName = this->tm_.name("Brown");
  auto type1 = this->tm_.enumerator("Brown", codeName);
  auto type2 = this->tm_.enumerator("Brown", codeName);

  EXPECT_EQ(type1->name, "Brown");
  EXPECT_EQ(type1->name, type2->name);
  EXPECT_EQ(type1, type2);
}

TEST_F(TypeMapTest, EnumerationInserts) {
  this->tm_.stack.emplace_back("Test");
  auto type1 = this->tm_.enumeration("Test", this->tm_.name("Test"), {
    this->tm_.enumerator("Brown", this->tm_.name("Brown"))
  });
  this->tm_.stack.pop_back();

  this->tm_.stack.emplace_back("Test1");
  auto type2 = this->tm_.enumeration("Test1", this->tm_.name("Test1"), {
    this->tm_.enumerator("Red", this->tm_.name("Red")),
    this->tm_.enumerator("Green", this->tm_.name("Green"))
  });
  this->tm_.stack.pop_back();

  EXPECT_NE(this->tm_.get("Test"), nullptr);
  EXPECT_NE(this->tm_.get("Test1"), nullptr);
  EXPECT_FALSE(type1->builtin);
  EXPECT_FALSE(type2->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeEnum>(type1->body));
  EXPECT_TRUE(std::holds_alternative<TypeEnum>(type2->body));

  auto enum1Body = std::get<TypeEnum>(type1->body);
  auto enum2Body = std::get<TypeEnum>(type2->body);

  EXPECT_EQ(enum1Body.members.size(), 1);
  EXPECT_EQ(enum2Body.members.size(), 2);
}

TEST_F(TypeMapTest, EnumerationDoesNotInsertExact) {
  auto codeName = this->tm_.name("Test");

  this->tm_.stack.emplace_back("Test");
  auto type1 = this->tm_.enumeration("Test", codeName, {
    this->tm_.enumerator("Brown", this->tm_.name("Brown"))
  });
  this->tm_.stack.pop_back();

  this->tm_.stack.emplace_back("Test");
  auto type2 = this->tm_.enumeration("Test", codeName, {
    this->tm_.enumerator("Brown", this->tm_.name("Brown"))
  });
  this->tm_.stack.pop_back();

  EXPECT_EQ(type1->name, "Test");
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

  EXPECT_NE(this->tm_.get("fn$0"), nullptr);
  EXPECT_NE(this->tm_.get("fn$1"), nullptr);
  EXPECT_NE(this->tm_.get("fn$2"), nullptr);
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

TEST_F(TypeMapTest, FunctionInsertsMethod) {
  auto obj = this->tm_.obj("Test", "Test_0");

  auto type1MethodInfo = TypeFnMethodInfo{false, "", nullptr, false};
  auto type1 = this->tm_.fn("Test_0.test1_0", {}, this->tm_.get("void"), type1MethodInfo);

  auto type2MethodInfo = TypeFnMethodInfo{true, "self_0", obj, false};

  auto type2 = this->tm_.fn("Test_0.test2_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"), type2MethodInfo);

  auto type3MethodInfo = TypeFnMethodInfo{true, "self2_0", this->tm_.ref(obj), true};

  auto type3 = this->tm_.fn("Test_0.test3_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, false, false},
    TypeFnParam{"b", this->tm_.get("str"), false, false, true}
  }, this->tm_.get("str"), type3MethodInfo);

  EXPECT_NE(this->tm_.get("fn$0"), nullptr);
  EXPECT_NE(this->tm_.get("fn$1"), nullptr);
  EXPECT_NE(this->tm_.get("fn$2"), nullptr);
  EXPECT_FALSE(type1->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeFn>(type1->body));

  auto fn1Body = std::get<TypeFn>(type1->body);
  auto fn2Body = std::get<TypeFn>(type2->body);
  auto fn3Body = std::get<TypeFn>(type3->body);

  EXPECT_TRUE(fn1Body.isMethod);
  EXPECT_FALSE(fn1Body.methodInfo.isSelfFirst);
  EXPECT_EQ(fn1Body.methodInfo.selfCodeName, "");
  EXPECT_EQ(fn1Body.methodInfo.selfType, nullptr);
  EXPECT_FALSE(fn1Body.methodInfo.isSelfMut);
  EXPECT_TRUE(fn2Body.isMethod);
  EXPECT_TRUE(fn2Body.methodInfo.isSelfFirst);
  EXPECT_EQ(fn2Body.methodInfo.selfCodeName, "self_0");
  EXPECT_TRUE(fn2Body.methodInfo.selfType->matchExact(obj));
  EXPECT_FALSE(fn2Body.methodInfo.isSelfMut);
  EXPECT_TRUE(fn3Body.isMethod);
  EXPECT_TRUE(fn3Body.methodInfo.isSelfFirst);
  EXPECT_EQ(fn3Body.methodInfo.selfCodeName, "self2_0");
  EXPECT_TRUE(fn3Body.methodInfo.selfType->matchExact(this->tm_.ref(obj)));
  EXPECT_TRUE(fn3Body.methodInfo.isSelfMut);
}

TEST_F(TypeMapTest, FunctionInsertsBetweenFunctionAndMethod) {
  this->tm_.fn("Test_0.test1_0", {}, this->tm_.get("void"), TypeFnMethodInfo{false, "", nullptr, false});
  this->tm_.fn("Test_0.test1_0", {}, this->tm_.get("void"));

  EXPECT_NE(this->tm_.get("fn$0"), nullptr);
  EXPECT_NE(this->tm_.get("fn$1"), nullptr);
}

TEST_F(TypeMapTest, FunctionDoesNotInsert) {
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

TEST_F(TypeMapTest, FunctionDoesNotInsertMethod) {
  auto typeMethodInfo = TypeFnMethodInfo{false, "", nullptr, false};

  auto type1 = this->tm_.fn("test1_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"), typeMethodInfo);

  auto type2 = this->tm_.fn("test2_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"), typeMethodInfo);

  auto type3 = this->tm_.fn(std::nullopt, {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"), typeMethodInfo);

  auto type4 = this->tm_.fn(std::nullopt, {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"), typeMethodInfo);

  EXPECT_EQ(type1->name, "fn$0");
  EXPECT_EQ(type1->name, type2->name);
  EXPECT_EQ(type3->name, "fn$0");
  EXPECT_EQ(type3, type4);
}

TEST_F(TypeMapTest, GetReturnsItem) {
  this->tm_.fn("test", {}, this->tm_.get("void"));
  this->tm_.obj("Test", "Test");

  EXPECT_NE(this->tm_.get("fn$0"), nullptr);
  EXPECT_NE(this->tm_.get("Test"), nullptr);
}

TEST_F(TypeMapTest, GetReturnsSelf) {
  EXPECT_EQ(this->tm_.get("Self"), nullptr);
  this->tm_.self = this->tm_.obj("Test", "Test");
  EXPECT_EQ(this->tm_.get("Self"), this->tm_.self);
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

TEST_F(TypeMapTest, HasSelf) {
  EXPECT_FALSE(this->tm_.has("Self"));
  this->tm_.self = this->tm_.obj("Test", "Test");
  EXPECT_TRUE(this->tm_.has("Self"));
}

TEST_F(TypeMapTest, HasNotExisting) {
  EXPECT_FALSE(this->tm_.has("test"));
}

TEST_F(TypeMapTest, IsSelf) {
  auto type = this->tm_.obj("Test", "Test");

  EXPECT_FALSE(this->tm_.isSelf(this->tm_.get("int")));
  this->tm_.self = type;
  EXPECT_TRUE(this->tm_.isSelf(type));
  EXPECT_TRUE(this->tm_.isSelf(this->tm_.ref(type)));
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
    TypeField{"a", this->tm_.get("int"), false, false, false}
  });

  auto type3 = this->tm_.obj("Test3", "Test3_0", {
    TypeField{"b", this->tm_.get("any"), false, false, false},
    TypeField{"c", this->tm_.get("str"), false, false, false}
  });

  EXPECT_NE(this->tm_.get("Test1"), nullptr);
  EXPECT_NE(this->tm_.get("Test2"), nullptr);
  EXPECT_NE(this->tm_.get("Test3"), nullptr);
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

TEST_F(TypeMapTest, OptionalInserts) {
  auto type1 = this->tm_.opt(this->tm_.get("int"));
  auto type2 = this->tm_.opt(this->tm_.get("str"));

  EXPECT_NE(this->tm_.get("opt_int"), nullptr);
  EXPECT_NE(this->tm_.get("opt_str"), nullptr);
  EXPECT_FALSE(type1->builtin);
  EXPECT_FALSE(type2->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeOptional>(type1->body));
  EXPECT_TRUE(std::holds_alternative<TypeOptional>(type2->body));
  EXPECT_TRUE(this->tm_.get("int")->matchExact(std::get<TypeOptional>(type1->body).type));
  EXPECT_TRUE(this->tm_.get("str")->matchExact(std::get<TypeOptional>(type2->body).type));
}

TEST_F(TypeMapTest, OptionalDoesNotInsertExact) {
  auto type1 = this->tm_.opt(this->tm_.get("int"));
  auto type2 = this->tm_.opt(this->tm_.get("int"));

  EXPECT_EQ(type1->name, "opt_int");
  EXPECT_EQ(type1->name, type2->name);
  EXPECT_EQ(type1, type2);
}

TEST_F(TypeMapTest, ReferenceInserts) {
  auto type1 = this->tm_.ref(this->tm_.get("int"));
  auto type2 = this->tm_.ref(this->tm_.get("str"));

  EXPECT_FALSE(type1->builtin);
  EXPECT_FALSE(type2->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeRef>(type1->body));
  EXPECT_TRUE(std::holds_alternative<TypeRef>(type2->body));
  EXPECT_TRUE(this->tm_.get("int")->matchExact(std::get<TypeRef>(type1->body).refType));
  EXPECT_TRUE(this->tm_.get("str")->matchExact(std::get<TypeRef>(type2->body).refType));
}

TEST_F(TypeMapTest, ReferenceDoesNotInsertExact) {
  auto type1 = this->tm_.ref(this->tm_.get("int"));
  auto type2 = this->tm_.ref(this->tm_.get("int"));

  EXPECT_EQ(type1->name, "ref_int");
  EXPECT_EQ(type1->name, type2->name);
  EXPECT_EQ(type1, type2);
}
