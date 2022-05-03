/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_CODEGEN_HPP
#define SRC_CODEGEN_HPP

#include "AST.hpp"

struct CodegenBuiltins {
  bool fnAlloc = false;
  bool fnCstrConcatStr = false;
  bool fnStrConcatCstr = false;
  bool fnStrConcatStr = false;
  bool fnStrCopy = false;
  bool fnStrDeinit = false;
  bool fnStrInit = false;
  bool fnStrReinit = false;
  bool libMath = false;
  bool libStdbool = false;
  bool libStddef = false;
  bool libStdint = false;
  bool libStdio = false;
  bool libStdlib = false;
  bool libString = false;
  bool typeStr = false;
};

class Codegen {
 public:
  AST *ast;
  CodegenBuiltins builtins = {};
  std::vector<std::string> flags;
  std::size_t indent = 0;
  Reader *reader;
  VarMap varMap;

  static void compile (const std::string &, const std::tuple<std::string, std::string> &);
  static std::string name (const std::string &);

  explicit Codegen (AST *);

  virtual std::tuple<std::string, std::string> gen ();

 private:
  Codegen (const Codegen &);
  Codegen &operator= (const Codegen &);

  std::string _block (const ASTBlock &);
  std::string _exprAccess (const ASTMemberObj &);
  std::string _flags () const;
  std::tuple<std::string, std::string, std::string> _node (const ASTNode &);
  std::tuple<std::string, std::string, std::string> _nodeExpr (const std::shared_ptr<ASTNodeExpr> &, bool = false);
  std::string _type (const std::shared_ptr<Type> &, bool);
};

#endif
