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

std::string Codegen::_exprAccess (const ASTNodeExpr &nodeExpr, Type *targetType, const ASTNode &parent, std::string &decl, bool root) {
  auto line = std::to_string(nodeExpr.start.line);
  auto col = std::to_string(nodeExpr.start.col + 1);
  auto exprAccess = std::get<ASTExprAccess>(*nodeExpr.body);
  auto code = std::string();

  if (exprAccess.expr != std::nullopt && std::holds_alternative<std::shared_ptr<Var>>(*exprAccess.expr)) {
    auto objVar = std::get<std::shared_ptr<Var>>(*exprAccess.expr);

    if (objVar->builtin && (objVar->codeName == "@math_MaxFloat" || objVar->codeName == "@math_MaxF64")) {
      code = this->_apiEval("_{DBL_MAX}");
    } else if (objVar->builtin && (objVar->codeName == "@math_MinFloat" || objVar->codeName == "@math_MinF64")) {
      code = this->_apiEval("-_{DBL_MAX}");
    } else if (objVar->builtin && objVar->codeName == "@math_MaxF32") {
      code = this->_apiEval("_{FLT_MAX}");
    } else if (objVar->builtin && objVar->codeName == "@math_MinF32") {
      code = this->_apiEval("-_{FLT_MAX}");
    } else if (objVar->builtin && (objVar->codeName == "@math_MaxInt" || objVar->codeName == "@math_MaxI32")) {
      code = this->_apiEval("_{INT32_MAX}");
    } else if (objVar->builtin && (objVar->codeName == "@math_MinInt" || objVar->codeName == "@math_MinI32")) {
      code = this->_apiEval("_{INT32_MIN}");
    } else if (objVar->builtin && objVar->codeName == "@math_MaxI8") {
      code = this->_apiEval("_{INT8_MAX}");
    } else if (objVar->builtin && objVar->codeName == "@math_MinI8") {
      code = this->_apiEval("_{INT8_MIN}");
    } else if (objVar->builtin && objVar->codeName == "@math_MaxI16") {
      code = this->_apiEval("_{INT16_MAX}");
    } else if (objVar->builtin && objVar->codeName == "@math_MinI16") {
      code = this->_apiEval("_{INT16_MIN}");
    } else if (objVar->builtin && objVar->codeName == "@math_MaxI64") {
      code = this->_apiEval("_{INT64_MAX}");
    } else if (objVar->builtin && objVar->codeName == "@math_MinI64") {
      code = this->_apiEval("_{INT64_MIN}");
    } else if (objVar->builtin && objVar->codeName == "@math_MaxU8") {
      code = this->_apiEval("_{UINT8_MAX}");
    } else if (objVar->builtin && objVar->codeName == "@math_MaxU16") {
      code = this->_apiEval("_{UINT16_MAX}");
    } else if (objVar->builtin && objVar->codeName == "@math_MaxU32") {
      code = this->_apiEval("_{UINT32_MAX}");
    } else if (objVar->builtin && objVar->codeName == "@math_MaxU64") {
      code = this->_apiEval("_{UINT64_MAX}");
    } else if (objVar->builtin && (
      objVar->codeName == "@math_MinU8" ||
      objVar->codeName == "@math_MinU16" ||
      objVar->codeName == "@math_MinU32" ||
      objVar->codeName == "@math_MinU64"
    )) {
      code = "0";
    } else if (objVar->builtin && objVar->codeName == "@os_EOL") {
      code = this->_apiEval("_{str_alloc}(_{THE_EOL})");
      code = !root ? code : this->_genFreeFn(objVar->type, code);
    } else if (objVar->builtin && objVar->codeName == "@os_NAME") {
      code = this->_apiEval("_{os_name}(" + line + ", " + col + ")");
      code = !root ? code : this->_genFreeFn(objVar->type, code);
    } else if (objVar->builtin && objVar->codeName == "@path_SEP") {
      code = this->_apiEval("_{str_alloc}(_{THE_PATH_SEP})");
      code = !root ? code : this->_genFreeFn(objVar->type, code);
    } else if (objVar->builtin && objVar->codeName == "@process_args") {
      this->needMainArgs = true;
      code = this->_apiEval("_{process_args}()");
      code = !root ? code : this->_genFreeFn(objVar->type, code);
    } else if (objVar->builtin && objVar->codeName == "@process_env") {
      code = this->_apiEval("_{process_env}()");
      code = !root ? code : this->_genFreeFn(objVar->type, code);
    } else if (objVar->builtin && objVar->codeName == "@process_home") {
      code = this->_apiEval("_{process_home}(" + line + ", " + col + ")");
      code = !root ? code : this->_genFreeFn(objVar->type, code);
    } else {
      auto objCode = Codegen::name(objVar->codeName);
      auto objType = this->state.typeCasts.contains(objCode) ? this->state.typeCasts[objCode] : objVar->type;

      code = objCode;

      if (objVar->type->isAny() && !objType->isAny()) {
        auto typeName = this->_typeNameAny(objType);
        this->_activateEntity(typeName);
        code = "((struct " + typeName + " *) " + code + ".d)->d";
      } else if (objVar->type->isOpt() && !objType->isOpt()) {
        code = "*" + code;
      } else if (objVar->type->isUnion() && (!objType->isUnion() || objVar->type->hasSubType(objType))) {
        code = code + ".v" + this->_typeDefIdx(objType);
      }

      if (this->state.contextVars.contains(objCode) && (nodeExpr.type->isRef() || !targetType->isRef())) {
        code = "*" + code;
      }

      if (
        !this->state.contextVars.contains(objCode) &&
        ((!nodeExpr.type->isRef() && targetType->isRef()) || targetType->isRefOf(nodeExpr.type))
      ) {
        code = "&" + code;
      } else if (nodeExpr.type->isRefOf(targetType)) {
        auto refNodeExprType = Type::actual(nodeExpr.type);

        while (refNodeExprType->isRefOf(targetType)) {
          code = "*" + code;
          refNodeExprType = Type::actual(std::get<TypeRef>(refNodeExprType->body).refType);
        }
      }

      auto nodeExprTypeRefAny = targetType->isAny() && !nodeExpr.type->isRefOf(targetType);
      auto shouldCopyVar = !objType->isRef() || (!targetType->isRef() && !targetType->hasSubType(objType) && !nodeExprTypeRefAny);

      if (!root && shouldCopyVar) {
        code = this->_genCopyFn(Type::real(objType), code);
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

      if (typeField.callInfo.isSelfFirst) {
        code = this->_nodeExpr(objNodeExpr, typeField.callInfo.selfType, parent, decl, typeField.callInfo.isSelfMut);
      }

      if (this->throws && typeField.callInfo.throws) {
        code += ", " + line + ", " + col;
      }

      code = this->_apiEval("_{" + typeField.callInfo.codeName + "}(" + code + ")", 1);
      fieldTypeHasCallInfo = true;
      fieldType = typeField.type;
    } else if (exprAccess.prop != std::nullopt && objTypeInfo.realType->isEnum()) {
      auto enumType = std::get<TypeEnum>(objTypeInfo.realType->body);

      for (const auto &member : enumType.members) {
        if (member->name == *exprAccess.prop) {
          code = Codegen::name(member->codeName);
          break;
        }
      }
    } else if (exprAccess.prop != std::nullopt) {
      auto objCode = this->_nodeExpr(objNodeExpr, objTypeInfo.realType, parent, decl, true);

      if (objCode.starts_with("*")) {
        objCode = "(" + objCode + ")";
      }

      code = objCode + "->" + Codegen::name(*exprAccess.prop);

      for (const auto &field : objTypeInfo.realType->fields) {
        if (field.name == *exprAccess.prop) {
          originalObjMemberType = field.type;
          break;
        }
      }
    } else if (exprAccess.elem != std::nullopt) {
      auto objCode = this->_nodeExpr(objNodeExpr, objTypeInfo.realType, parent, decl, true);
      auto objElemCode = this->_nodeExpr(*exprAccess.elem, this->ast->typeMap.get("int"), parent, decl);

      if (objTypeInfo.realType->isArray()) {
        code = this->_apiEval("_{" + objTypeInfo.realTypeName + "_at}", 1);
      } else if (objTypeInfo.realType->isStr()) {
        code = this->_apiEval("_{str_at}");
      }

      code += "(" + objCode + ", " + objElemCode + ", " + line + ", " + col + ")";
    }

    auto objMemberType = this->state.typeCasts.contains(code) ? this->state.typeCasts[code] : originalObjMemberType;

    if (originalObjMemberType->isOpt() && !objMemberType->isOpt()) {
      code = "*" + code;
    }

    if (!nodeExpr.type->isRef() && targetType->isRef()) {
      code = "&" + code;
    } else if (nodeExpr.type->isRef() && !targetType->isRef()) {
      code = "*" + code;
    }

    auto isExprAccessEnumField = objTypeInfo.realType->isEnum() && exprAccess.prop != std::nullopt;
    auto isExprAccessRef = nodeExpr.type->isRef() && targetType->isRef();

    if (!root && !isExprAccessEnumField && !isExprAccessRef && !(fieldTypeHasCallInfo && !fieldType->isRef())) {
      code = this->_genCopyFn(Type::real(nodeExpr.type), code);
    }

    if (
      root &&
      nodeExpr.type->shouldBeFreed() &&
      (exprAccess.prop == std::nullopt || (fieldType != nullptr && fieldType->shouldBeFreed()))
    ) {
      code = this->_genFreeFn(nodeExpr.type, code);
    }
  } else if (exprAccess.expr == std::nullopt && exprAccess.prop != std::nullopt) {
    auto memberName = *exprAccess.prop;
    auto enumType = std::get<TypeEnum>(nodeExpr.type->body);

    auto member = std::find_if(
      enumType.members.begin(),
      enumType.members.end(),
      [&] (const auto &it) -> bool {
        return it->name == memberName;
      }
    );

    if (member != enumType.members.end()) {
      code = Codegen::name((*member)->codeName);
    }
  }

  return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
}
