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

struct CodegenFunctions {
  bool str_init = false;
  bool str_clone = false;
  bool str_from_cstr = false;
  bool str_deinit = false;
};

struct Codegen {
  CodegenHeaders headers;
  CodegenFunctions functions;
  std::size_t indent;
  std::string forwardStructDeclarationsCode;
  std::string functionDeclarationsCode;
  std::string functionDefinitionsCode;
  std::string output;
  std::string structDeclarationsCode;
};

Codegen codegen (AST *);

#endif
