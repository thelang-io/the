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
#include "../codegen-metadata.hpp"
#include "../config.hpp"

std::vector<std::shared_ptr<Var>> Codegen::filterAsyncDeclarations (const std::vector<ASTNode> &nodes) {
  auto result = std::vector<std::shared_ptr<Var>>{};
  auto vals = std::set<std::string>{};

  for (const auto &node : nodes) {
    if (std::holds_alternative<ASTNodeFnDecl>(*node.body)) {
      auto nodeFnDecl = std::get<ASTNodeFnDecl>(*node.body);
      auto contextVarName = Codegen::name(nodeFnDecl.var->codeName);

      if (!vals.contains(nodeFnDecl.var->codeName)) {
        vals.insert(nodeFnDecl.var->codeName);
        result.push_back(nodeFnDecl.var);
      }
    } else if (std::holds_alternative<ASTNodeObjDecl>(*node.body)) {
      auto nodeObjDecl = std::get<ASTNodeObjDecl>(*node.body);

      for (const auto &method : nodeObjDecl.methods) {
        auto contextVarName = Codegen::name(method.var->codeName);

        if (!vals.contains(method.var->codeName)) {
          vals.insert(method.var->codeName);
          result.push_back(method.var);
        }
      }
    } else if (std::holds_alternative<ASTNodeVarDecl>(*node.body)) {
      auto nodeVarDecl = std::get<ASTNodeVarDecl>(*node.body);

      if (!vals.contains(nodeVarDecl.var->codeName)) {
        vals.insert(nodeVarDecl.var->codeName);
        result.push_back(nodeVarDecl.var);
      }
    }
  }

  return result;
}

std::string Codegen::getEnvVar (const std::string &name) {
  #if defined(OS_WINDOWS)
    auto buf = static_cast<char *>(nullptr);
    auto size = static_cast<std::size_t>(0);

    if (_dupenv_s(&buf, &size, name.c_str()) != 0 || buf == nullptr) {
      return "";
    }

    auto result = std::string(buf);
    free(buf);
    return result;
  #else
    const char *result = std::getenv(name.c_str());
    return result == nullptr ? "" : result;
  #endif
}

std::string Codegen::_ (const std::string &name, const std::optional<std::set<std::string> *> &dependencies) {
  auto result = name;

  if (codegenMetadata.contains(result)) {
    for (const auto &dependency : codegenMetadata.at(result)) {
      if (dependencies == std::nullopt) {
        this->_activateBuiltin(dependency);
      } else if (!(*dependencies)->contains(dependency)) {
        (*dependencies)->emplace(dependency);
      }
    }
  } else if (this->api.contains(result)) {
    if (dependencies == std::nullopt) {
      this->_activateBuiltin(result);
    } else if (!(*dependencies)->contains(result)) {
      (*dependencies)->emplace(result);
    }
  } else {
    if (result.starts_with("TYPE_") && this->ast->typeMap.has(result.substr(5))) {
      result = this->_typeDef(this->ast->typeMap.get(result.substr(5)));
    } else if (result.starts_with("array_")) {
      if (result.ends_with("_free")) {
        result = this->_typeNameArray(this->ast->typeMap.createArr(this->ast->typeMap.get(result.substr(6, result.size() - 11)))) + "_free";
      } else {
        result = this->_typeNameArray(this->ast->typeMap.createArr(this->ast->typeMap.get(result.substr(6))));
      }
    } else if (result.starts_with("map_") || result.starts_with("pair_")) {
      auto isPair = result.starts_with("pair_");
      auto nameSliced = isPair ? result.substr(5) : result.substr(4);
      auto keyType = this->ast->typeMap.get(nameSliced.substr(0, nameSliced.find('_')));
      auto valueType = this->ast->typeMap.get(nameSliced.substr(nameSliced.find('_') + 1));

      result = this->_typeNameMap(this->ast->typeMap.createMap(keyType, valueType));

      if (isPair) {
        result = Codegen::typeName("pair_" + result.substr(Codegen::typeName("map_").size()));
      }
    }

    if (dependencies == std::nullopt) {
      try {
        this->_activateBuiltin(result);
      } catch (const Error &) {
        this->_activateEntity(result);
      }
    } else if (!(*dependencies)->contains(result)) {
      (*dependencies)->emplace(result);
    }
  }

  return result;
}

