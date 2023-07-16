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

#include "codegen-ast/CodegenAST.hpp"
#include "AST.hpp"
#include "ASTChecker.hpp"
#include "CodegenAPIItem.hpp"
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
  bool libPthread = false;
  bool libPwd = false;
  bool libSetJmp = false;
  bool libStdNoReturn = false;
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
  bool typeErrStack = false;
  bool typeErrState = false;
  bool typeRequest = false;
  bool typeStr = false;
  bool typeThreadpool = false;
  bool typeThreadpoolFunc = false;
  bool typeThreadpoolJob = false;
  bool typeThreadpoolThread = false;
  bool typeWinReparseDataBuffer = false;
  bool varEnviron = false;
  bool varErrState = false;
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
  bool insideAsync = false;
  std::map<std::string, Type *> typeCasts = {};
  std::size_t asyncCounter = 0;
  std::shared_ptr<std::size_t> asyncCounterLoopBreak = nullptr;
  std::shared_ptr<std::size_t> asyncCounterLoopContinue = nullptr;
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

struct CodegenStateBuiltinsEntities {
  std::optional<std::vector<std::string> *> builtins = std::nullopt;
  std::optional<std::vector<std::string> *> entities = std::nullopt;
};

class Codegen {
 public:
  std::map<std::string, CodegenAPIItem> api = {};
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
  std::vector<CodegenStateBuiltinsEntities> builtinsEntitiesBuffer = {};
  std::vector<std::string> bufferBuiltins = {};
  std::vector<std::string> bufferEntities = {};
  bool async = false;
  bool throws = false;

  static void compile (const std::string &, const std::tuple<std::string, std::vector<std::string>> &, const std::string &, bool = false);

  // todo test
  template <typename T>
  static std::size_t countAsyncLoopDepth (const std::vector<ASTNode> &nodes, std::size_t prevDepth) {
    auto depth = prevDepth;

    for (const auto &node : nodes) {
      auto flattenNodes = ASTChecker::flattenNode({ node });

      for (const auto &flattenNode : flattenNodes) {
        if (std::holds_alternative<ASTNodeLoop>(*flattenNode.body)) {
          auto nodeLoop = std::get<ASTNodeLoop>(*flattenNode.body);

          if (ASTChecker(nodeLoop.body).has<T>()) {
            auto newDepth = Codegen::countAsyncLoopDepth<T>(nodeLoop.body, prevDepth + 1);

            if (newDepth > depth) {
              depth = newDepth;
            }
          }
        }
      }
    }

    return depth;
  }

  // todo test
  static std::vector<std::shared_ptr<Var>> filterAsyncDeclarations (const std::vector<ASTNode> &);
  static std::string getEnvVar (const std::string &);
  static std::string name (const std::string &);
  static std::string stringifyFlags (const std::vector<std::string> &);
  static std::string typeName (const std::string &);

  explicit Codegen (AST *);

  virtual std::tuple<std::string, std::vector<std::string>> gen ();

 private:
  Codegen (const Codegen &);
  Codegen &operator= (const Codegen &);

