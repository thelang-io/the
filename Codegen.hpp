/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_CODEGEN_HPP
#define SRC_CODEGEN_HPP

#include "AST.hpp"

struct CodegenHeaders {
  bool math = false;
  bool stdbool = false;
  bool stdio = false;
  bool stdlib = false;
  bool string = false;
};

struct CodegenFunctionsStr {
  bool init = false;
  bool copy = false;
  bool deinit = false;
  bool to_cstr = false;

  bool concat = false;
};

struct CodegenFunctions {
  CodegenFunctionsStr s;
};

struct Codegen {
  CodegenHeaders headers;
  CodegenFunctions functions;
  std::size_t indent;
  std::string functionDeclarationsCode;
  std::string functionDefinitionsCode;
  std::string output;
  std::string structDeclarationsCode;
  std::string structDefinitionsCode;
};

Codegen codegen (AST *);

#endif
