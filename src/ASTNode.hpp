/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_AST_NODE_HPP
#define SRC_AST_NODE_HPP

#include "ASTExpr.hpp"

struct ASTNode;
struct ASTNodeBreak;
struct ASTNodeContinue;
struct ASTNodeFnDecl;
struct ASTNodeFnDeclParam;
struct ASTNodeIf;
struct ASTNodeLoop;
struct ASTNodeMain;
struct ASTNodeObjDecl;
struct ASTNodeReturn;
struct ASTNodeVarDecl;

using ASTBlock = std::vector<ASTNode>;

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

using ASTNodeIfCond = std::variant<ASTBlock, ASTNodeIf>;

struct ASTNodeBreak {
};

struct ASTNodeContinue {
};

struct ASTNodeFnDecl {
  std::shared_ptr<Var> var;
  std::vector<std::shared_ptr<Var>> stack;
  std::vector<ASTNodeFnDeclParam> params;
  ASTBlock body;
};

struct ASTNodeFnDeclParam {
  std::shared_ptr<Var> var;
  std::optional<ASTNodeExpr> init;
};

struct ASTNodeIf {
  std::shared_ptr<ASTNode> cond;
  ASTBlock body;
  std::optional<std::shared_ptr<ASTNodeIfCond>> alt;
};

struct ASTNodeLoop {
  std::optional<std::shared_ptr<ASTNode>> init;
  std::optional<ASTNodeExpr> cond;
  std::optional<ASTNodeExpr> upd;
  ASTBlock body;
};

struct ASTNodeMain {
  ASTBlock body;
};

struct ASTNodeObjDecl {
  std::shared_ptr<Var> var;
};

struct ASTNodeReturn {
  std::optional<ASTNodeExpr> body;
};

struct ASTNodeVarDecl {
  std::shared_ptr<Var> var;
  std::optional<ASTNodeExpr> init;
};

struct ASTNode {
  ASTNodeBody body;
};

#endif
