/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <fstream>
#include <sstream>
#include "Error.hpp"
#include "Reader.hpp"

Reader::Reader (const fs::path &p) {
  fs::path canonicalPath;

  try {
    canonicalPath = fs::canonical(p);
  } catch (const std::exception &ex) {
    throw Error(R"(No such file ")" + p.string() + R"(")");
  }

  if (!fs::is_regular_file(canonicalPath)) {
    throw Error(R"(Path ")" + canonicalPath.string() + R"(" is not a file)");
  }

  auto f = std::ifstream(canonicalPath);

  if (f.fail()) {
    throw Error(R"(Unable to read file ")" + canonicalPath.string() + R"(")");
  }

  std::stringstream content;
  content << f.rdbuf();

  this->loc = ReaderLocation();
  this->path = canonicalPath;
  this->_content = content.str();
  this->_size = this->_content.length();
}

bool Reader::eof () const {
  return this->loc.pos >= this->_size;
}

char Reader::next () {
  if (this->eof()) {
    throw Error("Tried to read on Reader eof");
  }

  auto ch = this->_content[this->loc.pos];

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
