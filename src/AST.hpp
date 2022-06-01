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

  static void populateParents (ASTBlock &, ASTNode * = nullptr);

  explicit AST (Parser *);

  virtual ASTBlock gen ();
  virtual std::string xml ();

 private:
  AST (const AST &);
  AST &operator= (const AST &);

  ASTBlock _block (const ParserBlock &, VarStack &);
  ASTExprAccess _exprAccess (const std::shared_ptr<ParserMemberObj> &, VarStack &);
  Type *_exprAccessType (const std::shared_ptr<ParserMemberObj> &);
  void _forwardNode (const ParserBlock &);
  ASTNode _node (const ParserStmt &, VarStack &);
  ASTNodeExpr _nodeExpr (const ParserStmtExpr &, VarStack &);
  Type *_nodeExprType (const ParserStmtExpr &);
  ASTNodeIf _nodeIf (const ParserStmtIf &, VarStack &);
  Type *_type (const ParserType &);
  ASTNode _wrapNode (const ParserStmt &, const ASTNodeBody &);
  ASTNodeExpr _wrapNodeExpr (const ParserStmtExpr &, const ASTExpr &);
};

#endif
