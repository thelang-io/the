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
  bool stdio = false;
};

struct Codegen {
  CodegenHeaders headers;
  std::string mainBody;
  VarMap *varMap;

  inline ~Codegen () {
    delete this->varMap;
  }
};

std::string codegen (const AST *ast);

#endif
