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

#ifndef SRC_CPARSER_HPP
#define SRC_CPARSER_HPP

#include <string>
#include <vector>

enum CParserTokenType {
  CPARSER_ID,
  CPARSER_REPLACEMENT,
  CPARSER_WHITESPACE,
  CPARSER_OP_AMP,
  CPARSER_OP_STAR,
  CPARSER_OP_LBRACE,
  CPARSER_OP_LPAR,
  CPARSER_OP_RBRACE,
  CPARSER_OP_RPAR,
  CPARSER_OP_COMMA,
  CPARSER_OP_DOT_DOT_DOT,
};

struct CParserParam {
  std::string name;
  std::string type;
};

struct CParser {
  std::string name;
  std::vector<CParserParam> params;
  std::string returnType;

  std::string decl () const;
};

struct CParserToken {
  CParserTokenType type;
  std::string val;
};

CParser cParse (const std::string &);

#endif
