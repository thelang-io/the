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

std::shared_ptr<CodegenASTExpr> Codegen::_exprAccess (const ASTNodeExpr &nodeExpr, Type *targetType, const ASTNode &parent, std::shared_ptr<CodegenASTStmt> *c, bool root) {
  auto line = std::to_string(nodeExpr.start.line);
  auto col = std::to_string(nodeExpr.start.col + 1);
  auto exprAccess = std::get<ASTExprAccess>(*nodeExpr.body);
  auto expr = CodegenASTExprNull::create();

  if (exprAccess.expr != std::nullopt && std::holds_alternative<std::shared_ptr<Var>>(*exprAccess.expr)) {
    auto objVar = std::get<std::shared_ptr<Var>>(*exprAccess.expr);

    if (objVar->builtin && (objVar->codeName == "@math_MaxFloat" || objVar->codeName == "@math_MaxF64")) {
      expr = CodegenASTExprAccess::create(this->_("DBL_MAX"));
    } else if (objVar->builtin && (objVar->codeName == "@math_MinFloat" || objVar->codeName == "@math_MinF64")) {
      expr = CodegenASTExprUnary::create("-", CodegenASTExprAccess::create(this->_("DBL_MAX")));
    } else if (objVar->builtin && objVar->codeName == "@math_MaxF32") {
      expr = CodegenASTExprAccess::create(this->_("FLT_MAX"));
    } else if (objVar->builtin && objVar->codeName == "@math_MinF32") {
      expr = CodegenASTExprUnary::create("-", CodegenASTExprAccess::create(this->_("FLT_MAX")));
    } else if (objVar->builtin && (objVar->codeName == "@math_MaxInt" || objVar->codeName == "@math_MaxI32")) {
      expr = CodegenASTExprAccess::create(this->_("INT32_MAX"));
    } else if (objVar->builtin && (objVar->codeName == "@math_MinInt" || objVar->codeName == "@math_MinI32")) {
      expr = CodegenASTExprAccess::create(this->_("INT32_MIN"));
    } else if (objVar->builtin && objVar->codeName == "@math_MaxI8") {
      expr = CodegenASTExprAccess::create(this->_("INT8_MAX"));
    } else if (objVar->builtin && objVar->codeName == "@math_MinI8") {
      expr = CodegenASTExprAccess::create(this->_("INT8_MIN"));
    } else if (objVar->builtin && objVar->codeName == "@math_MaxI16") {
      expr = CodegenASTExprAccess::create(this->_("INT16_MAX"));
    } else if (objVar->builtin && objVar->codeName == "@math_MinI16") {
      expr = CodegenASTExprAccess::create(this->_("INT16_MIN"));
    } else if (objVar->builtin && objVar->codeName == "@math_MaxI64") {
      expr = CodegenASTExprAccess::create(this->_("INT64_MAX"));
    } else if (objVar->builtin && objVar->codeName == "@math_MinI64") {
      expr = CodegenASTExprAccess::create(this->_("INT64_MIN"));
    } else if (objVar->builtin && objVar->codeName == "@math_MaxU8") {
      expr = CodegenASTExprAccess::create(this->_("UINT8_MAX"));
    } else if (objVar->builtin && objVar->codeName == "@math_MaxU16") {
      expr = CodegenASTExprAccess::create(this->_("UINT16_MAX"));
    } else if (objVar->builtin && objVar->codeName == "@math_MaxU32") {
      expr = CodegenASTExprAccess::create(this->_("UINT32_MAX"));
    } else if (objVar->builtin && objVar->codeName == "@math_MaxU64") {
      expr = CodegenASTExprAccess::create(this->_("UINT64_MAX"));
    } else if (objVar->builtin && (
      objVar->codeName == "@math_MinU8" ||
      objVar->codeName == "@math_MinU16" ||
      objVar->codeName == "@math_MinU32" ||
      objVar->codeName == "@math_MinU64"
    )) {
      expr = CodegenASTExprLiteral::create("0");
    } else if (objVar->builtin && objVar->codeName == "@os_ARCH") {
      expr = CodegenASTExprCall::create(
        CodegenASTExprAccess::create(this->_("os_arch")),
        {
          this->_genErrState(ASTChecker(parent).insideMain(), false),
          CodegenASTExprLiteral::create(line),
          CodegenASTExprLiteral::create(col)
        }
      );

      if (root) {
        expr = this->_genFreeFn(objVar->type, expr);
      }
    } else if (objVar->builtin && objVar->codeName == "@os_EOL") {
      expr = CodegenASTExprCall::create(
        CodegenASTExprAccess::create(this->_("str_alloc")),
        {CodegenASTExprAccess::create(this->_("THE_EOL"))}
      );

      if (root) {
        expr = this->_genFreeFn(objVar->type, expr);
      }
    } else if (objVar->builtin && objVar->codeName == "@os_NAME") {
      expr = CodegenASTExprCall::create(
        CodegenASTExprAccess::create(this->_("os_name")),
        {
          this->_genErrState(ASTChecker(parent).insideMain(), false),
          CodegenASTExprLiteral::create(line),
          CodegenASTExprLiteral::create(col)
        }
      );

      if (root) {
        expr = this->_genFreeFn(objVar->type, expr);
      }
    } else if (objVar->builtin && objVar->codeName == "@path_SEP") {
      expr = CodegenASTExprCall::create(
        CodegenASTExprAccess::create(this->_("str_alloc")),
        {CodegenASTExprAccess::create(this->_("THE_PATH_SEP"))}
      );

      if (root) {
        expr = this->_genFreeFn(objVar->type, expr);
      }
    } else if (objVar->builtin && objVar->codeName == "@process_args") {
      this->needMainArgs = true;
      expr = CodegenASTExprCall::create(CodegenASTExprAccess::create(this->_("process_args")));

      if (root) {
        expr = this->_genFreeFn(objVar->type, expr);
      }
    } else if (objVar->builtin && objVar->codeName == "@process_env") {
      expr = CodegenASTExprCall::create(CodegenASTExprAccess::create(this->_("process_env")));

      if (root) {
        expr = this->_genFreeFn(objVar->type, expr);
      }
    } else if (objVar->builtin && objVar->codeName == "@process_home") {
      expr = CodegenASTExprCall::create(
        CodegenASTExprAccess::create(this->_("process_home")),
        {
          this->_genErrState(ASTChecker(parent).insideMain(), false),
          CodegenASTExprLiteral::create(line),
          CodegenASTExprLiteral::create(col)
        }
      );

      if (root) {
        expr = this->_genFreeFn(objVar->type, expr);
      }
    } else {
      auto objName = Codegen::name(objVar->codeName);
      auto objType = this->state.typeCasts.contains(objName) ? this->state.typeCasts[objName] : objVar->type;

      expr = CodegenASTExprAccess::create(objName);

      if (objVar->type->isAny() && !objType->isAny()) {
        auto typeName = this->_typeNameAny(objType);

        expr = CodegenASTExprAccess::create(
          CodegenASTExprCast::create(
            CodegenASTType::create("struct " + this->_(typeName) + " *"),
            CodegenASTExprAccess::create(expr, "d")
          )->wrap(),
          "d",
          true
        );
      } else if (objVar->type->isOpt() && !objType->isOpt()) {
        expr = CodegenASTExprUnary::create("*", expr);
      } else if (objVar->type->isUnion() && (!objType->isUnion() || objVar->type->hasSubType(objType))) {
        expr = CodegenASTExprAccess::create(expr, "v" + this->_typeDefIdx(objType));
      }

      if (this->state.contextVars.contains(objName) && (nodeExpr.type->isRef() || !targetType->isRef())) {
        expr = CodegenASTExprUnary::create("*", expr);
      }

      if (
        !this->state.contextVars.contains(objName) &&
        ((!nodeExpr.type->isRef() && targetType->isRef()) || targetType->isRefOf(nodeExpr.type))
      ) {
        expr = CodegenASTExprUnary::create("&", expr);
      } else if (nodeExpr.type->isRefOf(targetType)) {
        auto refNodeExprType = Type::actual(nodeExpr.type);

        while (refNodeExprType->isRefOf(targetType)) {
          expr = CodegenASTExprUnary::create("*", expr);
          refNodeExprType = Type::actual(std::get<TypeRef>(refNodeExprType->body).refType);
        }
      }

      auto nodeExprTypeRefAny = targetType->isAny() && !nodeExpr.type->isRefOf(targetType);
      auto shouldCopyVar = !objType->isRef() || (!targetType->isRef() && !targetType->hasSubType(objType) && !nodeExprTypeRefAny);

      if (!root && shouldCopyVar) {
        expr = this->_genCopyFn(Type::real(objType), expr);
      }
    }
  } else if (exprAccess.expr != std::nullopt && std::holds_alternative<ASTNodeExpr>(*exprAccess.expr)) {
    auto objNodeExpr = std::get<ASTNodeExpr>(*exprAccess.expr);
    auto objTypeInfo = this->_typeInfo(objNodeExpr.type);
    auto originalObjMemberType = nodeExpr.type;
    auto fieldTypeHasCallInfo = false;
    auto fieldType = static_cast<Type *>(nullptr);

    if (
      exprAccess.prop != std::nullopt &&
      objTypeInfo.realType->hasField(*exprAccess.prop) &&
      objTypeInfo.realType->getField(*exprAccess.prop).builtin &&
      !objTypeInfo.realType->getField(*exprAccess.prop).callInfo.empty()
    ) {
      auto typeField = objTypeInfo.realType->getField(*exprAccess.prop);
      auto cArgs = std::vector<std::shared_ptr<CodegenASTExpr>>{};

      if (this->throws && typeField.callInfo.throws) {
        cArgs.push_back(this->_genErrState(ASTChecker(parent).insideMain(), false));
        cArgs.push_back(CodegenASTExprLiteral::create(line));
        cArgs.push_back(CodegenASTExprLiteral::create(col));
      }

      if (typeField.callInfo.isSelfFirst) {
        cArgs.push_back(
          this->_nodeExpr(objNodeExpr, typeField.callInfo.selfType, parent, c, typeField.callInfo.isSelfMut)
        );
      }

      expr = CodegenASTExprCall::create(
        CodegenASTExprAccess::create(this->_(typeField.callInfo.codeName)),
        cArgs
      );

      fieldTypeHasCallInfo = true;
      fieldType = typeField.type;
    } else if (exprAccess.prop != std::nullopt && objTypeInfo.realType->isEnum()) {
      auto enumType = std::get<TypeEnum>(objTypeInfo.realType->body);

      for (const auto &member : enumType.members) {
        if (member->name == *exprAccess.prop) {
          expr = CodegenASTExprAccess::create(Codegen::name(member->codeName));
          break;
        }
      }
    } else if (exprAccess.prop != std::nullopt && objTypeInfo.realType->isNamespace()) {
      if (!objTypeInfo.realType->hasField(*exprAccess.prop)) {
        throw Error("tried access expression on non-existing namespace member");
      }

      auto typeField = objTypeInfo.realType->getField(*exprAccess.prop);
      auto memberName = Codegen::name(typeField.callInfo.codeName);
      expr = CodegenASTExprAccess::create(memberName);

      if (this->state.contextVars.contains(memberName) && (nodeExpr.type->isRef() || !targetType->isRef())) {
        expr = CodegenASTExprUnary::create("*", expr);
      }
    } else if (exprAccess.prop != std::nullopt) {
      auto cObj = this->_nodeExpr(objNodeExpr, objTypeInfo.realType, parent, c, true);

      if (cObj->isPointer()) {
        cObj = cObj->wrap();
      }

      expr = CodegenASTExprAccess::create(cObj, Codegen::name(*exprAccess.prop), true);

      for (const auto &field : objTypeInfo.realType->fields) {
        if (field.name == *exprAccess.prop) {
          originalObjMemberType = field.type;
          break;
        }
      }
    } else if (exprAccess.elem != std::nullopt) {
      auto cObj = this->_nodeExpr(objNodeExpr, objTypeInfo.realType, parent, c, true);
      auto cObjElem = this->_nodeExpr(*exprAccess.elem, this->ast->typeMap.get("int"), parent, c);
      auto fnName = objTypeInfo.realType->isArray() ? objTypeInfo.realTypeName + "_at" : "str_at";

      expr = CodegenASTExprCall::create(
        CodegenASTExprAccess::create(this->_(fnName)),
        {
          this->_genErrState(ASTChecker(parent).insideMain(), false),
          CodegenASTExprLiteral::create(line),
          CodegenASTExprLiteral::create(col),
          cObj,
          cObjElem
        }
      );
    }

    auto exprText = expr->str();

    auto objMemberType = this->state.typeCasts.contains(exprText)
      ? this->state.typeCasts[exprText]
      : originalObjMemberType;

    if (originalObjMemberType->isOpt() && !objMemberType->isOpt()) {
      expr = CodegenASTExprUnary::create("*", expr);
    }

    if (!nodeExpr.type->isRef() && targetType->isRef()) {
      expr = CodegenASTExprUnary::create("&", expr);
    } else if (nodeExpr.type->isRef() && !targetType->isRef()) {
      expr = CodegenASTExprUnary::create("*", expr);
    }

    auto isEnumField = objTypeInfo.realType->isEnum() && exprAccess.prop != std::nullopt;
    auto isRef = nodeExpr.type->isRef() && targetType->isRef();

    if (!root && !isEnumField && !isRef && !(fieldTypeHasCallInfo && !fieldType->isRef())) {
      expr = this->_genCopyFn(Type::real(nodeExpr.type), expr);
    }

    if (
      root &&
      nodeExpr.type->shouldBeFreed() &&
      (exprAccess.prop == std::nullopt || (fieldType != nullptr && fieldType->shouldBeFreed()))
    ) {
      expr = this->_genFreeFn(nodeExpr.type, expr);
    }
  } else if (exprAccess.expr == std::nullopt && exprAccess.prop != std::nullopt) {
    auto memberName = *exprAccess.prop;
    auto enumType = std::get<TypeEnum>(nodeExpr.type->body);
    auto members = enumType.members;

    auto member = std::find_if(members.begin(), members.end(), [&] (const auto &it) -> bool {
      return it->name == memberName;
    });

    if (member != members.end()) {
      expr = CodegenASTExprAccess::create(Codegen::name((*member)->codeName));
    }
  }

  return this->_wrapNodeExpr(nodeExpr, targetType, root, expr);
}
