/**
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
  size_t col = 1;
  size_t line = 1;
  size_t pos = 0;
};

class Reader {
 public:
  explicit Reader (const fs::path &path);

  bool eof () const;
  ReaderLocation loc () const;
  char next ();
  void seek (ReaderLocation loc);

 private:
  std::string _content;
  ReaderLocation _loc;
  fs::path _path;
};

#endif
