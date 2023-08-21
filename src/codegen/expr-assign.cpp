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

std::shared_ptr<CodegenASTExpr> Codegen::_exprAssign (const ASTNodeExpr &nodeExpr, Type *targetType, const ASTNode &parent, std::shared_ptr<CodegenASTStmt> *c, bool root) {
  auto exprAssign = std::get<ASTExprAssign>(*nodeExpr.body);
  auto cLeft = this->_nodeExpr(exprAssign.left, nodeExpr.type, parent, c, true);
  auto expr = CodegenASTExprNull::create();

  if (Type::actual(exprAssign.left.type)->isAny() || Type::actual(exprAssign.right.type)->isAny()) {
    auto cRight = this->_nodeExpr(exprAssign.right, nodeExpr.type, parent, c);
    expr = this->_genReallocFn(this->ast->typeMap.get("any"), cLeft, cRight);

    if (!root) {
      expr = this->_genCopyFn(this->ast->typeMap.get("any"), expr);
    }
  } else if (
    Type::actual(exprAssign.left.type)->isArray() ||
    Type::actual(exprAssign.left.type)->isFn() ||
    Type::actual(exprAssign.left.type)->isMap() ||
    Type::actual(exprAssign.left.type)->isObj() ||
    Type::actual(exprAssign.left.type)->isOpt() ||
    Type::actual(exprAssign.left.type)->isUnion()
  ) {
    auto cRight = this->_nodeExpr(exprAssign.right, nodeExpr.type, parent, c);
    expr = this->_genReallocFn(Type::actual(exprAssign.left.type), cLeft, cRight);

    if (!root) {
      expr = this->_genCopyFn(Type::actual(exprAssign.left.type), expr);
    }
  } else if (Type::actual(exprAssign.left.type)->isStr() || Type::actual(exprAssign.right.type)->isStr()) {
    auto cRight = CodegenASTExprNull::create();

    if (exprAssign.op == AST_EXPR_ASSIGN_ADD) {
      if (exprAssign.right.isLit()) {
        cRight = CodegenASTExprCall::create(
          CodegenASTExprAccess::create(this->_("str_concat_cstr")),
          {
            this->_genCopyFn(this->ast->typeMap.get("str"), cLeft),
            CodegenASTExprLiteral::create(exprAssign.right.litBody())
          }
        );
      } else {
        cRight = CodegenASTExprCall::create(
          CodegenASTExprAccess::create(this->_("str_concat_str")),
          {
            this->_genCopyFn(this->ast->typeMap.get("str"), cLeft),
            this->_nodeExpr(exprAssign.right, nodeExpr.type, parent, c)
          }
        );
      }
    } else {
      cRight = this->_nodeExpr(exprAssign.right, nodeExpr.type, parent, c);
    }

    expr = this->_genReallocFn(this->ast->typeMap.get("str"), cLeft, cRight);

    if (!root) {
      expr = this->_genCopyFn(this->ast->typeMap.get("str"), expr);
    }
  } else {
    auto opCode = std::string("=");
    auto cRight = this->_nodeExpr(exprAssign.right, nodeExpr.type, parent, c);

    if (exprAssign.op == AST_EXPR_ASSIGN_AND) {
      cRight = CodegenASTExprBinary::create(cLeft, "&&", cRight);
    } else if (exprAssign.op == AST_EXPR_ASSIGN_OR) {
      cRight = CodegenASTExprBinary::create(cLeft, "||", cRight);
    } else {
      if (exprAssign.op == AST_EXPR_ASSIGN_ADD) opCode = "+=";
      else if (exprAssign.op == AST_EXPR_ASSIGN_BIT_AND) opCode = "&=";
      else if (exprAssign.op == AST_EXPR_ASSIGN_BIT_OR) opCode = "|=";
      else if (exprAssign.op == AST_EXPR_ASSIGN_BIT_XOR) opCode = "^=";
      else if (exprAssign.op == AST_EXPR_ASSIGN_DIV) opCode = "/=";
      else if (exprAssign.op == AST_EXPR_ASSIGN_EQ) opCode = "=";
      else if (exprAssign.op == AST_EXPR_ASSIGN_LSHIFT) opCode = "<<=";
      else if (exprAssign.op == AST_EXPR_ASSIGN_MOD) opCode = "%=";
      else if (exprAssign.op == AST_EXPR_ASSIGN_MUL) opCode = "*=";
      else if (exprAssign.op == AST_EXPR_ASSIGN_RSHIFT) opCode = ">>=";
      else if (exprAssign.op == AST_EXPR_ASSIGN_SUB) opCode = "-=";
    }

    expr = CodegenASTExprAssign::create(cLeft, opCode, cRight);
  }

  return this->_wrapNodeExpr(nodeExpr, targetType, root, expr);
}
