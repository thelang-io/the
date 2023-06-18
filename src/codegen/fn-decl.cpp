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
#include "../ASTChecker.hpp"
#include "../Codegen.hpp"
#include "../config.hpp"

std::string Codegen::_fnDecl (
  std::shared_ptr<Var> var,
  const std::vector<std::shared_ptr<Var>> &stack,
  const std::vector<ASTFnDeclParam> &params,
  const std::optional<ASTBlock> &body,
  const ASTNode &node,
  CodegenPhase phase
) {
  if (body == std::nullopt) {
    return "";
  }

  auto codeName = var->codeName;
  auto typeName = Codegen::typeName(codeName);
  auto varTypeInfo = this->_typeInfo(var->type);
  auto fnType = std::get<TypeFn>(var->type->body);
  auto paramsName = varTypeInfo.typeName + "P";
  auto contextName = typeName + "X";
  auto hasSelfParam = fnType.isMethod && fnType.callInfo.isSelfFirst;
  auto flattenAsyncBody = !fnType.async ? ASTBlock{} : ASTChecker::flattenNode(*body);
  auto flattenAwaitCalls = !fnType.async ? std::vector<ASTNodeExpr>{} : ASTChecker::flattenExpr(ASTChecker::flattenNodeExprs(*body));
  auto asyncBreakNodesCount = static_cast<std::size_t>(0);
  auto asyncContinueNodesCount = static_cast<std::size_t>(0);
  auto code = std::string();

  for (const auto &item : flattenAsyncBody) {
    if (std::holds_alternative<ASTNodeLoop>(*item.body)) {
      asyncBreakNodesCount += ASTChecker(item).has<ASTNodeBreak>() ? 1 : 0;
      asyncContinueNodesCount += ASTChecker(item).has<ASTNodeContinue>() ? 1 : 0;
    }
  }

  flattenAsyncBody.erase(std::remove_if(flattenAsyncBody.begin(), flattenAsyncBody.end(), [] (const auto &it) -> bool {
    return
      !std::holds_alternative<ASTNodeFnDecl>(*it.body) &&
      !(std::holds_alternative<ASTNodeObjDecl>(*it.body) && !std::get<ASTNodeObjDecl>(*it.body).methods.empty()) &&
      !std::holds_alternative<ASTNodeVarDecl>(*it.body);
  }), flattenAsyncBody.end());

  flattenAwaitCalls.erase(std::remove_if(flattenAwaitCalls.begin(), flattenAwaitCalls.end(), [] (const auto &it) -> bool {
    return !std::holds_alternative<ASTExprAwait>(*it.body) || it.type->isVoid();
  }), flattenAwaitCalls.end());

  if (phase == CODEGEN_PHASE_ALLOC || phase == CODEGEN_PHASE_FULL) {
    auto initialIndent = this->indent;
    auto initialStateCleanUp = this->state.cleanUp;
    auto initialStateContextVars = this->state.contextVars;
    auto initialStateInsideAsync = this->state.insideAsync;
    auto initialStateReturnType = this->state.returnType;
    auto initialStateAsyncCounter = this->state.asyncCounter;
    auto contextEntityIdx = 0;

    this->state.cleanUp = CodegenCleanUp(CODEGEN_CLEANUP_FN, &initialStateCleanUp);

    if (!stack.empty() || !flattenAsyncBody.empty()) {
      contextEntityIdx = this->_apiEntity(contextName, CODEGEN_ENTITY_OBJ, [&] (auto &decl, auto &def) {
        decl += "struct " + contextName + ";";
        def += "struct " + contextName + " {" EOL;

        for (const auto &contextVar : stack) {
          auto contextVarName = Codegen::name(contextVar->codeName);
          auto contextVarTypeInfo = this->_typeInfo(contextVar->type);
          auto typeRefCode = contextVar->mut ? contextVarTypeInfo.typeRefCode : contextVarTypeInfo.typeRefCodeConst;

          def += "  " + typeRefCode + contextVarName + ";" EOL;
        }

        for (const auto &asyncNode : flattenAsyncBody) {
          if (std::holds_alternative<ASTNodeFnDecl>(*asyncNode.body)) {
            auto nodeFnDecl = std::get<ASTNodeFnDecl>(*asyncNode.body);
            auto contextVarName = Codegen::name(nodeFnDecl.var->codeName);
            auto typeInfo = this->_typeInfo(nodeFnDecl.var->type);

            def += "  " + typeInfo.typeCode + contextVarName + ";" EOL;
          } else if (std::holds_alternative<ASTNodeObjDecl>(*asyncNode.body)) {
            auto nodeObjDecl = std::get<ASTNodeObjDecl>(*asyncNode.body);

            for (const auto &method : nodeObjDecl.methods) {
              auto contextVarName = Codegen::name(method.var->codeName);
              auto typeInfo = this->_typeInfo(method.var->type);

              def += "  " + typeInfo.typeCode + contextVarName + ";" EOL;
            }
          } else if (std::holds_alternative<ASTNodeVarDecl>(*asyncNode.body)) {
            auto nodeVarDecl = std::get<ASTNodeVarDecl>(*asyncNode.body);
            auto contextVarName = Codegen::name(nodeVarDecl.var->codeName);
            auto typeInfo = this->_typeInfo(nodeVarDecl.var->type);

            def += "  " + typeInfo.typeCode + contextVarName + ";" EOL;
          }
        }

        for (auto i = static_cast<std::size_t>(0); i < asyncBreakNodesCount; i++) {
          def += "  unsigned char b" + std::to_string(i + 1) + ";" EOL;
        }

        for (auto i = static_cast<std::size_t>(0); i < asyncContinueNodesCount; i++) {
          def += "  unsigned char c" + std::to_string(i + 1) + ";" EOL;
        }

        for (const auto &awaitExpr : flattenAwaitCalls) {
          auto exprAwait = std::get<ASTExprAwait>(*awaitExpr.body);
          auto typeInfo = this->_typeInfo(awaitExpr.type);

          def += "  " + typeInfo.typeCode + "t" + std::to_string(exprAwait.id) + ";" EOL;
        }

        def += "};";
        return 0;
      });
    }

    this->_apiEntity(typeName, CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
      this->varMap.save();
      this->indent = 2;

      auto bodyCode = std::string();

      if (!params.empty() || hasSelfParam || this->throws) {
        bodyCode += "  struct _{" + paramsName + "} *p = pp;" EOL;
      }

      if (this->throws) {
        bodyCode += R"(  _{error_stack_push}(&_{err_state}, ")" + this->reader->path +  R"(", ")" + var->name + R"(", p->line, p->col);)" EOL;
        this->state.cleanUp.add(this->_apiEval("if (_{err_state}.id != -1) _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);"));
        this->state.cleanUp.add("_{error_stack_pop}(&_{err_state});");
      }

      if (!stack.empty() || !flattenAsyncBody.empty()) {
        bodyCode += "  struct _{" + contextName + "} *x = px;" EOL;

        for (const auto &contextVar : stack) {
          auto contextVarName = Codegen::name(contextVar->codeName);
          auto contextVarTypeInfo = this->_typeInfo(contextVar->type);
          auto typeRefCode = contextVar->mut ? contextVarTypeInfo.typeRefCode : contextVarTypeInfo.typeRefCodeConst;

          bodyCode += "  " + typeRefCode + contextVarName + " = x->" + contextVarName + ";" EOL;
          this->state.contextVars.insert(contextVarName);
        }

        for (const auto &asyncNode : flattenAsyncBody) {
          if (std::holds_alternative<ASTNodeFnDecl>(*asyncNode.body)) {
            auto nodeFnDecl = std::get<ASTNodeFnDecl>(*asyncNode.body);
            auto contextVarName = Codegen::name(nodeFnDecl.var->codeName);
            auto typeInfo = this->_typeInfo(nodeFnDecl.var->type);

            bodyCode += "  " + typeInfo.typeRefCode + contextVarName + " = &x->" + contextVarName + ";" EOL;
            this->state.contextVars.insert(contextVarName);
          } else if (std::holds_alternative<ASTNodeObjDecl>(*asyncNode.body)) {
            auto nodeObjDecl = std::get<ASTNodeObjDecl>(*asyncNode.body);

            for (const auto &method : nodeObjDecl.methods) {
              auto contextVarName = Codegen::name(method.var->codeName);
              auto typeInfo = this->_typeInfo(method.var->type);

              bodyCode += "  " + typeInfo.typeRefCode + contextVarName + " = &x->" + contextVarName + ";" EOL;
              this->state.contextVars.insert(contextVarName);
            }
          } else if (std::holds_alternative<ASTNodeVarDecl>(*asyncNode.body)) {
            auto nodeVarDecl = std::get<ASTNodeVarDecl>(*asyncNode.body);
            auto contextVarName = Codegen::name(nodeVarDecl.var->codeName);
            auto typeInfo = this->_typeInfo(nodeVarDecl.var->type);

            bodyCode += "  " + typeInfo.typeRefCode + contextVarName + " = &x->" + contextVarName + ";" EOL;
            this->state.contextVars.insert(contextVarName);
          }
        }

        for (auto i = static_cast<std::size_t>(0); i < asyncBreakNodesCount; i++) {
          auto idx = std::to_string(i + 1);
          bodyCode += "  unsigned char *b" + idx + " = &x->b" + idx + ";" EOL;
        }

        for (auto i = static_cast<std::size_t>(0); i < asyncContinueNodesCount; i++) {
          auto idx = std::to_string(i + 1);
          bodyCode += "  unsigned char *c" + idx + " = &x->c" + idx + ";" EOL;
        }

        for (const auto &awaitExpr : flattenAwaitCalls) {
          auto exprAwait = std::get<ASTExprAwait>(*awaitExpr.body);
          auto contextVarName = "t" + std::to_string(exprAwait.id);
          auto typeInfo = this->_typeInfo(awaitExpr.type);

          bodyCode += "  " + typeInfo.typeRefCode + contextVarName + " = &x->" + contextVarName + ";" EOL;
        }
      }

      if (hasSelfParam) {
        auto selfTypeInfo = this->_typeInfo(fnType.callInfo.selfType);
        bodyCode += "  " + (fnType.callInfo.isSelfMut ? selfTypeInfo.typeCode : selfTypeInfo.typeCodeConst) +
          Codegen::name(fnType.callInfo.selfCodeName) + " = p->self;" EOL;
      }

      if (!params.empty()) {
        auto paramsDecl = std::string();
        auto paramsCode = std::string();
        auto paramIdx = static_cast<std::size_t>(0);

        for (const auto &param : params) {
          auto paramName = Codegen::name(param.var->codeName);
          auto paramTypeInfo = this->_typeInfo(param.var->type);
          auto paramIdxStr = std::to_string(paramIdx);

          paramsCode += "  " + (param.var->mut ? paramTypeInfo.typeCode : paramTypeInfo.typeCodeConst) + paramName + " = ";

          if (param.init == std::nullopt) {
            paramsCode += "p->n" + paramIdxStr;
          } else {
            auto initCode = this->_nodeExpr(*param.init, paramTypeInfo.type, node, paramsDecl);
            paramsCode += "p->o" + paramIdxStr + " == 1 ? p->n" + paramIdxStr + " : " + initCode;
          }

          paramsCode += ";" EOL;

          if (paramTypeInfo.type->shouldBeFreed()) {
            this->state.cleanUp.add(this->_genFreeFn(paramTypeInfo.type, paramName) + ";");
          }

          paramIdx++;
        }

        bodyCode += paramsDecl + paramsCode;
      }

      if (hasSelfParam && fnType.callInfo.selfType->shouldBeFreed()) {
        this->state.cleanUp.add(this->_genFreeFn(fnType.callInfo.selfType, Codegen::name(fnType.callInfo.selfCodeName)) + ";");
      }

      if (fnType.async) {
        bodyCode += "  switch (step) {" EOL;
        bodyCode += "    case 0: {" EOL;
      }

      auto returnTypeInfo = this->_typeInfo(fnType.returnType);

      this->state.returnType = returnTypeInfo.type;
      this->state.insideAsync = fnType.async;
      this->state.asyncCounter = 0;
      this->indent = this->state.insideAsync ? 4 : 0;
      bodyCode += this->_block(*body, false);
      this->varMap.restore();

      if (!returnTypeInfo.type->isVoid() && this->state.cleanUp.valueVarUsed) {
        if (this->state.insideAsync) {
          bodyCode.insert(0, "  " + returnTypeInfo.typeRefCode + "v = pv;" EOL);
        } else {
          bodyCode.insert(0, "  " + returnTypeInfo.typeCode + "v;" EOL);
          bodyCode += this->state.cleanUp.gen(this->state.insideAsync ? 6 : 2);
        }
      } else {
        bodyCode += this->state.cleanUp.gen(this->state.insideAsync ? 6 : 2);
      }

      if (!returnTypeInfo.type->isVoid() && this->state.cleanUp.valueVarUsed && !this->state.insideAsync) {
        bodyCode += "  return v;" EOL;
      }

      if (this->state.cleanUp.returnVarUsed) {
        bodyCode.insert(0, "  unsigned char " + this->state.cleanUp.currentReturnVar() + " = 0;" EOL);
      }

      if (fnType.async) {
        bodyCode += "    }" EOL;
        bodyCode += "  }" EOL;
        bodyCode += "  return -1;" EOL;
      }

      decl += (this->state.insideAsync ? "int " : returnTypeInfo.typeCode) + typeName + " (";
      def += (this->state.insideAsync ? "int " : returnTypeInfo.typeCode) + typeName + " (";

      if (fnType.async) {
        decl += this->_apiEval("_{threadpool_t} *, _{threadpool_job_t} *, ");
        def += this->_apiEval("_{threadpool_t} *tp, _{threadpool_job_t} *job, ");
      }

      decl += "void *";
      def += "void *px";

      if (this->throws || !params.empty() || hasSelfParam || fnType.async) {
        decl += ", void *";
        def += ", void *pp";
      }

      if (fnType.async) {
        decl += ", void *, int";
        def += ", void *pv, int step";
      }

      decl += ");";
      def += ") {" EOL + bodyCode + "}";

      return stack.empty() && flattenAsyncBody.empty() ? 0 : contextEntityIdx;
    });

    if (!stack.empty() || !flattenAsyncBody.empty()) {
      this->_apiEntity(typeName + "_alloc", CODEGEN_ENTITY_FN, [&] (auto &decl, auto &def) {
        decl += "void " + typeName + "_alloc (" + varTypeInfo.typeRefCode + ", struct _{" + contextName + "});";
        def += "void " + typeName + "_alloc (" + varTypeInfo.typeRefCode + "n, struct _{" + contextName + "} x) {" EOL;
        def += "  _{size_t} l = sizeof(struct _{" + contextName + "});" EOL;
        def += "  struct _{" + contextName + "} *r = _{alloc}(l);" EOL;
        def += "  _{memcpy}(r, &x, l);" EOL;
        def += "  n->f = &_{" + typeName + "};" EOL;
        def += "  n->x = r;" EOL;
        def += "  n->l = l;" EOL;
        def += "}";

        return 0;
      });
    }

    this->indent = initialIndent;
    this->state.cleanUp = initialStateCleanUp;
    this->state.contextVars = initialStateContextVars;
    this->state.insideAsync = initialStateInsideAsync;
    this->state.returnType = initialStateReturnType;
    this->state.asyncCounter = initialStateAsyncCounter;
  }

  auto fnName = var->builtin ? codeName : Codegen::name(codeName);

  if ((phase == CODEGEN_PHASE_ALLOC || phase == CODEGEN_PHASE_FULL) && stack.empty() && flattenAsyncBody.empty()) {
    code += std::string(this->indent, ' ') + (this->state.insideAsync ? "*" : "const " + varTypeInfo.typeCode) + fnName;
    code += this->_apiEval(" = (" + varTypeInfo.typeCodeTrimmed + ") {&_{" + typeName + "}, _{NULL}, 0};" EOL);
  } else if ((phase == CODEGEN_PHASE_ALLOC || phase == CODEGEN_PHASE_FULL) && !this->state.insideAsync) {
    code += std::string(this->indent, ' ') + "const " + varTypeInfo.typeCode + fnName;
    code += ";" EOL;
  }

  if ((phase == CODEGEN_PHASE_INIT || phase == CODEGEN_PHASE_FULL) && (!stack.empty() || !flattenAsyncBody.empty())) {
    code += std::string(this->indent, ' ') + this->_apiEval("_{" + typeName + "_alloc}((" + varTypeInfo.typeRefCode + ") ");
    code += (this->state.insideAsync ? "" : "&") + fnName + ", ";
    this->_activateEntity(contextName);
    code += "(struct " + contextName + ") {";

    auto contextVarIdx = static_cast<std::size_t>(0);

    for (const auto &contextVar : stack) {
      auto contextVarName = Codegen::name(contextVar->codeName);

      code += contextVarIdx == 0 ? "" : ", ";
      code += (this->state.contextVars.contains(contextVarName) ? "" : "&") + contextVarName;

      contextVarIdx++;
    }

    code += "});" EOL;

    if (varTypeInfo.type->shouldBeFreed()) {
      this->state.cleanUp.add(this->_genFreeFn(varTypeInfo.type, (this->async && node.parent != nullptr ? "*" : "") + fnName) + ";");
    }
  }

  return code;
}
