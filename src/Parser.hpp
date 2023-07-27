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

#ifndef SRC_PARSER_HPP
#define SRC_PARSER_HPP

#include "ParserStmt.hpp"

class Parser {
 public:
  Lexer *lexer;
  Reader *reader;

  explicit Parser (Lexer *);

  virtual std::string doc ();
  virtual ParserStmt next (bool = true, bool = false);
  virtual std::string xml ();

 private:
  Parser (const Parser &);
  Parser &operator= (const Parser &);

  ParserBlock _block (bool = false);
  std::optional<ParserStmtExpr> _stmtExpr (bool = true);
  std::optional<ParserType> _type ();
  std::tuple<ParserStmtExpr, bool> _wrapExpr (
    const ParserStmtExpr &,
    ReaderLocation,
    const Token &,
    int,
    const std::function<std::tuple<ParserStmtExpr, bool> (const ParserStmtExpr &, ReaderLocation, const Token &)> &
  );
  std::tuple<ParserStmtExpr, bool> _wrapExprAccess (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprAs (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprAssign (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprBinary (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprCall (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprCond (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprIs (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprObj (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprUnary (const ParserStmtExpr &, ReaderLocation, const Token &);
  ParserStmt _wrapStmt (bool, const ParserStmtBody &, ReaderLocation) const;
  ParserStmtExpr _wrapStmtExpr (const ParserStmtExpr &);
  ParserStmt _wrapStmtLoop (bool, const Token &, bool, bool = false);
  ParserType _wrapType (const ParserType &);
};

#endif
