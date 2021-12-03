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
  VAR_BOOL,
  VAR_BYTE,
  VAR_CHAR,
  VAR_FLOAT,
  VAR_F32,
  VAR_F64,
  VAR_FN,
  VAR_INT,
  VAR_I8,
  VAR_I16,
  VAR_I32,
  VAR_I64,
  VAR_OBJ,
  VAR_STR,
  VAR_U8,
  VAR_U16,
  VAR_U32,
  VAR_U64
};

struct VarMapFnParam {
  std::string name;
  VarMapItemType type;
  bool required = false;
};

struct VarMapFn {
  std::string hiddenName;
  VarMapItemType returnType;
  std::vector<VarMapFnParam *> params;
  std::size_t optionalParams;
};

struct VarMapObjField {
  std::string name;
  VarMapItemType type;
};

struct VarMapObj {
  std::string hiddenName;
  std::vector<VarMapObjField *> fields;
};

struct VarMapItem {
  VarMapItemType type;
  std::string name;
  VarMapFn *fn;
  VarMapObj *obj;
  int frame;

  ~VarMapItem ();
};

class VarMap {
 public:
  int frame = 0;
  std::vector<VarMapItem *> items;

  ~VarMap ();

  void add (VarMapItemType type, const std::string &name);

  void addFn (
    const std::string &name,
    const std::string &hiddenName,
    VarMapItemType returnType,
    const std::vector<VarMapFnParam *> &params,
    std::size_t optionalParams
  );

  void addObj (const std::string &name, const std::string &hiddenName, const std::vector<VarMapObjField *> &fields);
  std::string genId (const std::vector<std::string> &stack, const std::string &id) const;
  const VarMapItem *get (const std::string &name) const;
  const VarMapItem *getFn (const std::string &name) const;
  const VarMapItem *getObj (const std::string &name) const;
  void restore ();
  void save ();
};

#endif
