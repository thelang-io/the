/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "utils.hpp"

class ReaderTest : public testing::Test {
 protected:
  Reader *r1_;
  Reader *r2_;
  Reader *r3_;

  void SetUp () override {
    auto emptyFile = std::ofstream("test_empty.txt");
    emptyFile.close();

    auto regularFile = std::ofstream("test_regular.txt");
    regularFile << "Hello, World!";
    regularFile.close();

    auto multilineFile = std::ofstream("test_multiline.txt");
    multilineFile << "Hello, Anna!\nHello, Nina!\nHello, Rosa!\n";
    multilineFile.close();

    this->r1_ = new Reader("test_empty.txt");
    this->r2_ = new Reader("test_regular.txt");
    this->r3_ = new Reader("test_multiline.txt");
  }

  void TearDown () override {
    delete this->r1_;
    delete this->r2_;
    delete this->r3_;

    std::filesystem::remove("test_empty.txt");
    std::filesystem::remove("test_regular.txt");
    std::filesystem::remove("test_multiline.txt");
  }
};

TEST_F(ReaderTest, ThrowsOnNotExisting) {
  EXPECT_THROW_WITH_MESSAGE({
    Reader("test.jpg");
  }, R"(Error: no such file "test.jpg")");
}

TEST_F(ReaderTest, ThrowsOnDirectory) {
  EXPECT_THROW_WITH_MESSAGE({
    Reader("/dev/null");
  }, R"(Error: path "/dev/null" is not a file)");
}

TEST_F(ReaderTest, ReadsFile) {
  EXPECT_EQ(this->r2_->loc, (ReaderLocation{0, 1, 0}));
  EXPECT_EQ(this->r2_->path, std::filesystem::current_path() / "test_regular.txt");
  EXPECT_EQ(this->r2_->content, "Hello, World!");
  EXPECT_EQ(this->r2_->size, 13);
}

TEST_F(ReaderTest, NoEofOnNonEmpty) {
  EXPECT_FALSE(this->r2_->eof());
}

TEST_F(ReaderTest, EofOnEmpty) {
  EXPECT_TRUE(this->r1_->eof());
}

TEST_F(ReaderTest, ReadsNext) {
  EXPECT_EQ(this->r2_->next(), 'H');
  EXPECT_EQ(this->r2_->next(), 'e');
  EXPECT_EQ(this->r2_->next(), 'l');
  EXPECT_EQ(this->r2_->next(), 'l');
  EXPECT_EQ(this->r2_->next(), 'o');
}

TEST_F(ReaderTest, EofOnNext) {
  for (auto i = 0; i < 13; i++) {
    this->r2_->next();
  }

  EXPECT_TRUE(this->r2_->eof());
}

TEST_F(ReaderTest, SeeksTo) {
  this->r2_->seek(ReaderLocation{7, 1, 7});

  EXPECT_EQ(this->r2_->next(), 'W');
  EXPECT_EQ(this->r2_->next(), 'o');
  EXPECT_EQ(this->r2_->next(), 'r');
  EXPECT_EQ(this->r2_->next(), 'l');
  EXPECT_EQ(this->r2_->next(), 'd');
  EXPECT_EQ(this->r2_->loc, (ReaderLocation{12, 1, 12}));
}

TEST_F(ReaderTest, EofOnSeek) {
  this->r2_->seek(ReaderLocation{12, 1, 12});
  EXPECT_FALSE(this->r2_->eof());
}

TEST_F(ReaderTest, ReadsMultiline) {
  while (!this->r3_->eof()) {
    this->r3_->next();
  }

  EXPECT_EQ(this->r3_->loc, (ReaderLocation{39, 4, 0}));
}

TEST_F(ReaderTest, ThrowsOnNextOnEof) {
  EXPECT_THROW_WITH_MESSAGE({
    this->r1_->next();
  }, "Error: tried to read on reader eof");
}
