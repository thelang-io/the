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
  TypeMap tm_;

  void SetUp () override {
    this->tm_.init();
  }
};

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

TEST_F(TypeTest, CheckIfFloat) {
  EXPECT_TRUE(this->tm_.get("float")->isFloat());
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

  EXPECT_TRUE(TypeMap::fn(this->tm_.get("void"), {})->isFn());

  EXPECT_TRUE(TypeMap::fn(this->tm_.get("void"), {
    {"a", this->tm_.get("int"), true, false}
  })->isFn());

  EXPECT_TRUE(TypeMap::fn(this->tm_.get("str"), {
    {"a", this->tm_.get("str"), false, false},
    {"b", this->tm_.get("int"), false, true}
  })->isFn());
}

TEST_F(TypeTest, CheckIfF32) {
  EXPECT_TRUE(this->tm_.get("f32")->isF32());
}

TEST_F(TypeTest, CheckIfF64) {
  EXPECT_TRUE(this->tm_.get("f64")->isF64());
}

TEST_F(TypeTest, CheckIfInt) {
  EXPECT_TRUE(this->tm_.get("int")->isInt());
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

  EXPECT_TRUE(this->tm_.get("any")->match(this->tm_.get("test1")));
  EXPECT_TRUE(this->tm_.get("test1")->match(this->tm_.get("test1")));
  EXPECT_TRUE(this->tm_.get("test1")->match(this->tm_.get("test2")));
  EXPECT_FALSE(this->tm_.get("test1")->match(this->tm_.get("test3")));
  EXPECT_FALSE(this->tm_.get("test1")->match(this->tm_.get("test4")));
  EXPECT_FALSE(this->tm_.get("test1")->match(this->tm_.get("test5")));
  EXPECT_FALSE(this->tm_.get("test1")->match(this->tm_.get("test6")));
  EXPECT_FALSE(this->tm_.get("test1")->match(this->tm_.get("test7")));
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
