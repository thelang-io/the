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

#include "char.hpp"
#include "../config.hpp"

const std::vector<std::string> codegenChar = {
  R"(unsigned char char_byte (char c) {)" EOL
  R"(  return (unsigned char) c;)" EOL
  R"(})" EOL,

  R"(_{bool} char_isDigit (char c) {)" EOL
  R"(  return _{isdigit}(c);)" EOL
  R"(})" EOL,

  R"(_{bool} char_isLetter (char c) {)" EOL
  R"(  return _{isalpha}(c);)" EOL
  R"(})" EOL,

  R"(_{bool} char_isLetterOrDigit (char c) {)" EOL
  R"(  return _{isalnum}(c);)" EOL
  R"(})" EOL,

  R"(_{bool} char_isLower (char c) {)" EOL
  R"(  return _{islower}(c);)" EOL
  R"(})" EOL,

  R"(_{bool} char_isUpper (char c) {)" EOL
  R"(  return _{isupper}(c);)" EOL
  R"(})" EOL,

  R"(_{bool} char_isWhitespace (char c) {)" EOL
  R"(  return _{isspace}(c);)" EOL
  R"(})" EOL,

  R"(char char_lower (char c) {)" EOL
  R"(  return _{tolower}(c);)" EOL
  R"(})" EOL,

  R"(_{struct str} char_repeat (char c, _{int32_t} k) {)" EOL
  R"(  if (k <= 0) return _{str_alloc}("");)" EOL
  R"(  _{size_t} l = (_{size_t}) k;)" EOL
  R"(  char *d = _{alloc}(l);)" EOL
  R"(  _{size_t} i = 0;)" EOL
  R"(  while (i < l) d[i++] = c;)" EOL
  R"(  return (_{struct str}) {d, l};)" EOL
  R"(})" EOL,

  R"(_{struct str} char_str (char c) {)" EOL
  R"(  char buf[2] = {c, '\0'};)" EOL
  R"(  return _{str_alloc}(buf);)" EOL
  R"(})" EOL,

  R"(char char_upper (char c) {)" EOL
  R"(  return _{toupper}(c);)" EOL
  R"(})" EOL
};
