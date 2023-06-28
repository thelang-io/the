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
  std::size_t countAsyncLoopDepth (const std::vector<ASTNode> &nodes, std::size_t prevDepth) {
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

  int _apiEntity (const std::string &, CodegenEntityType, const std::optional<std::function<int (std::string &, std::string &)>> & = std::nullopt);
  std::string _apiEval (const std::string &, int = 0, const std::string & = "", const std::optional<std::set<std::string> *> & = std::nullopt);
  void _activateBuiltin (const std::string &, std::optional<std::vector<std::string> *> = std::nullopt);
  void _activateEntity (const std::string &, std::optional<std::vector<std::string> *> = std::nullopt);
  CodegenASTStmt &_block (CodegenASTStmt &, ASTBlock &, bool = true, const std::string & = "", bool = false);
  CodegenASTStmt &_blockAsync (CodegenASTStmt &, ASTBlock &, bool = true, const std::string & = "", bool = false);
  std::tuple<std::map<std::string, Type *>, std::map<std::string, Type *>> _evalTypeCasts (const ASTNodeExpr &, const ASTNode &);
  std::string _exprAccess (const ASTNodeExpr &, Type *, const ASTNode &, std::string &, bool);
  std::string _exprArray (const ASTNodeExpr &, Type *, const ASTNode &, std::string &, bool);
  std::string _exprAssign (const ASTNodeExpr &, Type *, const ASTNode &, std::string &, bool);
  std::string _exprAwait (const ASTNodeExpr &, Type *, const ASTNode &, std::string &, bool);
  std::string _exprBinary (const ASTNodeExpr &, Type *, const ASTNode &, std::string &, bool);
  std::string _exprCall (const ASTNodeExpr &, Type *, const ASTNode &, std::string &, bool, std::size_t);
  std::string _exprCallDefaultArg (const CodegenTypeInfo &);
  std::string _exprCallPrintArg (const CodegenTypeInfo &, const ASTNodeExpr &, const ASTNode &, std::string &);
  std::string _exprCallPrintArgSign (const CodegenTypeInfo &, const ASTNodeExpr &);
  std::string _exprCond (const ASTNodeExpr &, Type *, const ASTNode &, std::string &, bool);
  std::string _exprIs (const ASTNodeExpr &, Type *, const ASTNode &, std::string &, bool);
  std::string _exprLit (const ASTNodeExpr &, Type *, const ASTNode &, std::string &, bool);
  std::string _exprMap (const ASTNodeExpr &, Type *, const ASTNode &, std::string &, bool);
  std::string _exprObj (const ASTNodeExpr &, Type *, const ASTNode &, std::string &, bool);
  std::string _exprObjDefaultField (const CodegenTypeInfo &);
  std::string _exprRef (const ASTNodeExpr &, Type *, const ASTNode &, std::string &, bool);
  std::string _exprUnary (const ASTNodeExpr &, Type *, const ASTNode &, std::string &, bool);
  std::string _fnDecl (std::shared_ptr<Var>, const std::vector<std::shared_ptr<Var>> &, const std::vector<ASTFnDeclParam> &, std::optional<ASTBlock> &, const ASTNode &, CodegenPhase);
  std::string _genCopyFn (Type *, const std::string &);
  std::string _genEqFn (Type *, const std::string &, const std::string &, bool = false);
  std::string _genFreeFn (Type *, const std::string &);
  std::string _genReallocFn (Type *, const std::string &, const std::string &);
  std::string _genStrFn (Type *, const std::string &, bool = true, bool = true);
  CodegenASTStmt &_node (CodegenASTStmt &, const ASTNode &, bool = true, CodegenPhase = CODEGEN_PHASE_FULL);
  CodegenASTStmt &_nodeAsync (CodegenASTStmt &, const ASTNode &, bool = true, CodegenPhase = CODEGEN_PHASE_FULL);
  CodegenASTStmt &_nodeBreak (CodegenASTStmt &, const ASTNode &);
  CodegenASTStmt &_nodeBreakAsync (CodegenASTStmt &, const ASTNode &);
  CodegenASTStmt &_nodeContinue (CodegenASTStmt &, const ASTNode &);
  CodegenASTStmt &_nodeContinueAsync (CodegenASTStmt &, const ASTNode &);
  CodegenASTStmt &_nodeEnumDecl (CodegenASTStmt &, const ASTNode &, bool, CodegenPhase);
  CodegenASTStmt &_nodeExprDecl (CodegenASTStmt &, const ASTNode &, bool, CodegenPhase);
  CodegenASTStmt &_nodeExprDeclAsync (CodegenASTStmt &, const ASTNode &, bool, CodegenPhase);
  CodegenASTExpr _nodeExpr (const ASTNodeExpr &, Type *, const ASTNode &, CodegenASTStmt &, bool = false, std::size_t = 0);
  CodegenASTStmt &_nodeFnDecl (CodegenASTStmt &, const ASTNode &, bool, CodegenPhase);
  CodegenASTStmt &_nodeIf (CodegenASTStmt &, const ASTNode &);
  CodegenASTStmt &_nodeIfAsync (CodegenASTStmt &, const ASTNode &);
  CodegenASTStmt &_nodeLoop (CodegenASTStmt &, const ASTNode &);
  CodegenASTStmt &_nodeLoopAsync (CodegenASTStmt &, const ASTNode &);
  CodegenASTStmt &_nodeMain (CodegenASTStmt &, const ASTNode &, bool, CodegenPhase);
  CodegenASTStmt &_nodeMainAsync (CodegenASTStmt &, const ASTNode &, bool, CodegenPhase);
  CodegenASTStmt &_nodeObjDecl (CodegenASTStmt &, const ASTNode &, bool, CodegenPhase);
  CodegenASTStmt &_nodeReturn (CodegenASTStmt &, const ASTNode &);
  CodegenASTStmt &_nodeReturnAsync (CodegenASTStmt &, const ASTNode &);
  CodegenASTStmt &_nodeThrow (CodegenASTStmt &, const ASTNode &, bool, CodegenPhase);
  CodegenASTStmt &_nodeThrowAsync (CodegenASTStmt &, const ASTNode &, bool, CodegenPhase);
  CodegenASTStmt &_nodeTry (CodegenASTStmt &, const ASTNode &, bool, CodegenPhase);
  CodegenASTStmt &_nodeTryAsync (CodegenASTStmt &, const ASTNode &, bool, CodegenPhase);
  CodegenASTStmt &_nodeVarDecl (CodegenASTStmt &, const ASTNode &, bool, CodegenPhase);
  CodegenASTStmt &_nodeVarDeclAsync (CodegenASTStmt &, const ASTNode &, bool, CodegenPhase);
  std::string _nodeVarDeclInit (const CodegenTypeInfo &);
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
  std::string _wrapNode (const ASTNode &, bool, CodegenPhase, const std::string &);
  std::string _wrapNodeExpr (const ASTNodeExpr &, Type *, bool, const std::string &);
};

#endif
