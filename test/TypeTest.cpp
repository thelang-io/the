/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include "../src/TypeMap.hpp"

class TypeTest : public testing::Test {
 protected:
  TypeMap typeMap_;

  void SetUp () override {
    this->typeMap_.init();
  }
};

TEST_F(TypeTest, CheckIfAny) {
  EXPECT_TRUE(this->typeMap_.get("any")->isAny());
}

TEST_F(TypeTest, CheckIfBool) {
  EXPECT_TRUE(this->typeMap_.get("bool")->isBool());
}

TEST_F(TypeTest, CheckIfByte) {
  EXPECT_TRUE(this->typeMap_.get("byte")->isByte());
}

TEST_F(TypeTest, CheckIfChar) {
  EXPECT_TRUE(this->typeMap_.get("char")->isChar());
}

TEST_F(TypeTest, CheckIfFloat) {
  EXPECT_TRUE(this->typeMap_.get("float")->isFloat());
}

TEST_F(TypeTest, CheckIfFn) {
  this->typeMap_.add("test1", {}, this->typeMap_.get("int"));

  this->typeMap_.add("test2", {
    {"a", this->typeMap_.get("int"), true, false}
  }, this->typeMap_.get("int"));

  this->typeMap_.add("test3", {
    {"a", this->typeMap_.get("str"), false, false},
    {"a", this->typeMap_.get("int"), false, true}
  }, this->typeMap_.get("str"));

  EXPECT_TRUE(this->typeMap_.get("test1")->isFn());
  EXPECT_TRUE(this->typeMap_.get("test2")->isFn());
  EXPECT_TRUE(this->typeMap_.get("test3")->isFn());
}

TEST_F(TypeTest, CheckIfF32) {
  EXPECT_TRUE(this->typeMap_.get("f32")->isF32());
}

TEST_F(TypeTest, CheckIfF64) {
  EXPECT_TRUE(this->typeMap_.get("f64")->isF64());
}

TEST_F(TypeTest, CheckIfInt) {
  EXPECT_TRUE(this->typeMap_.get("int")->isInt());
}

TEST_F(TypeTest, CheckIfI8) {
  EXPECT_TRUE(this->typeMap_.get("i8")->isI8());
}

TEST_F(TypeTest, CheckIfI16) {
  EXPECT_TRUE(this->typeMap_.get("i16")->isI16());
}

TEST_F(TypeTest, CheckIfI32) {
  EXPECT_TRUE(this->typeMap_.get("i32")->isI32());
}

TEST_F(TypeTest, CheckIfI64) {
  EXPECT_TRUE(this->typeMap_.get("i64")->isI64());
}

TEST_F(TypeTest, CheckIfObj) {
  this->typeMap_.add("Test1", {});

  this->typeMap_.add("Test2", {
    {"a", this->typeMap_.get("int")}
  });

  this->typeMap_.add("Test3", {
    {"a", this->typeMap_.get("int")},
    {"b", this->typeMap_.get("str")}
  });

  EXPECT_TRUE(this->typeMap_.get("Test1")->isObj());
  EXPECT_TRUE(this->typeMap_.get("Test2")->isObj());
  EXPECT_TRUE(this->typeMap_.get("Test3")->isObj());
}

TEST_F(TypeTest, CheckIfStr) {
  EXPECT_TRUE(this->typeMap_.get("str")->isStr());
}

TEST_F(TypeTest, CheckIfU8) {
  EXPECT_TRUE(this->typeMap_.get("u8")->isU8());
}

TEST_F(TypeTest, CheckIfU16) {
  EXPECT_TRUE(this->typeMap_.get("u16")->isU16());
}

TEST_F(TypeTest, CheckIfU32) {
  EXPECT_TRUE(this->typeMap_.get("u32")->isU32());
}

TEST_F(TypeTest, CheckIfU64) {
  EXPECT_TRUE(this->typeMap_.get("u64")->isU64());
}

TEST_F(TypeTest, CheckIfVoid) {
  EXPECT_TRUE(this->typeMap_.get("void")->isVoid());
}

TEST_F(TypeTest, Matches) {
  EXPECT_TRUE(this->typeMap_.get("bool")->match(this->typeMap_.get("bool")));
  EXPECT_TRUE(this->typeMap_.get("byte")->match(this->typeMap_.get("byte")));
  EXPECT_TRUE(this->typeMap_.get("char")->match(this->typeMap_.get("char")));
  EXPECT_TRUE(this->typeMap_.get("void")->match(this->typeMap_.get("void")));
}

