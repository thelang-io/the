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

CodegenASTExpr Codegen::_exprCallDefaultArg (const CodegenTypeInfo &typeInfo) {
  if (typeInfo.type->isAny()) {
    return CodegenASTExprCast::create(
      CodegenASTType::create(this->_("struct any")),
      CodegenASTExprInitList::create()
    );
  } else if (
    typeInfo.type->isArray() ||
    typeInfo.type->isMap() ||
    typeInfo.type->isFn() ||
    typeInfo.type->isUnion()
  ) {
    return CodegenASTExprCast::create(
      CodegenASTType::create("struct " + this->_(typeInfo.typeName)),
      CodegenASTExprInitList::create()
    );
  } else if (typeInfo.type->isBool()) {
    return CodegenASTExprAccess::create(this->_("false"));
  } else if (typeInfo.type->isChar()) {
    return CodegenASTExprLiteral::create(R"('\0')");
  } else if (typeInfo.type->isObj() && typeInfo.type->builtin && typeInfo.type->codeName == "@buffer_Buffer") {
    return CodegenASTExprCast::create(
      CodegenASTType::create(this->_("struct buffer")),
      CodegenASTExprInitList::create()
    );
  } else if (typeInfo.type->isObj() || typeInfo.type->isOpt()) {
    return CodegenASTExprAccess::create(this->_("NULL"));
  } else if (typeInfo.type->isStr()) {
    return CodegenASTExprCast::create(
      CodegenASTType::create(this->_("struct str")),
      CodegenASTExprInitList::create()
    );
  } else {
    return CodegenASTExprLiteral::create("0");
  }
}

