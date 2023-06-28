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

void Codegen::_activateBuiltin (const std::string &name, std::optional<std::vector<std::string> *> entityBuiltins) {
  if (entityBuiltins != std::nullopt || this->state.builtins != std::nullopt) {
    auto b = entityBuiltins == std::nullopt ? *this->state.builtins : *entityBuiltins;

    if (std::find(b->begin(), b->end(), name) == b->end()) {
      b->push_back(name);
    }

    return;
  }

  if (name == "definitions") {
    this->builtins.definitions = true;
  } else if (name == "externSystemFunction036") {
    this->builtins.externSystemFunction036 = true;
    this->_activateBuiltin("definitions");

    if (std::find(this->flags.begin(), this->flags.end(), "W:-ladvapi32") == this->flags.end()) {
      this->flags.emplace_back("W:-ladvapi32");
    }
  } else if (name == "libArpaInet") {
    this->builtins.libArpaInet = true;
  } else if (name == "libCtype") {
    this->builtins.libCtype = true;
  } else if (name == "libDirent") {
    this->builtins.libDirent = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libErrno") {
    this->builtins.libErrno = true;
  } else if (name == "libFcntl") {
    this->builtins.libFcntl = true;
  } else if (name == "libFloat") {
    this->builtins.libFloat = true;
  } else if (name == "libInttypes") {
    this->builtins.libInttypes = true;
  } else if (name == "libMath") {
    this->builtins.libMath = true;

    if (std::find(this->flags.begin(), this->flags.end(), "U:-lm") == this->flags.end()) {
      this->flags.emplace_back("U:-lm");
    }
  } else if (name == "libNetdb") {
    this->builtins.libNetdb = true;
  } else if (name == "libNetinetIn") {
    this->builtins.libNetinetIn = true;
  } else if (name == "libOpensslRand") {
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
    this->builtins.libPthread = true;
  } else if (name == "libPwd") {
    this->builtins.libPwd = true;
  } else if (name == "libSetJmp") {
    this->builtins.libSetJmp = true;
  } else if (name == "libStdNoReturn") {
    this->builtins.libStdNoReturn = true;
  } else if (name == "libStdarg") {
    this->builtins.libStdarg = true;
  } else if (name == "libStdbool") {
    this->builtins.libStdbool = true;
  } else if (name == "libStddef") {
    this->builtins.libStddef = true;
  } else if (name == "libStdint") {
    this->builtins.libStdint = true;
  } else if (name == "libStdio") {
    this->builtins.libStdio = true;
  } else if (name == "libStdlib") {
    this->builtins.libStdlib = true;
  } else if (name == "libString") {
    this->builtins.libString = true;
  } else if (name == "libSysSocket") {
    this->builtins.libSysSocket = true;
  } else if (name == "libSysStat") {
    this->builtins.libSysStat = true;
  } else if (name == "libSysTypes") {
    this->builtins.libSysTypes = true;
  } else if (name == "libSysUtsname") {
    this->builtins.libSysUtsname = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libTime") {
    this->builtins.libTime = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libUnistd") {
    this->builtins.libUnistd = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libWinDirect") {
    this->builtins.libWinDirect = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libWinIo") {
    this->builtins.libWinIo = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libWinWs2tcpip") {
    this->builtins.libWinWs2tcpip = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libWindows") {
    this->builtins.libWindows = true;
    this->_activateBuiltin("definitions");
  } else if (name == "libWinsock2") {
    this->builtins.libWinsock2 = true;
    this->_activateBuiltin("definitions");
  } else if (name == "typeAny") {
    this->builtins.typeAny = true;
    this->_activateBuiltin("libStdlib");
  } else if (name == "typeBuffer") {
    this->builtins.typeBuffer = true;
    this->_activateBuiltin("libStdlib");
  } else if (name == "typeErrStack") {
    this->builtins.typeErrStack = true;
  } else if (name == "typeErrState") {
    this->builtins.typeErrState = true;
    this->_activateBuiltin("libSetJmp");
    this->_activateBuiltin("typeErrStack");
  } else if (name == "typeRequest") {
    this->builtins.typeRequest = true;
    this->_activateBuiltin("definitions");
    this->_activateBuiltin("libOpensslSsl");
    this->_activateBuiltin("libWinsock2");
  } else if (name == "typeStr") {
    this->builtins.typeStr = true;
    this->_activateBuiltin("libStdlib");
  } else if (name == "typeThreadpool") {
    this->builtins.typeThreadpool = true;
    this->_activateBuiltin("libPthread");
    this->_activateBuiltin("libStdbool");
    this->_activateBuiltin("typeThreadpoolJob");
    this->_activateBuiltin("typeThreadpoolThread");
  } else if (name == "typeThreadpoolFunc") {
    this->builtins.typeThreadpoolFunc = true;
    this->_activateBuiltin("typeThreadpool");
    this->_activateBuiltin("typeThreadpoolJob");
  } else if (name == "typeThreadpoolJob") {
    this->builtins.typeThreadpoolJob = true;
    this->_activateBuiltin("libStdbool");
  } else if (name == "typeThreadpoolThread") {
    this->builtins.typeThreadpoolThread = true;
    this->_activateBuiltin("libPthread");
  } else if (name == "typeWinReparseDataBuffer") {
    this->builtins.typeWinReparseDataBuffer = true;
    this->_activateBuiltin("definitions");
    this->_activateBuiltin("libWindows");
  } else if (name == "varEnviron") {
    this->builtins.varEnviron = true;
    this->_activateBuiltin("libUnistd");
  } else if (name == "varErrState") {
    this->builtins.varErrState = true;
    this->_activateBuiltin("typeErrState");
  } else if (name == "varLibOpensslInit") {
    this->builtins.varLibOpensslInit = true;
    this->_activateBuiltin("libStdbool");
  } else if (name == "varLibWs2Init") {
    this->builtins.varLibWs2Init = true;
    this->_activateBuiltin("libStdbool");
  } else if (this->api.contains(name)) {
    this->api[name].enabled = true;

    for (const auto &dep : this->api[name].dependencies) {
      auto existsInEntities = false;

      for (const auto &entity : this->entities) {
        if (entity.name == dep) {
          existsInEntities = true;
          break;
        }
      }

      if (existsInEntities) {
        this->_activateEntity(dep);
      } else {
        this->_activateBuiltin(dep);
      }
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

      for (const auto &builtin : entity.builtins) {
        this->_activateBuiltin(builtin);
      }

      for (const auto &child : entity.entities) {
        this->_activateEntity(child);
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
      auto sameAsSelfName = selfName == name;

      if (!sameAsSelfName && codegenMetadata.contains(name)) {
        for (const auto &dependency : codegenMetadata.at(name)) {
          if (dependencies == std::nullopt) {
            this->_activateBuiltin(dependency);
          } else if (!(*dependencies)->contains(dependency)) {
            (*dependencies)->emplace(dependency);
          }
        }
      } else if (!sameAsSelfName && this->api.contains(name)) {
        if (dependencies == std::nullopt) {
          this->_activateBuiltin(name);
        } else if (!(*dependencies)->contains(name)) {
          (*dependencies)->emplace(name);
        }
      } else if (!sameAsSelfName && name.starts_with("TYPE_") && this->ast->typeMap.has(name.substr(5))) {
        name = this->_typeDef(this->ast->typeMap.get(name.substr(5)));

        if (dependencies == std::nullopt) {
          this->_activateEntity(name);
        } else if (!(*dependencies)->contains(name)) {
          (*dependencies)->emplace(name);
        }
      } else if (!sameAsSelfName && name.starts_with("array_")) {
        if (name.ends_with("_free")) {
          name = this->_typeNameArray(this->ast->typeMap.createArr(this->ast->typeMap.get(name.substr(6, name.size() - 11)))) + "_free";
        } else {
          name = this->_typeNameArray(this->ast->typeMap.createArr(this->ast->typeMap.get(name.substr(6))));
        }

        if (dependencies == std::nullopt) {
          this->_activateEntity(name);
        } else if (!(*dependencies)->contains(name)) {
          (*dependencies)->emplace(name);
        }
      } else if (!sameAsSelfName && (name.starts_with("map_") || name.starts_with("pair_"))) {
        auto isPair = name.starts_with("pair_");
        auto nameSliced = isPair ? name.substr(5) : name.substr(4);
        auto keyType = this->ast->typeMap.get(nameSliced.substr(0, nameSliced.find('_')));
        auto valueType = this->ast->typeMap.get(nameSliced.substr(nameSliced.find('_') + 1));

        name = this->_typeNameMap(this->ast->typeMap.createMap(keyType, valueType));

        if (isPair) {
          name = Codegen::typeName("pair_" + name.substr(Codegen::typeName("map_").size()));
        }

        if (dependencies == std::nullopt) {
          this->_activateEntity(name);
        } else if (!(*dependencies)->contains(name)) {
          (*dependencies)->emplace(name);
        }
      } else if (!sameAsSelfName) {
        auto existsInEntities = false;

        for (const auto &entity : this->entities) {
          if (entity.name == name) {
            existsInEntities = true;
            break;
          }
        }

        if (!existsInEntities) {
          throw Error("can't find code generator entity `" + name + "`");
        }

        if (dependencies == std::nullopt) {
          this->_activateEntity(name);
        } else if (!(*dependencies)->contains(name)) {
          (*dependencies)->emplace(name);
        }
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

std::string Codegen::_genCopyFn (Type *type, const std::string &code) {
  auto initialState = this->state;
  auto result = code;

  if (type->isAlias()) {
    result = this->_genCopyFn(std::get<TypeAlias>(type->body).type, code);
  } else if (type->isAny()) {
    result = this->_apiEval("_{any_copy}(" + result + ")", 1);
  } else if (type->isObj() && type->builtin && type->codeName == "@buffer_Buffer") {
    result = this->_apiEval("_{buffer_copy}(" + result + ")", 1);
  } else if (
    type->isArray() ||
    type->isFn() ||
    type->isMap() ||
    type->isObj() ||
    type->isOpt() ||
    type->isUnion()
  ) {
    auto typeInfo = this->_typeInfo(type);

    this->_activateEntity(typeInfo.realTypeName + "_copy");
    result = typeInfo.realTypeName + "_copy(" + result + ")";
  } else if (type->isStr()) {
    result = this->_apiEval("_{str_copy}(" + result + ")", 1);
  }

  return result;
}

std::string Codegen::_genEqFn (Type *type, const std::string &leftCode, const std::string &rightCode, bool reverse) {
  auto initialState = this->state;
  auto realType = Type::real(type);
  auto direction = std::string(reverse ? "ne" : "eq");
  auto code = std::string();

  if (realType->isObj() && realType->builtin && realType->codeName == "@buffer_Buffer") {
    code = this->_apiEval("_{buffer_" + direction + "}(" + this->_genCopyFn(realType, leftCode) + ", " + this->_genCopyFn(realType, rightCode) + ")", 1);
  } else if (
    realType->isArray() ||
    realType->isMap() ||
    realType->isObj() ||
    realType->isOpt() ||
    realType->isUnion()
  ) {
    auto typeInfo = this->_typeInfo(realType);

    this->_activateEntity(typeInfo.realTypeName + "_" + direction);
    code = typeInfo.realTypeName + "_" + direction + "(";
    code += this->_genCopyFn(typeInfo.realType, leftCode) + ", ";
    code += this->_genCopyFn(typeInfo.realType, rightCode) + ")";
  } else if (realType->isStr()) {
    code = this->_apiEval("_{str_" + direction + "_str}(" + this->_genCopyFn(realType, leftCode) + ", " + this->_genCopyFn(realType, rightCode) + ")", 1);
  } else {
    code = leftCode + " " + (reverse ? "!=" : "==") + " " + rightCode;
  }

  return code;
}

std::string Codegen::_genFreeFn (Type *type, const std::string &code) {
  auto initialState = this->state;
  auto result = code;

  if (type->isAlias()) {
    result = this->_genFreeFn(std::get<TypeAlias>(type->body).type, code);
  } else if (type->isAny()) {
    result = this->_apiEval("_{any_free}((_{struct any}) " + result + ")", 2);
  } else if (type->isObj() && type->builtin && type->codeName == "@buffer_Buffer") {
    result = this->_apiEval("_{buffer_free}((_{struct buffer}) " + result + ")", 2);
  } else if (
    type->isArray() ||
    type->isFn() ||
    type->isMap() ||
    type->isObj() ||
    type->isOpt() ||
    type->isUnion()
  ) {
    auto typeInfo = this->_typeInfo(type);

    this->_activateEntity(typeInfo.realTypeName + "_free");
    result = typeInfo.realTypeName + "_free((" + typeInfo.realTypeCodeTrimmed + ") " + result + ")";
  } else if (type->isStr()) {
    result = this->_apiEval("_{str_free}((_{struct str}) " + result + ")", 2);
  }

  return result;
}

std::string Codegen::_genReallocFn (Type *type, const std::string &leftCode, const std::string &rightCode) {
  auto initialState = this->state;
  auto result = std::string();

  if (type->isAny()) {
    result = this->_apiEval(leftCode + " = _{any_realloc}(" + leftCode + ", " + rightCode + ")", 1);
  } else if (type->isObj() && type->builtin && type->codeName == "@buffer_Buffer") {
    result = this->_apiEval(leftCode + " = _{buffer_realloc}(" + leftCode + ", " + rightCode + ")", 1);
  } else if (
    type->isArray() ||
    type->isFn() ||
    type->isMap() ||
    type->isObj() ||
    type->isOpt() ||
    type->isUnion()
  ) {
    auto typeInfo = this->_typeInfo(type);

    this->_activateEntity(typeInfo.typeName + "_realloc");
    result = leftCode + " = " + typeInfo.typeName + "_realloc(" + leftCode + ", " + rightCode + ")";
  } else if (type->isStr()) {
    result = this->_apiEval(leftCode + " = _{str_realloc}(" + leftCode + ", " + rightCode + ")", 1);
  }

  return result;
}

std::string Codegen::_genStrFn (Type *type, const std::string &code, bool copy, bool escape) {
  auto initialState = this->state;
  auto realType = Type::real(type);
  auto result = code;

  if (realType->isAny()) {
    result = this->_apiEval("_{any_str}(" + (copy ? this->_genCopyFn(realType, result) : result) + ")", 1);
  } else if (realType->isObj() && realType->builtin && realType->codeName == "@buffer_Buffer") {
    result = this->_apiEval("_{buffer_str}(" + (copy ? this->_genCopyFn(realType, result) : result) + ")", 1);
  } else if (
    realType->isArray() ||
    realType->isFn() ||
    realType->isMap() ||
    realType->isObj() ||
    realType->isOpt() ||
    realType->isUnion()
  ) {
    auto typeInfo = this->_typeInfo(realType);

    this->_activateEntity(typeInfo.realTypeName + "_str");
    result = typeInfo.realTypeName + "_str(" + (copy ? this->_genCopyFn(realType, result) : result) + ")";
  } else if (realType->isEnum()) {
    result = this->_apiEval("_{enum_str}(" + result + ")", 1);
  } else if (realType->isStr() && escape) {
    result = this->_apiEval("_{str_escape}(" + result + ")", 1);
  } else if (realType->isStr()) {
    result = copy ? this->_genCopyFn(realType, result) : result;
  } else {
    auto typeInfo = this->_typeInfo(realType);
    result = this->_apiEval("_{" + typeInfo.realTypeName + "_str}(" + result + ")", 1);
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
