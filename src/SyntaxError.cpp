/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "SyntaxError.hpp"

SyntaxError::SyntaxError (Reader *reader, const ReaderLocation &start, const std::string &msg) {
  const auto end = reader->loc();
  reader->seek({start.pos - start.col, start.line});
  auto line = std::string();

  while (!reader->eof()) {
    char ch = reader->next();

    if (ch == '\n') {
      break;
    }

    line += ch;
  }

  const auto colNumStr = std::to_string(start.col + 1);
  const auto lineNumStr = std::to_string(start.line);

  const auto underlineLen = start.line == end.line
    ? line.length() > end.col
      ? end.col - start.col
      : line.length() > start.col
        ? line.length() - start.col
        : 0
    : line.length() > start.col
      ? line.length() - start.col
      : 0;

  this->message = reader->path().string() + ':' +
    lineNumStr + ':' + colNumStr + ": " + msg + "\n" +
    "  " + lineNumStr + " | " + line + '\n' +
    "  " + std::string(lineNumStr.length(), ' ') +
    " | " + std::string(start.col, ' ') + '^';

  if (underlineLen != 0) {
    this->message += std::string(underlineLen - 1, '~');
  }

  this->message += '\n';
  this->name = "SyntaxError";
}