void Codegen::_activateBuiltin (const std::string &name, std::optional<std::vector<std::string> *> entityBuiltins) {
  if (entityBuiltins != std::nullopt || this->state.builtins != std::nullopt) {
    auto b = entityBuiltins == std::nullopt ? *this->state.builtins : *entityBuiltins;

    if (std::find(b->begin(), b->end(), name) == b->end()) {
      b->push_back(name);
    }

    return;
  }

  if (name == "definitions") {
    if (this->builtins.definitions) return;
    this->builtins.definitions = true;
  } else if (name == "externSystemFunction036") {
    if (this->builtins.externSystemFunction036) return;
    this->builtins.externSystemFunction036 = true;
    this->_activateBuiltin("definitions");

    if (std::find(this->flags.begin(), this->flags.end(), "W:-ladvapi32") == this->flags.end()) {
      this->flags.emplace_back("W:-ladvapi32");
    }
  } else if (name == "libArpaInet") {
    if (this->builtins.libArpaInet) return;
    this->builtins.libArpaInet = true;
  } else if (name == "libCtype") {
    if (this->builtins.libCtype) return;
    this->builtins.libCtype = true;
  } else if (name == "libDirent") {
    if (this->builtins.libDirent) return;
    this->builtins.libDirent = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libErrno") {
    if (this->builtins.libErrno) return;
    this->builtins.libErrno = true;
  } else if (name == "libFcntl") {
    if (this->builtins.libFcntl) return;
    this->builtins.libFcntl = true;
  } else if (name == "libFloat") {
    if (this->builtins.libFloat) return;
    this->builtins.libFloat = true;
  } else if (name == "libInttypes") {
    if (this->builtins.libInttypes) return;
    this->builtins.libInttypes = true;
  } else if (name == "libMath") {
    if (this->builtins.libMath) return;
    this->builtins.libMath = true;

    if (std::find(this->flags.begin(), this->flags.end(), "U:-lm") == this->flags.end()) {
      this->flags.emplace_back("U:-lm");
    }
  } else if (name == "libNetdb") {
    if (this->builtins.libNetdb) return;
    this->builtins.libNetdb = true;
  } else if (name == "libNetinetIn") {
    if (this->builtins.libNetinetIn) return;
    this->builtins.libNetinetIn = true;
  } else if (name == "libOpensslRand") {
    if (this->builtins.libOpensslRand) return;
    this->builtins.libOpensslRand = true;

    if (std::find(this->flags.begin(), this->flags.end(), "A:-lssl") == this->flags.end()) {
      this->flags.emplace_back("A:-lssl");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "A:-lcrypto") == this->flags.end()) {
      this->flags.emplace_back("A:-lcrypto");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "W:-lws2_32") == this->flags.end()) {
      this->flags.emplace_back("W:-lws2_32");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "W:-lgdi32") == this->flags.end()) {
      this->flags.emplace_back("W:-lgdi32");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "W:-ladvapi32") == this->flags.end()) {
      this->flags.emplace_back("W:-ladvapi32");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "W:-lcrypt32") == this->flags.end()) {
      this->flags.emplace_back("W:-lcrypt32");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "W:-luser32") == this->flags.end()) {
      this->flags.emplace_back("W:-luser32");
    }
  } else if (name == "libOpensslSsl") {
    if (this->builtins.libOpensslSsl) return;
    this->builtins.libOpensslSsl = true;

    if (std::find(this->flags.begin(), this->flags.end(), "A:-lssl") == this->flags.end()) {
      this->flags.emplace_back("A:-lssl");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "A:-lcrypto") == this->flags.end()) {
      this->flags.emplace_back("A:-lcrypto");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "W:-lws2_32") == this->flags.end()) {
      this->flags.emplace_back("W:-lws2_32");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "W:-lgdi32") == this->flags.end()) {
      this->flags.emplace_back("W:-lgdi32");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "W:-ladvapi32") == this->flags.end()) {
      this->flags.emplace_back("W:-ladvapi32");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "W:-lcrypt32") == this->flags.end()) {
      this->flags.emplace_back("W:-lcrypt32");
    }

    if (std::find(this->flags.begin(), this->flags.end(), "W:-luser32") == this->flags.end()) {
      this->flags.emplace_back("W:-luser32");
    }
  } else if (name == "libPthread") {
    if (this->builtins.libPthread) return;
    this->builtins.libPthread = true;

    if (std::find(this->flags.begin(), this->flags.end(), "U:-pthread") == this->flags.end()) {
      this->flags.emplace_back("U:-pthread");
    }
  } else if (name == "libPwd") {
    if (this->builtins.libPwd) return;
    this->builtins.libPwd = true;
  } else if (name == "libSetJmp") {
    if (this->builtins.libSetJmp) return;
    this->builtins.libSetJmp = true;
  } else if (name == "libStdNoReturn") {
    if (this->builtins.libStdNoReturn) return;
    this->builtins.libStdNoReturn = true;
  } else if (name == "libStdarg") {
    if (this->builtins.libStdarg) return;
    this->builtins.libStdarg = true;
  } else if (name == "libStdbool") {
    if (this->builtins.libStdbool) return;
    this->builtins.libStdbool = true;
  } else if (name == "libStddef") {
    if (this->builtins.libStddef) return;
    this->builtins.libStddef = true;
  } else if (name == "libStdint") {
    if (this->builtins.libStdint) return;
    this->builtins.libStdint = true;
  } else if (name == "libStdio") {
    if (this->builtins.libStdio) return;
    this->builtins.libStdio = true;
  } else if (name == "libStdlib") {
    if (this->builtins.libStdlib) return;
    this->builtins.libStdlib = true;
  } else if (name == "libString") {
    if (this->builtins.libString) return;
    this->builtins.libString = true;
  } else if (name == "libSysSocket") {
    if (this->builtins.libSysSocket) return;
    this->builtins.libSysSocket = true;
  } else if (name == "libSysStat") {
    if (this->builtins.libSysStat) return;
    this->builtins.libSysStat = true;
  } else if (name == "libSysTypes") {
    if (this->builtins.libSysTypes) return;
    this->builtins.libSysTypes = true;
  } else if (name == "libSysUtsname") {
    if (this->builtins.libSysUtsname) return;
    this->builtins.libSysUtsname = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libTime") {
    if (this->builtins.libTime) return;
    this->builtins.libTime = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libUnistd") {
    if (this->builtins.libUnistd) return;
    this->builtins.libUnistd = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libWinDirect") {
    if (this->builtins.libWinDirect) return;
    this->builtins.libWinDirect = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libWinIo") {
    if (this->builtins.libWinIo) return;
    this->builtins.libWinIo = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libWinWs2tcpip") {
    if (this->builtins.libWinWs2tcpip) return;
    this->builtins.libWinWs2tcpip = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libWindows") {
    if (this->builtins.libWindows) return;
    this->builtins.libWindows = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libWinsock2") {
    if (this->builtins.libWinsock2) return;
    this->builtins.libWinsock2 = true;
    this->_activateBuiltin("definitions");
  } else if (name == "typeAny") {
    if (this->builtins.typeAny) return;
    this->builtins.typeAny = true;
    this->_activateBuiltin("libStdlib");
  } else if (name == "typeBuffer") {
    if (this->builtins.typeBuffer) return;
    this->builtins.typeBuffer = true;
    this->_activateBuiltin("libStdlib");
  } else if (name == "typeErrStack") {
    if (this->builtins.typeErrStack) return;
    this->builtins.typeErrStack = true;
  } else if (name == "typeErrState") {
    if (this->builtins.typeErrState) return;
    this->builtins.typeErrState = true;
    this->_activateBuiltin("libSetJmp");
    this->_activateBuiltin("typeErrStack");
  } else if (name == "typeRequest") {
    if (this->builtins.typeRequest) return;
    this->builtins.typeRequest = true;
    this->_activateBuiltin("definitions");
    this->_activateBuiltin("libOpensslSsl");
    this->_activateBuiltin("libWinsock2");
  } else if (name == "typeStr") {
    if (this->builtins.typeStr) return;
    this->builtins.typeStr = true;
    this->_activateBuiltin("libStdlib");
  } else if (name == "typeThreadpool") {
    if (this->builtins.typeThreadpool) return;
    this->builtins.typeThreadpool = true;
    this->_activateBuiltin("libPthread");
    this->_activateBuiltin("libStdbool");
    this->_("threadpool_job_t");
    this->_("threadpool_thread_t");
  } else if (name == "typeThreadpoolFunc") {
    if (this->builtins.typeThreadpoolFunc) return;
    this->builtins.typeThreadpoolFunc = true;
    this->_("threadpool_t");
    this->_("threadpool_job_t");
  } else if (name == "typeThreadpoolJob") {
    if (this->builtins.typeThreadpoolJob) return;
    this->builtins.typeThreadpoolJob = true;
    this->_("threadpool_func_t");
  } else if (name == "typeThreadpoolThread") {
    if (this->builtins.typeThreadpoolThread) return;
    this->builtins.typeThreadpoolThread = true;
    this->_activateBuiltin("libPthread");
    this->_("threadpool_t");
  } else if (name == "typeWinReparseDataBuffer") {
    if (this->builtins.typeWinReparseDataBuffer) return;
    this->builtins.typeWinReparseDataBuffer = true;
    this->_activateBuiltin("definitions");
    this->_activateBuiltin("libWindows");
  } else if (name == "varEnviron") {
    if (this->builtins.varEnviron) return;
    this->builtins.varEnviron = true;
    this->_activateBuiltin("libUnistd");
  } else if (name == "varErrState") {
    if (this->builtins.varErrState) return;
    this->builtins.varErrState = true;
    this->_activateBuiltin("libStdlib");
    this->_activateBuiltin("typeErrState");
  } else if (name == "varLibOpensslInit") {
    if (this->builtins.varLibOpensslInit) return;
    this->builtins.varLibOpensslInit = true;
    this->_activateBuiltin("libStdbool");
  } else if (name == "varLibWs2Init") {
    if (this->builtins.varLibWs2Init) return;
    this->builtins.varLibWs2Init = true;
    this->_activateBuiltin("libStdbool");
  } else if (this->api.contains(name)) {
    this->api[name].enabled = true;

    for (const auto &it : this->api[name].dependencies) {
      this->_(it);
    }
  } else {
    throw Error("tried activating unknown builtin `" + name + "`");
  }
}

