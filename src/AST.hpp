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
  ASTExprAccess _exprAccess (const std::shared_ptr<ParserMemberObj> &, VarStack &);
  Type *_exprAccessType (const std::shared_ptr<ParserMemberObj> &);
  void _forwardStmt (const ParserBlock &);
  ASTNode _stmt (const ParserStmt &, VarStack &);
  ASTNodeExpr _stmtExpr (const ParserStmtExpr &, VarStack &);
  Type *_stmtExprType (const ParserStmtExpr &);
  ASTNodeIf _stmtIf (const ParserStmtIf &, VarStack &);
  Type *_type (const ParserType &);
  ASTNode _wrapNode (const ParserStmt &, const ASTNodeBody &);
  ASTNodeExpr _wrapNodeExpr (const ParserStmtExpr &, const ASTExpr &);
};

#endif
