/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_PARSER_HPP
#define SRC_PARSER_HPP

#include "ParserStmt.hpp"

class Parser;

class Parser {
 public:
  Lexer *lexer;

  explicit Parser (Lexer *);

  virtual ParserStmt next ();

 private:
  Parser (const Parser &);
  Parser &operator= (const Parser &);

  ParserBlock _block ();
  ParserStmt _stmt (const ParserStmtBody &, ReaderLocation, ReaderLocation) const;
  ParserStmtExpr _stmtExpr (bool = false);
  ParserStmtIf _stmtIf ();
  ParserStmtLoop _stmtLoop (const std::optional<std::shared_ptr<ParserStmt>> &);
  ParserStmtExpr _wrapStmtExpr (const ParserStmtExpr &);
};

#endif
