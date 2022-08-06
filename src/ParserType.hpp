/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
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
struct ParserTypeRef;

using ParserTypeBody = std::variant<ParserTypeArray, ParserTypeFn, ParserTypeId, ParserTypeRef>;

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

struct ParserTypeRef {
  ParserType refType;
};

#endif
