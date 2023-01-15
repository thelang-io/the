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
#include "utils.hpp"

class TypeTest : public testing::Test {
 protected:
  Type *any_;
  Type *arr_;
  Type *enum_;
  Type *fn_;
  Type *obj_;
  Type *opt_;
  Type *ref_;
  TypeMap tm_;

  void SetUp () override {
    this->tm_.init();
    this->any_ = this->tm_.get("any");
    this->arr_ = this->tm_.arrayOf(this->tm_.get("int"));

    this->tm_.stack.emplace_back("TestEnum");
    this->enum_ = this->tm_.enumeration("TestEnum", "TestEnum_0", {
      this->tm_.enumerator("Red"),
      this->tm_.enumerator("Brown")
    });
    this->tm_.stack.pop_back();

    this->fn_ = this->tm_.fn("test_0", {
      TypeFnParam{"a", this->tm_.get("int"), false, true, false},
      TypeFnParam{"b", this->tm_.get("int"), false, false, true}
    }, this->tm_.get("int"));

    auto objMethod = this->tm_.fn("Test_0.m_0", {}, this->tm_.get("void"), TypeFnMethodInfo{false, "", nullptr, false});

    this->obj_ = this->tm_.obj("Test", "Test_0", {
      TypeField{"a", this->tm_.get("int"), false, false, false},
      TypeField{"m", objMethod, false, true, false}
    });

    this->opt_ = this->tm_.opt(this->tm_.get("int"));
    this->ref_ = this->tm_.ref(this->tm_.get("int"));
  }
};

TEST_F(TypeTest, LargestNumbers) {
  EXPECT_TRUE(Type::largest(this->tm_.get("f64"), this->tm_.get("f64"))->isF64());
  EXPECT_TRUE(Type::largest(this->tm_.get("f64"), this->tm_.get("float"))->isF64());
  EXPECT_TRUE(Type::largest(this->tm_.get("f64"), this->tm_.get("f32"))->isF64());
  EXPECT_TRUE(Type::largest(this->tm_.get("f64"), this->tm_.get("u32"))->isF64());
  EXPECT_TRUE(Type::largest(this->tm_.get("f64"), this->tm_.get("u16"))->isF64());
  EXPECT_TRUE(Type::largest(this->tm_.get("f64"), this->tm_.get("u8"))->isF64());
  EXPECT_TRUE(Type::largest(this->tm_.get("f64"), this->tm_.get("i64"))->isF64());
  EXPECT_TRUE(Type::largest(this->tm_.get("f64"), this->tm_.get("i32"))->isF64());
  EXPECT_TRUE(Type::largest(this->tm_.get("f64"), this->tm_.get("int"))->isF64());
  EXPECT_TRUE(Type::largest(this->tm_.get("f64"), this->tm_.get("i16"))->isF64());
  EXPECT_TRUE(Type::largest(this->tm_.get("f64"), this->tm_.get("i8"))->isF64());

  EXPECT_TRUE(Type::largest(this->tm_.get("float"), this->tm_.get("f64"))->isFloat());
  EXPECT_TRUE(Type::largest(this->tm_.get("float"), this->tm_.get("float"))->isFloat());
  EXPECT_TRUE(Type::largest(this->tm_.get("float"), this->tm_.get("f32"))->isFloat());
  EXPECT_TRUE(Type::largest(this->tm_.get("float"), this->tm_.get("u32"))->isFloat());
  EXPECT_TRUE(Type::largest(this->tm_.get("float"), this->tm_.get("u16"))->isFloat());
  EXPECT_TRUE(Type::largest(this->tm_.get("float"), this->tm_.get("u8"))->isFloat());
  EXPECT_TRUE(Type::largest(this->tm_.get("float"), this->tm_.get("i64"))->isFloat());
  EXPECT_TRUE(Type::largest(this->tm_.get("float"), this->tm_.get("i32"))->isFloat());
  EXPECT_TRUE(Type::largest(this->tm_.get("float"), this->tm_.get("int"))->isFloat());
  EXPECT_TRUE(Type::largest(this->tm_.get("float"), this->tm_.get("i16"))->isFloat());
  EXPECT_TRUE(Type::largest(this->tm_.get("float"), this->tm_.get("i8"))->isFloat());

  EXPECT_TRUE(Type::largest(this->tm_.get("f32"), this->tm_.get("f32"))->isF32());
  EXPECT_TRUE(Type::largest(this->tm_.get("f32"), this->tm_.get("u16"))->isF32());
  EXPECT_TRUE(Type::largest(this->tm_.get("f32"), this->tm_.get("u8"))->isF32());
  EXPECT_TRUE(Type::largest(this->tm_.get("f32"), this->tm_.get("i32"))->isF32());
  EXPECT_TRUE(Type::largest(this->tm_.get("f32"), this->tm_.get("int"))->isF32());
  EXPECT_TRUE(Type::largest(this->tm_.get("f32"), this->tm_.get("i16"))->isF32());
  EXPECT_TRUE(Type::largest(this->tm_.get("f32"), this->tm_.get("i8"))->isF32());

  EXPECT_TRUE(Type::largest(this->tm_.get("i64"), this->tm_.get("u32"))->isI64());
  EXPECT_TRUE(Type::largest(this->tm_.get("i64"), this->tm_.get("u16"))->isI64());
  EXPECT_TRUE(Type::largest(this->tm_.get("i64"), this->tm_.get("u8"))->isI64());
  EXPECT_TRUE(Type::largest(this->tm_.get("i64"), this->tm_.get("i64"))->isI64());
  EXPECT_TRUE(Type::largest(this->tm_.get("i64"), this->tm_.get("i32"))->isI64());
  EXPECT_TRUE(Type::largest(this->tm_.get("i64"), this->tm_.get("int"))->isI64());
  EXPECT_TRUE(Type::largest(this->tm_.get("i64"), this->tm_.get("i16"))->isI64());
  EXPECT_TRUE(Type::largest(this->tm_.get("i64"), this->tm_.get("i8"))->isI64());

  EXPECT_TRUE(Type::largest(this->tm_.get("i32"), this->tm_.get("u16"))->isI32());
  EXPECT_TRUE(Type::largest(this->tm_.get("i32"), this->tm_.get("u8"))->isI32());
  EXPECT_TRUE(Type::largest(this->tm_.get("i32"), this->tm_.get("i32"))->isI32());
  EXPECT_TRUE(Type::largest(this->tm_.get("i32"), this->tm_.get("int"))->isI32());
  EXPECT_TRUE(Type::largest(this->tm_.get("i32"), this->tm_.get("i16"))->isI32());
  EXPECT_TRUE(Type::largest(this->tm_.get("i32"), this->tm_.get("i8"))->isI32());

  EXPECT_TRUE(Type::largest(this->tm_.get("int"), this->tm_.get("u16"))->isInt());
  EXPECT_TRUE(Type::largest(this->tm_.get("int"), this->tm_.get("u8"))->isInt());
  EXPECT_TRUE(Type::largest(this->tm_.get("int"), this->tm_.get("i32"))->isInt());
  EXPECT_TRUE(Type::largest(this->tm_.get("int"), this->tm_.get("int"))->isInt());
  EXPECT_TRUE(Type::largest(this->tm_.get("int"), this->tm_.get("i16"))->isInt());
  EXPECT_TRUE(Type::largest(this->tm_.get("int"), this->tm_.get("i8"))->isInt());

  EXPECT_TRUE(Type::largest(this->tm_.get("i16"), this->tm_.get("u8"))->isI16());
  EXPECT_TRUE(Type::largest(this->tm_.get("i16"), this->tm_.get("i16"))->isI16());
  EXPECT_TRUE(Type::largest(this->tm_.get("i16"), this->tm_.get("i8"))->isI16());

  EXPECT_TRUE(Type::largest(this->tm_.get("u64"), this->tm_.get("u64"))->isU64());
  EXPECT_TRUE(Type::largest(this->tm_.get("u64"), this->tm_.get("u32"))->isU64());
  EXPECT_TRUE(Type::largest(this->tm_.get("u64"), this->tm_.get("u16"))->isU64());
  EXPECT_TRUE(Type::largest(this->tm_.get("u64"), this->tm_.get("u8"))->isU64());
  EXPECT_TRUE(Type::largest(this->tm_.get("u32"), this->tm_.get("u32"))->isU32());
  EXPECT_TRUE(Type::largest(this->tm_.get("u32"), this->tm_.get("u16"))->isU32());
  EXPECT_TRUE(Type::largest(this->tm_.get("u32"), this->tm_.get("u8"))->isU32());
  EXPECT_TRUE(Type::largest(this->tm_.get("u16"), this->tm_.get("u16"))->isU16());
  EXPECT_TRUE(Type::largest(this->tm_.get("u16"), this->tm_.get("u8"))->isU16());

  EXPECT_TRUE(Type::largest(this->tm_.get("i8"), this->tm_.get("i8"))->isI8());
  EXPECT_TRUE(Type::largest(this->tm_.get("u8"), this->tm_.get("u8"))->isU8());

  EXPECT_TRUE(Type::largest(this->tm_.get("f64"), this->tm_.get("f64"))->isF64());
  EXPECT_TRUE(Type::largest(this->tm_.get("float"), this->tm_.get("f64"))->isFloat());
  EXPECT_TRUE(Type::largest(this->tm_.get("f32"), this->tm_.get("f64"))->isF64());
  EXPECT_TRUE(Type::largest(this->tm_.get("u32"), this->tm_.get("f64"))->isF64());
  EXPECT_TRUE(Type::largest(this->tm_.get("u16"), this->tm_.get("f64"))->isF64());
  EXPECT_TRUE(Type::largest(this->tm_.get("u8"), this->tm_.get("f64"))->isF64());
  EXPECT_TRUE(Type::largest(this->tm_.get("i64"), this->tm_.get("f64"))->isF64());
  EXPECT_TRUE(Type::largest(this->tm_.get("i32"), this->tm_.get("f64"))->isF64());
  EXPECT_TRUE(Type::largest(this->tm_.get("int"), this->tm_.get("f64"))->isF64());
  EXPECT_TRUE(Type::largest(this->tm_.get("i16"), this->tm_.get("f64"))->isF64());
  EXPECT_TRUE(Type::largest(this->tm_.get("i8"), this->tm_.get("f64"))->isF64());

  EXPECT_TRUE(Type::largest(this->tm_.get("f64"), this->tm_.get("float"))->isF64());
  EXPECT_TRUE(Type::largest(this->tm_.get("float"), this->tm_.get("float"))->isFloat());
  EXPECT_TRUE(Type::largest(this->tm_.get("f32"), this->tm_.get("float"))->isFloat());
  EXPECT_TRUE(Type::largest(this->tm_.get("u32"), this->tm_.get("float"))->isFloat());
  EXPECT_TRUE(Type::largest(this->tm_.get("u16"), this->tm_.get("float"))->isFloat());
  EXPECT_TRUE(Type::largest(this->tm_.get("u8"), this->tm_.get("float"))->isFloat());
  EXPECT_TRUE(Type::largest(this->tm_.get("i64"), this->tm_.get("float"))->isFloat());
  EXPECT_TRUE(Type::largest(this->tm_.get("i32"), this->tm_.get("float"))->isFloat());
  EXPECT_TRUE(Type::largest(this->tm_.get("int"), this->tm_.get("float"))->isFloat());
  EXPECT_TRUE(Type::largest(this->tm_.get("i16"), this->tm_.get("float"))->isFloat());
  EXPECT_TRUE(Type::largest(this->tm_.get("i8"), this->tm_.get("float"))->isFloat());

  EXPECT_TRUE(Type::largest(this->tm_.get("f32"), this->tm_.get("f32"))->isF32());
  EXPECT_TRUE(Type::largest(this->tm_.get("u16"), this->tm_.get("f32"))->isF32());
  EXPECT_TRUE(Type::largest(this->tm_.get("u8"), this->tm_.get("f32"))->isF32());
  EXPECT_TRUE(Type::largest(this->tm_.get("i32"), this->tm_.get("f32"))->isF32());
  EXPECT_TRUE(Type::largest(this->tm_.get("int"), this->tm_.get("f32"))->isF32());
  EXPECT_TRUE(Type::largest(this->tm_.get("i16"), this->tm_.get("f32"))->isF32());
  EXPECT_TRUE(Type::largest(this->tm_.get("i8"), this->tm_.get("f32"))->isF32());

  EXPECT_TRUE(Type::largest(this->tm_.get("u32"), this->tm_.get("i64"))->isI64());
  EXPECT_TRUE(Type::largest(this->tm_.get("u16"), this->tm_.get("i64"))->isI64());
  EXPECT_TRUE(Type::largest(this->tm_.get("u8"), this->tm_.get("i64"))->isI64());
  EXPECT_TRUE(Type::largest(this->tm_.get("i64"), this->tm_.get("i64"))->isI64());
  EXPECT_TRUE(Type::largest(this->tm_.get("i32"), this->tm_.get("i64"))->isI64());
  EXPECT_TRUE(Type::largest(this->tm_.get("int"), this->tm_.get("i64"))->isI64());
  EXPECT_TRUE(Type::largest(this->tm_.get("i16"), this->tm_.get("i64"))->isI64());
  EXPECT_TRUE(Type::largest(this->tm_.get("i8"), this->tm_.get("i64"))->isI64());

  EXPECT_TRUE(Type::largest(this->tm_.get("u16"), this->tm_.get("i32"))->isI32());
  EXPECT_TRUE(Type::largest(this->tm_.get("u8"), this->tm_.get("i32"))->isI32());
  EXPECT_TRUE(Type::largest(this->tm_.get("i32"), this->tm_.get("i32"))->isI32());
  EXPECT_TRUE(Type::largest(this->tm_.get("int"), this->tm_.get("i32"))->isInt());
  EXPECT_TRUE(Type::largest(this->tm_.get("i16"), this->tm_.get("i32"))->isI32());
  EXPECT_TRUE(Type::largest(this->tm_.get("i8"), this->tm_.get("i32"))->isI32());

  EXPECT_TRUE(Type::largest(this->tm_.get("u16"), this->tm_.get("int"))->isInt());
  EXPECT_TRUE(Type::largest(this->tm_.get("u8"), this->tm_.get("int"))->isInt());
  EXPECT_TRUE(Type::largest(this->tm_.get("i32"), this->tm_.get("int"))->isI32());
  EXPECT_TRUE(Type::largest(this->tm_.get("int"), this->tm_.get("int"))->isInt());
  EXPECT_TRUE(Type::largest(this->tm_.get("i16"), this->tm_.get("int"))->isInt());
  EXPECT_TRUE(Type::largest(this->tm_.get("i8"), this->tm_.get("int"))->isInt());

  EXPECT_TRUE(Type::largest(this->tm_.get("u8"), this->tm_.get("i16"))->isI16());
  EXPECT_TRUE(Type::largest(this->tm_.get("i16"), this->tm_.get("i16"))->isI16());
  EXPECT_TRUE(Type::largest(this->tm_.get("i8"), this->tm_.get("i16"))->isI16());

  EXPECT_TRUE(Type::largest(this->tm_.get("u64"), this->tm_.get("u64"))->isU64());
  EXPECT_TRUE(Type::largest(this->tm_.get("u32"), this->tm_.get("u64"))->isU64());
  EXPECT_TRUE(Type::largest(this->tm_.get("u16"), this->tm_.get("u64"))->isU64());
  EXPECT_TRUE(Type::largest(this->tm_.get("u8"), this->tm_.get("u64"))->isU64());
  EXPECT_TRUE(Type::largest(this->tm_.get("u32"), this->tm_.get("u32"))->isU32());
  EXPECT_TRUE(Type::largest(this->tm_.get("u16"), this->tm_.get("u32"))->isU32());
  EXPECT_TRUE(Type::largest(this->tm_.get("u8"), this->tm_.get("u32"))->isU32());
  EXPECT_TRUE(Type::largest(this->tm_.get("u16"), this->tm_.get("u16"))->isU16());
  EXPECT_TRUE(Type::largest(this->tm_.get("u8"), this->tm_.get("u16"))->isU16());

  EXPECT_TRUE(Type::largest(this->tm_.get("i8"), this->tm_.get("i8"))->isI8());
  EXPECT_TRUE(Type::largest(this->tm_.get("u8"), this->tm_.get("u8"))->isU8());
}