TEST_F(TypeTest, MatchesAny) {
  EXPECT_TRUE(this->typeMap_.get("any")->match(this->typeMap_.get("any")));
  EXPECT_TRUE(this->typeMap_.get("any")->match(this->typeMap_.get("bool")));
  EXPECT_TRUE(this->typeMap_.get("any")->match(this->typeMap_.get("byte")));
  EXPECT_TRUE(this->typeMap_.get("any")->match(this->typeMap_.get("char")));
  EXPECT_TRUE(this->typeMap_.get("any")->match(this->typeMap_.get("float")));
  EXPECT_TRUE(this->typeMap_.get("any")->match(this->typeMap_.get("f32")));
  EXPECT_TRUE(this->typeMap_.get("any")->match(this->typeMap_.get("f64")));
  EXPECT_TRUE(this->typeMap_.get("any")->match(this->typeMap_.get("int")));
  EXPECT_TRUE(this->typeMap_.get("any")->match(this->typeMap_.get("i8")));
  EXPECT_TRUE(this->typeMap_.get("any")->match(this->typeMap_.get("i16")));
  EXPECT_TRUE(this->typeMap_.get("any")->match(this->typeMap_.get("i32")));
  EXPECT_TRUE(this->typeMap_.get("any")->match(this->typeMap_.get("i64")));
  EXPECT_TRUE(this->typeMap_.get("any")->match(this->typeMap_.get("str")));
  EXPECT_TRUE(this->typeMap_.get("any")->match(this->typeMap_.get("u8")));
  EXPECT_TRUE(this->typeMap_.get("any")->match(this->typeMap_.get("u16")));
  EXPECT_TRUE(this->typeMap_.get("any")->match(this->typeMap_.get("u32")));
  EXPECT_TRUE(this->typeMap_.get("any")->match(this->typeMap_.get("u64")));
  EXPECT_TRUE(this->typeMap_.get("any")->match(this->typeMap_.get("void")));
}

TEST_F(TypeTest, MatchesFloat) {
  EXPECT_TRUE(this->typeMap_.get("f32")->match(this->typeMap_.get("i8")));
  EXPECT_TRUE(this->typeMap_.get("f32")->match(this->typeMap_.get("i16")));
  EXPECT_TRUE(this->typeMap_.get("f32")->match(this->typeMap_.get("i32")));
  EXPECT_TRUE(this->typeMap_.get("f32")->match(this->typeMap_.get("int")));
  EXPECT_FALSE(this->typeMap_.get("f32")->match(this->typeMap_.get("i64")));
  EXPECT_TRUE(this->typeMap_.get("f32")->match(this->typeMap_.get("u8")));
  EXPECT_TRUE(this->typeMap_.get("f32")->match(this->typeMap_.get("u16")));
  EXPECT_FALSE(this->typeMap_.get("f32")->match(this->typeMap_.get("u32")));
  EXPECT_FALSE(this->typeMap_.get("f32")->match(this->typeMap_.get("u64")));
  EXPECT_TRUE(this->typeMap_.get("f32")->match(this->typeMap_.get("f32")));
  EXPECT_FALSE(this->typeMap_.get("f32")->match(this->typeMap_.get("f64")));
  EXPECT_FALSE(this->typeMap_.get("f32")->match(this->typeMap_.get("float")));
  EXPECT_TRUE(this->typeMap_.get("f64")->match(this->typeMap_.get("i8")));
  EXPECT_TRUE(this->typeMap_.get("f64")->match(this->typeMap_.get("i16")));
  EXPECT_TRUE(this->typeMap_.get("f64")->match(this->typeMap_.get("i32")));
  EXPECT_TRUE(this->typeMap_.get("f64")->match(this->typeMap_.get("int")));
  EXPECT_TRUE(this->typeMap_.get("f64")->match(this->typeMap_.get("i64")));
  EXPECT_TRUE(this->typeMap_.get("f64")->match(this->typeMap_.get("u8")));
  EXPECT_TRUE(this->typeMap_.get("f64")->match(this->typeMap_.get("u16")));
  EXPECT_TRUE(this->typeMap_.get("f64")->match(this->typeMap_.get("u32")));
  EXPECT_FALSE(this->typeMap_.get("f64")->match(this->typeMap_.get("u64")));
  EXPECT_TRUE(this->typeMap_.get("f64")->match(this->typeMap_.get("f32")));
  EXPECT_TRUE(this->typeMap_.get("f64")->match(this->typeMap_.get("f64")));
  EXPECT_TRUE(this->typeMap_.get("f64")->match(this->typeMap_.get("float")));
  EXPECT_TRUE(this->typeMap_.get("float")->match(this->typeMap_.get("i8")));
  EXPECT_TRUE(this->typeMap_.get("float")->match(this->typeMap_.get("i16")));
  EXPECT_TRUE(this->typeMap_.get("float")->match(this->typeMap_.get("i32")));
  EXPECT_TRUE(this->typeMap_.get("float")->match(this->typeMap_.get("int")));
  EXPECT_TRUE(this->typeMap_.get("float")->match(this->typeMap_.get("i64")));
  EXPECT_TRUE(this->typeMap_.get("float")->match(this->typeMap_.get("u8")));
  EXPECT_TRUE(this->typeMap_.get("float")->match(this->typeMap_.get("u16")));
  EXPECT_TRUE(this->typeMap_.get("float")->match(this->typeMap_.get("u32")));
  EXPECT_FALSE(this->typeMap_.get("float")->match(this->typeMap_.get("u64")));
  EXPECT_TRUE(this->typeMap_.get("float")->match(this->typeMap_.get("f32")));
  EXPECT_TRUE(this->typeMap_.get("float")->match(this->typeMap_.get("f64")));
  EXPECT_TRUE(this->typeMap_.get("float")->match(this->typeMap_.get("float")));
}