  std::string _ (const std::string &, const std::optional<std::set<std::string> *> & = std::nullopt);
  int _apiEntity (const std::string &, CodegenEntityType, const std::optional<std::function<int (std::string &, std::string &)>> & = std::nullopt);
  std::string _apiEval (const std::string &, int = 0, const std::string & = "", const std::optional<std::set<std::string> *> & = std::nullopt);
  void _activateBuiltin (const std::string &, std::optional<std::vector<std::string> *> = std::nullopt);
  void _activateEntity (const std::string &, std::optional<std::vector<std::string> *> = std::nullopt);
  void _block (std::shared_ptr<CodegenASTStmt> *, const ASTBlock &, bool = true, const std::shared_ptr<CodegenASTStmt> & = nullptr, bool = false);
  void _blockAsync (std::shared_ptr<CodegenASTStmt> *, const ASTBlock &, bool = true, const std::shared_ptr<CodegenASTStmt> & = nullptr, bool = false);
  std::tuple<std::map<std::string, Type *>, std::map<std::string, Type *>> _evalTypeCasts (const ASTNodeExpr &, const ASTNode &);
  std::shared_ptr<CodegenASTExpr> _exprAccess (const ASTNodeExpr &, Type *, const ASTNode &, std::shared_ptr<CodegenASTStmt> *, bool);
  std::shared_ptr<CodegenASTExpr> _exprArray (const ASTNodeExpr &, Type *, const ASTNode &, std::shared_ptr<CodegenASTStmt> *, bool);
  std::shared_ptr<CodegenASTExpr> _exprAssign (const ASTNodeExpr &, Type *, const ASTNode &, std::shared_ptr<CodegenASTStmt> *, bool);
  std::shared_ptr<CodegenASTExpr> _exprAwait (const ASTNodeExpr &, Type *, const ASTNode &, std::shared_ptr<CodegenASTStmt> *, bool);
  std::shared_ptr<CodegenASTExpr> _exprBinary (const ASTNodeExpr &, Type *, const ASTNode &, std::shared_ptr<CodegenASTStmt> *, bool);
  std::shared_ptr<CodegenASTExpr> _exprCall (const ASTNodeExpr &, Type *, const ASTNode &, std::shared_ptr<CodegenASTStmt> *, bool, std::size_t);
  std::shared_ptr<CodegenASTExpr> _exprCallDefaultArg (const CodegenTypeInfo &);
  std::shared_ptr<CodegenASTExpr> _exprCallPrintArg (const CodegenTypeInfo &, const ASTNodeExpr &, const ASTNode &, std::shared_ptr<CodegenASTStmt> *);
  std::string _exprCallPrintArgSign (const CodegenTypeInfo &, const ASTNodeExpr &);
  std::shared_ptr<CodegenASTExpr> _exprCond (const ASTNodeExpr &, Type *, const ASTNode &, std::shared_ptr<CodegenASTStmt> *, bool);
  std::shared_ptr<CodegenASTExpr> _exprIs (const ASTNodeExpr &, Type *, const ASTNode &, std::shared_ptr<CodegenASTStmt> *, bool);
  std::shared_ptr<CodegenASTExpr> _exprLit (const ASTNodeExpr &, Type *, bool);
  std::shared_ptr<CodegenASTExpr> _exprMap (const ASTNodeExpr &, Type *, const ASTNode &, std::shared_ptr<CodegenASTStmt> *, bool);
  std::shared_ptr<CodegenASTExpr> _exprObj (const ASTNodeExpr &, Type *, const ASTNode &, std::shared_ptr<CodegenASTStmt> *, bool);
  std::shared_ptr<CodegenASTExpr> _exprObjDefaultField (const CodegenTypeInfo &);
  std::shared_ptr<CodegenASTExpr> _exprRef (const ASTNodeExpr &, Type *, const ASTNode &, std::shared_ptr<CodegenASTStmt> *, bool);
  std::shared_ptr<CodegenASTExpr> _exprUnary (const ASTNodeExpr &, Type *, const ASTNode &, std::shared_ptr<CodegenASTStmt> *, bool);
  void _fnDecl (std::shared_ptr<CodegenASTStmt> *, std::shared_ptr<Var>, const std::vector<std::shared_ptr<Var>> &, const std::vector<ASTFnDeclParam> &, const std::optional<ASTBlock> &, const ASTNode &, CodegenPhase);
  void _fnDeclInitErrorHandling (std::shared_ptr<CodegenASTStmt> *, const std::string &);
  std::shared_ptr<CodegenASTStmt> _genAsyncReturn (const std::shared_ptr<std::size_t> &);
  std::shared_ptr<CodegenASTExpr> _genCopyFn (Type *, const std::shared_ptr<CodegenASTExpr> &);
  std::shared_ptr<CodegenASTExpr> _genEqFn (Type *, const std::shared_ptr<CodegenASTExpr> &, const std::shared_ptr<CodegenASTExpr> &, bool = false);
  std::shared_ptr<CodegenASTExpr> _genErrState (bool, bool, const std::string & = "");
  std::shared_ptr<CodegenASTExpr> _genFreeFn (Type *, const std::shared_ptr<CodegenASTExpr> &);
  std::shared_ptr<CodegenASTExpr> _genReallocFn (Type *, const std::shared_ptr<CodegenASTExpr> &, const std::shared_ptr<CodegenASTExpr> &);
  std::shared_ptr<CodegenASTExpr> _genStrFn (Type *, const std::shared_ptr<CodegenASTExpr> &, bool = true, bool = true);
  void _node (std::shared_ptr<CodegenASTStmt> *, const ASTNode &, CodegenPhase = CODEGEN_PHASE_FULL);
  void _nodeAsync (std::shared_ptr<CodegenASTStmt> *, const ASTNode &, CodegenPhase = CODEGEN_PHASE_FULL);
  void _nodeBreak (std::shared_ptr<CodegenASTStmt> *, const ASTNode &);
  void _nodeBreakAsync (std::shared_ptr<CodegenASTStmt> *, const ASTNode &);
  void _nodeContinue (std::shared_ptr<CodegenASTStmt> *, const ASTNode &);
  void _nodeContinueAsync (std::shared_ptr<CodegenASTStmt> *, const ASTNode &);
  void _nodeEnumDecl (std::shared_ptr<CodegenASTStmt> *, const ASTNode &, CodegenPhase);
  void _nodeExprDecl (std::shared_ptr<CodegenASTStmt> *, const ASTNode &);
  std::shared_ptr<CodegenASTExpr> _nodeExpr (const ASTNodeExpr &, Type *, const ASTNode &, std::shared_ptr<CodegenASTStmt> *, bool = false, std::size_t = 0);
  void _nodeFnDecl (std::shared_ptr<CodegenASTStmt> *, const ASTNode &, CodegenPhase);
  void _nodeIf (std::shared_ptr<CodegenASTStmt> *, const ASTNode &);
  void _nodeIfAsync (std::shared_ptr<CodegenASTStmt> *, const ASTNode &);
  void _nodeLoop (std::shared_ptr<CodegenASTStmt> *, const ASTNode &);
  void _nodeLoopAsync (std::shared_ptr<CodegenASTStmt> *, const ASTNode &);
  void _nodeMain (std::shared_ptr<CodegenASTStmt> *, const ASTNode &);
  void _nodeMainAsync (std::shared_ptr<CodegenASTStmt> *, const ASTNode &);
  void _nodeObjDecl (std::shared_ptr<CodegenASTStmt> *, const ASTNode &, CodegenPhase);
  void _nodeReturn (std::shared_ptr<CodegenASTStmt> *, const ASTNode &);
  void _nodeReturnAsync (std::shared_ptr<CodegenASTStmt> *, const ASTNode &);
  void _nodeThrow (std::shared_ptr<CodegenASTStmt> *, const ASTNode &);
  void _nodeThrowAsync (std::shared_ptr<CodegenASTStmt> *, const ASTNode &);
  void _nodeTry (std::shared_ptr<CodegenASTStmt> *, const ASTNode &);
  void _nodeTryAsync (std::shared_ptr<CodegenASTStmt> *, const ASTNode &);
  void _nodeVarDecl (std::shared_ptr<CodegenASTStmt> *, const ASTNode &);
  void _nodeVarDeclAsync (std::shared_ptr<CodegenASTStmt> *, const ASTNode &);
  std::shared_ptr<CodegenASTExpr> _nodeVarDeclInit (const CodegenTypeInfo &);
  void _restoreStateBuiltinsEntities ();
  void _saveStateBuiltinsEntities ();
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
  std::shared_ptr<CodegenASTExpr> _wrapNodeExpr (const ASTNodeExpr &, Type *, bool, const std::shared_ptr<CodegenASTExpr> &);
};

#endif
