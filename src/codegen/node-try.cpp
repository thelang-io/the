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

void Codegen::_nodeTry (std::shared_ptr<CodegenASTStmt> *c, const ASTNode &node) {
  auto nodeTry = std::get<ASTNodeTry>(*node.body);
  auto initialStateCleanUp = this->state.cleanUp;
  this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &initialStateCleanUp);

  *c = (*c)->append(
    CodegenASTStmtSwitch::create(
      CodegenASTExprCall::create(
        CodegenASTExprAccess::create(this->_("setjmp")),
        {
          CodegenASTExprAccess::create(
            CodegenASTExprAccess::create(CodegenASTExprAccess::create(this->_("err_state")), "buf"),
            CodegenASTExprUnary::create(
              CodegenASTExprAccess::create(CodegenASTExprAccess::create(this->_("err_state")), "buf_idx"),
              "++"
            )
          )
        }
      )
    )
  );

  *c = (*c)->append(CodegenASTStmtCase::create(CodegenASTExprLiteral::create("0"), CodegenASTStmtCompound::create()));
  this->varMap.save();

  auto blockCleanUp = CodegenASTStmtCompound::create({
    CodegenASTStmtIf::create(
      CodegenASTExprBinary::create(
        CodegenASTExprAccess::create(CodegenASTExprAccess::create(this->_("err_state")), "id"),
        "!=",
        CodegenASTExprLiteral::create("-1")
      ),
      CodegenASTExprCall::create(
        CodegenASTExprAccess::create(this->_("longjmp")),
        {
          CodegenASTExprAccess::create(
            CodegenASTExprAccess::create(CodegenASTExprAccess::create(this->_("err_state")), "buf"),
            CodegenASTExprBinary::create(
              CodegenASTExprAccess::create(CodegenASTExprAccess::create(this->_("err_state")), "buf_idx"),
              "-",
              CodegenASTExprLiteral::create("1")
            )
          ),
          CodegenASTExprAccess::create(CodegenASTExprAccess::create(this->_("err_state")), "id")
        }
      )->stmt()
    ),
    CodegenASTExprUnary::create(
      CodegenASTExprAccess::create(CodegenASTExprAccess::create(this->_("err_state")), "buf_idx"),
      "--"
    )->stmt()
  });

  this->_block(c, nodeTry.body, true, blockCleanUp, true);
  (*c)->append(CodegenASTStmtBreak::create());
  *c = (*c)->exit();
  this->varMap.restore();
  this->state.cleanUp = initialStateCleanUp;

  for (const auto &handler : nodeTry.handlers) {
    auto handlerVarDecl = std::get<ASTNodeVarDecl>(*handler.param.body);
    auto handlerTypeInfo = this->_typeInfo(handlerVarDecl.var->type);
    auto handlerDef = this->_typeDef(handlerVarDecl.var->type);
    auto handleCodeName = Codegen::name(handlerVarDecl.var->codeName);

    *c = (*c)->append(CodegenASTStmtCase::create(
      CodegenASTExprAccess::create(this->_(handlerDef)),
      CodegenASTStmtCompound::create()
    ));

    (*c)->append(
      CodegenASTExprUnary::create(
        CodegenASTExprAccess::create(CodegenASTExprAccess::create(this->_("err_state")), "buf_idx"),
        "--"
      )->stmt()
    );

    (*c)->append(
      CodegenASTExprCall::create(
        CodegenASTExprAccess::create(this->_("error_unset")),
        {CodegenASTExprUnary::create("&", CodegenASTExprAccess::create(this->_("err_state")))}
      )->stmt()
    );

    (*c)->append(
      CodegenASTStmtVarDecl::create(
        CodegenASTType::create(handlerTypeInfo.typeCodeConst),
        CodegenASTExprAccess::create(handleCodeName),
        CodegenASTExprCast::create(
          CodegenASTType::create(handlerTypeInfo.typeCodeTrimmed),
          CodegenASTExprAccess::create(CodegenASTExprAccess::create(this->_("err_state")), "ctx")
        )
      )
    );

    this->varMap.save();

    this->_block(
      c,
      handler.body,
      true,
      this->_genFreeFn(handlerTypeInfo.type, CodegenASTExprAccess::create(handleCodeName))->stmt()
    );

    (*c)->append(CodegenASTStmtBreak::create());
    *c = (*c)->exit();
    this->varMap.restore();
  }

  *c = (*c)->append(CodegenASTStmtCase::create(nullptr, CodegenASTStmtCompound::create()));

  (*c)->append(
    CodegenASTExprUnary::create(
      CodegenASTExprAccess::create(CodegenASTExprAccess::create(this->_("err_state")), "buf_idx"),
      "--"
    )->stmt()
  );

  (*c)->append(CodegenASTStmtGoto::create(this->state.cleanUp.currentLabel()));
  *c = (*c)->exit()->exit();
}

void Codegen::_nodeTryAsync (std::shared_ptr<CodegenASTStmt> *c, const ASTNode &node) {
  this->_nodeTry(c, node);
}
