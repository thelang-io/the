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

class TypeMatchTest : public testing::Test {
 protected:
  Type *alias_;
  Type *any_;
  Type *arr_;
  Type *enum_;
  Type *fn_;
  Type *map_;
  Type *obj_;
  Type *opt_;
  Type *ref_;
  TypeMap tm_;
  Type *union_;

  void SetUp () override {
    this->tm_.init("test");
    this->alias_ = this->tm_.createAlias("Alias", this->tm_.get("int"));
    this->any_ = this->tm_.get("any");
    this->arr_ = this->tm_.createArr(this->tm_.get("int"));

    this->tm_.stack.emplace_back("TestEnum");
    this->enum_ = this->tm_.createEnum("TestEnum", {
      this->tm_.createEnumerator("Red"),
      this->tm_.createEnumerator("Brown")
    });
    this->tm_.stack.pop_back();

    this->fn_ = this->tm_.createFn({
      TypeFnParam{"a", this->tm_.get("int"), false, true, false},
      TypeFnParam{"b", this->tm_.get("int"), false, false, true}
    }, this->tm_.get("int"), false);

    this->map_ = this->tm_.createMap(this->tm_.get("str"), this->tm_.get("str"));
    auto objMethod = this->tm_.createMethod({}, this->tm_.get("void"), false, TypeCallInfo{"TestSDm_0", false, "", nullptr, false});

    this->obj_ = this->tm_.createObj("Test", {
      TypeField{"a", this->tm_.get("int"), false, false},
      TypeField{"m", objMethod, false, false}
    });

    this->opt_ = this->tm_.createOpt(this->tm_.get("int"));
    this->ref_ = this->tm_.createRef(this->tm_.get("int"));
    this->union_ = this->tm_.createUnion({this->tm_.get("int"), this->tm_.get("str")});
  }
};

TEST_F(TypeMatchTest, MatchesNice) {
  EXPECT_TRUE(this->alias_->matchNice(this->alias_));
  EXPECT_TRUE(this->any_->matchNice(this->any_));
  EXPECT_TRUE(this->arr_->matchNice(this->arr_));
  EXPECT_TRUE(this->enum_->matchNice(this->enum_));
  EXPECT_TRUE(this->fn_->matchNice(this->fn_));
  EXPECT_TRUE(this->map_->matchNice(this->map_));
  EXPECT_TRUE(this->obj_->matchNice(this->obj_));
  EXPECT_TRUE(this->opt_->matchNice(this->opt_));
  EXPECT_TRUE(this->ref_->matchNice(this->ref_));
  EXPECT_TRUE(this->union_->matchNice(this->union_));

  EXPECT_TRUE(this->tm_.get("bool")->matchNice(this->tm_.get("bool")));
  EXPECT_TRUE(this->tm_.get("byte")->matchNice(this->tm_.get("byte")));
  EXPECT_TRUE(this->tm_.get("byte")->matchNice(this->tm_.get("int")));
  EXPECT_TRUE(this->tm_.get("int")->matchNice(this->tm_.get("byte")));
  EXPECT_TRUE(this->tm_.get("char")->matchNice(this->tm_.get("char")));
  EXPECT_TRUE(this->tm_.get("str")->matchNice(this->tm_.get("str")));
  EXPECT_TRUE(this->tm_.get("void")->matchNice(this->tm_.get("void")));
}

TEST_F(TypeMatchTest, MatchesNiceAlias) {
  auto type1 = this->tm_.createAlias("Alias2", this->tm_.get("i8"));
  auto type2 = this->tm_.createAlias("Alias3", this->tm_.get("str"));

  EXPECT_TRUE(this->alias_->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(this->alias_));

  EXPECT_TRUE(type1->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(type2));
  EXPECT_FALSE(type2->matchNice(type1));
  EXPECT_TRUE(type1->matchNice(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("i8")->matchNice(type1));
}

TEST_F(TypeMatchTest, MatchesNiceAny) {
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->alias_));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->any_));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->arr_));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->enum_));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->fn_));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->map_));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->obj_));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->opt_));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->ref_));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->union_));

  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("any")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("bool")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("byte")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("char")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("float")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("f32")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("f64")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("int")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("i16")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("i32")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("str")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("u16")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("u32")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("u64")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("void")));
}

