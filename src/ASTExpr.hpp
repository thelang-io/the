/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_AST_EXPR_HPP
#define SRC_AST_EXPR_HPP

#include <optional>
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
  AST_EXPR_UNARY_INCREMENT,
  AST_EXPR_UNARY_LOGICAL_NOT,
  AST_EXPR_UNARY_NEGATION,
  AST_EXPR_UNARY_PLUS
};

struct ASTExprAccess;
struct ASTExprAssign;
struct ASTExprBinary;
struct ASTExprCall;
struct ASTExprCond;
struct ASTExprLit;
struct ASTExprObj;
struct ASTExprUnary;
struct ASTMember;

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

struct ASTNodeExpr {
  Type *type;
  std::shared_ptr<ASTExpr> body;
  bool parenthesized = false;

  bool isLit () const;
  std::string litBody () const;
  std::string xml (std::size_t = 0) const;
};

using ASTMemberObj = std::variant<std::shared_ptr<Var>, ASTMember>;

struct ASTMember {
  std::shared_ptr<ASTMemberObj> obj;
  std::string prop;
};

struct ASTExprAccess {
  std::shared_ptr<ASTMemberObj> body;

  std::string xml (std::size_t = 0) const;
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
  Type *calleeType;
  std::vector<ASTExprCallArg> args;
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

struct ASTExprObjProp {
  std::string id;
  ASTNodeExpr init;
};

struct ASTExprObj {
  Type *type;
  std::vector<ASTExprObjProp> props;
};

struct ASTExprUnary {
  ASTNodeExpr arg;
  ASTExprUnaryOp op;
  bool prefix = false;
};

#endif
