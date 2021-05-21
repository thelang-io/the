/**
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_LEXER_HPP
#define SRC_LEXER_HPP

#include <functional>
#include "Token.hpp"

class Lexer {
 public:
  explicit Lexer (Reader *);
  Token next ();

 private:
  ReaderLocation _end;
  Reader *_reader;
  ReaderLocation _start;
  std::string _val;

  Token _lexOpEq (TokenType, TokenType);
  Token _lexOpEq2 (char, TokenType, TokenType, TokenType, TokenType);
  Token _lexOpEqDouble (char, TokenType, TokenType, TokenType);
  void _lookupInvalidLitInt ();
  Token _token (TokenType);
  void _walk (const std::function<bool (char)> &);
};

#endif
