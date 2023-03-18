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

TEST_F(TypeMapTest, AliasInserts) {
  auto type1 = this->tm_.createAlias("Test1", this->tm_.get("int"));
  auto type2 = this->tm_.createAlias("Test2", this->tm_.get("str"));

  EXPECT_NE(this->tm_.get("Test1"), nullptr);
  EXPECT_NE(this->tm_.get("Test2"), nullptr);
  EXPECT_FALSE(type1->builtin);
  EXPECT_FALSE(type2->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeAlias>(type1->body));
  EXPECT_TRUE(std::holds_alternative<TypeAlias>(type2->body));

  auto alias1Body = std::get<TypeAlias>(type1->body);
  auto alias2Body = std::get<TypeAlias>(type2->body);

  EXPECT_TRUE(this->tm_.get("int")->matchStrict(alias1Body.type));
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(alias2Body.type));
}

TEST_F(TypeMapTest, ArrayInserts) {
  auto type1 = this->tm_.createArr(this->tm_.get("int"));
  auto type2 = this->tm_.createArr(this->tm_.get("str"));

  EXPECT_NE(this->tm_.get("array_int"), nullptr);
  EXPECT_NE(this->tm_.get("array_str"), nullptr);
  EXPECT_FALSE(type1->builtin);
  EXPECT_FALSE(type2->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeArray>(type1->body));
  EXPECT_TRUE(std::holds_alternative<TypeArray>(type2->body));

  auto arr1Body = std::get<TypeArray>(type1->body);
  auto arr2Body = std::get<TypeArray>(type2->body);

  EXPECT_TRUE(this->tm_.get("int")->matchStrict(arr1Body.elementType));
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(arr2Body.elementType));
}

TEST_F(TypeMapTest, ArrayInsertsAlias) {
  auto type1 = this->tm_.createArr(this->tm_.createAlias("Alias1", this->tm_.get("int")));
  auto type2 = this->tm_.createArr(this->tm_.createAlias("Alias2", this->tm_.get("str")));

  EXPECT_NE(this->tm_.get("array_int"), nullptr);
  EXPECT_NE(this->tm_.get("array_str"), nullptr);
  EXPECT_FALSE(type1->builtin);
  EXPECT_FALSE(type2->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeArray>(type1->body));
  EXPECT_TRUE(std::holds_alternative<TypeArray>(type2->body));

  auto arr1Body = std::get<TypeArray>(type1->body);
  auto arr2Body = std::get<TypeArray>(type2->body);

  EXPECT_TRUE(this->tm_.get("int")->matchStrict(arr1Body.elementType));
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(arr2Body.elementType));
}

TEST_F(TypeMapTest, ArrayDoesNotInsertExact) {
  auto type1 = this->tm_.createArr(this->tm_.get("int"));
  auto type2 = this->tm_.createArr(this->tm_.get("int"));

  EXPECT_EQ(type1->name, "array_int");
  EXPECT_EQ(type1->name, type2->name);
  EXPECT_EQ(type1, type2);
}

TEST_F(TypeMapTest, EnumeratorInserts) {
  auto type1 = this->tm_.createEnumerator("Brown", this->tm_.name("Brown"));
  auto type2 = this->tm_.createEnumerator("Red", this->tm_.name("Red"));

  EXPECT_NE(this->tm_.get("Brown"), nullptr);
  EXPECT_NE(this->tm_.get("Red"), nullptr);
  EXPECT_FALSE(type1->builtin);
  EXPECT_FALSE(type2->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeEnumerator>(type1->body));
  EXPECT_TRUE(std::holds_alternative<TypeEnumerator>(type2->body));
}

TEST_F(TypeMapTest, EnumeratorDoesNotInsertExact) {
  auto codeName = this->tm_.name("Brown");
  auto type1 = this->tm_.createEnumerator("Brown", codeName);
  auto type2 = this->tm_.createEnumerator("Brown", codeName);

  EXPECT_EQ(type1->name, "Brown");
  EXPECT_EQ(type1->name, type2->name);
  EXPECT_EQ(type1, type2);
}

