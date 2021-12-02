/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_CODEGEN_HPP
#define SRC_CODEGEN_HPP

#include "AST.hpp"
#include "VarMap.hpp"

struct CodegenHeaders {
  bool boolean = false;
  bool math = false;
  bool stdio = false;
};

struct CodegenFnDecl {
  std::string hiddenName;
  const StmtFnDecl *stmt;
};

struct Codegen {
  CodegenHeaders headers;
  std::string body;
  std::vector<CodegenFnDecl *> fnDecls;
  VarMap *varMap;

  ~Codegen ();
};

std::string codegen (const AST *ast);

#endif
