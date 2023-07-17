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
            this->_genErrState(ASTChecker(node).insideMain(), false, "buf"),
            CodegenASTExprUnary::create(this->_genErrState(ASTChecker(node).insideMain(), false, "buf_idx"), "++")
          )
        }
      )
    )
  );

  *c = (*c)->append(
    CodegenASTStmtCase::create(CodegenASTExprLiteral::create("0"), CodegenASTStmtCompound::create())
  );

  this->varMap.save();

  auto blockCleanUp = CodegenASTStmtCompound::create({
    CodegenASTStmtIf::create(
      CodegenASTExprBinary::create(
        this->_genErrState(ASTChecker(node).insideMain(), false, "id"),
        "!=",
        CodegenASTExprLiteral::create("-1")
      ),
      CodegenASTExprCall::create(
        CodegenASTExprAccess::create(this->_("longjmp")),
        {
          CodegenASTExprAccess::create(
            this->_genErrState(ASTChecker(node).insideMain(), false, "buf"),
            CodegenASTExprBinary::create(
              this->_genErrState(ASTChecker(node).insideMain(), false, "buf_idx"),
              "-",
              CodegenASTExprLiteral::create("1")
            )
          ),
          this->_genErrState(ASTChecker(node).insideMain(), false, "id")
        }
      )->stmt()
    ),
    CodegenASTExprUnary::create(this->_genErrState(ASTChecker(node).insideMain(), false, "buf_idx"), "--")->stmt()
  });

  this->_block(c, nodeTry.body, true, blockCleanUp, true);
  (*c)->append(CodegenASTStmtBreak::create());
  *c = (*c)->exit()->exit();
  this->varMap.restore();
  this->state.cleanUp = initialStateCleanUp;

  for (const auto &handler : nodeTry.handlers) {
    auto handlerVarDecl = std::get<ASTNodeVarDecl>(*handler.param.body);
    auto handlerTypeInfo = this->_typeInfo(handlerVarDecl.var->type);
    auto handlerDef = this->_typeDef(handlerVarDecl.var->type);
    auto handleCodeName = Codegen::name(handlerVarDecl.var->codeName);

    *c = (*c)->append(
      CodegenASTStmtCase::create(
        CodegenASTExprAccess::create(this->_(handlerDef)),
        CodegenASTStmtCompound::create()
      )
    );

    (*c)->append(
      CodegenASTExprUnary::create(this->_genErrState(ASTChecker(node).insideMain(), false, "buf_idx"), "--")->stmt()
    );

    (*c)->append(
      CodegenASTExprCall::create(
        CodegenASTExprAccess::create(this->_("error_unset")),
        {this->_genErrState(ASTChecker(node).insideMain(), false)}
      )->stmt()
    );

    (*c)->append(
      CodegenASTStmtVarDecl::create(
        CodegenASTType::create(handlerTypeInfo.typeCodeConst),
        CodegenASTExprAccess::create(handleCodeName),
        CodegenASTExprCast::create(
          CodegenASTType::create(handlerTypeInfo.typeCodeTrimmed),
          this->_genErrState(ASTChecker(node).insideMain(), false, "ctx")
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
    *c = (*c)->exit()->exit();
    this->varMap.restore();
  }

  *c = (*c)->append(CodegenASTStmtCase::create(nullptr, CodegenASTStmtCompound::create()));

  (*c)->append(
    CodegenASTExprUnary::create(this->_genErrState(ASTChecker(node).insideMain(), false, "buf_idx"), "--")->stmt()
  );

  (*c)->append(CodegenASTStmtGoto::create(this->state.cleanUp.currentLabel()));
  *c = (*c)->exit()->exit()->exit();
}

void Codegen::_nodeTryAsync (std::shared_ptr<CodegenASTStmt> *c, const ASTNode &node) {
  auto nodeTry = std::get<ASTNodeTry>(*node.body);
  auto initialStateCleanUp = this->state.cleanUp;
  auto endAsyncCounter = std::make_shared<std::size_t>(0);
  auto handlersAsyncCounters = std::vector<std::shared_ptr<std::size_t>>{};

  auto checkCleanUp = std::vector<std::shared_ptr<CodegenASTStmt>>{
    CodegenASTStmtIf::create(
      CodegenASTExprBinary::create(
        this->_genErrState(ASTChecker(node).insideMain(), false, "id"),
        "==",
        CodegenASTExprLiteral::create("-1")
      ),
      this->_genAsyncReturn(endAsyncCounter)
    )
  };

  for (const auto &handler : nodeTry.handlers) {
    auto handlerVarDecl = std::get<ASTNodeVarDecl>(*handler.param.body);
    auto handlerDef = this->_typeDef(handlerVarDecl.var->type);

    handlersAsyncCounters.push_back(std::make_shared<std::size_t>(0));

    checkCleanUp.push_back(
      CodegenASTStmtIf::create(
        CodegenASTExprBinary::create(
          this->_genErrState(ASTChecker(node).insideMain(), false, "id"),
          "==",
          CodegenASTExprAccess::create(this->_(handlerDef))
        ),
        this->_genAsyncReturn(handlersAsyncCounters.back())
      )
    );
  }

  this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_BLOCK, &initialStateCleanUp);
  this->state.cleanUp.merge(CodegenASTStmtCompound::create(checkCleanUp));
  this->varMap.save();
  this->_block(c, nodeTry.body, true);
  this->varMap.restore();

  if (!(*c)->endsWith<CodegenASTStmtReturn>()) {
    (*c)->append(this->_genAsyncReturn(endAsyncCounter));
  } else {
    *c = (*c)->increaseAsyncCounter(this->state.asyncCounter);
    (*c)->append(this->_genAsyncReturn(endAsyncCounter));
  }

  this->state.cleanUp.genAsync(c, this->state.asyncCounter);
  this->state.cleanUp = initialStateCleanUp;

  (*c)->append(
    initialStateCleanUp.hasCleanUp(CODEGEN_CLEANUP_FN)
      ? this->_genAsyncReturn(initialStateCleanUp.currentLabelAsync())
      : CodegenASTStmtBreak::create()
  );

  *c = (*c)->increaseAsyncCounter(this->state.asyncCounter);

  for (auto idx = static_cast<std::size_t>(0); idx < nodeTry.handlers.size(); idx++) {
    auto handler = nodeTry.handlers[idx];
    auto handlerVarDecl = std::get<ASTNodeVarDecl>(*handler.param.body);
    auto handlerTypeInfo = this->_typeInfo(handlerVarDecl.var->type);
    auto handlerCodeName = Codegen::name(handlerVarDecl.var->codeName);
    auto cHandlerFree = CodegenASTExprUnary::create("*", CodegenASTExprAccess::create(handlerCodeName));

    *handlersAsyncCounters[idx] = this->state.asyncCounter;

    (*c)->append(
      CodegenASTExprCall::create(
        CodegenASTExprAccess::create(this->_("error_unset")),
        {this->_genErrState(ASTChecker(node).insideMain(), false)}
      )->stmt()
    );

    (*c)->append(
      CodegenASTExprAssign::create(
        CodegenASTExprUnary::create("*", CodegenASTExprAccess::create(handlerCodeName)),
        "=",
        CodegenASTExprCast::create(
          CodegenASTType::create(handlerTypeInfo.typeCodeTrimmed),
          this->_genErrState(ASTChecker(node).insideMain(), false, "ctx")
        )
      )->stmt()
    );

    this->varMap.save();
    this->_block(c, handler.body, true, this->_genFreeFn(handlerTypeInfo.type, cHandlerFree)->stmt());
    this->varMap.restore();

    if (!(*c)->endsWith<CodegenASTStmtReturn>()) {
      (*c)->append(this->_genAsyncReturn(endAsyncCounter));
    }

    *c = (*c)->increaseAsyncCounter(this->state.asyncCounter);
  }

  *endAsyncCounter = this->state.asyncCounter;
}
