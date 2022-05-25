/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_VAR_HPP
#define SRC_VAR_HPP

#include "Type.hpp"

struct Var {
  std::string name;
  std::string codeName;
  Type *type;
  bool mut;
  bool builtin;
  std::size_t frame;

  std::string xml (std::size_t = 0) const;
};

#endif
