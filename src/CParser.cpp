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

#include "CParser.hpp"
#include <algorithm>
#include <cctype>
#include "Error.hpp"

std::string extractDecl (const std::string &s) {
  auto result = std::string();

  for (auto i = static_cast<std::size_t>(0); i < s.size(); i++) {
    auto ch = s[i];

    if (ch == '\n' || (ch == '\r' && i + 1 < s.size() && s[i + 1] == '\n')) {
      break;
    } else {
      result += ch;
    }
  }

  result.erase(result.size() - 2, 2);
  return result;
}

std::vector<CParserToken> tokenize (const std::string &s) {
  auto result = std::vector<CParserToken>{};

  for (auto i = static_cast<std::size_t>(0); i < s.size(); i++) {
    if (std::isspace(s[i])) {
      auto val = std::string(1, s[i]);

      while (i + 1 < s.size() && std::isspace(s[i + 1])) {
        val += s[++i];
      }

      result.push_back(CParserToken{CPARSER_WHITESPACE, val});
    } else if (s[i] == '_' && i + 1 < s.size() && s[i + 1] == '{') {
      auto val = std::string(1, s[i]);

      while (i + 1 < s.size()) {
        val += s[++i];

        if (s[i] == '}') {
          break;
        }
      }

      result.push_back(CParserToken{CPARSER_REPLACEMENT, val});
    } else if (std::isalpha(s[i]) || s[i] == '_') {
      auto val = std::string(1, s[i]);

      while (i + 1 < s.size() && (std::isalnum(s[i + 1]) || s[i + 1] == '_' || s[i + 1] == '$')) {
        val += s[++i];
      }

      result.push_back(CParserToken{CPARSER_ID, val});
    } else if (s[i] == '.' && i + 2 < s.size() && s[i + 1] == '.' && s[i + 2] == '.') {
      result.push_back(CParserToken{CPARSER_OP_DOT_DOT_DOT, std::string("...")});
      i += 2;
    } else if (s[i] == '(') {
      result.push_back(CParserToken{CPARSER_OP_LPAR, std::string(1, s[i])});
    } else if (s[i] == ')') {
      result.push_back(CParserToken{CPARSER_OP_RPAR, std::string(1, s[i])});
    } else if (s[i] == '*') {
      result.push_back(CParserToken{CPARSER_OP_STAR, std::string(1, s[i])});
    } else if (s[i] == ',') {
      result.push_back(CParserToken{CPARSER_OP_COMMA, std::string(1, s[i])});
    } else {
      throw Error("unexpected token `" + std::string(1, s[i]) + "` in C parser");
    }
  }

  return result;
}

void tkWhitespace (const std::vector<CParserToken> &tokens, std::size_t *i) {
  for (; *i + 1 < tokens.size() && tokens[*i].type == CPARSER_WHITESPACE; (*i)++);
}

bool tkLookahead (const std::vector<CParserToken> &tokens, std::size_t *i, CParserTokenType type) {
  auto j = *i;
  for (; j + 1 < tokens.size() && tokens[j + 1].type == CPARSER_WHITESPACE; j++);
  auto r = j + 1 < tokens.size() && tokens[j + 1].type == type;
  if (r) *i = j;
  return r;
}

std::size_t tkBehind (const std::vector<CParserToken> &tokens, std::size_t i) {
  for (; i != 0 && tokens[i].type == CPARSER_WHITESPACE; i--);
  return i;
}

std::string CParser::decl () const {
  auto paramsCode = std::string();

  for (auto i = static_cast<std::size_t>(0); i < this->params.size(); i++) {
    auto param = this->params[i];
    auto paramType = param.type;

    paramType.erase(std::find_if(paramType.rbegin(), paramType.rend(), [] (auto it) {
      return !std::isspace(it);
    }).base(), paramType.end());

    paramsCode += i == 0 ? "" : ", ";
    paramsCode += paramType;
  }

  return this->returnType + this->name + " (" + paramsCode + ")";
}

CParser cParse (const std::string &s) {
  auto decl = extractDecl(s);
  auto tokens = tokenize(decl);
  auto cParser = CParser{};
  auto i = static_cast<std::size_t>(0);

  for (; i < tokens.size(); i++) {
    if (tkLookahead(tokens, &i, CPARSER_OP_LPAR)) {
      cParser.name = tokens[tkBehind(tokens, i)].val;
      break;
    }

    cParser.returnType += tokens[i].val;
  }

  if (cParser.name.empty()) {
    throw Error("position of name not found in `" + s + "`");
  }

  auto blocks = static_cast<std::size_t>(0);
  auto paramType = std::string();
  i += 2;

  if (tokens[i].type != CPARSER_OP_RPAR) {
    while (i < tokens.size()) {
      if (tokens[i].type == CPARSER_OP_RPAR && blocks == 0) {
        if (tokens[i - 1].type == CPARSER_ID && tokens[i - 1].val != paramType) {
          cParser.params.push_back(CParserParam{tokens[i - 1].val, paramType.substr(0, paramType.size() - tokens[i - 1].val.size())});
        } else {
          cParser.params.push_back(CParserParam{"", paramType});
        }

        break;
      } else if (tokens[i].type == CPARSER_OP_COMMA && blocks == 0) {
        if (tokens[i - 1].type == CPARSER_ID && tokens[i - 1].val != paramType) {
          cParser.params.push_back(CParserParam{tokens[i - 1].val, paramType.substr(0, paramType.size() - tokens[i - 1].val.size())});
        } else {
          cParser.params.push_back(CParserParam{"", paramType});
        }

        paramType = "";
        i += 1;
        tkWhitespace(tokens, &i);
        continue;
      } else if (tokens[i].type == CPARSER_OP_LPAR) {
        blocks++;
      } else if (tokens[i].type == CPARSER_OP_RPAR) {
        blocks--;
      }

      paramType += tokens[i].val;
      i++;
    }
  }

  return cParser;
}
