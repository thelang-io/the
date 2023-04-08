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
    this->tm_.init();
    this->alias_ = this->tm_.createAlias("Alias", this->tm_.get("int"));
    this->any_ = this->tm_.get("any");
    this->arr_ = this->tm_.createArr(this->tm_.get("int"));

    this->tm_.stack.emplace_back("TestEnum");
    this->enum_ = this->tm_.createEnum("TestEnum", "TestEnum_0", {
      this->tm_.createEnumerator("Red", this->tm_.name("Red")),
      this->tm_.createEnumerator("Brown", this->tm_.name("Brown"))
    });
    this->tm_.stack.pop_back();

    this->fn_ = this->tm_.createFn({
      TypeFnParam{"a", this->tm_.get("int"), false, true, false},
      TypeFnParam{"b", this->tm_.get("int"), false, false, true}
    }, this->tm_.get("int"));

    this->map_ = this->tm_.createMap(this->tm_.get("str"), this->tm_.get("str"));
    auto objMethod = this->tm_.createMethod({}, this->tm_.get("void"), TypeCallInfo{"TestSDm_0", false, "", nullptr, false});

    this->obj_ = this->tm_.createObj("Test", "Test_0", {
      TypeField{"a", this->tm_.get("int"), false, false},
      TypeField{"m", objMethod, false, false}
    });

    this->opt_ = this->tm_.createOpt(this->tm_.get("int"));
    this->ref_ = this->tm_.createRef(this->tm_.get("int"));
    this->union_ = this->tm_.createUnion({this->tm_.get("int"), this->tm_.get("str")});
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

TEST_F(TypeTest, ActualOnAlias) {
  EXPECT_EQ(Type::actual(this->alias_), this->tm_.get("int"));
}

TEST_F(TypeTest, ActualOnActual) {
  EXPECT_EQ(Type::actual(this->tm_.get("int")), this->tm_.get("int"));
}

TEST_F(TypeTest, RealOnAlias) {
  EXPECT_EQ(Type::real(this->alias_), this->tm_.get("int"));
}

TEST_F(TypeTest, RealOnReal) {
  EXPECT_EQ(Type::real(this->tm_.get("int")), this->tm_.get("int"));
}

TEST_F(TypeTest, RealOnRef) {
  EXPECT_EQ(Type::real(this->ref_), this->tm_.get("int"));
}

TEST_F(TypeTest, FieldNthGetsZero) {
  auto type1 = this->tm_.createAlias("Test", this->obj_);
  auto type2 = this->tm_.createRef(this->obj_);

  EXPECT_EQ(this->obj_->fieldNth(0)->name, "a");
  EXPECT_EQ(type1->fieldNth(0)->name, "a");
  EXPECT_EQ(type2->fieldNth(0)->name, "a");
}

TEST_F(TypeTest, FieldNthGetsOne) {
  auto type1 = this->tm_.createObj("Test1", this->tm_.name("Test1"), {
    TypeField{"a", this->tm_.get("int"), false, false},
    TypeField{"b", this->tm_.get("int"), false, false}
  });

  auto type2 = this->tm_.createAlias("Test", type1);
  auto type3 = this->tm_.createRef(type1);

  EXPECT_EQ(type1->fieldNth(1)->name, "b");
  EXPECT_EQ(type2->fieldNth(1)->name, "b");
  EXPECT_EQ(type3->fieldNth(1)->name, "b");
}

TEST_F(TypeTest, FieldNthReturnsNull) {
  auto type1 = this->tm_.createObj("Test1", this->tm_.name("Test1"), {});
  auto type2 = this->tm_.createAlias("Test", type1);
  auto type3 = this->tm_.createRef(type2);

  EXPECT_EQ(type1->fieldNth(0), std::nullopt);
  EXPECT_EQ(type2->fieldNth(0), std::nullopt);
  EXPECT_EQ(type3->fieldNth(0), std::nullopt);
  EXPECT_EQ(this->obj_->fieldNth(1), std::nullopt);
}

TEST_F(TypeTest, GetsEnumerator) {
  EXPECT_EQ(this->enum_->getEnumerator("Red")->codeName, "TestEnumSDRed_0");
}

TEST_F(TypeTest, GetsNonExistingEnumerator) {
  EXPECT_THROW_WITH_MESSAGE({
    this->any_->getEnumerator("a");
  }, "tried to get a member of non-enum");

  EXPECT_THROW_WITH_MESSAGE({
    this->enum_->getEnumerator("White");
  }, "tried to get non-existing enum member");
}

TEST_F(TypeTest, GetsField) {
  EXPECT_EQ(this->alias_->getField("str").name, "str");
  EXPECT_EQ(this->any_->getField("str").name, "str");
  EXPECT_EQ(this->arr_->getField("len").name, "len");
  EXPECT_EQ(this->enum_->getField("str").name, "str");
  EXPECT_EQ(this->map_->getField("str").name, "str");
  EXPECT_EQ(this->obj_->getField("a").name, "a");
  EXPECT_EQ(this->opt_->getField("str").name, "str");
  EXPECT_EQ(this->ref_->getField("str").name, "str");
  EXPECT_EQ(this->union_->getField("str").name, "str");
  EXPECT_EQ(this->tm_.get("int")->getField("str").name, "str");
  EXPECT_EQ(this->tm_.get("str")->getField("len").name, "len");
}

