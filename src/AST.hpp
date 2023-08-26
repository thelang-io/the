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

#ifndef SRC_AST_HPP
#define SRC_AST_HPP

#include <map>
#include "Parser.hpp"
#include "VarMap.hpp"

class AST;
struct ASTExprAccess;
struct ASTExprArray;
struct ASTExprAs;
struct ASTExprAssign;
struct ASTExprAwait;
struct ASTExprBinary;
struct ASTExprCall;
struct ASTExprClosure;
struct ASTExprCond;
struct ASTExprIs;
struct ASTExprLit;
struct ASTExprMap;
struct ASTExprObj;
struct ASTExprRef;
struct ASTExprUnary;
struct ASTNodeBreak;
struct ASTNodeContinue;
struct ASTNodeEnumDecl;
struct ASTNodeExportDecl;
struct ASTNodeFnDecl;
struct ASTNodeIf;
struct ASTNodeImportDecl;
struct ASTNodeLoop;
struct ASTNodeMain;
struct ASTNodeObjDecl;
struct ASTNodeReturn;
struct ASTNodeThrow;
struct ASTNodeTry;
struct ASTNodeTypeDecl;
struct ASTNodeVarDecl;

using ASTExpr = std::variant<
  ASTExprAccess,
  ASTExprArray,
  ASTExprAs,
  ASTExprAssign,
  ASTExprAwait,
  ASTExprBinary,
  ASTExprCall,
  ASTExprClosure,
  ASTExprCond,
  ASTExprIs,
  ASTExprLit,
  ASTExprMap,
  ASTExprObj,
  ASTExprRef,
  ASTExprUnary
>;

struct ASTNodeExpr {
  Type *type;
  std::shared_ptr<ASTExpr> body;
  bool parenthesized = false;
  ReaderLocation start;
  ReaderLocation end;
  ASTNodeExpr *parent = nullptr;

  bool isLit () const;
  std::string litBody () const;
  std::string xml (std::size_t = 0) const;
};

using ASTNodeBody = std::variant<
  ASTNodeBreak,
  ASTNodeContinue,
  ASTNodeEnumDecl,
  ASTNodeExportDecl,
  ASTNodeExpr,
  ASTNodeFnDecl,
  ASTNodeIf,
  ASTNodeImportDecl,
  ASTNodeLoop,
  ASTNodeMain,
  ASTNodeObjDecl,
  ASTNodeReturn,
  ASTNodeThrow,
  ASTNodeTry,
  ASTNodeTypeDecl,
  ASTNodeVarDecl
>;

struct ASTNode {
  std::shared_ptr<ASTNodeBody> body;
  ASTNode *parent;
  ReaderLocation start;
  ReaderLocation end;

  std::string xml (std::size_t = 0) const;
};

using ASTBlock = std::vector<ASTNode>;

struct ASTCatchClause {
  ASTNode param;
  ASTBlock body;

  std::string xml (std::size_t = 0) const;
};

struct ASTFnParam {
  std::shared_ptr<Var> var;
  std::optional<ASTNodeExpr> init;
};

struct ASTObjProp {
  std::string name;
  ASTNodeExpr init;

  std::string xml (const std::string &, std::size_t = 0) const;
};

enum ASTExprAssignOp {
  AST_EXPR_ASSIGN_ADD,
  AST_EXPR_ASSIGN_AND,
  AST_EXPR_ASSIGN_BIT_AND,
  AST_EXPR_ASSIGN_BIT_OR,
  AST_EXPR_ASSIGN_BIT_XOR,
  AST_EXPR_ASSIGN_DIV,
  AST_EXPR_ASSIGN_EQ,
  AST_EXPR_ASSIGN_LSHIFT,
  AST_EXPR_ASSIGN_MOD,
  AST_EXPR_ASSIGN_MUL,
  AST_EXPR_ASSIGN_OR,
  AST_EXPR_ASSIGN_RSHIFT,
  AST_EXPR_ASSIGN_SUB
};

