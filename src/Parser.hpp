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

  virtual ParserStmt next ();
  virtual std::string xml ();

 private:
  Parser (const Parser &);
  Parser &operator= (const Parser &);

  ParserBlock _block ();
  std::optional<ParserStmtExpr> _stmtExpr (bool = true);
  ParserStmtIf _stmtIf ();
  ParserStmtLoop _stmtLoop (const std::optional<ParserStmt> &);
  std::optional<ParserType> _type ();
  std::tuple<ParserStmtExpr, bool> _wrapExprAssign (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprBinary (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprCall (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprCond (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprObj (const ParserStmtExpr &, ReaderLocation, const Token &);
  std::tuple<ParserStmtExpr, bool> _wrapExprUnary (const ParserStmtExpr &, ReaderLocation, const Token &);
  ParserStmt _wrapStmt (const ParserStmtBody &, ReaderLocation) const;
  ParserStmtExpr _wrapStmtExpr (const ParserStmtExpr &);
};

#endif
