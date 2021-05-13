/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <fstream>
#include "Error.hpp"
#include "Reader.hpp"

Reader::Reader (const fs::path &path) {
  try {
    this->_path = fs::canonical(path);
  } catch (const fs::filesystem_error &err) {
    throw Error("Path '" + path.string() + "' does not exists");
  }

  if (!fs::is_regular_file(this->_path)) {
    throw Error("Path '" + path.string() + "' is not a file");
  }

  std::ifstream file(this->_path);

  if (!file) {
    throw Error("Unable to read '" + path.string() + "'");
  }

  file.seekg(0, std::ios::end);
  std::ifstream::pos_type size = file.tellg();
  file.seekg(0, std::ios::beg);
  this->_content.resize(static_cast<std::string::size_type>(size));
  file.read(&this->_content[0], size);
}

bool Reader::eof () const {
  return this->_loc.pos >= this->_content.size();
}

ReaderLocation Reader::loc () const {
  return this->_loc;
}

char Reader::next () {
  if (this->eof()) {
    return -1;
  }

  char ch = this->_content[this->_loc.pos];

  if (ch == '\n') {
    this->_loc.col = 0;
    this->_loc.line += 1;
  } else {
    this->_loc.col += 1;
  }

  this->_loc.pos += 1;
  return ch;
}

void Reader::seek (ReaderLocation loc) {
  this->_loc = loc;
}
