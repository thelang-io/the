/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gmock/gmock.h>
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
    auto f0 = std::ofstream(this->p0_);
    f0 << this->c0_;
    auto f1 = std::ofstream(this->p1_);
    f1 << this->c1_;
    auto f2 = std::ofstream(this->p2_);
    f2 << this->c2_;
  }

  void TearDown () override {
    fs::remove(this->p0_);
    fs::remove(this->p1_);
    fs::remove(this->p2_);
  }
};

TEST_F(ReaderTest, Read) {
  const auto rl = ReaderLocation{};

  const auto r0 = Reader(this->p0_);
  EXPECT_EQ(r0.content(), this->c0_);
  EXPECT_TRUE(r0.eof());
  EXPECT_EQ(r0.loc(), rl);
  EXPECT_EQ(r0.path(), this->p0_);

  const auto r1 = Reader(this->p1_);
  EXPECT_EQ(r1.content(), this->c1_);
  EXPECT_FALSE(r1.eof());
  EXPECT_EQ(r1.loc(), rl);
  EXPECT_EQ(r1.path(), this->p1_);

  const auto r2 = Reader(this->p2_);
  EXPECT_EQ(r2.content(), this->c2_);
  EXPECT_FALSE(r2.eof());
  EXPECT_EQ(r2.loc(), rl);
  EXPECT_EQ(r2.path(), this->p2_);
}

TEST_F(ReaderTest, Throw) {
  const auto nilFile = this->tmp_ / "test_0.out";
  const auto directory = this->tmp_ / "test_1";
  const auto protFile = this->p1_;

  fs::create_directory(directory);
  fs::permissions(protFile, fs::perms::none);

  EXPECT_THROW((Reader(nilFile)), Error);
  EXPECT_THROW((Reader(directory)), Error);
  EXPECT_THROW((Reader(protFile)), Error);
  fs::remove(directory);

  auto r0 = Reader(this->p0_);
  EXPECT_THROW(r0.next(), Error);
}

TEST_F(ReaderTest, ReadCharByChar) {
  auto r1 = Reader(this->p1_);

  for (size_t i = 0; !r1.eof(); i++) {
    EXPECT_EQ(r1.next(), this->c1_[i]);
    EXPECT_EQ(r1.loc().pos, i + 1);
  }

  EXPECT_TRUE(r1.eof());
  auto r2 = Reader(this->p2_);

  for (size_t i = 0; !r2.eof(); i++) {
    EXPECT_EQ(r2.next(), this->c2_[i]);
    EXPECT_EQ(r2.loc().pos, i + 1);
  }

  EXPECT_TRUE(r2.eof());
}

TEST_F(ReaderTest, Location) {
  auto r2 = Reader(this->p2_);
  auto rl = ReaderLocation{};

  while (!r2.eof()) {
    const auto ch = r2.next();

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
  auto r2 = Reader(this->p2_);
  const auto beg = r2.loc();
  auto mid = ReaderLocation{};
  const auto midPos = this->c2_.length() / 2;

  while (!r2.eof()) {
    r2.next();

    if (r2.loc().pos == midPos) {
      mid = r2.loc();
    }
  }

  const auto end = r2.loc();
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

TEST(ReaderLocationTest, EqualAnotherLocation) {
  EXPECT_EQ(ReaderLocation{}, ReaderLocation{});

  const auto rl1 = ReaderLocation{4, 2, 2};
  const auto rl2 = ReaderLocation{4, 2, 2};
  EXPECT_EQ(rl1, rl2);
}
