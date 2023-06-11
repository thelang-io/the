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

std::string Codegen::_exprBinary (const ASTNodeExpr &nodeExpr, Type *targetType, const ASTNode &parent, std::string &decl, bool root) {
  auto exprBinary = std::get<ASTExprBinary>(*nodeExpr.body);
  auto code = std::string();

  if (
    exprBinary.op == AST_EXPR_BINARY_EQ &&
    Type::real(exprBinary.left.type)->isObj() &&
    Type::real(exprBinary.right.type)->isObj() &&
    Type::real(exprBinary.left.type)->builtin &&
    Type::real(exprBinary.right.type)->builtin &&
    Type::real(exprBinary.left.type)->codeName == "@buffer_Buffer" &&
    Type::real(exprBinary.right.type)->codeName == "@buffer_Buffer"
  ) {
    auto leftCode = this->_nodeExpr(exprBinary.left, Type::real(exprBinary.left.type), parent, decl);
    auto rightCode = this->_nodeExpr(exprBinary.right, Type::real(exprBinary.right.type), parent, decl);
    code = this->_apiEval("_{buffer_eq}(" + leftCode + ", " + rightCode + ")", 1);
  } else if (
    exprBinary.op == AST_EXPR_BINARY_NE &&
    Type::real(exprBinary.left.type)->isObj() &&
    Type::real(exprBinary.right.type)->isObj() &&
    Type::real(exprBinary.left.type)->builtin &&
    Type::real(exprBinary.right.type)->builtin &&
    Type::real(exprBinary.left.type)->codeName == "@buffer_Buffer" &&
    Type::real(exprBinary.right.type)->codeName == "@buffer_Buffer"
  ) {
    auto leftCode = this->_nodeExpr(exprBinary.left, Type::real(exprBinary.left.type), parent, decl);
    auto rightCode = this->_nodeExpr(exprBinary.right, Type::real(exprBinary.right.type), parent, decl);
    code = this->_apiEval("_{buffer_ne}(" + leftCode + ", " + rightCode + ")", 1);
  } else if (exprBinary.op == AST_EXPR_BINARY_EQ && (
    (Type::real(exprBinary.left.type)->isArray() && Type::real(exprBinary.right.type)->isArray()) ||
    (Type::real(exprBinary.left.type)->isMap() && Type::real(exprBinary.right.type)->isMap()) ||
    (Type::real(exprBinary.left.type)->isObj() && Type::real(exprBinary.right.type)->isObj()) ||
    (Type::real(exprBinary.left.type)->isOpt() && Type::real(exprBinary.right.type)->isOpt())
  )) {
    auto typeInfo = this->_typeInfo(exprBinary.left.type);
    auto leftCode = this->_nodeExpr(exprBinary.left, typeInfo.realType, parent, decl);
    auto rightCode = this->_nodeExpr(exprBinary.right, typeInfo.realType, parent, decl);

    this->_activateEntity(typeInfo.realTypeName + "_eq");
    code = typeInfo.realTypeName + "_eq(" + leftCode + ", " + rightCode + ")";
  } else if (exprBinary.op == AST_EXPR_BINARY_NE && (
    (Type::real(exprBinary.left.type)->isArray() && Type::real(exprBinary.right.type)->isArray()) ||
    (Type::real(exprBinary.left.type)->isMap() && Type::real(exprBinary.right.type)->isMap()) ||
    (Type::real(exprBinary.left.type)->isObj() && Type::real(exprBinary.right.type)->isObj()) ||
    (Type::real(exprBinary.left.type)->isOpt() && Type::real(exprBinary.right.type)->isOpt())
  )) {
    auto typeInfo = this->_typeInfo(exprBinary.left.type);
    auto leftCode = this->_nodeExpr(exprBinary.left, typeInfo.realType, parent, decl);
    auto rightCode = this->_nodeExpr(exprBinary.right, typeInfo.realType, parent, decl);

    this->_activateEntity(typeInfo.realTypeName + "_ne");
    code = typeInfo.realTypeName + "_ne(" + leftCode + ", " + rightCode + ")";
  } else if (exprBinary.op == AST_EXPR_BINARY_EQ && (
    Type::real(exprBinary.left.type)->isStr() &&
    Type::real(exprBinary.right.type)->isStr()
  )) {
    if (exprBinary.left.isLit() && exprBinary.right.isLit()) {
      code = this->_apiEval("_{cstr_eq_cstr}(" + exprBinary.left.litBody() + ", " + exprBinary.right.litBody() + ")", 1);
    } else if (exprBinary.left.isLit()) {
      auto rightCode = this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str"), parent, decl);
      code = this->_apiEval("_{cstr_eq_str}(" + exprBinary.left.litBody() + ", " + rightCode + ")", 1);
    } else if (exprBinary.right.isLit()) {
      auto leftCode = this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str"), parent, decl);
      code = this->_apiEval("_{str_eq_cstr}(" + leftCode + ", " + exprBinary.right.litBody() + ")", 1);
    } else {
      auto leftCode = this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str"), parent, decl);
      auto rightCode = this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str"), parent, decl);
      code = this->_apiEval("_{str_eq_str}(" + leftCode + ", " + rightCode + ")", 1);
    }
  } else if (exprBinary.op == AST_EXPR_BINARY_NE && (
    Type::real(exprBinary.left.type)->isStr() &&
    Type::real(exprBinary.right.type)->isStr()
  )) {
    if (exprBinary.left.isLit() && exprBinary.right.isLit()) {
      code = this->_apiEval("_{cstr_ne_cstr}(" + exprBinary.left.litBody() + ", " + exprBinary.right.litBody() + ")", 1);
    } else if (exprBinary.left.isLit()) {
      auto rightCode = this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str"), parent, decl);
      code = this->_apiEval("_{cstr_ne_str}(" + exprBinary.left.litBody() + ", " + rightCode + ")", 1);
    } else if (exprBinary.right.isLit()) {
      auto leftCode = this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str"), parent, decl);
      code = this->_apiEval("_{str_ne_cstr}(" + leftCode + ", " + exprBinary.right.litBody() + ")", 1);
    } else {
      auto leftCode = this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str"), parent, decl);
      auto rightCode = this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str"), parent, decl);
      code = this->_apiEval("_{str_ne_str}(" + leftCode + ", " + rightCode + ")", 1);
    }
  } else if (exprBinary.op == AST_EXPR_BINARY_ADD && (
    Type::real(exprBinary.left.type)->isStr() &&
    Type::real(exprBinary.right.type)->isStr()
  )) {
    if (root && nodeExpr.isLit()) {
      return this->_wrapNodeExpr(nodeExpr, targetType, root, nodeExpr.litBody());
    } else if (nodeExpr.isLit()) {
      code = this->_apiEval("_{str_alloc}(" + nodeExpr.litBody() + ")", 1);
    } else if (exprBinary.left.isLit()) {
      code = this->_apiEval("_{cstr_concat_str}(" + exprBinary.left.litBody(), 1);
      code += ", " + this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str"), parent, decl) + ")";
    } else if (exprBinary.right.isLit()) {
      code = this->_apiEval("_{str_concat_cstr}(" + this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str"), parent, decl), 1);
      code += ", " + exprBinary.right.litBody() + ")";
    } else {
      code = this->_apiEval("_{str_concat_str}(" + this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str"), parent, decl), 1);
      code += ", " + this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str"), parent, decl) + ")";
    }

    code = !root ? code : this->_genFreeFn(Type::real(exprBinary.left.type), code);
  } else if (exprBinary.op == AST_EXPR_BINARY_EQ && (
    (Type::real(exprBinary.left.type)->isUnion() || Type::real(exprBinary.right.type)->isUnion())
  )) {
    auto unionType = Type::real(Type::real(exprBinary.left.type)->isUnion() ? exprBinary.left.type : exprBinary.right.type);
    auto typeInfo = this->_typeInfo(unionType);
    auto leftCode = this->_nodeExpr(exprBinary.left, typeInfo.realType, parent, decl);
    auto rightCode = this->_nodeExpr(exprBinary.right, typeInfo.realType, parent, decl);

    this->_activateEntity(typeInfo.realTypeName + "_eq");
    code = typeInfo.realTypeName + "_eq(" + leftCode + ", " + rightCode + ")";
  } else if (exprBinary.op == AST_EXPR_BINARY_NE && (
    (Type::real(exprBinary.left.type)->isUnion() || Type::real(exprBinary.right.type)->isUnion())
  )) {
    auto unionType = Type::real(Type::real(exprBinary.left.type)->isUnion() ? exprBinary.left.type : exprBinary.right.type);
    auto typeInfo = this->_typeInfo(unionType);
    auto leftCode = this->_nodeExpr(exprBinary.left, typeInfo.realType, parent, decl);
    auto rightCode = this->_nodeExpr(exprBinary.right, typeInfo.realType, parent, decl);

    this->_activateEntity(typeInfo.realTypeName + "_ne");
    code = typeInfo.realTypeName + "_ne(" + leftCode + ", " + rightCode + ")";
  } else {
    auto leftCode = this->_nodeExpr(exprBinary.left, Type::real(exprBinary.left.type), parent, decl);
    auto rightCode = this->_nodeExpr(exprBinary.right, Type::real(exprBinary.right.type), parent, decl);
    auto opCode = std::string();

    if (exprBinary.op == AST_EXPR_BINARY_ADD) opCode = " + ";
    else if (exprBinary.op == AST_EXPR_BINARY_AND) opCode = " && ";
    else if (exprBinary.op == AST_EXPR_BINARY_BIT_AND) opCode = " & ";
    else if (exprBinary.op == AST_EXPR_BINARY_BIT_OR) opCode = " | ";
    else if (exprBinary.op == AST_EXPR_BINARY_BIT_XOR) opCode = " ^ ";
    else if (exprBinary.op == AST_EXPR_BINARY_DIV) opCode = " / ";
    else if (exprBinary.op == AST_EXPR_BINARY_EQ) opCode = " == ";
    else if (exprBinary.op == AST_EXPR_BINARY_GE) opCode = " >= ";
    else if (exprBinary.op == AST_EXPR_BINARY_GT) opCode = " > ";
    else if (exprBinary.op == AST_EXPR_BINARY_LSHIFT) opCode = " << ";
    else if (exprBinary.op == AST_EXPR_BINARY_LE) opCode = " <= ";
    else if (exprBinary.op == AST_EXPR_BINARY_LT) opCode = " < ";
    else if (exprBinary.op == AST_EXPR_BINARY_MOD) opCode = " % ";
    else if (exprBinary.op == AST_EXPR_BINARY_MUL) opCode = " * ";
    else if (exprBinary.op == AST_EXPR_BINARY_NE) opCode = " != ";
    else if (exprBinary.op == AST_EXPR_BINARY_OR) opCode = " || ";
    else if (exprBinary.op == AST_EXPR_BINARY_RSHIFT) opCode = " >> ";
    else if (exprBinary.op == AST_EXPR_BINARY_SUB) opCode = " - ";

    code = leftCode + opCode + rightCode;
  }

  return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
}
