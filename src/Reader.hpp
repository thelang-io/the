/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_READER_HPP
#define SRC_READER_HPP

#include <filesystem>

namespace fs = std::filesystem;

struct ReaderLocation {
  std::size_t pos = 0;
  std::size_t line = 1;
  std::size_t col = 0;
};

class Reader {
 public:
  ReaderLocation loc;
  fs::path path;

  explicit Reader (const fs::path &path);

  bool eof () const;
  char next ();
  void seek (ReaderLocation loc);

 private:
  std::string _content;
  size_t _size;
};

#endif
