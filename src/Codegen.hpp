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
  bool libMath = false;
  bool libStdbool = false;
  bool libStdint = false;
  bool typeInt = false;
};

class Codegen {
 public:
  AST *ast;
  CodegenBuiltins builtins = {};
  std::vector<std::string> flags;
  std::size_t indent = 0;
  Reader *reader;

  static std::string name (const std::string &, bool = false);

  explicit Codegen (AST *);

  virtual std::tuple<std::string, std::string> gen ();

 private:
  Codegen (const Codegen &);
  Codegen &operator= (const Codegen &);

  std::string _block (const ASTBlock &);
  std::string _exprAccess (const ASTMemberObj &);
  std::string _flags () const;
  std::tuple<std::string, std::string, std::string> _node (const ASTNode &);
  std::tuple<std::string, std::string, std::string> _nodeExpr (const std::shared_ptr<ASTNodeExpr> &);
  std::string _type (const std::shared_ptr<Type> &, bool);
};

#endif