TEST_F(TypeMatchTest, MatchesNiceFloat) {
  EXPECT_TRUE(this->tm_.get("f32")->matchNice(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("f32")->matchNice(this->tm_.get("i16")));
  EXPECT_TRUE(this->tm_.get("f32")->matchNice(this->tm_.get("i32")));
  EXPECT_TRUE(this->tm_.get("f32")->matchNice(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("f32")->matchNice(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("f32")->matchNice(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("f32")->matchNice(this->tm_.get("u16")));
  EXPECT_FALSE(this->tm_.get("f32")->matchNice(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("f32")->matchNice(this->tm_.get("u64")));
  EXPECT_TRUE(this->tm_.get("f32")->matchNice(this->tm_.get("f32")));
  EXPECT_FALSE(this->tm_.get("f32")->matchNice(this->tm_.get("f64")));
  EXPECT_FALSE(this->tm_.get("f32")->matchNice(this->tm_.get("float")));
  EXPECT_TRUE(this->tm_.get("f64")->matchNice(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("f64")->matchNice(this->tm_.get("i16")));
  EXPECT_TRUE(this->tm_.get("f64")->matchNice(this->tm_.get("i32")));
  EXPECT_TRUE(this->tm_.get("f64")->matchNice(this->tm_.get("int")));
  EXPECT_TRUE(this->tm_.get("f64")->matchNice(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("f64")->matchNice(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("f64")->matchNice(this->tm_.get("u16")));
  EXPECT_TRUE(this->tm_.get("f64")->matchNice(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("f64")->matchNice(this->tm_.get("u64")));
  EXPECT_TRUE(this->tm_.get("f64")->matchNice(this->tm_.get("f32")));
  EXPECT_TRUE(this->tm_.get("f64")->matchNice(this->tm_.get("f64")));
  EXPECT_TRUE(this->tm_.get("f64")->matchNice(this->tm_.get("float")));
  EXPECT_TRUE(this->tm_.get("float")->matchNice(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("float")->matchNice(this->tm_.get("i16")));
  EXPECT_TRUE(this->tm_.get("float")->matchNice(this->tm_.get("i32")));
  EXPECT_TRUE(this->tm_.get("float")->matchNice(this->tm_.get("int")));
  EXPECT_TRUE(this->tm_.get("float")->matchNice(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("float")->matchNice(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("float")->matchNice(this->tm_.get("u16")));
  EXPECT_TRUE(this->tm_.get("float")->matchNice(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("float")->matchNice(this->tm_.get("u64")));
  EXPECT_TRUE(this->tm_.get("float")->matchNice(this->tm_.get("f32")));
  EXPECT_TRUE(this->tm_.get("float")->matchNice(this->tm_.get("f64")));
  EXPECT_TRUE(this->tm_.get("float")->matchNice(this->tm_.get("float")));
}

TEST_F(TypeMatchTest, MatchesNiceInteger) {
  EXPECT_TRUE(this->tm_.get("i8")->matchNice(this->tm_.get("i8")));
  EXPECT_FALSE(this->tm_.get("i8")->matchNice(this->tm_.get("i16")));
  EXPECT_FALSE(this->tm_.get("i8")->matchNice(this->tm_.get("i32")));
  EXPECT_FALSE(this->tm_.get("i8")->matchNice(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("i8")->matchNice(this->tm_.get("i64")));
  EXPECT_FALSE(this->tm_.get("i8")->matchNice(this->tm_.get("u8")));
  EXPECT_FALSE(this->tm_.get("i8")->matchNice(this->tm_.get("u16")));
  EXPECT_FALSE(this->tm_.get("i8")->matchNice(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("i8")->matchNice(this->tm_.get("u64")));
  EXPECT_FALSE(this->tm_.get("i8")->matchNice(this->tm_.get("f32")));
  EXPECT_FALSE(this->tm_.get("i8")->matchNice(this->tm_.get("f64")));
  EXPECT_FALSE(this->tm_.get("i8")->matchNice(this->tm_.get("float")));

  EXPECT_TRUE(this->tm_.get("i16")->matchNice(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("i16")->matchNice(this->tm_.get("i16")));
  EXPECT_FALSE(this->tm_.get("i16")->matchNice(this->tm_.get("i32")));
  EXPECT_FALSE(this->tm_.get("i16")->matchNice(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("i16")->matchNice(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("i16")->matchNice(this->tm_.get("u8")));
  EXPECT_FALSE(this->tm_.get("i16")->matchNice(this->tm_.get("u16")));
  EXPECT_FALSE(this->tm_.get("i16")->matchNice(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("i16")->matchNice(this->tm_.get("u64")));
  EXPECT_FALSE(this->tm_.get("i16")->matchNice(this->tm_.get("f32")));
  EXPECT_FALSE(this->tm_.get("i16")->matchNice(this->tm_.get("f64")));
  EXPECT_FALSE(this->tm_.get("i16")->matchNice(this->tm_.get("float")));

  EXPECT_TRUE(this->tm_.get("i32")->matchNice(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("i32")->matchNice(this->tm_.get("i16")));
  EXPECT_TRUE(this->tm_.get("i32")->matchNice(this->tm_.get("i32")));
  EXPECT_TRUE(this->tm_.get("i32")->matchNice(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("i32")->matchNice(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("i32")->matchNice(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("i32")->matchNice(this->tm_.get("u16")));
  EXPECT_FALSE(this->tm_.get("i32")->matchNice(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("i32")->matchNice(this->tm_.get("u64")));
  EXPECT_FALSE(this->tm_.get("i32")->matchNice(this->tm_.get("f32")));
  EXPECT_FALSE(this->tm_.get("i32")->matchNice(this->tm_.get("f64")));
  EXPECT_FALSE(this->tm_.get("i32")->matchNice(this->tm_.get("float")));

  EXPECT_TRUE(this->tm_.get("int")->matchNice(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("int")->matchNice(this->tm_.get("i16")));
  EXPECT_TRUE(this->tm_.get("int")->matchNice(this->tm_.get("i32")));
  EXPECT_TRUE(this->tm_.get("int")->matchNice(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchNice(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("int")->matchNice(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("int")->matchNice(this->tm_.get("u16")));
  EXPECT_FALSE(this->tm_.get("int")->matchNice(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("int")->matchNice(this->tm_.get("u64")));
  EXPECT_FALSE(this->tm_.get("int")->matchNice(this->tm_.get("f32")));
  EXPECT_FALSE(this->tm_.get("int")->matchNice(this->tm_.get("f64")));
  EXPECT_FALSE(this->tm_.get("int")->matchNice(this->tm_.get("float")));

  EXPECT_TRUE(this->tm_.get("i64")->matchNice(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("i64")->matchNice(this->tm_.get("i16")));
  EXPECT_TRUE(this->tm_.get("i64")->matchNice(this->tm_.get("i32")));
  EXPECT_TRUE(this->tm_.get("i64")->matchNice(this->tm_.get("int")));
  EXPECT_TRUE(this->tm_.get("i64")->matchNice(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("i64")->matchNice(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("i64")->matchNice(this->tm_.get("u16")));
  EXPECT_TRUE(this->tm_.get("i64")->matchNice(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("i64")->matchNice(this->tm_.get("u64")));
  EXPECT_FALSE(this->tm_.get("i64")->matchNice(this->tm_.get("f32")));
  EXPECT_FALSE(this->tm_.get("i64")->matchNice(this->tm_.get("f64")));
  EXPECT_FALSE(this->tm_.get("i64")->matchNice(this->tm_.get("float")));

  EXPECT_FALSE(this->tm_.get("u8")->matchNice(this->tm_.get("i8")));
  EXPECT_FALSE(this->tm_.get("u8")->matchNice(this->tm_.get("i16")));
  EXPECT_FALSE(this->tm_.get("u8")->matchNice(this->tm_.get("i32")));
  EXPECT_FALSE(this->tm_.get("u8")->matchNice(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("u8")->matchNice(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("u8")->matchNice(this->tm_.get("u8")));
  EXPECT_FALSE(this->tm_.get("u8")->matchNice(this->tm_.get("u16")));
  EXPECT_FALSE(this->tm_.get("u8")->matchNice(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("u8")->matchNice(this->tm_.get("u64")));
  EXPECT_FALSE(this->tm_.get("u8")->matchNice(this->tm_.get("f32")));
  EXPECT_FALSE(this->tm_.get("u8")->matchNice(this->tm_.get("f64")));
  EXPECT_FALSE(this->tm_.get("u8")->matchNice(this->tm_.get("float")));

  EXPECT_FALSE(this->tm_.get("u16")->matchNice(this->tm_.get("i8")));
  EXPECT_FALSE(this->tm_.get("u16")->matchNice(this->tm_.get("i16")));
  EXPECT_FALSE(this->tm_.get("u16")->matchNice(this->tm_.get("i32")));
  EXPECT_FALSE(this->tm_.get("u16")->matchNice(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("u16")->matchNice(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("u16")->matchNice(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("u16")->matchNice(this->tm_.get("u16")));
  EXPECT_FALSE(this->tm_.get("u16")->matchNice(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("u16")->matchNice(this->tm_.get("u64")));
  EXPECT_FALSE(this->tm_.get("u16")->matchNice(this->tm_.get("f32")));
  EXPECT_FALSE(this->tm_.get("u16")->matchNice(this->tm_.get("f64")));
  EXPECT_FALSE(this->tm_.get("u16")->matchNice(this->tm_.get("float")));

  EXPECT_FALSE(this->tm_.get("u32")->matchNice(this->tm_.get("i8")));
  EXPECT_FALSE(this->tm_.get("u32")->matchNice(this->tm_.get("i16")));
  EXPECT_FALSE(this->tm_.get("u32")->matchNice(this->tm_.get("i32")));
  EXPECT_FALSE(this->tm_.get("u32")->matchNice(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("u32")->matchNice(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("u32")->matchNice(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("u32")->matchNice(this->tm_.get("u16")));
  EXPECT_TRUE(this->tm_.get("u32")->matchNice(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("u32")->matchNice(this->tm_.get("u64")));
  EXPECT_FALSE(this->tm_.get("u32")->matchNice(this->tm_.get("f32")));
  EXPECT_FALSE(this->tm_.get("u32")->matchNice(this->tm_.get("f64")));
  EXPECT_FALSE(this->tm_.get("u32")->matchNice(this->tm_.get("float")));

  EXPECT_FALSE(this->tm_.get("u64")->matchNice(this->tm_.get("i8")));
  EXPECT_FALSE(this->tm_.get("u64")->matchNice(this->tm_.get("i16")));
  EXPECT_FALSE(this->tm_.get("u64")->matchNice(this->tm_.get("i32")));
  EXPECT_FALSE(this->tm_.get("u64")->matchNice(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("u64")->matchNice(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("u64")->matchNice(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("u64")->matchNice(this->tm_.get("u16")));
  EXPECT_TRUE(this->tm_.get("u64")->matchNice(this->tm_.get("u32")));
  EXPECT_TRUE(this->tm_.get("u64")->matchNice(this->tm_.get("u64")));
  EXPECT_FALSE(this->tm_.get("u64")->matchNice(this->tm_.get("f32")));
  EXPECT_FALSE(this->tm_.get("u64")->matchNice(this->tm_.get("f64")));
  EXPECT_FALSE(this->tm_.get("u64")->matchNice(this->tm_.get("float")));
}

TEST_F(TypeMatchTest, MatchesNiceArray) {
  auto type1 = this->tm_.createArr(this->tm_.get("int"));
  auto type2 = this->tm_.createArr(this->tm_.get("str"));

  EXPECT_TRUE(type1->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(type2));
  EXPECT_FALSE(type2->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchNice(type1));
}

TEST_F(TypeMatchTest, MatchesNiceEnum) {
  this->tm_.stack.emplace_back("Test2");
  auto type1 = this->tm_.createEnum("Test2", {
    this->tm_.createEnumerator("Brown")
  });
  this->tm_.stack.pop_back();

  this->tm_.stack.emplace_back("Test2");
  auto type2 = this->tm_.createEnum("Test2", {
    this->tm_.createEnumerator("Brown"),
    this->tm_.createEnumerator("Red")
  });
  this->tm_.stack.pop_back();

  this->tm_.stack.emplace_back("Test3");
  auto type3 = this->tm_.createEnum("Test3", {
    this->tm_.createEnumerator("Brown"),
    this->tm_.createEnumerator("Red")
  });
  this->tm_.stack.pop_back();

  EXPECT_TRUE(type1->matchNice(type1));
  EXPECT_TRUE(type1->matchNice(type2));
  EXPECT_TRUE(type2->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(type3));
  EXPECT_FALSE(type3->matchNice(type1));
  EXPECT_TRUE(type1->matchNice(this->tm_.get("int")));
  EXPECT_TRUE(this->tm_.get("int")->matchNice(type1));
}

TEST_F(TypeMatchTest, MatchesNiceFunction) {
  auto ref1 = this->tm_.createRef(this->tm_.get("int"));
  auto ref2 = this->tm_.createRef(this->tm_.get("str"));

  auto type1 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type2 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type3 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("int"), false);

  auto type4 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("str"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type5 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("str"), false);

  auto type6 = this->tm_.createFn({}, this->tm_.get("int"), false);

  auto type7 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("int"), false);

  auto type8 = this->tm_.createFn({
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type9 = this->tm_.createFn({
    TypeFnParam{std::nullopt, ref1, false, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type10 = this->tm_.createFn({
    TypeFnParam{std::nullopt, ref1, true, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type11 = this->tm_.createFn({
    TypeFnParam{std::nullopt, ref2, true, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type12 = this->tm_.createFn({
    TypeFnParam{"a", ref2, true, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), true);

  auto type13 = this->tm_.createFn({
    TypeFnParam{std::nullopt, ref2, true, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), true);

  EXPECT_FALSE(type1->matchNice(this->tm_.get("any")));
  EXPECT_TRUE(type1->matchNice(type1));
  EXPECT_TRUE(type1->matchNice(type2));
  EXPECT_FALSE(type1->matchNice(type3));
  EXPECT_FALSE(type1->matchNice(type4));
  EXPECT_FALSE(type1->matchNice(type5));
  EXPECT_FALSE(type1->matchNice(type6));
  EXPECT_FALSE(type1->matchNice(type7));
  EXPECT_FALSE(type1->matchNice(type8));
  EXPECT_TRUE(type8->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(type9));
  EXPECT_TRUE(type9->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(type10));
  EXPECT_FALSE(type10->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(type11));
  EXPECT_FALSE(type11->matchNice(type1));
  EXPECT_FALSE(type10->matchNice(type11));
  EXPECT_FALSE(type11->matchNice(type10));
  EXPECT_FALSE(type11->matchNice(type12));
  EXPECT_FALSE(type12->matchNice(type11));
  EXPECT_FALSE(type12->matchNice(type13));
  EXPECT_TRUE(type13->matchNice(type12));
}

TEST_F(TypeMatchTest, MatchesNiceMap) {
  auto type1 = this->tm_.createMap(this->tm_.get("int"), this->tm_.get("int"));
  auto type2 = this->tm_.createMap(this->tm_.get("str"), this->tm_.get("str"));

  EXPECT_TRUE(type1->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(type2));
  EXPECT_FALSE(type2->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchNice(type1));
}

TEST_F(TypeMatchTest, MatchesNiceObject) {
  auto type1 = this->tm_.createObj("Test1", {
    TypeField{"a", this->tm_.get("int"), false, false},
    TypeField{"b", this->tm_.get("int"), false, false}
  });

  auto type2 = this->tm_.createObj("Test2");

  auto type3 = this->tm_.createObj("Test3", {
    TypeField{"a", this->tm_.get("int"), false, false}
  });

  auto type4 = this->tm_.createObj("Test4", {
    TypeField{"a", this->tm_.get("int"), false, false},
    TypeField{"b", this->tm_.get("int"), false, false}
  });

  auto type5 = this->tm_.createObj("Test5", {
    TypeField{"a", this->tm_.get("int"), false, false},
    TypeField{"b", this->tm_.get("str"), false, false}
  });

  EXPECT_TRUE(type1->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(type2));
  EXPECT_FALSE(type1->matchNice(type3));
  EXPECT_FALSE(type1->matchNice(type4));
  EXPECT_FALSE(type1->matchNice(type5));
  EXPECT_FALSE(type1->matchNice(this->tm_.get("int")));
}

TEST_F(TypeMatchTest, MatchesNiceObjectMethod) {
  auto type1 = this->tm_.createMethod({}, this->tm_.get("void"), false, TypeCallInfo{});
  auto type2CallInfo = TypeCallInfo{"", false, "", nullptr, false};
  auto type2 = this->tm_.createMethod({}, this->tm_.get("void"), false, type2CallInfo);
  auto type3CallInfo = TypeCallInfo{"self1_0", true, "a", this->obj_, false};
  auto type3 = this->tm_.createMethod({}, this->tm_.get("void"), false, type3CallInfo);
  auto type4CallInfo = TypeCallInfo{"self1_0", true, "a", this->obj_, true};
  auto type4 = this->tm_.createMethod({}, this->tm_.get("void"), false, type4CallInfo);
  auto type5CallInfo = TypeCallInfo{"self1_0", true, "b", this->tm_.createRef(this->obj_), true};
  auto type5 = this->tm_.createMethod({}, this->tm_.get("void"), false, type5CallInfo);

  EXPECT_TRUE(type2->matchNice(type2));
  EXPECT_TRUE(type1->matchNice(type2));

  EXPECT_TRUE(type3->matchNice(type3));
  EXPECT_FALSE(type2->matchNice(type3));
  EXPECT_FALSE(type3->matchNice(type2));

  EXPECT_TRUE(type4->matchNice(type4));
  EXPECT_TRUE(type3->matchNice(type4));
  EXPECT_TRUE(type4->matchNice(type3));

  EXPECT_TRUE(type5->matchNice(type5));
  EXPECT_TRUE(type3->matchNice(type5));
  EXPECT_TRUE(type4->matchNice(type5));
  EXPECT_FALSE(type5->matchNice(type3));
  EXPECT_TRUE(type5->matchNice(type4));
}

TEST_F(TypeMatchTest, MatchesNiceOptional) {
  auto type1 = this->tm_.createOpt(this->tm_.get("int"));
  auto type2 = this->tm_.createOpt(this->tm_.get("str"));

  EXPECT_TRUE(type1->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(type2));
  EXPECT_FALSE(type2->matchNice(type1));
  EXPECT_TRUE(type1->matchNice(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchNice(type1));
}

TEST_F(TypeMatchTest, MatchesNiceReference) {
  auto type1 = this->tm_.createRef(this->tm_.get("int"));
  auto type2 = this->tm_.createRef(this->tm_.get("int"));

  EXPECT_TRUE(type1->matchNice(type1));
  EXPECT_TRUE(type1->matchNice(type2));
  EXPECT_TRUE(type2->matchNice(type1));
  EXPECT_TRUE(type1->matchNice(this->tm_.get("int")));
  EXPECT_TRUE(this->tm_.get("int")->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(this->tm_.get("str")));
  EXPECT_FALSE(this->tm_.get("str")->matchNice(type1));
}

TEST_F(TypeMatchTest, MatchesNiceUnion) {
  auto type1 = this->tm_.createUnion({this->tm_.get("int"), this->tm_.get("float")});
  auto type2 = this->tm_.createUnion({this->tm_.get("char"), this->tm_.get("str")});
  auto type3 = this->tm_.createUnion({this->tm_.get("u8"), this->tm_.get("u16"), this->tm_.get("u32"), this->tm_.get("u64")});

  EXPECT_TRUE(type1->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(type2));
  EXPECT_FALSE(type1->matchNice(type3));
  EXPECT_FALSE(type2->matchNice(type1));
  EXPECT_FALSE(type3->matchNice(type1));
  EXPECT_TRUE(type1->matchNice(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchNice(type1));
}

TEST_F(TypeMatchTest, MatchesStrict) {
  EXPECT_TRUE(this->alias_->matchStrict(this->alias_));
  EXPECT_TRUE(this->any_->matchStrict(this->any_));
  EXPECT_TRUE(this->arr_->matchStrict(this->arr_));
  EXPECT_TRUE(this->enum_->matchStrict(this->enum_));
  EXPECT_TRUE(this->fn_->matchStrict(this->fn_));
  EXPECT_TRUE(this->map_->matchStrict(this->map_));
  EXPECT_TRUE(this->obj_->matchStrict(this->obj_));
  EXPECT_TRUE(this->opt_->matchStrict(this->opt_));
  EXPECT_TRUE(this->ref_->matchStrict(this->ref_));
  EXPECT_TRUE(this->union_->matchStrict(this->union_));

  EXPECT_TRUE(this->tm_.get("any")->matchStrict(this->tm_.get("any")));
  EXPECT_TRUE(this->tm_.get("bool")->matchStrict(this->tm_.get("bool")));
  EXPECT_TRUE(this->tm_.get("byte")->matchStrict(this->tm_.get("byte")));
  EXPECT_TRUE(this->tm_.get("char")->matchStrict(this->tm_.get("char")));
  EXPECT_TRUE(this->tm_.get("float")->matchStrict(this->tm_.get("float")));
  EXPECT_TRUE(this->tm_.get("f32")->matchStrict(this->tm_.get("f32")));
  EXPECT_TRUE(this->tm_.get("f64")->matchStrict(this->tm_.get("f64")));
  EXPECT_TRUE(this->tm_.get("int")->matchStrict(this->tm_.get("int")));
  EXPECT_TRUE(this->tm_.get("i8")->matchStrict(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("i16")->matchStrict(this->tm_.get("i16")));
  EXPECT_TRUE(this->tm_.get("i32")->matchStrict(this->tm_.get("i32")));
  EXPECT_TRUE(this->tm_.get("i64")->matchStrict(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("str")->matchStrict(this->tm_.get("str")));
  EXPECT_TRUE(this->tm_.get("u8")->matchStrict(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("u16")->matchStrict(this->tm_.get("u16")));
  EXPECT_TRUE(this->tm_.get("u32")->matchStrict(this->tm_.get("u32")));
  EXPECT_TRUE(this->tm_.get("u64")->matchStrict(this->tm_.get("u64")));
  EXPECT_TRUE(this->tm_.get("void")->matchStrict(this->tm_.get("void")));
}

TEST_F(TypeMatchTest, MatchesStrictAlias) {
  auto type1 = this->tm_.createAlias("Alias2", this->tm_.get("i8"));
  auto type2 = this->tm_.createAlias("Alias3", this->tm_.get("str"));

  EXPECT_FALSE(this->alias_->matchStrict(type1));
  EXPECT_FALSE(type1->matchStrict(this->alias_));

  EXPECT_TRUE(type1->matchStrict(type1));
  EXPECT_FALSE(type1->matchStrict(type2));
  EXPECT_FALSE(type2->matchStrict(type1));
  EXPECT_TRUE(type1->matchStrict(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("i8")->matchStrict(type1));
}

TEST_F(TypeMatchTest, MatchesStrictArray) {
  auto type1 = this->tm_.createArr(this->tm_.get("int"));
  auto type2 = this->tm_.createArr(this->tm_.get("str"));

  EXPECT_TRUE(type1->matchStrict(type1));
  EXPECT_FALSE(type1->matchStrict(type2));
  EXPECT_FALSE(type2->matchStrict(type1));
  EXPECT_FALSE(type1->matchStrict(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchStrict(type1));
}

TEST_F(TypeMatchTest, MatchesStrictEnum) {
  this->tm_.stack.emplace_back("Test2");
  auto type1 = this->tm_.createEnum("Test2", {
    this->tm_.createEnumerator("Brown")
  });
  this->tm_.stack.pop_back();

  this->tm_.stack.emplace_back("Test2");
  auto type2 = this->tm_.createEnum("Test2", {
    this->tm_.createEnumerator("Brown"),
    this->tm_.createEnumerator("Red")
  });
  this->tm_.stack.pop_back();

  this->tm_.stack.emplace_back("Test3");
  auto type3 = this->tm_.createEnum("Test3", {
    this->tm_.createEnumerator("Brown"),
    this->tm_.createEnumerator("Red")
  });
  this->tm_.stack.pop_back();

  EXPECT_TRUE(type1->matchStrict(type1));
  EXPECT_TRUE(type1->matchStrict(type2));
  EXPECT_TRUE(type2->matchStrict(type1));
  EXPECT_FALSE(type1->matchStrict(type3));
  EXPECT_FALSE(type3->matchStrict(type1));
  EXPECT_FALSE(type1->matchStrict(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchStrict(type1));
}

TEST_F(TypeMatchTest, MatchesStrictFunction) {
  auto ref1 = this->tm_.createRef(this->tm_.get("int"));
  auto ref2 = this->tm_.createRef(this->tm_.get("str"));

  auto type1 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type2 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type3 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("int"), false);

  auto type4 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("str"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type5 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("str"), false);

  auto type6 = this->tm_.createFn({}, this->tm_.get("int"), false);

  auto type7 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("int"), false);

  auto type8 = this->tm_.createFn({
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type9 = this->tm_.createFn({
    TypeFnParam{std::nullopt, ref1, false, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type10 = this->tm_.createFn({
    TypeFnParam{std::nullopt, ref1, true, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type11 = this->tm_.createFn({
    TypeFnParam{std::nullopt, ref2, true, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type12 = this->tm_.createFn({
    TypeFnParam{"a", ref2, true, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), true);

  auto type13 = this->tm_.createFn({
    TypeFnParam{std::nullopt, ref2, true, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), true);

  auto type14 = this->tm_.createFn({
    TypeFnParam{std::nullopt, ref2, true, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), true);

  EXPECT_FALSE(type1->matchStrict(this->tm_.get("any")));
  EXPECT_TRUE(type1->matchStrict(type1));
  EXPECT_TRUE(type1->matchStrict(type2));
  EXPECT_FALSE(type1->matchStrict(type3));
  EXPECT_FALSE(type1->matchStrict(type4));
  EXPECT_FALSE(type1->matchStrict(type5));
  EXPECT_FALSE(type1->matchStrict(type6));
  EXPECT_FALSE(type1->matchStrict(type7));
  EXPECT_TRUE(type1->matchStrict(type8));
  EXPECT_TRUE(type8->matchStrict(type1));
  EXPECT_FALSE(type1->matchStrict(type9));
  EXPECT_FALSE(type9->matchStrict(type1));
  EXPECT_FALSE(type1->matchStrict(type10));
  EXPECT_FALSE(type10->matchStrict(type1));
  EXPECT_FALSE(type1->matchStrict(type11));
  EXPECT_FALSE(type11->matchStrict(type1));
  EXPECT_FALSE(type10->matchStrict(type11));
  EXPECT_FALSE(type11->matchStrict(type10));
  EXPECT_FALSE(type11->matchStrict(type12));
  EXPECT_FALSE(type12->matchStrict(type11));
  EXPECT_TRUE(type12->matchStrict(type13));
  EXPECT_TRUE(type12->matchStrict(type14));
  EXPECT_TRUE(type13->matchStrict(type12));
  EXPECT_TRUE(type13->matchStrict(type14));
  EXPECT_TRUE(type14->matchStrict(type13));
  EXPECT_TRUE(type14->matchStrict(type12));
}

TEST_F(TypeMatchTest, MatchesStrictExactFunction) {
  auto ref1 = this->tm_.createRef(this->tm_.get("int"));
  auto ref2 = this->tm_.createRef(this->tm_.get("str"));

  auto type1 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type2 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type3 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("int"), false);

  auto type4 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("str"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type5 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("str"), false);

  auto type6 = this->tm_.createFn({}, this->tm_.get("int"), false);

  auto type7 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("int"), false);

  auto type8 = this->tm_.createFn({
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type9 = this->tm_.createFn({
    TypeFnParam{std::nullopt, ref1, false, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type10 = this->tm_.createFn({
    TypeFnParam{std::nullopt, ref1, true, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type11 = this->tm_.createFn({
    TypeFnParam{std::nullopt, ref2, true, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), false);

  auto type12 = this->tm_.createFn({
    TypeFnParam{"a", ref2, true, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), true);

  auto type13 = this->tm_.createFn({
    TypeFnParam{std::nullopt, ref2, true, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), true);

  auto type14 = this->tm_.createFn({
    TypeFnParam{std::nullopt, ref2, true, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"), true);

  EXPECT_FALSE(type1->matchStrict(this->tm_.get("any"), true));
  EXPECT_TRUE(type1->matchStrict(type1, true));
  EXPECT_TRUE(type1->matchStrict(type2, true));
  EXPECT_FALSE(type1->matchStrict(type3, true));
  EXPECT_FALSE(type1->matchStrict(type4, true));
  EXPECT_FALSE(type1->matchStrict(type5, true));
  EXPECT_FALSE(type1->matchStrict(type6, true));
  EXPECT_FALSE(type1->matchStrict(type7, true));
  EXPECT_FALSE(type1->matchStrict(type8, true));
  EXPECT_FALSE(type8->matchStrict(type1, true));
  EXPECT_FALSE(type1->matchStrict(type9, true));
  EXPECT_FALSE(type9->matchStrict(type1, true));
  EXPECT_FALSE(type1->matchStrict(type10, true));
  EXPECT_FALSE(type10->matchStrict(type1, true));
  EXPECT_FALSE(type1->matchStrict(type11, true));
  EXPECT_FALSE(type11->matchStrict(type1, true));
  EXPECT_FALSE(type10->matchStrict(type11, true));
  EXPECT_FALSE(type11->matchStrict(type10, true));
  EXPECT_FALSE(type11->matchStrict(type12, true));
  EXPECT_FALSE(type12->matchStrict(type11, true));
  EXPECT_FALSE(type12->matchStrict(type13, true));
  EXPECT_FALSE(type12->matchStrict(type14, true));
  EXPECT_FALSE(type13->matchStrict(type12, true));
  EXPECT_TRUE(type13->matchStrict(type14, true));
  EXPECT_TRUE(type14->matchStrict(type13, true));
  EXPECT_FALSE(type14->matchStrict(type12, true));
}

TEST_F(TypeMatchTest, MatchesStrictMap) {
  auto type1 = this->tm_.createMap(this->tm_.get("int"), this->tm_.get("int"));
  auto type2 = this->tm_.createMap(this->tm_.get("str"), this->tm_.get("str"));

  EXPECT_TRUE(type1->matchStrict(type1));
  EXPECT_FALSE(type1->matchStrict(type2));
  EXPECT_FALSE(type2->matchStrict(type1));
  EXPECT_FALSE(type1->matchStrict(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchStrict(type1));
}

TEST_F(TypeMatchTest, MatchesStrictObject) {
  auto type1 = this->tm_.createObj("Test1", {
    TypeField{"a", this->tm_.get("int"), false, false},
    TypeField{"b", this->tm_.get("int"), false, false}
  });

  auto type2 = this->tm_.createObj("Test2");

  auto type3 = this->tm_.createObj("Test3", {
    TypeField{"a", this->tm_.get("int"), false, false}
  });

  auto type4 = this->tm_.createObj("Test4", {
    TypeField{"a", this->tm_.get("int"), false, false},
    TypeField{"b", this->tm_.get("int"), false, false}
  });

  auto type5 = this->tm_.createObj("Test5", {
    TypeField{"a", this->tm_.get("int"), false, false},
    TypeField{"b", this->tm_.get("str"), false, false}
  });

  EXPECT_TRUE(type1->matchStrict(type1));
  EXPECT_FALSE(type1->matchStrict(type2));
  EXPECT_FALSE(type1->matchStrict(type3));
  EXPECT_FALSE(type1->matchStrict(type4));
  EXPECT_FALSE(type1->matchStrict(type5));
  EXPECT_FALSE(type1->matchStrict(this->tm_.get("int")));
}

TEST_F(TypeMatchTest, MatchesStrictObjectMethod) {
  auto type1 = this->tm_.createMethod({}, this->tm_.get("void"), false, TypeCallInfo{});
  auto type2CallInfo = TypeCallInfo{"", false, "", nullptr, false};
  auto type2 = this->tm_.createMethod({}, this->tm_.get("void"), false, type2CallInfo);
  auto type3CallInfo = TypeCallInfo{"self1_0", true, "a", this->obj_, false};
  auto type3 = this->tm_.createMethod({}, this->tm_.get("void"), false, type3CallInfo);
  auto type4CallInfo = TypeCallInfo{"self1_0", true, "a", this->obj_, true};
  auto type4 = this->tm_.createMethod({}, this->tm_.get("void"), false, type4CallInfo);
  auto type5CallInfo = TypeCallInfo{"self1_0", true, "a", this->tm_.createRef(this->obj_), true};
  auto type5 = this->tm_.createMethod({}, this->tm_.get("void"), false, type5CallInfo);

  EXPECT_TRUE(type2->matchStrict(type2));
  EXPECT_TRUE(type1->matchStrict(type2));

  EXPECT_TRUE(type3->matchStrict(type3));
  EXPECT_FALSE(type2->matchStrict(type3));
  EXPECT_FALSE(type3->matchStrict(type2));

  EXPECT_TRUE(type4->matchStrict(type4));
  EXPECT_FALSE(type3->matchStrict(type4));
  EXPECT_FALSE(type4->matchStrict(type3));

  EXPECT_TRUE(type5->matchStrict(type5));
  EXPECT_FALSE(type3->matchStrict(type5));
  EXPECT_FALSE(type4->matchStrict(type5));
  EXPECT_FALSE(type5->matchStrict(type3));
  EXPECT_FALSE(type5->matchStrict(type4));
}

TEST_F(TypeMatchTest, MatchesStrictOptional) {
  auto type1 = this->tm_.createOpt(this->tm_.get("int"));
  auto type2 = this->tm_.createOpt(this->tm_.get("str"));

  EXPECT_TRUE(type1->matchStrict(type1));
  EXPECT_FALSE(type1->matchStrict(type2));
  EXPECT_FALSE(type2->matchStrict(type1));
  EXPECT_FALSE(type1->matchStrict(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchStrict(type1));
}

TEST_F(TypeMatchTest, MatchesStrictReference) {
  auto type1 = this->tm_.createRef(this->tm_.get("int"));
  auto type2 = this->tm_.createRef(this->tm_.get("int"));

  EXPECT_TRUE(type1->matchStrict(type1));
  EXPECT_TRUE(type1->matchStrict(type2));
  EXPECT_TRUE(type2->matchStrict(type1));
  EXPECT_FALSE(type1->matchStrict(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchStrict(type1));
  EXPECT_FALSE(type1->matchStrict(this->tm_.get("str")));
  EXPECT_FALSE(this->tm_.get("str")->matchStrict(type1));
}

TEST_F(TypeMatchTest, MatchesStrictUnion) {
  auto type1 = this->tm_.createUnion({this->tm_.get("int"), this->tm_.get("float")});
  auto type2 = this->tm_.createUnion({this->tm_.get("char"), this->tm_.get("str")});
  auto type3 = this->tm_.createUnion({this->tm_.get("u8"), this->tm_.get("u16"), this->tm_.get("u32"), this->tm_.get("u64")});

  EXPECT_TRUE(type1->matchStrict(type1));
  EXPECT_FALSE(type1->matchStrict(type2));
  EXPECT_FALSE(type1->matchStrict(type3));
  EXPECT_FALSE(type2->matchStrict(type1));
  EXPECT_FALSE(type3->matchStrict(type1));
  EXPECT_FALSE(type1->matchStrict(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchStrict(type1));
}
