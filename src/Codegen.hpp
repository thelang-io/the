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

struct CodegenPolyfills {
  bool pow = false;
  bool printf = false;
};

struct Codegen {
  VarMap *varMap;
  std::vector<std::string> stack = {};
  CodegenPolyfills polyfills = {};
  std::string functionDeclarationsCode = "";
  std::string topLevelStatementsCode = "";
  std::string functionDefinitionsCode = "";
  std::string mainBodyCode = "";

  ~Codegen ();
};

std::string codegen (const AST *ast);

#endif
