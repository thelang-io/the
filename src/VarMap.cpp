/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "Error.hpp"
#include "VarMap.hpp"

VarMapItem::~VarMapItem () {
  if (this->type == VAR_FN) {
    for (auto param : this->fn->params) {
      delete param;
    }

    delete this->fn;
  } else if (this->type == VAR_OBJ) {
    for (auto field : this->obj->fields) {
      delete field;
    }

    delete this->obj;
  }
}

VarMap::~VarMap () {
  for (auto it : this->items) {
    delete it;
  }
}

void VarMap::add (VarMapItemType type, const std::string &name) {
  this->items.push_back(new VarMapItem{type, name, nullptr, nullptr, this->frame});
}

void VarMap::addFn (
  const std::string &name,
  const std::string &hiddenName,
  VarMapItemType returnType,
  const std::vector<VarMapFnParam *> &params,
  std::size_t optionalParams
) {
  auto fn = new VarMapFn{hiddenName, returnType, params, optionalParams};
  this->items.push_back(new VarMapItem{VAR_FN, name, fn, nullptr, this->frame});
}

void VarMap::addObj (
  const std::string &name,
  const std::string &hiddenName,
  const std::vector<VarMapObjField *> &fields
) {
  auto obj = new VarMapObj{hiddenName, fields};
  this->items.push_back(new VarMapItem{VAR_OBJ, name, nullptr, obj, this->frame});
}

std::string VarMap::genId (const std::vector<std::string> &stack, const std::string &id) const {
  auto result = std::string();

  for (auto it : stack) {
    result += (it.substr(0, 6) == "__THE_" ? it.substr(6) : it) + "_SD_";
  }

  result = "__THE_" + result + (id.substr(0, 6) == "__THE_" ? id.substr(6) : id) + "_";

  for (auto idx = static_cast<std::size_t>(0);; idx++) {
    auto name = result + std::to_string(idx);
    auto exists = false;

    for (auto it : this->items) {
      if (it->name == name) {
        exists = true;
        break;
      }
    }

    if (!exists) {
      result = name;
      break;
    }
  }

  return result;
}

const VarMapItem *VarMap::get (const std::string &name) const {
  for (auto it : this->items) {
    if (
      it->name == name ||
      (it->type == VAR_FN && it->fn->hiddenName == name && it->frame != -1) ||
      (it->type == VAR_OBJ && it->obj->hiddenName == name)
    ) {
      return it;
    }
  }

  throw Error("Tried to access non existing VarMap item");
}

const VarMapItem *VarMap::getFn (const std::string &name) const {
  for (auto it : this->items) {
    if (it->type == VAR_FN && it->fn->hiddenName == name && it->frame != -1) {
      return it;
    }
  }

  throw Error("Tried to access non existing VarMap function");
}

const VarMapItem *VarMap::getObj (const std::string &name) const {
  for (auto it : this->items) {
    if (it->type == VAR_OBJ && it->obj->hiddenName == name) {
      return it;
    }
  }

  throw Error("Tried to access non existing VarMap object");
}

void VarMap::restore () {
  for (auto it = this->items.begin(); it != this->items.end();) {
    auto item = *it;

    if (item->frame == this->frame && item->type == VAR_FN) {
      item->frame = -1;
    } else if (item->frame == this->frame) {
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