TEST_F(TypeTest, MatchesInteger) {
  EXPECT_TRUE(this->typeMap_.get("i8")->match(this->typeMap_.get("i8")));
  EXPECT_FALSE(this->typeMap_.get("i8")->match(this->typeMap_.get("i16")));
  EXPECT_FALSE(this->typeMap_.get("i8")->match(this->typeMap_.get("i32")));
  EXPECT_FALSE(this->typeMap_.get("i8")->match(this->typeMap_.get("int")));
  EXPECT_FALSE(this->typeMap_.get("i8")->match(this->typeMap_.get("i64")));
  EXPECT_FALSE(this->typeMap_.get("i8")->match(this->typeMap_.get("u8")));
  EXPECT_FALSE(this->typeMap_.get("i8")->match(this->typeMap_.get("u16")));
  EXPECT_FALSE(this->typeMap_.get("i8")->match(this->typeMap_.get("u32")));
  EXPECT_FALSE(this->typeMap_.get("i8")->match(this->typeMap_.get("u64")));
  EXPECT_FALSE(this->typeMap_.get("i8")->match(this->typeMap_.get("f32")));
  EXPECT_FALSE(this->typeMap_.get("i8")->match(this->typeMap_.get("f64")));
  EXPECT_FALSE(this->typeMap_.get("i8")->match(this->typeMap_.get("float")));

  EXPECT_TRUE(this->typeMap_.get("i16")->match(this->typeMap_.get("i8")));
  EXPECT_TRUE(this->typeMap_.get("i16")->match(this->typeMap_.get("i16")));
  EXPECT_FALSE(this->typeMap_.get("i16")->match(this->typeMap_.get("i32")));
  EXPECT_FALSE(this->typeMap_.get("i16")->match(this->typeMap_.get("int")));
  EXPECT_FALSE(this->typeMap_.get("i16")->match(this->typeMap_.get("i64")));
  EXPECT_TRUE(this->typeMap_.get("i16")->match(this->typeMap_.get("u8")));
  EXPECT_FALSE(this->typeMap_.get("i16")->match(this->typeMap_.get("u16")));
  EXPECT_FALSE(this->typeMap_.get("i16")->match(this->typeMap_.get("u32")));
  EXPECT_FALSE(this->typeMap_.get("i16")->match(this->typeMap_.get("u64")));
  EXPECT_FALSE(this->typeMap_.get("i16")->match(this->typeMap_.get("f32")));
  EXPECT_FALSE(this->typeMap_.get("i16")->match(this->typeMap_.get("f64")));
  EXPECT_FALSE(this->typeMap_.get("i16")->match(this->typeMap_.get("float")));

  EXPECT_TRUE(this->typeMap_.get("i32")->match(this->typeMap_.get("i8")));
  EXPECT_TRUE(this->typeMap_.get("i32")->match(this->typeMap_.get("i16")));
  EXPECT_TRUE(this->typeMap_.get("i32")->match(this->typeMap_.get("i32")));
  EXPECT_TRUE(this->typeMap_.get("i32")->match(this->typeMap_.get("int")));
  EXPECT_FALSE(this->typeMap_.get("i32")->match(this->typeMap_.get("i64")));
  EXPECT_TRUE(this->typeMap_.get("i32")->match(this->typeMap_.get("u8")));
  EXPECT_TRUE(this->typeMap_.get("i32")->match(this->typeMap_.get("u16")));
  EXPECT_FALSE(this->typeMap_.get("i32")->match(this->typeMap_.get("u32")));
  EXPECT_FALSE(this->typeMap_.get("i32")->match(this->typeMap_.get("u64")));
  EXPECT_FALSE(this->typeMap_.get("i32")->match(this->typeMap_.get("f32")));
  EXPECT_FALSE(this->typeMap_.get("i32")->match(this->typeMap_.get("f64")));
  EXPECT_FALSE(this->typeMap_.get("i32")->match(this->typeMap_.get("float")));

  EXPECT_TRUE(this->typeMap_.get("int")->match(this->typeMap_.get("i8")));
  EXPECT_TRUE(this->typeMap_.get("int")->match(this->typeMap_.get("i16")));
  EXPECT_TRUE(this->typeMap_.get("int")->match(this->typeMap_.get("i32")));
  EXPECT_TRUE(this->typeMap_.get("int")->match(this->typeMap_.get("int")));
  EXPECT_FALSE(this->typeMap_.get("int")->match(this->typeMap_.get("i64")));
  EXPECT_TRUE(this->typeMap_.get("int")->match(this->typeMap_.get("u8")));
  EXPECT_TRUE(this->typeMap_.get("int")->match(this->typeMap_.get("u16")));
  EXPECT_FALSE(this->typeMap_.get("int")->match(this->typeMap_.get("u32")));
  EXPECT_FALSE(this->typeMap_.get("int")->match(this->typeMap_.get("u64")));
  EXPECT_FALSE(this->typeMap_.get("int")->match(this->typeMap_.get("f32")));
  EXPECT_FALSE(this->typeMap_.get("int")->match(this->typeMap_.get("f64")));
  EXPECT_FALSE(this->typeMap_.get("int")->match(this->typeMap_.get("float")));

  EXPECT_TRUE(this->typeMap_.get("i64")->match(this->typeMap_.get("i8")));
  EXPECT_TRUE(this->typeMap_.get("i64")->match(this->typeMap_.get("i16")));
  EXPECT_TRUE(this->typeMap_.get("i64")->match(this->typeMap_.get("i32")));
  EXPECT_TRUE(this->typeMap_.get("i64")->match(this->typeMap_.get("int")));
  EXPECT_TRUE(this->typeMap_.get("i64")->match(this->typeMap_.get("i64")));
  EXPECT_TRUE(this->typeMap_.get("i64")->match(this->typeMap_.get("u8")));
  EXPECT_TRUE(this->typeMap_.get("i64")->match(this->typeMap_.get("u16")));
  EXPECT_TRUE(this->typeMap_.get("i64")->match(this->typeMap_.get("u32")));
  EXPECT_FALSE(this->typeMap_.get("i64")->match(this->typeMap_.get("u64")));
  EXPECT_FALSE(this->typeMap_.get("i64")->match(this->typeMap_.get("f32")));
  EXPECT_FALSE(this->typeMap_.get("i64")->match(this->typeMap_.get("f64")));
  EXPECT_FALSE(this->typeMap_.get("i64")->match(this->typeMap_.get("float")));

  EXPECT_FALSE(this->typeMap_.get("u8")->match(this->typeMap_.get("i8")));
  EXPECT_FALSE(this->typeMap_.get("u8")->match(this->typeMap_.get("i16")));
  EXPECT_FALSE(this->typeMap_.get("u8")->match(this->typeMap_.get("i32")));
  EXPECT_FALSE(this->typeMap_.get("u8")->match(this->typeMap_.get("int")));
  EXPECT_FALSE(this->typeMap_.get("u8")->match(this->typeMap_.get("i64")));
  EXPECT_TRUE(this->typeMap_.get("u8")->match(this->typeMap_.get("u8")));
  EXPECT_FALSE(this->typeMap_.get("u8")->match(this->typeMap_.get("u16")));
  EXPECT_FALSE(this->typeMap_.get("u8")->match(this->typeMap_.get("u32")));
  EXPECT_FALSE(this->typeMap_.get("u8")->match(this->typeMap_.get("u64")));
  EXPECT_FALSE(this->typeMap_.get("u8")->match(this->typeMap_.get("f32")));
  EXPECT_FALSE(this->typeMap_.get("u8")->match(this->typeMap_.get("f64")));
  EXPECT_FALSE(this->typeMap_.get("u8")->match(this->typeMap_.get("float")));

  EXPECT_FALSE(this->typeMap_.get("u16")->match(this->typeMap_.get("i8")));
  EXPECT_FALSE(this->typeMap_.get("u16")->match(this->typeMap_.get("i16")));
  EXPECT_FALSE(this->typeMap_.get("u16")->match(this->typeMap_.get("i32")));
  EXPECT_FALSE(this->typeMap_.get("u16")->match(this->typeMap_.get("int")));
  EXPECT_FALSE(this->typeMap_.get("u16")->match(this->typeMap_.get("i64")));
  EXPECT_TRUE(this->typeMap_.get("u16")->match(this->typeMap_.get("u8")));
  EXPECT_TRUE(this->typeMap_.get("u16")->match(this->typeMap_.get("u16")));
  EXPECT_FALSE(this->typeMap_.get("u16")->match(this->typeMap_.get("u32")));
  EXPECT_FALSE(this->typeMap_.get("u16")->match(this->typeMap_.get("u64")));
  EXPECT_FALSE(this->typeMap_.get("u16")->match(this->typeMap_.get("f32")));
  EXPECT_FALSE(this->typeMap_.get("u16")->match(this->typeMap_.get("f64")));
  EXPECT_FALSE(this->typeMap_.get("u16")->match(this->typeMap_.get("float")));

  EXPECT_FALSE(this->typeMap_.get("u32")->match(this->typeMap_.get("i8")));
  EXPECT_FALSE(this->typeMap_.get("u32")->match(this->typeMap_.get("i16")));
  EXPECT_FALSE(this->typeMap_.get("u32")->match(this->typeMap_.get("i32")));
  EXPECT_FALSE(this->typeMap_.get("u32")->match(this->typeMap_.get("int")));
  EXPECT_FALSE(this->typeMap_.get("u32")->match(this->typeMap_.get("i64")));
  EXPECT_TRUE(this->typeMap_.get("u32")->match(this->typeMap_.get("u8")));
  EXPECT_TRUE(this->typeMap_.get("u32")->match(this->typeMap_.get("u16")));
  EXPECT_TRUE(this->typeMap_.get("u32")->match(this->typeMap_.get("u32")));
  EXPECT_FALSE(this->typeMap_.get("u32")->match(this->typeMap_.get("u64")));
  EXPECT_FALSE(this->typeMap_.get("u32")->match(this->typeMap_.get("f32")));
  EXPECT_FALSE(this->typeMap_.get("u32")->match(this->typeMap_.get("f64")));
  EXPECT_FALSE(this->typeMap_.get("u32")->match(this->typeMap_.get("float")));

  EXPECT_FALSE(this->typeMap_.get("u64")->match(this->typeMap_.get("i8")));
  EXPECT_FALSE(this->typeMap_.get("u64")->match(this->typeMap_.get("i16")));
  EXPECT_FALSE(this->typeMap_.get("u64")->match(this->typeMap_.get("i32")));
  EXPECT_FALSE(this->typeMap_.get("u64")->match(this->typeMap_.get("int")));
  EXPECT_FALSE(this->typeMap_.get("u64")->match(this->typeMap_.get("i64")));
  EXPECT_TRUE(this->typeMap_.get("u64")->match(this->typeMap_.get("u8")));
  EXPECT_TRUE(this->typeMap_.get("u64")->match(this->typeMap_.get("u16")));
  EXPECT_TRUE(this->typeMap_.get("u64")->match(this->typeMap_.get("u32")));
  EXPECT_TRUE(this->typeMap_.get("u64")->match(this->typeMap_.get("u64")));
  EXPECT_FALSE(this->typeMap_.get("u64")->match(this->typeMap_.get("f32")));
  EXPECT_FALSE(this->typeMap_.get("u64")->match(this->typeMap_.get("f64")));
  EXPECT_FALSE(this->typeMap_.get("u64")->match(this->typeMap_.get("float")));
}