void Codegen::_activateEntity (const std::string &name, std::optional<std::vector<std::string> *> entityEntities) {
  if (entityEntities != std::nullopt || this->state.entities != std::nullopt) {
    auto e = entityEntities == std::nullopt ? *this->state.entities : *entityEntities;

    if (std::find(e->begin(), e->end(), name) == e->end()) {
      e->push_back(name);
    }

    return;
  }

  for (auto &entity : this->entities) {
    if (entity.name != name) {
      continue;
    }

    if (!entity.active) {
      entity.active = true;

      for (const auto &it : entity.builtins) {
        this->_(it);
      }

      for (const auto &it : entity.entities) {
        this->_(it);
      }
    }

    return;
  }

  throw Error("tried activating unknown entity `" + name + "`");
}

int Codegen::_apiEntity (
  const std::string &name,
  CodegenEntityType type,
  const std::optional<std::function<int (std::string &, std::string &)>> &fn
) {
  this->_saveStateBuiltinsEntities();
  auto entity = CodegenEntity{name, type};
  this->state.builtins = &entity.builtins;
  this->state.entities = &entity.entities;
  auto newPos = fn == std::nullopt ? 0 : (*fn)(entity.decl, entity.def);
  entity.decl = entity.decl.empty() ? entity.decl : this->_apiEval(entity.decl);
  entity.def = entity.def.empty() ? entity.def : this->_apiEval(entity.def);
  this->_restoreStateBuiltinsEntities();

  if (newPos == 0) {
    this->entities.push_back(entity);
    return static_cast<int>(this->entities.size()) - 1;
  } else {
    this->entities.insert(this->entities.begin() + newPos, entity);
    return newPos;
  }
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
std::string Codegen::_apiEval (
  const std::string &code,
  int limit,
  const std::string &selfName,
  const std::optional<std::set<std::string> *> &dependencies
) {
  auto name = std::string();
  auto isName = false;
  auto result = std::string();
  result.reserve(code.size());

  for (auto i = static_cast<std::size_t>(0); i < code.size(); i++) {
    auto ch = code[i];

    if (limit != -1 && ch == '_' && i + 1 < code.size() && code[i + 1] == '{') {
      isName = true;
      i += 1;
    } else if (isName && ch == '}') {
      if (selfName != name) {
        this->_(name, dependencies);
      }

      result += name;
      isName = false;
      limit = limit == 1 ? -1 : limit == 0 ? 0 : limit - 1;
      name.clear();
    } else if (isName) {
      name += ch;
    } else {
      result += ch;
    }
  }

  return result;
}

std::shared_ptr<CodegenASTStmt> Codegen::_genAsyncReturn (const std::shared_ptr<std::size_t> &counter) {
  return CodegenASTStmtReturn::create(
    CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_("threadpool_insert")),
      {
        CodegenASTExprAccess::create("tp"),
        CodegenASTExprCall::create(
          CodegenASTExprAccess::create(this->_("threadpool_job_step")),
          {
            CodegenASTExprAccess::create("job"),
            CodegenASTExprLiteral::create(counter)
          }
        )
      }
    )
  );
}

