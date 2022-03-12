/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "ASTNode.hpp"

std::string ASTNode::xml (std::size_t indent) const {
  return std::string(indent, ' ');
}