TEST_F(TypeTest, MatchesFunction) {
  this->typeMap_.add("test1", {
    {"a", this->typeMap_.get("int"), true, false},
    {"b", this->typeMap_.get("int"), false, true}
  }, this->typeMap_.get("int"));

  this->typeMap_.add("test2", {
    {"a", this->typeMap_.get("int"), true, false},
    {"b", this->typeMap_.get("int"), false, true}
  }, this->typeMap_.get("int"));

  this->typeMap_.add("test3", {
    {"a", this->typeMap_.get("int"), true, false},
    {"b", this->typeMap_.get("int"), true, false}
  }, this->typeMap_.get("int"));

  this->typeMap_.add("test4", {
    {"a", this->typeMap_.get("int"), true, false},
    {"b", this->typeMap_.get("str"), false, true}
  }, this->typeMap_.get("int"));

  this->typeMap_.add("test5", {
    {"a", this->typeMap_.get("int"), true, false},
    {"b", this->typeMap_.get("int"), false, true}
  }, this->typeMap_.get("str"));

  this->typeMap_.add("test6", {}, this->typeMap_.get("int"));

  this->typeMap_.add("test7", {
    {"a", this->typeMap_.get("int"), true, false}
  }, this->typeMap_.get("int"));

  EXPECT_TRUE(this->typeMap_.get("any")->match(this->typeMap_.get("test1")));
  EXPECT_TRUE(this->typeMap_.get("test1")->match(this->typeMap_.get("test1")));
  EXPECT_TRUE(this->typeMap_.get("test1")->match(this->typeMap_.get("test2")));
  EXPECT_FALSE(this->typeMap_.get("test1")->match(this->typeMap_.get("test3")));
  EXPECT_FALSE(this->typeMap_.get("test1")->match(this->typeMap_.get("test4")));
  EXPECT_FALSE(this->typeMap_.get("test1")->match(this->typeMap_.get("test5")));
  EXPECT_FALSE(this->typeMap_.get("test1")->match(this->typeMap_.get("test6")));
  EXPECT_FALSE(this->typeMap_.get("test1")->match(this->typeMap_.get("test7")));
  EXPECT_FALSE(this->typeMap_.get("test1")->match(this->typeMap_.get("any")));
}

