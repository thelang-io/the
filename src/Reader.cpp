/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <filesystem>
#include <fstream>
#include <sstream>
#include "Error.hpp"
#include "Reader.hpp"

std::string ReaderLocation::str () const {
  return std::to_string(this->line) + ":" + std::to_string(this->col + 1);
}

Reader::Reader (const std::string &p) {
  std::string canonicalPath;

  try {
    canonicalPath = std::filesystem::canonical(p);
  } catch (const std::exception &ex) {
    throw Error(R"(Error: no such file ")" + p + R"(")");
  }

  if (!std::filesystem::is_regular_file(canonicalPath)) {
    throw Error(R"(Error: path ")" + p + R"(" is not a file)");
  }

  auto f = std::ifstream(canonicalPath);

  if (!f.is_open()) {
    // TODO Test
    throw Error(R"(Error: unable to open file ")" + p + R"(")");
  } else if (f.fail()) {
    // TODO Test
    throw Error(R"(Error: unable to read file ")" + p + R"(")");
  }

  std::stringstream c;
  c << f.rdbuf();

  this->loc = ReaderLocation{};
  this->path = canonicalPath;
  this->content = c.str();
  this->size = this->content.length();
}

bool Reader::eof () const {
  return this->loc.pos >= this->size;
}

char Reader::next () {
  if (this->eof()) {
    throw Error("Error: tried to read on reader eof");
  }

  auto ch = this->content[this->loc.pos];

  if (ch == '\n') {
    this->loc.col = 0;
    this->loc.line += 1;
  } else {
    this->loc.col += 1;
  }

  this->loc.pos += 1;
  return ch;
}

void Reader::seek (ReaderLocation l) {
  this->loc = l;
}

bool operator== (const ReaderLocation &lhs, const ReaderLocation &rhs) {
  return lhs.pos == rhs.pos && lhs.line == rhs.line && lhs.col == rhs.col;
}
