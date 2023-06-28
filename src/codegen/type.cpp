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

std::tuple<std::map<std::string, Type *>, std::map<std::string, Type *>> Codegen::_evalTypeCasts (const ASTNodeExpr &nodeExpr, const ASTNode &parent) {
  auto cDecl = CodegenASTStmtCompound::create();
  auto initialTypeCasts = this->state.typeCasts;
  auto bodyTypeCasts = std::map<std::string, Type *>{};
  auto altTypeCasts = std::map<std::string, Type *>{};

  if (std::holds_alternative<ASTExprBinary>(*nodeExpr.body)) {
    auto exprBinary = std::get<ASTExprBinary>(*nodeExpr.body);

    if (exprBinary.op == AST_EXPR_BINARY_AND || exprBinary.op == AST_EXPR_BINARY_OR) {
      auto [leftBodyTypeCasts, leftAltTypeCasts] = this->_evalTypeCasts(exprBinary.left, parent);

      if (exprBinary.op == AST_EXPR_BINARY_AND) {
        auto copyLeftBodyTypeCasts = leftBodyTypeCasts;
        copyLeftBodyTypeCasts.merge(this->state.typeCasts);
        copyLeftBodyTypeCasts.swap(this->state.typeCasts);
      }

      auto [rightBodyTypeCasts, rightAltTypeCasts] = this->_evalTypeCasts(exprBinary.right, parent);

      if (exprBinary.op == AST_EXPR_BINARY_AND) {
        bodyTypeCasts.merge(rightBodyTypeCasts);
        bodyTypeCasts.merge(leftBodyTypeCasts);
        altTypeCasts.merge(rightAltTypeCasts);
        altTypeCasts.merge(leftAltTypeCasts);
      } else {
        for (const auto &[name, value] : rightBodyTypeCasts) {
          if (leftBodyTypeCasts.contains(name)) {
            bodyTypeCasts[name] = value;
          }
        }

        for (const auto &[name, value] : rightAltTypeCasts) {
          if (leftAltTypeCasts.contains(name)) {
            altTypeCasts[name] = value;
          }
        }
      }
    } else if (exprBinary.op == AST_EXPR_BINARY_EQ || exprBinary.op == AST_EXPR_BINARY_NE) {
      if (
        (std::holds_alternative<ASTExprAccess>(*exprBinary.left.body) && std::holds_alternative<ASTExprLit>(*exprBinary.right.body)) ||
        (std::holds_alternative<ASTExprAccess>(*exprBinary.right.body) && std::holds_alternative<ASTExprLit>(*exprBinary.left.body))
      ) {
        auto exprBinaryLeft = std::holds_alternative<ASTExprAccess>(*exprBinary.left.body) ? exprBinary.left : exprBinary.right;
        auto exprBinaryRight = std::holds_alternative<ASTExprLit>(*exprBinary.right.body) ? exprBinary.right : exprBinary.left;
        auto exprBinaryRightLit = std::get<ASTExprLit>(*exprBinaryRight.body);

        if (exprBinaryLeft.type->isOpt() && exprBinaryRightLit.type == AST_EXPR_LIT_NIL) {
          auto exprBinaryLeftAccessCode = this->_nodeExpr(exprBinaryLeft, exprBinaryLeft.type, parent, cDecl, true).str();
          auto exprBinaryLeftAccessTypeOpt = std::get<TypeOptional>(exprBinaryLeft.type->body);

          if (exprBinary.op == AST_EXPR_BINARY_EQ) {
            altTypeCasts[exprBinaryLeftAccessCode] = exprBinaryLeftAccessTypeOpt.type;
          } else {
            bodyTypeCasts[exprBinaryLeftAccessCode] = exprBinaryLeftAccessTypeOpt.type;
          }
        }
      } else if (
        (std::holds_alternative<ASTExprAssign>(*exprBinary.left.body) && std::holds_alternative<ASTExprLit>(*exprBinary.right.body)) ||
        (std::holds_alternative<ASTExprAssign>(*exprBinary.right.body) && std::holds_alternative<ASTExprLit>(*exprBinary.left.body))
      ) {
        auto exprBinaryLeft = std::holds_alternative<ASTExprAssign>(*exprBinary.left.body) ? exprBinary.left : exprBinary.right;
        auto exprBinaryLeftAssign = std::get<ASTExprAssign>(*exprBinaryLeft.body);
        auto exprBinaryRight = std::holds_alternative<ASTExprLit>(*exprBinary.right.body) ? exprBinary.right : exprBinary.left;
        auto exprBinaryRightLit = std::get<ASTExprLit>(*exprBinaryRight.body);

        if (exprBinaryLeft.type->isOpt() && exprBinaryRightLit.type == AST_EXPR_LIT_NIL) {
          auto exprBinaryLeftAccessCode = this->_nodeExpr(exprBinaryLeftAssign.left, exprBinaryLeftAssign.left.type, parent, cDecl, true).str();
          auto exprBinaryLeftAccessTypeOpt = std::get<TypeOptional>(exprBinaryLeftAssign.left.type->body);

          if (exprBinary.op == AST_EXPR_BINARY_EQ) {
            altTypeCasts[exprBinaryLeftAccessCode] = exprBinaryLeftAccessTypeOpt.type;
          } else {
            bodyTypeCasts[exprBinaryLeftAccessCode] = exprBinaryLeftAccessTypeOpt.type;
          }
        }
      }
    }
  } else if (std::holds_alternative<ASTExprIs>(*nodeExpr.body)) {
    auto exprIs = std::get<ASTExprIs>(*nodeExpr.body);

    if (std::holds_alternative<ASTExprAccess>(*exprIs.expr.body)) {
      auto exprAccess = exprIs.expr;
      auto exprRealType = Type::real(exprAccess.type);

      if (exprRealType->isAny() || exprRealType->isOpt() || exprRealType->isUnion()) {
        auto exprBinaryLeftAccessCode = this->_nodeExpr(exprAccess, exprRealType, parent, cDecl, true).str();
        bodyTypeCasts[exprBinaryLeftAccessCode] = exprIs.type;

        if (exprRealType->isUnion()) {
          altTypeCasts[exprBinaryLeftAccessCode] = this->ast->typeMap.unionSub(exprRealType, exprIs.type);
        }
      }
    } else if (
      std::holds_alternative<ASTExprAssign>(*exprIs.expr.body) &&
      std::holds_alternative<ASTExprAccess>(*std::get<ASTExprAssign>(*exprIs.expr.body).left.body)
    ) {
      auto exprAccess = std::get<ASTExprAssign>(*exprIs.expr.body).left;
      auto exprRealType = Type::real(exprAccess.type);

      if (exprRealType->isAny() || exprRealType->isOpt() || exprRealType->isUnion()) {
        auto exprBinaryLeftAccessCode = this->_nodeExpr(exprAccess, exprRealType, parent, cDecl, true).str();
        bodyTypeCasts[exprBinaryLeftAccessCode] = exprIs.type;

        if (exprRealType->isUnion()) {
          altTypeCasts[exprBinaryLeftAccessCode] = this->ast->typeMap.unionSub(exprRealType, exprIs.type);
        }
      }
    }
  }

  this->state.typeCasts = initialTypeCasts;
  return std::make_tuple(bodyTypeCasts, altTypeCasts);
}

