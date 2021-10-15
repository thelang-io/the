/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_READER_HPP
#define SRC_READER_HPP

#include <filesystem>
#include "ReaderLocation.hpp"

namespace fs = std::filesystem;

class Reader {
 public:
  ReaderLocation loc;
  fs::path path;

  explicit Reader (const fs::path &path);

  bool eof () const;
  char next ();
  void seek (const ReaderLocation &location);

 private:
  std::string _content;
  size_t _size;
};

#endif
