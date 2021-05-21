/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "SyntaxError.hpp"

SyntaxError::SyntaxError (Reader *reader, const ReaderLocation &loc, const std::string &msg) {
  reader->seek({loc.pos - loc.col, loc.line});
  auto lineContent = std::string();

  while (!reader->eof()) {
    char ch = reader->next();

    if (ch == '\n') {
      break;
    }

    lineContent += ch;
  }

  reader->seek(loc);

  const auto colStr = std::to_string(loc.col + 1);
  const auto lineStr = std::to_string(loc.line);

  this->message = reader->path().string() + ":" +
    lineStr + ":" + colStr + ": " + msg + "\n" +
    "  " + lineStr + " | " + lineContent + "\n" +
    "  " + std::string(lineStr.length(), ' ') +
    " | " + std::string(loc.col, ' ') + "^\n";

  this->name = "SyntaxError";
}
