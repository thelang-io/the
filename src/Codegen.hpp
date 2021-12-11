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
  bool math = false;
  bool stdbool = false;
  bool stdio = false;
  bool stdlib = false;
  bool string = false;
};

struct CodegenFunctions {
  bool str_init = false;
  bool str_from_cstr = false;
  bool str_deinit = false;
};

struct Codegen {
  VarMap *varMap;
  std::vector<std::string> stack = {};
  std::size_t indent = 0;
  CodegenHeaders headers = {};
  CodegenFunctions functions = {};
  std::string functionDeclarationsCode = "";
  std::string structDeclarationsCode = "";
  std::string topLevelStatementsCode = "";
  std::string functionDefinitionsCode = "";
  std::string mainBodyCode = "";

  ~Codegen ();
};

std::string codegen (const AST *ast);

#endif
