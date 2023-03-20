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

#include "float.hpp"
#include "../config.hpp"

const std::vector<std::string> codegenFloat = {
  "_{struct str} f32_str (float f) {" EOL
  "  char buf[512];" EOL
  R"(  _{sprintf}(buf, "%f", f);)" EOL
  "  return _{str_alloc}(buf);" EOL
  "}" EOL,

  "_{struct str} f64_str (double f) {" EOL
  "  char buf[512];" EOL
  R"(  _{sprintf}(buf, "%f", f);)" EOL
  "  return _{str_alloc}(buf);" EOL
  "}" EOL,

  "_{struct str} float_str (double f) {" EOL
  "  char buf[512];" EOL
  R"(  _{sprintf}(buf, "%f", f);)" EOL
  "  return _{str_alloc}(buf);" EOL
  "}" EOL
};
