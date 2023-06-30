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

CodegenASTExpr Codegen::_exprBinary (const ASTNodeExpr &nodeExpr, Type *targetType, const ASTNode &parent, CodegenASTStmt *c, bool root) {
  auto exprBinary = std::get<ASTExprBinary>(*nodeExpr.body);
  auto direction = std::string(exprBinary.op == AST_EXPR_BINARY_EQ ? "eq" : "ne");
  auto expr = CodegenASTExpr{};

  if (
    (exprBinary.op == AST_EXPR_BINARY_EQ || exprBinary.op == AST_EXPR_BINARY_NE) &&
    Type::real(exprBinary.left.type)->isObj() &&
    Type::real(exprBinary.right.type)->isObj() &&
    Type::real(exprBinary.left.type)->builtin &&
    Type::real(exprBinary.right.type)->builtin &&
    Type::real(exprBinary.left.type)->codeName == "@buffer_Buffer" &&
    Type::real(exprBinary.right.type)->codeName == "@buffer_Buffer"
  ) {
    auto cLeft = this->_nodeExpr(exprBinary.left, Type::real(exprBinary.left.type), parent, c);
    auto cRight = this->_nodeExpr(exprBinary.right, Type::real(exprBinary.right.type), parent, c);
    expr = CodegenASTExprCall::create(CodegenASTExprAccess::create(this->_("buffer_" + direction)), {cLeft, cRight});
  } else if ((exprBinary.op == AST_EXPR_BINARY_EQ || exprBinary.op == AST_EXPR_BINARY_NE) && (
    (Type::real(exprBinary.left.type)->isArray() && Type::real(exprBinary.right.type)->isArray()) ||
    (Type::real(exprBinary.left.type)->isMap() && Type::real(exprBinary.right.type)->isMap()) ||
    (Type::real(exprBinary.left.type)->isObj() && Type::real(exprBinary.right.type)->isObj()) ||
    (Type::real(exprBinary.left.type)->isOpt() && Type::real(exprBinary.right.type)->isOpt())
  )) {
    auto typeInfo = this->_typeInfo(exprBinary.left.type);
    auto cLeft = this->_nodeExpr(exprBinary.left, typeInfo.realType, parent, c);
    auto cRight = this->_nodeExpr(exprBinary.right, typeInfo.realType, parent, c);

    expr = CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_(typeInfo.realTypeName + "_" + direction)),
      {cLeft, cRight}
    );
  } else if ((exprBinary.op == AST_EXPR_BINARY_EQ || exprBinary.op == AST_EXPR_BINARY_NE) && (
    Type::real(exprBinary.left.type)->isStr() &&
    Type::real(exprBinary.right.type)->isStr()
  )) {
    if (exprBinary.left.isLit() && exprBinary.right.isLit()) {
      auto cLeft = CodegenASTExprLiteral::create(exprBinary.left.litBody());
      auto cRight = CodegenASTExprLiteral::create(exprBinary.right.litBody());

      expr = CodegenASTExprCall::create(
        CodegenASTExprAccess::create(this->_("cstr_" + direction + "_cstr")),
        {cLeft, cRight}
      );
    } else if (exprBinary.left.isLit()) {
      auto cLeft = CodegenASTExprLiteral::create(exprBinary.left.litBody());
      auto cRight = this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str"), parent, c);

      expr = CodegenASTExprCall::create(
        CodegenASTExprAccess::create(this->_("cstr_" + direction + "_str")),
        {cLeft, cRight}
      );
    } else if (exprBinary.right.isLit()) {
      auto cLeft = this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str"), parent, c);
      auto cRight = CodegenASTExprLiteral::create(exprBinary.right.litBody());

      expr = CodegenASTExprCall::create(
        CodegenASTExprAccess::create(this->_("str_" + direction + "_cstr")),
        {cLeft, cRight}
      );
    } else {
      auto cLeft = this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str"), parent, c);
      auto cRight = this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str"), parent, c);

      expr = CodegenASTExprCall::create(
        CodegenASTExprAccess::create(this->_("str_" + direction + "_str")),
        {cLeft, cRight}
      );
    }
  } else if (exprBinary.op == AST_EXPR_BINARY_ADD && (
    Type::real(exprBinary.left.type)->isStr() &&
    Type::real(exprBinary.right.type)->isStr()
  )) {
    if (root && nodeExpr.isLit()) {
      expr = CodegenASTExprLiteral::create(nodeExpr.litBody());
    } else if (nodeExpr.isLit()) {
      auto cArg = CodegenASTExprLiteral::create(nodeExpr.litBody());
      expr = CodegenASTExprCall::create(CodegenASTExprAccess::create(this->_("str_alloc")), {cArg});
    } else if (exprBinary.left.isLit()) {
      auto cLeft = CodegenASTExprLiteral::create(exprBinary.left.litBody());
      auto cRight = this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str"), parent, c);
      expr = CodegenASTExprCall::create(CodegenASTExprAccess::create(this->_("cstr_concat_str")), {cLeft, cRight});
    } else if (exprBinary.right.isLit()) {
      auto cLeft = this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str"), parent, c);
      auto cRight = CodegenASTExprLiteral::create(exprBinary.right.litBody());
      expr = CodegenASTExprCall::create(CodegenASTExprAccess::create(this->_("str_concat_cstr")), {cLeft, cRight});
    } else {
      auto cLeft = this->_nodeExpr(exprBinary.left, this->ast->typeMap.get("str"), parent, c);
      auto cRight = this->_nodeExpr(exprBinary.right, this->ast->typeMap.get("str"), parent, c);
      expr = CodegenASTExprCall::create(CodegenASTExprAccess::create(this->_("str_concat_str")), {cLeft, cRight});
    }

    if (root && !nodeExpr.isLit()) {
      expr = this->_genFreeFn(Type::real(exprBinary.left.type), expr);
    }
  } else if ((exprBinary.op == AST_EXPR_BINARY_EQ || exprBinary.op == AST_EXPR_BINARY_NE) && (
    (Type::real(exprBinary.left.type)->isUnion() || Type::real(exprBinary.right.type)->isUnion())
  )) {
    auto unionType = Type::real(Type::real(exprBinary.left.type)->isUnion() ? exprBinary.left.type : exprBinary.right.type);
    auto typeInfo = this->_typeInfo(unionType);
    auto cLeft = this->_nodeExpr(exprBinary.left, typeInfo.realType, parent, c);
    auto cRight = this->_nodeExpr(exprBinary.right, typeInfo.realType, parent, c);

    expr = CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_(typeInfo.realTypeName + "_" + direction)),
      {cLeft, cRight}
    );
  } else {
    auto cLeft = this->_nodeExpr(exprBinary.left, Type::real(exprBinary.left.type), parent, c);
    auto cRight = this->_nodeExpr(exprBinary.right, Type::real(exprBinary.right.type), parent, c);
    auto opCode = std::string();

    if (exprBinary.op == AST_EXPR_BINARY_ADD) opCode = "+";
    else if (exprBinary.op == AST_EXPR_BINARY_AND) opCode = "&&";
    else if (exprBinary.op == AST_EXPR_BINARY_BIT_AND) opCode = "&";
    else if (exprBinary.op == AST_EXPR_BINARY_BIT_OR) opCode = "|";
    else if (exprBinary.op == AST_EXPR_BINARY_BIT_XOR) opCode = "^";
    else if (exprBinary.op == AST_EXPR_BINARY_DIV) opCode = "/";
    else if (exprBinary.op == AST_EXPR_BINARY_EQ) opCode = "==";
    else if (exprBinary.op == AST_EXPR_BINARY_GE) opCode = ">=";
    else if (exprBinary.op == AST_EXPR_BINARY_GT) opCode = ">";
    else if (exprBinary.op == AST_EXPR_BINARY_LSHIFT) opCode = "<<";
    else if (exprBinary.op == AST_EXPR_BINARY_LE) opCode = "<=";
    else if (exprBinary.op == AST_EXPR_BINARY_LT) opCode = "<";
    else if (exprBinary.op == AST_EXPR_BINARY_MOD) opCode = "%";
    else if (exprBinary.op == AST_EXPR_BINARY_MUL) opCode = "*";
    else if (exprBinary.op == AST_EXPR_BINARY_NE) opCode = "!=";
    else if (exprBinary.op == AST_EXPR_BINARY_OR) opCode = "||";
    else if (exprBinary.op == AST_EXPR_BINARY_RSHIFT) opCode = ">>";
    else if (exprBinary.op == AST_EXPR_BINARY_SUB) opCode = "-";

    expr = CodegenASTExprBinary::create(cLeft, opCode, cRight);
  }

  return this->_wrapNodeExpr(nodeExpr, targetType, root, expr);
}
