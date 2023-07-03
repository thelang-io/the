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

#include <algorithm>
#include "../Codegen.hpp"

std::shared_ptr<CodegenASTExpr> Codegen::_exprLit (const ASTNodeExpr &nodeExpr, Type *targetType, bool root) {
  auto exprLit = std::get<ASTExprLit>(*nodeExpr.body);
  auto expr = CodegenASTExprLiteral::create(exprLit.body);

  if (exprLit.type == AST_EXPR_LIT_BOOL) {
    expr = CodegenASTExprAccess::create(this->_(exprLit.body));
  } else if (exprLit.type == AST_EXPR_LIT_INT_DEC) {
    auto val = std::stoull(exprLit.body);

    if (val > 9223372036854775807) {
      expr = CodegenASTExprLiteral::create(exprLit.body + "U");
    }
  } else if (exprLit.type == AST_EXPR_LIT_INT_OCT) {
    auto val = exprLit.body;
    val.erase(std::remove(val.begin(), val.end(), 'O'), val.end());
    val.erase(std::remove(val.begin(), val.end(), 'o'), val.end());
    expr = CodegenASTExprLiteral::create(val);
  } else if (exprLit.type == AST_EXPR_LIT_NIL) {
    expr = CodegenASTExprAccess::create(this->_("NULL"));
  } else if (!root && exprLit.type == AST_EXPR_LIT_STR) {
    expr = CodegenASTExprCall::create(CodegenASTExprAccess::create(this->_("str_alloc")), {expr});
  }

  return this->_wrapNodeExpr(nodeExpr, targetType, root, expr);
}
