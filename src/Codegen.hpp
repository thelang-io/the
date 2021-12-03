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

struct Codegen {
  VarMap *varMap;
  std::vector<std::string> stack;
  CodegenHeaders headers;
  std::string topLevel;
  std::string main;
  std::string definitions;

  ~Codegen ();
};

std::string codegen (const AST *ast);

#endif
