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

  auto file = std::ifstream(this->_path);

  if (!file) {
    throw Error("Unable to read '" + path.string() + "'");
  }

  file.seekg(0, std::ios::end);
  const auto size = file.tellg();
  file.seekg(0, std::ios::beg);

  this->content_.resize(static_cast<std::string::size_type>(size));
  file.read(&this->content_[0], size);
}

std::string Reader::content () const {
  return this->content_;
}

bool Reader::eof () const {
  return this->_loc.pos >= this->content_.size();
}

ReaderLocation Reader::loc () const {
  return this->_loc;
}

char Reader::next () {
  if (this->eof()) {
    throw Error("Tried to read on file end");
  }

  const auto ch = this->content_[this->_loc.pos];

  if (ch == '\n') {
    this->_loc.col = 0;
    this->_loc.line += 1;
  } else {
    this->_loc.col += 1;
  }

  this->_loc.pos += 1;
  return ch;
}

fs::path Reader::path () const {
  return this->_path;
}

void Reader::seek (const ReaderLocation &loc) {
  this->_loc = loc;
}

bool operator== (const ReaderLocation &lhs, const ReaderLocation &rhs) {
  return lhs.col == rhs.col && lhs.line == rhs.line && lhs.pos == rhs.pos;
}

bool operator!= (const ReaderLocation &lhs, const ReaderLocation &rhs) {
  return !(lhs == rhs);
}
