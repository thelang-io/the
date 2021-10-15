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

Reader::Reader (const fs::path &path) {
  fs::path canonicalPath;

  try {
    canonicalPath = fs::canonical(path);
  } catch (const std::exception &ex) {
    throw Error("No such file \"" + path.string() + "\"");
  }

  if (!fs::is_regular_file(canonicalPath)) {
    throw Error("Path \"" + canonicalPath.string() + "\" is not a file");
  }

  std::ifstream f(canonicalPath);

  if (f.fail()) {
    throw Error("Unable to read file \"" + canonicalPath.string() + "\"");
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
    throw Error("Tried to read on reader eof");
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

void Reader::seek (const ReaderLocation &location) {
  this->loc = location;
}