std::shared_ptr<CodegenASTExpr> Codegen::_genCopyFn (Type *type, const std::shared_ptr<CodegenASTExpr> &expr) {
  auto result = expr;

  if (type->isAlias()) {
    result = this->_genCopyFn(std::get<TypeAlias>(type->body).type, result);
  } else if (type->isAny()) {
    result = CodegenASTExprCall::create(CodegenASTExprAccess::create(this->_("any_copy")), {result});
  } else if (type->isObj() && type->builtin && type->codeName == "@buffer_Buffer") {
    result = CodegenASTExprCall::create(CodegenASTExprAccess::create(this->_("buffer_copy")), {result});
  } else if (
    type->isArray() ||
    type->isFn() ||
    type->isMap() ||
    type->isObj() ||
    type->isOpt() ||
    type->isUnion()
  ) {
    auto typeInfo = this->_typeInfo(type);

    result = CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_(typeInfo.realTypeName + "_copy")),
      {result}
    );
  } else if (type->isStr()) {
    result = CodegenASTExprCall::create(CodegenASTExprAccess::create(this->_("str_copy")), {result});
  }

  return result;
}

std::shared_ptr<CodegenASTExpr> Codegen::_genEqFn (Type *type, const std::shared_ptr<CodegenASTExpr> &leftExpr, const std::shared_ptr<CodegenASTExpr> &rightExpr, bool reverse) {
  auto realType = Type::real(type);
  auto direction = std::string(reverse ? "ne" : "eq");

  if (realType->isObj() && realType->builtin && realType->codeName == "@buffer_Buffer") {
    return CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_("buffer_" + direction)),
      {this->_genCopyFn(realType, leftExpr), this->_genCopyFn(realType, rightExpr)}
    );
  } else if (
    realType->isArray() ||
    realType->isMap() ||
    realType->isObj() ||
    realType->isOpt() ||
    realType->isUnion()
  ) {
    auto typeInfo = this->_typeInfo(realType);

    return CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_(typeInfo.realTypeName + "_" + direction)),
      {this->_genCopyFn(typeInfo.realType, leftExpr), this->_genCopyFn(typeInfo.realType, rightExpr)}
    );
  } else if (realType->isStr()) {
    return CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_("str_" + direction + "_str")),
      {this->_genCopyFn(realType, leftExpr), this->_genCopyFn(realType, rightExpr)}
    );
  } else {
    return CodegenASTExprAssign::create(leftExpr, reverse ? "!=" : "==", rightExpr);
  }
}

