/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_CODEGEN_HPP
#define SRC_CODEGEN_HPP

#include <set>
#include "AST.hpp"
#include "CodegenCleanUp.hpp"

enum CodegenEntityType {
  CODEGEN_ENTITY_FN,
  CODEGEN_ENTITY_OBJ
};

struct CodegenBuiltins {
  bool fnAlloc = false;
  bool fnBoolStr = false;
  bool fnByteStr = false;
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
  bool fnPrint = false;
  bool fnStrAlloc = false;
  bool fnStrConcatCstr = false;
  bool fnStrConcatStr = false;
  bool fnStrCopy = false;
  bool fnStrEqCstr = false;
  bool fnStrEqStr = false;
  bool fnStrEscape = false;
  bool fnStrFree = false;
  bool fnStrNeCstr = false;
  bool fnStrNeStr = false;
  bool fnStrNot = false;
  bool fnStrRealloc = false;
  bool fnU8Str = false;
  bool fnU16Str = false;
  bool fnU32Str = false;
  bool fnU64Str = false;
  bool libInttypes = false;
  bool libStdarg = false;
  bool libStdbool = false;
  bool libStdint = false;
  bool libStdio = false;
  bool libStdlib = false;
  bool libString = false;
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
};

class Codegen {
 public:
  AST *ast;
  CodegenBuiltins builtins = {};
  std::vector<std::string> flags;
  std::size_t indent = 0;
  Reader *reader;
  VarMap varMap;
  std::vector<CodegenEntity> entities;
  CodegenState state;
  std::vector<std::tuple<std::size_t, const Type *>> functions = {};

  static void compile (const std::string &, const std::tuple<std::string, std::string> &, bool = false);
  static std::string name (const std::string &);
  static std::string typeName (const std::string &);

  explicit Codegen (AST *);

  virtual std::tuple<std::string, std::string> gen ();

 private:
  Codegen (const Codegen &);
  Codegen &operator= (const Codegen &);

  void _activateBuiltin (const std::string &, std::optional<std::vector<std::string> *> = std::nullopt);
  void _activateEntity (const std::string &, std::optional<std::vector<std::string> *> = std::nullopt);
  std::string _block (const ASTBlock &, bool = true);
  std::string _exprAccess (const std::shared_ptr<ASTMemberObj> &);
  std::string _flags () const;
  std::string _node (const ASTNode &, bool = true);
  std::string _nodeIf (const ASTNodeIf &);
  std::string _nodeExpr (const ASTNodeExpr &, bool = false);
  std::string _type (const Type *, bool, bool = false);
  std::string _typeNameFn (const Type *);
  std::string _wrapNode (const ASTNode &, const std::string &) const;
  std::string _wrapNodeExpr (const ASTNodeExpr &, const std::string &) const;
};

#endif
