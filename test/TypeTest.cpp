/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include "../src/TypeMap.hpp"
#include "utils.hpp"

class TypeTest : public testing::Test {
 protected:
  TypeMap tm_;

  void SetUp () override {
    this->tm_.init();
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
  auto ref1 = this->tm_.ref(this->tm_.get("int"));
  EXPECT_EQ(Type::real(ref1), this->tm_.get("int"));
}

TEST_F(TypeTest, GetsProp) {
  auto ref1 = this->tm_.ref(this->tm_.get("int"));

  EXPECT_NE(this->tm_.get("int")->getProp("str"), nullptr);
  EXPECT_NE(ref1->getProp("str"), nullptr);
  EXPECT_EQ(this->tm_.get("str")->getProp("len"), this->tm_.get("int"));

  this->tm_.obj("Test1", "Test1_0", {
    TypeObjField{"a", this->tm_.get("int")}
  });

  EXPECT_EQ(this->tm_.get("Test1")->getProp("a"), this->tm_.get("int"));
}

TEST_F(TypeTest, GetsNonExistingProp) {
  auto ref1 = this->tm_.ref(this->tm_.get("int"));

  EXPECT_THROW_WITH_MESSAGE({
    this->tm_.get("int")->getProp("str")->getProp("str");
  }, "tried to get non-existing prop type");

  EXPECT_THROW_WITH_MESSAGE({
    ref1->getProp("str")->getProp("str");
  }, "tried to get non-existing prop type");

  EXPECT_THROW_WITH_MESSAGE({
    this->tm_.get("int")->getProp("a");
  }, "tried to get non-existing prop type");

  EXPECT_THROW_WITH_MESSAGE({
    ref1->getProp("a");
  }, "tried to get non-existing prop type");

  EXPECT_THROW_WITH_MESSAGE({
    this->tm_.get("str")->getProp("len2");
  }, "tried to get non-existing prop type");

  auto type1 = this->tm_.fn("test1_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"));

  EXPECT_THROW_WITH_MESSAGE({
    type1->getProp("a");
  }, "tried to get non-existing prop type");

  this->tm_.obj("Test1", "Test1_0", {
    TypeObjField{"a", this->tm_.get("int")}
  });

  EXPECT_THROW_WITH_MESSAGE({
    this->tm_.get("Test1")->getProp("b");
  }, "tried to get non-existing prop type");
}

TEST_F(TypeTest, HasProp) {
  auto ref1 = this->tm_.ref(this->tm_.get("int"));

  EXPECT_TRUE(this->tm_.get("int")->hasProp("str"));
  EXPECT_TRUE(ref1->hasProp("str"));
  EXPECT_TRUE(this->tm_.get("str")->hasProp("len"));

  this->tm_.obj("Test1", "Test1_0", {
    TypeObjField{"a", this->tm_.get("int")}
  });

  EXPECT_TRUE(this->tm_.get("Test1")->hasProp("a"));
}

TEST_F(TypeTest, HasNonExistingProp) {
  auto ref1 = this->tm_.ref(this->tm_.get("int"));

  EXPECT_FALSE(this->tm_.get("int")->getProp("str")->hasProp("str"));
  EXPECT_FALSE(ref1->getProp("str")->hasProp("str"));
  EXPECT_FALSE(this->tm_.get("int")->hasProp("a"));
  EXPECT_FALSE(ref1->hasProp("a"));
  EXPECT_FALSE(this->tm_.get("str")->hasProp("len2"));

  auto type1 = this->tm_.fn("test1_0", {
    TypeFnParam{"a", this->tm_.get("int"), false, true, false},
    TypeFnParam{"b", this->tm_.get("int"), false, false, true}
  }, this->tm_.get("int"));

  EXPECT_FALSE(type1->hasProp("a"));

  this->tm_.obj("Test1", "Test1_0", {
    TypeObjField{"a", this->tm_.get("int")}
  });

  EXPECT_FALSE(this->tm_.get("Test1")->hasProp("b"));
}