TEST_F(TypeTest, LargestNonNumbers) {
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("any"), this->tm_.get("any")), "tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("any"), this->tm_.get("int")), "tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("int"), this->tm_.get("any")), "tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("bool"), this->tm_.get("bool")), "tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("bool"), this->tm_.get("int")), "tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("int"), this->tm_.get("bool")), "tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("byte"), this->tm_.get("byte")), "tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("byte"), this->tm_.get("int")), "tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("int"), this->tm_.get("byte")), "tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("char"), this->tm_.get("char")), "tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("char"), this->tm_.get("int")), "tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("int"), this->tm_.get("char")), "tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("str"), this->tm_.get("str")), "tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("str"), this->tm_.get("int")), "tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("int"), this->tm_.get("str")), "tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("void"), this->tm_.get("void")), "tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("void"), this->tm_.get("int")), "tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("int"), this->tm_.get("void")), "tried to find largest type of non-number");
}

TEST_F(TypeTest, RealOnReal) {
  EXPECT_EQ(Type::real(this->tm_.get("int")), this->tm_.get("int"));
}

TEST_F(TypeTest, RealOnRef) {
  EXPECT_EQ(Type::real(this->ref_), this->tm_.get("int"));
}

TEST_F(TypeTest, GetsEnumerator) {
  EXPECT_EQ(this->enum_->getEnumerator("Red")->codeName, "TestEnumSDRed_0");
}

TEST_F(TypeTest, GetsProp) {
  EXPECT_NE(this->any_->getProp("str"), nullptr);
  EXPECT_EQ(this->arr_->getProp("len"), this->tm_.get("int"));
  EXPECT_NE(this->enum_->getProp("str"), nullptr);
  EXPECT_EQ(this->obj_->getProp("a"), this->tm_.get("int"));
  EXPECT_NE(this->opt_->getProp("str"), nullptr);
  EXPECT_NE(this->ref_->getProp("str"), nullptr);
  EXPECT_NE(this->tm_.get("int")->getProp("str"), nullptr);
  EXPECT_EQ(this->tm_.get("str")->getProp("len"), this->tm_.get("int"));
}

TEST_F(TypeTest, GetsNonExistingEnumerator) {
  EXPECT_THROW_WITH_MESSAGE({
    this->any_->getEnumerator("a");
  }, "tried to get a member of non-enum");

  EXPECT_THROW_WITH_MESSAGE({
    this->enum_->getEnumerator("White");
  }, "tried to get non-existing enum member");
}

TEST_F(TypeTest, GetsNonExistingProp) {
  EXPECT_THROW_WITH_MESSAGE({
    this->any_->getProp("a");
  }, "tried to get non-existing prop type");

  EXPECT_THROW_WITH_MESSAGE({
    this->arr_->getProp("a");
  }, "tried to get non-existing prop type");

  EXPECT_THROW_WITH_MESSAGE({
    this->enum_->getProp("a");
  }, "tried to get non-existing prop type");

  EXPECT_THROW_WITH_MESSAGE({
    this->fn_->getProp("a");
  }, "tried to get non-existing prop type");

  EXPECT_THROW_WITH_MESSAGE({
    this->obj_->getProp("b");
  }, "tried to get non-existing prop type");

  EXPECT_THROW_WITH_MESSAGE({
    this->opt_->getProp("b");
  }, "tried to get non-existing prop type");

  EXPECT_THROW_WITH_MESSAGE({
    this->ref_->getProp("a");
  }, "tried to get non-existing prop type");

  EXPECT_THROW_WITH_MESSAGE({
    this->tm_.get("int")->getProp("a");
  }, "tried to get non-existing prop type");

  EXPECT_THROW_WITH_MESSAGE({
    this->tm_.get("str")->getProp("a");
  }, "tried to get non-existing prop type");
}

TEST_F(TypeTest, HasEnumerator) {
  EXPECT_TRUE(this->enum_->hasEnumerator("Red"));
}

TEST_F(TypeTest, HasProp) {
  EXPECT_TRUE(this->any_->hasProp("str"));
  EXPECT_TRUE(this->arr_->hasProp("str"));
  EXPECT_TRUE(this->enum_->hasProp("str"));
  EXPECT_TRUE(this->obj_->hasProp("a"));
  EXPECT_TRUE(this->opt_->hasProp("str"));
  EXPECT_TRUE(this->ref_->hasProp("str"));
  EXPECT_TRUE(this->tm_.get("int")->hasProp("str"));
  EXPECT_TRUE(this->tm_.get("str")->hasProp("len"));
}

TEST_F(TypeTest, HasNonExistingEnumerator) {
  EXPECT_FALSE(this->any_->hasEnumerator("White"));
  EXPECT_FALSE(this->enum_->hasEnumerator("White"));
}

TEST_F(TypeTest, HasNonExistingProp) {
  EXPECT_FALSE(this->any_->hasProp("a"));
  EXPECT_FALSE(this->arr_->hasProp("a"));
  EXPECT_FALSE(this->enum_->hasProp("a"));
  EXPECT_FALSE(this->fn_->hasProp("a"));
  EXPECT_FALSE(this->obj_->hasProp("b"));
  EXPECT_FALSE(this->opt_->hasProp("b"));
  EXPECT_FALSE(this->ref_->hasProp("a"));
  EXPECT_FALSE(this->tm_.get("int")->hasProp("a"));
  EXPECT_FALSE(this->tm_.get("str")->hasProp("a"));
}

TEST_F(TypeTest, CheckIfAny) {
  EXPECT_TRUE(this->any_->isAny());
  EXPECT_TRUE(this->tm_.get("any")->isAny());
}

TEST_F(TypeTest, CheckIfArray) {
  EXPECT_TRUE(this->arr_->isArray());
  EXPECT_TRUE(this->tm_.arrayOf(this->tm_.get("int"))->isArray());
}

