/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include <fstream>
#include "../src/Error.hpp"
#include "../src/Reader.hpp"

class ReaderTest : public ::testing::Test {
 protected:
  const fs::path tmp_ = fs::canonical(fs::temp_directory_path());
  const fs::path p0_ = this->tmp_ / "test0.out";
  const std::string c0_ = "";
  const fs::path p1_ = this->tmp_ / "test1.out";
  const std::string c1_ = "Lorem ipsum dolor sit amet";
  const fs::path p2_ = this->tmp_ / "test2.out";
  const std::string c2_ = "Lorem ipsum\ndolor sit amet\n";

  void SetUp () override {
    std::ofstream f0(this->p0_);
    f0 << this->c0_;
    std::ofstream f1(this->p1_);
    f1 << this->c1_;
    std::ofstream f2(this->p2_);
    f2 << this->c2_;
  }

  void TearDown () override {
    fs::remove(this->p0_);
    fs::remove(this->p1_);
    fs::remove(this->p2_);
  }
};

TEST_F(ReaderTest, Read) {
  const ReaderLocation rl{};

  const Reader r0(this->p0_);
  EXPECT_EQ(r0.content(), this->c0_);
  EXPECT_TRUE(r0.eof());
  EXPECT_EQ(r0.loc(), rl);
  EXPECT_EQ(r0.path(), this->p0_);

  const Reader r1(this->p1_);
  EXPECT_EQ(r1.content(), this->c1_);
  EXPECT_FALSE(r1.eof());
  EXPECT_EQ(r1.loc(), rl);
  EXPECT_EQ(r1.path(), this->p1_);

  const Reader r2(this->p2_);
  EXPECT_EQ(r2.content(), this->c2_);
  EXPECT_FALSE(r2.eof());
  EXPECT_EQ(r2.loc(), rl);
  EXPECT_EQ(r2.path(), this->p2_);
}

TEST_F(ReaderTest, Throw) {
  const fs::path nilFile = this->tmp_ / "test_0.out";
  const fs::path directory = this->tmp_ / "test_1";
  const fs::path protFile = this->p1_;

  fs::create_directory(directory);
  fs::permissions(protFile, fs::perms::none);

  EXPECT_THROW((Reader(nilFile)), Error);
  EXPECT_THROW((Reader(directory)), Error);
  EXPECT_THROW((Reader(protFile)), Error);
  fs::remove(directory);

  Reader r0(this->p0_);
  EXPECT_THROW(r0.next(), Error);
}

TEST_F(ReaderTest, ReadCharByChar) {
  Reader r1(this->p1_);

  for (size_t i = 0; !r1.eof(); i++) {
    EXPECT_EQ(r1.next(), this->c1_[i]);
    EXPECT_EQ(r1.loc().pos, i + 1);
  }

  EXPECT_TRUE(r1.eof());
  Reader r2(this->p2_);

  for (size_t i = 0; !r2.eof(); i++) {
    EXPECT_EQ(r2.next(), this->c2_[i]);
    EXPECT_EQ(r2.loc().pos, i + 1);
  }

  EXPECT_TRUE(r2.eof());
}

TEST_F(ReaderTest, Location) {
  Reader r2(this->p2_);
  ReaderLocation rl{};

  while (!r2.eof()) {
    char ch = r2.next();

    if (ch == '\n') {
      rl.col = 0;
      rl.line += 1;
    } else {
      rl.col += 1;
    }

    rl.pos += 1;
    EXPECT_EQ(r2.loc(), rl);
  }
}

TEST_F(ReaderTest, Seek) {
  Reader r2(this->p2_);
  const ReaderLocation beg = r2.loc();
  ReaderLocation mid;
  const size_t midPos = this->c2_.length() / 2;
  ReaderLocation end;

  while (!r2.eof()) {
    r2.next();

    if (r2.loc().pos == midPos) {
      mid = r2.loc();
    }
  }

  end = r2.loc();
  r2.seek(beg);

  for (size_t i = 0; !r2.eof(); i++) {
    EXPECT_EQ(r2.next(), this->c2_[i]);
    EXPECT_EQ(r2.loc().pos, i + 1);
  }

  r2.seek(mid);

  for (size_t i = midPos; !r2.eof(); i++) {
    EXPECT_EQ(r2.next(), this->c2_[i]);
    EXPECT_EQ(r2.loc().pos, i + 1);
  }

  r2.seek(beg);
  r2.seek(end);
  EXPECT_TRUE(r2.eof());
}
