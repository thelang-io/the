/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Var.hpp"
#include "config.hpp"

std::string Var::xml (std::size_t indent) const {
  if (this->builtin) {
    return std::string(indent, ' ') + R"(<BuiltinVar name=")" + this->name + R"(" />)";
  }

  auto attrs = R"( name=")" + this->name + R"(")";

  attrs += R"( codeName=")" + this->codeName + R"(")";
  attrs += this->mut ? " mut" : "";
  attrs += R"( frame=")" + std::to_string(this->frame) + R"(")";

  auto result = std::string(indent, ' ') + "<Var" + attrs + ">" EOL;
  result += this->type->xml(indent + 2) + EOL;
  return result + std::string(indent, ' ') + "</Var>";
}