enum ASTExprBinaryOp {
  AST_EXPR_BINARY_ADD,
  AST_EXPR_BINARY_AND,
  AST_EXPR_BINARY_BIT_AND,
  AST_EXPR_BINARY_BIT_OR,
  AST_EXPR_BINARY_BIT_XOR,
  AST_EXPR_BINARY_DIV,
  AST_EXPR_BINARY_EQ,
  AST_EXPR_BINARY_GE,
  AST_EXPR_BINARY_GT,
  AST_EXPR_BINARY_LSHIFT,
  AST_EXPR_BINARY_LE,
  AST_EXPR_BINARY_LT,
  AST_EXPR_BINARY_MOD,
  AST_EXPR_BINARY_MUL,
  AST_EXPR_BINARY_NE,
  AST_EXPR_BINARY_OR,
  AST_EXPR_BINARY_RSHIFT,
  AST_EXPR_BINARY_SUB
};

enum ASTExprLitType {
  AST_EXPR_LIT_BOOL,
  AST_EXPR_LIT_CHAR,
  AST_EXPR_LIT_FLOAT,
  AST_EXPR_LIT_INT_BIN,
  AST_EXPR_LIT_INT_DEC,
  AST_EXPR_LIT_INT_HEX,
  AST_EXPR_LIT_INT_OCT,
  AST_EXPR_LIT_NIL,
  AST_EXPR_LIT_STR
};

enum ASTExprUnaryOp {
  AST_EXPR_UNARY_BIT_NOT,
  AST_EXPR_UNARY_DECREMENT,
  AST_EXPR_UNARY_INCREMENT,
  AST_EXPR_UNARY_MINUS,
  AST_EXPR_UNARY_NOT,
  AST_EXPR_UNARY_PLUS
};

struct ASTExprAccess {
  std::optional<std::variant<std::shared_ptr<Var>, ASTNodeExpr>> expr;
  std::optional<ASTNodeExpr> elem;
  std::optional<std::string> prop;
};

struct ASTExprArray {
  std::vector<ASTNodeExpr> elements;
};

struct ASTExprAs {
  ASTNodeExpr expr;
  Type *type;
};

struct ASTExprAssign {
  ASTNodeExpr left;
  ASTExprAssignOp op;
  ASTNodeExpr right;
};

struct ASTExprAwait {
  ASTNodeExpr arg;
  std::size_t id;
};

struct ASTExprBinary {
  ASTNodeExpr left;
  ASTExprBinaryOp op;
  ASTNodeExpr right;
};

struct ASTExprCallArg {
  std::optional<std::string> id;
  ASTNodeExpr expr;
};

struct ASTExprCall {
  ASTNodeExpr callee;
  std::vector<ASTExprCallArg> args;
};

struct ASTExprClosure {
  std::shared_ptr<Var> var;
  std::vector<std::shared_ptr<Var>> stack;
  std::vector<ASTFnParam> params;
  ASTBlock body;
};

struct ASTExprCond {
  ASTNodeExpr cond;
  ASTNodeExpr body;
  ASTNodeExpr alt;
};

struct ASTExprIs {
  ASTNodeExpr expr;
  Type *type;
};

struct ASTExprLit {
  ASTExprLitType type;
  std::string body;
};

struct ASTExprMap {
  std::vector<ASTObjProp> props;
};

struct ASTExprObj {
  std::vector<ASTObjProp> props;
};

struct ASTExprRef {
  ASTNodeExpr expr;
};

struct ASTExprUnary {
  ASTNodeExpr arg;
  ASTExprUnaryOp op;
  bool prefix = false;
};

struct ASTNodeBreak {
};

struct ASTNodeContinue {
};

struct ASTNodeEnumDeclMember {
  std::string id;
  std::optional<ASTNodeExpr> init;
};

struct ASTNodeEnumDecl {
  Type *type;
  std::vector<ASTNodeEnumDeclMember> members;
};

struct ASTNodeExportDecl {
  std::optional<ASTNode> declaration;
  Type *declarationType;
};

struct ASTNodeFnDecl {
  std::shared_ptr<Var> var;
  std::vector<std::shared_ptr<Var>> stack;
  std::vector<ASTFnParam> params;
  std::optional<ASTBlock> body;
};

