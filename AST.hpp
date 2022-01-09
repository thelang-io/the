/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_AST_HPP
#define SRC_AST_HPP

#include "Reader.hpp"
#include "VarMap.hpp"

enum ASTExprAssignOp {
  AST_EXPR_ASSIGN_ADD,
  AST_EXPR_ASSIGN_BITWISE_AND,
  AST_EXPR_ASSIGN_BITWISE_OR,
  AST_EXPR_ASSIGN_BITWISE_XOR,
  AST_EXPR_ASSIGN_COALESCE,
  AST_EXPR_ASSIGN_DIVIDE,
  AST_EXPR_ASSIGN_EQUAL,
  AST_EXPR_ASSIGN_LEFT_SHIFT,
  AST_EXPR_ASSIGN_LOGICAL_AND,
  AST_EXPR_ASSIGN_LOGICAL_OR,
  AST_EXPR_ASSIGN_MULTIPLY,
  AST_EXPR_ASSIGN_POWER,
  AST_EXPR_ASSIGN_REMAINDER,
  AST_EXPR_ASSIGN_RIGHT_SHIFT,
  AST_EXPR_ASSIGN_SUBTRACT
};

enum ASTExprBinaryOp {
  AST_EXPR_BINARY_ADD,
  AST_EXPR_BINARY_BITWISE_AND,
  AST_EXPR_BINARY_BITWISE_OR,
  AST_EXPR_BINARY_BITWISE_XOR,
  AST_EXPR_BINARY_COALESCE,
  AST_EXPR_BINARY_DIVIDE,
  AST_EXPR_BINARY_EQUAL,
  AST_EXPR_BINARY_GREATER_EQUAL,
  AST_EXPR_BINARY_GREATER_THAN,
  AST_EXPR_BINARY_LEFT_SHIFT,
  AST_EXPR_BINARY_LESS_EQUAL,
  AST_EXPR_BINARY_LESS_THAN,
  AST_EXPR_BINARY_LOGICAL_AND,
  AST_EXPR_BINARY_LOGICAL_OR,
  AST_EXPR_BINARY_MULTIPLY,
  AST_EXPR_BINARY_NOT_EQUAL,
  AST_EXPR_BINARY_POWER,
  AST_EXPR_BINARY_REMAINDER,
  AST_EXPR_BINARY_RIGHT_SHIFT,
  AST_EXPR_BINARY_SUBTRACT
};

enum ASTExprLitType {
  AST_EXPR_LIT_BOOL,
  AST_EXPR_LIT_CHAR,
  AST_EXPR_LIT_FLOAT,
  AST_EXPR_LIT_INT_BIN,
  AST_EXPR_LIT_INT_DEC,
  AST_EXPR_LIT_INT_HEX,
  AST_EXPR_LIT_INT_OCT,
  AST_EXPR_LIT_STR
};

enum ASTExprUnaryOp {
  AST_EXPR_UNARY_BITWISE_NOT,
  AST_EXPR_UNARY_DECREMENT,
  AST_EXPR_UNARY_DOUBLE_LOGICAL_NOT,
  AST_EXPR_UNARY_INCREMENT,
  AST_EXPR_UNARY_LOGICAL_NOT,
  AST_EXPR_UNARY_NEGATION,
  AST_EXPR_UNARY_PLUS
};

struct AST;
struct ASTExprAssign;
struct ASTExprBinary;
struct ASTExprCall;
struct ASTExprCond;
struct ASTExprLit;
struct ASTExprObj;
struct ASTExprUnary;
struct ASTId;
struct ASTMember;
struct ASTNodeBreak;
struct ASTNodeContinue;
struct ASTNodeExpr;
struct ASTNodeFnDecl;
struct ASTNodeIf;
struct ASTNodeLoop;
struct ASTNodeMain;
struct ASTNodeObjDecl;
struct ASTNodeReturn;
struct ASTNodeVarDecl;

using ASTExprAccess = std::variant<ASTId, ASTMember>;

using ASTExpr = std::variant<
  ASTExprAccess,
  ASTExprAssign,
  ASTExprBinary,
  ASTExprCall,
  ASTExprCond,
  ASTExprLit,
  ASTExprObj,
  ASTExprUnary
>;

using ASTNode = std::variant<
  ASTNodeBreak,
  ASTNodeContinue,
  ASTNodeExpr,
  ASTNodeFnDecl,
  ASTNodeIf,
  ASTNodeLoop,
  ASTNodeMain,
  ASTNodeObjDecl,
  ASTNodeReturn,
  ASTNodeVarDecl
>;

using ASTBlock = std::vector<ASTNode>;
using ASTNodeIfCond = std::variant<ASTBlock, ASTNodeIf>;

struct AST {
  ASTBlock nodes;
  FnMap fnMap;
  VarMap varMap;
  TypeMap typeMap;
};

struct ASTId {
  Var v;
};

struct ASTMember {
  std::shared_ptr<ASTExprAccess> obj;
  std::string prop;
};

struct ASTNodeExpr {
  std::shared_ptr<ASTExpr> expr;
  std::shared_ptr<Type> type;
  bool parenthesized = false;
};

struct ASTExprAssign {
  ASTExprAccess left;
  ASTExprAssignOp op;
  ASTNodeExpr right;
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
  ASTExprAccess callee;
  std::shared_ptr<Fn> fn;
  std::vector<ASTExprCallArg> args;
};

struct ASTExprCond {
  ASTNodeExpr cond;
  ASTNodeExpr body;
  ASTNodeExpr alt;
};

struct ASTExprLit {
  ASTExprLitType type;
  std::string val;
};

struct ASTExprObj {
  std::shared_ptr<Type> obj;
  std::map<std::string, ASTNodeExpr> props;
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

struct ASTNodeFnDeclParam {
  std::shared_ptr<Type> type;
  std::optional<ASTNodeExpr> init;
};

struct ASTNodeFnDecl {
  std::shared_ptr<Fn> fn;
  std::map<std::string, ASTNodeFnDeclParam> params;
  ASTBlock body;
};

struct ASTNodeIf {
  ASTNodeExpr cond;
  ASTBlock body;
  std::optional<std::shared_ptr<ASTNodeIfCond>> alt;
};

struct ASTNodeLoop {
  std::optional<std::shared_ptr<ASTNode>> init;
  std::optional<ASTNodeExpr> cond;
  std::optional<ASTNodeExpr> upd;
  ASTBlock body;
};

struct ASTNodeMain {
  ASTBlock body;
};

struct ASTNodeObjDecl {
  std::string name;
  std::shared_ptr<Type> obj;
};

struct ASTNodeReturn {
  ASTNodeExpr arg;
};

struct ASTNodeVarDecl {
  Var v;
  std::optional<ASTNodeExpr> init;
};

AST analyze (Reader *);

#endif
