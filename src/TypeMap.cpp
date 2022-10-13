/*!
 * Copyright (c) 2018 Aaron Delasy
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "TypeMap.hpp"
#include <algorithm>
#include <limits>

Type *TypeMap::arrayOf (Type *elementType) {
  for (const auto &item : this->_items) {
    if (item->isArray() && std::get<TypeArray>(item->body).elementType->matchExact(elementType)) {
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(Type{"array_" + elementType->name, "@array_" + elementType->name, TypeArray{elementType}}));
  auto self = this->_items.back().get();

  auto joinTypeFn = TypeFn{this->get("str"), {
    TypeFnParam{"separator", this->get("str"), false, false, false}
  }};

  auto popTypeFn = TypeFn{this->get("void")};

  auto pushTypeFn = TypeFn{this->get("void"), {
    TypeFnParam{"element", elementType, false, false, true}
  }};

  auto reverseTypeFn = TypeFn{self};

  auto sliceTypeFn = TypeFn{self, {
    TypeFnParam{"start", this->get("i64"), false, false, false},
    TypeFnParam{"end", this->get("i64"), false, false, false}
  }};

  auto strTypeFn = TypeFn{this->get("str")};

  self->fields.push_back(TypeField{"len", this->get("int"), false, true});

  this->_items.push_back(std::make_unique<Type>(Type{self->name + ".join", "@array.join", joinTypeFn, {}, true}));
  self->fields.push_back(TypeField{"join", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{self->name + ".pop", "@array.pop", popTypeFn, {}, true}));
  self->fields.push_back(TypeField{"pop", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{self->name + ".push", "@array.push", pushTypeFn, {}, true}));
  self->fields.push_back(TypeField{"push", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{self->name + ".reverse", "@array.reverse", reverseTypeFn, {}, true}));
  self->fields.push_back(TypeField{"reverse", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{self->name + ".slice", "@array.slice", sliceTypeFn, {}, true}));
  self->fields.push_back(TypeField{"slice", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{self->name + ".str", "@array.str", strTypeFn, {}, true}));
  self->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});

  return self;
}

Type *TypeMap::fn (const std::optional<std::string> &codeName, const std::vector<TypeFnParam> &params, Type *returnType) {
  auto newType = Type{"", codeName == std::nullopt ? "@" : *codeName, TypeFn{returnType, params}};

  for (auto &item : this->_items) {
    if (!item->builtin && item->isFn() && item->codeName == newType.codeName && item->matchExact(&newType)) {
      return item.get();
    }
  }

  for (auto &item : this->_items) {
    if (!item->builtin && item->isFn() && item->matchNice(&newType)) {
      newType.name = item->name;
      break;
    }
  }

  if (newType.name.empty()) {
    newType.name = "fn$" + std::to_string(this->_fnIdx++);
  }

  this->_items.push_back(std::make_unique<Type>(newType));
  auto self = this->_items.back().get();

  auto strTypeFn = TypeFn{this->get("str")};

  this->_items.push_back(std::make_unique<Type>(Type{self->name + ".str", "@fn.str", strTypeFn, {}, true}));
  self->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});

  return self;
}

Type *TypeMap::get (const std::string &name) {
  for (const auto &item : this->_items) {
    if (item->name == name) {
      return item.get();
    }
  }

  return nullptr;
}

bool TypeMap::has (const std::string &name) {
  return std::any_of(this->_items.begin(), this->_items.end(), [&name] (const auto &it) -> bool {
    return it->name == name;
  });
}

void TypeMap::init () {
  this->stack.reserve(std::numeric_limits<short>::max());

  this->_items.push_back(std::make_unique<Type>(Type{"buffer_Buffer", "@buffer_Buffer", TypeObj{}, {}, true}));
  auto bufferBufferType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"fs_Stats", "@fs_Stats", TypeObj{}, {}, true}));
  auto fsStatsType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"any", "@any", TypeObj{}, {}, true}));
  auto anyType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"bool", "@bool", TypeObj{}, {}, true}));
  auto boolType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"byte", "@byte", TypeObj{}, {}, true}));
  auto byteType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"char", "@char", TypeObj{}, {}, true}));
  auto charType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"f32", "@f32", TypeObj{}, {}, true}));
  auto f32Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"f64", "@f64", TypeObj{}, {}, true}));
  auto f64Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"float", "@float", TypeObj{}, {}, true}));
  auto floatType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"i8", "@i8", TypeObj{}, {}, true}));
  auto i8Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"i16", "@i16", TypeObj{}, {}, true}));
  auto i16Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"i32", "@i32", TypeObj{}, {}, true}));
  auto i32Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"i64", "@i64", TypeObj{}, {}, true}));
  auto i64Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"int", "@int", TypeObj{}, {}, true}));
  auto intType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"str", "@str", TypeObj{}, {}, true}));
  auto strType = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"u8", "@u8", TypeObj{}, {}, true}));
  auto u8Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"u16", "@u16", TypeObj{}, {}, true}));
  auto u16Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"u32", "@u32", TypeObj{}, {}, true}));
  auto u32Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"u64", "@u64", TypeObj{}, {}, true}));
  auto u64Type = this->_items.back().get();
  this->_items.push_back(std::make_unique<Type>(Type{"void", "@void", TypeObj{}, {}, true}));
  auto voidType = this->_items.back().get();

  this->_items.push_back(std::make_unique<Type>(Type{"buffer_Buffer.str", "@buffer_Buffer.str", TypeFn{strType}, {}, true}));
  bufferBufferType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});

  fsStatsType->fields.push_back(TypeField{"dev", i32Type, false, false});
  fsStatsType->fields.push_back(TypeField{"mode", u16Type, false, false});
  fsStatsType->fields.push_back(TypeField{"nlink", u16Type, false, false});
  fsStatsType->fields.push_back(TypeField{"ino", u64Type, false, false});
  fsStatsType->fields.push_back(TypeField{"uid", intType, false, false});
  fsStatsType->fields.push_back(TypeField{"gid", intType, false, false});
  fsStatsType->fields.push_back(TypeField{"rdev", i32Type, false, false});
  fsStatsType->fields.push_back(TypeField{"atime", i32Type, false, false});
  fsStatsType->fields.push_back(TypeField{"atimeNs", i32Type, false, false});
  fsStatsType->fields.push_back(TypeField{"mtime", i32Type, false, false});
  fsStatsType->fields.push_back(TypeField{"mtimeNs", i32Type, false, false});
  fsStatsType->fields.push_back(TypeField{"ctime", i32Type, false, false});
  fsStatsType->fields.push_back(TypeField{"ctimeNs", i32Type, false, false});
  fsStatsType->fields.push_back(TypeField{"birthtime", i32Type, false, false});
  fsStatsType->fields.push_back(TypeField{"birthtimeNs", i32Type, false, false});
  fsStatsType->fields.push_back(TypeField{"size", i64Type, false, false});
  fsStatsType->fields.push_back(TypeField{"blocks", i64Type, false, false});
  fsStatsType->fields.push_back(TypeField{"blksize", i32Type, false, false});
  this->_items.push_back(std::make_unique<Type>(Type{"fs_Stats.str", "@fs_Stats.str", TypeFn{strType}, {}, true}));
  fsStatsType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});

  this->_items.push_back(std::make_unique<Type>(Type{"any.str", "@any.str", TypeFn{strType}, {}, true}));
  anyType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"bool.str", "@bool.str", TypeFn{strType}, {}, true}));
  boolType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"byte.str", "@byte.str", TypeFn{strType}, {}, true}));
  byteType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});

  auto repeatCharTypeFn = TypeFn{strType, {
    TypeFnParam{"times", u32Type, false, true, false}
  }};

  this->_items.push_back(std::make_unique<Type>(Type{"char.isAlpha", "@char.isAlpha", TypeFn{boolType}, {}, true}));
  charType->fields.push_back(TypeField{"isAlpha", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"char.isAlphaNum", "@char.isAlphaNum", TypeFn{boolType}, {}, true}));
  charType->fields.push_back(TypeField{"isAlphaNum", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"char.isDigit", "@char.isDigit", TypeFn{boolType}, {}, true}));
  charType->fields.push_back(TypeField{"isDigit", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"char.isSpace", "@char.isSpace", TypeFn{boolType}, {}, true}));
  charType->fields.push_back(TypeField{"isSpace", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"char.repeat", "@char.repeat", repeatCharTypeFn, {}, true}));
  charType->fields.push_back(TypeField{"repeat", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"char.str", "@char.str", TypeFn{strType}, {}, true}));
  charType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});

  this->_items.push_back(std::make_unique<Type>(Type{"f32.str", "@f32.str", TypeFn{strType}, {}, true}));
  f32Type->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"f64.str", "@f64.str", TypeFn{strType}, {}, true}));
  f64Type->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"float.str", "@float.str", TypeFn{strType}, {}, true}));
  floatType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"i8.str", "@i8.str", TypeFn{strType}, {}, true}));
  i8Type->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"i16.str", "@i16.str", TypeFn{strType}, {}, true}));
  i16Type->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"i32.str", "@i32.str", TypeFn{strType}, {}, true}));
  i32Type->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"i64.str", "@i64.str", TypeFn{strType}, {}, true}));
  i64Type->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"int.str", "@int.str", TypeFn{strType}, {}, true}));
  intType->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});

  auto findStrTypeFn = TypeFn{intType, {
    TypeFnParam{"search", strType, false, true, false}
  }};

  auto linesStrTypeFn = TypeFn{this->arrayOf(strType), {
    TypeFnParam{"keepLineBreaks", boolType, false, false, false}
  }};

  auto sliceStrTypeFn = TypeFn{strType, {
    TypeFnParam{"start", i64Type, false, false, false},
    TypeFnParam{"end", i64Type, false, false, false}
  }};

  strType->fields.push_back(TypeField{"len", intType, false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"str.find", "@str.find", findStrTypeFn, {}, true}));
  strType->fields.push_back(TypeField{"find", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"str.lines", "@str.lines", linesStrTypeFn, {}, true}));
  strType->fields.push_back(TypeField{"lines", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"str.toLower", "@str.lower", TypeFn{strType}, {}, true}));
  strType->fields.push_back(TypeField{"lower", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"str.toLowerFirst", "@str.lowerFirst", TypeFn{strType}, {}, true}));
  strType->fields.push_back(TypeField{"lowerFirst", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"str.slice", "@str.slice", sliceStrTypeFn, {}, true}));
  strType->fields.push_back(TypeField{"slice", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"str.toBuffer", "@str.toBuffer", TypeFn{bufferBufferType}, {}, true}));
  strType->fields.push_back(TypeField{"toBuffer", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"str.trim", "@str.trim", TypeFn{strType}, {}, true}));
  strType->fields.push_back(TypeField{"trim", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"str.upper", "@str.upper", TypeFn{strType}, {}, true}));
  strType->fields.push_back(TypeField{"upper", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"str.upperFirst", "@str.upperFirst", TypeFn{strType}, {}, true}));
  strType->fields.push_back(TypeField{"upperFirst", this->_items.back().get(), false, true});

  this->_items.push_back(std::make_unique<Type>(Type{"u8.str", "@u8.str", TypeFn{strType}, {}, true}));
  u8Type->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"u16.str", "@u16.str", TypeFn{strType}, {}, true}));
  u16Type->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"u32.str", "@u32.str", TypeFn{strType}, {}, true}));
  u32Type->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});
  this->_items.push_back(std::make_unique<Type>(Type{"u64.str", "@u64.str", TypeFn{strType}, {}, true}));
  u64Type->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});

  auto exitTypeFn = TypeFn{voidType, {
    TypeFnParam{"status", intType, false, false, false}
  }};

  auto fsChmodSyncTypeFn = TypeFn{voidType, {
    TypeFnParam{"path", strType, false, true, false},
    TypeFnParam{"mode", intType, false, true, false}
  }};

  auto fsChownSyncTypeFn = TypeFn{voidType, {
    TypeFnParam{"path", strType, false, true, false},
    TypeFnParam{"uid", intType, false, true, false},
    TypeFnParam{"gid", intType, false, true, false}
  }};

  auto fsExistsSyncTypeFn = TypeFn{boolType, {
    TypeFnParam{"path", strType, false, true, false}
  }};

  auto fsIsAbsoluteSyncTypeFn = TypeFn{boolType, {
    TypeFnParam{"path", strType, false, true, false}
  }};

  auto fsIsFileSyncTypeFn = TypeFn{boolType, {
    TypeFnParam{"path", strType, false, true, false}
  }};

  auto fsIsDirectorySyncTypeFn = TypeFn{boolType, {
    TypeFnParam{"path", strType, false, true, false}
  }};

  auto fsIsSymbolicLinkSyncTypeFn = TypeFn{boolType, {
    TypeFnParam{"path", strType, false, true, false}
  }};

  auto fsMkdirSyncTypeFn = TypeFn{voidType, {
    TypeFnParam{"path", strType, false, true, false},
    TypeFnParam{"recursive", boolType, false, false, false}
  }};

  auto fsReadFileSyncTypeFn = TypeFn{bufferBufferType, {
    TypeFnParam{"path", strType, false, true, false}
  }};

  auto fsRealpathSyncTypeFn = TypeFn{strType, {
    TypeFnParam{"path", strType, false, true, false}
  }};

  auto fsRmSyncTypeFn = TypeFn{voidType, {
    TypeFnParam{"path", strType, false, true, false}
  }};

  auto fsRmdirSyncTypeFn = TypeFn{voidType, {
    TypeFnParam{"path", strType, false, true, false}
  }};

  auto fsScandirSyncTypeFn = TypeFn{this->arrayOf(strType), {
    TypeFnParam{"path", strType, false, true, false}
  }};

  auto fsStatSyncTypeFn = TypeFn{fsStatsType, {
    TypeFnParam{"path", strType, false, true, false}
  }};

  auto fsWriteFileSyncTypeFn = TypeFn{voidType, {
    TypeFnParam{"path", strType, false, true, false},
    TypeFnParam{"data", bufferBufferType, false, true, false}
  }};

  auto pathBasenameTypeFn = TypeFn{strType, {
    TypeFnParam{"path", strType, false, true, false}
  }};

  auto pathDirnameTypeFn = TypeFn{strType, {
    TypeFnParam{"path", strType, false, true, false}
  }};

  auto printTypeFn = TypeFn{voidType, {
    TypeFnParam{"items", anyType, false, false, true},
    TypeFnParam{"separator", strType, false, false, false},
    TypeFnParam{"terminator", strType, false, false, false},
    TypeFnParam{"to", strType, false, false, false}
  }};

  auto processRunTypeFn = TypeFn{bufferBufferType, {
    TypeFnParam{"command", strType, false, true, false}
  }};

  auto sleepSyncTypeFn = TypeFn{voidType, {
    TypeFnParam{"milliseconds", u32Type, false, true, false}
  }};

  this->_items.push_back(std::make_unique<Type>(Type{"exit", "@exit", exitTypeFn, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"fs_chmodSync", "@fs_chmodSync", fsChmodSyncTypeFn, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"fs_chownSync", "@fs_chownSync", fsChownSyncTypeFn, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"fs_existsSync", "@fs_existsSync", fsExistsSyncTypeFn, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"fs_isAbsoluteSync", "@fs_isAbsoluteSync", fsIsAbsoluteSyncTypeFn, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"fs_isFileSync", "@fs_isFileSync", fsIsFileSyncTypeFn, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"fs_isDirectorySync", "@fs_isDirectorySync", fsIsDirectorySyncTypeFn, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"fs_isSymbolicLinkSync", "@fs_isSymbolicLinkSync", fsIsSymbolicLinkSyncTypeFn, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"fs_mkdirSync", "@fs_mkdirSync", fsMkdirSyncTypeFn, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"fs_readFileSync", "@fs_readFileSync", fsReadFileSyncTypeFn, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"fs_realpathSync", "@fs_realpathSync", fsRealpathSyncTypeFn, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"fs_rmSync", "@fs_rmSync", fsRmSyncTypeFn, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"fs_rmdirSync", "@fs_rmdirSync", fsRmdirSyncTypeFn, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"fs_scandirSync", "@fs_scandirSync", fsScandirSyncTypeFn, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"fs_statSync", "@fs_statSync", fsStatSyncTypeFn, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"fs_writeFileSync", "@fs_writeFileSync", fsWriteFileSyncTypeFn, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"os_name", "@os_name", TypeFn{strType}, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"print", "@print", printTypeFn, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"path_basename", "@path_basename", pathBasenameTypeFn, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"path_dirname", "@path_dirname", pathDirnameTypeFn, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"process_cwd", "@process_cwd", TypeFn{strType}, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"process_getgid", "@process_getgid", TypeFn{intType}, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"process_getuid", "@process_getuid", TypeFn{intType}, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"process_runSync", "@process_runSync", processRunTypeFn, {}, true}));
  this->_items.push_back(std::make_unique<Type>(Type{"sleepSync", "@sleepSync", sleepSyncTypeFn, {}, true}));
}

std::string TypeMap::name (const std::string &name) const {
  auto fullName = std::string();

  for (const auto &item : this->stack) {
    fullName += item + "SD";
  }

  fullName += name + "_";

  for (auto idx = static_cast<std::size_t>(0);; idx++) {
    auto fullNameTest = fullName + std::to_string(idx);
    auto exists = false;

    for (const auto &item : this->_items) {
      if (item->codeName == fullNameTest) {
        exists = true;
        break;
      }
    }

    if (!exists) {
      return fullNameTest;
    }
  }
}

Type *TypeMap::obj (const std::string &name, const std::string &codeName, const std::vector<TypeField> &fields) {
  this->_items.push_back(std::make_unique<Type>(Type{name, codeName, TypeObj{}, fields}));
  auto self = this->_items.back().get();

  auto strTypeFn = TypeFn{this->get("str")};

  this->_items.push_back(std::make_unique<Type>(Type{self->name + ".str", "@obj.str", strTypeFn, {}, true}));
  self->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});

  return self;
}

Type *TypeMap::opt (Type *type) {
  for (const auto &item : this->_items) {
    if (item->isOpt() && std::get<TypeOptional>(item->body).type->matchExact(type)) {
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(Type{"opt_" + type->name, "@opt_" + type->name, TypeOptional{type}}));
  auto self = this->_items.back().get();

  auto strTypeFn = TypeFn{this->get("str")};

  this->_items.push_back(std::make_unique<Type>(Type{self->name + ".str", "@opt.str", strTypeFn, {}, true}));
  self->fields.push_back(TypeField{"str", this->_items.back().get(), false, true});

  return self;
}

Type *TypeMap::ref (Type *refType) {
  for (const auto &item : this->_items) {
    if (item->isRef() && std::get<TypeRef>(item->body).refType->matchExact(refType)) {
      return item.get();
    }
  }

  this->_items.push_back(std::make_unique<Type>(Type{"ref_" + refType->name, "@ref_" + refType->name, TypeRef{refType}}));
  return this->_items.back().get();
}