TEST_F(TypeTest, MatchesObject) {
  this->typeMap_.add("Test1", {
    {"a", this->typeMap_.get("int")},
    {"b", this->typeMap_.get("int")}
  });

  this->typeMap_.add("Test2", {});

  this->typeMap_.add("Test3", {
    {"a", this->typeMap_.get("int")}
  });

  this->typeMap_.add("Test4", {
    {"a", this->typeMap_.get("int")},
    {"b", this->typeMap_.get("int")}
  });

  this->typeMap_.add("Test5", {
    {"a", this->typeMap_.get("int")},
    {"b", this->typeMap_.get("str")}
  });

  EXPECT_TRUE(this->typeMap_.get("any")->match(this->typeMap_.get("Test1")));
  EXPECT_TRUE(this->typeMap_.get("Test1")->match(this->typeMap_.get("Test1")));
  EXPECT_FALSE(this->typeMap_.get("Test1")->match(this->typeMap_.get("Test2")));
  EXPECT_FALSE(this->typeMap_.get("Test1")->match(this->typeMap_.get("Test3")));
  EXPECT_FALSE(this->typeMap_.get("Test1")->match(this->typeMap_.get("Test4")));
  EXPECT_FALSE(this->typeMap_.get("Test1")->match(this->typeMap_.get("Test5")));
  EXPECT_FALSE(this->typeMap_.get("Test1")->match(this->typeMap_.get("int")));
}

TEST_F(TypeTest, MatchesString) {
  EXPECT_TRUE(this->typeMap_.get("str")->match(this->typeMap_.get("char")));
  EXPECT_TRUE(this->typeMap_.get("str")->match(this->typeMap_.get("str")));
}
