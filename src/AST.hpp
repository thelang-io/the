/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_AST_HPP
#define SRC_AST_HPP

#include "Parser.hpp"

class AST {
 public:
  bool mainPresent = false;
  Block *mainBody;

  ~AST ();
  void add (Stmt *stmt);
};

#endif
