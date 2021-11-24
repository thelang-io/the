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
  VAR_CHAR,
  VAR_FLOAT,
  VAR_FN,
  VAR_INT,
  VAR_STR
};

struct VarMapItemParam {
  std::string name;
  VarMapItemType type;
  bool required = false;
};

struct VarMapItemFn {
  VarMapItemType returnType;
  std::vector<VarMapItemParam *> params;
  std::size_t optionalParams;
};

struct VarMapItem {
  VarMapItemType type;
  std::string name;
  VarMapItemFn *fn;
  std::size_t frame = 0;
};

class VarMap {
 public:
  std::size_t frame = 0;
  std::vector<VarMapItem *> items;

  ~VarMap ();

  void add (VarMapItemType type, const std::string &name);

  void addFn (
    const std::string &name,
    VarMapItemType returnType,
    const std::vector<VarMapItemParam *> &params,
    std::size_t optionalParams
  );

  const VarMapItem &get (const std::string &name) const;
  const VarMapItemFn &getFn (const std::string &name) const;
  void restore ();
  void save ();
};

#endif