TEST_F(TypeTest, GetsNonExistingField) {
  EXPECT_THROW_WITH_MESSAGE({
    this->alias_->getField("a");
  }, "tried to get non-existing field");

  EXPECT_THROW_WITH_MESSAGE({
    this->any_->getField("a");
  }, "tried to get non-existing field");

  EXPECT_THROW_WITH_MESSAGE({
    this->arr_->getField("a");
  }, "tried to get non-existing field");

  EXPECT_THROW_WITH_MESSAGE({
    this->enum_->getField("a");
  }, "tried to get non-existing field");

  EXPECT_THROW_WITH_MESSAGE({
    this->fn_->getField("a");
  }, "tried to get non-existing field");

  EXPECT_THROW_WITH_MESSAGE({
    this->map_->getField("b");
  }, "tried to get non-existing field");

  EXPECT_THROW_WITH_MESSAGE({
    this->obj_->getField("b");
  }, "tried to get non-existing field");

  EXPECT_THROW_WITH_MESSAGE({
    this->opt_->getField("b");
  }, "tried to get non-existing field");

  EXPECT_THROW_WITH_MESSAGE({
    this->ref_->getField("a");
  }, "tried to get non-existing field");

  EXPECT_THROW_WITH_MESSAGE({
    this->union_->getField("a");
  }, "tried to get non-existing field");

  EXPECT_THROW_WITH_MESSAGE({
    this->tm_.get("int")->getField("a");
  }, "tried to get non-existing field");

  EXPECT_THROW_WITH_MESSAGE({
    this->tm_.get("str")->getField("a");
  }, "tried to get non-existing field");
}

TEST_F(TypeTest, GetsProp) {
  EXPECT_NE(this->alias_->getProp("str"), nullptr);
  EXPECT_NE(this->any_->getProp("str"), nullptr);
  EXPECT_EQ(this->arr_->getProp("len"), this->tm_.get("int"));
  EXPECT_NE(this->enum_->getProp("str"), nullptr);
  EXPECT_NE(this->map_->getProp("str"), nullptr);
  EXPECT_EQ(this->obj_->getProp("a"), this->tm_.get("int"));
  EXPECT_NE(this->opt_->getProp("str"), nullptr);
  EXPECT_NE(this->ref_->getProp("str"), nullptr);
  EXPECT_NE(this->union_->getProp("str"), nullptr);
  EXPECT_NE(this->tm_.get("int")->getProp("str"), nullptr);
  EXPECT_EQ(this->tm_.get("str")->getProp("len"), this->tm_.get("int"));
}

TEST_F(TypeTest, GetsNonExistingProp) {
  EXPECT_THROW_WITH_MESSAGE({
    this->alias_->getProp("a");
  }, "tried to get non-existing prop type");

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
    this->map_->getProp("b");
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
    this->union_->getProp("a");
  }, "tried to get non-existing prop type");

  EXPECT_THROW_WITH_MESSAGE({
    this->tm_.get("int")->getProp("a");
  }, "tried to get non-existing prop type");

  EXPECT_THROW_WITH_MESSAGE({
    this->tm_.get("str")->getProp("a");
  }, "tried to get non-existing prop type");
}

TEST_F(TypeTest, HasField) {
  EXPECT_TRUE(this->alias_->hasField("str"));
  EXPECT_TRUE(this->any_->hasField("str"));
  EXPECT_TRUE(this->arr_->hasField("str"));
  EXPECT_TRUE(this->enum_->hasField("str"));
  EXPECT_TRUE(this->map_->hasField("str"));
  EXPECT_TRUE(this->obj_->hasField("a"));
  EXPECT_TRUE(this->opt_->hasField("str"));
  EXPECT_TRUE(this->ref_->hasField("str"));
  EXPECT_TRUE(this->union_->hasField("str"));
  EXPECT_TRUE(this->tm_.get("int")->hasField("str"));
  EXPECT_TRUE(this->tm_.get("str")->hasField("len"));
}

TEST_F(TypeTest, HasNonExistingField) {
  EXPECT_FALSE(this->alias_->hasField("a"));
  EXPECT_FALSE(this->any_->hasField("a"));
  EXPECT_FALSE(this->arr_->hasField("a"));
  EXPECT_FALSE(this->enum_->hasField("a"));
  EXPECT_FALSE(this->fn_->hasField("a"));
  EXPECT_FALSE(this->map_->hasField("b"));
  EXPECT_FALSE(this->obj_->hasField("b"));
  EXPECT_FALSE(this->opt_->hasField("b"));
  EXPECT_FALSE(this->ref_->hasField("a"));
  EXPECT_FALSE(this->union_->hasField("a"));
  EXPECT_FALSE(this->tm_.get("int")->hasField("a"));
  EXPECT_FALSE(this->tm_.get("str")->hasField("a"));
}