TEST_F(TypeTest, CheckIfNotArray) {
  EXPECT_FALSE(this->any_->isArray());
  EXPECT_FALSE(this->enum_->isArray());
  EXPECT_FALSE(this->fn_->isArray());
  EXPECT_FALSE(this->obj_->isArray());
  EXPECT_FALSE(this->opt_->isArray());
  EXPECT_FALSE(this->ref_->isArray());

  EXPECT_FALSE(this->tm_.get("any")->isArray());
  EXPECT_FALSE(this->tm_.get("bool")->isArray());
  EXPECT_FALSE(this->tm_.get("byte")->isArray());
  EXPECT_FALSE(this->tm_.get("char")->isArray());
  EXPECT_FALSE(this->tm_.get("f32")->isArray());
  EXPECT_FALSE(this->tm_.get("f64")->isArray());
  EXPECT_FALSE(this->tm_.get("float")->isArray());
  EXPECT_FALSE(this->tm_.get("i8")->isArray());
  EXPECT_FALSE(this->tm_.get("i16")->isArray());
  EXPECT_FALSE(this->tm_.get("i32")->isArray());
  EXPECT_FALSE(this->tm_.get("i64")->isArray());
  EXPECT_FALSE(this->tm_.get("int")->isArray());
  EXPECT_FALSE(this->tm_.get("str")->isArray());
  EXPECT_FALSE(this->tm_.get("u8")->isArray());
  EXPECT_FALSE(this->tm_.get("u16")->isArray());
  EXPECT_FALSE(this->tm_.get("u32")->isArray());
  EXPECT_FALSE(this->tm_.get("u64")->isArray());
  EXPECT_FALSE(this->tm_.get("void")->isArray());
}

TEST_F(TypeTest, CheckIfBool) {
  EXPECT_TRUE(this->tm_.get("bool")->isBool());
}

TEST_F(TypeTest, CheckIfByte) {
  EXPECT_TRUE(this->tm_.get("byte")->isByte());
}

TEST_F(TypeTest, CheckIfChar) {
  EXPECT_TRUE(this->tm_.get("char")->isChar());
}

TEST_F(TypeTest, CheckIfEnum) {
  EXPECT_TRUE(this->enum_->isEnum());

  this->tm_.stack.emplace_back("Test2");
  EXPECT_TRUE(this->tm_.enumeration("Test2", this->tm_.name("Test2"), {
    this->tm_.enumerator("Red")
  })->isEnum());
  this->tm_.stack.pop_back();
}

TEST_F(TypeTest, CheckIfNotEnum) {
  EXPECT_FALSE(this->any_->isEnum());
  EXPECT_FALSE(this->arr_->isEnum());
  EXPECT_FALSE(this->fn_->isEnum());
  EXPECT_FALSE(this->obj_->isEnum());
  EXPECT_FALSE(this->opt_->isEnum());
  EXPECT_FALSE(this->ref_->isEnum());

  EXPECT_FALSE(this->tm_.get("any")->isEnum());
  EXPECT_FALSE(this->tm_.get("bool")->isEnum());
  EXPECT_FALSE(this->tm_.get("byte")->isEnum());
  EXPECT_FALSE(this->tm_.get("char")->isEnum());
  EXPECT_FALSE(this->tm_.get("f32")->isEnum());
  EXPECT_FALSE(this->tm_.get("f64")->isEnum());
  EXPECT_FALSE(this->tm_.get("float")->isEnum());
  EXPECT_FALSE(this->tm_.get("i8")->isEnum());
  EXPECT_FALSE(this->tm_.get("i16")->isEnum());
  EXPECT_FALSE(this->tm_.get("i32")->isEnum());
  EXPECT_FALSE(this->tm_.get("i64")->isEnum());
  EXPECT_FALSE(this->tm_.get("int")->isEnum());
  EXPECT_FALSE(this->tm_.get("str")->isEnum());
  EXPECT_FALSE(this->tm_.get("u8")->isEnum());
  EXPECT_FALSE(this->tm_.get("u16")->isEnum());
  EXPECT_FALSE(this->tm_.get("u32")->isEnum());
  EXPECT_FALSE(this->tm_.get("u64")->isEnum());
  EXPECT_FALSE(this->tm_.get("void")->isEnum());
}

TEST_F(TypeTest, CheckIfF32) {
  EXPECT_TRUE(this->tm_.get("f32")->isF32());
}

TEST_F(TypeTest, CheckIfF64) {
  EXPECT_TRUE(this->tm_.get("f64")->isF64());
}

TEST_F(TypeTest, CheckIfFloat) {
  EXPECT_TRUE(this->tm_.get("float")->isFloat());
}

TEST_F(TypeTest, CheckIfFloatNumber) {
  EXPECT_TRUE(this->tm_.get("f32")->isFloatNumber());
  EXPECT_TRUE(this->tm_.get("f64")->isFloatNumber());
  EXPECT_TRUE(this->tm_.get("float")->isFloatNumber());
}

TEST_F(TypeTest, CheckIfNotFloatNumber) {
  EXPECT_FALSE(this->any_->isFloatNumber());
  EXPECT_FALSE(this->arr_->isFloatNumber());
  EXPECT_FALSE(this->enum_->isFloatNumber());
  EXPECT_FALSE(this->fn_->isFloatNumber());
  EXPECT_FALSE(this->obj_->isFloatNumber());
  EXPECT_FALSE(this->opt_->isFloatNumber());
  EXPECT_FALSE(this->ref_->isFloatNumber());

  EXPECT_FALSE(this->tm_.get("i8")->isFloatNumber());
  EXPECT_FALSE(this->tm_.get("i16")->isFloatNumber());
  EXPECT_FALSE(this->tm_.get("i32")->isFloatNumber());
  EXPECT_FALSE(this->tm_.get("int")->isFloatNumber());
  EXPECT_FALSE(this->tm_.get("i64")->isFloatNumber());
  EXPECT_FALSE(this->tm_.get("u8")->isFloatNumber());
  EXPECT_FALSE(this->tm_.get("u16")->isFloatNumber());
  EXPECT_FALSE(this->tm_.get("u32")->isFloatNumber());
  EXPECT_FALSE(this->tm_.get("u64")->isFloatNumber());
  EXPECT_FALSE(this->tm_.get("any")->isFloatNumber());
  EXPECT_FALSE(this->tm_.get("bool")->isFloatNumber());
  EXPECT_FALSE(this->tm_.get("byte")->isFloatNumber());
  EXPECT_FALSE(this->tm_.get("char")->isFloatNumber());
  EXPECT_FALSE(this->tm_.get("str")->isFloatNumber());
  EXPECT_FALSE(this->tm_.get("void")->isFloatNumber());
}

