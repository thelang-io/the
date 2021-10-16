/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Error.hpp"

Error::Error (const std::string &message) {
  this->_message = message;
}

const char *Error::what () const noexcept {
  return this->_message.c_str();
}

SyntaxError::SyntaxError (Reader *reader, ReaderLocation start, const std::string &message) {
  auto end = reader->loc;
  reader->seek({start.pos - start.col, start.line});
  auto line = std::string();

  while (!reader->eof()) {
    auto ch = reader->next();

    if (ch == '\n') {
      break;
    }

    line += ch;
  }

  auto colNumStr = std::to_string(start.col + 1);
  auto lineNumStr = std::to_string(start.line);

  auto underlineLen = start.line == end.line
    ? line.length() > end.col
      ? end.col - start.col
      : line.length() > start.col
        ? line.length() - start.col
        : 0
    : line.length() > start.col
      ? line.length() - start.col
      : 0;

  this->_message = reader->path.string() + ':' +
    lineNumStr + ':' + colNumStr + ": " + message + "\n" +
    "  " + lineNumStr + " | " + line + '\n' +
    "  " + std::string(lineNumStr.length(), ' ') +
    " | " + std::string(start.col, ' ') + '^';

  if (underlineLen != 0) {
    this->_message += std::string(underlineLen - 1, '~');
  }
}

const char *SyntaxError::what () const noexcept {
  return this->_message.c_str();
}