std::string Codegen::_type (Type *type) {
  if (type->isAny()) {
    this->_activateBuiltin("typeAny");
    return "struct any ";
  } else if (type->isArray()) {
    auto typeName = this->_typeNameArray(type);
    this->_activateEntity(typeName);
    return "struct " + typeName + " ";
  } else if (type->isByte()) {
    return "unsigned char ";
  } else if (type->isChar()) {
    return "char ";
  } else if (type->isEnum()) {
    auto typeName = Codegen::typeName(type->codeName);
    this->_activateEntity(typeName);
    return "enum " + typeName + " ";
  } else if (type->isF32()) {
    return "float ";
  } else if (type->isF64() || type->isFloat()) {
    return "double ";
  } else if (type->isBool()) {
    this->_activateBuiltin("libStdbool");
    return "bool ";
  } else if (type->isFn()) {
    if (type->builtin) {
      return type->name;
    }

    auto typeName = this->_typeNameFn(type);
    return this->_apiEval("struct _{" + typeName + "} ", 1);
  } else if (type->isI8()) {
    this->_activateBuiltin("libStdint");
    return "int8_t ";
  } else if (type->isI16()) {
    this->_activateBuiltin("libStdint");
    return "int16_t ";
  } else if (type->isI32() || type->isInt()) {
    this->_activateBuiltin("libStdint");
    return "int32_t ";
  } else if (type->isI64()) {
    this->_activateBuiltin("libStdint");
    return "int64_t ";
  } else if (type->isMap()) {
    auto typeName = this->_typeNameMap(type);
    this->_activateEntity(typeName);
    return "struct " + typeName + " ";
  } else if (type->isObj() && type->builtin && type->codeName == "@buffer_Buffer") {
    this->_activateBuiltin("typeBuffer");
    return "struct buffer ";
  } else if (type->isObj()) {
    auto typeName = type->builtin ? type->name : Codegen::typeName(type->codeName);
    this->_activateEntity(typeName);
    return "struct " + typeName + " *";
  } else if (type->isOpt()) {
    this->_typeNameOpt(type);
    auto optType = std::get<TypeOptional>(type->body);
    return this->_type(optType.type) + "*";
  } else if (type->isRef()) {
    auto refType = std::get<TypeRef>(type->body);
    return this->_type(refType.refType) + "*";
  } else if (type->isStr()) {
    this->_activateBuiltin("typeStr");
    return "struct str ";
  } else if (type->isU8()) {
    this->_activateBuiltin("libStdint");
    return "uint8_t ";
  } else if (type->isU16()) {
    this->_activateBuiltin("libStdint");
    return "uint16_t ";
  } else if (type->isU32()) {
    this->_activateBuiltin("libStdint");
    return "uint32_t ";
  } else if (type->isU64()) {
    this->_activateBuiltin("libStdint");
    return "uint64_t ";
  } else if (type->isUnion()) {
    auto typeName = this->_typeNameUnion(type);
    this->_activateEntity(typeName);
    return "struct " + typeName + " ";
  } else if (type->isVoid()) {
    return "void ";
  }

  throw Error("tried generating unknown type");
}