TEST_F(TypeTest, CheckIfAny) {
  EXPECT_TRUE(this->tm_.get("any")->isAny());
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

  EXPECT_TRUE(type1->isFn());
  EXPECT_TRUE(type2->isFn());
  EXPECT_TRUE(type3->isFn());
  EXPECT_TRUE(type4->isFn());
  EXPECT_TRUE(type5->isFn());
  EXPECT_TRUE(type6->isFn());
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
  EXPECT_FALSE(this->tm_.get("any")->isNumber());
  EXPECT_FALSE(this->tm_.get("bool")->isNumber());
  EXPECT_FALSE(this->tm_.get("byte")->isNumber());
  EXPECT_FALSE(this->tm_.get("char")->isNumber());
  EXPECT_FALSE(this->tm_.get("str")->isNumber());
  EXPECT_FALSE(this->tm_.get("void")->isNumber());
}

TEST_F(TypeTest, CheckIfObj) {
  this->tm_.obj("Test1", "Test1_0");

  this->tm_.obj("Test2", "Test2_0", {
    TypeObjField{"a", this->tm_.get("int")}
  });

  this->tm_.obj("Test3", "Test3_0", {
    TypeObjField{"a", this->tm_.get("int")},
    TypeObjField{"b", this->tm_.get("str")}
  });

  EXPECT_TRUE(this->tm_.get("Test1")->isObj());
  EXPECT_TRUE(this->tm_.get("Test2")->isObj());
  EXPECT_TRUE(this->tm_.get("Test3")->isObj());
}

TEST_F(TypeTest, CheckIfRef) {
  auto ref1 = this->tm_.ref(this->tm_.get("str"));
  EXPECT_TRUE(ref1->isRef());
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
  EXPECT_TRUE(this->tm_.get("bool")->match(this->tm_.get("bool")));
  EXPECT_TRUE(this->tm_.get("byte")->match(this->tm_.get("byte")));
  EXPECT_TRUE(this->tm_.get("char")->match(this->tm_.get("char")));
  EXPECT_TRUE(this->tm_.get("str")->match(this->tm_.get("str")));
  EXPECT_TRUE(this->tm_.get("void")->match(this->tm_.get("void")));
}

TEST_F(TypeTest, MatchesAny) {
  auto ref1 = this->tm_.ref(this->tm_.get("int"));

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
  EXPECT_TRUE(this->tm_.get("any")->match(ref1));
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

  EXPECT_TRUE(this->tm_.get("any")->match(type1));
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
  this->tm_.obj("Test1", "Test1_0", {
    TypeObjField{"a", this->tm_.get("int")},
    TypeObjField{"b", this->tm_.get("int")}
  });

  this->tm_.obj("Test2", "Test2_0");

  this->tm_.obj("Test3", "Test3_0", {
    TypeObjField{"a", this->tm_.get("int")}
  });

  this->tm_.obj("Test4", "Test4_0", {
    TypeObjField{"a", this->tm_.get("int")},
    TypeObjField{"b", this->tm_.get("int")}
  });

  this->tm_.obj("Test5", "Test5_0", {
    TypeObjField{"a", this->tm_.get("int")},
    TypeObjField{"b", this->tm_.get("str")}
  });

  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("Test1")));
  EXPECT_TRUE(this->tm_.get("Test1")->match(this->tm_.get("Test1")));
  EXPECT_FALSE(this->tm_.get("Test1")->match(this->tm_.get("Test2")));
  EXPECT_FALSE(this->tm_.get("Test1")->match(this->tm_.get("Test3")));
  EXPECT_FALSE(this->tm_.get("Test1")->match(this->tm_.get("Test4")));
  EXPECT_FALSE(this->tm_.get("Test1")->match(this->tm_.get("Test5")));
  EXPECT_FALSE(this->tm_.get("Test1")->match(this->tm_.get("int")));
}

