/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef READER_HPP
#define READER_HPP

#include <string>

struct ReaderLocation {
  std::size_t pos = 0;
  std::size_t line = 1;
  std::size_t col = 0;
};

class Reader {
 public:
  ReaderLocation loc;
  std::string path;

  explicit Reader (const std::string &);
  Reader (const Reader &) = delete;
  Reader &operator= (const Reader &) = delete;

  bool eof () const;
  char next ();
  void seek (ReaderLocation);

 private:
  std::string _content;
  std::size_t _size;
};

#endif
