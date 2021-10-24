/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Error.hpp"
#include "VarMap.hpp"

VarMap::~VarMap () {
  for (auto it : this->items) {
    if (it->type == VAR_FN) {
      for (auto param : it->fn->params) {
        delete param;
      }

      delete it->fn;
    }

    delete it;
  }
}

void VarMap::add (VarMapItemType type, const std::string &name) {
  this->items.push_back(new VarMapItem{type, name, nullptr, this->frame});
}

void VarMap::addFn (
  const std::string &name,
  const std::string &hiddenName,
  VarMapItemType returnType,
  const std::vector<VarMapItemParam *> &params
) {
  auto fn = new VarMapItemFn{hiddenName, returnType, params};
  this->items.push_back(new VarMapItem{VAR_FN, name, fn, this->frame});
}

const VarMapItem &VarMap::get (const std::string &name) const {
  for (auto it : this->items) {
    if (it->name == name) {
      return *it;
    }
  }

  throw Error("Tried to access non existing VarMap item");
}

const VarMapItemFn &VarMap::getFn (const std::string &name) const {
  for (auto it : this->items) {
    if (it->type == VAR_FN && it->name == name) {
      return *it->fn;
    }
  }

  throw Error("Tried to access non existing VarMap function item");
}

void VarMap::restore () {
  for (auto it = this->items.begin(); it != this->items.end();) {
    auto item = *it;

    if (item->frame == this->frame) {
      if (item->type == VAR_FN) {
        for (auto param : item->fn->params) {
          delete param;
        }

        delete item->fn;
      }

      delete item;
      this->items.erase(it);
      continue;
    }

    it++;
  }

  this->frame--;
}

void VarMap::save () {
  this->frame++;
}
