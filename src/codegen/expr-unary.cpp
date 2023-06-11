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

#include "../Codegen.hpp"

std::string Codegen::_exprUnary (const ASTNodeExpr &nodeExpr, Type *targetType, const ASTNode &parent, std::string &decl, bool root) {
  auto exprUnary = std::get<ASTExprUnary>(*nodeExpr.body);
  auto argCode = this->_nodeExpr(exprUnary.arg, nodeExpr.type, parent, decl);
  auto opCode = std::string();

  if (exprUnary.op == AST_EXPR_UNARY_BIT_NOT) opCode = "~";
  else if (exprUnary.op == AST_EXPR_UNARY_DECREMENT) opCode = "--";
  else if (exprUnary.op == AST_EXPR_UNARY_INCREMENT) opCode = "++";
  else if (exprUnary.op == AST_EXPR_UNARY_MINUS) opCode = "-";
  else if (exprUnary.op == AST_EXPR_UNARY_NOT) opCode = "!";
  else if (exprUnary.op == AST_EXPR_UNARY_PLUS) opCode = "+";

  if (exprUnary.op == AST_EXPR_UNARY_NOT && exprUnary.arg.type->isFloatNumber()) {
    argCode = this->_apiEval("((_{bool}) " + argCode + ")", 1);
  } else if (exprUnary.op == AST_EXPR_UNARY_NOT && exprUnary.arg.type->isStr()) {
    argCode = this->_apiEval("_{str_not}(" + argCode + ")", 1);
    opCode = "";
  } else if (argCode.starts_with("*")) {
    argCode = "(" + argCode + ")";
  }

  return this->_wrapNodeExpr(nodeExpr, targetType, root, exprUnary.prefix ? opCode + argCode : argCode + opCode);
}