TEST_F(TypeTest, MatchesReference) {
  auto ref1 = this->tm_.ref(this->tm_.get("int"));
  auto ref2 = this->tm_.ref(this->tm_.get("int"));

  EXPECT_TRUE(ref1->match(ref1));
  EXPECT_TRUE(ref1->match(ref2));
  EXPECT_TRUE(ref2->match(ref1));
  EXPECT_TRUE(ref1->match(this->tm_.get("int")));
  EXPECT_TRUE(this->tm_.get("int")->match(ref1));
  EXPECT_FALSE(ref1->match(this->tm_.get("str")));
  EXPECT_FALSE(this->tm_.get("str")->match(ref1));
}

TEST_F(TypeTest, MatchesExact) {
  auto ref1 = this->tm_.ref(this->tm_.get("int"));

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
  EXPECT_TRUE(ref1->matchExact(ref1));
  EXPECT_TRUE(this->tm_.get("str")->matchExact(this->tm_.get("str")));
  EXPECT_TRUE(this->tm_.get("u8")->matchExact(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("u16")->matchExact(this->tm_.get("u16")));
  EXPECT_TRUE(this->tm_.get("u32")->matchExact(this->tm_.get("u32")));
  EXPECT_TRUE(this->tm_.get("u64")->matchExact(this->tm_.get("u64")));
  EXPECT_TRUE(this->tm_.get("void")->matchExact(this->tm_.get("void")));
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

  EXPECT_FALSE(this->tm_.get("any")->matchExact(type1));
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
  this->tm_.obj("Test1", "Test1_0", {
    TypeObjField{"a", this->tm_.get("int")},
    TypeObjField{"b", this->tm_.get("int")}
  });

  this->tm_.obj("Test2", "Test2_0");

  this->tm_.obj("Test3", "Test3_0", {
    TypeObjField{"a", this->tm_.get("int")}
  });

  this->tm_.obj("Test4", "Test4_0", {
    TypeObjField{"a", this->tm_.get("int")},
    TypeObjField{"b", this->tm_.get("int")}
  });

  this->tm_.obj("Test5", "Test5_0", {
    TypeObjField{"a", this->tm_.get("int")},
    TypeObjField{"b", this->tm_.get("str")}
  });

  EXPECT_FALSE(this->tm_.get("any")->matchExact(this->tm_.get("Test1")));
  EXPECT_TRUE(this->tm_.get("Test1")->matchExact(this->tm_.get("Test1")));
  EXPECT_FALSE(this->tm_.get("Test1")->matchExact(this->tm_.get("Test2")));
  EXPECT_FALSE(this->tm_.get("Test1")->matchExact(this->tm_.get("Test3")));
  EXPECT_FALSE(this->tm_.get("Test1")->matchExact(this->tm_.get("Test4")));
  EXPECT_FALSE(this->tm_.get("Test1")->matchExact(this->tm_.get("Test5")));
  EXPECT_FALSE(this->tm_.get("Test1")->matchExact(this->tm_.get("int")));
}

TEST_F(TypeTest, MatchesExactReference) {
  auto ref1 = this->tm_.ref(this->tm_.get("int"));
  auto ref2 = this->tm_.ref(this->tm_.get("int"));

  EXPECT_TRUE(ref1->matchExact(ref1));
  EXPECT_TRUE(ref1->matchExact(ref2));
  EXPECT_TRUE(ref2->matchExact(ref1));
  EXPECT_FALSE(ref1->matchExact(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchExact(ref1));
  EXPECT_FALSE(ref1->matchExact(this->tm_.get("str")));
  EXPECT_FALSE(this->tm_.get("str")->matchExact(ref1));
}

TEST_F(TypeTest, MatchesNice) {
  EXPECT_TRUE(this->tm_.get("bool")->matchNice(this->tm_.get("bool")));
  EXPECT_TRUE(this->tm_.get("byte")->matchNice(this->tm_.get("byte")));
  EXPECT_TRUE(this->tm_.get("char")->matchNice(this->tm_.get("char")));
  EXPECT_TRUE(this->tm_.get("str")->matchNice(this->tm_.get("str")));
  EXPECT_TRUE(this->tm_.get("void")->matchNice(this->tm_.get("void")));
}

TEST_F(TypeTest, MatchesNiceAny) {
  auto ref1 = this->tm_.ref(this->tm_.get("int"));

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
  EXPECT_TRUE(this->tm_.get("any")->matchNice(ref1));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("str")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("u8")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("u16")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("u32")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("u64")));
  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("void")));
}

