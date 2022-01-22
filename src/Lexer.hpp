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

class LexerError;

class Lexer {
  friend class LexerError;

 public:
  ReaderLocation loc;
  std::string val;

  explicit Lexer (Reader *);

  Token next ();

 private:
  Reader *_reader;

  Lexer (const Lexer &);
  Lexer &operator= (const Lexer &);

  Token _opEq (TokenType, TokenType);
  Token _opEq2 (char, TokenType, TokenType, TokenType, const std::optional<TokenType> & = std::nullopt);
  Token _tok (TokenType);
  void _walk (const std::function<bool (char)> &);
  void _whitespace ();
};

class LexerError : public Error {
 public:
  LexerError (Lexer *, const std::string &);
};

#endif