TEST_F(TypeTest, HasEnumerator) {
  EXPECT_TRUE(this->enum_->hasEnumerator("Red"));
}

TEST_F(TypeTest, HasNonExistingEnumerator) {
  EXPECT_FALSE(this->any_->hasEnumerator("White"));
  EXPECT_FALSE(this->enum_->hasEnumerator("White"));
}

TEST_F(TypeTest, HasProp) {
  EXPECT_TRUE(this->alias_->hasProp("str"));
  EXPECT_TRUE(this->any_->hasProp("str"));
  EXPECT_TRUE(this->arr_->hasProp("str"));
  EXPECT_TRUE(this->enum_->hasProp("str"));
  EXPECT_TRUE(this->map_->hasProp("str"));
  EXPECT_TRUE(this->obj_->hasProp("a"));
  EXPECT_TRUE(this->opt_->hasProp("str"));
  EXPECT_TRUE(this->ref_->hasProp("str"));
  EXPECT_TRUE(this->union_->hasProp("str"));
  EXPECT_TRUE(this->tm_.get("int")->hasProp("str"));
  EXPECT_TRUE(this->tm_.get("str")->hasProp("len"));
}

TEST_F(TypeTest, HasNonExistingProp) {
  EXPECT_FALSE(this->alias_->hasProp("a"));
  EXPECT_FALSE(this->any_->hasProp("a"));
  EXPECT_FALSE(this->arr_->hasProp("a"));
  EXPECT_FALSE(this->enum_->hasProp("a"));
  EXPECT_FALSE(this->fn_->hasProp("a"));
  EXPECT_FALSE(this->map_->hasProp("b"));
  EXPECT_FALSE(this->obj_->hasProp("b"));
  EXPECT_FALSE(this->opt_->hasProp("b"));
  EXPECT_FALSE(this->ref_->hasProp("a"));
  EXPECT_FALSE(this->union_->hasProp("a"));
  EXPECT_FALSE(this->tm_.get("int")->hasProp("a"));
  EXPECT_FALSE(this->tm_.get("str")->hasProp("a"));
}

TEST_F(TypeTest, HasSubType) {
  EXPECT_TRUE(this->union_->hasSubType(this->tm_.get("int")));
  EXPECT_TRUE(this->union_->hasSubType(this->tm_.get("str")));
}

TEST_F(TypeTest, HasSubTypeOnAlias) {
  auto type = this->tm_.createAlias("Alias2", this->union_);
  EXPECT_TRUE(type->hasSubType(this->tm_.get("int")));
  EXPECT_TRUE(type->hasSubType(this->tm_.get("str")));
}

TEST_F(TypeTest, HasSubTypeOnRef) {
  auto type = this->tm_.createRef(this->union_);
  EXPECT_TRUE(type->hasSubType(this->tm_.get("int")));
  EXPECT_TRUE(type->hasSubType(this->tm_.get("str")));
}

TEST_F(TypeTest, HasNonExistingSubType) {
  EXPECT_FALSE(this->alias_->hasSubType(this->tm_.get("float")));
  EXPECT_FALSE(this->any_->hasSubType(this->tm_.get("float")));
  EXPECT_FALSE(this->arr_->hasSubType(this->tm_.get("float")));
  EXPECT_FALSE(this->enum_->hasSubType(this->tm_.get("float")));
  EXPECT_FALSE(this->fn_->hasSubType(this->tm_.get("float")));
  EXPECT_FALSE(this->map_->hasSubType(this->tm_.get("float")));
  EXPECT_FALSE(this->obj_->hasSubType(this->tm_.get("float")));
  EXPECT_FALSE(this->opt_->hasSubType(this->tm_.get("float")));
  EXPECT_FALSE(this->ref_->hasSubType(this->tm_.get("float")));
  EXPECT_FALSE(this->union_->hasSubType(this->tm_.get("float")));
  EXPECT_FALSE(this->tm_.get("int")->hasSubType(this->tm_.get("float")));
  EXPECT_FALSE(this->tm_.get("str")->hasSubType(this->tm_.get("float")));
}

TEST_F(TypeTest, CheckIfAlias) {
  EXPECT_TRUE(this->alias_->isAlias());
  EXPECT_TRUE(this->tm_.createAlias("Alias2", this->tm_.get("int"))->isAlias());
}