std::string Codegen::_typeDef (Type *type) {
  this->_saveStateBuiltinsEntities();
  auto typeInfo = this->_typeInfo(type);
  this->_restoreStateBuiltinsEntities();

  auto typeName = "TYPE_" + std::string(typeInfo.typeName.starts_with("__THE_1_") ? typeInfo.typeName.substr(8) : typeInfo.typeName);

  for (const auto &entity : this->entities) {
    if (entity.name == typeName) {
      return typeName;
    }
  }

  this->_apiEntity(typeName, CODEGEN_ENTITY_DEF, [&] (auto &decl, [[maybe_unused]] auto &def) {
    decl += "#define " + typeName + " " + std::to_string(this->lastTypeIdx++);
    return 0;
  });

  return typeName;
}

std::string Codegen::_typeDefIdx (Type *type) {
  auto typeName = this->_typeDef(type);
  auto result = std::string();

  for (const auto &entity : this->entities) {
    if (entity.name == typeName) {
      result = entity.decl.substr(entity.decl.find_last_of(' ') + 1);
      break;
    }
  }

  return result;
}

CodegenTypeInfo Codegen::_typeInfo (Type *type) {
  auto typeInfoItem = this->_typeInfoItem(type);

  if (!type->isRef()) {
    return CodegenTypeInfo{
      typeInfoItem.type,
      typeInfoItem.typeName,
      typeInfoItem.typeCode,
      typeInfoItem.typeCodeConst,
      typeInfoItem.typeCodeTrimmed,
      typeInfoItem.typeCodeConstTrimmed,
      typeInfoItem.typeRefCode,
      typeInfoItem.typeRefCodeConst,
      typeInfoItem.type,
      typeInfoItem.typeName,
      typeInfoItem.typeCode,
      typeInfoItem.typeCodeConst,
      typeInfoItem.typeCodeTrimmed,
      typeInfoItem.typeCodeConstTrimmed,
      typeInfoItem.typeRefCode,
      typeInfoItem.typeRefCodeConst
    };
  }

  auto realType = Type::real(type);
  auto realTypeInfoItem = this->_typeInfoItem(realType);

  return CodegenTypeInfo{
    typeInfoItem.type,
    typeInfoItem.typeName,
    typeInfoItem.typeCode,
    typeInfoItem.typeCodeConst,
    typeInfoItem.typeCodeTrimmed,
    typeInfoItem.typeCodeConstTrimmed,
    typeInfoItem.typeRefCode,
    typeInfoItem.typeRefCodeConst,
    realTypeInfoItem.type,
    realTypeInfoItem.typeName,
    realTypeInfoItem.typeCode,
    realTypeInfoItem.typeCodeConst,
    realTypeInfoItem.typeCodeTrimmed,
    realTypeInfoItem.typeCodeConstTrimmed,
    realTypeInfoItem.typeRefCode,
    realTypeInfoItem.typeRefCodeConst
  };
}

CodegenTypeInfoItem Codegen::_typeInfoItem (Type *type) {
  if (type->isAlias() && !type->builtin) {
    return this->_typeInfoItem(std::get<TypeAlias>(type->body).type);
  }

  auto typeCode = this->_type(type);
  auto typeCodeTrimmed = typeCode.substr(0, typeCode.find_last_not_of(' ') + 1);
  auto typeName = std::string();

  if (type->isArray() && !type->builtin) {
    typeName = this->_typeNameArray(type);
  } else if ((type->isEnum() || type->isObj()) && !type->builtin) {
    typeName = Codegen::typeName(type->codeName);
  } else if (type->isFn() && !type->builtin) {
    typeName = this->_typeNameFn(type);
  } else if (type->isMap() && !type->builtin) {
    typeName = this->_typeNameMap(type);
  } else if (type->isOpt() && !type->builtin) {
    typeName = this->_typeNameOpt(type);
  } else if (type->isUnion() && !type->builtin) {
    typeName = this->_typeNameUnion(type);
  } else {
    typeName = type->name;
  }

  return CodegenTypeInfoItem{
    type,
    typeName,
    typeCode,
    "const " + typeCode,
    typeCodeTrimmed,
    "const " + typeCodeTrimmed,
    typeCode + "*",
    "const " + typeCode + "*"
  };
}