TEST_F(TypeTest, CheckIfFn) {
  auto type1 = this->tm_.fn("test1_0", {}, this->tm_.get("int"));

  auto type2 = this->tm_.fn("test2_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("int"));

  auto type3 = this->tm_.fn("test3_0", {
    TypeFnParam{"a", this->tm_.get("str"), false, false, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("str"));

  auto type4 = this->tm_.fn(std::nullopt, {}, this->tm_.get("void"));

  auto type5 = this->tm_.fn(std::nullopt, {
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"));

  auto type6 = this->tm_.fn(std::nullopt, {
    TypeFnParam{std::nullopt, this->tm_.get("str"), false, false, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("str"));

  EXPECT_TRUE(this->fn_->isFn());
  EXPECT_TRUE(type1->isFn());
  EXPECT_TRUE(type2->isFn());
  EXPECT_TRUE(type3->isFn());
  EXPECT_TRUE(type4->isFn());
  EXPECT_TRUE(type5->isFn());
  EXPECT_TRUE(type6->isFn());
}

TEST_F(TypeTest, CheckIfNotFn) {
  EXPECT_FALSE(this->any_->isFn());
  EXPECT_FALSE(this->arr_->isFn());
  EXPECT_FALSE(this->enum_->isFn());
  EXPECT_FALSE(this->obj_->isFn());
  EXPECT_FALSE(this->opt_->isFn());
  EXPECT_FALSE(this->ref_->isFn());

  EXPECT_FALSE(this->tm_.get("any")->isFn());
  EXPECT_FALSE(this->tm_.get("bool")->isFn());
  EXPECT_FALSE(this->tm_.get("byte")->isFn());
  EXPECT_FALSE(this->tm_.get("char")->isFn());
  EXPECT_FALSE(this->tm_.get("f32")->isFn());
  EXPECT_FALSE(this->tm_.get("f64")->isFn());
  EXPECT_FALSE(this->tm_.get("float")->isFn());
  EXPECT_FALSE(this->tm_.get("i8")->isFn());
  EXPECT_FALSE(this->tm_.get("i16")->isFn());
  EXPECT_FALSE(this->tm_.get("i32")->isFn());
  EXPECT_FALSE(this->tm_.get("i64")->isFn());
  EXPECT_FALSE(this->tm_.get("int")->isFn());
  EXPECT_FALSE(this->tm_.get("str")->isFn());
  EXPECT_FALSE(this->tm_.get("u8")->isFn());
  EXPECT_FALSE(this->tm_.get("u16")->isFn());
  EXPECT_FALSE(this->tm_.get("u32")->isFn());
  EXPECT_FALSE(this->tm_.get("u64")->isFn());
  EXPECT_FALSE(this->tm_.get("void")->isFn());
}

TEST_F(TypeTest, CheckIfI8) {
  EXPECT_TRUE(this->tm_.get("i8")->isI8());
}

TEST_F(TypeTest, CheckIfI16) {
  EXPECT_TRUE(this->tm_.get("i16")->isI16());
}

TEST_F(TypeTest, CheckIfI32) {
  EXPECT_TRUE(this->tm_.get("i32")->isI32());
}

TEST_F(TypeTest, CheckIfI64) {
  EXPECT_TRUE(this->tm_.get("i64")->isI64());
}

TEST_F(TypeTest, CheckIfInt) {
  EXPECT_TRUE(this->tm_.get("int")->isInt());
}

TEST_F(TypeTest, CheckIfIntNumber) {
  EXPECT_TRUE(this->tm_.get("i8")->isIntNumber());
  EXPECT_TRUE(this->tm_.get("i16")->isIntNumber());
  EXPECT_TRUE(this->tm_.get("i32")->isIntNumber());
  EXPECT_TRUE(this->tm_.get("int")->isIntNumber());
  EXPECT_TRUE(this->tm_.get("i64")->isIntNumber());
  EXPECT_TRUE(this->tm_.get("u8")->isIntNumber());
  EXPECT_TRUE(this->tm_.get("u16")->isIntNumber());
  EXPECT_TRUE(this->tm_.get("u32")->isIntNumber());
  EXPECT_TRUE(this->tm_.get("u64")->isIntNumber());
}

TEST_F(TypeTest, CheckIfNotIntNumber) {
  EXPECT_FALSE(this->any_->isIntNumber());
  EXPECT_FALSE(this->arr_->isIntNumber());
  EXPECT_FALSE(this->enum_->isIntNumber());
  EXPECT_FALSE(this->fn_->isIntNumber());
  EXPECT_FALSE(this->obj_->isIntNumber());
  EXPECT_FALSE(this->opt_->isIntNumber());
  EXPECT_FALSE(this->ref_->isIntNumber());

  EXPECT_FALSE(this->tm_.get("f32")->isIntNumber());
  EXPECT_FALSE(this->tm_.get("f64")->isIntNumber());
  EXPECT_FALSE(this->tm_.get("float")->isIntNumber());
  EXPECT_FALSE(this->tm_.get("any")->isIntNumber());
  EXPECT_FALSE(this->tm_.get("bool")->isIntNumber());
  EXPECT_FALSE(this->tm_.get("byte")->isIntNumber());
  EXPECT_FALSE(this->tm_.get("char")->isIntNumber());
  EXPECT_FALSE(this->tm_.get("str")->isIntNumber());
  EXPECT_FALSE(this->tm_.get("void")->isIntNumber());
}

TEST_F(TypeTest, CheckIfMethod) {
  EXPECT_TRUE(this->tm_.get("Test")->getProp("m")->isMethod());
}

TEST_F(TypeTest, CheckIfNotMethod) {
  EXPECT_FALSE(this->any_->isMethod());
  EXPECT_FALSE(this->arr_->isMethod());
  EXPECT_FALSE(this->enum_->isMethod());
  EXPECT_FALSE(this->fn_->isMethod());
  EXPECT_FALSE(this->obj_->isMethod());
  EXPECT_FALSE(this->opt_->isMethod());
  EXPECT_FALSE(this->ref_->isMethod());

  EXPECT_FALSE(this->tm_.get("i8")->isMethod());
  EXPECT_FALSE(this->tm_.get("i16")->isMethod());
  EXPECT_FALSE(this->tm_.get("i32")->isMethod());
  EXPECT_FALSE(this->tm_.get("int")->isMethod());
  EXPECT_FALSE(this->tm_.get("i64")->isMethod());
  EXPECT_FALSE(this->tm_.get("f32")->isMethod());
  EXPECT_FALSE(this->tm_.get("f64")->isMethod());
  EXPECT_FALSE(this->tm_.get("float")->isMethod());
  EXPECT_FALSE(this->tm_.get("any")->isMethod());
  EXPECT_FALSE(this->tm_.get("bool")->isMethod());
  EXPECT_FALSE(this->tm_.get("byte")->isMethod());
  EXPECT_FALSE(this->tm_.get("char")->isMethod());
  EXPECT_FALSE(this->tm_.get("str")->isMethod());
  EXPECT_FALSE(this->tm_.get("u8")->isMethod());
  EXPECT_FALSE(this->tm_.get("u16")->isMethod());
  EXPECT_FALSE(this->tm_.get("u32")->isMethod());
  EXPECT_FALSE(this->tm_.get("u64")->isMethod());
  EXPECT_FALSE(this->tm_.get("void")->isMethod());
}

TEST_F(TypeTest, CheckIfNumber) {
  EXPECT_TRUE(this->tm_.get("f32")->isNumber());
  EXPECT_TRUE(this->tm_.get("f64")->isNumber());
  EXPECT_TRUE(this->tm_.get("float")->isNumber());
  EXPECT_TRUE(this->tm_.get("i8")->isNumber());
  EXPECT_TRUE(this->tm_.get("i16")->isNumber());
  EXPECT_TRUE(this->tm_.get("i32")->isNumber());
  EXPECT_TRUE(this->tm_.get("int")->isNumber());
  EXPECT_TRUE(this->tm_.get("i64")->isNumber());
  EXPECT_TRUE(this->tm_.get("u8")->isNumber());
  EXPECT_TRUE(this->tm_.get("u16")->isNumber());
  EXPECT_TRUE(this->tm_.get("u32")->isNumber());
  EXPECT_TRUE(this->tm_.get("u64")->isNumber());
}

TEST_F(TypeTest, CheckIfNotNumber) {
  EXPECT_FALSE(this->any_->isNumber());
  EXPECT_FALSE(this->arr_->isNumber());
  EXPECT_FALSE(this->enum_->isNumber());
  EXPECT_FALSE(this->fn_->isNumber());
  EXPECT_FALSE(this->obj_->isNumber());
  EXPECT_FALSE(this->opt_->isNumber());
  EXPECT_FALSE(this->ref_->isNumber());

  EXPECT_FALSE(this->tm_.get("any")->isNumber());
  EXPECT_FALSE(this->tm_.get("bool")->isNumber());
  EXPECT_FALSE(this->tm_.get("byte")->isNumber());
  EXPECT_FALSE(this->tm_.get("char")->isNumber());
  EXPECT_FALSE(this->tm_.get("str")->isNumber());
  EXPECT_FALSE(this->tm_.get("void")->isNumber());
}

TEST_F(TypeTest, CheckIfObj) {
  auto type1 = this->tm_.obj("Test1", "Test1_0");

  auto type2 = this->tm_.obj("Test2", "Test2_0", {
    TypeField{"a", this->tm_.get("int"), false, false, false}
  });

  auto type3 = this->tm_.obj("Test3", "Test3_0", {
    TypeField{"a", this->tm_.get("int"), false, false, false},
    TypeField{"b", this->tm_.get("str"), false, false, false}
  });

  EXPECT_TRUE(this->obj_->isObj());
  EXPECT_TRUE(type1->isObj());
  EXPECT_TRUE(type2->isObj());
  EXPECT_TRUE(type3->isObj());
}

TEST_F(TypeTest, CheckIfNotObj) {
  EXPECT_FALSE(this->any_->isObj());
  EXPECT_FALSE(this->arr_->isObj());
  EXPECT_FALSE(this->enum_->isObj());
  EXPECT_FALSE(this->fn_->isObj());
  EXPECT_FALSE(this->opt_->isObj());
  EXPECT_FALSE(this->ref_->isObj());

  EXPECT_FALSE(this->tm_.get("any")->isObj());
  EXPECT_FALSE(this->tm_.get("bool")->isObj());
  EXPECT_FALSE(this->tm_.get("byte")->isObj());
  EXPECT_FALSE(this->tm_.get("char")->isObj());
  EXPECT_FALSE(this->tm_.get("f32")->isObj());
  EXPECT_FALSE(this->tm_.get("f64")->isObj());
  EXPECT_FALSE(this->tm_.get("float")->isObj());
  EXPECT_FALSE(this->tm_.get("i8")->isObj());
  EXPECT_FALSE(this->tm_.get("i16")->isObj());
  EXPECT_FALSE(this->tm_.get("i32")->isObj());
  EXPECT_FALSE(this->tm_.get("i64")->isObj());
  EXPECT_FALSE(this->tm_.get("int")->isObj());
  EXPECT_FALSE(this->tm_.get("str")->isObj());
  EXPECT_FALSE(this->tm_.get("u8")->isObj());
  EXPECT_FALSE(this->tm_.get("u16")->isObj());
  EXPECT_FALSE(this->tm_.get("u32")->isObj());
  EXPECT_FALSE(this->tm_.get("u64")->isObj());
  EXPECT_FALSE(this->tm_.get("void")->isObj());
}

TEST_F(TypeTest, CheckIfOptional) {
  EXPECT_TRUE(this->opt_->isOpt());
}

TEST_F(TypeTest, CheckIfNotOptional) {
  EXPECT_FALSE(this->any_->isOpt());
  EXPECT_FALSE(this->arr_->isOpt());
  EXPECT_FALSE(this->enum_->isOpt());
  EXPECT_FALSE(this->fn_->isOpt());
  EXPECT_FALSE(this->obj_->isOpt());
  EXPECT_FALSE(this->ref_->isOpt());

  EXPECT_FALSE(this->tm_.get("any")->isOpt());
  EXPECT_FALSE(this->tm_.get("bool")->isOpt());
  EXPECT_FALSE(this->tm_.get("byte")->isOpt());
  EXPECT_FALSE(this->tm_.get("char")->isOpt());
  EXPECT_FALSE(this->tm_.get("f32")->isOpt());
  EXPECT_FALSE(this->tm_.get("f64")->isOpt());
  EXPECT_FALSE(this->tm_.get("float")->isOpt());
  EXPECT_FALSE(this->tm_.get("i8")->isOpt());
  EXPECT_FALSE(this->tm_.get("i16")->isOpt());
  EXPECT_FALSE(this->tm_.get("i32")->isOpt());
  EXPECT_FALSE(this->tm_.get("i64")->isOpt());
  EXPECT_FALSE(this->tm_.get("int")->isOpt());
  EXPECT_FALSE(this->tm_.get("str")->isOpt());
  EXPECT_FALSE(this->tm_.get("u8")->isOpt());
  EXPECT_FALSE(this->tm_.get("u16")->isOpt());
  EXPECT_FALSE(this->tm_.get("u32")->isOpt());
  EXPECT_FALSE(this->tm_.get("u64")->isOpt());
  EXPECT_FALSE(this->tm_.get("void")->isOpt());
}

TEST_F(TypeTest, CheckIfRef) {
  EXPECT_TRUE(this->ref_->isRef());
}

TEST_F(TypeTest, CheckIfNotRef) {
  EXPECT_FALSE(this->any_->isRef());
  EXPECT_FALSE(this->arr_->isRef());
  EXPECT_FALSE(this->enum_->isRef());
  EXPECT_FALSE(this->fn_->isRef());
  EXPECT_FALSE(this->obj_->isRef());
  EXPECT_FALSE(this->opt_->isRef());

  EXPECT_FALSE(this->tm_.get("any")->isRef());
  EXPECT_FALSE(this->tm_.get("bool")->isRef());
  EXPECT_FALSE(this->tm_.get("byte")->isRef());
  EXPECT_FALSE(this->tm_.get("char")->isRef());
  EXPECT_FALSE(this->tm_.get("f32")->isRef());
  EXPECT_FALSE(this->tm_.get("f64")->isRef());
  EXPECT_FALSE(this->tm_.get("float")->isRef());
  EXPECT_FALSE(this->tm_.get("i8")->isRef());
  EXPECT_FALSE(this->tm_.get("i16")->isRef());
  EXPECT_FALSE(this->tm_.get("i32")->isRef());
  EXPECT_FALSE(this->tm_.get("i64")->isRef());
  EXPECT_FALSE(this->tm_.get("int")->isRef());
  EXPECT_FALSE(this->tm_.get("str")->isRef());
  EXPECT_FALSE(this->tm_.get("u8")->isRef());
  EXPECT_FALSE(this->tm_.get("u16")->isRef());
  EXPECT_FALSE(this->tm_.get("u32")->isRef());
  EXPECT_FALSE(this->tm_.get("u64")->isRef());
  EXPECT_FALSE(this->tm_.get("void")->isRef());
}

TEST_F(TypeTest, CheckIfRefExt) {
  auto type1 = this->tm_.opt(this->ref_);

  EXPECT_TRUE(this->ref_->isRefExt());
  EXPECT_TRUE(type1->isRefExt());
}

TEST_F(TypeTest, CheckIfNotRefExt) {
  EXPECT_FALSE(this->any_->isRefExt());
  EXPECT_FALSE(this->arr_->isRefExt());
  EXPECT_FALSE(this->enum_->isRefExt());
  EXPECT_FALSE(this->fn_->isRefExt());
  EXPECT_FALSE(this->obj_->isRefExt());

  EXPECT_FALSE(this->tm_.get("any")->isRefExt());
  EXPECT_FALSE(this->tm_.get("bool")->isRefExt());
  EXPECT_FALSE(this->tm_.get("byte")->isRefExt());
  EXPECT_FALSE(this->tm_.get("char")->isRefExt());
  EXPECT_FALSE(this->tm_.get("f32")->isRefExt());
  EXPECT_FALSE(this->tm_.get("f64")->isRefExt());
  EXPECT_FALSE(this->tm_.get("float")->isRefExt());
  EXPECT_FALSE(this->tm_.get("i8")->isRefExt());
  EXPECT_FALSE(this->tm_.get("i16")->isRefExt());
  EXPECT_FALSE(this->tm_.get("i32")->isRefExt());
  EXPECT_FALSE(this->tm_.get("i64")->isRefExt());
  EXPECT_FALSE(this->tm_.get("int")->isRefExt());
  EXPECT_FALSE(this->tm_.get("str")->isRefExt());
  EXPECT_FALSE(this->tm_.get("u8")->isRefExt());
  EXPECT_FALSE(this->tm_.get("u16")->isRefExt());
  EXPECT_FALSE(this->tm_.get("u32")->isRefExt());
  EXPECT_FALSE(this->tm_.get("u64")->isRefExt());
  EXPECT_FALSE(this->tm_.get("void")->isRefExt());
}

TEST_F(TypeTest, CheckIfSmallForVarArg) {
  EXPECT_FALSE(this->any_->isSmallForVarArg());
  EXPECT_FALSE(this->arr_->isSmallForVarArg());
  EXPECT_FALSE(this->enum_->isSmallForVarArg());
  EXPECT_FALSE(this->fn_->isSmallForVarArg());
  EXPECT_FALSE(this->obj_->isSmallForVarArg());
  EXPECT_FALSE(this->opt_->isSmallForVarArg());
  EXPECT_FALSE(this->ref_->isSmallForVarArg());

  EXPECT_FALSE(this->tm_.get("any")->isSmallForVarArg());
  EXPECT_TRUE(this->tm_.get("bool")->isSmallForVarArg());
  EXPECT_TRUE(this->tm_.get("byte")->isSmallForVarArg());
  EXPECT_TRUE(this->tm_.get("char")->isSmallForVarArg());
  EXPECT_TRUE(this->tm_.get("f32")->isSmallForVarArg());
  EXPECT_FALSE(this->tm_.get("f64")->isSmallForVarArg());
  EXPECT_FALSE(this->tm_.get("float")->isSmallForVarArg());
  EXPECT_TRUE(this->tm_.get("i8")->isSmallForVarArg());
  EXPECT_TRUE(this->tm_.get("i16")->isSmallForVarArg());
  EXPECT_FALSE(this->tm_.get("i32")->isSmallForVarArg());
  EXPECT_FALSE(this->tm_.get("i64")->isSmallForVarArg());
  EXPECT_FALSE(this->tm_.get("int")->isSmallForVarArg());
  EXPECT_FALSE(this->tm_.get("str")->isSmallForVarArg());
  EXPECT_TRUE(this->tm_.get("u8")->isSmallForVarArg());
  EXPECT_TRUE(this->tm_.get("u16")->isSmallForVarArg());
  EXPECT_FALSE(this->tm_.get("u32")->isSmallForVarArg());
  EXPECT_FALSE(this->tm_.get("u64")->isSmallForVarArg());
  EXPECT_FALSE(this->tm_.get("void")->isSmallForVarArg());
}

TEST_F(TypeTest, CheckIfStr) {
  EXPECT_TRUE(this->tm_.get("str")->isStr());
}

TEST_F(TypeTest, CheckIfU8) {
  EXPECT_TRUE(this->tm_.get("u8")->isU8());
}

TEST_F(TypeTest, CheckIfU16) {
  EXPECT_TRUE(this->tm_.get("u16")->isU16());
}

TEST_F(TypeTest, CheckIfU32) {
  EXPECT_TRUE(this->tm_.get("u32")->isU32());
}

TEST_F(TypeTest, CheckIfU64) {
  EXPECT_TRUE(this->tm_.get("u64")->isU64());
}

TEST_F(TypeTest, CheckIfVoid) {
  EXPECT_TRUE(this->tm_.get("void")->isVoid());
}

TEST_F(TypeTest, Matches) {
  EXPECT_TRUE(this->any_->match(this->any_));
  EXPECT_TRUE(this->arr_->match(this->arr_));
  EXPECT_TRUE(this->enum_->match(this->enum_));
  EXPECT_TRUE(this->fn_->match(this->fn_));
  EXPECT_TRUE(this->obj_->match(this->obj_));
  EXPECT_TRUE(this->opt_->match(this->opt_));
  EXPECT_TRUE(this->ref_->match(this->ref_));

  EXPECT_TRUE(this->tm_.get("bool")->match(this->tm_.get("bool")));
  EXPECT_TRUE(this->tm_.get("byte")->match(this->tm_.get("byte")));
  EXPECT_TRUE(this->tm_.get("byte")->match(this->tm_.get("int")));
  EXPECT_TRUE(this->tm_.get("int")->match(this->tm_.get("byte")));
  EXPECT_TRUE(this->tm_.get("char")->match(this->tm_.get("char")));
  EXPECT_TRUE(this->tm_.get("str")->match(this->tm_.get("str")));
  EXPECT_TRUE(this->tm_.get("void")->match(this->tm_.get("void")));
}

TEST_F(TypeTest, MatchesAny) {
  EXPECT_TRUE(this->tm_.get("any")->match(this->any_));
  EXPECT_TRUE(this->tm_.get("any")->match(this->arr_));
  EXPECT_TRUE(this->tm_.get("any")->match(this->enum_));
  EXPECT_TRUE(this->tm_.get("any")->match(this->fn_));
  EXPECT_TRUE(this->tm_.get("any")->match(this->obj_));
  EXPECT_TRUE(this->tm_.get("any")->match(this->opt_));
  EXPECT_TRUE(this->tm_.get("any")->match(this->ref_));

  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("any")));
  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("bool")));
  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("byte")));
  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("char")));
  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("float")));
  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("f32")));
  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("f64")));
  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("int")));
  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("i16")));
  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("i32")));
  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("str")));
  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("u16")));
  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("u32")));
  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("u64")));
  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("void")));
}