CodegenASTExpr Codegen::_exprCallPrintArg (const CodegenTypeInfo &typeInfo, const ASTNodeExpr &nodeExpr, const ASTNode &parent, CodegenASTStmt *c) {
  if (typeInfo.type->isStr() && nodeExpr.isLit()) {
    return CodegenASTExprLiteral::create(nodeExpr.litBody());
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
    return this->_genStrFn(typeInfo.type, this->_nodeExpr(nodeExpr, typeInfo.type, parent, c), false, false);
  }

  return this->_nodeExpr(nodeExpr, typeInfo.type, parent, c);
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

CodegenASTExpr Codegen::_exprCall (
  const ASTNodeExpr &nodeExpr,
  Type *targetType,
  const ASTNode &parent,
  CodegenASTStmt *c,
  bool root,
  std::size_t awaitCallId
) {
  auto line = std::to_string(nodeExpr.start.line);
  auto col = std::to_string(nodeExpr.start.col + 1);
  auto exprCall = std::get<ASTExprCall>(*nodeExpr.body);
  auto calleeTypeInfo = this->_typeInfo(exprCall.callee.type);
  auto fnType = std::get<TypeFn>(calleeTypeInfo.realType->body);
  auto expr = CodegenASTExpr{};

  if (calleeTypeInfo.realType->builtin && calleeTypeInfo.realType->codeName == "@print") {
    auto separator = CodegenASTExprLiteral::create(R"(" ")");
    auto terminator = CodegenASTExprAccess::create(this->_("THE_EOL"));
    auto to = CodegenASTExprAccess::create(this->_("stdout"));

    for (const auto &exprCallArg : exprCall.args) {
      if (exprCallArg.id != std::nullopt && exprCallArg.id == "separator") {
        separator = this->_nodeExpr(exprCallArg.expr, this->ast->typeMap.get("str"), parent, c);
      } else if (exprCallArg.id != std::nullopt && exprCallArg.id == "terminator") {
        terminator = this->_nodeExpr(exprCallArg.expr, this->ast->typeMap.get("str"), parent, c);
      } else if (exprCallArg.id != std::nullopt && exprCallArg.id == "to") {
        if (exprCallArg.expr.isLit()) {
          auto toLitBody = exprCallArg.expr.litBody();
          to = CodegenASTExprAccess::create(this->_(toLitBody == R"("stderr")" ? "stderr" : "stdout"));
        } else {
          auto cTo = this->_nodeExpr(exprCallArg.expr, this->ast->typeMap.get("str"), parent, c);

          to = CodegenASTExprCond::create(
            CodegenASTExprCall::create(
              CodegenASTExprAccess::create(this->_("cstr_eq_str")),
              {CodegenASTExprLiteral::create(this->_("stderr")), cTo}
            ),
            CodegenASTExprAccess::create(this->_("stderr")),
            CodegenASTExprAccess::create(this->_("stdout"))
          );
        }
      }
    }

    auto cArgs = std::vector<CodegenASTExpr>{to};
    auto fmtString = std::string("");
    auto argIdx = static_cast<std::size_t>(0);

    for (const auto &exprCallArg : exprCall.args) {
      if (exprCallArg.id != "items") {
        continue;
      }

      auto argTypeInfo = this->_typeInfo(exprCallArg.expr.type);
      auto cArg = this->_exprCallPrintArg(argTypeInfo, exprCallArg.expr, parent, c);

      if (argIdx != 0 && !separator.isEmptyString()) {
        fmtString += separator.isLiteral() ? "z" : "s";
        cArgs.push_back(separator);
      }

      fmtString += this->_exprCallPrintArgSign(argTypeInfo, exprCallArg.expr);
      cArgs.push_back(cArg);
      argIdx++;
    }

    if (!terminator.isEmptyString()) {
      fmtString += terminator.isLiteral() ? "z" : "s";
      cArgs.push_back(terminator);
    }

    cArgs.insert(cArgs.begin() + 1, CodegenASTExprLiteral::create(R"(")" + fmtString + R"(")"));
    expr = CodegenASTExprCall::create(CodegenASTExprAccess::create(this->_("print")), cArgs);
  } else if (calleeTypeInfo.realType->builtin && calleeTypeInfo.realType->codeName == "@utils_swap") {
    auto argTypeInfo = this->_typeInfo(exprCall.args[0].expr.type);
    auto argRealTypeInfo = argTypeInfo;

    if (argTypeInfo.type->isRef()) {
      argRealTypeInfo = this->_typeInfo(std::get<TypeRef>(argTypeInfo.type->body).refType);
    }

    expr = CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_("utils_swap")),
      {
        this->_nodeExpr(exprCall.args[0].expr, argTypeInfo.type, parent, c),
        this->_nodeExpr(exprCall.args[1].expr, argTypeInfo.type, parent, c),
        CodegenASTExprCall::create(
          CodegenASTExprAccess::create("sizeof"),
          {CodegenASTType::create(argRealTypeInfo.typeCodeTrimmed)}
        )
      }
    );
  } else {
    auto hasSelfParam = fnType.isMethod && fnType.callInfo.isSelfFirst;
    auto hasThrowParams = this->throws && !calleeTypeInfo.realType->builtin;
    auto hasParams = (!fnType.params.empty() || hasSelfParam || hasThrowParams) && !calleeTypeInfo.realType->builtin;
    auto paramsName = calleeTypeInfo.realTypeName + "P";
    auto cParamsArgs = std::vector<CodegenASTExpr>{};

    if (hasThrowParams) {
      cParamsArgs.push_back(CodegenASTExprLiteral::create(line));
      cParamsArgs.push_back(CodegenASTExprLiteral::create(col));
    }

    if (hasSelfParam) {
      auto exprAccess = std::get<ASTExprAccess>(*exprCall.callee.body);
      auto nodeExprAccess = std::get<ASTNodeExpr>(*exprAccess.expr);

      if (calleeTypeInfo.realType->builtin) {
        cParamsArgs.push_back(
          this->_nodeExpr(nodeExprAccess, fnType.callInfo.selfType, parent, c, fnType.callInfo.isSelfMut)
        );
      } else {
        cParamsArgs.push_back(
          this->_genCopyFn(
            fnType.callInfo.selfType,
            this->_nodeExpr(nodeExprAccess, fnType.callInfo.selfType, parent, c, true)
          )
        );
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
        cParamsArgs.push_back(CodegenASTExprLiteral::create(foundArg == std::nullopt ? "0" : "1"));
      }

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

        auto cVariadicArgs = std::vector<CodegenASTExpr>{
          CodegenASTExprLiteral::create(std::to_string(variadicArgs.size()))
        };

        for (const auto &variadicArg : variadicArgs) {
          cVariadicArgs.push_back(this->_nodeExpr(variadicArg.expr, paramTypeElementType, parent, c));
        }

        cParamsArgs.push_back(
          CodegenASTExprCall::create(
            CodegenASTExprAccess::create(this->_(paramTypeInfo.typeName + "_alloc")),
            cVariadicArgs
          )
        );
      } else if (foundArg != std::nullopt) {
        cParamsArgs.push_back(this->_nodeExpr(foundArg->expr, paramTypeInfo.type, parent, c));
      } else {
        cParamsArgs.push_back(this->_exprCallDefaultArg(paramTypeInfo));
      }
    }

    auto cParams = std::optional<CodegenASTExpr>{};

    if (hasParams) {
      cParams = CodegenASTExprCall::create(
        CodegenASTExprAccess::create(this->_("xalloc")),
        {
          CodegenASTExprUnary::create("&", CodegenASTExprCast::create(
            CodegenASTType::create("struct " + this->_(paramsName)),
            CodegenASTExprInitList::create(cParamsArgs)
          )),
          CodegenASTExprCall::create(
            CodegenASTExprAccess::create("sizeof"),
            {CodegenASTType::create("struct " + this->_(paramsName))}
          )
        }
      );
    }

    auto fnName = CodegenASTExprAccess::create(Codegen::name(fnType.callInfo.codeName));

    if (calleeTypeInfo.realType->builtin && !fnType.callInfo.empty()) {
      fnName = CodegenASTExprAccess::create(this->_(fnType.callInfo.codeName));
    } else if (fnType.callInfo.empty()) {
      fnName = this->_nodeExpr(exprCall.callee, calleeTypeInfo.realType, parent, c, true);
    }

    if (
      fnType.isMethod &&
      this->state.contextVars.contains(fnName.str()) &&
      (nodeExpr.type->isRef() || !targetType->isRef())
    ) {
      fnName = CodegenASTExprUnary::create("*", fnName);
    }

    if (fnName.isPointer()) {
      fnName = fnName.wrap();
    }

    if (fnType.async) {
      // todo if not awaited then no parent job
      // todo if not awaited, return value incorrect
      auto parentIsSyncMain = ASTChecker(parent.parent).is<ASTNodeMain>() && !std::get<ASTNodeMain>(*parent.parent->body).async;

      expr = CodegenASTExprCall::create(
        CodegenASTExprAccess::create(this->_("threadpool_add")),
        {
          CodegenASTExprAccess::create("tp"),
          CodegenASTExprAccess::create(fnName, "f"),
          CodegenASTExprCall::create(
            CodegenASTExprAccess::create(this->_("xalloc")),
            {CodegenASTExprAccess::create(fnName, "x"), CodegenASTExprAccess::create(fnName, "l")}
          ),
          cParams != std::nullopt ? *cParams : CodegenASTExprAccess::create(this->_("NULL")),
          CodegenASTExprAccess::create(
            root || fnType.returnType->isVoid()
              ? this->_("NULL")
              : "t" + std::to_string(awaitCallId)
          ),
          parentIsSyncMain
            ? CodegenASTExprAccess::create(this->_("NULL"))
            : CodegenASTExprCall::create(
                CodegenASTExprAccess::create(this->_("threadpool_job_ref")),
                {CodegenASTExprAccess::create("job")}
              )
        }
      );
    } else {
      auto isBuiltin = !calleeTypeInfo.realType->builtin || fnType.callInfo.empty();
      auto cArgs = std::vector<CodegenASTExpr>{};

      if (isBuiltin) {
        cArgs.push_back(CodegenASTExprAccess::create(fnName, "x"));
      }

      if (cParams != std::nullopt) {
        cArgs.push_back(*cParams);
      }

      if (this->throws && calleeTypeInfo.realType->builtin && fnType.callInfo.throws) {
        cArgs.push_back(CodegenASTExprLiteral::create(line));
        cArgs.push_back(CodegenASTExprLiteral::create(col));
      }

      expr = CodegenASTExprCall::create(
        isBuiltin ? CodegenASTExprAccess::create(fnName, "f") : fnName,
        cArgs
      );
    }
  }

  if (!root && nodeExpr.type->isRef() && !targetType->isRef()) {
    expr = this->_genCopyFn(targetType, CodegenASTExprUnary::create("*", expr));
  }

  if (root) {
    expr = this->_genFreeFn(nodeExpr.type, expr);
  }

  return this->_wrapNodeExpr(nodeExpr, targetType, root, expr);
}
