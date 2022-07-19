/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "CodegenCleanUp.hpp"
#include "Error.hpp"
#include "config.hpp"

CodegenCleanUp::CodegenCleanUp (CodegenCleanUpType t, CodegenCleanUp *p) {
  this->type = t;
  this->parent = p;
  this->labelIdx = this->parent->labelIdx;
  this->breakVarIdx = this->parent->breakVarIdx;
}

void CodegenCleanUp::add (const std::string &content) {
  if (this->empty() || this->_data.back().labelUsed) {
    this->_data.push_back({"L" + std::to_string(this->labelIdx), content});
    this->labelIdx += 1;

    auto p = this->parent;

    while (p != nullptr) {
      p->labelIdx = this->labelIdx;
      p = p->parent;
    }
  } else {
    this->_data.back().content.insert(0, content + EOL);
  }
}

std::string CodegenCleanUp::currentBreakVar () {
  this->breakVarUsed = true;

  if (this->type == CODEGEN_CLEANUP_LOOP) {
    return "b" + std::to_string(this->breakVarIdx);
  } else if (this->parent == nullptr) {
    throw Error("Error: tried getting break var on nullptr in CodegenCleanUp");
  }

  return this->parent->currentBreakVar();
}

std::string CodegenCleanUp::currentLabel () {
  this->valueVarUsed = true;

  if (this->type == CODEGEN_CLEANUP_FN && this->empty()) {
    this->add("");
  }

  if (!this->empty()) {
    this->_data.back().labelUsed = true;
    return this->_data.back().label;
  } else if (this->parent == nullptr) {
    throw Error("Error: tried getting current label on nullptr in CodegenCleanUp");
  }

  return this->parent->currentLabel();
}

std::string CodegenCleanUp::currentReturnVar () {
  this->returnVarUsed = true;

  if (this->type == CODEGEN_CLEANUP_FN) {
    return "r";
  } else if (this->parent == nullptr) {
    throw Error("Error: tried getting return var on nullptr in CodegenCleanUp");
  }

  return this->parent->currentReturnVar();
}

std::string CodegenCleanUp::currentValueVar () {
  this->valueVarUsed = true;

  if (this->type == CODEGEN_CLEANUP_FN) {
    return "v";
  } else if (this->parent == nullptr) {
    throw Error("Error: tried getting value var on nullptr in CodegenCleanUp");
  }

  return this->parent->currentValueVar();
}

bool CodegenCleanUp::empty () const {
  return this->_data.empty();
}

std::string CodegenCleanUp::gen (std::size_t indent) const {
  auto result = std::string();

  for (auto it = this->_data.rbegin(); it != this->_data.rend(); it++) {
    auto item = *it;

    if (item.labelUsed) {
      result += item.label + ":" EOL;
    }

    if (!item.content.empty()) {
      auto next = static_cast<std::size_t>(0);
      auto last = static_cast<std::size_t>(0);

      while ((next = item.content.find(EOL, last)) != std::string::npos) {
        result += std::string(indent, ' ') + item.content.substr(last, next - last) + EOL;
        last = next + 1;
      }

      result += std::string(indent, ' ') + item.content.substr(last) + EOL;
    }
  }

  return result;
}

bool CodegenCleanUp::hasCleanUp (CodegenCleanUpType t) const {
  return !this->empty() || (this->type != t && this->parent != nullptr && this->parent->hasCleanUp(t));
}
