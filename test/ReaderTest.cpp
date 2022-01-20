/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <gtest/gtest.h>
#include <fstream>
#include "../src/Error.hpp"
#include "../src/Reader.hpp"
#include "utils.hpp"

class ReaderTest : public ::testing::Test {
 protected:
  Reader *r1_;
  Reader *r2_;
  Reader *r3_;

  void SetUp () override {
    auto failFile = std::ofstream("test_fail.txt");
    failFile.close();
    std::filesystem::permissions("test_fail.txt", std::filesystem::perms::none);

    auto emptyFile = std::ofstream("test_empty.txt");
    emptyFile << "";
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

    std::filesystem::remove("test_fail.txt");
    std::filesystem::remove("test_empty.txt");
    std::filesystem::remove("test_regular.txt");
    std::filesystem::remove("test_multiline.txt");
  }
};

TEST_F(ReaderTest, ThrowsOnNotExisting) {
  EXPECT_THROW_WITH_MESSAGE({
    Reader("test.jpg");
  }, Error, R"(No such file "test.jpg")");
}

TEST_F(ReaderTest, ThrowsOnDirectory) {
  EXPECT_THROW_WITH_MESSAGE({
    Reader("/dev/null");
  }, Error, R"(Path "/dev/null" is not a file)");
}

TEST_F(ReaderTest, ThrowsOnNoPerms) {
  EXPECT_THROW_WITH_MESSAGE({
    Reader("test_fail.txt");
  }, Error, R"(Unable to read file "test_fail.txt")");
}

TEST_F(ReaderTest, ReadsFile) {
  EXPECT_EQ(this->r2_->loc.pos, 0);
  EXPECT_EQ(this->r2_->loc.line, 1);
  EXPECT_EQ(this->r2_->loc.col, 0);
  EXPECT_EQ(this->r2_->path, std::filesystem::current_path() / "test_regular.txt");
  EXPECT_EQ(this->r2_->content, "Hello, World!");
  EXPECT_EQ(this->r2_->size, 13);
}

TEST_F(ReaderTest, NoEofOnNonEmpty) {
  EXPECT_EQ(this->r2_->eof(), false);
}

TEST_F(ReaderTest, EofOnEmpty) {
  EXPECT_EQ(this->r1_->eof(), true);
}

TEST_F(ReaderTest, ReadsNext) {
  EXPECT_EQ(this->r2_->next(), 'H');
  EXPECT_EQ(this->r2_->next(), 'e');
  EXPECT_EQ(this->r2_->next(), 'l');
  EXPECT_EQ(this->r2_->next(), 'l');
  EXPECT_EQ(this->r2_->next(), 'o');
}

TEST_F(ReaderTest, EofOnNext) {
  for (auto i = static_cast<std::size_t>(0); i < 13; i++) {
    this->r2_->next();
  }

  EXPECT_EQ(this->r2_->eof(), true);
}

TEST_F(ReaderTest, SeeksTo) {
  this->r2_->seek(ReaderLocation{7, 1, 7});
  EXPECT_EQ(this->r2_->next(), 'W');
  EXPECT_EQ(this->r2_->next(), 'o');
  EXPECT_EQ(this->r2_->next(), 'r');
  EXPECT_EQ(this->r2_->next(), 'l');
  EXPECT_EQ(this->r2_->next(), 'd');
}

TEST_F(ReaderTest, EofOnSeek) {
  this->r2_->seek(ReaderLocation{12, 1, 12});
  EXPECT_EQ(this->r2_->eof(), false);
}

TEST_F(ReaderTest, ReadsMultiline) {
  while (!this->r3_->eof()) {
    this->r3_->next();
  }

  EXPECT_EQ(this->r3_->loc.pos, 39);
  EXPECT_EQ(this->r3_->loc.line, 4);
  EXPECT_EQ(this->r3_->loc.col, 0);
}
