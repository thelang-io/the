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
  explicit Reader (const fs::path &);
  Reader (const Reader &) = delete;
  Reader &operator= (const Reader &) = delete;

  [[nodiscard]] virtual std::string content () const;
  [[nodiscard]] virtual bool eof () const;
  [[nodiscard]] virtual ReaderLocation loc () const;
  virtual char next ();
  [[nodiscard]] virtual fs::path path () const;
  virtual void seek (const ReaderLocation &);

 protected:
  std::string content_;

 private:
  ReaderLocation _loc;
  fs::path _path;
};

bool operator== (const ReaderLocation &, const ReaderLocation &);
bool operator!= (const ReaderLocation &, const ReaderLocation &);

#endif