TEST_F(TypeTest, CheckIfNotAlias) {
  EXPECT_FALSE(this->arr_->isAlias());
  EXPECT_FALSE(this->any_->isAlias());
  EXPECT_FALSE(this->enum_->isAlias());
  EXPECT_FALSE(this->fn_->isAlias());
  EXPECT_FALSE(this->map_->isAlias());
  EXPECT_FALSE(this->obj_->isAlias());
  EXPECT_FALSE(this->opt_->isAlias());
  EXPECT_FALSE(this->ref_->isAlias());
  EXPECT_FALSE(this->union_->isAlias());

  EXPECT_FALSE(this->tm_.get("any")->isAlias());
  EXPECT_FALSE(this->tm_.get("bool")->isAlias());
  EXPECT_FALSE(this->tm_.get("byte")->isAlias());
  EXPECT_FALSE(this->tm_.get("char")->isAlias());
  EXPECT_FALSE(this->tm_.get("f32")->isAlias());
  EXPECT_FALSE(this->tm_.get("f64")->isAlias());
  EXPECT_FALSE(this->tm_.get("float")->isAlias());
  EXPECT_FALSE(this->tm_.get("i8")->isAlias());
  EXPECT_FALSE(this->tm_.get("i16")->isAlias());
  EXPECT_FALSE(this->tm_.get("i32")->isAlias());
  EXPECT_FALSE(this->tm_.get("i64")->isAlias());
  EXPECT_FALSE(this->tm_.get("int")->isAlias());
  EXPECT_FALSE(this->tm_.get("str")->isAlias());
  EXPECT_FALSE(this->tm_.get("u8")->isAlias());
  EXPECT_FALSE(this->tm_.get("u16")->isAlias());
  EXPECT_FALSE(this->tm_.get("u32")->isAlias());
  EXPECT_FALSE(this->tm_.get("u64")->isAlias());
  EXPECT_FALSE(this->tm_.get("void")->isAlias());
}

TEST_F(TypeTest, CheckIfAny) {
  EXPECT_TRUE(this->any_->isAny());
  EXPECT_TRUE(this->tm_.get("any")->isAny());
}

TEST_F(TypeTest, CheckIfArray) {
  EXPECT_TRUE(this->arr_->isArray());
  EXPECT_TRUE(this->tm_.createArr(this->tm_.get("int"))->isArray());
}

TEST_F(TypeTest, CheckIfNotArray) {
  EXPECT_FALSE(this->alias_->isArray());
  EXPECT_FALSE(this->any_->isArray());
  EXPECT_FALSE(this->enum_->isArray());
  EXPECT_FALSE(this->fn_->isArray());
  EXPECT_FALSE(this->map_->isArray());
  EXPECT_FALSE(this->obj_->isArray());
  EXPECT_FALSE(this->opt_->isArray());
  EXPECT_FALSE(this->ref_->isArray());
  EXPECT_FALSE(this->union_->isArray());

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
  EXPECT_TRUE(this->tm_.createEnum("Test2", this->tm_.name("Test2"), {
    this->tm_.createEnumerator("Red", this->tm_.name("Red"))
  })->isEnum());
  this->tm_.stack.pop_back();
}