std::shared_ptr<CodegenASTExpr> Codegen::_genErrState (bool insideSyncMain, bool shouldCopy, const std::string &prop) {
  auto cExpr = insideSyncMain
    ? CodegenASTExprUnary::create("&", CodegenASTExprAccess::create(this->_("err_state")))
    : CodegenASTExprAccess::create("fn_err_state");

  if (!prop.empty() && insideSyncMain) {
    cExpr = CodegenASTExprAccess::create(CodegenASTExprAccess::create(this->_("err_state")), prop);
  } else if (!prop.empty()) {
    cExpr = CodegenASTExprAccess::create(cExpr, prop, true);
  } else if (shouldCopy) {
    cExpr = CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_("xalloc")),
      {
        cExpr,
        CodegenASTExprCall::create(
          CodegenASTExprAccess::create("sizeof"),
          {CodegenASTType::create(this->_("err_state_t"))}
        )
      }
    );
  }

  return cExpr;
}

std::shared_ptr<CodegenASTExpr> Codegen::_genFreeFn (Type *type, const std::shared_ptr<CodegenASTExpr> &expr) {
  auto result = expr;

  if (type->isAlias()) {
    result = this->_genFreeFn(std::get<TypeAlias>(type->body).type, result);
  } else if (type->isAny()) {
    result = CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_("any_free")),
      {CodegenASTExprCast::create(CodegenASTType::create(this->_("struct any")), result)}
    );
  } else if (type->isObj() && type->builtin && type->codeName == "@buffer_Buffer") {
    result = CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_("buffer_free")),
      {CodegenASTExprCast::create(CodegenASTType::create(this->_("struct buffer")), result)}
    );
  } else if (
    type->isArray() ||
    type->isFn() ||
    type->isMap() ||
    type->isObj() ||
    type->isOpt() ||
    type->isUnion()
  ) {
    auto typeInfo = this->_typeInfo(type);

    result = CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_(typeInfo.realTypeName + "_free")),
      {CodegenASTExprCast::create(CodegenASTType::create(typeInfo.realTypeCodeTrimmed), result)}
    );
  } else if (type->isStr()) {
    result = CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_("str_free")),
      {CodegenASTExprCast::create(CodegenASTType::create(this->_("struct str")), result)}
    );
  }

  return result;
}