TEST_F(TypeMapTest, EnumerationInserts) {
  this->tm_.stack.emplace_back("Test");
  auto type1 = this->tm_.createEnum("Test", this->tm_.name("Test"), {
    this->tm_.createEnumerator("Brown", this->tm_.name("Brown"))
  });
  this->tm_.stack.pop_back();

  this->tm_.stack.emplace_back("Test1");
  auto type2 = this->tm_.createEnum("Test1", this->tm_.name("Test1"), {
    this->tm_.createEnumerator("Red", this->tm_.name("Red")),
    this->tm_.createEnumerator("Green", this->tm_.name("Green"))
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
  auto type1 = this->tm_.createEnum("Test", codeName, {
    this->tm_.createEnumerator("Brown", this->tm_.name("Brown"))
  });
  this->tm_.stack.pop_back();

  this->tm_.stack.emplace_back("Test");
  auto type2 = this->tm_.createEnum("Test", codeName, {
    this->tm_.createEnumerator("Brown", this->tm_.name("Brown"))
  });
  this->tm_.stack.pop_back();

  EXPECT_EQ(type1->name, "Test");
  EXPECT_EQ(type1->name, type2->name);
  EXPECT_EQ(type1, type2);
}

TEST_F(TypeMapTest, FunctionInserts) {
  auto type1 = this->tm_.createFn({}, this->tm_.get("void"));

  auto type2 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"));

  auto type3 = this->tm_.createFn({
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

  EXPECT_TRUE(this->tm_.get("void")->matchStrict(fn1Body.returnType));
  EXPECT_TRUE(this->tm_.get("void")->matchStrict(fn2Body.returnType));
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(fn3Body.returnType));
  EXPECT_EQ(fn1Body.params.size(), 0);
  EXPECT_EQ(fn2Body.params.size(), 1);
  EXPECT_EQ(fn3Body.params.size(), 2);
  EXPECT_TRUE(this->tm_.get("int")->matchStrict(fn2Body.params[0].type));
  EXPECT_FALSE(fn2Body.params[0].mut);
  EXPECT_TRUE(fn2Body.params[0].required);
  EXPECT_FALSE(fn2Body.params[0].variadic);
  EXPECT_TRUE(this->tm_.get("int")->matchStrict(fn3Body.params[0].type));
  EXPECT_FALSE(fn3Body.params[0].mut);
  EXPECT_FALSE(fn3Body.params[0].required);
  EXPECT_FALSE(fn3Body.params[0].variadic);
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(fn3Body.params[1].type));
  EXPECT_FALSE(fn3Body.params[1].mut);
  EXPECT_FALSE(fn3Body.params[1].required);
  EXPECT_TRUE(fn3Body.params[1].variadic);
}

TEST_F(TypeMapTest, FunctionInsertsBetweenFunctionAndMethod) {
  this->tm_.createMethod({}, this->tm_.get("void"), TypeCallInfo{"TestSDa_0", false, "", nullptr, false});
  this->tm_.createFn({}, this->tm_.get("void"));

  EXPECT_NE(this->tm_.get("fn$0"), nullptr);
  EXPECT_NE(this->tm_.get("fn$1"), nullptr);
}

TEST_F(TypeMapTest, FunctionDoesNotInsert) {
  auto type1 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"));

  auto type2 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"));

  auto type3 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"));

  auto type4 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"));

  EXPECT_EQ(type1->name, "fn$1000");
  EXPECT_EQ(type1->name, type2->name);
  EXPECT_EQ(type3->name, "fn$1000");
  EXPECT_EQ(type3, type4);
}

TEST_F(TypeMapTest, FunctionDoesNotInsertSimilar) {
  auto type1 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"));

  auto type2 = this->tm_.createFn({
    TypeFnParam{"b", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"));

  EXPECT_EQ(type1->name, type2->name);
  EXPECT_EQ(type1->codeName, type2->codeName);
}

TEST_F(TypeMapTest, MethodInserts) {
  auto obj = this->tm_.createObj("Test", "Test_0");

  auto type1CallInfo = TypeCallInfo{"TestSDa_0", false, "", nullptr, false};
  auto type1 = this->tm_.createMethod({}, this->tm_.get("void"), type1CallInfo);

  auto type2CallInfo = TypeCallInfo{"TestSDb_0", true, "self_0", obj, false};

  auto type2 = this->tm_.createMethod({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"), type2CallInfo);

  auto type3CallInfo = TypeCallInfo{"TestSDc_0", true, "self2_0", this->tm_.createRef(obj), true};

  auto type3 = this->tm_.createMethod({
    TypeFnParam{"a", this->tm_.get("int"), false, false, false},
    TypeFnParam{"b", this->tm_.get("str"), false, false, true}
  }, this->tm_.get("str"), type3CallInfo);

  EXPECT_NE(this->tm_.get("fn$0"), nullptr);
  EXPECT_NE(this->tm_.get("fn$1"), nullptr);
  EXPECT_NE(this->tm_.get("fn$2"), nullptr);
  EXPECT_FALSE(type1->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeFn>(type1->body));

  auto fn1Body = std::get<TypeFn>(type1->body);
  auto fn2Body = std::get<TypeFn>(type2->body);
  auto fn3Body = std::get<TypeFn>(type3->body);

  EXPECT_TRUE(fn1Body.isMethod);
  EXPECT_FALSE(fn1Body.callInfo.isSelfFirst);
  EXPECT_EQ(fn1Body.callInfo.selfCodeName, "");
  EXPECT_EQ(fn1Body.callInfo.selfType, nullptr);
  EXPECT_FALSE(fn1Body.callInfo.isSelfMut);
  EXPECT_TRUE(fn2Body.isMethod);
  EXPECT_TRUE(fn2Body.callInfo.isSelfFirst);
  EXPECT_EQ(fn2Body.callInfo.selfCodeName, "self_0");
  EXPECT_TRUE(fn2Body.callInfo.selfType->matchStrict(obj));
  EXPECT_FALSE(fn2Body.callInfo.isSelfMut);
  EXPECT_TRUE(fn3Body.isMethod);
  EXPECT_TRUE(fn3Body.callInfo.isSelfFirst);
  EXPECT_EQ(fn3Body.callInfo.selfCodeName, "self2_0");
  EXPECT_TRUE(fn3Body.callInfo.selfType->matchStrict(this->tm_.createRef(obj)));
  EXPECT_TRUE(fn3Body.callInfo.isSelfMut);
}

TEST_F(TypeMapTest, MethodDoesNotInsert) {
  auto typeCallInfo = TypeCallInfo{"TestSDa_0", false, "", nullptr, false};

  auto type1 = this->tm_.createMethod({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"), typeCallInfo);

  auto type2 = this->tm_.createMethod({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"), typeCallInfo);

  auto type3 = this->tm_.createMethod({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"), typeCallInfo);

  auto type4 = this->tm_.createMethod({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"), typeCallInfo);

  EXPECT_EQ(type1->name, "fn$1000");
  EXPECT_EQ(type1->name, type2->name);
  EXPECT_EQ(type3->name, "fn$1000");
  EXPECT_EQ(type3, type4);
}

TEST_F(TypeMapTest, MethodDoesNotInsertSimilar) {
  auto type1 = this->tm_.createMethod({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"), TypeCallInfo{"TestSDa_0", false, "", nullptr, false});

  auto type2 = this->tm_.createMethod({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"), TypeCallInfo{"TestSDb_0", false, "", nullptr, false});

  EXPECT_EQ(type1->name, type2->name);
  EXPECT_EQ(type1->codeName, type2->codeName);
}

TEST_F(TypeMapTest, GetReturnsItem) {
  this->tm_.createFn({}, this->tm_.get("void"));
  this->tm_.createObj("Test", "Test");

  EXPECT_NE(this->tm_.get("fn$0"), nullptr);
  EXPECT_NE(this->tm_.get("Test"), nullptr);
}

TEST_F(TypeMapTest, GetReturnsSelf) {
  EXPECT_EQ(this->tm_.get("Self"), nullptr);
  this->tm_.self = this->tm_.createObj("Test", "Test");
  EXPECT_EQ(this->tm_.get("Self"), this->tm_.self);
}

TEST_F(TypeMapTest, GetReturnsNull) {
  EXPECT_EQ(this->tm_.get("test"), nullptr);
}

TEST_F(TypeMapTest, HasExisting) {
  this->tm_.createFn({}, this->tm_.get("void"));
  this->tm_.createObj("Test", "Test");

  EXPECT_TRUE(this->tm_.has("fn$0"));
  EXPECT_TRUE(this->tm_.has("Test"));
}

TEST_F(TypeMapTest, HasSelf) {
  EXPECT_FALSE(this->tm_.has("Self"));
  this->tm_.self = this->tm_.createObj("Test", "Test");
  EXPECT_TRUE(this->tm_.has("Self"));
}

TEST_F(TypeMapTest, HasNotExisting) {
  EXPECT_FALSE(this->tm_.has("test"));
}

TEST_F(TypeMapTest, IsSelf) {
  auto type = this->tm_.createObj("Test", "Test");

  EXPECT_FALSE(this->tm_.isSelf(this->tm_.get("int")));
  this->tm_.self = type;
  EXPECT_TRUE(this->tm_.isSelf(type));
  EXPECT_TRUE(this->tm_.isSelf(this->tm_.createRef(type)));
}

TEST_F(TypeMapTest, NameGeneratesValid) {
  EXPECT_EQ(this->tm_.name("test"), "test_0");
  this->tm_.createObj("test", "test_0");
  EXPECT_EQ(this->tm_.name("test"), "test_1");

  this->tm_.stack.emplace_back("main");

  EXPECT_EQ(this->tm_.name("test"), "mainSDtest_0");
  this->tm_.createObj("test", "mainSDtest_0");
  EXPECT_EQ(this->tm_.name("test"), "mainSDtest_1");

  this->tm_.stack.emplace_back("hello");

  EXPECT_EQ(this->tm_.name("world"), "mainSDhelloSDworld_0");
  this->tm_.createObj("world", "mainSDhelloSDworld_0");
  EXPECT_EQ(this->tm_.name("world"), "mainSDhelloSDworld_1");

  this->tm_.stack.emplace_back("world");

  EXPECT_EQ(this->tm_.name("test"), "mainSDhelloSDworldSDtest_0");
  this->tm_.createObj("test", "mainSDhelloSDworldSDtest_0");
  EXPECT_EQ(this->tm_.name("test"), "mainSDhelloSDworldSDtest_1");
}

TEST_F(TypeMapTest, MapInserts) {
  auto type1 = this->tm_.createMap(this->tm_.get("int"), this->tm_.get("int"));
  auto type2 = this->tm_.createMap(this->tm_.get("str"), this->tm_.get("str"));

  EXPECT_NE(this->tm_.get("map_intMSintME"), nullptr);
  EXPECT_NE(this->tm_.get("map_strMSstrME"), nullptr);
  EXPECT_FALSE(type1->builtin);
  EXPECT_FALSE(type2->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeBodyMap>(type1->body));
  EXPECT_TRUE(std::holds_alternative<TypeBodyMap>(type2->body));
  EXPECT_TRUE(this->tm_.get("int")->matchStrict(std::get<TypeBodyMap>(type1->body).keyType));
  EXPECT_TRUE(this->tm_.get("int")->matchStrict(std::get<TypeBodyMap>(type1->body).valueType));
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(std::get<TypeBodyMap>(type2->body).keyType));
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(std::get<TypeBodyMap>(type2->body).valueType));
}

TEST_F(TypeMapTest, MapInsertsAlias) {
  auto alias1 = this->tm_.createAlias("Alias1", this->tm_.get("int"));
  auto alias2 = this->tm_.createAlias("Alias2", this->tm_.get("str"));
  auto type1 = this->tm_.createMap(alias1, alias1);
  auto type2 = this->tm_.createMap(alias2, alias2);

  EXPECT_NE(this->tm_.get("map_intMSintME"), nullptr);
  EXPECT_NE(this->tm_.get("map_strMSstrME"), nullptr);
  EXPECT_FALSE(type1->builtin);
  EXPECT_FALSE(type2->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeBodyMap>(type1->body));
  EXPECT_TRUE(std::holds_alternative<TypeBodyMap>(type2->body));
  EXPECT_TRUE(this->tm_.get("int")->matchStrict(std::get<TypeBodyMap>(type1->body).keyType));
  EXPECT_TRUE(this->tm_.get("int")->matchStrict(std::get<TypeBodyMap>(type1->body).valueType));
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(std::get<TypeBodyMap>(type2->body).keyType));
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(std::get<TypeBodyMap>(type2->body).valueType));
}

TEST_F(TypeMapTest, MapDoesNotInsertExact) {
  auto type1 = this->tm_.createMap(this->tm_.get("int"), this->tm_.get("int"));
  auto type2 = this->tm_.createMap(this->tm_.get("int"), this->tm_.get("int"));

  EXPECT_EQ(type1->name, "map_intMSintME");
  EXPECT_EQ(type1->name, type2->name);
  EXPECT_EQ(type1, type2);
}

TEST_F(TypeMapTest, ObjectInserts) {
  auto type1 = this->tm_.createObj("Test1", "Test1_0");

  auto type2 = this->tm_.createObj("Test2", "Test2_0", {
    TypeField{"a", this->tm_.get("int"), false, false}
  });

  auto type3 = this->tm_.createObj("Test3", "Test3_0", {
    TypeField{"b", this->tm_.get("any"), false, false},
    TypeField{"c", this->tm_.get("str"), false, false}
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
  EXPECT_TRUE(this->tm_.get("int")->matchStrict(type2->fields[0].type));
  EXPECT_EQ(type3->fields[0].name, "b");
  EXPECT_TRUE(this->tm_.get("any")->matchStrict(type3->fields[0].type));
  EXPECT_EQ(type3->fields[1].name, "c");
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(type3->fields[1].type));
}

TEST_F(TypeMapTest, OptionalInserts) {
  auto type1 = this->tm_.createOpt(this->tm_.get("int"));
  auto type2 = this->tm_.createOpt(this->tm_.get("str"));

  EXPECT_NE(this->tm_.get("opt_int"), nullptr);
  EXPECT_NE(this->tm_.get("opt_str"), nullptr);
  EXPECT_FALSE(type1->builtin);
  EXPECT_FALSE(type2->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeOptional>(type1->body));
  EXPECT_TRUE(std::holds_alternative<TypeOptional>(type2->body));
  EXPECT_TRUE(this->tm_.get("int")->matchStrict(std::get<TypeOptional>(type1->body).type));
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(std::get<TypeOptional>(type2->body).type));
}

TEST_F(TypeMapTest, OptionalInsertsAlias) {
  auto type1 = this->tm_.createOpt(this->tm_.createAlias("Alias1", this->tm_.get("int")));
  auto type2 = this->tm_.createOpt(this->tm_.createAlias("Alias2", this->tm_.get("str")));

  EXPECT_NE(this->tm_.get("opt_int"), nullptr);
  EXPECT_NE(this->tm_.get("opt_str"), nullptr);
  EXPECT_FALSE(type1->builtin);
  EXPECT_FALSE(type2->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeOptional>(type1->body));
  EXPECT_TRUE(std::holds_alternative<TypeOptional>(type2->body));
  EXPECT_TRUE(this->tm_.get("int")->matchStrict(std::get<TypeOptional>(type1->body).type));
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(std::get<TypeOptional>(type2->body).type));
}

TEST_F(TypeMapTest, OptionalDoesNotInsertExact) {
  auto type1 = this->tm_.createOpt(this->tm_.get("int"));
  auto type2 = this->tm_.createOpt(this->tm_.get("int"));

  EXPECT_EQ(type1->name, "opt_int");
  EXPECT_EQ(type1->name, type2->name);
  EXPECT_EQ(type1, type2);
}

TEST_F(TypeMapTest, ReferenceInserts) {
  auto type1 = this->tm_.createRef(this->tm_.get("int"));
  auto type2 = this->tm_.createRef(this->tm_.get("str"));

  EXPECT_FALSE(type1->builtin);
  EXPECT_FALSE(type2->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeRef>(type1->body));
  EXPECT_TRUE(std::holds_alternative<TypeRef>(type2->body));
  EXPECT_TRUE(this->tm_.get("int")->matchStrict(std::get<TypeRef>(type1->body).refType));
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(std::get<TypeRef>(type2->body).refType));
}

TEST_F(TypeMapTest, ReferenceInsertsAlias) {
  auto type1 = this->tm_.createRef(this->tm_.createAlias("Alias1", this->tm_.get("int")));
  auto type2 = this->tm_.createRef(this->tm_.createAlias("Alias2", this->tm_.get("str")));

  EXPECT_FALSE(type1->builtin);
  EXPECT_FALSE(type2->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeRef>(type1->body));
  EXPECT_TRUE(std::holds_alternative<TypeRef>(type2->body));
  EXPECT_TRUE(this->tm_.get("int")->matchStrict(std::get<TypeRef>(type1->body).refType));
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(std::get<TypeRef>(type2->body).refType));
}

TEST_F(TypeMapTest, ReferenceDoesNotInsertExact) {
  auto type1 = this->tm_.createRef(this->tm_.get("int"));
  auto type2 = this->tm_.createRef(this->tm_.get("int"));

  EXPECT_EQ(type1->name, "ref_int");
  EXPECT_EQ(type1->name, type2->name);
  EXPECT_EQ(type1, type2);
}

TEST_F(TypeMapTest, UnionInserts) {
  auto type1 = this->tm_.createUnion({this->tm_.get("float"), this->tm_.get("int")});
  auto type2 = this->tm_.createUnion({this->tm_.get("char"), this->tm_.get("str")});

  EXPECT_FALSE(type1->builtin);
  EXPECT_FALSE(type2->builtin);
  EXPECT_TRUE(std::holds_alternative<TypeUnion>(type1->body));
  EXPECT_TRUE(std::holds_alternative<TypeUnion>(type2->body));

  auto union1Body = std::get<TypeUnion>(type1->body);
  auto union2Body = std::get<TypeUnion>(type2->body);

  EXPECT_TRUE(this->tm_.get("float")->matchStrict(union1Body.subTypes[0]));
  EXPECT_TRUE(this->tm_.get("int")->matchStrict(union1Body.subTypes[1]));
  EXPECT_TRUE(this->tm_.get("char")->matchStrict(union2Body.subTypes[0]));
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(union2Body.subTypes[1]));
}

TEST_F(TypeMapTest, UnionDoesNotInsertExact) {
  auto type1 = this->tm_.createUnion({this->tm_.get("float"), this->tm_.get("int")});
  auto type2 = this->tm_.createUnion({this->tm_.get("float"), this->tm_.get("int")});

  EXPECT_EQ(type1->name, type2->name);
  EXPECT_EQ(type1->codeName, type2->codeName);
}

TEST_F(TypeMapTest, UnionAddAddsType) {
  auto type1 = this->tm_.createUnion({this->tm_.get("i8"), this->tm_.get("i16"), this->tm_.get("i32")});
  auto type2 = this->tm_.unionAdd(type1, this->tm_.get("i64"));

  EXPECT_TRUE(type2->isUnion());
  EXPECT_EQ(std::get<TypeUnion>(type2->body).subTypes.size(), 4);
}

TEST_F(TypeMapTest, UnionAddCreatesUnion) {
  auto type2 = this->tm_.unionAdd(this->tm_.get("int"), this->tm_.get("str"));

  EXPECT_TRUE(type2->isUnion());
  EXPECT_EQ(std::get<TypeUnion>(type2->body).subTypes.size(), 2);
}

TEST_F(TypeMapTest, UnionSubSubtractsType) {
  auto type1 = this->tm_.createUnion({this->tm_.get("float"), this->tm_.get("int"), this->tm_.get("bool")});
  auto type2 = this->tm_.unionSub(type1, this->tm_.get("bool"));

  EXPECT_TRUE(type2->isUnion());
  EXPECT_EQ(std::get<TypeUnion>(type2->body).subTypes.size(), 2);
}

TEST_F(TypeMapTest, UnionSubSubtractsTwoOfSameType) {
  auto type1 = this->tm_.createUnion({this->tm_.get("float"), this->tm_.get("int"), this->tm_.get("bool"), this->tm_.get("bool")});
  auto type2 = this->tm_.unionSub(type1, this->tm_.get("bool"));

  EXPECT_TRUE(type2->isUnion());
  EXPECT_EQ(std::get<TypeUnion>(type2->body).subTypes.size(), 2);
}

TEST_F(TypeMapTest, UnionSubSubtractsTwoOfSameTypeAndReturnsSingle) {
  auto type1 = this->tm_.createUnion({this->tm_.get("float"), this->tm_.get("bool"), this->tm_.get("bool")});
  auto type2 = this->tm_.unionSub(type1, this->tm_.get("bool"));

  EXPECT_TRUE(type2->isFloat());
}

TEST_F(TypeMapTest, UnionSubReturnsSame) {
  auto type1 = this->tm_.createUnion({this->tm_.get("float"), this->tm_.get("int")});
  auto type2 = this->tm_.unionSub(type1, this->tm_.get("str"));

  EXPECT_TRUE(type1->matchStrict(type2, true));
}

TEST_F(TypeMapTest, UnionSubReturnsFirst) {
  auto type1 = this->tm_.createUnion({this->tm_.get("bool"), this->tm_.get("bool")});
  auto type2 = this->tm_.unionSub(type1, this->tm_.get("bool"));

  EXPECT_TRUE(type2->isBool());
}

TEST_F(TypeMapTest, UnionSubReturnsSingle) {
  auto type1 = this->tm_.createUnion({this->tm_.get("float"), this->tm_.get("bool")});
  auto type2 = this->tm_.unionSub(type1, this->tm_.get("bool"));

  EXPECT_TRUE(type2->isFloat());
}