TEST_F(TypeTest, CheckIfNotEnum) {
  EXPECT_FALSE(this->alias_->isEnum());
  EXPECT_FALSE(this->any_->isEnum());
  EXPECT_FALSE(this->arr_->isEnum());
  EXPECT_FALSE(this->fn_->isEnum());
  EXPECT_FALSE(this->map_->isEnum());
  EXPECT_FALSE(this->obj_->isEnum());
  EXPECT_FALSE(this->opt_->isEnum());
  EXPECT_FALSE(this->ref_->isEnum());
  EXPECT_FALSE(this->union_->isEnum());

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

TEST_F(TypeTest, CheckIfEnumerator) {
  EXPECT_TRUE(this->tm_.createEnumerator("Brown", this->tm_.name("Brown"))->isEnumerator());
}

TEST_F(TypeTest, CheckIfNotEnumerator) {
  EXPECT_FALSE(this->alias_->isEnumerator());
  EXPECT_FALSE(this->any_->isEnumerator());
  EXPECT_FALSE(this->arr_->isEnumerator());
  EXPECT_FALSE(this->enum_->isEnumerator());
  EXPECT_FALSE(this->fn_->isEnumerator());
  EXPECT_FALSE(this->map_->isEnumerator());
  EXPECT_FALSE(this->obj_->isEnumerator());
  EXPECT_FALSE(this->opt_->isEnumerator());
  EXPECT_FALSE(this->ref_->isEnumerator());
  EXPECT_FALSE(this->union_->isEnumerator());

  EXPECT_FALSE(this->tm_.get("any")->isEnumerator());
  EXPECT_FALSE(this->tm_.get("bool")->isEnumerator());
  EXPECT_FALSE(this->tm_.get("byte")->isEnumerator());
  EXPECT_FALSE(this->tm_.get("char")->isEnumerator());
  EXPECT_FALSE(this->tm_.get("f32")->isEnumerator());
  EXPECT_FALSE(this->tm_.get("f64")->isEnumerator());
  EXPECT_FALSE(this->tm_.get("float")->isEnumerator());
  EXPECT_FALSE(this->tm_.get("i8")->isEnumerator());
  EXPECT_FALSE(this->tm_.get("i16")->isEnumerator());
  EXPECT_FALSE(this->tm_.get("i32")->isEnumerator());
  EXPECT_FALSE(this->tm_.get("i64")->isEnumerator());
  EXPECT_FALSE(this->tm_.get("int")->isEnumerator());
  EXPECT_FALSE(this->tm_.get("str")->isEnumerator());
  EXPECT_FALSE(this->tm_.get("u8")->isEnumerator());
  EXPECT_FALSE(this->tm_.get("u16")->isEnumerator());
  EXPECT_FALSE(this->tm_.get("u32")->isEnumerator());
  EXPECT_FALSE(this->tm_.get("u64")->isEnumerator());
  EXPECT_FALSE(this->tm_.get("void")->isEnumerator());
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
  EXPECT_FALSE(this->alias_->isFloatNumber());
  EXPECT_FALSE(this->any_->isFloatNumber());
  EXPECT_FALSE(this->arr_->isFloatNumber());
  EXPECT_FALSE(this->enum_->isFloatNumber());
  EXPECT_FALSE(this->fn_->isFloatNumber());
  EXPECT_FALSE(this->map_->isFloatNumber());
  EXPECT_FALSE(this->obj_->isFloatNumber());
  EXPECT_FALSE(this->opt_->isFloatNumber());
  EXPECT_FALSE(this->ref_->isFloatNumber());
  EXPECT_FALSE(this->union_->isFloatNumber());

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
  auto type1 = this->tm_.createFn({}, this->tm_.get("int"));

  auto type2 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("int"), false, true, false}
  }, this->tm_.get("int"));

  auto type3 = this->tm_.createFn({
    TypeFnParam{"a", this->tm_.get("str"), false, false, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("str"));

  auto type4 = this->tm_.createFn({}, this->tm_.get("void"));

  auto type5 = this->tm_.createFn({
    TypeFnParam{std::nullopt, this->tm_.get("int"), false, true, false}
  }, this->tm_.get("void"));

  auto type6 = this->tm_.createFn({
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
  EXPECT_FALSE(this->alias_->isFn());
  EXPECT_FALSE(this->any_->isFn());
  EXPECT_FALSE(this->arr_->isFn());
  EXPECT_FALSE(this->enum_->isFn());
  EXPECT_FALSE(this->map_->isFn());
  EXPECT_FALSE(this->obj_->isFn());
  EXPECT_FALSE(this->opt_->isFn());
  EXPECT_FALSE(this->ref_->isFn());
  EXPECT_FALSE(this->union_->isFn());

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
  EXPECT_FALSE(this->alias_->isIntNumber());
  EXPECT_FALSE(this->any_->isIntNumber());
  EXPECT_FALSE(this->arr_->isIntNumber());
  EXPECT_FALSE(this->enum_->isIntNumber());
  EXPECT_FALSE(this->fn_->isIntNumber());
  EXPECT_FALSE(this->map_->isIntNumber());
  EXPECT_FALSE(this->obj_->isIntNumber());
  EXPECT_FALSE(this->opt_->isIntNumber());
  EXPECT_FALSE(this->ref_->isIntNumber());
  EXPECT_FALSE(this->union_->isIntNumber());

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

TEST_F(TypeTest, CheckIfMap) {
  EXPECT_TRUE(this->map_->isMap());
  EXPECT_TRUE(this->tm_.createMap(this->tm_.get("int"), this->tm_.get("int")));
}

TEST_F(TypeTest, CheckIfNotMap) {
  EXPECT_FALSE(this->alias_->isMap());
  EXPECT_FALSE(this->any_->isMap());
  EXPECT_FALSE(this->arr_->isMap());
  EXPECT_FALSE(this->enum_->isMap());
  EXPECT_FALSE(this->fn_->isMap());
  EXPECT_FALSE(this->obj_->isMap());
  EXPECT_FALSE(this->opt_->isMap());
  EXPECT_FALSE(this->ref_->isMap());
  EXPECT_FALSE(this->union_->isMap());

  EXPECT_FALSE(this->tm_.get("any")->isMap());
  EXPECT_FALSE(this->tm_.get("bool")->isMap());
  EXPECT_FALSE(this->tm_.get("byte")->isMap());
  EXPECT_FALSE(this->tm_.get("char")->isMap());
  EXPECT_FALSE(this->tm_.get("f32")->isMap());
  EXPECT_FALSE(this->tm_.get("f64")->isMap());
  EXPECT_FALSE(this->tm_.get("float")->isMap());
  EXPECT_FALSE(this->tm_.get("i8")->isMap());
  EXPECT_FALSE(this->tm_.get("i16")->isMap());
  EXPECT_FALSE(this->tm_.get("i32")->isMap());
  EXPECT_FALSE(this->tm_.get("i64")->isMap());
  EXPECT_FALSE(this->tm_.get("int")->isMap());
  EXPECT_FALSE(this->tm_.get("str")->isMap());
  EXPECT_FALSE(this->tm_.get("u8")->isMap());
  EXPECT_FALSE(this->tm_.get("u16")->isMap());
  EXPECT_FALSE(this->tm_.get("u32")->isMap());
  EXPECT_FALSE(this->tm_.get("u64")->isMap());
  EXPECT_FALSE(this->tm_.get("void")->isMap());
}

TEST_F(TypeTest, CheckIfMethod) {
  EXPECT_TRUE(this->tm_.get("Test")->getProp("m")->isMethod());
}

TEST_F(TypeTest, CheckIfNotMethod) {
  EXPECT_FALSE(this->alias_->isMethod());
  EXPECT_FALSE(this->any_->isMethod());
  EXPECT_FALSE(this->arr_->isMethod());
  EXPECT_FALSE(this->enum_->isMethod());
  EXPECT_FALSE(this->fn_->isMethod());
  EXPECT_FALSE(this->map_->isMethod());
  EXPECT_FALSE(this->obj_->isMethod());
  EXPECT_FALSE(this->opt_->isMethod());
  EXPECT_FALSE(this->ref_->isMethod());
  EXPECT_FALSE(this->union_->isMethod());

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
  EXPECT_FALSE(this->alias_->isNumber());
  EXPECT_FALSE(this->any_->isNumber());
  EXPECT_FALSE(this->arr_->isNumber());
  EXPECT_FALSE(this->enum_->isNumber());
  EXPECT_FALSE(this->fn_->isNumber());
  EXPECT_FALSE(this->map_->isNumber());
  EXPECT_FALSE(this->obj_->isNumber());
  EXPECT_FALSE(this->opt_->isNumber());
  EXPECT_FALSE(this->ref_->isNumber());
  EXPECT_FALSE(this->union_->isNumber());

  EXPECT_FALSE(this->tm_.get("any")->isNumber());
  EXPECT_FALSE(this->tm_.get("bool")->isNumber());
  EXPECT_FALSE(this->tm_.get("byte")->isNumber());
  EXPECT_FALSE(this->tm_.get("char")->isNumber());
  EXPECT_FALSE(this->tm_.get("str")->isNumber());
  EXPECT_FALSE(this->tm_.get("void")->isNumber());
}

TEST_F(TypeTest, CheckIfObj) {
  auto type1 = this->tm_.createObj("Test1", "Test1_0");

  auto type2 = this->tm_.createObj("Test2", "Test2_0", {
    TypeField{"a", this->tm_.get("int"), false, false}
  });

  auto type3 = this->tm_.createObj("Test3", "Test3_0", {
    TypeField{"a", this->tm_.get("int"), false, false},
    TypeField{"b", this->tm_.get("str"), false, false}
  });

  EXPECT_TRUE(this->obj_->isObj());
  EXPECT_TRUE(type1->isObj());
  EXPECT_TRUE(type2->isObj());
  EXPECT_TRUE(type3->isObj());
}

TEST_F(TypeTest, CheckIfNotObj) {
  EXPECT_FALSE(this->alias_->isObj());
  EXPECT_FALSE(this->any_->isObj());
  EXPECT_FALSE(this->arr_->isObj());
  EXPECT_FALSE(this->enum_->isObj());
  EXPECT_FALSE(this->fn_->isObj());
  EXPECT_FALSE(this->map_->isObj());
  EXPECT_FALSE(this->opt_->isObj());
  EXPECT_FALSE(this->ref_->isObj());
  EXPECT_FALSE(this->union_->isObj());

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
  EXPECT_FALSE(this->alias_->isOpt());
  EXPECT_FALSE(this->any_->isOpt());
  EXPECT_FALSE(this->arr_->isOpt());
  EXPECT_FALSE(this->enum_->isOpt());
  EXPECT_FALSE(this->fn_->isOpt());
  EXPECT_FALSE(this->map_->isOpt());
  EXPECT_FALSE(this->obj_->isOpt());
  EXPECT_FALSE(this->ref_->isOpt());
  EXPECT_FALSE(this->union_->isOpt());

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
  EXPECT_TRUE(this->tm_.createRef(this->tm_.get("u64"))->isRef());
}

TEST_F(TypeTest, CheckIfNotRef) {
  EXPECT_FALSE(this->alias_->isRef());
  EXPECT_FALSE(this->any_->isRef());
  EXPECT_FALSE(this->arr_->isRef());
  EXPECT_FALSE(this->enum_->isRef());
  EXPECT_FALSE(this->fn_->isRef());
  EXPECT_FALSE(this->map_->isRef());
  EXPECT_FALSE(this->obj_->isRef());
  EXPECT_FALSE(this->opt_->isRef());
  EXPECT_FALSE(this->union_->isRef());

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

TEST_F(TypeTest, CheckIfRefOf) {
  EXPECT_TRUE(this->ref_->isRefOf(this->tm_.get("int")));
  EXPECT_TRUE(this->tm_.createRef(this->tm_.get("u64"))->isRefOf(this->tm_.get("u64")));
}

TEST_F(TypeTest, CheckIfNotRefOf) {
  EXPECT_FALSE(this->alias_->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->any_->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->arr_->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->enum_->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->fn_->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->map_->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->obj_->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->opt_->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->ref_->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->union_->isRefOf(this->tm_.get("float")));

  EXPECT_FALSE(this->tm_.get("any")->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->tm_.get("bool")->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->tm_.get("byte")->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->tm_.get("char")->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->tm_.get("f32")->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->tm_.get("f64")->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->tm_.get("float")->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->tm_.get("i8")->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->tm_.get("i16")->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->tm_.get("i32")->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->tm_.get("i64")->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->tm_.get("int")->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->tm_.get("str")->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->tm_.get("u8")->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->tm_.get("u16")->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->tm_.get("u32")->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->tm_.get("u64")->isRefOf(this->tm_.get("float")));
  EXPECT_FALSE(this->tm_.get("void")->isRefOf(this->tm_.get("float")));
}

TEST_F(TypeTest, CheckIfSafeForTernaryAlt) {
  EXPECT_TRUE(this->tm_.createAlias("Alias2", this->tm_.get("str"))->isSafeForTernaryAlt());
  EXPECT_TRUE(this->any_->isSafeForTernaryAlt());
  EXPECT_TRUE(this->arr_->isSafeForTernaryAlt());
  EXPECT_TRUE(this->fn_->isSafeForTernaryAlt());
  EXPECT_TRUE(this->map_->isSafeForTernaryAlt());
  EXPECT_TRUE(this->obj_->isSafeForTernaryAlt());
  EXPECT_TRUE(this->opt_->isSafeForTernaryAlt());
  EXPECT_TRUE(this->union_->isSafeForTernaryAlt());
  EXPECT_TRUE(this->tm_.get("str")->isSafeForTernaryAlt());
}

TEST_F(TypeTest, CheckIfNotSafeForTernaryAlt) {
  EXPECT_FALSE(this->alias_->isSafeForTernaryAlt());
  EXPECT_FALSE(this->enum_->isSafeForTernaryAlt());
  EXPECT_FALSE(this->ref_->isSafeForTernaryAlt());

  EXPECT_FALSE(this->tm_.get("bool")->isSafeForTernaryAlt());
  EXPECT_FALSE(this->tm_.get("byte")->isSafeForTernaryAlt());
  EXPECT_FALSE(this->tm_.get("char")->isSafeForTernaryAlt());
  EXPECT_FALSE(this->tm_.get("f32")->isSafeForTernaryAlt());
  EXPECT_FALSE(this->tm_.get("f64")->isSafeForTernaryAlt());
  EXPECT_FALSE(this->tm_.get("float")->isSafeForTernaryAlt());
  EXPECT_FALSE(this->tm_.get("i8")->isSafeForTernaryAlt());
  EXPECT_FALSE(this->tm_.get("i16")->isSafeForTernaryAlt());
  EXPECT_FALSE(this->tm_.get("i32")->isSafeForTernaryAlt());
  EXPECT_FALSE(this->tm_.get("i64")->isSafeForTernaryAlt());
  EXPECT_FALSE(this->tm_.get("int")->isSafeForTernaryAlt());
  EXPECT_FALSE(this->tm_.get("u8")->isSafeForTernaryAlt());
  EXPECT_FALSE(this->tm_.get("u16")->isSafeForTernaryAlt());
  EXPECT_FALSE(this->tm_.get("u32")->isSafeForTernaryAlt());
  EXPECT_FALSE(this->tm_.get("u64")->isSafeForTernaryAlt());
  EXPECT_FALSE(this->tm_.get("void")->isSafeForTernaryAlt());
}

TEST_F(TypeTest, CheckIfSmallForVarArg) {
  EXPECT_FALSE(this->alias_->isSmallForVarArg());
  EXPECT_FALSE(this->any_->isSmallForVarArg());
  EXPECT_FALSE(this->arr_->isSmallForVarArg());
  EXPECT_FALSE(this->enum_->isSmallForVarArg());
  EXPECT_FALSE(this->fn_->isSmallForVarArg());
  EXPECT_FALSE(this->map_->isSmallForVarArg());
  EXPECT_FALSE(this->obj_->isSmallForVarArg());
  EXPECT_FALSE(this->opt_->isSmallForVarArg());
  EXPECT_FALSE(this->ref_->isSmallForVarArg());
  EXPECT_FALSE(this->union_->isSmallForVarArg());

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

TEST_F(TypeTest, CheckIfUnion) {
  EXPECT_TRUE(this->union_->isUnion());
  EXPECT_TRUE(this->tm_.createUnion({this->tm_.get("f32"), this->tm_.get("f64")})->isUnion());
}

TEST_F(TypeTest, CheckIfNotUnion) {
  EXPECT_FALSE(this->alias_->isUnion());
  EXPECT_FALSE(this->any_->isUnion());
  EXPECT_FALSE(this->arr_->isUnion());
  EXPECT_FALSE(this->enum_->isUnion());
  EXPECT_FALSE(this->fn_->isUnion());
  EXPECT_FALSE(this->map_->isUnion());
  EXPECT_FALSE(this->obj_->isUnion());
  EXPECT_FALSE(this->opt_->isUnion());
  EXPECT_FALSE(this->ref_->isUnion());

  EXPECT_FALSE(this->tm_.get("any")->isUnion());
  EXPECT_FALSE(this->tm_.get("bool")->isUnion());
  EXPECT_FALSE(this->tm_.get("byte")->isUnion());
  EXPECT_FALSE(this->tm_.get("char")->isUnion());
  EXPECT_FALSE(this->tm_.get("f32")->isUnion());
  EXPECT_FALSE(this->tm_.get("f64")->isUnion());
  EXPECT_FALSE(this->tm_.get("float")->isUnion());
  EXPECT_FALSE(this->tm_.get("i8")->isUnion());
  EXPECT_FALSE(this->tm_.get("i16")->isUnion());
  EXPECT_FALSE(this->tm_.get("i32")->isUnion());
  EXPECT_FALSE(this->tm_.get("i64")->isUnion());
  EXPECT_FALSE(this->tm_.get("int")->isUnion());
  EXPECT_FALSE(this->tm_.get("str")->isUnion());
  EXPECT_FALSE(this->tm_.get("u8")->isUnion());
  EXPECT_FALSE(this->tm_.get("u16")->isUnion());
  EXPECT_FALSE(this->tm_.get("u32")->isUnion());
  EXPECT_FALSE(this->tm_.get("u64")->isUnion());
  EXPECT_FALSE(this->tm_.get("void")->isUnion());
}

TEST_F(TypeTest, CheckIfVoid) {
  EXPECT_TRUE(this->tm_.get("void")->isVoid());
}

TEST_F(TypeTest, ShouldBeFreed) {
  EXPECT_FALSE(this->alias_->shouldBeFreed());
  EXPECT_TRUE(this->tm_.createAlias("Alias2", this->tm_.get("str"))->shouldBeFreed());
  EXPECT_TRUE(this->any_->shouldBeFreed());
  EXPECT_TRUE(this->arr_->shouldBeFreed());
  EXPECT_FALSE(this->enum_->shouldBeFreed());
  EXPECT_TRUE(this->fn_->shouldBeFreed());
  EXPECT_TRUE(this->map_->shouldBeFreed());
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

TEST_F(TypeTest, ShouldBeFreedUnion) {
  auto type1 = this->tm_.createUnion({this->tm_.get("int"), this->tm_.get("int")});
  auto type2 = this->tm_.createUnion({this->tm_.get("int"), this->tm_.get("str")});
  auto type3 = this->tm_.createUnion({this->tm_.get("str"), this->tm_.get("int")});

  EXPECT_FALSE(type1->shouldBeFreed());
  EXPECT_TRUE(type2->shouldBeFreed());
  EXPECT_TRUE(type3->shouldBeFreed());
}

TEST_F(TypeTest, VaArgCodeGenerates) {
  EXPECT_EQ(this->alias_->vaArgCode("test"), "test");
  EXPECT_EQ(this->any_->vaArgCode("test"), "test");
  EXPECT_EQ(this->arr_->vaArgCode("test"), "test");
  EXPECT_EQ(this->enum_->vaArgCode("test"), "test");
  EXPECT_EQ(this->fn_->vaArgCode("test"), "test");
  EXPECT_EQ(this->map_->vaArgCode("test"), "test");
  EXPECT_EQ(this->obj_->vaArgCode("test"), "test");
  EXPECT_EQ(this->opt_->vaArgCode("test"), "test");
  EXPECT_EQ(this->ref_->vaArgCode("test"), "test");
  EXPECT_EQ(this->union_->vaArgCode("test"), "test");

  EXPECT_EQ(this->tm_.get("any")->vaArgCode("test"), "test");
  EXPECT_EQ(this->tm_.get("bool")->vaArgCode("test"), "int");
  EXPECT_EQ(this->tm_.get("byte")->vaArgCode("test"), "int");
  EXPECT_EQ(this->tm_.get("char")->vaArgCode("test"), "int");
  EXPECT_EQ(this->tm_.get("f32")->vaArgCode("test"), "double");
  EXPECT_EQ(this->tm_.get("f64")->vaArgCode("test"), "test");
  EXPECT_EQ(this->tm_.get("float")->vaArgCode("test"), "test");
  EXPECT_EQ(this->tm_.get("i8")->vaArgCode("test"), "int");
  EXPECT_EQ(this->tm_.get("i16")->vaArgCode("test"), "int");
  EXPECT_EQ(this->tm_.get("i32")->vaArgCode("test"), "test");
  EXPECT_EQ(this->tm_.get("i64")->vaArgCode("test"), "test");
  EXPECT_EQ(this->tm_.get("int")->vaArgCode("test"), "test");
  EXPECT_EQ(this->tm_.get("str")->vaArgCode("test"), "test");
  EXPECT_EQ(this->tm_.get("u8")->vaArgCode("test"), "int");
  EXPECT_EQ(this->tm_.get("u16")->vaArgCode("test"), "int");
  EXPECT_EQ(this->tm_.get("u32")->vaArgCode("test"), "test");
  EXPECT_EQ(this->tm_.get("u64")->vaArgCode("test"), "test");
  EXPECT_EQ(this->tm_.get("void")->vaArgCode("test"), "test");
}
