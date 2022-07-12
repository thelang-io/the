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
}

TEST_F(TypeTest, LargestNonNumbers) {
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("any"), this->tm_.get("any")), "Error: tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("any"), this->tm_.get("int")), "Error: tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("int"), this->tm_.get("any")), "Error: tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("bool"), this->tm_.get("bool")), "Error: tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("bool"), this->tm_.get("int")), "Error: tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("int"), this->tm_.get("bool")), "Error: tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("byte"), this->tm_.get("byte")), "Error: tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("byte"), this->tm_.get("int")), "Error: tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("int"), this->tm_.get("byte")), "Error: tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("char"), this->tm_.get("char")), "Error: tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("char"), this->tm_.get("int")), "Error: tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("int"), this->tm_.get("char")), "Error: tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("str"), this->tm_.get("str")), "Error: tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("str"), this->tm_.get("int")), "Error: tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("int"), this->tm_.get("str")), "Error: tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("void"), this->tm_.get("void")), "Error: tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("void"), this->tm_.get("int")), "Error: tried to find largest type of non-number");
  EXPECT_THROW_WITH_MESSAGE(Type::largest(this->tm_.get("int"), this->tm_.get("void")), "Error: tried to find largest type of non-number");
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
  this->tm_.add("test1", {}, this->tm_.get("int"));

  this->tm_.add("test2", {
    {"a", this->tm_.get("int"), true, false}
  }, this->tm_.get("int"));

  this->tm_.add("test3", {
    {"a", this->tm_.get("str"), false, false},
    {"b", this->tm_.get("int"), false, true}
  }, this->tm_.get("str"));

  EXPECT_TRUE(this->tm_.get("test1")->isFn());
  EXPECT_TRUE(this->tm_.get("test2")->isFn());
  EXPECT_TRUE(this->tm_.get("test3")->isFn());

  EXPECT_TRUE(this->tm_.fn({}, this->tm_.get("void"))->isFn());

  EXPECT_TRUE(this->tm_.fn({
    {nullptr, this->tm_.get("int"), true, false}
  }, this->tm_.get("void"))->isFn());

  EXPECT_TRUE(this->tm_.fn({
    {nullptr, this->tm_.get("str"), false, false},
    {nullptr, this->tm_.get("int"), false, true}
  }, this->tm_.get("str"))->isFn());
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
  this->tm_.add("Test1", {});

  this->tm_.add("Test2", {
    {"a", this->tm_.get("int")}
  });

  this->tm_.add("Test3", {
    {"a", this->tm_.get("int")},
    {"b", this->tm_.get("str")}
  });

  EXPECT_TRUE(this->tm_.get("Test1")->isObj());
  EXPECT_TRUE(this->tm_.get("Test2")->isObj());
  EXPECT_TRUE(this->tm_.get("Test3")->isObj());
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

TEST_F(TypeTest, MatchesFunction) {
  this->tm_.add("test1", {
    {"a", this->tm_.get("int"), true, false},
    {"b", this->tm_.get("int"), false, true}
  }, this->tm_.get("int"));

  this->tm_.add("test2", {
    {"a", this->tm_.get("int"), true, false},
    {"b", this->tm_.get("int"), false, true}
  }, this->tm_.get("int"));

  this->tm_.add("test3", {
    {"a", this->tm_.get("int"), true, false},
    {"b", this->tm_.get("int"), true, false}
  }, this->tm_.get("int"));

  this->tm_.add("test4", {
    {"a", this->tm_.get("int"), true, false},
    {"b", this->tm_.get("str"), false, true}
  }, this->tm_.get("int"));

  this->tm_.add("test5", {
    {"a", this->tm_.get("int"), true, false},
    {"b", this->tm_.get("int"), false, true}
  }, this->tm_.get("str"));

  this->tm_.add("test6", {}, this->tm_.get("int"));

  this->tm_.add("test7", {
    {"a", this->tm_.get("int"), true, false}
  }, this->tm_.get("int"));

  this->tm_.add("test8", {
    {nullptr, this->tm_.get("int"), true, false},
    {nullptr, this->tm_.get("int"), false, true}
  }, this->tm_.get("int"));

  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("test1")));
  EXPECT_TRUE(this->tm_.get("test1")->match(this->tm_.get("test1")));
  EXPECT_TRUE(this->tm_.get("test1")->match(this->tm_.get("test2")));
  EXPECT_FALSE(this->tm_.get("test1")->match(this->tm_.get("test3")));
  EXPECT_FALSE(this->tm_.get("test1")->match(this->tm_.get("test4")));
  EXPECT_FALSE(this->tm_.get("test1")->match(this->tm_.get("test5")));
  EXPECT_FALSE(this->tm_.get("test1")->match(this->tm_.get("test6")));
  EXPECT_FALSE(this->tm_.get("test1")->match(this->tm_.get("test7")));
  EXPECT_FALSE(this->tm_.get("test1")->match(this->tm_.get("test8")));
  EXPECT_FALSE(this->tm_.get("test1")->match(this->tm_.get("any")));
}

TEST_F(TypeTest, MatchesObject) {
  this->tm_.add("Test1", {
    {"a", this->tm_.get("int")},
    {"b", this->tm_.get("int")}
  });

  this->tm_.add("Test2", {});

  this->tm_.add("Test3", {
    {"a", this->tm_.get("int")}
  });

  this->tm_.add("Test4", {
    {"a", this->tm_.get("int")},
    {"b", this->tm_.get("int")}
  });

  this->tm_.add("Test5", {
    {"a", this->tm_.get("int")},
    {"b", this->tm_.get("str")}
  });

  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("Test1")));
  EXPECT_TRUE(this->tm_.get("Test1")->match(this->tm_.get("Test1")));
  EXPECT_FALSE(this->tm_.get("Test1")->match(this->tm_.get("Test2")));
  EXPECT_FALSE(this->tm_.get("Test1")->match(this->tm_.get("Test3")));
  EXPECT_FALSE(this->tm_.get("Test1")->match(this->tm_.get("Test4")));
  EXPECT_FALSE(this->tm_.get("Test1")->match(this->tm_.get("Test5")));
  EXPECT_FALSE(this->tm_.get("Test1")->match(this->tm_.get("int")));
}
