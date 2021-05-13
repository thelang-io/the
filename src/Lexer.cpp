/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Lexer.hpp"

Lexer::Lexer (Reader *reader) {
  this->_reader = reader;
}

Token Lexer::next () {
  if (this->_reader->eof()) {
    return Token(eof, "\0");
  }

  auto ch = this->_reader->next();
  auto val = std::string{ch};

  if (Token::isWhitespace(ch)) {
    while (!this->_reader->eof()) {
      const auto loc = this->_reader->loc();
      ch = this->_reader->next();

      if (!Token::isWhitespace(ch)) {
        this->_reader->seek(loc);
        break;
      }

      val += ch;
    }

    return Token(whitespace, val);
  } else if (Token::isIdStart(ch)) {
    while (!this->_reader->eof()) {
      const auto loc = this->_reader->loc();
      ch = this->_reader->next();

      if (!Token::isIdContinue(ch)) {
        this->_reader->seek(loc);
        break;
      }

      val += ch;
    }

    if (val == "as") {
      const auto loc = this->_reader->loc();
      ch = this->_reader->next();

      if (ch == '?') {
        return Token(kwAsSafe, val);
      } else {
        this->_reader->seek(loc);
        return Token(kwAs, val);
      }
    }

    if (val == "async") return Token(kwAsync, val);
    else if (val == "await") return Token(kwAwait, val);
    else if (val == "break") return Token(kwBreak, val);
    else if (val == "case") return Token(kwCase, val);
    else if (val == "catch") return Token(kwCatch, val);
    else if (val == "class") return Token(kwClass, val);
    else if (val == "const") return Token(kwConst, val);
    else if (val == "continue") return Token(kwContinue, val);
    else if (val == "default") return Token(kwDefault, val);
    else if (val == "deinit") return Token(kwDeinit, val);
    else if (val == "elif") return Token(kwElif, val);
    else if (val == "else") return Token(kwElse, val);
    else if (val == "enum") return Token(kwEnum, val);
    else if (val == "export") return Token(kwExport, val);
    else if (val == "fallthrough") return Token(kwFallthrough, val);
    else if (val == "false") return Token(kwFalse, val);
    else if (val == "fn") return Token(kwFn, val);
    else if (val == "from") return Token(kwFrom, val);
    else if (val == "if") return Token(kwIf, val);
    else if (val == "import") return Token(kwImport, val);
    else if (val == "in") return Token(kwIn, val);
    else if (val == "init") return Token(kwInit, val);
    else if (val == "interface") return Token(kwInterface, val);
    else if (val == "is") return Token(kwIs, val);
    else if (val == "loop") return Token(kwLoop, val);
    else if (val == "main") return Token(kwMain, val);
    else if (val == "match") return Token(kwMatch, val);
    else if (val == "mut") return Token(kwMut, val);
    else if (val == "new") return Token(kwNew, val);
    else if (val == "nil") return Token(kwNil, val);
    else if (val == "obj") return Token(kwObj, val);
    else if (val == "op") return Token(kwOp, val);
    else if (val == "override") return Token(kwOverride, val);
    else if (val == "priv") return Token(kwPriv, val);
    else if (val == "prot") return Token(kwProt, val);
    else if (val == "pub") return Token(kwPub, val);
    else if (val == "return") return Token(kwReturn, val);
    else if (val == "static") return Token(kwStatic, val);
    else if (val == "super") return Token(kwSuper, val);
    else if (val == "this") return Token(kwThis, val);
    else if (val == "throw") return Token(kwThrow, val);
    else if (val == "true") return Token(kwTrue, val);
    else if (val == "try") return Token(kwTry, val);
    else if (val == "union") return Token(kwUnion, val);

    return Token(litId, val);
  }

  return Token(unknown, "");
}
