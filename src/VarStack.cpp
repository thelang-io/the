/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "VarStack.hpp"

std::string Var::xml (std::size_t indent) const {
  if (this->builtin) {
    return std::string(indent, ' ') + R"(<BuiltinVar name=")" + this->name + R"(" />)";
  }

  auto result = std::string(indent, ' ') + R"(<Var builtin=")" + std::string(this->builtin ? "true" : "false");

  result += R"(" codeName=")" + this->codeName;
  result += R"(" frame=")" + std::to_string(this->frame);
  result += R"(" mut=")" + std::string(this->mut ? "true" : "false");
  result += R"(" name=")" + this->name + R"(">)" "\n";
  result += this->type->xml(indent + 2) + "\n";

  return result + std::string(indent, ' ') + "</Var>";
}

VarStack::VarStack (const std::vector<std::shared_ptr<Var>> &items) {
  this->_items.reserve(items.size());

  for (const auto &item : items) {
    this->_items.emplace_back(std::make_tuple(item, false));
  }
}

void VarStack::mark (const std::shared_ptr<Var> &var) {
  for (auto &item : this->_items) {
    if (std::get<0>(item)->codeName == var->codeName) {
      std::get<1>(item) = true;
    }
  }
}

std::vector<std::shared_ptr<Var>> VarStack::snapshot () const {
  auto result = std::vector<std::shared_ptr<Var>>{};

  for (auto &item : this->_items) {
    if (std::get<1>(item)) {
      result.push_back(std::get<0>(item));
    }
  }

  return result;
}
