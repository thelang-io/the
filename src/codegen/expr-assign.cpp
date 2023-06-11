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

std::string Codegen::_exprAssign (const ASTNodeExpr &nodeExpr, Type *targetType, const ASTNode &parent, std::string &decl, bool root) {
  auto exprAssign = std::get<ASTExprAssign>(*nodeExpr.body);
  auto leftCode = this->_nodeExpr(exprAssign.left, nodeExpr.type, parent, decl, true);
  auto code = std::string();

  if (Type::actual(exprAssign.left.type)->isAny() || Type::actual(exprAssign.right.type)->isAny()) {
    code = this->_genReallocFn(this->ast->typeMap.get("any"), leftCode, this->_nodeExpr(exprAssign.right, nodeExpr.type, parent, decl));
    code = root ? code : this->_genCopyFn(this->ast->typeMap.get("any"), code);
  } else if (
    Type::actual(exprAssign.left.type)->isArray() ||
    Type::actual(exprAssign.left.type)->isFn() ||
    Type::actual(exprAssign.left.type)->isMap() ||
    Type::actual(exprAssign.left.type)->isObj() ||
    Type::actual(exprAssign.left.type)->isOpt() ||
    Type::actual(exprAssign.left.type)->isUnion()
  ) {
    code = this->_genReallocFn(Type::actual(exprAssign.left.type), leftCode, this->_nodeExpr(exprAssign.right, nodeExpr.type, parent, decl));
    code = root ? code : this->_genCopyFn(Type::actual(exprAssign.left.type), code);
  } else if (Type::actual(exprAssign.left.type)->isStr() || Type::actual(exprAssign.right.type)->isStr()) {
    auto rightCode = std::string();

    if (exprAssign.op == AST_EXPR_ASSIGN_ADD) {
      if (exprAssign.right.isLit()) {
        rightCode = this->_apiEval("_{str_concat_cstr}(" + this->_genCopyFn(this->ast->typeMap.get("str"), leftCode), 1);
        rightCode += ", " + exprAssign.right.litBody() + ")";
      } else {
        rightCode = this->_apiEval("_{str_concat_str}(" + this->_genCopyFn(this->ast->typeMap.get("str"), leftCode), 1);
        rightCode += ", " + this->_nodeExpr(exprAssign.right, nodeExpr.type, parent, decl) + ")";
      }
    } else {
      rightCode = this->_nodeExpr(exprAssign.right, nodeExpr.type, parent, decl);
    }

    code = this->_genReallocFn(this->ast->typeMap.get("str"), leftCode, rightCode);
    code = root ? code : this->_genCopyFn(this->ast->typeMap.get("str"), code);
  } else {
    auto opCode = std::string(" = ");
    auto rightCode = this->_nodeExpr(exprAssign.right, nodeExpr.type, parent, decl);

    if (exprAssign.op == AST_EXPR_ASSIGN_AND) {
      rightCode = leftCode + " && " + rightCode;
    } else if (exprAssign.op == AST_EXPR_ASSIGN_OR) {
      rightCode = leftCode + " || " + rightCode;
    } else {
      if (exprAssign.op == AST_EXPR_ASSIGN_ADD) opCode = " += ";
      else if (exprAssign.op == AST_EXPR_ASSIGN_BIT_AND) opCode = " &= ";
      else if (exprAssign.op == AST_EXPR_ASSIGN_BIT_OR) opCode = " |= ";
      else if (exprAssign.op == AST_EXPR_ASSIGN_BIT_XOR) opCode = " ^= ";
      else if (exprAssign.op == AST_EXPR_ASSIGN_DIV) opCode = " /= ";
      else if (exprAssign.op == AST_EXPR_ASSIGN_EQ) opCode = " = ";
      else if (exprAssign.op == AST_EXPR_ASSIGN_LSHIFT) opCode = " <<= ";
      else if (exprAssign.op == AST_EXPR_ASSIGN_MOD) opCode = " %= ";
      else if (exprAssign.op == AST_EXPR_ASSIGN_MUL) opCode = " *= ";
      else if (exprAssign.op == AST_EXPR_ASSIGN_RSHIFT) opCode = " >>= ";
      else if (exprAssign.op == AST_EXPR_ASSIGN_SUB) opCode = " -= ";
    }

    code = leftCode + opCode + rightCode;
  }

  return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
}
