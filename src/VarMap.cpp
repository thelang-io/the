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
    } else if (it->type == VAR_OBJ) {
      for (auto field : it->obj->fields) {
        delete field;
      }

      delete it->obj;
    }

    delete it;
  }
}

void VarMap::add (VarMapItemType type, const std::string &name) {
  this->items.push_back(new VarMapItem{type, name, nullptr, this->frame, nullptr});
}

void VarMap::addFn (
  const std::string &name,
  VarMapItemType returnType,
  const std::vector<VarMapFnParam *> &params,
  std::size_t optionalParams
) {
  auto fn = new VarMapFn{returnType, params, optionalParams};
  this->items.push_back(new VarMapItem{VAR_FN, name, fn, this->frame, nullptr});
}

void VarMap::addObj (const std::string &name, const std::vector<VarMapObjField *> &fields) {
  auto obj = new VarMapObj{fields};
  this->items.push_back(new VarMapItem{VAR_OBJ, name, nullptr, this->frame, obj});
}

const VarMapItem &VarMap::get (const std::string &name) const {
  for (auto it : this->items) {
    if (it->name == name) {
      return *it;
    }
  }

  throw Error("Tried to access non existing VarMap item");
}

const VarMapFn &VarMap::getFn (const std::string &name) const {
  for (auto it : this->items) {
    if (it->type == VAR_FN && it->name == name) {
      return *it->fn;
    }
  }

  throw Error("Tried to access non existing VarMap function");
}

const VarMapObj &VarMap::getObj (const std::string &name) const {
  for (auto it : this->items) {
    if (it->type == VAR_OBJ && it->name == name) {
      return *it->obj;
    }
  }

  throw Error("Tried to access non existing VarMap object");
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
      } else if (item->type == VAR_OBJ) {
        for (auto field : item->obj->fields) {
          delete field;
        }

        delete item->obj;
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