struct ASTNodeIf {
  ASTNodeExpr cond;
  ASTBlock body;
  std::optional<std::variant<ASTBlock, ASTNode>> alt;
};

struct ASTNodeImportDeclSpecifier {
  std::optional<std::string> imported;
  std::string local;
};

struct ASTNodeImportDecl {
  std::vector<ASTNodeImportDeclSpecifier> specifiers;
  std::string source;
};

struct ASTNodeLoop {
  std::optional<ASTNode> init;
  std::optional<ASTNodeExpr> cond;
  std::optional<ASTNodeExpr> upd;
  ASTBlock body;
};

struct ASTNodeMain {
  std::vector<std::shared_ptr<Var>> stack;
  ASTBlock body;
  bool async;
};

struct ASTNodeObjDeclMethod {
  std::shared_ptr<Var> var;
  std::vector<std::shared_ptr<Var>> stack = {};
  std::vector<ASTFnParam> params = {};
  std::optional<ASTBlock> body = {};
};

struct ASTNodeObjDecl {
  Type *type;
  std::vector<ASTNodeObjDeclMethod> methods;
};

struct ASTNodeReturn {
  std::optional<ASTNodeExpr> body;
};

struct ASTNodeThrow {
  ASTNodeExpr arg;
};

struct ASTNodeTry {
  ASTBlock body;
  std::vector<ASTCatchClause> handlers;
};

struct ASTNodeTypeDecl {
  Type *type;
};

struct ASTNodeVarDecl {
  std::shared_ptr<Var> var;
  std::optional<ASTNodeExpr> init;
};

enum ASTPhase {
  AST_PHASE_ALLOC,
  AST_PHASE_INIT,
  AST_PHASE_FULL
};

struct ASTState {
  bool insideLoopInit = false;
  Type *returnType = nullptr;
};

struct ASTImport {
  std::size_t priority;
  std::shared_ptr<Reader> reader;
  std::shared_ptr<Lexer> lexer;
  std::shared_ptr<Parser> parser;
  std::shared_ptr<AST> ast;
  ASTBlock nodes = {};
};

class AST {
 public:
  std::string cwd;
  std::size_t priority;
  Parser *parser;
  Reader *reader;
  std::shared_ptr<std::vector<ASTImport>> imports;
  std::vector<std::size_t> deps;
  TypeMap typeMap;
  VarMap varMap;
  ASTState state;
  std::map<std::string, Type *> typeCasts = {};

  static std::string getExportCodeName (const ASTNode &);
  static std::string getExportName (const ASTNode &);
  static Type *getExportType (const ASTNode &);
  static std::shared_ptr<Var> getExportVar (const ASTNode &);
  static void populateExprAwaitId (ASTBlock &);
  static void populateParent (ASTNode &, ASTNode *);
  static void populateParentExpr (ASTNodeExpr &, ASTNodeExpr *, ASTNode *);
  static void populateParents (ASTBlock &, ASTNode * = nullptr);

  explicit AST (Parser *, const std::optional<std::string> & = std::nullopt, const std::shared_ptr<std::vector<ASTImport>> & = nullptr);
  virtual ~AST () = default;

  virtual ASTBlock gen ();
  virtual std::string xml ();

 private:
  AST (const AST &);
  AST &operator= (const AST &);

  ASTBlock _block (const ParserBlock &, VarStack &, bool = false);
  std::tuple<std::map<std::string, Type *>, std::map<std::string, Type *>> _evalTypeCasts (const ParserStmtExpr &);
  void _forwardNode (const ParserBlock &, ASTPhase);
  ASTNode _node (const ParserStmt &, VarStack &);
  ASTNodeExpr _nodeExpr (const ParserStmtExpr &, Type *, VarStack &);
  Type *_nodeExprType (const ParserStmtExpr &, Type *);
  Type *_type (const ParserType &);
  ASTNode _wrapNode (const ParserStmt &, const ASTNodeBody &);
  ASTNodeExpr _wrapNodeExpr (const ParserStmtExpr &, Type *, const ASTExpr &);
  Type *_wrapNodeExprType (const ParserStmtExpr &, Type *, Type *);
};

std::string exprLitTypeStr (ASTExprLitType);

#endif
