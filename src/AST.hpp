/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_AST_HPP
#define SRC_AST_HPP

#include "ASTNode.hpp"
#include "Parser.hpp"

class AST;

class AST {
 public:
  Parser *parser;
  Reader *reader;
  TypeMap typeMap;
  VarMap varMap;

  explicit AST (Parser *);

  virtual ASTBlock gen ();

 private:
  AST (const AST &);
  AST &operator= (const AST &);

  ASTBlock _block (const ParserBlock &, VarStack &);
  ASTExprAccess _exprAccess (const ParserMemberObj &, VarStack &) const;
  std::shared_ptr<Type> _exprAccessType (const ParserMemberObj &) const;
  void _forwardStmt (const ParserBlock &);
  ASTNode _stmt (const ParserStmt &, VarStack &);
  std::shared_ptr<ASTNodeExpr> _stmtExpr (const std::shared_ptr<ParserStmtExpr> &, VarStack &) const;
  std::shared_ptr<Type> _stmtExprType (const std::shared_ptr<ParserStmtExpr> &) const;
  ASTNodeIf _stmtIf (const ParserStmtIf &, VarStack &);
  std::shared_ptr<Type> _type (const std::shared_ptr<ParserType> &) const;
};

#endif
