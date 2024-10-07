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

std::shared_ptr<CodegenASTExpr> Codegen::_nodeVarDeclInit (const CodegenTypeInfo &typeInfo) {
  if (typeInfo.type->isAny()) {
    return CodegenASTExprInitList::create({
      CodegenASTExprLiteral::create("0"),
      CodegenASTExprAccess::create(this->_("NULL")),
      CodegenASTExprLiteral::create("0"),
      CodegenASTExprAccess::create(this->_("NULL")),
      CodegenASTExprAccess::create(this->_("NULL"))
    });
  } else if (typeInfo.type->isArray() || typeInfo.type->isMap()) {
    return CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_(typeInfo.typeName + "_alloc")),
      {CodegenASTExprLiteral::create("0")}
    );
  } else if (typeInfo.type->isBool()) {
    return CodegenASTExprAccess::create(this->_("false"));
  } else if (typeInfo.type->isChar()) {
    return CodegenASTExprLiteral::create(R"('\0')");
  } else if (typeInfo.type->isFn() || typeInfo.type->isRef() || typeInfo.type->isUnion()) {
    throw Error("tried node variable declaration of invalid type");
  } else if (typeInfo.type->isObj()) {
    auto cArgs = std::vector<std::shared_ptr<CodegenASTExpr>>{};

    for (const auto &typeField : typeInfo.type->fields) {
      if (!typeField.builtin && !typeField.type->isMethod()) {
        cArgs.push_back(this->_exprObjDefaultField(typeField.name, this->_typeInfo(typeField.type)));
      }
    }

    return CodegenASTExprCall::create(CodegenASTExprAccess::create(this->_(typeInfo.typeName + "_alloc")), cArgs);
  } else if (typeInfo.type->isOpt()) {
    return CodegenASTExprAccess::create(this->_("NULL"));
  } else if (typeInfo.type->isStr()) {
    return CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_("str_alloc")),
      {CodegenASTExprLiteral::create(R"("")")}
    );
  } else {
    return CodegenASTExprLiteral::create("0");
  }
}

void Codegen::_nodeVarDecl (std::shared_ptr<CodegenASTStmt> *c, const ASTNode &node, CodegenPhase phase) {
  if (phase != CODEGEN_PHASE_ALLOC && phase != CODEGEN_PHASE_FULL) {
    return;
  }

  auto nodeVarDecl = std::get<ASTNodeVarDecl>(*node.body);
  auto name = Codegen::name(nodeVarDecl.var->codeName);
  auto typeInfo = this->_typeInfo(nodeVarDecl.var->type);

  auto cInit = nodeVarDecl.init == std::nullopt
    ? this->_nodeVarDeclInit(typeInfo)
    : this->_nodeExpr(*nodeVarDecl.init, typeInfo.type, node, c);

  (*c)->append(
    CodegenASTStmtVarDecl::create(
      CodegenASTType::create(nodeVarDecl.var->mut ? typeInfo.typeCode : typeInfo.typeCodeConst),
      CodegenASTExprAccess::create(name),
      cInit
    )
  );

  if (typeInfo.type->shouldBeFreed()) {
    this->state.cleanUp.add(this->_genFreeFn(typeInfo.type, CodegenASTExprAccess::create(name))->stmt());
  }
}

void Codegen::_nodeVarDeclAsync (std::shared_ptr<CodegenASTStmt> *c, const ASTNode &node, CodegenPhase phase) {
  if (phase != CODEGEN_PHASE_ALLOC && phase != CODEGEN_PHASE_FULL) {
    return;
  }

  auto nodeVarDecl = std::get<ASTNodeVarDecl>(*node.body);
  auto name = Codegen::name(nodeVarDecl.var->codeName);
  auto typeInfo = this->_typeInfo(nodeVarDecl.var->type);

  auto cInit = nodeVarDecl.init == std::nullopt
    ? this->_nodeVarDeclInit(typeInfo)
    : this->_nodeExpr(*nodeVarDecl.init, typeInfo.type, node, c);

  (*c)->append(
    CodegenASTExprAssign::create(
      CodegenASTExprUnary::create("*", CodegenASTExprAccess::create(name)),
      "=",
      cInit
    )->stmt()
  );

  if (typeInfo.type->shouldBeFreed()) {
    this->state.cleanUp.add(
      this->_genFreeFn(typeInfo.type, CodegenASTExprUnary::create("*", CodegenASTExprAccess::create(name)))->stmt()
    );
  }
}
