/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "CodegenCleanUp.hpp"
#include "Error.hpp"
#include "config.hpp"

CodegenCleanUp::CodegenCleanUp (CodegenCleanUp *p, bool inherit) {
  this->parent = p;
  this->labelIdx = this->parent->labelIdx;

  if (inherit) {
    this->parentHasCleanUp = this->parent->parentHasCleanUp || !this->parent->_data.empty();
    this->returnVarUsed = this->parent->returnVarUsed;
    this->valueVarUsed = this->parent->valueVarUsed;
  }
}

void CodegenCleanUp::add (const std::string &content) {
  if (this->_data.empty() || this->_data.back().labelUsed) {
    this->_data.push_back({"L" + std::to_string(this->labelIdx), content});
    this->_setLabelIdx(this->labelIdx + 1);
  } else {
    this->_data.back().content.insert(0, content + EOL);
  }
}

std::string CodegenCleanUp::currentLabel () {
  if (!this->empty()) {
    this->_data.back().labelUsed = true;
    return this->_data.back().label;
  } else if (this->parent == nullptr) {
    throw Error("Error: tried getting current label on nullptr in CodegenCleanUp");
  }

  return this->parent->currentLabel();
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

CodegenCleanUp &CodegenCleanUp::update (const CodegenCleanUp &child) {
  this->returnVarUsed = child.returnVarUsed;
  this->valueVarUsed = child.valueVarUsed;

  return *this;
}

void CodegenCleanUp::_setLabelIdx (std::size_t newLabelIdx) {
  this->labelIdx = newLabelIdx;

  if (this->parent != nullptr) {
    this->parent->_setLabelIdx(newLabelIdx);
  }
}
