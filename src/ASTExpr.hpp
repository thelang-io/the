/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_AST_EXPR_HPP
#define SRC_AST_EXPR_HPP

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

struct ASTExprAccess;
struct ASTExprAssign;
struct ASTExprBinary;
struct ASTExprCall;
struct ASTExprCallArg;
struct ASTExprCond;
struct ASTExprLit;
struct ASTExprObj;
struct ASTExprObjProp;
struct ASTExprUnary;
struct ASTMember;
struct ASTNodeExpr;

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

using ASTMemberObj = std::variant<std::shared_ptr<Var>, ASTMember>;

struct ASTMember {
  std::shared_ptr<ASTMemberObj> obj;
  std::string prop;
};

struct ASTNodeExpr {
  std::shared_ptr<Type> type;
  std::shared_ptr<ASTExpr> body;
  bool parenthesized = false;
};

struct ASTExprAccess {
  ASTMemberObj body;
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

struct ASTExprCall {
  ASTExprAccess callee;
  std::shared_ptr<Type> calleeType;
  std::vector<ASTExprCallArg> args;
};

struct ASTExprCallArg {
  std::optional<std::string> id;
  ASTNodeExpr expr;
};

struct ASTExprCond {
  ASTNodeExpr cond;
  ASTNodeExpr body;
  ASTNodeExpr alt;
};

struct ASTExprLit {
  ASTExprLitType type;
  std::string body;
};

struct ASTExprObj {
  std::shared_ptr<Type> objType;
  std::vector<ASTExprObjProp> props;
};

struct ASTExprObjProp {
  std::string id;
  ASTNodeExpr init;
};

struct ASTExprUnary {
  ASTNodeExpr arg;
  ASTExprUnaryOp op;
  bool prefix = false;
};

#endif
