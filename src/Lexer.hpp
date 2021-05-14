/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_LEXER_HPP
#define SRC_LEXER_HPP

#include "Token.hpp"

class Lexer {
 public:
  Lexer (Reader *reader);
  Token next ();

 private:
  ReaderLocation _end;
  Reader *_reader;
  ReaderLocation _start;
  std::string _val;

  Token _opEq (const TokenType tt1, const TokenType tt2);

  Token _opEq2 (
    const char ch,
    const TokenType tt1,
    const TokenType tt2,
    const TokenType tt3,
    const TokenType tt4
  );

  Token _opEqDouble (
    const char ch,
    const TokenType tt1,
    const TokenType tt2,
    const TokenType tt3
  );

  Token _token (const TokenType tt);
};

#endif
