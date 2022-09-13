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

#ifndef SRC_PARSER_TYPE_HPP
#define SRC_PARSER_TYPE_HPP

#include <memory>
#include <optional>
#include <variant>
#include <vector>
#include "Token.hpp"

struct ParserTypeArray;
struct ParserTypeFn;
struct ParserTypeId;
struct ParserTypeOptional;
struct ParserTypeRef;

using ParserTypeBody = std::variant<ParserTypeArray, ParserTypeFn, ParserTypeId, ParserTypeOptional, ParserTypeRef>;

struct ParserType {
  std::shared_ptr<ParserTypeBody> body;
  bool parenthesized;
  ReaderLocation start;
  ReaderLocation end;

  std::string xml (std::size_t = 0) const;
};

struct ParserTypeArray {
  ParserType elementType;
};

struct ParserTypeFnParam {
  std::optional<Token> id;
  ParserType type;
  bool mut;
  bool variadic;
};

struct ParserTypeFn {
  std::vector<ParserTypeFnParam> params;
  ParserType returnType;
};

struct ParserTypeId {
  Token id;
};

struct ParserTypeOptional {
  ParserType type;
};

struct ParserTypeRef {
  ParserType refType;
};

#endif
