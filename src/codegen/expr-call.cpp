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

std::string Codegen::_exprCallDefaultArg (const CodegenTypeInfo &typeInfo) {
  if (typeInfo.type->isAny()) {
    return this->_apiEval("(_{struct any}) {}");
  } else if (
    typeInfo.type->isArray() ||
    typeInfo.type->isMap() ||
    typeInfo.type->isFn() ||
    typeInfo.type->isUnion()
  ) {
    this->_activateEntity(typeInfo.typeName);
    return "(struct " + typeInfo.typeName + ") {}";
  } else if (typeInfo.type->isBool()) {
    return this->_apiEval("_{false}");
  } else if (typeInfo.type->isChar()) {
    return R"('\0')";
  } else if (typeInfo.type->isObj() && typeInfo.type->builtin && typeInfo.type->codeName == "@buffer_Buffer") {
    return this->_apiEval("(_{struct buffer}) {}");
  } else if (typeInfo.type->isObj() || typeInfo.type->isOpt()) {
    return this->_apiEval("_{NULL}");
  } else if (typeInfo.type->isStr()) {
    return this->_apiEval("(_{struct str}) {}");
  } else {
    return "0";
  }
}

std::string Codegen::_exprCallPrintArg (const CodegenTypeInfo &typeInfo, const ASTNodeExpr &nodeExpr, const ASTNode &parent, std::string &decl) {
  if (typeInfo.type->isStr() && nodeExpr.isLit()) {
    return nodeExpr.litBody();
  } else if (
    typeInfo.type->isAny() ||
    typeInfo.type->isArray() ||
    typeInfo.type->isEnum() ||
    typeInfo.type->isFn() ||
    typeInfo.type->isMap() ||
    typeInfo.type->isObj() ||
    typeInfo.type->isOpt() ||
    typeInfo.type->isStr() ||
    typeInfo.type->isUnion()
  ) {
    return this->_genStrFn(typeInfo.type, this->_nodeExpr(nodeExpr, typeInfo.type, parent, decl), false, false);
  }

  return this->_nodeExpr(nodeExpr, typeInfo.type, parent, decl);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
std::string Codegen::_exprCallPrintArgSign (const CodegenTypeInfo &typeInfo, const ASTNodeExpr &nodeExpr) {
  if (
    typeInfo.type->isAny() ||
    typeInfo.type->isArray() ||
    typeInfo.type->isEnum() ||
    typeInfo.type->isFn() ||
    typeInfo.type->isMap() ||
    typeInfo.type->isObj() ||
    typeInfo.type->isOpt() ||
    typeInfo.type->isUnion()
  ) {
    return "s";
  } else if (typeInfo.type->isRef()) {
    return "p";
  } else if (typeInfo.type->isStr() && nodeExpr.isLit()) {
    return "z";
  } else {
    if (typeInfo.type->isBool()) return "t";
    else if (typeInfo.type->isByte()) return "b";
    else if (typeInfo.type->isChar()) return "c";
    else if (typeInfo.type->isF32()) return "e";
    else if (typeInfo.type->isF64()) return "g";
    else if (typeInfo.type->isFloat()) return "f";
    else if (typeInfo.type->isI8()) return "h";
    else if (typeInfo.type->isI16()) return "j";
    else if (typeInfo.type->isI32()) return "k";
    else if (typeInfo.type->isI64()) return "l";
    else if (typeInfo.type->isInt()) return "i";
    else if (typeInfo.type->isStr()) return "s";
    else if (typeInfo.type->isU8()) return "v";
    else if (typeInfo.type->isU16()) return "w";
    else if (typeInfo.type->isU32()) return "u";
    else if (typeInfo.type->isU64()) return "y";
  }

  throw Error("tried print unknown entity type");
}

std::string Codegen::_exprCall (
  const ASTNodeExpr &nodeExpr,
  Type *targetType,
  const ASTNode &parent,
  std::string &decl,
  bool root,
  std::size_t awaitCallId
) {
  auto line = std::to_string(nodeExpr.start.line);
  auto col = std::to_string(nodeExpr.start.col + 1);
  auto exprCall = std::get<ASTExprCall>(*nodeExpr.body);
  auto calleeTypeInfo = this->_typeInfo(exprCall.callee.type);
  auto fnType = std::get<TypeFn>(calleeTypeInfo.realType->body);
  auto code = std::string();

  if (calleeTypeInfo.realType->builtin && calleeTypeInfo.realType->codeName == "@print") {
    auto separator = std::string(R"(" ")");
    auto isSeparatorLit = true;
    auto terminator = this->_apiEval("_{THE_EOL}");
    auto isTerminatorLit = true;
    auto to = this->_apiEval("_{stdout}");

    for (const auto &exprCallArg : exprCall.args) {
      if (exprCallArg.id != std::nullopt && exprCallArg.id == "separator") {
        if (exprCallArg.expr.isLit()) {
          separator = exprCallArg.expr.litBody();
        } else {
          separator = this->_nodeExpr(exprCallArg.expr, this->ast->typeMap.get("str"), parent, decl);
          isSeparatorLit = false;
        }
      } else if (exprCallArg.id != std::nullopt && exprCallArg.id == "terminator") {
        if (exprCallArg.expr.isLit()) {
          terminator = exprCallArg.expr.litBody();
        } else {
          terminator = this->_nodeExpr(exprCallArg.expr, this->ast->typeMap.get("str"), parent, decl);
          isTerminatorLit = false;
        }
      } else if (exprCallArg.id != std::nullopt && exprCallArg.id == "to") {
        if (exprCallArg.expr.isLit()) {
          to = this->_apiEval(exprCallArg.expr.litBody() == R"("stderr")" ? "_{stderr}" : "_{stdout}");
        } else {
          to = this->_apiEval(R"(_{cstr_eq_str}("stderr")");
          to += ", " + this->_nodeExpr(exprCallArg.expr, this->ast->typeMap.get("str"), parent, decl) + ") ? ";
          to += this->_apiEval("_{stderr} : _{stdout}");
        }
      }
    }

    code = "_{print}(" + to + R"(, ")";

    auto argsCode = std::string();
    auto argIdx = static_cast<std::size_t>(0);

    for (const auto &exprCallArg : exprCall.args) {
      if (exprCallArg.id != "items") {
        continue;
      }

      auto argTypeInfo = this->_typeInfo(exprCallArg.expr.type);

      if (argIdx != 0 && separator != R"("")") {
        code += isSeparatorLit ? "z" : "s";
        argsCode += separator + ", ";
      }

      code += this->_exprCallPrintArgSign(argTypeInfo, exprCallArg.expr);
      argsCode += this->_exprCallPrintArg(argTypeInfo, exprCallArg.expr, parent, decl) + ", ";
      argIdx++;
    }

    if (terminator == R"("")") {
      code += R"(", )" + argsCode.substr(0, argsCode.size() - 2);
    } else {
      code += std::string(isTerminatorLit ? "z" : "s") + R"(", )" + argsCode + terminator;
    }

    code += ")";
    code = this->_apiEval(code, 1);
  } else if (calleeTypeInfo.realType->builtin && calleeTypeInfo.realType->codeName == "@utils_swap") {
    auto argTypeInfo = this->_typeInfo(exprCall.args[0].expr.type);
    auto argRealTypeInfo = argTypeInfo;

    if (argTypeInfo.type->isRef()) {
      argRealTypeInfo = this->_typeInfo(std::get<TypeRef>(argTypeInfo.type->body).refType);
    }

    code = this->_apiEval(
      "_{utils_swap}(" + this->_nodeExpr(exprCall.args[0].expr, argTypeInfo.type, parent, decl) +
      ", " + this->_nodeExpr(exprCall.args[1].expr, argTypeInfo.type, parent, decl) + ", sizeof(" + argRealTypeInfo.typeCodeTrimmed + "))",
      1
    );
  } else {
    auto hasSelfParam = fnType.isMethod && fnType.callInfo.isSelfFirst;
    auto hasThrowParams = this->throws && !calleeTypeInfo.realType->builtin;
    auto hasParams = (!fnType.params.empty() || hasSelfParam || hasThrowParams) && !calleeTypeInfo.realType->builtin;
    auto paramsName = calleeTypeInfo.realTypeName + "P";
    auto paramsCode = std::string();

    if (hasParams) {
      paramsCode += this->_apiEval(", _{xalloc}(&(struct _{" + paramsName + "}) {");
    }

    if (hasThrowParams) {
      paramsCode += line + ", " + col;
    }

    if (hasSelfParam) {
      auto exprAccess = std::get<ASTExprAccess>(*exprCall.callee.body);
      auto nodeExprAccess = std::get<ASTNodeExpr>(*exprAccess.expr);

      paramsCode += !hasThrowParams ? "" : ", ";

      if (calleeTypeInfo.realType->builtin) {
        paramsCode += this->_nodeExpr(nodeExprAccess, fnType.callInfo.selfType, parent, decl, fnType.callInfo.isSelfMut);
      } else {
        paramsCode += this->_genCopyFn(fnType.callInfo.selfType, this->_nodeExpr(nodeExprAccess, fnType.callInfo.selfType, parent, decl, true));
      }
    }

    for (auto i = static_cast<std::size_t>(0); i < fnType.params.size(); i++) {
      auto param = fnType.params[i];
      auto paramTypeInfo = this->_typeInfo(param.type);
      auto foundArg = std::optional<ASTExprCallArg>{};
      auto foundArgIdx = static_cast<std::size_t>(0);

      if (param.name != std::nullopt) {
        for (auto j = static_cast<std::size_t>(0); j < exprCall.args.size(); j++) {
          if (exprCall.args[j].id == param.name) {
            foundArgIdx = j;
            foundArg = exprCall.args[foundArgIdx];
            break;
          }
        }
      } else if (i < exprCall.args.size()) {
        foundArgIdx = i;
        foundArg = exprCall.args[foundArgIdx];
      }

      if (!param.required && !param.variadic) {
        paramsCode += std::string(i == 0 && !hasThrowParams && !hasSelfParam ? "" : ", ") + (foundArg == std::nullopt ? "0" : "1");
      }

      paramsCode += i == 0 && (param.required || param.variadic) && !hasThrowParams && !hasSelfParam ? "" : ", ";

      if (param.variadic) {
        auto paramTypeElementType = std::get<TypeArray>(param.type->body).elementType;
        auto variadicArgs = std::vector<ASTExprCallArg>{};

        if (foundArg != std::nullopt) {
          variadicArgs.push_back(*foundArg);

          for (auto j = foundArgIdx + 1; j < exprCall.args.size(); j++) {
            auto exprCallArg = exprCall.args[j];

            if (exprCallArg.id != std::nullopt && *exprCallArg.id != param.name) {
              break;
            }

            variadicArgs.push_back(exprCallArg);
          }
        }

        paramsCode += this->_apiEval("_{" + paramTypeInfo.typeName + "_alloc}(") + std::to_string(variadicArgs.size());

        for (const auto &variadicArg : variadicArgs) {
          paramsCode += ", " + this->_nodeExpr(variadicArg.expr, paramTypeElementType, parent, decl);
        }

        paramsCode += ")";
      } else if (foundArg != std::nullopt) {
        paramsCode += this->_nodeExpr(foundArg->expr, paramTypeInfo.type, parent, decl);
      } else {
        paramsCode += this->_exprCallDefaultArg(paramTypeInfo);
      }
    }

    if (hasParams) {
      paramsCode += this->_apiEval("}, sizeof(struct _{" + paramsName + "}))");
    }

    auto fnName = std::string();

    if (calleeTypeInfo.realType->builtin && !fnType.callInfo.empty()) {
      fnName = this->_apiEval("_{" + fnType.callInfo.codeName + "}");
    } else if (fnType.callInfo.empty()) {
      fnName = this->_nodeExpr(exprCall.callee, calleeTypeInfo.realType, parent, decl, true);
    } else {
      fnName = Codegen::name(fnType.callInfo.codeName);
    }

    if (
      fnType.isMethod &&
      this->state.contextVars.contains(fnName) &&
      (nodeExpr.type->isRef() || !targetType->isRef())
    ) {
      fnName = "*" + fnName;
    }

    if (fnName.starts_with("*")) {
      fnName = "(" + fnName + ")";
    }

    if (fnType.async) {
      // todo if not awaited then no parent job
      // todo if not awaited, return value incorrect
      auto parentIsSyncMain = ASTChecker(parent).parentIs<ASTNodeMain>() && !std::get<ASTNodeMain>(*parent.parent->body).async;

      paramsCode = paramsCode.empty() ? this->_apiEval(", _{NULL}") : paramsCode;
      code = this->_apiEval("_{threadpool_add}(tp, " + fnName + ".f");
      code += this->_apiEval(", _{xalloc}(" + fnName + ".x, " + fnName + ".l)") + paramsCode;
      code += ", " + (fnType.returnType->isVoid() ? this->_apiEval("_{NULL}") : "t" + std::to_string(awaitCallId)) + ", ";
      code += this->_apiEval(parentIsSyncMain ? "_{NULL}" : "_{threadpool_job_ref}(job)");
      code += ")";
    } else {
      code = fnName;
      code += (!calleeTypeInfo.realType->builtin || fnType.callInfo.empty()) ? ".f(" + code + ".x" : "(";
      code += paramsCode;

      if (this->throws && calleeTypeInfo.realType->builtin && fnType.callInfo.throws) {
        code += paramsCode.empty() ? "" : ", ";
        code += line + ", " + col;
      }

      code += ")";
    }
  }

  if (!root && nodeExpr.type->isRef() && !targetType->isRef()) {
    code = this->_genCopyFn(targetType, "*" + code);
  }

  code = !root ? code : this->_genFreeFn(nodeExpr.type, code);
  return this->_wrapNodeExpr(nodeExpr, targetType, root, code);
}
