/*!
 * Copyright (c) 2018 Aaron Delasy
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SRC_LEXER_HPP
#define SRC_LEXER_HPP

#include <functional>
#include <optional>
#include "Error.hpp"
#include "Token.hpp"

class Lexer {
 public:
  Reader *reader;
  ReaderLocation loc;
  std::string val;

  explicit Lexer (Reader *);
  virtual ~Lexer () = default;

  virtual std::tuple<ReaderLocation, Token> next (bool = false);
  virtual void seek (ReaderLocation);

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
  std::optional<Token> _whitespace (bool = false);
};

#endif
