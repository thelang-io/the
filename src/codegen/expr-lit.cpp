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

std::string Codegen::_exprLit (const ASTNodeExpr &nodeExpr, Type *targetType, [[maybe_unused]] const ASTNode &parent, [[maybe_unused]] std::string &decl, bool root) {
  auto exprLit = std::get<ASTExprLit>(*nodeExpr.body);
  auto code = exprLit.body;

  if (exprLit.type == AST_EXPR_LIT_BOOL) {
    code = this->_apiEval("_{" + code + "}");
  } else if (exprLit.type == AST_EXPR_LIT_INT_DEC) {
    auto val = std::stoull(code);

    if (val > 9223372036854775807) {
      code += "U";
    }
  } else if (exprLit.type == AST_EXPR_LIT_INT_OCT) {
    code.erase(std::remove(code.begin(), code.end(), 'O'), code.end());
    code.erase(std::remove(code.begin(), code.end(), 'o'), code.end());
  } else if (exprLit.type == AST_EXPR_LIT_NIL) {
    code = this->_apiEval("_{NULL}");
  } else if (!root && exprLit.type == AST_EXPR_LIT_STR) {
    code = this->_apiEval("_{str_alloc}(" + code + ")", 1);
  }

  return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
}
