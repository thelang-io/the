/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_LEXER_HPP
#define SRC_LEXER_HPP

#include <functional>
#include <optional>
#include "Error.hpp"
#include "Token.hpp"

class Lexer;

class Lexer {
 public:
  Reader *reader;
  ReaderLocation loc;
  std::string val;

  explicit Lexer (Reader *);

  virtual Token next ();
  virtual void seek (ReaderLocation);
  virtual void whitespace ();

 private:
  Lexer (const Lexer &);
  Lexer &operator= (const Lexer &);

  Token _litFloat (TokenType);
  Token _litNum (const std::function<bool (char)> &, TokenType);
  Token _opEq (TokenType, TokenType);
  Token _opEq2 (char, TokenType, TokenType, TokenType, const std::optional<TokenType> & = std::nullopt);
  Token _tok (TokenType);
  void _walk (const std::function<bool (char)> &);
  void _walkLitFloatExp (ReaderLocation);
};

#endif
