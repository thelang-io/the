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
  CODEGEN_ENTITY_FN,
  CODEGEN_ENTITY_OBJ
};

enum CodegenPhase {
  CODEGEN_PHASE_ALLOC,
  CODEGEN_PHASE_INIT,
  CODEGEN_PHASE_FULL
};

struct CodegenApiItem {
  bool enabled = false;
  std::string name;
  std::string decl;
  std::string def;
  std::set<std::string> dependencies;
};

struct CodegenBuiltins {
  bool definitions = false;
  bool fnAlloc = false;
  bool fnAnyCopy = false;
  bool fnAnyFree = false;
  bool fnAnyRealloc = false;
  bool fnAnyStr = false;
  bool fnBoolStr = false;
  bool fnBufferCopy = false;
  bool fnBufferEq = false;
  bool fnBufferFree = false;
  bool fnBufferNe = false;
  bool fnBufferRealloc = false;
  bool fnBufferStr = false;
  bool fnByteStr = false;
  bool fnCharIsAlpha = false;
  bool fnCharIsAlphaNum = false;
  bool fnCharIsDigit = false;
  bool fnCharIsSpace = false;
  bool fnCharRepeat = false;
  bool fnCharStr = false;
  bool fnCstrConcatStr = false;
  bool fnCstrEqCstr = false;
  bool fnCstrEqStr = false;
  bool fnCstrNeCstr = false;
  bool fnCstrNeStr = false;
  bool fnF32Str = false;
  bool fnF64Str = false;
  bool fnFloatStr = false;
  bool fnI8Str = false;
  bool fnI16Str = false;
  bool fnI32Str = false;
  bool fnI64Str = false;
  bool fnIntStr = false;
  bool fnOSName = false;
  bool fnPathBasename = false;
  bool fnPathDirname = false;
  bool fnPrint = false;
  bool fnReAlloc = false;
  bool fnSleepSync = false;
  bool fnStrAlloc = false;
  bool fnStrAt = false;
  bool fnStrConcatCstr = false;
  bool fnStrConcatStr = false;
  bool fnStrCopy = false;
  bool fnStrEqCstr = false;
  bool fnStrEqStr = false;
  bool fnStrEscape = false;
  bool fnStrFind = false;
  bool fnStrFree = false;
  bool fnStrLen = false;
  bool fnStrLines = false;
  bool fnStrLower = false;
  bool fnStrLowerFirst = false;
  bool fnStrNeCstr = false;
  bool fnStrNeStr = false;
  bool fnStrNot = false;
  bool fnStrRealloc = false;
  bool fnStrSlice = false;
  bool fnStrToBuffer = false;
  bool fnStrTrim = false;
  bool fnStrUpper = false;
  bool fnStrUpperFirst = false;
  bool fnU8Str = false;
  bool fnU16Str = false;
  bool fnU32Str = false;
  bool fnU64Str = false;
  bool libCtype = false;
  bool libDirent = false;
  bool libInttypes = false;
  bool libStdarg = false;
  bool libStdbool = false;
  bool libStddef = false;
  bool libStdint = false;
  bool libStdio = false;
  bool libStdlib = false;
  bool libString = false;
  bool libSysStat = false;
  bool libSysUtsname = false;
  bool libUnistd = false;
  bool libWinDirect = false;
  bool libWinIo = false;
  bool libWindows = false;
  bool typeAny = false;
  bool typeBuffer = false;
  bool typeStr = false;
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

class Codegen {
 public:
  std::map<std::string, CodegenApiItem> api = {};
  std::map<std::string, std::string> metadata = {};
  AST *ast;
  Reader *reader;
  CodegenState state;
  VarMap varMap;
  std::map<std::string, const Type *> anyTypes;
  CodegenBuiltins builtins = {};
  std::vector<CodegenEntity> entities;
  std::vector<std::string> flags;
  std::size_t indent = 0;
  std::size_t lastAnyIdx = 1;
  bool needMainArgs = false;

  static void compile (const std::string &, const std::tuple<std::string, std::string> &, const std::string &, bool = false);
  static std::string name (const std::string &);
  static std::string typeName (const std::string &);

  explicit Codegen (AST *);

  virtual std::tuple<std::string, std::string> gen ();

 private:
  Codegen (const Codegen &);
  Codegen &operator= (const Codegen &);

  void _apiLoad (const std::vector<std::string> &);
  // todo add limit param
  std::string _apiEval (const std::string &, const std::optional<std::set<std::string> *> & = std::nullopt);
  void _apiMetadata ();
  void _activateBuiltin (const std::string &, std::optional<std::vector<std::string> *> = std::nullopt);
  void _activateEntity (const std::string &, std::optional<std::vector<std::string> *> = std::nullopt);
  std::string _block (const ASTBlock &, bool = true);
  std::tuple<std::map<std::string, Type *>, std::map<std::string, Type *>> _evalTypeCasts (const ASTNodeExpr &);
  std::string _flags () const;
  std::string _genCopyFn (
    Type *,
    const std::string &,
    const std::optional<std::vector<std::string> *> & = std::nullopt,
    const std::optional<std::vector<std::string> *> & = std::nullopt
  );
  std::string _genEqFn (
    Type *,
    const std::string &,
    const std::string &,
    const std::optional<std::vector<std::string> *> & = std::nullopt,
    const std::optional<std::vector<std::string> *> & = std::nullopt,
    bool = false
  );
  std::string _genFreeFn (
    Type *,
    const std::string &,
    const std::optional<std::vector<std::string> *> & = std::nullopt,
    const std::optional<std::vector<std::string> *> & = std::nullopt
  );
  std::string _genReallocFn (
    Type *,
    const std::string &,
    const std::string &,
    const std::optional<std::vector<std::string> *> & = std::nullopt,
    const std::optional<std::vector<std::string> *> & = std::nullopt
  );
  std::string _genStrFn (
    Type *,
    const std::string &,
    const std::optional<std::vector<std::string> *> & = std::nullopt,
    const std::optional<std::vector<std::string> *> & = std::nullopt,
    bool = true,
    bool = true
  );
  std::string _node (const ASTNode &, bool = true, CodegenPhase phase = CODEGEN_PHASE_FULL);
  std::string _nodeExpr (const ASTNodeExpr &, Type *, bool = false);
  std::string _type (const Type *);
  CodegenTypeInfo _typeInfo (Type *);
  std::string _typeNameAny (Type *);
  std::string _typeNameArray (const Type *);
  std::string _typeNameFn (const Type *);
  std::string _typeNameOpt (const Type *);
  void _typeObj (Type *, bool = false);
  std::string _wrapNode (const ASTNode &, const std::string &);
  std::string _wrapNodeExpr (const ASTNodeExpr &, Type *, bool, const std::string &);
};

#endif
