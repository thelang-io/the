/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_READER_HPP
#define SRC_READER_HPP

#include <string>
#include <tuple>

struct ReaderLocation {
  std::size_t pos = 0;
  std::size_t line = 1;
  std::size_t col = 0;

  std::string str () const;
};

class Reader {
 public:
  ReaderLocation loc;
  std::string path;
  std::string content;
  std::size_t size;

  explicit Reader (const std::string &);

  virtual bool eof () const;
  virtual std::tuple<ReaderLocation, char> next ();
  virtual void seek (ReaderLocation);

 private:
  Reader (const Reader &);
  Reader &operator= (const Reader &);
};

bool operator== (const ReaderLocation &, const ReaderLocation &);

#endif
