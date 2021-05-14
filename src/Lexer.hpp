/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_LEXER_HPP
#define SRC_LEXER_HPP

#include "Reader.hpp"
#include "Token.hpp"

class Lexer {
 public:
  Lexer (Reader *reader);
  Token next ();

 private:
  Reader *_reader;

  Token _lexOpEq (const char ch, const TokenType tt1, const TokenType tt2);

  Token _lexOpEq2 (
    const char ch,
    const TokenType tt1,
    const TokenType tt2,
    const TokenType tt3,
    const TokenType tt4
  );

  Token _lexOpEqDouble (
    const char ch,
    const TokenType tt1,
    const TokenType tt2,
    const TokenType tt3
  );
};

#endif