std::shared_ptr<CodegenASTExpr> Codegen::_genReallocFn (Type *type, const std::shared_ptr<CodegenASTExpr> &leftExpr, const std::shared_ptr<CodegenASTExpr> &rightExpr) {
  auto result = CodegenASTExprNull::create();

  if (type->isAny()) {
    result = CodegenASTExprAssign::create(
      leftExpr,
      "=",
      CodegenASTExprCall::create(CodegenASTExprAccess::create(this->_("any_realloc")), {leftExpr, rightExpr})
    );
  } else if (type->isObj() && type->builtin && type->codeName == "@buffer_Buffer") {
    result = CodegenASTExprAssign::create(
      leftExpr,
      "=",
      CodegenASTExprCall::create(CodegenASTExprAccess::create(this->_("buffer_realloc")), {leftExpr, rightExpr})
    );
  } else if (
    type->isArray() ||
    type->isFn() ||
    type->isMap() ||
    type->isObj() ||
    type->isOpt() ||
    type->isUnion()
  ) {
    auto typeInfo = this->_typeInfo(type);

    result = CodegenASTExprAssign::create(
      leftExpr,
      "=",
      CodegenASTExprCall::create(
        CodegenASTExprAccess::create(this->_(typeInfo.typeName + "_realloc")),
        {leftExpr, rightExpr}
      )
    );
  } else if (type->isStr()) {
    result = CodegenASTExprAssign::create(
      leftExpr,
      "=",
      CodegenASTExprCall::create(CodegenASTExprAccess::create(this->_("str_realloc")), {leftExpr, rightExpr})
    );
  }

  return result;
}

std::shared_ptr<CodegenASTExpr> Codegen::_genStrFn (Type *type, const std::shared_ptr<CodegenASTExpr> &expr, bool copy, bool escape) {
  auto realType = Type::real(type);
  auto result = expr;

  if (realType->isAny()) {
    result = CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_("any_str")),
      {copy ? this->_genCopyFn(realType, result) : result}
    );
  } else if (realType->isObj() && realType->builtin && realType->codeName == "@buffer_Buffer") {
    result = CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_("buffer_str")),
      {copy ? this->_genCopyFn(realType, result) : result}
    );
  } else if (
    realType->isArray() ||
    realType->isFn() ||
    realType->isMap() ||
    realType->isObj() ||
    realType->isOpt() ||
    realType->isUnion()
  ) {
    auto typeInfo = this->_typeInfo(realType);

    result = CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_(typeInfo.realTypeName + "_str")),
      {copy ? this->_genCopyFn(realType, result) : result}
    );
  } else if (realType->isEnum()) {
    result = CodegenASTExprCall::create(CodegenASTExprAccess::create(this->_("enum_str")), {result});
  } else if (realType->isStr() && escape) {
    result = CodegenASTExprCall::create(CodegenASTExprAccess::create(this->_("str_escape")), {result});
  } else if (realType->isStr()) {
    result = copy ? this->_genCopyFn(realType, result) : result;
  } else {
    auto typeInfo = this->_typeInfo(realType);

    result = CodegenASTExprCall::create(
      CodegenASTExprAccess::create(this->_(typeInfo.realTypeName + "_str")),
      {result}
    );
  }

  return result;
}

void Codegen::_restoreStateBuiltinsEntities () {
  this->state.builtins = this->builtinsEntitiesBuffer.back().builtins;
  this->state.entities = this->builtinsEntitiesBuffer.back().entities;
  this->builtinsEntitiesBuffer.pop_back();
}

void Codegen::_saveStateBuiltinsEntities () {
  this->builtinsEntitiesBuffer.push_back(CodegenStateBuiltinsEntities{
    this->state.builtins,
    this->state.entities
  });

  if (this->state.builtins == std::nullopt) {
    this->state.builtins = &this->bufferBuiltins;
  }

  if (this->state.entities == std::nullopt) {
    this->state.entities = &this->bufferEntities;
  }
}
