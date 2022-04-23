/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include "../src/VarMap.hpp"

class VarStackTest : public testing::Test {
 protected:
  VarMap vm_;
  TypeMap tm_;

  void SetUp () override {
    this->tm_.init();
    this->vm_.init(this->tm_);
  }
};

TEST_F(VarStackTest, Constructs) {
  this->vm_.add("test1", this->vm_.name("test1"), this->tm_.get("int"), false);
  this->vm_.add("test2", this->vm_.name("test2"), this->tm_.get("str"), false);
  this->vm_.add("test3", this->vm_.name("test3"), this->tm_.get("any"), false);

  EXPECT_NO_THROW(VarStack({}));
  EXPECT_NO_THROW(VarStack({this->vm_.get("test1")}));
  EXPECT_NO_THROW(VarStack({this->vm_.get("test2"), this->vm_.get("test3")}));
}

TEST_F(VarStackTest, MarksAndSnapshot) {
  this->vm_.add("test1", this->vm_.name("test1"), this->tm_.get("int"), false);
  this->vm_.add("test2", this->vm_.name("test2"), this->tm_.get("str"), false);
  this->vm_.add("test3", this->vm_.name("test3"), this->tm_.get("any"), false);

  auto t1 = this->vm_.get("test1");
  auto t2 = this->vm_.get("test2");
  auto t3 = this->vm_.get("test3");
  auto vs = VarStack({t1, t2, t3});

  auto s1 = vs.snapshot();
  EXPECT_EQ(s1.size(), 0);

  vs.mark(t1);
  auto s2 = vs.snapshot();
  EXPECT_EQ(s2.size(), 1);

  vs.mark(t2);
  auto s3 = vs.snapshot();
  EXPECT_EQ(s3.size(), 2);
}
