/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_AST_NODE_HPP
#define SRC_AST_NODE_HPP

#include "ASTExpr.hpp"

struct ASTNodeBreak;
struct ASTNodeContinue;
struct ASTNodeFnDecl;
struct ASTNodeIf;
struct ASTNodeLoop;
struct ASTNodeMain;
struct ASTNodeObjDecl;
struct ASTNodeReturn;
struct ASTNodeVarDecl;

using ASTNodeBody = std::variant<
  ASTNodeBreak,
  ASTNodeContinue,
  ASTNodeExpr,
  ASTNodeFnDecl,
  ASTNodeIf,
  ASTNodeLoop,
  ASTNodeMain,
  ASTNodeObjDecl,
  ASTNodeReturn,
  ASTNodeVarDecl
>;

struct ASTNode {
  std::shared_ptr<ASTNodeBody> body;
  ASTNode *parent;

  std::string xml (std::size_t = 0) const;
};

using ASTBlock = std::vector<ASTNode>;

struct ASTNodeBreak {
};

struct ASTNodeContinue {
};

struct ASTNodeFnDeclParam {
  std::shared_ptr<Var> var;
  std::optional<ASTNodeExpr> init;
};

struct ASTNodeFnDecl {
  std::shared_ptr<Var> var;
  std::vector<std::shared_ptr<Var>> stack;
  std::vector<ASTNodeFnDeclParam> params;
  ASTBlock body;
};

struct ASTNodeIf {
  ASTNodeExpr cond;
  ASTBlock body;
  std::optional<std::variant<ASTBlock, ASTNode>> alt;
};

struct ASTNodeLoop {
  std::optional<ASTNode> init;
  std::optional<ASTNodeExpr> cond;
  std::optional<ASTNodeExpr> upd;
  ASTBlock body;
};

struct ASTNodeMain {
  ASTBlock body;
};

struct ASTNodeObjDecl {
  Type *type;
};

struct ASTNodeReturn {
  std::optional<ASTNodeExpr> body;
};

struct ASTNodeVarDecl {
  std::shared_ptr<Var> var;
  std::optional<ASTNodeExpr> init;
};

#endif
