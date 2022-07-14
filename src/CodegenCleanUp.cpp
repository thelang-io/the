/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "CodegenCleanUp.hpp"
#include "config.hpp"

void CodegenCleanUp::add (const std::string &content) {
  if (this->_data.empty() || this->_data.back().labelUsed) {
    this->_data.push_back({"L" + std::to_string(this->labelIdx), content});
    this->labelIdx++;
  } else {
    this->_data.back().content.insert(0, content + EOL);
  }
}

std::string CodegenCleanUp::currentLabel () {
  this->_data.back().labelUsed = true;
  return this->_data.back().label;
}

bool CodegenCleanUp::empty () const {
  return this->_data.empty();
}

std::string CodegenCleanUp::gen (std::size_t indent) const {
  auto result = std::string();

  for (auto it = this->_data.rbegin(); it != this->_data.rend(); it++) {
    auto next = static_cast<std::size_t>(0);
    auto last = static_cast<std::size_t>(0);
    auto item = *it;

    if (item.labelUsed) {
      result += item.label + ":" EOL;
    }

    while ((next = item.content.find(EOL, last)) != std::string::npos) {
      result += std::string(indent, ' ') + item.content.substr(last, next - last) + EOL;
      last = next + 1;
    }

    result += std::string(indent, ' ') + item.content.substr(last) + EOL;
  }

  return result;
}

CodegenCleanUp &CodegenCleanUp::update (std::size_t newLabelIdx) {
  this->labelIdx = newLabelIdx;
  return *this;
}
