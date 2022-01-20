/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_READER_HPP
#define SRC_READER_HPP

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
  std::string content;
  std::size_t size;

  explicit Reader (const std::string &);
  Reader (const Reader &) = delete;
  Reader &operator= (const Reader &) = delete;

  bool eof () const;
  char next ();
  void seek (ReaderLocation);
};

#endif
