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
  size_t pos = 0;
  size_t line = 1;
  size_t col = 0;
};

class Reader {
 public:
  Reader (const fs::path &path);
  Reader (const Reader &) = delete;
  Reader &operator= (const Reader &) = delete;

  virtual std::string content () const;
  virtual bool eof () const;
  virtual ReaderLocation loc () const;
  virtual char next ();
  virtual fs::path path () const;
  virtual void seek (const ReaderLocation &loc);

 private:
  std::string _content;
  ReaderLocation _loc;
  fs::path _path;
};

bool operator== (const ReaderLocation &lhs, const ReaderLocation &rhs);
bool operator!= (const ReaderLocation &lhs, const ReaderLocation &rhs);

#endif
