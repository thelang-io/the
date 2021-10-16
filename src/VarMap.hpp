/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_VAR_MAP_HPP
#define SRC_VAR_MAP_HPP

#include <string>
#include <vector>

enum VarMapItemType {
  varChar,
  varFloat,
  varIntDec,
  varStr
};

struct VarMapItem {
  VarMapItemType type;
  std::string name;
};

class VarMap {
 public:
  std::vector<VarMapItem> items;

  void add (VarMapItemType type, const std::string &name);
  const VarMapItem &get (const std::string &name) const;
};

#endif