TEST_F(TypeTest, MatchesNiceFloat) {
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

TEST_F(TypeTest, MatchesNiceInteger) {
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

  EXPECT_TRUE(this->tm_.get("any")->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(this->tm_.get("any")));
  EXPECT_TRUE(type1->matchNice(type1));
  EXPECT_TRUE(type1->matchNice(type2));
  EXPECT_FALSE(type1->matchNice(type3));
  EXPECT_FALSE(type1->matchNice(type4));
  EXPECT_FALSE(type1->matchNice(type5));
  EXPECT_FALSE(type1->matchNice(type6));
  EXPECT_FALSE(type1->matchNice(type7));
  EXPECT_TRUE(type1->matchNice(type8));
  EXPECT_TRUE(type8->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(type9));
  EXPECT_FALSE(type9->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(type10));
  EXPECT_FALSE(type10->matchNice(type1));
  EXPECT_FALSE(type1->matchNice(type11));
  EXPECT_FALSE(type11->matchNice(type1));
  EXPECT_FALSE(type10->matchNice(type11));
  EXPECT_FALSE(type11->matchNice(type10));
}

TEST_F(TypeTest, MatchesNiceObject) {
  this->tm_.obj("Test1", "Test1_0", {
    TypeObjField{"a", this->tm_.get("int")},
    TypeObjField{"b", this->tm_.get("int")}
  });

  this->tm_.obj("Test2", "Test2_0");

  this->tm_.obj("Test3", "Test3_0", {
    TypeObjField{"a", this->tm_.get("int")}
  });

  this->tm_.obj("Test4", "Test4_0", {
    TypeObjField{"a", this->tm_.get("int")},
    TypeObjField{"b", this->tm_.get("int")}
  });

  this->tm_.obj("Test5", "Test5_0", {
    TypeObjField{"a", this->tm_.get("int")},
    TypeObjField{"b", this->tm_.get("str")}
  });

  EXPECT_TRUE(this->tm_.get("any")->matchNice(this->tm_.get("Test1")));
  EXPECT_TRUE(this->tm_.get("Test1")->matchNice(this->tm_.get("Test1")));
  EXPECT_FALSE(this->tm_.get("Test1")->matchNice(this->tm_.get("Test2")));
  EXPECT_FALSE(this->tm_.get("Test1")->matchNice(this->tm_.get("Test3")));
  EXPECT_FALSE(this->tm_.get("Test1")->matchNice(this->tm_.get("Test4")));
  EXPECT_FALSE(this->tm_.get("Test1")->matchNice(this->tm_.get("Test5")));
  EXPECT_FALSE(this->tm_.get("Test1")->matchNice(this->tm_.get("int")));
}

TEST_F(TypeTest, MatchesNiceReference) {
  auto ref1 = this->tm_.ref(this->tm_.get("int"));
  auto ref2 = this->tm_.ref(this->tm_.get("int"));

  EXPECT_TRUE(ref1->matchNice(ref1));
  EXPECT_TRUE(ref1->matchNice(ref2));
  EXPECT_TRUE(ref2->matchNice(ref1));
  EXPECT_FALSE(ref1->matchNice(this->tm_.get("int")));
  EXPECT_FALSE(this->tm_.get("int")->matchNice(ref1));
  EXPECT_FALSE(ref1->matchNice(this->tm_.get("str")));
  EXPECT_FALSE(this->tm_.get("str")->matchNice(ref1));
}
