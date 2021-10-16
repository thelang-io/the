/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_AST_HPP
#define SRC_AST_HPP

#include "Stmt.hpp"

class AST {
 public:
  bool mainPresent = false;
  std::vector<Stmt> mainBody;

  void add (const Stmt &stmt);
};

#endif