TEST_F(TypeTest, MatchesFloat) {
  EXPECT_TRUE(this->tm_.get("f32")->match(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("f32")->match(this->tm_.get("i16")));
  EXPECT_TRUE(this->tm_.get("f32")->match(this->tm_.get("i32")));
  EXPECT_TRUE(this->tm_.get("f32")->match(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("f32")->match(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("f32")->match(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("f32")->match(this->tm_.get("u16")));
  EXPECT_FALSE(this->tm_.get("f32")->match(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("f32")->match(this->tm_.get("u64")));
  EXPECT_TRUE(this->tm_.get("f32")->match(this->tm_.get("f32")));
  EXPECT_FALSE(this->tm_.get("f32")->match(this->tm_.get("f64")));
  EXPECT_FALSE(this->tm_.get("f32")->match(this->tm_.get("float")));
  EXPECT_TRUE(this->tm_.get("f64")->match(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("f64")->match(this->tm_.get("i16")));
  EXPECT_TRUE(this->tm_.get("f64")->match(this->tm_.get("i32")));
  EXPECT_TRUE(this->tm_.get("f64")->match(this->tm_.get("int")));
  EXPECT_TRUE(this->tm_.get("f64")->match(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("f64")->match(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("f64")->match(this->tm_.get("u16")));
  EXPECT_TRUE(this->tm_.get("f64")->match(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("f64")->match(this->tm_.get("u64")));
  EXPECT_TRUE(this->tm_.get("f64")->match(this->tm_.get("f32")));
  EXPECT_TRUE(this->tm_.get("f64")->match(this->tm_.get("f64")));
  EXPECT_TRUE(this->tm_.get("f64")->match(this->tm_.get("float")));
  EXPECT_TRUE(this->tm_.get("float")->match(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("float")->match(this->tm_.get("i16")));
  EXPECT_TRUE(this->tm_.get("float")->match(this->tm_.get("i32")));
  EXPECT_TRUE(this->tm_.get("float")->match(this->tm_.get("int")));
  EXPECT_TRUE(this->tm_.get("float")->match(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("float")->match(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("float")->match(this->tm_.get("u16")));
  EXPECT_TRUE(this->tm_.get("float")->match(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("float")->match(this->tm_.get("u64")));
  EXPECT_TRUE(this->tm_.get("float")->match(this->tm_.get("f32")));
  EXPECT_TRUE(this->tm_.get("float")->match(this->tm_.get("f64")));
  EXPECT_TRUE(this->tm_.get("float")->match(this->tm_.get("float")));
}

TEST_F(TypeTest, MatchesInteger) {
  EXPECT_TRUE(this->tm_.get("i8")->match(this->tm_.get("i8")));
  EXPECT_FALSE(this->tm_.get("i8")->match(this->tm_.get("i16")));
  EXPECT_FALSE(this->tm_.get("i8")->match(this->tm_.get("i32")));
  EXPECT_FALSE(this->tm_.get("i8")->match(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("i8")->match(this->tm_.get("i64")));
  EXPECT_FALSE(this->tm_.get("i8")->match(this->tm_.get("u8")));
  EXPECT_FALSE(this->tm_.get("i8")->match(this->tm_.get("u16")));
  EXPECT_FALSE(this->tm_.get("i8")->match(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("i8")->match(this->tm_.get("u64")));
  EXPECT_FALSE(this->tm_.get("i8")->match(this->tm_.get("f32")));
  EXPECT_FALSE(this->tm_.get("i8")->match(this->tm_.get("f64")));
  EXPECT_FALSE(this->tm_.get("i8")->match(this->tm_.get("float")));

  EXPECT_TRUE(this->tm_.get("i16")->match(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("i16")->match(this->tm_.get("i16")));
  EXPECT_FALSE(this->tm_.get("i16")->match(this->tm_.get("i32")));
  EXPECT_FALSE(this->tm_.get("i16")->match(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("i16")->match(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("i16")->match(this->tm_.get("u8")));
  EXPECT_FALSE(this->tm_.get("i16")->match(this->tm_.get("u16")));
  EXPECT_FALSE(this->tm_.get("i16")->match(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("i16")->match(this->tm_.get("u64")));
  EXPECT_FALSE(this->tm_.get("i16")->match(this->tm_.get("f32")));
  EXPECT_FALSE(this->tm_.get("i16")->match(this->tm_.get("f64")));
  EXPECT_FALSE(this->tm_.get("i16")->match(this->tm_.get("float")));

  EXPECT_TRUE(this->tm_.get("i32")->match(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("i32")->match(this->tm_.get("i16")));
  EXPECT_TRUE(this->tm_.get("i32")->match(this->tm_.get("i32")));
  EXPECT_TRUE(this->tm_.get("i32")->match(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("i32")->match(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("i32")->match(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("i32")->match(this->tm_.get("u16")));
  EXPECT_FALSE(this->tm_.get("i32")->match(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("i32")->match(this->tm_.get("u64")));
  EXPECT_FALSE(this->tm_.get("i32")->match(this->tm_.get("f32")));
  EXPECT_FALSE(this->tm_.get("i32")->match(this->tm_.get("f64")));
  EXPECT_FALSE(this->tm_.get("i32")->match(this->tm_.get("float")));

  EXPECT_TRUE(this->tm_.get("int")->match(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("int")->match(this->tm_.get("i16")));
  EXPECT_TRUE(this->tm_.get("int")->match(this->tm_.get("i32")));
  EXPECT_TRUE(this->tm_.get("int")->match(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->match(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("int")->match(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("int")->match(this->tm_.get("u16")));
  EXPECT_FALSE(this->tm_.get("int")->match(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("int")->match(this->tm_.get("u64")));
  EXPECT_FALSE(this->tm_.get("int")->match(this->tm_.get("f32")));
  EXPECT_FALSE(this->tm_.get("int")->match(this->tm_.get("f64")));
  EXPECT_FALSE(this->tm_.get("int")->match(this->tm_.get("float")));

  EXPECT_TRUE(this->tm_.get("i64")->match(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("i64")->match(this->tm_.get("i16")));
  EXPECT_TRUE(this->tm_.get("i64")->match(this->tm_.get("i32")));
  EXPECT_TRUE(this->tm_.get("i64")->match(this->tm_.get("int")));
  EXPECT_TRUE(this->tm_.get("i64")->match(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("i64")->match(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("i64")->match(this->tm_.get("u16")));
  EXPECT_TRUE(this->tm_.get("i64")->match(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("i64")->match(this->tm_.get("u64")));
  EXPECT_FALSE(this->tm_.get("i64")->match(this->tm_.get("f32")));
  EXPECT_FALSE(this->tm_.get("i64")->match(this->tm_.get("f64")));
  EXPECT_FALSE(this->tm_.get("i64")->match(this->tm_.get("float")));

  EXPECT_FALSE(this->tm_.get("u8")->match(this->tm_.get("i8")));
  EXPECT_FALSE(this->tm_.get("u8")->match(this->tm_.get("i16")));
  EXPECT_FALSE(this->tm_.get("u8")->match(this->tm_.get("i32")));
  EXPECT_FALSE(this->tm_.get("u8")->match(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("u8")->match(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("u8")->match(this->tm_.get("u8")));
  EXPECT_FALSE(this->tm_.get("u8")->match(this->tm_.get("u16")));
  EXPECT_FALSE(this->tm_.get("u8")->match(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("u8")->match(this->tm_.get("u64")));
  EXPECT_FALSE(this->tm_.get("u8")->match(this->tm_.get("f32")));
  EXPECT_FALSE(this->tm_.get("u8")->match(this->tm_.get("f64")));
  EXPECT_FALSE(this->tm_.get("u8")->match(this->tm_.get("float")));

  EXPECT_FALSE(this->tm_.get("u16")->match(this->tm_.get("i8")));
  EXPECT_FALSE(this->tm_.get("u16")->match(this->tm_.get("i16")));
  EXPECT_FALSE(this->tm_.get("u16")->match(this->tm_.get("i32")));
  EXPECT_FALSE(this->tm_.get("u16")->match(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("u16")->match(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("u16")->match(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("u16")->match(this->tm_.get("u16")));
  EXPECT_FALSE(this->tm_.get("u16")->match(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("u16")->match(this->tm_.get("u64")));
  EXPECT_FALSE(this->tm_.get("u16")->match(this->tm_.get("f32")));
  EXPECT_FALSE(this->tm_.get("u16")->match(this->tm_.get("f64")));
  EXPECT_FALSE(this->tm_.get("u16")->match(this->tm_.get("float")));

  EXPECT_FALSE(this->tm_.get("u32")->match(this->tm_.get("i8")));
  EXPECT_FALSE(this->tm_.get("u32")->match(this->tm_.get("i16")));
  EXPECT_FALSE(this->tm_.get("u32")->match(this->tm_.get("i32")));
  EXPECT_FALSE(this->tm_.get("u32")->match(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("u32")->match(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("u32")->match(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("u32")->match(this->tm_.get("u16")));
  EXPECT_TRUE(this->tm_.get("u32")->match(this->tm_.get("u32")));
  EXPECT_FALSE(this->tm_.get("u32")->match(this->tm_.get("u64")));
  EXPECT_FALSE(this->tm_.get("u32")->match(this->tm_.get("f32")));
  EXPECT_FALSE(this->tm_.get("u32")->match(this->tm_.get("f64")));
  EXPECT_FALSE(this->tm_.get("u32")->match(this->tm_.get("float")));

  EXPECT_FALSE(this->tm_.get("u64")->match(this->tm_.get("i8")));
  EXPECT_FALSE(this->tm_.get("u64")->match(this->tm_.get("i16")));
  EXPECT_FALSE(this->tm_.get("u64")->match(this->tm_.get("i32")));
  EXPECT_FALSE(this->tm_.get("u64")->match(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("u64")->match(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("u64")->match(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("u64")->match(this->tm_.get("u16")));
  EXPECT_TRUE(this->tm_.get("u64")->match(this->tm_.get("u32")));
  EXPECT_TRUE(this->tm_.get("u64")->match(this->tm_.get("u64")));
  EXPECT_FALSE(this->tm_.get("u64")->match(this->tm_.get("f32")));
  EXPECT_FALSE(this->tm_.get("u64")->match(this->tm_.get("f64")));
  EXPECT_FALSE(this->tm_.get("u64")->match(this->tm_.get("float")));
}

TEST_F(TypeTest, MatchesArray) {
  auto type1 = this->tm_.arrayOf(this->tm_.get("int"));
  auto type2 = this->tm_.arrayOf(this->tm_.get("str"));

  EXPECT_TRUE(type1->match(type1));
  EXPECT_FALSE(type1->match(type2));
  EXPECT_FALSE(type2->match(type1));
  EXPECT_FALSE(type1->match(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->match(type1));
}

TEST_F(TypeTest, MatchesEnum) {
  this->tm_.stack.emplace_back("Test2");
  auto type1 = this->tm_.enumeration("Test2", "Test2_0", {
    this->tm_.enumerator("Brown")
  });
  this->tm_.stack.pop_back();

  this->tm_.stack.emplace_back("Test2");
  auto type2 = this->tm_.enumeration("Test2", "Test2_0", {
    this->tm_.enumerator("Brown"),
    this->tm_.enumerator("Red")
  });
  this->tm_.stack.pop_back();

  this->tm_.stack.emplace_back("Test3");
  auto type3 = this->tm_.enumeration("Test3", "Test3_0", {
    this->tm_.enumerator("Brown"),
    this->tm_.enumerator("Red")
  });
  this->tm_.stack.pop_back();

  EXPECT_TRUE(type1->match(type1));
  EXPECT_TRUE(type1->match(type2));
  EXPECT_TRUE(type2->match(type1));
  EXPECT_FALSE(type1->match(type3));
  EXPECT_FALSE(type3->match(type1));
  EXPECT_TRUE(type1->match(this->tm_.get("int")));
  EXPECT_TRUE(this->tm_.get("int")->match(type1));
}

TEST_F(TypeTest, MatchesFunction) {
  auto ref1 = this->tm_.ref(this->tm_.get("int"));
  auto ref2 = this->tm_.ref(this->tm_.get("str"));

  auto type1 = this->tm_.fn("test1_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"));

  auto type2 = this->tm_.fn("test2_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"));

  auto type3 = this->tm_.fn("test3_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("int"));

  auto type4 = this->tm_.fn("test4_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("str"), false, false, true}
  }, this->tm_.get("int"));

  auto type5 = this->tm_.fn("test5_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("str"));

  auto type6 = this->tm_.fn("test6_0", {}, this->tm_.get("int"));

  auto type7 = this->tm_.fn("test7_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("int"));

  auto type8 = this->tm_.fn("test8_0", {
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"));

  auto type9 = this->tm_.fn("test9_0", {
    TypeFnParam{std::nullopt, ref1, false, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"));

  auto type10 = this->tm_.fn("test10_0", {
    TypeFnParam{std::nullopt, ref1, true, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"));

  auto type11 = this->tm_.fn("test11_0", {
    TypeFnParam{std::nullopt, ref2, true, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"));

  EXPECT_FALSE(type1->match(this->tm_.get("any")));
  EXPECT_TRUE(type1->match(type1));
  EXPECT_TRUE(type1->match(type2));
  EXPECT_FALSE(type1->match(type3));
  EXPECT_FALSE(type1->match(type4));
  EXPECT_FALSE(type1->match(type5));
  EXPECT_FALSE(type1->match(type6));
  EXPECT_FALSE(type1->match(type7));
  EXPECT_FALSE(type1->match(type8));
  EXPECT_TRUE(type8->match(type1));
  EXPECT_FALSE(type1->match(type9));
  EXPECT_TRUE(type9->match(type1));
  EXPECT_FALSE(type1->match(type10));
  EXPECT_FALSE(type10->match(type1));
  EXPECT_FALSE(type1->match(type11));
  EXPECT_FALSE(type11->match(type1));
  EXPECT_FALSE(type10->match(type11));
  EXPECT_FALSE(type11->match(type10));
}

TEST_F(TypeTest, MatchesObject) {
  auto type1 = this->tm_.obj("Test1", "Test1_0", {
    TypeField{"a", this->tm_.get("int"), false, false, false},
    TypeField{"b", this->tm_.get("int"), false, false, false}
  });

  auto type2 = this->tm_.obj("Test2", "Test2_0");

  auto type3 = this->tm_.obj("Test3", "Test3_0", {
    TypeField{"a", this->tm_.get("int"), false, false, false}
  });

  auto type4 = this->tm_.obj("Test4", "Test4_0", {
    TypeField{"a", this->tm_.get("int"), false, false, false},
    TypeField{"b", this->tm_.get("int"), false, false, false}
  });

  auto type5 = this->tm_.obj("Test5", "Test5_0", {
    TypeField{"a", this->tm_.get("int"), false, false, false},
    TypeField{"b", this->tm_.get("str"), false, false, false}
  });

  EXPECT_TRUE(type1->match(type1));
  EXPECT_FALSE(type1->match(type2));
  EXPECT_FALSE(type1->match(type3));
  EXPECT_FALSE(type1->match(type4));
  EXPECT_FALSE(type1->match(type5));
  EXPECT_FALSE(type1->match(this->tm_.get("int")));
}

TEST_F(TypeTest, MatchesObjectMethod) {
  auto type1 = this->tm_.fn("test1", {}, this->tm_.get("void"));
  auto type2MethodInfo = TypeFnMethodInfo{false, "", nullptr, false};
  auto type2 = this->tm_.fn("test2", {}, this->tm_.get("void"), type2MethodInfo);
  auto type3MethodInfo = TypeFnMethodInfo{true, "self1_0", this->obj_, false};
  auto type3 = this->tm_.fn("test3", {}, this->tm_.get("void"), type3MethodInfo);
  auto type4MethodInfo = TypeFnMethodInfo{true, "self1_0", this->obj_, true};
  auto type4 = this->tm_.fn("test4", {}, this->tm_.get("void"), type4MethodInfo);
  auto type5MethodInfo = TypeFnMethodInfo{true, "self1_0", this->tm_.ref(this->obj_), true};
  auto type5 = this->tm_.fn("test5", {}, this->tm_.get("void"), type5MethodInfo);

  EXPECT_TRUE(type2->match(type2));
  EXPECT_FALSE(type1->match(type2));

  EXPECT_TRUE(type3->match(type3));
  EXPECT_FALSE(type2->match(type3));
  EXPECT_FALSE(type3->match(type2));

  EXPECT_TRUE(type4->match(type4));
  EXPECT_TRUE(type3->match(type4));
  EXPECT_TRUE(type4->match(type3));

  EXPECT_TRUE(type5->match(type5));
  EXPECT_TRUE(type3->match(type5));
  EXPECT_TRUE(type4->match(type5));
  EXPECT_FALSE(type5->match(type3));
  EXPECT_TRUE(type5->match(type4));
}

TEST_F(TypeTest, MatchesOptional) {
  auto type1 = this->tm_.opt(this->tm_.get("int"));
  auto type2 = this->tm_.opt(this->tm_.get("str"));

  EXPECT_TRUE(type1->match(type1));
  EXPECT_FALSE(type1->match(type2));
  EXPECT_FALSE(type2->match(type1));
  EXPECT_TRUE(type1->match(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->match(type1));
}

TEST_F(TypeTest, MatchesReference) {
  auto type1 = this->tm_.ref(this->tm_.get("int"));
  auto type2 = this->tm_.ref(this->tm_.get("int"));

  EXPECT_TRUE(type1->match(type1));
  EXPECT_TRUE(type1->match(type2));
  EXPECT_TRUE(type2->match(type1));
  EXPECT_TRUE(type1->match(this->tm_.get("int")));
  EXPECT_TRUE(this->tm_.get("int")->match(type1));
  EXPECT_FALSE(type1->match(this->tm_.get("str")));
  EXPECT_FALSE(this->tm_.get("str")->match(type1));
}

TEST_F(TypeTest, MatchesExact) {
  EXPECT_TRUE(this->any_->matchExact(this->any_));
  EXPECT_TRUE(this->arr_->matchExact(this->arr_));
  EXPECT_TRUE(this->enum_->matchExact(this->enum_));
  EXPECT_TRUE(this->fn_->matchExact(this->fn_));
  EXPECT_TRUE(this->obj_->matchExact(this->obj_));
  EXPECT_TRUE(this->opt_->matchExact(this->opt_));
  EXPECT_TRUE(this->ref_->matchExact(this->ref_));

  EXPECT_TRUE(this->tm_.get("any")->matchExact(this->tm_.get("any")));
  EXPECT_TRUE(this->tm_.get("bool")->matchExact(this->tm_.get("bool")));
  EXPECT_TRUE(this->tm_.get("byte")->matchExact(this->tm_.get("byte")));
  EXPECT_TRUE(this->tm_.get("char")->matchExact(this->tm_.get("char")));
  EXPECT_TRUE(this->tm_.get("float")->matchExact(this->tm_.get("float")));
  EXPECT_TRUE(this->tm_.get("f32")->matchExact(this->tm_.get("f32")));
  EXPECT_TRUE(this->tm_.get("f64")->matchExact(this->tm_.get("f64")));
  EXPECT_TRUE(this->tm_.get("int")->matchExact(this->tm_.get("int")));
  EXPECT_TRUE(this->tm_.get("i8")->matchExact(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("i16")->matchExact(this->tm_.get("i16")));
  EXPECT_TRUE(this->tm_.get("i32")->matchExact(this->tm_.get("i32")));
  EXPECT_TRUE(this->tm_.get("i64")->matchExact(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("str")->matchExact(this->tm_.get("str")));
  EXPECT_TRUE(this->tm_.get("u8")->matchExact(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("u16")->matchExact(this->tm_.get("u16")));
  EXPECT_TRUE(this->tm_.get("u32")->matchExact(this->tm_.get("u32")));
  EXPECT_TRUE(this->tm_.get("u64")->matchExact(this->tm_.get("u64")));
  EXPECT_TRUE(this->tm_.get("void")->matchExact(this->tm_.get("void")));
}

TEST_F(TypeTest, MatchesExactArray) {
  auto type1 = this->tm_.arrayOf(this->tm_.get("int"));
  auto type2 = this->tm_.arrayOf(this->tm_.get("str"));

  EXPECT_TRUE(type1->matchExact(type1));
  EXPECT_FALSE(type1->matchExact(type2));
  EXPECT_FALSE(type2->matchExact(type1));
  EXPECT_FALSE(type1->matchExact(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchExact(type1));
}

TEST_F(TypeTest, MatchesExactEnum) {
  this->tm_.stack.emplace_back("Test2");
  auto type1 = this->tm_.enumeration("Test2", "Test2_0", {
    this->tm_.enumerator("Brown")
  });
  this->tm_.stack.pop_back();

  this->tm_.stack.emplace_back("Test2");
  auto type2 = this->tm_.enumeration("Test2", "Test2_0", {
    this->tm_.enumerator("Brown"),
    this->tm_.enumerator("Red")
  });
  this->tm_.stack.pop_back();

  this->tm_.stack.emplace_back("Test3");
  auto type3 = this->tm_.enumeration("Test3", "Test3_0", {
    this->tm_.enumerator("Brown"),
    this->tm_.enumerator("Red")
  });
  this->tm_.stack.pop_back();

  EXPECT_TRUE(type1->matchExact(type1));
  EXPECT_TRUE(type1->matchExact(type2));
  EXPECT_TRUE(type2->matchExact(type1));
  EXPECT_FALSE(type1->matchExact(type3));
  EXPECT_FALSE(type3->matchExact(type1));
  EXPECT_FALSE(type1->matchExact(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchExact(type1));
}

TEST_F(TypeTest, MatchesExactFunction) {
  auto ref1 = this->tm_.ref(this->tm_.get("int"));
  auto ref2 = this->tm_.ref(this->tm_.get("str"));

  auto type1 = this->tm_.fn("test1_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"));

  auto type2 = this->tm_.fn("test2_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"));

  auto type3 = this->tm_.fn("test3_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("int"));

  auto type4 = this->tm_.fn("test4_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("str"), false, false, true}
  }, this->tm_.get("int"));

  auto type5 = this->tm_.fn("test5_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("str"));

  auto type6 = this->tm_.fn("test6_0", {}, this->tm_.get("int"));

  auto type7 = this->tm_.fn("test7_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("int"));

  auto type8 = this->tm_.fn("test8_0", {
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"));

  auto type9 = this->tm_.fn("test9_0", {
    TypeFnParam{std::nullopt, ref1, false, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"));

  auto type10 = this->tm_.fn("test10_0", {
    TypeFnParam{std::nullopt, ref1, true, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"));

  auto type11 = this->tm_.fn("test11_0", {
    TypeFnParam{std::nullopt, ref2, true, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"));

  EXPECT_FALSE(type1->matchExact(this->tm_.get("any")));
  EXPECT_TRUE(type1->matchExact(type1));
  EXPECT_TRUE(type1->matchExact(type2));
  EXPECT_FALSE(type1->matchExact(type3));
  EXPECT_FALSE(type1->matchExact(type4));
  EXPECT_FALSE(type1->matchExact(type5));
  EXPECT_FALSE(type1->matchExact(type6));
  EXPECT_FALSE(type1->matchExact(type7));
  EXPECT_FALSE(type1->matchExact(type8));
  EXPECT_FALSE(type8->matchExact(type1));
  EXPECT_FALSE(type1->matchExact(type9));
  EXPECT_FALSE(type9->matchExact(type1));
  EXPECT_FALSE(type1->matchExact(type10));
  EXPECT_FALSE(type10->matchExact(type1));
  EXPECT_FALSE(type1->matchExact(type11));
  EXPECT_FALSE(type11->matchExact(type1));
  EXPECT_FALSE(type10->matchExact(type11));
  EXPECT_FALSE(type11->matchExact(type10));
}

TEST_F(TypeTest, MatchesExactObject) {
  auto type1 = this->tm_.obj("Test1", "Test1_0", {
    TypeField{"a", this->tm_.get("int"), false, false, false},
    TypeField{"b", this->tm_.get("int"), false, false, false}
  });

  auto type2 = this->tm_.obj("Test2", "Test2_0");

  auto type3 = this->tm_.obj("Test3", "Test3_0", {
    TypeField{"a", this->tm_.get("int"), false, false, false}
  });

  auto type4 = this->tm_.obj("Test4", "Test4_0", {
    TypeField{"a", this->tm_.get("int"), false, false, false},
    TypeField{"b", this->tm_.get("int"), false, false, false}
  });

  auto type5 = this->tm_.obj("Test5", "Test5_0", {
    TypeField{"a", this->tm_.get("int"), false, false, false},
    TypeField{"b", this->tm_.get("str"), false, false, false}
  });

  EXPECT_TRUE(type1->matchExact(type1));
  EXPECT_FALSE(type1->matchExact(type2));
  EXPECT_FALSE(type1->matchExact(type3));
  EXPECT_FALSE(type1->matchExact(type4));
  EXPECT_FALSE(type1->matchExact(type5));
  EXPECT_FALSE(type1->matchExact(this->tm_.get("int")));
}

TEST_F(TypeTest, MatchesExactObjectMethod) {
  auto type1 = this->tm_.fn("test1", {}, this->tm_.get("void"));
  auto type2MethodInfo = TypeFnMethodInfo{false, "", nullptr, false};
  auto type2 = this->tm_.fn("test2", {}, this->tm_.get("void"), type2MethodInfo);
  auto type3MethodInfo = TypeFnMethodInfo{true, "self1_0", this->obj_, false};
  auto type3 = this->tm_.fn("test3", {}, this->tm_.get("void"), type3MethodInfo);
  auto type4MethodInfo = TypeFnMethodInfo{true, "self1_0", this->obj_, true};
  auto type4 = this->tm_.fn("test4", {}, this->tm_.get("void"), type4MethodInfo);
  auto type5MethodInfo = TypeFnMethodInfo{true, "self1_0", this->tm_.ref(this->obj_), true};
  auto type5 = this->tm_.fn("test5", {}, this->tm_.get("void"), type5MethodInfo);

  EXPECT_TRUE(type2->matchExact(type2));
  EXPECT_FALSE(type1->matchExact(type2));

  EXPECT_TRUE(type3->matchExact(type3));
  EXPECT_FALSE(type2->matchExact(type3));
  EXPECT_FALSE(type3->matchExact(type2));

  EXPECT_TRUE(type4->matchExact(type4));
  EXPECT_FALSE(type3->matchExact(type4));
  EXPECT_FALSE(type4->matchExact(type3));

  EXPECT_TRUE(type5->matchExact(type5));
  EXPECT_FALSE(type3->matchExact(type5));
  EXPECT_FALSE(type4->matchExact(type5));
  EXPECT_FALSE(type5->matchExact(type3));
  EXPECT_FALSE(type5->matchExact(type4));
}

TEST_F(TypeTest, MatchesExactOptional) {
  auto type1 = this->tm_.opt(this->tm_.get("int"));
  auto type2 = this->tm_.opt(this->tm_.get("str"));

  EXPECT_TRUE(type1->matchExact(type1));
  EXPECT_FALSE(type1->matchExact(type2));
  EXPECT_FALSE(type2->matchExact(type1));
  EXPECT_FALSE(type1->matchExact(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchExact(type1));
}

TEST_F(TypeTest, MatchesExactReference) {
  auto type1 = this->tm_.ref(this->tm_.get("int"));
  auto type2 = this->tm_.ref(this->tm_.get("int"));

  EXPECT_TRUE(type1->matchExact(type1));
  EXPECT_TRUE(type1->matchExact(type2));
  EXPECT_TRUE(type2->matchExact(type1));
  EXPECT_FALSE(type1->matchExact(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchExact(type1));
  EXPECT_FALSE(type1->matchExact(this->tm_.get("str")));
  EXPECT_FALSE(this->tm_.get("str")->matchExact(type1));
}

TEST_F(TypeTest, MatchesNice) {
  EXPECT_TRUE(this->any_->matchNice(this->any_));
  EXPECT_TRUE(this->arr_->matchNice(this->arr_));
  EXPECT_TRUE(this->enum_->matchNice(this->enum_));
  EXPECT_TRUE(this->fn_->matchNice(this->fn_));
  EXPECT_TRUE(this->obj_->matchNice(this->obj_));
  EXPECT_TRUE(this->opt_->matchNice(this->opt_));
  EXPECT_TRUE(this->ref_->matchNice(this->ref_));

  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("any")));
  EXPECT_TRUE(this->tm_.get("bool")->matchNice(this->tm_.get("bool")));
  EXPECT_TRUE(this->tm_.get("byte")->matchNice(this->tm_.get("byte")));
  EXPECT_TRUE(this->tm_.get("char")->matchNice(this->tm_.get("char")));
  EXPECT_TRUE(this->tm_.get("float")->matchNice(this->tm_.get("float")));
  EXPECT_TRUE(this->tm_.get("f32")->matchNice(this->tm_.get("f32")));
  EXPECT_TRUE(this->tm_.get("f64")->matchNice(this->tm_.get("f64")));
  EXPECT_TRUE(this->tm_.get("int")->matchNice(this->tm_.get("int")));
  EXPECT_TRUE(this->tm_.get("i8")->matchNice(this->tm_.get("i8")));
  EXPECT_TRUE(this->tm_.get("i16")->matchNice(this->tm_.get("i16")));
  EXPECT_TRUE(this->tm_.get("i32")->matchNice(this->tm_.get("i32")));
  EXPECT_TRUE(this->tm_.get("i64")->matchNice(this->tm_.get("i64")));
  EXPECT_TRUE(this->tm_.get("str")->matchNice(this->tm_.get("str")));
  EXPECT_TRUE(this->tm_.get("u8")->matchNice(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("u16")->matchNice(this->tm_.get("u16")));
  EXPECT_TRUE(this->tm_.get("u32")->matchNice(this->tm_.get("u32")));
  EXPECT_TRUE(this->tm_.get("u64")->matchNice(this->tm_.get("u64")));
  EXPECT_TRUE(this->tm_.get("void")->matchNice(this->tm_.get("void")));
}

TEST_F(TypeTest, MatchesNiceArray) {
  auto type1 = this->tm_.arrayOf(this->tm_.get("int"));
  auto type2 = this->tm_.arrayOf(this->tm_.get("str"));

  EXPECT_TRUE(type1->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(type2));
  EXPECT_FALSE(type2->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchNice(type1));
}

TEST_F(TypeTest, MatchesNiceEnum) {
  this->tm_.stack.emplace_back("Test2");
  auto type1 = this->tm_.enumeration("Test2", "Test2_0", {
    this->tm_.enumerator("Brown")
  });
  this->tm_.stack.pop_back();

  this->tm_.stack.emplace_back("Test2");
  auto type2 = this->tm_.enumeration("Test2", "Test2_0", {
    this->tm_.enumerator("Brown"),
    this->tm_.enumerator("Red")
  });
  this->tm_.stack.pop_back();

  this->tm_.stack.emplace_back("Test3");
  auto type3 = this->tm_.enumeration("Test3", "Test3_0", {
    this->tm_.enumerator("Brown"),
    this->tm_.enumerator("Red")
  });
  this->tm_.stack.pop_back();

  EXPECT_TRUE(type1->matchNice(type1));
  EXPECT_TRUE(type1->matchNice(type2));
  EXPECT_TRUE(type2->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(type3));
  EXPECT_FALSE(type3->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchNice(type1));
}

TEST_F(TypeTest, MatchesNiceFunction) {
  auto ref1 = this->tm_.ref(this->tm_.get("int"));
  auto ref2 = this->tm_.ref(this->tm_.get("str"));

  auto type1 = this->tm_.fn("test1_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"));

  auto type2 = this->tm_.fn("test2_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"));

  auto type3 = this->tm_.fn("test3_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("int"));

  auto type4 = this->tm_.fn("test4_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("str"), false, false, true}
  }, this->tm_.get("int"));

  auto type5 = this->tm_.fn("test5_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("str"));

  auto type6 = this->tm_.fn("test6_0", {}, this->tm_.get("int"));

  auto type7 = this->tm_.fn("test7_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("int"));

  auto type8 = this->tm_.fn("test8_0", {
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"));

  auto type9 = this->tm_.fn("test9_0", {
    TypeFnParam{std::nullopt, ref1, false, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"));

  auto type10 = this->tm_.fn("test10_0", {
    TypeFnParam{std::nullopt, ref1, true, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"));

  auto type11 = this->tm_.fn("test11_0", {
    TypeFnParam{std::nullopt, ref2, true, true, false},
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"));

  auto type12 = this->tm_.fn("test12_0", {
    TypeFnParam{"a", this->tm_.get("int"), true, true, false},
    TypeFnParam{"b", this->tm_.get("int"), true, false, true}
  }, this->tm_.get("int"));

  EXPECT_FALSE(type1->matchNice(this->tm_.get("any")));
  EXPECT_TRUE(type1->matchNice(type1));
  EXPECT_TRUE(type1->matchNice(type2));
  EXPECT_FALSE(type1->matchNice(type3));
  EXPECT_FALSE(type1->matchNice(type4));
  EXPECT_FALSE(type1->matchNice(type5));
  EXPECT_FALSE(type1->matchNice(type6));
  EXPECT_FALSE(type1->matchNice(type7));
  EXPECT_TRUE(type1->matchNice(type8));
  EXPECT_FALSE(type1->matchNice(type9));
  EXPECT_FALSE(type1->matchNice(type10));
  EXPECT_FALSE(type1->matchNice(type11));
  EXPECT_TRUE(type1->matchNice(type12));

  EXPECT_TRUE(type8->matchNice(type1));
  EXPECT_FALSE(type9->matchNice(type1));
  EXPECT_FALSE(type10->matchNice(type1));
  EXPECT_FALSE(type11->matchNice(type1));
  EXPECT_FALSE(type10->matchNice(type11));
  EXPECT_FALSE(type11->matchNice(type10));
  EXPECT_FALSE(type12->matchNice(type1));
}

TEST_F(TypeTest, MatchesNiceObject) {
  auto type1 = this->tm_.obj("Test1", "Test1_0", {
    TypeField{"a", this->tm_.get("int"), false, false, false},
    TypeField{"b", this->tm_.get("int"), false, false, false}
  });

  auto type2 = this->tm_.obj("Test2", "Test2_0");

  auto type3 = this->tm_.obj("Test3", "Test3_0", {
    TypeField{"a", this->tm_.get("int"), false, false, false}
  });

  auto type4 = this->tm_.obj("Test4", "Test4_0", {
    TypeField{"a", this->tm_.get("int"), false, false, false},
    TypeField{"b", this->tm_.get("int"), false, false, false}
  });

  auto type5 = this->tm_.obj("Test5", "Test5_0", {
    TypeField{"a", this->tm_.get("int"), false, false, false},
    TypeField{"b", this->tm_.get("str"), false, false, false}
  });

  EXPECT_TRUE(type1->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(type2));
  EXPECT_FALSE(type1->matchNice(type3));
  EXPECT_FALSE(type1->matchNice(type4));
  EXPECT_FALSE(type1->matchNice(type5));
  EXPECT_FALSE(type1->matchNice(this->tm_.get("int")));
}

TEST_F(TypeTest, MatchesNiceObjectMethod) {
  auto type1 = this->tm_.fn("test1", {}, this->tm_.get("void"));
  auto type2MethodInfo = TypeFnMethodInfo{false, "", nullptr, false};
  auto type2 = this->tm_.fn("test2", {}, this->tm_.get("void"), type2MethodInfo);
  auto type3MethodInfo = TypeFnMethodInfo{true, "self1_0", this->obj_, false};
  auto type3 = this->tm_.fn("test3", {}, this->tm_.get("void"), type3MethodInfo);
  auto type4MethodInfo = TypeFnMethodInfo{true, "self1_0", this->obj_, true};
  auto type4 = this->tm_.fn("test4", {}, this->tm_.get("void"), type4MethodInfo);
  auto type5MethodInfo = TypeFnMethodInfo{true, "self1_0", this->tm_.ref(this->obj_), true};
  auto type5 = this->tm_.fn("test5", {}, this->tm_.get("void"), type5MethodInfo);

  EXPECT_TRUE(type2->matchNice(type2));
  EXPECT_FALSE(type1->matchNice(type2));

  EXPECT_TRUE(type3->matchNice(type3));
  EXPECT_FALSE(type2->matchNice(type3));
  EXPECT_FALSE(type3->matchNice(type2));

  EXPECT_TRUE(type4->matchNice(type4));
  EXPECT_FALSE(type3->matchNice(type4));
  EXPECT_FALSE(type4->matchNice(type3));

  EXPECT_TRUE(type5->matchNice(type5));
  EXPECT_FALSE(type3->matchNice(type5));
  EXPECT_FALSE(type4->matchNice(type5));
  EXPECT_FALSE(type5->matchNice(type3));
  EXPECT_FALSE(type5->matchNice(type4));
}

TEST_F(TypeTest, MatchesNiceOptional) {
  auto type1 = this->tm_.opt(this->tm_.get("int"));
  auto type2 = this->tm_.opt(this->tm_.get("str"));

  EXPECT_TRUE(type1->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(type2));
  EXPECT_FALSE(type2->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchNice(type1));
}

TEST_F(TypeTest, MatchesNiceReference) {
  auto type1 = this->tm_.ref(this->tm_.get("int"));
  auto type2 = this->tm_.ref(this->tm_.get("int"));

  EXPECT_TRUE(type1->matchNice(type1));
  EXPECT_TRUE(type1->matchNice(type2));
  EXPECT_TRUE(type2->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(this->tm_.get("str")));
  EXPECT_FALSE(this->tm_.get("str")->matchNice(type1));
}

TEST_F(TypeTest, ShouldBeFreed) {
  EXPECT_TRUE(this->any_->shouldBeFreed());
  EXPECT_TRUE(this->arr_->shouldBeFreed());
  EXPECT_FALSE(this->enum_->shouldBeFreed());
  EXPECT_TRUE(this->fn_->shouldBeFreed());
  EXPECT_TRUE(this->obj_->shouldBeFreed());
  EXPECT_TRUE(this->opt_->shouldBeFreed());
  EXPECT_FALSE(this->ref_->shouldBeFreed());

  EXPECT_FALSE(this->tm_.get("bool")->shouldBeFreed());
  EXPECT_FALSE(this->tm_.get("byte")->shouldBeFreed());
  EXPECT_FALSE(this->tm_.get("char")->shouldBeFreed());
  EXPECT_FALSE(this->tm_.get("f32")->shouldBeFreed());
  EXPECT_FALSE(this->tm_.get("f64")->shouldBeFreed());
  EXPECT_FALSE(this->tm_.get("float")->shouldBeFreed());
  EXPECT_FALSE(this->tm_.get("i8")->shouldBeFreed());
  EXPECT_FALSE(this->tm_.get("i16")->shouldBeFreed());
  EXPECT_FALSE(this->tm_.get("i32")->shouldBeFreed());
  EXPECT_FALSE(this->tm_.get("i64")->shouldBeFreed());
  EXPECT_FALSE(this->tm_.get("int")->shouldBeFreed());
  EXPECT_TRUE(this->tm_.get("str")->shouldBeFreed());
  EXPECT_FALSE(this->tm_.get("u8")->shouldBeFreed());
  EXPECT_FALSE(this->tm_.get("u16")->shouldBeFreed());
  EXPECT_FALSE(this->tm_.get("u32")->shouldBeFreed());
  EXPECT_FALSE(this->tm_.get("u64")->shouldBeFreed());
  EXPECT_FALSE(this->tm_.get("void")->shouldBeFreed());
}
