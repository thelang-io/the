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
#include "../src/VarMap.hpp"

class VarStackTest : public testing::Test {
 protected:
  VarMap vm_;
  TypeMap tm_;

  void SetUp () override {
    this->tm_.init("test");
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
  auto t1 = this->vm_.add("test1", this->vm_.name("test1"), this->tm_.get("int"), false);
  auto t2 = this->vm_.add("test2", this->vm_.name("test2"), this->tm_.get("str"), false);
  auto t3 = this->vm_.add("test3", this->vm_.name("test3"), this->tm_.get("any"), false);
  auto t4 = this->vm_.add("test4", this->vm_.name("test4"), this->tm_.get("str"), false);
  auto t5 = this->vm_.add("test5", this->vm_.name("test5"), this->tm_.get("str"), false);
  auto vs = VarStack({t1, t2, t3, t4, t5});

  auto s1 = vs.snapshot();
  EXPECT_EQ(s1.size(), 0);

  vs.mark(t1);
  auto s2 = vs.snapshot();
  EXPECT_EQ(s2.size(), 1);

  vs.mark(t2);
  auto s3 = vs.snapshot();
  EXPECT_EQ(s3.size(), 2);

  vs.mark({t3, t4});
  auto s4 = vs.snapshot();
  EXPECT_EQ(s4.size(), 4);
}

TEST_F(VarStackTest, MarksAndSnapshotByCodeName) {
  auto t1 = this->vm_.add("test1", this->vm_.name("test1"), this->tm_.get("int"), false);
  auto t2 = this->vm_.add("test2", this->vm_.name("test2"), this->tm_.get("str"), false);
  auto t3 = this->vm_.add("test3", this->vm_.name("test3"), this->tm_.get("any"), false);
  auto t4 = this->vm_.add("test4", this->vm_.name("test4"), this->tm_.get("str"), false);
  auto t5 = this->vm_.add("test5", this->vm_.name("test5"), this->tm_.get("str"), false);
  auto vs = VarStack({t1, t2, t3, t4, t5});

  auto s1 = vs.snapshot();
  EXPECT_EQ(s1.size(), 0);

  vs.mark("test1_0");
  auto s2 = vs.snapshot();
  EXPECT_EQ(s2.size(), 1);

  vs.mark("test2_0");
  auto s3 = vs.snapshot();
  EXPECT_EQ(s3.size(), 2);
}
