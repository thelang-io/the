/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Error.hpp"

Error::Error (const std::string &message) {
  this->message_ = message;
}

const char *Error::what () const noexcept {
  return this->message_.c_str();
}

Error::Error (Reader *reader, ReaderLocation start, const std::string &message) : Error(reader, start, reader->loc, message) {
}

Error::Error (Reader *reader, ReaderLocation start, ReaderLocation end, const std::string &message) {
  auto line = std::string();
  reader->seek(ReaderLocation{start.pos - start.col, start.line});

  while (!reader->eof()) {
    auto [_, ch] = reader->next();

    if (ch == '\n') {
      break;
    }

    line += ch;
  }

  auto lineNumStr = std::to_string(start.line);

  this->message_ += reader->path + ':' + start.str() + ": " + message + "\n";
  this->message_ += "  " + lineNumStr + " | " + line + '\n';
  this->message_ += "  " + std::string(lineNumStr.length(), ' ') + " | " + std::string(start.col, ' ') + '^';

  auto underlineLen = static_cast<std::size_t>(0);

  if (start.line == end.line && line.length() > end.col) {
    underlineLen = end.col - start.col;
  } else if (line.length() > start.col) {
    underlineLen = line.length() - start.col;
  }

  if (underlineLen != 0) {
    this->message_ += std::string(underlineLen - 1, '~');
  }
}
