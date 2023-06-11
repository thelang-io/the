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

#include "int.hpp"
#include "../config.hpp"

const std::vector<std::string> codegenInt = {
  R"(_{struct str} i8_str (_{int8_t} d) {)" EOL
  R"(  char buf[24];)" EOL
  R"(  _{sprintf}(buf, "%" _{PRId8}, d);)" EOL
  R"(  return _{str_alloc}(buf);)" EOL
  R"(})" EOL,

  R"(_{struct str} i16_str (_{int16_t} d) {)" EOL
  R"(  char buf[24];)" EOL
  R"(  _{sprintf}(buf, "%" _{PRId16}, d);)" EOL
  R"(  return _{str_alloc}(buf);)" EOL
  R"(})" EOL,

  R"(_{struct str} i32_str (_{int32_t} d) {)" EOL
  R"(  char buf[24];)" EOL
  R"(  _{sprintf}(buf, "%" _{PRId32}, d);)" EOL
  R"(  return _{str_alloc}(buf);)" EOL
  R"(})" EOL,

  R"(_{struct str} i64_str (_{int64_t} d) {)" EOL
  R"(  char buf[24];)" EOL
  R"(  _{sprintf}(buf, "%" _{PRId64}, d);)" EOL
  R"(  return _{str_alloc}(buf);)" EOL
  R"(})" EOL,

  R"(_{struct str} int_str (_{int32_t} d) {)" EOL
  R"(  char buf[24];)" EOL
  R"(  _{sprintf}(buf, "%" _{PRId32}, d);)" EOL
  R"(  return _{str_alloc}(buf);)" EOL
  R"(})" EOL,

  R"(_{struct str} u8_str (uint8_t d) {)" EOL
  R"(  char buf[24];)" EOL
  R"(  _{sprintf}(buf, "%" _{PRIu8}, d);)" EOL
  R"(  return _{str_alloc}(buf);)" EOL
  R"(})" EOL,

  R"(_{struct str} u16_str (_{uint16_t} d) {)" EOL
  R"(  char buf[24];)" EOL
  R"(  _{sprintf}(buf, "%" _{PRIu16}, d);)" EOL
  R"(  return _{str_alloc}(buf);)" EOL
  R"(})" EOL,

  R"(_{struct str} u32_str (_{uint32_t} d) {)" EOL
  R"(  char buf[24];)" EOL
  R"(  _{sprintf}(buf, "%" _{PRIu32}, d);)" EOL
  R"(  return _{str_alloc}(buf);)" EOL
  R"(})" EOL,

  R"(_{struct str} u64_str (_{uint64_t} d) {)" EOL
  R"(  char buf[24];)" EOL
  R"(  _{sprintf}(buf, "%" _{PRIu64}, d);)" EOL
  R"(  return _{str_alloc}(buf);)" EOL
  R"(})" EOL
};
