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

  auto result = std::string(indent, ' ') + "<Var";

  result += this->codeName[0] == '@' ? "" : R"( codeName=")" + this->codeName + R"(")";
  result += R"( frame=")" + std::to_string(this->frame) + R"(")";
  result += R"( mut=")" + std::string(this->mut ? "true" : "false") + R"(")";
  result += this->name[0] == '@' ? "" : R"( name=")" + this->name + R"(")";
  result += ">" EOL + this->type->xml(indent + 2) + EOL;

  return result + std::string(indent, ' ') + "</Var>";
}
