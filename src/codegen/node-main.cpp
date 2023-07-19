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

void Codegen::_nodeMain (std::shared_ptr<CodegenASTStmt> *c, const ASTNode &node) {
  auto nodeMain = std::get<ASTNodeMain>(*node.body);
  this->varMap.save();
  this->_block(c, nodeMain.body);
  this->varMap.restore();
}

void Codegen::_nodeMainAsync (std::shared_ptr<CodegenASTStmt> *c, const ASTNode &node) {
  auto nodeMain = std::get<ASTNodeMain>(*node.body);
  auto returnType = this->ast->typeMap.get("void");
  auto asyncMainType = this->ast->typeMap.createFn({}, returnType, this->throws, this->async);
  auto asyncMainVar = std::make_shared<Var>(Var{"async_main", "async_main", asyncMainType, false, false, true, false, 0});
  auto paramsName = Codegen::typeName(asyncMainVar->type->name + "P");

  this->_fnDecl(c, asyncMainVar, nodeMain.stack, {}, nodeMain.body, node, CODEGEN_PHASE_FULL);

  auto cParams = this->throws
    ? CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_("xalloc")),
      {
        CodegenASTExprUnary::create("&", CodegenASTExprCast::create(
          CodegenASTType::create("struct " + this->_(paramsName)),
          CodegenASTExprInitList::create({this->_genErrState(true, true)})
        )),
        CodegenASTExprCall::create(
          CodegenASTExprAccess::create("sizeof"),
          {CodegenASTType::create("struct " + this->_(paramsName))}
        )
      }
    )
    : CodegenASTExprAccess::create(this->_("NULL"));

  (*c)->append(
    CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_("threadpool_add")),
      {
        CodegenASTExprAccess::create("tp"),
        CodegenASTExprAccess::create(CodegenASTExprAccess::create("async_main"), "f"),
        CodegenASTExprCall::create(
          CodegenASTExprAccess::create(this->_("xalloc")),
          {
            CodegenASTExprAccess::create(CodegenASTExprAccess::create("async_main"), "x"),
            CodegenASTExprAccess::create(CodegenASTExprAccess::create("async_main"), "l")
          }
        ),
        cParams,
        CodegenASTExprAccess::create(this->_("NULL")),
        CodegenASTExprAccess::create(this->_("NULL"))
      }
    )->stmt()
  );
}
