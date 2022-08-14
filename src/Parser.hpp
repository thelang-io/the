/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_PARSER_HPP
#define SRC_PARSER_HPP

#include "ParserStmt.hpp"

class Parser {
 public:
  Lexer *lexer;
  Reader *reader;

  explicit Parser (Lexer *);

  virtual ParserStmt next (bool = true);
  virtual std::string xml ();

 private:
  Parser (const Parser &);
  Parser &operator= (const Parser &);

  ParserBlock _block ();
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
  std::tuple<ParserStmtExpr, bool> _wrapExprAssign (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprBinary (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprCall (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprCond (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprObj (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprUnary (const ParserStmtExpr &, ReaderLocation, const Token &);
  ParserStmt _wrapStmt (bool, const ParserStmtBody &, ReaderLocation) const;
  ParserStmtExpr _wrapStmtExpr (const ParserStmtExpr &);
  ParserType _wrapType (const ParserType &);
};

#endif
