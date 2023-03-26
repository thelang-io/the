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

#ifndef SRC_CODEGEN_HPP
#define SRC_CODEGEN_HPP

#include <map>
#include <set>
#include "AST.hpp"
#include "CodegenCleanUp.hpp"

enum CodegenEntityType {
  CODEGEN_ENTITY_DEF,
  CODEGEN_ENTITY_ENUM,
  CODEGEN_ENTITY_FN,
  CODEGEN_ENTITY_OBJ
};

enum CodegenPhase {
  CODEGEN_PHASE_ALLOC,
  CODEGEN_PHASE_ALLOC_METHOD,
  CODEGEN_PHASE_INIT,
  CODEGEN_PHASE_FULL
};

struct CodegenApiItem {
  bool enabled = false;
  std::string name;
  std::string decl = "";
  std::string def = "";
  std::set<std::string> dependencies = {};
};

struct CodegenBuiltins {
  bool definitions = false;
  bool externSystemFunction036 = false;
  bool libArpaInet = false;
  bool libCtype = false;
  bool libDirent = false;
  bool libErrno = false;
  bool libFcntl = false;
  bool libFloat = false;
  bool libInttypes = false;
  bool libMath = false;
  bool libNetdb = false;
  bool libNetinetIn = false;
  bool libOpensslSsl = false;
  bool libOpensslRand = false;
  bool libPwd = false;
  bool libStdarg = false;
  bool libStdbool = false;
  bool libStddef = false;
  bool libStdint = false;
  bool libStdio = false;
  bool libStdlib = false;
  bool libString = false;
  bool libSysSocket = false;
  bool libSysStat = false;
  bool libSysTypes = false;
  bool libSysUtsname = false;
  bool libTime = false;
  bool libUnistd = false;
  bool libWinDirect = false;
  bool libWinIo = false;
  bool libWinWs2tcpip = false;
  bool libWindows = false;
  bool libWinsock2 = false;
  bool typeAny = false;
  bool typeBuffer = false;
  bool typeRequest = false;
  bool typeStr = false;
  bool typeWinReparseDataBuffer = false;
  bool varEnviron = false;
  bool varLibOpensslInit = false;
  bool varLibWs2Init = false;
};

struct CodegenEntity {
  std::string name;
  CodegenEntityType type;
  std::vector<std::string> builtins = {};
  std::vector<std::string> entities = {};
  std::string decl = "";
  std::string def = "";
  bool active = false;
};

struct CodegenState {
  std::optional<std::vector<std::string> *> builtins = std::nullopt;
  std::optional<std::vector<std::string> *> entities = std::nullopt;
  CodegenCleanUp cleanUp;
  std::set<std::string> contextVars = {};
  Type *returnType = nullptr;
  std::map<std::string, Type *> typeCasts = {};
};

struct CodegenTypeInfo {
  Type *type;
  std::string typeName;
  std::string typeCode;
  std::string typeCodeConst;
  std::string typeCodeTrimmed;
  std::string typeCodeConstTrimmed;
  std::string typeRefCode;
  std::string typeRefCodeConst;
  Type *realType;
  std::string realTypeName;
  std::string realTypeCode;
  std::string realTypeCodeConst;
  std::string realTypeCodeTrimmed;
  std::string realTypeCodeConstTrimmed;
  std::string realTypeRefCode;
  std::string realTypeRefCodeConst;
};

struct CodegenTypeInfoItem {
  Type *type;
  std::string typeName;
  std::string typeCode;
  std::string typeCodeConst;
  std::string typeCodeTrimmed;
  std::string typeCodeConstTrimmed;
  std::string typeRefCode;
  std::string typeRefCodeConst;
};

class Codegen {
 public:
  std::map<std::string, CodegenApiItem> api = {};
  AST *ast;
  Reader *reader;
  CodegenState state;
  VarMap varMap;
  std::map<std::string, const Type *> anyTypes;
  CodegenBuiltins builtins = {};
  std::vector<CodegenEntity> entities;
  std::vector<std::string> flags;
  std::size_t indent = 0;
  std::size_t lastTypeIdx = 1;
  bool needMainArgs = false;

  static void compile (
    const std::string &,
    const std::tuple<std::string, std::vector<std::string>> &,
    const std::string &,
    bool = false
  );
  static std::string getEnvVar (const std::string &);
  static std::string name (const std::string &);
  static std::string stringifyFlags (const std::vector<std::string> &);
  static std::string typeName (const std::string &);

  explicit Codegen (AST *);

  virtual std::tuple<std::string, std::vector<std::string>> gen ();

 private:
  Codegen (const Codegen &);
  Codegen &operator= (const Codegen &);

  int _apiEntity (
    const std::string &,
    CodegenEntityType,
    const std::optional<std::function<int (std::string &, std::string &)>> & = std::nullopt
  );
  std::string _apiEval (
    const std::string &,
    int = 0,
    const std::string & = "",
    const std::optional<std::set<std::string> *> & = std::nullopt
  );
  void _apiDecl (const std::vector<std::string> &);
  void _apiDef (const std::vector<std::string> &);
  void _activateBuiltin (const std::string &, std::optional<std::vector<std::string> *> = std::nullopt);
  void _activateEntity (const std::string &, std::optional<std::vector<std::string> *> = std::nullopt);
  std::string _block (const ASTBlock &, bool = true);
  std::tuple<std::map<std::string, Type *>, std::map<std::string, Type *>> _evalTypeCasts (const ASTNodeExpr &);
  std::string _exprAssign (const ASTExprAssign &);
  std::string _exprCallDefaultArg (const CodegenTypeInfo &);
  std::string _exprCallPrintArg (const CodegenTypeInfo &, const ASTNodeExpr &);
  std::string _exprCallPrintArgSign (const CodegenTypeInfo &, const ASTNodeExpr &);
  std::string _exprObjDefaultField (const CodegenTypeInfo &);
  std::string _fnDecl (
    Type *,
    const std::string &,
    const std::vector<std::shared_ptr<Var>> &,
    const std::vector<ASTFnDeclParam> &,
    const std::optional<ASTBlock> &,
    CodegenPhase
  );
  std::string _genCopyFn (Type *, const std::string &);
  std::string _genEqFn (Type *, const std::string &, const std::string &, bool = false);
  std::string _genFreeFn (Type *, const std::string &);
  std::string _genReallocFn (Type *, const std::string &, const std::string &);
  std::string _genStrFn (Type *, const std::string &, bool = true, bool = true);
  std::string _node (const ASTNode &, bool = true, CodegenPhase = CODEGEN_PHASE_FULL);
  std::string _nodeExpr (const ASTNodeExpr &, Type *, bool = false);
  std::string _nodeVarDeclInit (const CodegenTypeInfo &);
  std::string _type (Type *);
  std::string _typeDef (Type *);
  std::string _typeDefIdx (Type *);
  CodegenTypeInfo _typeInfo (Type *);
  CodegenTypeInfoItem _typeInfoItem (Type *);
  std::string _typeNameAny (Type *);
  std::string _typeNameArray (Type *);
  std::string _typeNameFn (Type *);
  std::string _typeNameMap (Type *);
  std::string _typeNameObj (Type *);
  std::string _typeNameObjDef (Type *, const std::map<std::string, std::string> & = {});
  std::string _typeNameOpt (Type *);
  std::string _typeNameUnion (Type *);
  std::string _wrapNode (const ASTNode &, const std::string &);
  std::string _wrapNodeExpr (const ASTNodeExpr &, Type *, bool, const std::string &);
};

#endif
