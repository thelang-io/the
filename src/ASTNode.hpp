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

#ifndef SRC_AST_NODE_HPP
#define SRC_AST_NODE_HPP

#include "ASTExpr.hpp"

struct ASTCatchClause;
struct ASTFnDeclParam;
struct ASTNodeBreak;
struct ASTNodeContinue;
struct ASTNodeEnumDecl;
struct ASTNodeFnDecl;
struct ASTNodeIf;
struct ASTNodeLoop;
struct ASTNodeMain;
struct ASTNodeObjDecl;
struct ASTNodeReturn;
struct ASTNodeThrow;
struct ASTNodeTry;
struct ASTNodeTypeDecl;
struct ASTNodeVarDecl;

using ASTNodeBody = std::variant<
  ASTNodeBreak,
  ASTNodeContinue,
  ASTNodeEnumDecl,
  ASTNodeExpr,
  ASTNodeFnDecl,
  ASTNodeIf,
  ASTNodeLoop,
  ASTNodeMain,
  ASTNodeObjDecl,
  ASTNodeReturn,
  ASTNodeThrow,
  ASTNodeTry,
  ASTNodeTypeDecl,
  ASTNodeVarDecl
>;

struct ASTNode {
  std::shared_ptr<ASTNodeBody> body;
  ASTNode *parent;
  ReaderLocation start;
  ReaderLocation end;

  std::string xml (std::size_t = 0) const;
};

using ASTBlock = std::vector<ASTNode>;

struct ASTCatchClause {
  ASTNode param;
  ASTBlock body;

  std::string xml (std::size_t = 0) const;
};

struct ASTFnDeclParam {
  std::shared_ptr<Var> var;
  std::optional<ASTNodeExpr> init;
};

struct ASTNodeBreak {
};

struct ASTNodeContinue {
};

struct ASTNodeEnumDeclMember {
  std::string id;
  std::optional<ASTNodeExpr> init;
};

struct ASTNodeEnumDecl {
  Type *type;
  std::vector<ASTNodeEnumDeclMember> members;
};

struct ASTNodeFnDecl {
  std::shared_ptr<Var> var;
  std::vector<std::shared_ptr<Var>> stack;
  std::vector<ASTFnDeclParam> params;
  std::optional<ASTBlock> body;
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
  std::vector<std::shared_ptr<Var>> stack;
  ASTBlock body;
};

struct ASTNodeObjDeclMethod {
  std::shared_ptr<Var> var;
  std::vector<std::shared_ptr<Var>> stack = {};
  std::vector<ASTFnDeclParam> params = {};
  std::optional<ASTBlock> body = {};
};

struct ASTNodeObjDecl {
  Type *type;
  std::vector<ASTNodeObjDeclMethod> methods;
};

struct ASTNodeReturn {
  std::optional<ASTNodeExpr> body;
};

struct ASTNodeThrow {
  ASTNodeExpr arg;
};

struct ASTNodeTry {
  ASTBlock body;
  std::vector<ASTCatchClause> handlers;
};

struct ASTNodeTypeDecl {
  Type *type;
};

struct ASTNodeVarDecl {
  std::shared_ptr<Var> var;
  std::optional<ASTNodeExpr> init;
};

#endif
