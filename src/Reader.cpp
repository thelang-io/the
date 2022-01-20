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

Reader::Reader (const std::string &p) {
  std::string canonicalPath;

  try {
    canonicalPath = std::filesystem::canonical(p);
  } catch (const std::exception &ex) {
    throw Error(R"(No such file ")" + p + R"(")");
  }

  if (!std::filesystem::is_regular_file(canonicalPath)) {
    throw Error(R"(Path ")" + p + R"(" is not a file)");
  }

  auto f = std::ifstream(canonicalPath);

  if (f.fail()) {
    throw Error(R"(Unable to read file ")" + p + R"(")");
  }

  std::stringstream c;
  c << f.rdbuf();

  this->loc = ReaderLocation();
  this->path = canonicalPath;
  this->content = c.str();
  this->size = this->content.length();
}

bool Reader::eof () const {
  return this->loc.pos >= this->size;
}

char Reader::next () {
  if (this->eof()) {
    throw Error("Tried to read on Reader eof");
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
